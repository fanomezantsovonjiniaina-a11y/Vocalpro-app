#include "CraftingLibrary.h"

FInventoryItem UCraftingLibrary::MakeItem(FName ID, FText Name, EItemCategory Cat, int32 Qty, float Weight)
{
    FInventoryItem Item;
    Item.ItemID = ID;
    Item.DisplayName = Name;
    Item.Category = Cat;
    Item.Quantity = Qty;
    Item.Weight = Weight;
    return Item;
}

TArray<FCraftingRecipe> UCraftingLibrary::BuildRecipeDatabase()
{
    TArray<FCraftingRecipe> Recipes;

    // ─── OUTILS ───────────────────────────────────────────
    {
        FCraftingRecipe R;
        R.RecipeID = "craft_hache";
        R.DisplayName = FText::FromString("Hache");
        R.Ingredients = {
            MakeItem("pierre", FText::FromString("Pierre"), EItemCategory::Stone, 2),
            MakeItem("hazo",   FText::FromString("Bois"),   EItemCategory::Wood,  1),
            MakeItem("liane",  FText::FromString("Liane"),  EItemCategory::Fiber, 1)
        };
        R.Output = MakeItem("hache", FText::FromString("Hache"), EItemCategory::Tool, 1, 0.8f);
        R.CraftTime = 5.f;
        Recipes.Add(R);
    }
    {
        FCraftingRecipe R;
        R.RecipeID = "craft_couteau";
        R.DisplayName = FText::FromString("Couteau Pierre");
        R.Ingredients = {
            MakeItem("pierre", FText::FromString("Pierre"), EItemCategory::Stone, 1),
            MakeItem("liane",  FText::FromString("Liane"),  EItemCategory::Fiber, 1)
        };
        R.Output = MakeItem("couteau", FText::FromString("Couteau"), EItemCategory::Tool, 1, 0.3f);
        R.CraftTime = 3.f;
        Recipes.Add(R);
    }
    {
        FCraftingRecipe R;
        R.RecipeID = "craft_lance";
        R.DisplayName = FText::FromString("Lance");
        R.Ingredients = {
            MakeItem("hazo",   FText::FromString("Bois"),   EItemCategory::Wood,  2),
            MakeItem("pierre", FText::FromString("Pierre"), EItemCategory::Stone, 1)
        };
        R.Output = MakeItem("lance", FText::FromString("Lance"), EItemCategory::Weapon, 1, 1.f);
        R.CraftTime = 6.f;
        Recipes.Add(R);
    }
    {
        FCraftingRecipe R;
        R.RecipeID = "craft_arc";
        R.DisplayName = FText::FromString("Arc");
        R.Ingredients = {
            MakeItem("hazo",  FText::FromString("Bois courbe"), EItemCategory::Wood,  1),
            MakeItem("liane", FText::FromString("Liane"),       EItemCategory::Fiber, 2)
        };
        R.Output = MakeItem("arc", FText::FromString("Arc"), EItemCategory::Weapon, 1, 0.5f);
        R.CraftTime = 8.f;
        Recipes.Add(R);
    }
    {
        FCraftingRecipe R;
        R.RecipeID = "craft_fleche";
        R.DisplayName = FText::FromString("Flèches (x5)");
        R.Ingredients = {
            MakeItem("hazo",   FText::FromString("Bois"),   EItemCategory::Wood,  3),
            MakeItem("plume",  FText::FromString("Plume"),  EItemCategory::Misc,  3),
            MakeItem("pierre", FText::FromString("Pierre"), EItemCategory::Stone, 2)
        };
        R.Output = MakeItem("fleche", FText::FromString("Flèche"), EItemCategory::Weapon, 5, 0.05f);
        R.CraftTime = 4.f;
        Recipes.Add(R);
    }

    // ─── NOURRITURE ───────────────────────────────────────
    {
        FCraftingRecipe R;
        R.RecipeID = "craft_viande_grillee";
        R.DisplayName = FText::FromString("Viande Grillée");
        R.Ingredients = {
            MakeItem("viande_crue", FText::FromString("Viande crue"), EItemCategory::Food, 1)
        };
        R.Output = MakeItem("viande_grillee", FText::FromString("Viande Grillée"), EItemCategory::Food, 1, 0.3f);
        R.CraftTime = 10.f;
        R.bRequiresFire = true;
        Recipes.Add(R);
    }
    {
        FCraftingRecipe R;
        R.RecipeID = "craft_eau_purifiee";
        R.DisplayName = FText::FromString("Eau Purifiée");
        R.Ingredients = {
            MakeItem("rano_maloto", FText::FromString("Eau sale"), EItemCategory::Water, 1),
            MakeItem("recipient",   FText::FromString("Récipient"), EItemCategory::Misc, 1)
        };
        R.Output = MakeItem("rano_madio", FText::FromString("Eau Purifiée"), EItemCategory::Water, 1, 0.5f);
        R.CraftTime = 8.f;
        R.bRequiresFire = true;
        Recipes.Add(R);
    }
    {
        FCraftingRecipe R;
        R.RecipeID = "craft_soupe";
        R.DisplayName = FText::FromString("Soupe d'Herbes");
        R.Ingredients = {
            MakeItem("rano_madio", FText::FromString("Eau Purifiée"), EItemCategory::Water, 1),
            MakeItem("herbe",      FText::FromString("Herbe"),        EItemCategory::Medicine, 2),
            MakeItem("recipient",  FText::FromString("Récipient"),    EItemCategory::Misc, 1)
        };
        R.Output = MakeItem("soupe", FText::FromString("Soupe Malagasy"), EItemCategory::Food, 1, 0.4f);
        R.CraftTime = 15.f;
        R.bRequiresFire = true;
        Recipes.Add(R);
    }

    // ─── MÉDECINE ─────────────────────────────────────────
    {
        FCraftingRecipe R;
        R.RecipeID = "craft_pansement";
        R.DisplayName = FText::FromString("Pansement");
        R.Ingredients = {
            MakeItem("ravina",  FText::FromString("Feuilles"), EItemCategory::Misc, 2),
            MakeItem("fibres",  FText::FromString("Fibres"),   EItemCategory::Fiber, 1)
        };
        R.Output = MakeItem("pansement", FText::FromString("Pansement"), EItemCategory::Medicine, 1, 0.05f);
        R.CraftTime = 3.f;
        Recipes.Add(R);
    }
    {
        FCraftingRecipe R;
        R.RecipeID = "craft_antidote";
        R.DisplayName = FText::FromString("Antidote Basique");
        R.Ingredients = {
            MakeItem("herbe_fanafody", FText::FromString("Herbe Fanafody"), EItemCategory::Medicine, 3),
            MakeItem("rano_madio",     FText::FromString("Eau Purifiée"),   EItemCategory::Water, 1)
        };
        R.Output = MakeItem("antidote", FText::FromString("Antidote"), EItemCategory::Medicine, 1, 0.1f);
        R.CraftTime = 5.f;
        Recipes.Add(R);
    }
    {
        FCraftingRecipe R;
        R.RecipeID = "craft_anti_malaria";
        R.DisplayName = FText::FromString("Remède Anti-Malaria");
        R.Ingredients = {
            MakeItem("kinina",     FText::FromString("Kinina (Quinquina)"), EItemCategory::Medicine, 2),
            MakeItem("rano_madio", FText::FromString("Eau chaude"),         EItemCategory::Water, 1)
        };
        R.Output = MakeItem("remede_malaria", FText::FromString("Remède Malaria"), EItemCategory::Medicine, 1, 0.1f);
        R.CraftTime = 8.f;
        R.bRequiresFire = true;
        Recipes.Add(R);
    }

    // ─── PROTECTION ───────────────────────────────────────
    {
        FCraftingRecipe R;
        R.RecipeID = "craft_moustiquaire";
        R.DisplayName = FText::FromString("Moustiquaire");
        R.Ingredients = {
            MakeItem("fibres", FText::FromString("Fibres"), EItemCategory::Fiber, 10),
            MakeItem("liane",  FText::FromString("Liane"),  EItemCategory::Fiber, 5)
        };
        R.Output = MakeItem("moustiquaire", FText::FromString("Moustiquaire"), EItemCategory::Misc, 1, 0.5f);
        R.CraftTime = 12.f;
        Recipes.Add(R);
    }
    {
        FCraftingRecipe R;
        R.RecipeID = "craft_armure";
        R.DisplayName = FText::FromString("Armure Légère");
        R.Ingredients = {
            MakeItem("peau",  FText::FromString("Peau"),  EItemCategory::Misc, 5),
            MakeItem("liane", FText::FromString("Liane"), EItemCategory::Fiber, 3)
        };
        R.Output = MakeItem("armure_legere", FText::FromString("Armure Légère"), EItemCategory::Misc, 1, 2.f);
        R.CraftTime = 20.f;
        Recipes.Add(R);
    }

    // ─── DÉFENSES ─────────────────────────────────────────
    {
        FCraftingRecipe R;
        R.RecipeID = "craft_piege";
        R.DisplayName = FText::FromString("Piège à Fosa");
        R.Ingredients = {
            MakeItem("hazo",  FText::FromString("Bois"),  EItemCategory::Wood,  3),
            MakeItem("liane", FText::FromString("Liane"), EItemCategory::Fiber, 2)
        };
        R.Output = MakeItem("piege", FText::FromString("Piège"), EItemCategory::Building, 1, 1.5f);
        R.CraftTime = 10.f;
        Recipes.Add(R);
    }
    {
        FCraftingRecipe R;
        R.RecipeID = "craft_torche";
        R.DisplayName = FText::FromString("Torche");
        R.Ingredients = {
            MakeItem("hazo",  FText::FromString("Bois"),  EItemCategory::Wood, 1),
            MakeItem("resine",FText::FromString("Résine"), EItemCategory::Misc, 1)
        };
        R.Output = MakeItem("torche", FText::FromString("Torche"), EItemCategory::Building, 1, 0.3f);
        R.CraftTime = 3.f;
        Recipes.Add(R);
    }

    return Recipes;
}

TArray<FCraftingRecipe> UCraftingLibrary::GetAllRecipes()
{
    static TArray<FCraftingRecipe> DB = BuildRecipeDatabase();
    return DB;
}

FCraftingRecipe UCraftingLibrary::GetRecipe(FName RecipeID)
{
    for (const FCraftingRecipe& R : GetAllRecipes())
        if (R.RecipeID == RecipeID) return R;
    return FCraftingRecipe();
}

TArray<FCraftingRecipe> UCraftingLibrary::GetRecipesByCategory(EItemCategory Category)
{
    TArray<FCraftingRecipe> Result;
    for (const FCraftingRecipe& R : GetAllRecipes())
        if (R.Output.Category == Category) Result.Add(R);
    return Result;
}
