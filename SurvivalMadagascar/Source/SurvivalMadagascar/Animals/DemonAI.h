#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Survival/SurvivalComponent.h"
#include "DemonAI.generated.h"

UENUM(BlueprintType)
enum class EDemonType : uint8
{
    Kalanoro,  // Kely, haingana, semi-visible, mihaza alina
    Kinoly,    // Fanahy maty — mamposa, masiaka amin'ny orana
    Angatra    // Invisible tanteraka — tsapana fotsiny, mikapoka avy amin'ny lavitra
};

UENUM(BlueprintType)
enum class EDemonState : uint8
{
    Dormant,    // Miandry (andro)
    Lurking,    // Mihodina mangingina
    Haunting,   // Mamposa ny player (fear effect)
    Attacking,  // Mikapoka!
    Banished    // Voaroaka (afo + fanafody)
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerHaunted);

UCLASS()
class SURVIVALMADAGASCAR_API ADemonAI : public ACharacter
{
    GENERATED_BODY()
public:
    ADemonAI();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) EDemonType DemonType = EDemonType::Kalanoro;
    UPROPERTY(Replicated, BlueprintReadOnly) EDemonState CurrentState = EDemonState::Dormant;
    UPROPERTY(EditDefaultsOnly) float HP = 150.f;
    UPROPERTY(EditDefaultsOnly) float AttackDamage = 35.f;

    // Visibility: Kalanoro=0.4, Kinoly=0.2, Angatra=0.05
    UPROPERTY(Replicated, BlueprintReadOnly) float Visibility = 0.4f;

    UPROPERTY(BlueprintAssignable) FOnPlayerHaunted OnPlayerHaunted;

    UFUNCTION(BlueprintCallable) void OnNightBegins();
    UFUNCTION(BlueprintCallable) void OnDayBegins();
    UFUNCTION(BlueprintCallable) void TakeDamage_Demon(float Amount, bool bHolyDamage = false);
    UFUNCTION(BlueprintPure) bool IsVisible() const { return Visibility > 0.1f; }

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    void UpdateAI(float DeltaTime);
    void UpdateVisibility(float DeltaTime);
    AActor* FindTarget() const;
    bool IsNearFire() const;

    AActor* Target = nullptr;
    float HauntTimer = 0.f;
    float PhaseTimer = 0.f;
    float AttackCooldown = 0.f;
};
