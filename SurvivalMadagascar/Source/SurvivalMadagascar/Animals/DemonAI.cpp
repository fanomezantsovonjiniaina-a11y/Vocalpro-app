#include "DemonAI.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Characters/SurvivalCharacter.h"

ADemonAI::ADemonAI()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
    switch(DemonType)
    {
    case EDemonType::Kalanoro: Visibility = 0.4f; HP = 150.f; AttackDamage = 35.f; break;
    case EDemonType::Kinoly:   Visibility = 0.2f; HP = 80.f;  AttackDamage = 20.f; break;
    case EDemonType::Angatra:  Visibility = 0.05f; HP = 50.f;  AttackDamage = 50.f; break;
    }
}

void ADemonAI::BeginPlay() { Super::BeginPlay(); CurrentState = EDemonState::Dormant; }

void ADemonAI::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ADemonAI, CurrentState);
    DOREPLIFETIME(ADemonAI, Visibility);
}

void ADemonAI::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if(!HasAuthority()) return;
    AttackCooldown = FMath::Max(0.f, AttackCooldown - DeltaTime);
    PhaseTimer += DeltaTime;
    UpdateVisibility(DeltaTime);
    UpdateAI(DeltaTime);
}

void ADemonAI::UpdateVisibility(float DeltaTime)
{
    float BaseVis;
    switch(DemonType)
    {
    case EDemonType::Kalanoro: BaseVis = 0.4f; break;
    case EDemonType::Kinoly:   BaseVis = 0.2f; break;
    case EDemonType::Angatra:  BaseVis = 0.05f; break;
    default: BaseVis = 0.3f;
    }
    // Pulse visibility
    Visibility = BaseVis + FMath::Sin(PhaseTimer * 2.f) * 0.1f;

    // More visible when attacking
    if(CurrentState == EDemonState::Attacking) Visibility = FMath::Min(1.f, Visibility + 0.3f);

    // Near fire = more visible (weakened)
    if(IsNearFire()) Visibility = FMath::Min(1.f, Visibility + 0.4f);
}

void ADemonAI::UpdateAI(float DeltaTime)
{
    if(CurrentState == EDemonState::Dormant || CurrentState == EDemonState::Banished) return;

    // Demons flee from fire
    if(IsNearFire()) { CurrentState = EDemonState::Lurking; return; }

    switch(CurrentState)
    {
    case EDemonState::Lurking:
    {
        Target = FindTarget();
        if(!Target) break;
        float D = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
        if(D < 2000.f) // 20m — start haunting
        {
            CurrentState = EDemonState::Haunting;
            HauntTimer = 5.f;
        }
        // Slow patrol
        FVector Dir = FMath::VRand(); Dir.Z = 0;
        GetCharacterMovement()->MaxWalkSpeed = 150.f;
        AddMovementInput(Dir * 0.2f);
        break;
    }
    case EDemonState::Haunting:
    {
        // Kalanoro: whispers, visions — player screen distortion (Blueprint)
        // Kinoly: shows death visions — player fatigue increases
        // Angatra: player hears footsteps, heartbeat spikes
        HauntTimer -= DeltaTime;
        if(Target)
        {
            if(DemonType == EDemonType::Kinoly)
            {
                if(ASurvivalCharacter* PC = Cast<ASurvivalCharacter>(Target))
                    PC->GetSurvivalComponent()->Stats.Fatigue = FMath::Min(100.f,
                        PC->GetSurvivalComponent()->Stats.Fatigue + 5.f * DeltaTime);
            }
            float D = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
            if(D < 500.f) // 5m — attack!
            {
                CurrentState = EDemonState::Attacking;
                OnPlayerHaunted.Broadcast();
            }
        }
        if(HauntTimer <= 0.f) CurrentState = EDemonState::Lurking;
        break;
    }
    case EDemonState::Attacking:
    {
        if(!Target) { CurrentState = EDemonState::Lurking; break; }
        GetCharacterMovement()->MaxWalkSpeed = DemonType == EDemonType::Kalanoro ? 500.f : 200.f;
        FVector Dir = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();

        // Angatra: teleports! (blinks toward player)
        if(DemonType == EDemonType::Angatra && FMath::RandRange(0,100) < 2)
        {
            FVector TeleportPos = Target->GetActorLocation() + FVector(FMath::RandRange(-200,200), FMath::RandRange(-200,200), 0);
            SetActorLocation(TeleportPos);
        }
        else AddMovementInput(Dir);

        float D = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
        if(D < 150.f && AttackCooldown <= 0.f)
        {
            if(ASurvivalCharacter* PC = Cast<ASurvivalCharacter>(Target))
            {
                PC->GetSurvivalComponent()->Server_ApplyDamage(AttackDamage);
                // Kinoly: apply Infection on hit
                if(DemonType == EDemonType::Kinoly)
                    PC->GetSurvivalComponent()->Server_ApplyStatusEffect(EStatusEffect::Infection);
            }
            AttackCooldown = 3.f;
            CurrentState = EDemonState::Haunting;
            HauntTimer = 3.f;
        }
        break;
    }
    default: break;
    }
}

AActor* ADemonAI::FindTarget() const
{
    TArray<AActor*> Players;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASurvivalCharacter::StaticClass(), Players);
    AActor* Best = nullptr; float BestDist = 9999999.f;
    for(AActor* P : Players)
    {
        float D = FVector::Dist(GetActorLocation(), P->GetActorLocation());
        if(D < BestDist) { BestDist = D; Best = P; }
    }
    return Best;
}

bool ADemonAI::IsNearFire() const
{
    TArray<AActor*> Fires;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Campfire"), Fires);
    for(AActor* F : Fires)
        if(FVector::Dist(GetActorLocation(), F->GetActorLocation()) < 800.f) return true;
    return false;
}

void ADemonAI::OnNightBegins()
{
    CurrentState = EDemonState::Lurking;
    GetCharacterMovement()->MaxWalkSpeed = 150.f;
}

void ADemonAI::OnDayBegins()
{
    CurrentState = EDemonState::Dormant;
    // Demons vanish at dawn
    SetActorHiddenInGame(true);
    SetLifeSpan(1.f);
}

void ADemonAI::TakeDamage_Demon(float Amount, bool bHolyDamage)
{
    float ActualDmg = bHolyDamage ? Amount * 3.f : Amount; // Holy fire = 3x
    HP = FMath::Max(0.f, HP - ActualDmg);
    if(HP <= 0.f)
    {
        CurrentState = EDemonState::Banished;
        SetLifeSpan(2.f);
    }
    else CurrentState = EDemonState::Lurking; // Retreat when hit
}
