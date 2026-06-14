#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Survival/SurvivalComponent.h"
#include "SurvivalHUD.generated.h"

UCLASS()
class SURVIVALMADAGASCAR_API USurvivalHUD : public UUserWidget
{
    GENERATED_BODY()

public:
    // Bind these in Blueprint WBP_SurvivalHUD
    UPROPERTY(meta=(BindWidget)) class UProgressBar* HPBar;
    UPROPERTY(meta=(BindWidget)) class UProgressBar* HungerBar;
    UPROPERTY(meta=(BindWidget)) class UProgressBar* ThirstBar;
    UPROPERTY(meta=(BindWidget)) class UProgressBar* FatigueBar;
    UPROPERTY(meta=(BindWidget)) class UTextBlock* TimeText;
    UPROPERTY(meta=(BindWidget)) class UTextBlock* DayText;
    UPROPERTY(meta=(BindWidget)) class UTextBlock* StatusEffectText;
    UPROPERTY(meta=(BindWidget)) class UImage* NightIcon;
    UPROPERTY(meta=(BindWidget)) class UImage* RainIcon;
    UPROPERTY(meta=(BindWidget)) class UHorizontalBox* QuickInventory;

    // Called by PlayerController every tick
    UFUNCTION(BlueprintCallable) void UpdateStats(const FSurvivalStats& Stats);
    UFUNCTION(BlueprintCallable) void UpdateTime(const FString& TimeStr, int32 Day, bool bIsNight, bool bIsRaining);
    UFUNCTION(BlueprintCallable) void ShowStatusEffect(EStatusEffect Effect);
    UFUNCTION(BlueprintCallable) void ShowNightWarning();
    UFUNCTION(BlueprintCallable) void ShowDangerAlert(const FString& Message);
    UFUNCTION(BlueprintCallable) void ShowReviveProgress(float Progress); // 0-1
    UFUNCTION(BlueprintCallable) void HideReviveProgress();

    // Teammate status (top of screen)
    UFUNCTION(BlueprintCallable) void UpdateTeammateStatus(int32 PlayerIndex, float HP, bool bIsDown);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
    void AnimateWarning();
    FTimerHandle WarningTimer;
    FTimerHandle AlertTimer;

    // Colors
    const FLinearColor ColorSafe    = FLinearColor(0.1f, 0.8f, 0.1f); // Vert
    const FLinearColor ColorWarning = FLinearColor(1.f,  0.6f, 0.f);  // Orange
    const FLinearColor ColorDanger  = FLinearColor(0.9f, 0.1f, 0.1f); // Rouge
};
