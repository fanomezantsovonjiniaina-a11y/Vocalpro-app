#include "WeatherSystem.h"
#include "Net/UnrealNetwork.h"
#include "../World/DayNightCycle.h"
#include "Kismet/GameplayStatics.h"

AWeatherSystem::AWeatherSystem() { PrimaryActorTick.bCanEverTick = true; bReplicates = true; }

void AWeatherSystem::BeginPlay()
{
    Super::BeginPlay();
    SetWeather(EWeatherType::Clear, 300.f);
}

void AWeatherSystem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if(!HasAuthority()) return;
    WeatherDuration -= DeltaTime;
    if(WeatherDuration <= 0.f) RollNextWeather();
    UpdateWeatherEffects(DeltaTime);
}

void AWeatherSystem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AWeatherSystem, CurrentWeather);
    DOREPLIFETIME(AWeatherSystem, RainIntensity);
    DOREPLIFETIME(AWeatherSystem, WindSpeed);
    DOREPLIFETIME(AWeatherSystem, Visibility);
    DOREPLIFETIME(AWeatherSystem, Temperature);
    DOREPLIFETIME(AWeatherSystem, WeatherDuration);
}

void AWeatherSystem::SetWeather(EWeatherType NewWeather, float Duration)
{
    EWeatherType Old = CurrentWeather;
    CurrentWeather = NewWeather;
    WeatherDuration = Duration;

    // Storm warning
    if(NewWeather == EWeatherType::Storm) OnStormWarning.Broadcast();
    if(NewWeather != Old) OnWeatherChanged.Broadcast(NewWeather);
}

void AWeatherSystem::UpdateWeatherEffects(float DeltaTime)
{
    switch(CurrentWeather)
    {
    case EWeatherType::Clear:
        RainIntensity = FMath::FInterpTo(RainIntensity, 0.f, DeltaTime, 1.f);
        WindSpeed = FMath::FInterpTo(WindSpeed, 5.f, DeltaTime, 0.5f);
        Visibility = FMath::FInterpTo(Visibility, 1.f, DeltaTime, 1.f);
        Temperature = FMath::FInterpTo(Temperature, 28.f, DeltaTime, 0.5f);
        break;
    case EWeatherType::Cloudy:
        RainIntensity = FMath::FInterpTo(RainIntensity, 0.f, DeltaTime, 0.5f);
        WindSpeed = FMath::FInterpTo(WindSpeed, 20.f, DeltaTime, 0.3f);
        Visibility = FMath::FInterpTo(Visibility, 0.8f, DeltaTime, 0.5f);
        Temperature = FMath::FInterpTo(Temperature, 24.f, DeltaTime, 0.3f);
        break;
    case EWeatherType::Rain:
        RainIntensity = FMath::FInterpTo(RainIntensity, 0.5f, DeltaTime, 0.5f);
        WindSpeed = FMath::FInterpTo(WindSpeed, 30.f, DeltaTime, 0.5f);
        Visibility = FMath::FInterpTo(Visibility, 0.6f, DeltaTime, 0.5f);
        Temperature = FMath::FInterpTo(Temperature, 20.f, DeltaTime, 0.3f);
        break;
    case EWeatherType::HeavyRain:
        RainIntensity = FMath::FInterpTo(RainIntensity, 0.9f, DeltaTime, 0.8f);
        WindSpeed = FMath::FInterpTo(WindSpeed, 60.f, DeltaTime, 0.8f);
        Visibility = FMath::FInterpTo(Visibility, 0.4f, DeltaTime, 0.8f);
        Temperature = FMath::FInterpTo(Temperature, 18.f, DeltaTime, 0.5f);
        break;
    case EWeatherType::Storm:
        RainIntensity = FMath::FInterpTo(RainIntensity, 1.f, DeltaTime, 1.f);
        WindSpeed = FMath::FInterpTo(WindSpeed, 100.f, DeltaTime, 1.f);
        Visibility = FMath::FInterpTo(Visibility, 0.2f, DeltaTime, 1.f);
        Temperature = FMath::FInterpTo(Temperature, 16.f, DeltaTime, 0.5f);
        break;
    case EWeatherType::Fog:
        RainIntensity = FMath::FInterpTo(RainIntensity, 0.1f, DeltaTime, 0.3f);
        WindSpeed = FMath::FInterpTo(WindSpeed, 2.f, DeltaTime, 0.2f);
        Visibility = FMath::FInterpTo(Visibility, 0.25f, DeltaTime, 0.5f);
        Temperature = FMath::FInterpTo(Temperature, 19.f, DeltaTime, 0.3f);
        break;
    }
}

EWeatherType AWeatherSystem::GetWeightedWeather(bool bIsNight) const
{
    // Weights: higher = more likely
    TArray<TPair<EWeatherType,int32>> Weights;
    if(bIsNight)
    {
        Weights = {
            {EWeatherType::Clear,     30},
            {EWeatherType::Cloudy,    25},
            {EWeatherType::Rain,      20},
            {EWeatherType::HeavyRain, 10},
            {EWeatherType::Storm,      5},
            {EWeatherType::Fog,       10}
        };
    }
    else
    {
        Weights = {
            {EWeatherType::Clear,     50},
            {EWeatherType::Cloudy,    25},
            {EWeatherType::Rain,      15},
            {EWeatherType::HeavyRain,  5},
            {EWeatherType::Storm,      2},
            {EWeatherType::Fog,        3}
        };
    }

    int32 Total = 0;
    for(auto& W : Weights) Total += W.Value;
    int32 Roll = FMath::RandRange(0, Total-1);
    int32 Acc = 0;
    for(auto& W : Weights) { Acc += W.Value; if(Roll < Acc) return W.Key; }
    return EWeatherType::Clear;
}

void AWeatherSystem::RollNextWeather()
{
    ADayNightCycle* DNC = Cast<ADayNightCycle>(
        UGameplayStatics::GetActorOfClass(GetWorld(), ADayNightCycle::StaticClass()));
    bool bIsNight = DNC ? DNC->bIsNight : false;

    EWeatherType Next = GetWeightedWeather(bIsNight);
    float Duration = FMath::RandRange(120.f, 600.f); // 2-10 min
    SetWeather(Next, Duration);
}

void AWeatherSystem::OnNewDay(int32 DayNumber)
{
    // Storm more likely as days progress
    if(DayNumber % 7 == 0) SetWeather(EWeatherType::Storm, 180.f);
}

bool AWeatherSystem::IsRaining() const
{
    return CurrentWeather == EWeatherType::Rain ||
           CurrentWeather == EWeatherType::HeavyRain ||
           CurrentWeather == EWeatherType::Storm;
}

float AWeatherSystem::GetFosaSpeedMultiplier() const
{
    switch(CurrentWeather)
    {
    case EWeatherType::Storm:     return 1.5f; // Fosa masiaka kokoa!
    case EWeatherType::HeavyRain: return 1.3f;
    case EWeatherType::Rain:      return 1.1f;
    default: return 1.f;
    }
}

float AWeatherSystem::GetDemonStrengthMultiplier() const
{
    switch(CurrentWeather)
    {
    case EWeatherType::Storm:     return 2.f;  // Démon mahery be amin'ny riodoza!
    case EWeatherType::HeavyRain: return 1.5f;
    case EWeatherType::Fog:       return 1.8f; // Fog = perfect for demons
    default: return 1.f;
    }
}

FText AWeatherSystem::GetWeatherDescription() const
{
    switch(CurrentWeather)
    {
    case EWeatherType::Clear:     return FText::FromString("☀️ Mazava tsara");
    case EWeatherType::Cloudy:    return FText::FromString("⛅ Rahona");
    case EWeatherType::Rain:      return FText::FromString("🌧️ Orana — Miaro ny afo!");
    case EWeatherType::HeavyRain: return FText::FromString("⛈️ Orana be — LOZA!");
    case EWeatherType::Storm:     return FText::FromString("🌪️ RIODOZA — Miafina!");
    case EWeatherType::Fog:       return FText::FromString("🌫️ Bodofotsy — Tsy hita!");
    default: return FText::GetEmpty();
    }
}
