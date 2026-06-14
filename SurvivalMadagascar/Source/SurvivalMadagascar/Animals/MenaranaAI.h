#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Survival/SurvivalComponent.h"
#include "MenaranaAI.generated.h"

UENUM(BlueprintType)
enum class EMenaranaType : uint8
{
    Boa,        // Lehibe, mamatotra — -30 HP, tsy misy poizina
    Langaha,    // Kely, poizina mafy — -8 HP/min x 5min
    Ithycyphus  // Ao amin'ny hazo, poizina modere — -4 HP/min x 3min
};

UENUM(BlueprintType)
enum class EMenaranaState : uint8
{
    Hidden,     // Miafina (tsy hita)
    Resting,    // Matory amin'ny masoandro
    Hunting,    // Mihaza biby kely
    Striking,   // Mikapoka player!
    Retreating  // Manalavitra
};

UCLASS()
class SURVIVALMADAGASCAR_API AMenaranaAI : public ACharacter
{
    GENERATED_BODY()

public:
    AMenaranaAI();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) EMenaranaType SnakeType = EMenaranaType::Langaha;
    UPROPERTY(Replicated, BlueprintReadOnly) EMenaranaState CurrentState = EMenaranaState::Hidden;
    UPROPERTY(EditDefaultsOnly) float HP = 80.f;
    UPROPERTY(EditDefaultsOnly) float DetectionRadius = 2.f; // 2m — very close!
    UPROPERTY(EditDefaultsOnly) float StrikeRange = 1.5f;

    UFUNCTION(BlueprintCallable) void TakeDamage_Snake(float Amount);
    UFUNCTION(BlueprintPure) EStatusEffect GetPoisonType() const;
    UFUNCTION(BlueprintPure) bool IsHidden() const { return CurrentState == EMenaranaState::Hidden; }

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    void UpdateAI(float DeltaTime);
    AActor* GetPlayerInRange() const;
    void StrikePlayer(AActor* Player);

    float StrikeCooldown = 0.f;
    bool bHasStruck = false;
};
