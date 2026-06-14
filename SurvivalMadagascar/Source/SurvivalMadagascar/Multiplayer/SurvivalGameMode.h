#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SurvivalGameMode.generated.h"

UCLASS()
class SURVIVALMADAGASCAR_API ASurvivalGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ASurvivalGameMode();

    UPROPERTY(EditDefaultsOnly) int32 MaxPlayers = 3;
    UPROPERTY(EditDefaultsOnly) int32 NightsToWin = 30;

    UPROPERTY(BlueprintReadOnly) int32 CurrentNight = 0;
    UPROPERTY(BlueprintReadOnly) int32 PlayersAlive = 0;

    UFUNCTION(BlueprintCallable) void OnNightBegins();
    UFUNCTION(BlueprintCallable) void OnDayBegins();
    UFUNCTION(BlueprintCallable) void OnPlayerDied(APlayerController* PC);
    UFUNCTION(BlueprintCallable) void OnPlayerRevived(APlayerController* PC);
    UFUNCTION(BlueprintCallable) void CheckVictoryCondition();

    virtual void PostLogin(APlayerController* NewPlayer) override;
    virtual void Logout(AController* Exiting) override;

protected:
    virtual void BeginPlay() override;

private:
    void SpawnFosa();
    void RespawnPlayer(APlayerController* PC);

    TArray<APlayerController*> ConnectedPlayers;
};
