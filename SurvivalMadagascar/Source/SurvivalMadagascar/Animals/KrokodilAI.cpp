#include "KrokodilAI.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Characters/SurvivalCharacter.h"

AKrokodilAI::AKrokodilAI()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
}

void AKrokodilAI::BeginPlay()
{
    Super::BeginPlay();
    WaterAnchor = GetActorLocation();
    CurrentState = EKrokodilState::Idle;
}

void AKrokodilAI::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (!HasAuthority()) return;

    AttackCooldown = FMath::Max(0.f, AttackCooldown - DeltaTime);
    UpdateAI(DeltaTime);
}

void AKrokodilAI::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AKrokodilAI, CurrentState);
    DOREPLIFETIME(AKrokodilAI, bIsInWater);
}

void AKrokodilAI::UpdateAI(float DeltaTime)
{
    // Krokodily tsy mivoaka lavitra amin'ny rano
    float DistFromWater = FVector::Dist(GetActorLocation(), WaterAnchor);
    if (DistFromWater > MaxLandDistance * 100.f && CurrentState != EKrokodilState::Attacking)
    {
        // Miverina amin'ny rano
        FVector ReturnDir = (WaterAnchor - GetActorLocation()).GetSafeNormal();
        AddMovementInput(ReturnDir);
        CurrentState = EKrokodilState::Retreating;
        return;
    }

    switch (CurrentState)
    {
    case EKrokodilState::Idle:
    {
        // Mangingina — mitady player ao anaty rano
        AActor* PlayerInWater = GetNearestPlayerInRange(WaterDetectionRadius * 100.f);
        if (PlayerInWater && IsPlayerInWater(PlayerInWater))
        {
            TargetPlayer = PlayerInWater;
            CurrentState = EKrokodilState::Lurking;
            LurkTimer = FMath::RandRange(0.5f, 2.f); // Miandry kely — suspense!
        }
        // Check player on land nearby (5m)
        AActor* PlayerOnLand = GetNearestPlayerInRange(LandDetectionRadius * 100.f);
        if (PlayerOnLand)
        {
            TargetPlayer = PlayerOnLand;
            LaunchAttack(); // Mivaingana avy hatrany!
        }
        break;
    }

    case EKrokodilState::Lurking:
    {
        // Miandry — miankina akaiky mangingina
        LurkTimer -= DeltaTime;
        if (LurkTimer <= 0.f)
        {
            LaunchAttack();
        }
        // Manakaiky mangingina
        if (TargetPlayer)
        {
            FVector Dir = (TargetPlayer->GetActorLocation() - GetActorLocation()).GetSafeNormal();
            AddMovementInput(Dir * 0.3f); // Moramora — tsy hita!
        }
        break;
    }

    case EKrokodilState::Attacking:
    {
        if (!TargetPlayer) { CurrentState = EKrokodilState::Idle; break; }

        GetCharacterMovement()->MaxWalkSpeed = WaterSpeed;
        FVector Dir = (TargetPlayer->GetActorLocation() - GetActorLocation()).GetSafeNormal();
        AddMovementInput(Dir);

        // Hamely raha akaiky
        float Dist = FVector::Dist(GetActorLocation(), TargetPlayer->GetActorLocation());
        if (Dist < 200.f && AttackCooldown <= 0.f)
        {
            if (ASurvivalCharacter* Player = Cast<ASurvivalCharacter>(TargetPlayer))
            {
                Player->GetSurvivalComponent()->Server_ApplyDamage(AttackDamage);
                AttackCooldown = 3.f;
            }
        }

        // Miverina amin'ny rano raha very ny target
        if (Dist > WaterDetectionRadius * 100.f)
        {
            TargetPlayer = nullptr;
            CurrentState = EKrokodilState::Retreating;
        }
        break;
    }

    case EKrokodilState::Retreating:
    {
        float Dist = FVector::Dist(GetActorLocation(), WaterAnchor);
        if (Dist < 300.f)
        {
            CurrentState = EKrokodilState::Idle;
            GetCharacterMovement()->MaxWalkSpeed = LandSpeed;
        }
        break;
    }

    default: break;
    }
}

AActor* AKrokodilAI::GetNearestPlayerInRange(float Radius) const
{
    TArray<AActor*> Players;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASurvivalCharacter::StaticClass(), Players);

    AActor* Nearest = nullptr;
    float NearestDist = Radius;

    for (AActor* P : Players)
    {
        float Dist = FVector::Dist(GetActorLocation(), P->GetActorLocation());
        if (Dist < NearestDist)
        {
            NearestDist = Dist;
            Nearest = P;
        }
    }
    return Nearest;
}

bool AKrokodilAI::IsPlayerInWater(AActor* Player) const
{
    if (!Player) return false;
    // Check via water volume overlap — tagged "WaterVolume"
    TArray<AActor*> WaterVolumes;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("WaterVolume"), WaterVolumes);
    for (AActor* W : WaterVolumes)
    {
        FBox Box = W->GetComponentsBoundingBox();
        if (Box.IsInsideOrOn(Player->GetActorLocation()))
            return true;
    }
    return false;
}

void AKrokodilAI::LaunchAttack()
{
    CurrentState = EKrokodilState::Attacking;
    GetCharacterMovement()->MaxWalkSpeed = WaterSpeed;
    // Play splash sound in Blueprint
}

bool AKrokodilAI::IsVulnerable(FVector HitLocation) const
{
    // Vulnerable: yeux (tsy head hitbox) sy ventre (tsy ambany)
    FVector ToHit = (HitLocation - GetActorLocation()).GetSafeNormal();
    float DotUp = FVector::DotProduct(ToHit, GetActorUpVector());
    return DotUp < -0.3f; // Atiny = ambany = vulnerable
}

void AKrokodilAI::TakeDamageFromPlayer(float Amount, FVector HitLocation)
{
    float ActualDamage = IsVulnerable(HitLocation) ? Amount * 3.f : Amount * 0.3f;
    HP = FMath::Max(0.f, HP - ActualDamage);

    if (HP <= 0.f)
    {
        CurrentState = EKrokodilState::Idle;
        SetLifeSpan(3.f);
    }
    else
    {
        // Angry — mihaza haingana
        CurrentState = EKrokodilState::Attacking;
    }
}
