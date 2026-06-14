#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Survival/SurvivalComponent.h"
#include "SurvivalCharacter.generated.h"

UENUM(BlueprintType)
enum class EPlayerRole : uint8
{
    Hunter,   // Combat + chasse
    Healer,   // Médecine + herbes
    Builder   // Construction + défense
};

UCLASS()
class SURVIVALMADAGASCAR_API ASurvivalCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ASurvivalCharacter();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) USurvivalComponent* SurvivalComponent;

    UPROPERTY(Replicated, BlueprintReadOnly) EPlayerRole Role = EPlayerRole::Hunter;
    UPROPERTY(Replicated, BlueprintReadOnly) bool bIsDown = false;     // Knocked down, waiting revive
    UPROPERTY(Replicated, BlueprintReadOnly) float ReviveProgress = 0.f;

    // Multiplayer — revive system
    UFUNCTION(BlueprintCallable, Server, Reliable) void Server_StartRevive(ASurvivalCharacter* Target);
    UFUNCTION(BlueprintCallable, Server, Reliable) void Server_StopRevive();
    UFUNCTION(BlueprintCallable, Server, Reliable) void Server_SetRole(EPlayerRole NewRole);

    // Interaction
    UFUNCTION(BlueprintCallable, Server, Reliable) void Server_Interact(AActor* Target);

    UFUNCTION(BlueprintPure) USurvivalComponent* GetSurvivalComponent() const { return SurvivalComponent; }
    UFUNCTION(BlueprintPure) bool IsDown() const { return bIsDown; }
    UFUNCTION(BlueprintPure) float GetMoveSpeedModifier() const;

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    void OnPlayerDeath();
    void UpdateRevive(float DeltaTime);

    ASurvivalCharacter* ReviveTarget = nullptr;
    float ReviveTimer = 0.f;
    static constexpr float ReviveDuration = 10.f;
    static constexpr float DownedTimeout = 60.f;
    float DownedTimer = 0.f;
};
