#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Survival/InventoryComponent.h"
#include "BuildingSystem.generated.h"

UENUM(BlueprintType)
enum class EBuildingType : uint8
{
    Campfire,       // Feu de camp — repousse Fosa
    Shelter_L1,     // Abri urgence (feuilles)
    Shelter_L2,     // Case Malagasy (bois)
    Shelter_L3,     // Compound fortifié
    Barricade,      // Mur bois
    StoneWall,      // Mur pierre
    Torch,          // Torche
    Trap,           // Piège Fosa
    Net,            // Filet ralentisseur
    Alarm,          // Alarme bambou
    StorageChest,   // Coffre commun
    WatchTower      // Tour de guet
};

USTRUCT(BlueprintType)
struct FBuildingRecipe
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite) EBuildingType Type;
    UPROPERTY(BlueprintReadWrite) TArray<FInventoryItem> Cost;
    UPROPERTY(BlueprintReadWrite) float BuildTime = 5.f;
    UPROPERTY(BlueprintReadWrite) int32 HP = 200;
    UPROPERTY(BlueprintReadWrite) FText DisplayName;
};

UCLASS()
class SURVIVALMADAGASCAR_API ABuildingActor : public AActor
{
    GENERATED_BODY()

public:
    ABuildingActor();

    UPROPERTY(Replicated, BlueprintReadOnly) EBuildingType BuildingType;
    UPROPERTY(Replicated, BlueprintReadOnly) int32 CurrentHP;
    UPROPERTY(Replicated, BlueprintReadOnly) int32 MaxHP;
    UPROPERTY(Replicated, BlueprintReadOnly) bool bIsActive = true;

    UFUNCTION(BlueprintCallable) void TakeDamage_Building(int32 Amount);
    UFUNCTION(BlueprintCallable) void Repair(int32 Amount);
    UFUNCTION(BlueprintPure) float GetHealthPercent() const;
    UFUNCTION(BlueprintPure) bool IsCampfire() const { return BuildingType == EBuildingType::Campfire; }
    UFUNCTION(BlueprintCallable) void ExtinguishFire(); // Called when raining

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};

UCLASS()
class SURVIVALMADAGASCAR_API ABuildingSystem : public AActor
{
    GENERATED_BODY()

public:
    ABuildingSystem();

    UFUNCTION(BlueprintCallable, Server, Reliable)
    void Server_PlaceBuilding(EBuildingType Type, FVector Location, FRotator Rotation, UInventoryComponent* PlayerInventory);

    UFUNCTION(BlueprintCallable, Server, Reliable)
    void Server_RemoveBuilding(ABuildingActor* Building, UInventoryComponent* PlayerInventory);

    UFUNCTION(BlueprintPure) static FBuildingRecipe GetRecipe(EBuildingType Type);
    UFUNCTION(BlueprintPure) static bool CanAfford(EBuildingType Type, UInventoryComponent* Inventory);

    UFUNCTION(BlueprintCallable) void OnRainStarted(); // Extinguish campfires
    UFUNCTION(BlueprintCallable) void OnRainStopped();

    UPROPERTY(Replicated) TArray<ABuildingActor*> PlacedBuildings;

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
