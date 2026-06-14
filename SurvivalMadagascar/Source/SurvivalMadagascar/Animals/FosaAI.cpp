#include "FosaAI.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"
#include "../Characters/SurvivalCharacter.h"

AFosaAI::AFosaAI()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
}

void AFosaAI::BeginPlay()
{
    Super::BeginPlay();
    CurrentState = EFosaState::Sleeping;
}

void AFosaAI::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!HasAuthority()) return;

    AttackCooldown = FMath::Max(0.f, AttackCooldown - DeltaTime);
    RoarCooldown = FMath::Max(0.f, RoarCooldown - DeltaTime);

    if (CurrentState != EFosaState::Sleeping)
        UpdateAI(DeltaTime);
}

void AFosaAI::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AFosaAI, CurrentState);
    DOREPLIFETIME(AFosaAI, TargetPlayer);
}

void AFosaAI::OnNightBegins()
{
    CurrentState = EFosaState::Patrolling;
    GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
}

void AFosaAI::OnDayBegins()
{
    CurrentState = EFosaState::Sleeping;
    TargetPlayer = nullptr;
    GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
}

void AFosaAI::UpdateAI(float DeltaTime)
{
    // Check retreat conditions first
    if (AreAllPlayersGrouped() || (TargetPlayer && IsPlayerNearFire(TargetPlayer)))
    {
        CurrentState = EFosaState::Retreating;
        TargetPlayer = nullptr;
    }

    switch (CurrentState)
    {
    case EFosaState::Patrolling:
        FindTarget();
        Patrol(DeltaTime);
        break;

    case EFosaState::Stalking:
        // Circle around the player, looking for isolation
        if (!TargetPlayer) { CurrentState = EFosaState::Patrolling; break; }
        if (RoarCooldown <= 0.f) { Roar(); RoarCooldown = 15.f; }
        CurrentState = EFosaState::Hunting; // Simplified: go straight to hunt
        break;

    case EFosaState::Hunting:
        if (!TargetPlayer) { CurrentState = EFosaState::Patrolling; break; }
        GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
        ChaseTarget();
        AttackTarget();
        break;

    case EFosaState::Retreating:
        // Move away from players
        GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
        FindTarget(); // Keep searching from a distance
        if (!AreAllPlayersGrouped())
            CurrentState = EFosaState::Patrolling;
        break;

    default:
        break;
    }
}

void AFosaAI::FindTarget()
{
    TArray<AActor*> Players;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASurvivalCharacter::StaticClass(), Players);

    AActor* BestTarget = nullptr;
    float BestScore = 0.f;

    for (AActor* Player : Players)
    {
        float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
        if (Distance > DetectionRadius * 100.f) continue; // Convert to cm
        if (IsPlayerNearFire(Player)) continue;

        // Isolated players get higher score
        float IsolationBonus = 1.f;
        for (AActor* Other : Players)
        {
            if (Other == Player) continue;
            if (FVector::Dist(Player->GetActorLocation(), Other->GetActorLocation()) < 1500.f)
                IsolationBonus = 0.3f; // Less attractive if grouped
        }

        float Score = (1.f / FMath::Max(Distance, 1.f)) * IsolationBonus;
        if (Score > BestScore)
        {
            BestScore = Score;
            BestTarget = Player;
        }
    }

    if (BestTarget && BestTarget != TargetPlayer)
    {
        TargetPlayer = BestTarget;
        CurrentState = EFosaState::Stalking;
    }
}

bool AFosaAI::IsPlayerNearFire(AActor* Player) const
{
    if (!Player) return false;

    // Find all fire actors in the world (tagged as "Campfire")
    TArray<AActor*> Fires;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Campfire"), Fires);

    for (AActor* Fire : Fires)
    {
        float DistToFire = FVector::Dist(Player->GetActorLocation(), Fire->GetActorLocation());
        if (DistToFire < FireFearRadius * 100.f)
            return true;
    }
    return false;
}

bool AFosaAI::AreAllPlayersGrouped() const
{
    TArray<AActor*> Players;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASurvivalCharacter::StaticClass(), Players);

    if (Players.Num() < 2) return false;

    // Check if all players are within 15m of each other
    for (int i = 0; i < Players.Num(); i++)
    {
        for (int j = i + 1; j < Players.Num(); j++)
        {
            if (FVector::Dist(Players[i]->GetActorLocation(), Players[j]->GetActorLocation()) > 1500.f)
                return false;
        }
    }

    // Also check if they have fire nearby
    return Players.Num() > 0 && IsPlayerNearFire(Players[0]);
}

void AFosaAI::ChaseTarget()
{
    if (!TargetPlayer) return;

    FVector Direction = (TargetPlayer->GetActorLocation() - GetActorLocation()).GetSafeNormal();
    AddMovementInput(Direction);
}

void AFosaAI::AttackTarget()
{
    if (!TargetPlayer || AttackCooldown > 0.f) return;

    float Distance = FVector::Dist(GetActorLocation(), TargetPlayer->GetActorLocation());
    if (Distance > AttackRadius * 100.f) return;

    // Deal damage
    if (ASurvivalCharacter* Player = Cast<ASurvivalCharacter>(TargetPlayer))
    {
        Player->GetSurvivalComponent()->Server_ApplyDamage(AttackDamage);
        AttackCooldown = 2.f;
    }
}

void AFosaAI::Patrol(float DeltaTime)
{
    // Random patrol using NavMesh
    float DistToDestination = FVector::Dist(GetActorLocation(), PatrolDestination);
    if (DistToDestination < 200.f)
    {
        // Pick new random destination
        FNavLocation NavLoc;
        UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
        if (NavSys && NavSys->GetRandomReachablePointInRadius(GetActorLocation(), 5000.f, NavLoc))
            PatrolDestination = NavLoc.Location;
    }

    FVector Direction = (PatrolDestination - GetActorLocation()).GetSafeNormal();
    AddMovementInput(Direction);
}

void AFosaAI::Roar()
{
    // Play roar sound — referenced in Blueprint
    // Players nearby will hear this as a warning
    UGameplayStatics::PlaySoundAtLocation(GetWorld(), nullptr, GetActorLocation()); // Sound set in BP
}

void AFosaAI::TakeDamageFromPlayer(float Amount)
{
    HP = FMath::Max(0.f, HP - Amount);
    if (HP <= 0.f)
    {
        CurrentState = EFosaState::Sleeping;
        // Destroy after delay (loot drop handled in BP)
        SetLifeSpan(3.f);
    }
}
