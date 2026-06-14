#include "LamboGidroAI.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Characters/SurvivalCharacter.h"

ALamboGidroAI::ALamboGidroAI() { PrimaryActorTick.bCanEverTick = true; bReplicates = true; }

void ALamboGidroAI::BeginPlay() { Super::BeginPlay(); GrazeTimer = FMath::RandRange(3.f,8.f); }

void ALamboGidroAI::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ALamboGidroAI, CurrentState);
}

void ALamboGidroAI::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if(!HasAuthority()) return;
    UpdateAI(DeltaTime);
}

void ALamboGidroAI::UpdateAI(float DeltaTime)
{
    TArray<AActor*> Players;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASurvivalCharacter::StaticClass(), Players);

    switch(CurrentState)
    {
    case ELamboState::Grazing:
    {
        GrazeTimer -= DeltaTime;
        // Random wander
        if(GrazeTimer <= 0.f)
        {
            FVector Dir = FMath::VRand(); Dir.Z = 0; Dir.Normalize();
            AddMovementInput(Dir * 0.3f);
            GrazeTimer = FMath::RandRange(2.f, 6.f);
        }
        // Check if player too close
        for(AActor* P : Players)
        {
            float D = FVector::Dist(GetActorLocation(), P->GetActorLocation());
            if(D < AlertRadius)
            {
                // Lambo has young nearby? Charge! Otherwise flee
                bool bHasYoung = FMath::RandBool();
                CurrentState = bHasYoung ? ELamboState::Charging : ELamboState::Fleeing;
                ChargeTarget = P;
                ChargeTimer = 3.f;
            }
        }
        break;
    }
    case ELamboState::Charging:
    {
        if(!ChargeTarget) { CurrentState = ELamboState::Grazing; break; }
        GetCharacterMovement()->MaxWalkSpeed = ChargeSpeed;
        FVector Dir = (ChargeTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal();
        AddMovementInput(Dir);
        float D = FVector::Dist(GetActorLocation(), ChargeTarget->GetActorLocation());
        if(D < 150.f)
        {
            if(ASurvivalCharacter* PC = Cast<ASurvivalCharacter>(ChargeTarget))
                PC->GetSurvivalComponent()->Server_ApplyDamage(ChargeDamage);
            CurrentState = ELamboState::Stunned;
            StunTimer = 2.f;
        }
        ChargeTimer -= DeltaTime;
        if(ChargeTimer <= 0.f) CurrentState = ELamboState::Fleeing;
        break;
    }
    case ELamboState::Fleeing:
    {
        GetCharacterMovement()->MaxWalkSpeed = 500.f;
        if(!Players.IsEmpty())
        {
            FVector Away = (GetActorLocation() - Players[0]->GetActorLocation()).GetSafeNormal();
            AddMovementInput(Away);
        }
        if(Players.IsEmpty() || FVector::Dist(GetActorLocation(), Players[0]->GetActorLocation()) > 1500.f)
            CurrentState = ELamboState::Grazing;
        break;
    }
    case ELamboState::Stunned:
        StunTimer -= DeltaTime;
        if(StunTimer <= 0.f) CurrentState = ELamboState::Fleeing;
        break;
    }
}

void ALamboGidroAI::TakeDamage_Lambo(float Amount)
{
    HP = FMath::Max(0.f, HP - Amount);
    CurrentState = ELamboState::Charging;
    if(HP <= 0.f) SetLifeSpan(2.f);
}
