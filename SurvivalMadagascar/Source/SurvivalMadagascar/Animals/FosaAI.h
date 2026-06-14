#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FosaAI.generated.h"

UENUM(BlueprintType)
enum class EFosaState : uint8
{
    Sleeping,   // Day: Fosa is sleeping, no danger
    Patrolling, // Night: searching for players
    Stalking,   // Night: spotted a player, circling
    Hunting,    // Night: chasing isolated player
    Retreating  // Near large fire or 3 players together
};

UCLASS()
class SURVIVALMADAGASCAR_API AFosaAI : public ACharacter
{
    GENERATED_BODY()

public:
    AFosaAI();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) float HP = 300.f;
    UPROPERTY(EditDefaultsOnly) float AttackDamage = 25.f;
    UPROPERTY(EditDefaultsOnly) float DetectionRadius = 80.f;    // meters
    UPROPERTY(EditDefaultsOnly) float FireFearRadius = 10.f;      // meters from fire
    UPROPERTY(EditDefaultsOnly) float AttackRadius = 2.f;
    UPROPERTY(EditDefaultsOnly) float PatrolSpeed = 300.f;
    UPROPERTY(EditDefaultsOnly) float ChaseSpeed = 600.f;

    UPROPERTY(Replicated, BlueprintReadOnly) EFosaState CurrentState = EFosaState::Sleeping;
    UPROPERTY(Replicated, BlueprintReadOnly) AActor* TargetPlayer = nullptr;

    UFUNCTION(BlueprintCallable) void TakeDamageFromPlayer(float Amount);
    UFUNCTION(BlueprintPure) bool IsActive() const { return CurrentState != EFosaState::Sleeping; }
    UFUNCTION(BlueprintPure) EFosaState GetState() const { return CurrentState; }

    // Called by DayNightCycle
    UFUNCTION(BlueprintCallable) void OnNightBegins();
    UFUNCTION(BlueprintCallable) void OnDayBegins();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    void UpdateAI(float DeltaTime);
    void FindTarget();
    bool IsPlayerNearFire(AActor* Player) const;
    bool AreAllPlayersGrouped() const;
    void AttackTarget();
    void Patrol(float DeltaTime);
    void ChaseTarget();
    void Roar();

    float AttackCooldown = 0.f;
    float RoarCooldown = 0.f;
    FVector PatrolDestination;
};
