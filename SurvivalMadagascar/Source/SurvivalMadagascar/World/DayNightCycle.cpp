#include "DayNightCycle.h"
#include "Net/UnrealNetwork.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Engine/ExponentialHeightFog.h"
#include "Kismet/GameplayStatics.h"

ADayNightCycle::ADayNightCycle()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
}

void ADayNightCycle::BeginPlay()
{
    Super::BeginPlay();
}

void ADayNightCycle::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!HasAuthority()) return;

    float HoursPerSecond = 1.f / RealSecondsPerGameHour;
    CurrentHour += HoursPerSecond * DeltaTime;

    if (CurrentHour >= 24.f)
    {
        CurrentHour -= 24.f;
        CurrentDay++;
        bWarningTriggered = false;
    }

    bIsNight = (CurrentHour >= 19.f || CurrentHour < 6.f);

    ETimeOfDay NewTime = GetTimeOfDayForHour(CurrentHour);
    if (NewTime != PreviousTimeOfDay)
    {
        TimeOfDay = NewTime;
        OnTimeOfDayChanged.Broadcast(TimeOfDay);
        PreviousTimeOfDay = NewTime;
    }

    // Warning at 17:30
    if (!bWarningTriggered && CurrentHour >= 17.5f && CurrentHour < 18.f)
    {
        bWarningTriggered = true;
        OnNightWarning.Broadcast();
    }

    OnHourChanged.Broadcast(CurrentHour);
    UpdateSunPosition();

    // Rain chance — higher at night
    if (FMath::RandRange(0, 10000) < (bIsNight ? 2 : 1))
        TryTriggerRain();
}

void ADayNightCycle::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ADayNightCycle, CurrentHour);
    DOREPLIFETIME(ADayNightCycle, CurrentDay);
    DOREPLIFETIME(ADayNightCycle, TimeOfDay);
    DOREPLIFETIME(ADayNightCycle, bIsNight);
    DOREPLIFETIME(ADayNightCycle, bIsRaining);
}

ETimeOfDay ADayNightCycle::GetTimeOfDayForHour(float Hour) const
{
    if (Hour >= 6.f  && Hour < 9.f)  return ETimeOfDay::Morning;
    if (Hour >= 9.f  && Hour < 15.f) return ETimeOfDay::Day;
    if (Hour >= 15.f && Hour < 18.f) return ETimeOfDay::Evening;
    if (Hour >= 18.f && Hour < 19.f) return ETimeOfDay::Dusk;
    if (Hour >= 19.f || Hour < 4.f)  return ETimeOfDay::Night;
    return ETimeOfDay::PreDawn;
}

void ADayNightCycle::UpdateSunPosition()
{
    if (!SunLight) return;

    // Sun angle: 0° at 6am, 180° at 18pm
    float SunAngle = (CurrentHour - 6.f) / 12.f * 180.f;
    SunLight->SetActorRotation(FRotator(SunAngle - 90.f, 0.f, 0.f));

    // Intensity based on time
    float Intensity = bIsNight ? 0.f : FMath::Sin(FMath::DegreesToRadians(SunAngle)) * 10.f;
    SunLight->GetLightComponent()->SetIntensity(FMath::Max(0.f, Intensity));
}

void ADayNightCycle::TryTriggerRain()
{
    bIsRaining = !bIsRaining;
}

void ADayNightCycle::SetRaining(bool bRaining)
{
    bIsRaining = bRaining;
}

float ADayNightCycle::GetDangerMultiplier() const
{
    if (TimeOfDay == ETimeOfDay::Night && bIsRaining) return 2.5f;
    if (TimeOfDay == ETimeOfDay::Night) return 1.5f;
    if (TimeOfDay == ETimeOfDay::Dusk) return 1.1f;
    return 1.f;
}

FString ADayNightCycle::GetTimeString() const
{
    int32 Hours = FMath::FloorToInt(CurrentHour);
    int32 Minutes = FMath::FloorToInt((CurrentHour - Hours) * 60.f);
    return FString::Printf(TEXT("Andro %d — %02d:%02d"), CurrentDay, Hours, Minutes);
}
