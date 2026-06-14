#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SurvivalComponent.generated.h"

UENUM(BlueprintType)
enum class EStatusEffect : uint8
{
    None,
    Malaria,
    SnakePoison,
    SpiderPoison,
    Infection,
    Dehydration
};

USTRUCT(BlueprintType)
struct FSurvivalStats
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite) float HP = 100.f;
    UPROPERTY(BlueprintReadWrite) float Hunger = 100.f;
    UPROPERTY(BlueprintReadWrite) float Thirst = 100.f;
    UPROPERTY(BlueprintReadWrite) float Fatigue = 0.f;
    UPROPERTY(BlueprintReadWrite) float Temperature = 37.f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatChanged, FSurvivalStats, Stats);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatusEffect, EStatusEffect, Effect);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SURVIVALMADAGASCAR_API USurvivalComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USurvivalComponent();

    UPROPERTY(BlueprintAssignable) FOnStatChanged OnStatChanged;
    UPROPERTY(BlueprintAssignable) FOnStatusEffect OnStatusEffectApplied;
    UPROPERTY(BlueprintAssignable) FOnDeath OnDeath;

    UPROPERTY(Replicated, BlueprintReadOnly) FSurvivalStats Stats;
    UPROPERTY(Replicated, BlueprintReadOnly) EStatusEffect CurrentEffect = EStatusEffect::None;

    // Rates per second
    UPROPERTY(EditDefaultsOnly) float HungerDecayRate = 0.033f;   // -2/min
    UPROPERTY(EditDefaultsOnly) float ThirstDecayRate = 0.05f;    // -3/min
    UPROPERTY(EditDefaultsOnly) float FatigueIncreaseRate = 0.017f; // +1/min

    UFUNCTION(BlueprintCallable, Server, Reliable) void Server_ApplyDamage(float Amount);
    UFUNCTION(BlueprintCallable, Server, Reliable) void Server_Eat(float Amount);
    UFUNCTION(BlueprintCallable, Server, Reliable) void Server_Drink(float Amount);
    UFUNCTION(BlueprintCallable, Server, Reliable) void Server_Sleep(float Duration);
    UFUNCTION(BlueprintCallable, Server, Reliable) void Server_ApplyStatusEffect(EStatusEffect Effect);
    UFUNCTION(BlueprintCallable, Server, Reliable) void Server_CureStatusEffect();

    UFUNCTION(BlueprintPure) bool IsAlive() const { return Stats.HP > 0.f; }
    UFUNCTION(BlueprintPure) bool IsCritical() const { return Stats.HP < 20.f; }
    UFUNCTION(BlueprintPure) bool IsHungry() const { return Stats.Hunger < 30.f; }
    UFUNCTION(BlueprintPure) bool IsThirsty() const { return Stats.Thirst < 30.f; }
    UFUNCTION(BlueprintPure) bool IsExhausted() const { return Stats.Fatigue > 80.f; }

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    void UpdateNeeds(float DeltaTime);
    void ApplyStatusEffectTick(float DeltaTime);
    void CheckDeathConditions();

    FTimerHandle StatusEffectTimer;
};
