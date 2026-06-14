#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InventoryComponent.h"
#include "CraftingLibrary.generated.h"

// All crafting recipes for Survival Madagascar
UCLASS()
class SURVIVALMADAGASCAR_API UCraftingLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure) static TArray<FCraftingRecipe> GetAllRecipes();
    UFUNCTION(BlueprintPure) static FCraftingRecipe GetRecipe(FName RecipeID);
    UFUNCTION(BlueprintPure) static TArray<FCraftingRecipe> GetRecipesByCategory(EItemCategory Category);

private:
    static TArray<FCraftingRecipe> BuildRecipeDatabase();
    static FInventoryItem MakeItem(FName ID, FText Name, EItemCategory Cat, int32 Qty, float Weight = 0.1f);
};
