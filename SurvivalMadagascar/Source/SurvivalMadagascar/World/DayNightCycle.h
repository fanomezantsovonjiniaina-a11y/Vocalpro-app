#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DayNightCycle.generated.h"

UENUM(BlueprintType)
enum class ETimeOfDay : uint8
{
    Morning,   // 06:00 - 09:00
    Day,       // 09:00 - 15:00
    Evening,   // 15:00 - 18:00
    Dusk,      // 18:00 - 19:00  WARNING zone
    Night,     // 19:00 - 04:00  DANGER zone
    PreDawn    // 04:00 - 06:00
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimeChanged, ETimeOfDay, NewTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHourChanged, float, Hour);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNightWarning);

UCLASS()
class SURVIVALMADAGASCAR_API ADayNightCycle : public AActor
{
    GENERATED_BODY()

public:
    ADayNightCycle();

    // 1 real minute = 1 in-game hour (20 min real = 1 day)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) float RealSecondsPerGameHour = 50.f;

    UPROPERTY(Replicated, BlueprintReadOnly) float CurrentHour = 6.f; // Start at 6am
    UPROPERTY(Replicated, BlueprintReadOnly) int32 CurrentDay = 1;
    UPROPERTY(Replicated, BlueprintReadOnly) ETimeOfDay TimeOfDay = ETimeOfDay::Morning;
    UPROPERTY(Replicated, BlueprintReadOnly) bool bIsNight = false;
    UPROPERTY(Replicated, BlueprintReadOnly) bool bIsRaining = false;

    UPROPERTY(BlueprintAssignable) FOnTimeChanged OnTimeOfDayChanged;
    UPROPERTY(BlueprintAssignable) FOnHourChanged OnHourChanged;
    UPROPERTY(BlueprintAssignable) FOnNightWarning OnNightWarning;

    UFUNCTION(BlueprintPure) float GetDangerMultiplier() const;
    UFUNCTION(BlueprintPure) FString GetTimeString() const;
    UFUNCTION(BlueprintCallable) void SetRaining(bool bRaining);

    // Sky/Light references — set in BP
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite) class ADirectionalLight* SunLight;
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite) class ASkyLight* SkyLight;
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite) class AExponentialHeightFog* Fog;

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    ETimeOfDay GetTimeOfDayForHour(float Hour) const;
    void UpdateSunPosition();
    void TryTriggerRain();

    ETimeOfDay PreviousTimeOfDay = ETimeOfDay::Morning;
    bool bWarningTriggered = false;
};
