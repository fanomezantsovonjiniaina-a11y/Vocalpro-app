#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Survival/InventoryComponent.h"
#include "FishingSystem.generated.h"

UENUM(BlueprintType)
enum class EFishType : uint8
{
    Tilapia,    // Rivière, facile — +20 faim
    Crevette,   // Estuaire — +15 faim
    Anguille,   // Marécage — +25 faim, DANGER choc électrique!
    PoissonMer, // Mer — rare, +40 faim (requiert pirogue)
    Nothing     // Rien
};

UENUM(BlueprintType)
enum class EPirogueState : uint8
{
    NotBuilt,
    Built,
    AtSea,
    Damaged
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFishCaught, EFishType, Fish);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnElectricShock);

UCLASS()
class SURVIVALMADAGASCAR_API AFishingSystem : public AActor
{
    GENERATED_BODY()
public:
    AFishingSystem();

    UPROPERTY(Replicated, BlueprintReadOnly) bool bIsFishing = false;
    UPROPERTY(Replicated, BlueprintReadOnly) float FishingProgress = 0.f;
    UPROPERTY(Replicated, BlueprintReadOnly) EPirogueState PirogueState = EPirogueState::NotBuilt;
    UPROPERTY(Replicated, BlueprintReadOnly) float PirogueHP = 100.f;

    UPROPERTY(BlueprintAssignable) FOnFishCaught OnFishCaught;
    UPROPERTY(BlueprintAssignable) FOnElectricShock OnElectricShock;

    // Pirogue recipe: Hazo x20 + Liane x10 + Pierre x5
    UFUNCTION(BlueprintCallable, Server, Reliable) void Server_BuildPirogue(UInventoryComponent* Inv);
    UFUNCTION(BlueprintCallable, Server, Reliable) void Server_StartFishing(AActor* Player, bool bInMarais);
    UFUNCTION(BlueprintCallable, Server, Reliable) void Server_StopFishing();
    UFUNCTION(BlueprintCallable, Server, Reliable) void Server_RepairPirogue(UInventoryComponent* Inv);
    UFUNCTION(BlueprintPure) bool CanFishInSea() const { return PirogueState == EPirogueState::Built; }

protected:
    virtual void Tick(float DeltaTime) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    EFishType RollFish(bool bInMarais, bool bAtSea) const;
    void GiveFishToPlayer();
    AActor* FishingPlayer = nullptr;
    bool bInMarais = false;
    float FishingTimer = 0.f;
    float FishWaitTime = 0.f;
};
