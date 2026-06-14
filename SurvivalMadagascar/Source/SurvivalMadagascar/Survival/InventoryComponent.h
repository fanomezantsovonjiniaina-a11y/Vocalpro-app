#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

UENUM(BlueprintType)
enum class EItemCategory : uint8
{
    Food,
    Water,
    Wood,
    Stone,
    Fiber,
    Medicine,
    Tool,
    Weapon,
    Building,
    Misc
};

USTRUCT(BlueprintType)
struct FInventoryItem
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite) FName ItemID;
    UPROPERTY(BlueprintReadWrite) FText DisplayName;
    UPROPERTY(BlueprintReadWrite) EItemCategory Category = EItemCategory::Misc;
    UPROPERTY(BlueprintReadWrite) int32 Quantity = 0;
    UPROPERTY(BlueprintReadWrite) int32 MaxStack = 10;
    UPROPERTY(BlueprintReadWrite) float Weight = 0.1f;
    UPROPERTY(BlueprintReadWrite) UTexture2D* Icon = nullptr;

    bool IsValid() const { return !ItemID.IsNone() && Quantity > 0; }
};

USTRUCT(BlueprintType)
struct FCraftingRecipe
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite) FName RecipeID;
    UPROPERTY(BlueprintReadWrite) FText DisplayName;
    UPROPERTY(BlueprintReadWrite) TArray<FInventoryItem> Ingredients;
    UPROPERTY(BlueprintReadWrite) FInventoryItem Output;
    UPROPERTY(BlueprintReadWrite) float CraftTime = 2.f;
    UPROPERTY(BlueprintReadWrite) bool bRequiresFire = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemAdded, FInventoryItem, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemRemoved, FName, ItemID);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SURVIVALMADAGASCAR_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UInventoryComponent();

    UPROPERTY(EditDefaultsOnly) int32 MaxSlots = 24;
    UPROPERTY(EditDefaultsOnly) float MaxWeight = 30.f;

    UPROPERTY(ReplicatedUsing=OnRep_Items, BlueprintReadOnly) TArray<FInventoryItem> Items;

    UPROPERTY(BlueprintAssignable) FOnInventoryChanged OnInventoryChanged;
    UPROPERTY(BlueprintAssignable) FOnItemAdded OnItemAdded;
    UPROPERTY(BlueprintAssignable) FOnItemRemoved OnItemRemoved;

    UFUNCTION(BlueprintCallable, Server, Reliable) void Server_AddItem(FInventoryItem Item);
    UFUNCTION(BlueprintCallable, Server, Reliable) void Server_RemoveItem(FName ItemID, int32 Amount);
    UFUNCTION(BlueprintCallable, Server, Reliable) void Server_TransferTo(UInventoryComponent* Target, FName ItemID, int32 Amount);
    UFUNCTION(BlueprintCallable, Server, Reliable) void Server_Craft(FCraftingRecipe Recipe);

    UFUNCTION(BlueprintPure) bool HasItem(FName ItemID, int32 Amount = 1) const;
    UFUNCTION(BlueprintPure) int32 GetItemCount(FName ItemID) const;
    UFUNCTION(BlueprintPure) float GetCurrentWeight() const;
    UFUNCTION(BlueprintPure) bool IsFull() const;
    UFUNCTION(BlueprintPure) bool CanCraft(const FCraftingRecipe& Recipe) const;

    UFUNCTION() void OnRep_Items();

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    FInventoryItem* FindItem(FName ItemID);
    bool bIsCrafting = false;
    FTimerHandle CraftTimer;
};
