#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChameleonIndicator.generated.h"

UENUM(BlueprintType)
enum class EChameleonColor : uint8
{
    Green,   // Salama tsara — tsy misy loza
    Yellow,  // Orana ho avy (5 min)
    Red,     // DANGER akaiky (Fosa na Menarana)
    Brown,   // Hafanana loatra
    Blue     // Alina miadana — tsy misy loza
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnColorChanged, EChameleonColor, NewColor);

UCLASS()
class SURVIVALMADAGASCAR_API AChameleonIndicator : public AActor
{
    GENERATED_BODY()

public:
    AChameleonIndicator();

    UPROPERTY(Replicated, BlueprintReadOnly) EChameleonColor CurrentColor = EChameleonColor::Green;
    UPROPERTY(EditDefaultsOnly) float CheckInterval = 5.f; // Check every 5 seconds
    UPROPERTY(EditDefaultsOnly) bool bIsTamed = false;

    UPROPERTY(BlueprintAssignable) FOnColorChanged OnColorChanged;

    UFUNCTION(BlueprintPure) static FLinearColor GetColorValue(EChameleonColor Color);
    UFUNCTION(BlueprintPure) static FText GetColorMeaning(EChameleonColor Color);

protected:
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    void UpdateColor();
    bool IsDangerNearby() const;
    bool IsRainComing() const;

    FTimerHandle CheckTimer;
    EChameleonColor PreviousColor = EChameleonColor::Green;
};
