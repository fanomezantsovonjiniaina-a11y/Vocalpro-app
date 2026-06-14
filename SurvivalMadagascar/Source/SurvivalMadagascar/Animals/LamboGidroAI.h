#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LamboGidroAI.generated.h"

UENUM(BlueprintType)
enum class ELamboState : uint8 { Grazing, Fleeing, Charging, Stunned };

UCLASS()
class SURVIVALMADAGASCAR_API ALamboGidroAI : public ACharacter
{
    GENERATED_BODY()
public:
    ALamboGidroAI();
    UPROPERTY(EditDefaultsOnly) float HP = 250.f;
    UPROPERTY(EditDefaultsOnly) float ChargeDamage = 20.f;
    UPROPERTY(EditDefaultsOnly) float ChargeSpeed = 700.f;
    UPROPERTY(EditDefaultsOnly) float AlertRadius = 500.f; // 5m
    UPROPERTY(Replicated, BlueprintReadOnly) ELamboState CurrentState = ELamboState::Grazing;
    UFUNCTION(BlueprintCallable) void TakeDamage_Lambo(float Amount);
protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
private:
    void UpdateAI(float DeltaTime);
    AActor* ChargeTarget = nullptr;
    float ChargeTimer = 0.f;
    float StunTimer = 0.f;
    float GrazeTimer = 0.f;
};
