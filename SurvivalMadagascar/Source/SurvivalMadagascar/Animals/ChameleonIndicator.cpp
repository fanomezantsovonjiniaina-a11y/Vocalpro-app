#include "ChameleonIndicator.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "../Animals/FosaAI.h"
#include "../Animals/MenaranaAI.h"
#include "../World/DayNightCycle.h"

AChameleonIndicator::AChameleonIndicator()
{
    bReplicates = true;
}

void AChameleonIndicator::BeginPlay()
{
    Super::BeginPlay();
    if (HasAuthority())
        GetWorld()->GetTimerManager().SetTimer(CheckTimer, this, &AChameleonIndicator::UpdateColor, CheckInterval, true);
}

void AChameleonIndicator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AChameleonIndicator, CurrentColor);
}

void AChameleonIndicator::UpdateColor()
{
    EChameleonColor NewColor = EChameleonColor::Green;

    // Priority: Danger > Rain > Temperature > Night > Safe
    if (IsDangerNearby())
        NewColor = EChameleonColor::Red;
    else if (IsRainComing())
        NewColor = EChameleonColor::Yellow;
    else
    {
        ADayNightCycle* DNC = Cast<ADayNightCycle>(
            UGameplayStatics::GetActorOfClass(GetWorld(), ADayNightCycle::StaticClass()));

        if (DNC)
        {
            if (DNC->bIsNight && !DNC->bIsRaining)
                NewColor = EChameleonColor::Blue;
            else if (DNC->CurrentHour >= 11.f && DNC->CurrentHour <= 14.f)
                NewColor = EChameleonColor::Brown; // Mitataovovonana — mafy ny hafanana
        }
    }

    if (NewColor != PreviousColor)
    {
        CurrentColor = NewColor;
        PreviousColor = NewColor;
        OnColorChanged.Broadcast(CurrentColor);
    }
}

bool AChameleonIndicator::IsDangerNearby() const
{
    float CheckRadius = bIsTamed ? 8000.f : 2000.f; // Tamed = 80m, Wild = 20m

    // Fosa active nearby
    TArray<AActor*> Fosas;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFosaAI::StaticClass(), Fosas);
    for (AActor* F : Fosas)
    {
        AFosaAI* Fosa = Cast<AFosaAI>(F);
        if (Fosa && Fosa->IsActive() &&
            FVector::Dist(GetActorLocation(), F->GetActorLocation()) < CheckRadius)
            return true;
    }

    // Snake nearby
    TArray<AActor*> Snakes;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMenaranaAI::StaticClass(), Snakes);
    for (AActor* S : Snakes)
        if (FVector::Dist(GetActorLocation(), S->GetActorLocation()) < 500.f) // 5m
            return true;

    return false;
}

bool AChameleonIndicator::IsRainComing() const
{
    ADayNightCycle* DNC = Cast<ADayNightCycle>(
        UGameplayStatics::GetActorOfClass(GetWorld(), ADayNightCycle::StaticClass()));
    if (!DNC) return false;
    // Yellow if night is within 30 min (hariva 17:00+) or rain coming
    return (DNC->CurrentHour >= 17.f && DNC->CurrentHour < 18.f);
}

FLinearColor AChameleonIndicator::GetColorValue(EChameleonColor Color)
{
    switch (Color)
    {
    case EChameleonColor::Green:  return FLinearColor(0.1f, 0.9f, 0.1f);
    case EChameleonColor::Yellow: return FLinearColor(1.f,  0.9f, 0.f);
    case EChameleonColor::Red:    return FLinearColor(0.9f, 0.05f, 0.05f);
    case EChameleonColor::Brown:  return FLinearColor(0.5f, 0.3f, 0.1f);
    case EChameleonColor::Blue:   return FLinearColor(0.1f, 0.4f, 0.9f);
    default: return FLinearColor::White;
    }
}

FText AChameleonIndicator::GetColorMeaning(EChameleonColor Color)
{
    switch (Color)
    {
    case EChameleonColor::Green:  return FText::FromString("Salama tsara");
    case EChameleonColor::Yellow: return FText::FromString("Orana ho avy — miorena!");
    case EChameleonColor::Red:    return FText::FromString("DANGER akaiky!");
    case EChameleonColor::Brown:  return FText::FromString("Hafanana loatra — misakafo");
    case EChameleonColor::Blue:   return FText::FromString("Alina miadana");
    default: return FText::GetEmpty();
    }
}
