#include "SurvivalHUD.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/HorizontalBox.h"
#include "TimerManager.h"

void USurvivalHUD::NativeConstruct()
{
    Super::NativeConstruct();

    if (NightIcon)  NightIcon->SetVisibility(ESlateVisibility::Hidden);
    if (RainIcon)   RainIcon->SetVisibility(ESlateVisibility::Hidden);
    if (StatusEffectText) StatusEffectText->SetVisibility(ESlateVisibility::Hidden);
}

void USurvivalHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
}

void USurvivalHUD::UpdateStats(const FSurvivalStats& Stats)
{
    if (!HPBar || !HungerBar || !ThirstBar || !FatigueBar) return;

    float HPPct      = Stats.HP      / 100.f;
    float HungerPct  = Stats.Hunger  / 100.f;
    float ThirstPct  = Stats.Thirst  / 100.f;
    float FatiguePct = Stats.Fatigue / 100.f;

    HPBar->SetPercent(HPPct);
    HungerBar->SetPercent(HungerPct);
    ThirstBar->SetPercent(ThirstPct);
    FatigueBar->SetPercent(1.f - FatiguePct); // Inverted: full = pas fatigué

    // Color change based on value
    auto GetBarColor = [this](float Pct) {
        if (Pct > 0.5f) return ColorSafe;
        if (Pct > 0.25f) return ColorWarning;
        return ColorDanger;
    };

    HPBar->SetFillColorAndOpacity(GetBarColor(HPPct));
    HungerBar->SetFillColorAndOpacity(GetBarColor(HungerPct));
    ThirstBar->SetFillColorAndOpacity(GetBarColor(ThirstPct));
}

void USurvivalHUD::UpdateTime(const FString& TimeStr, int32 Day, bool bIsNight, bool bIsRaining)
{
    if (TimeText) TimeText->SetText(FText::FromString(TimeStr));
    if (DayText)  DayText->SetText(FText::FromString(FString::Printf(TEXT("Alina %d / 30"), Day)));

    if (NightIcon) NightIcon->SetVisibility(bIsNight ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
    if (RainIcon)  RainIcon->SetVisibility(bIsRaining ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void USurvivalHUD::ShowStatusEffect(EStatusEffect Effect)
{
    if (!StatusEffectText) return;

    FString EffectName;
    switch (Effect)
    {
    case EStatusEffect::Malaria:     EffectName = "☠️ MALARIA"; break;
    case EStatusEffect::SnakePoison: EffectName = "🐍 POIZINA MENARANA"; break;
    case EStatusEffect::SpiderPoison:EffectName = "🕷️ POIZINA HAINTENY"; break;
    case EStatusEffect::Infection:   EffectName = "🤢 INFEKSIONA"; break;
    case EStatusEffect::Dehydration: EffectName = "💧 TSY MISY RANO"; break;
    default:
        StatusEffectText->SetVisibility(ESlateVisibility::Hidden);
        return;
    }

    StatusEffectText->SetText(FText::FromString(EffectName));
    StatusEffectText->SetColorAndOpacity(FSlateColor(ColorDanger));
    StatusEffectText->SetVisibility(ESlateVisibility::Visible);
}

void USurvivalHUD::ShowNightWarning()
{
    ShowDangerAlert("⚠️ NY ALINA MANAKAIKY... MIORENA!");
}

void USurvivalHUD::ShowDangerAlert(const FString& Message)
{
    if (!StatusEffectText) return;

    StatusEffectText->SetText(FText::FromString(Message));
    StatusEffectText->SetColorAndOpacity(FSlateColor(ColorWarning));
    StatusEffectText->SetVisibility(ESlateVisibility::Visible);

    // Auto-hide after 5 seconds
    GetWorld()->GetTimerManager().SetTimer(AlertTimer, [this]()
    {
        if (StatusEffectText)
            StatusEffectText->SetVisibility(ESlateVisibility::Hidden);
    }, 5.f, false);
}

void USurvivalHUD::ShowReviveProgress(float Progress)
{
    // Show revive progress bar — implemented in Blueprint WBP
    // Blueprint: Animate progress ring from 0 to Progress
}

void USurvivalHUD::HideReviveProgress()
{
    // Hide revive UI — implemented in Blueprint
}

void USurvivalHUD::UpdateTeammateStatus(int32 PlayerIndex, float HP, bool bIsDown)
{
    // Update teammate HP icons at top of screen
    // Blueprint handles the visual (portrait + HP bar per player)
}

void USurvivalHUD::AnimateWarning()
{
    // Flash warning — Blueprint animation
}
