#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "KrokodilAI.generated.h"

UENUM(BlueprintType)
enum class EKrokodilState : uint8
{
    Idle,       // Miandry mangingina (invisible ao anaty rano)
    Patrolling, // Mihodina ao anaty rano
    Lurking,    // Player akaiky — miandry fotoana
    Attacking,  // Mivaingana haingana!
    Retreating  // Raha very ny target
};

UCLASS()
class SURVIVALMADAGASCAR_API AKrokodilAI : public ACharacter
{
    GENERATED_BODY()

public:
    AKrokodilAI();

    UPROPERTY(EditDefaultsOnly) float HP = 500.f;
    UPROPERTY(EditDefaultsOnly) float AttackDamage = 40.f;
    UPROPERTY(EditDefaultsOnly) float WaterDetectionRadius = 50.f;  // 50m in water
    UPROPERTY(EditDefaultsOnly) float LandDetectionRadius = 5.f;    // 5m on land
    UPROPERTY(EditDefaultsOnly) float MaxLandDistance = 15.f;       // 15m max from water
    UPROPERTY(EditDefaultsOnly) float LandSpeed = 200.f;
    UPROPERTY(EditDefaultsOnly) float WaterSpeed = 500.f;

    UPROPERTY(Replicated, BlueprintReadOnly) EKrokodilState CurrentState = EKrokodilState::Idle;
    UPROPERTY(Replicated, BlueprintReadOnly) bool bIsInWater = false;

    UFUNCTION(BlueprintCallable) void TakeDamageFromPlayer(float Amount, FVector HitLocation);
    UFUNCTION(BlueprintPure) bool IsVulnerable(FVector HitLocation) const;

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    void UpdateAI(float DeltaTime);
    void FindTarget();
    AActor* GetNearestPlayerInRange(float Radius) const;
    bool IsPlayerInWater(AActor* Player) const;
    void LaunchAttack();

    AActor* TargetPlayer = nullptr;
    float AttackCooldown = 0.f;
    float LurkTimer = 0.f;
    FVector WaterAnchor; // Point de retour si trop loin de l'eau
};
