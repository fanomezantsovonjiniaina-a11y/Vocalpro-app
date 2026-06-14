#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeatherSystem.generated.h"

UENUM(BlueprintType)
enum class EWeatherType : uint8
{
    Clear,      // Mazava tsara
    Cloudy,     // Rahona — manamaizina kely
    Rain,       // Orana — maty afo, tsy henon'ny Fosa
    HeavyRain,  // Orana be — maty afo haingana, Fosa + Démon masiaka kokoa
    Storm,      // Riodoza — tena mampidi-doza! Tsy hita na inona
    Fog         // Bodofotsy — tsy hita lavitra
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeatherChanged, EWeatherType, NewWeather);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStormWarning);

UCLASS()
class SURVIVALMADAGASCAR_API AWeatherSystem : public AActor
{
    GENERATED_BODY()
public:
    AWeatherSystem();

    UPROPERTY(Replicated, BlueprintReadOnly) EWeatherType CurrentWeather = EWeatherType::Clear;
    UPROPERTY(Replicated, BlueprintReadOnly) float RainIntensity = 0.f;      // 0-1
    UPROPERTY(Replicated, BlueprintReadOnly) float WindSpeed = 0.f;          // 0-100
    UPROPERTY(Replicated, BlueprintReadOnly) float Visibility = 1.f;         // 0-1
    UPROPERTY(Replicated, BlueprintReadOnly) float Temperature = 28.f;       // Celsius
    UPROPERTY(Replicated, BlueprintReadOnly) float WeatherDuration = 0.f;    // seconds remaining

    UPROPERTY(BlueprintAssignable) FOnWeatherChanged OnWeatherChanged;
    UPROPERTY(BlueprintAssignable) FOnStormWarning OnStormWarning;

    UFUNCTION(BlueprintPure) bool IsRaining() const;
    UFUNCTION(BlueprintPure) bool IsStorming() const { return CurrentWeather == EWeatherType::Storm; }
    UFUNCTION(BlueprintPure) bool IsFoggy() const { return CurrentWeather == EWeatherType::Fog; }
    UFUNCTION(BlueprintPure) float GetFosaSpeedMultiplier() const;
    UFUNCTION(BlueprintPure) float GetDemonStrengthMultiplier() const;
    UFUNCTION(BlueprintPure) FText GetWeatherDescription() const;

    // Called by DayNightCycle
    UFUNCTION(BlueprintCallable) void OnNewDay(int32 DayNumber);

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    void SetWeather(EWeatherType NewWeather, float Duration);
    void UpdateWeatherEffects(float DeltaTime);
    void RollNextWeather();
    EWeatherType GetWeightedWeather(bool bIsNight) const;
};
