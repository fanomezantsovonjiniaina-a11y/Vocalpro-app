#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LemurAI.generated.h"

UENUM(BlueprintType)
enum class ELemurType : uint8
{
    Catta,   // Apprivoisable, trouve fruits
    Indri,   // Crie si Fosa proche
    AyeAye   // Rare, porte-bonheur
};

UENUM(BlueprintType)
enum class ELemurState : uint8
{
    Wild,        // Libre, fuit les joueurs
    Curious,     // Player donne nourriture — intéressé
    Taming,      // En cours d'apprivoisement (3 jours)
    Tamed,       // Apprivoisé — suit le joueur
    Alerting,    // Indri: crie car Fosa détectée!
    Fleeing      // Fosa trop proche
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLemurTamed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFosaDetected);

UCLASS()
class SURVIVALMADAGASCAR_API ALemurAI : public ACharacter
{
    GENERATED_BODY()

public:
    ALemurAI();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) ELemurType LemurType = ELemurType::Catta;
    UPROPERTY(Replicated, BlueprintReadOnly) ELemurState CurrentState = ELemurState::Wild;
    UPROPERTY(Replicated, BlueprintReadOnly) AActor* Owner = nullptr;
    UPROPERTY(Replicated, BlueprintReadOnly) int32 TamingDaysLeft = 3;

    UPROPERTY(BlueprintAssignable) FOnLemurTamed OnTamed;
    UPROPERTY(BlueprintAssignable) FOnFosaDetected OnFosaDetected;

    UFUNCTION(BlueprintCallable) void OfferFood(AActor* Player, FName FoodItemID);
    UFUNCTION(BlueprintCallable) void OnNewDay(); // Called by DayNightCycle
    UFUNCTION(BlueprintPure) bool IsTamed() const { return CurrentState == ELemurState::Tamed; }

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    void UpdateAI(float DeltaTime);
    void CheckForFosa();
    void FindFruitForOwner();
    bool IsFosaNearby(float Radius) const;

    AActor* FoodGiver = nullptr;
    float AlertCooldown = 0.f;
    float FruitSearchCooldown = 0.f;
};
