#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

UInventoryComponent::UInventoryComponent()
{
    SetIsReplicatedByDefault(true);
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UInventoryComponent, Items);
}

void UInventoryComponent::OnRep_Items()
{
    OnInventoryChanged.Broadcast();
}

void UInventoryComponent::Server_AddItem_Implementation(FInventoryItem Item)
{
    if (IsFull() || GetCurrentWeight() + Item.Weight * Item.Quantity > MaxWeight) return;

    // Try to stack with existing item
    FInventoryItem* Existing = FindItem(Item.ItemID);
    if (Existing && Existing->Quantity < Existing->MaxStack)
    {
        int32 CanAdd = FMath::Min(Item.Quantity, Existing->MaxStack - Existing->Quantity);
        Existing->Quantity += CanAdd;
        Item.Quantity -= CanAdd;
    }

    // Add remaining as new slot
    if (Item.Quantity > 0 && Items.Num() < MaxSlots)
    {
        Items.Add(Item);
    }

    OnItemAdded.Broadcast(Item);
    OnInventoryChanged.Broadcast();
}

void UInventoryComponent::Server_RemoveItem_Implementation(FName ItemID, int32 Amount)
{
    for (int32 i = Items.Num() - 1; i >= 0; i--)
    {
        if (Items[i].ItemID != ItemID) continue;

        if (Items[i].Quantity <= Amount)
        {
            Amount -= Items[i].Quantity;
            Items.RemoveAt(i);
        }
        else
        {
            Items[i].Quantity -= Amount;
            Amount = 0;
        }

        if (Amount <= 0) break;
    }

    OnItemRemoved.Broadcast(ItemID);
    OnInventoryChanged.Broadcast();
}

void UInventoryComponent::Server_TransferTo_Implementation(UInventoryComponent* Target, FName ItemID, int32 Amount)
{
    if (!Target) return;

    FInventoryItem* Source = FindItem(ItemID);
    if (!Source || Source->Quantity < Amount) return;

    FInventoryItem Transfer = *Source;
    Transfer.Quantity = Amount;

    Server_RemoveItem(ItemID, Amount);
    Target->Server_AddItem(Transfer);
}

void UInventoryComponent::Server_Craft_Implementation(FCraftingRecipe Recipe)
{
    if (bIsCrafting || !CanCraft(Recipe)) return;

    bIsCrafting = true;

    // Remove ingredients
    for (const FInventoryItem& Ingredient : Recipe.Ingredients)
        Server_RemoveItem(Ingredient.ItemID, Ingredient.Quantity);

    // Add output after craft time
    GetWorld()->GetTimerManager().SetTimer(CraftTimer, [this, Recipe]()
    {
        Server_AddItem(Recipe.Output);
        bIsCrafting = false;
    }, Recipe.CraftTime, false);
}

bool UInventoryComponent::HasItem(FName ItemID, int32 Amount) const
{
    return GetItemCount(ItemID) >= Amount;
}

int32 UInventoryComponent::GetItemCount(FName ItemID) const
{
    int32 Total = 0;
    for (const FInventoryItem& Item : Items)
        if (Item.ItemID == ItemID) Total += Item.Quantity;
    return Total;
}

float UInventoryComponent::GetCurrentWeight() const
{
    float Total = 0.f;
    for (const FInventoryItem& Item : Items)
        Total += Item.Weight * Item.Quantity;
    return Total;
}

bool UInventoryComponent::IsFull() const
{
    return Items.Num() >= MaxSlots;
}

bool UInventoryComponent::CanCraft(const FCraftingRecipe& Recipe) const
{
    for (const FInventoryItem& Ingredient : Recipe.Ingredients)
        if (!HasItem(Ingredient.ItemID, Ingredient.Quantity)) return false;
    return true;
}

FInventoryItem* UInventoryComponent::FindItem(FName ItemID)
{
    for (FInventoryItem& Item : Items)
        if (Item.ItemID == ItemID) return &Item;
    return nullptr;
}
