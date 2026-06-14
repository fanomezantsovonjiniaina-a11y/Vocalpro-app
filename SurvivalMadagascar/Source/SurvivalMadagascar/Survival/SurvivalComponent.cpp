#include "SurvivalComponent.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

USurvivalComponent::USurvivalComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    SetIsReplicatedByDefault(true);
}

void USurvivalComponent::BeginPlay()
{
    Super::BeginPlay();
}

void USurvivalComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!GetOwner()->HasAuthority()) return;

    UpdateNeeds(DeltaTime);
    ApplyStatusEffectTick(DeltaTime);
    CheckDeathConditions();
}

void USurvivalComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(USurvivalComponent, Stats);
    DOREPLIFETIME(USurvivalComponent, CurrentEffect);
}

void USurvivalComponent::UpdateNeeds(float DeltaTime)
{
    Stats.Hunger = FMath::Max(0.f, Stats.Hunger - HungerDecayRate * DeltaTime * 60.f);
    Stats.Thirst = FMath::Max(0.f, Stats.Thirst - ThirstDecayRate * DeltaTime * 60.f);
    Stats.Fatigue = FMath::Min(100.f, Stats.Fatigue + FatigueIncreaseRate * DeltaTime * 60.f);

    // HP drain if starving or dehydrated
    if (Stats.Hunger <= 0.f)
        Stats.HP = FMath::Max(0.f, Stats.HP - 5.f * DeltaTime);
    if (Stats.Thirst <= 0.f)
        Stats.HP = FMath::Max(0.f, Stats.HP - 8.f * DeltaTime);

    OnStatChanged.Broadcast(Stats);
}

void USurvivalComponent::ApplyStatusEffectTick(float DeltaTime)
{
    switch (CurrentEffect)
    {
    case EStatusEffect::Malaria:
        Stats.HP = FMath::Max(0.f, Stats.HP - 3.f * DeltaTime);
        Stats.Fatigue = FMath::Min(100.f, Stats.Fatigue + 2.f * DeltaTime);
        break;
    case EStatusEffect::SnakePoison:
        Stats.HP = FMath::Max(0.f, Stats.HP - 10.f * DeltaTime);
        break;
    case EStatusEffect::SpiderPoison:
        Stats.HP = FMath::Max(0.f, Stats.HP - 2.f * DeltaTime);
        break;
    case EStatusEffect::Infection:
        Stats.HP = FMath::Max(0.f, Stats.HP - 2.f * DeltaTime);
        break;
    default:
        break;
    }
}

void USurvivalComponent::CheckDeathConditions()
{
    if (Stats.HP <= 0.f && IsAlive())
    {
        Stats.HP = 0.f;
        OnDeath.Broadcast();
    }
}

void USurvivalComponent::Server_ApplyDamage_Implementation(float Amount)
{
    Stats.HP = FMath::Max(0.f, Stats.HP - Amount);
    OnStatChanged.Broadcast(Stats);
}

void USurvivalComponent::Server_Eat_Implementation(float Amount)
{
    Stats.Hunger = FMath::Min(100.f, Stats.Hunger + Amount);
    OnStatChanged.Broadcast(Stats);
}

void USurvivalComponent::Server_Drink_Implementation(float Amount)
{
    Stats.Thirst = FMath::Min(100.f, Stats.Thirst + Amount);
    OnStatChanged.Broadcast(Stats);
}

void USurvivalComponent::Server_Sleep_Implementation(float Duration)
{
    Stats.Fatigue = FMath::Max(0.f, Stats.Fatigue - Duration * 10.f);
    Stats.HP = FMath::Min(100.f, Stats.HP + Duration * 2.f);
    OnStatChanged.Broadcast(Stats);
}

void USurvivalComponent::Server_ApplyStatusEffect_Implementation(EStatusEffect Effect)
{
    CurrentEffect = Effect;
    OnStatusEffectApplied.Broadcast(Effect);
}

void USurvivalComponent::Server_CureStatusEffect_Implementation()
{
    CurrentEffect = EStatusEffect::None;
}
