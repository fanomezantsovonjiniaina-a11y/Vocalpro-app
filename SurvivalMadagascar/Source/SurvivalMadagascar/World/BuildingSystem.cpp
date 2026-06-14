#include "BuildingSystem.h"
#include "Net/UnrealNetwork.h"

// ─── ABuildingActor ───────────────────────────────────────────────────────────

ABuildingActor::ABuildingActor()
{
    bReplicates = true;
    Tags.Add(FName("Building"));
}

void ABuildingActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ABuildingActor, BuildingType);
    DOREPLIFETIME(ABuildingActor, CurrentHP);
    DOREPLIFETIME(ABuildingActor, MaxHP);
    DOREPLIFETIME(ABuildingActor, bIsActive);
}

void ABuildingActor::TakeDamage_Building(int32 Amount)
{
    CurrentHP = FMath::Max(0, CurrentHP - Amount);
    if (CurrentHP <= 0)
    {
        bIsActive = false;
        SetLifeSpan(2.f);
    }
}

void ABuildingActor::Repair(int32 Amount)
{
    CurrentHP = FMath::Min(MaxHP, CurrentHP + Amount);
    bIsActive = true;
}

float ABuildingActor::GetHealthPercent() const
{
    return MaxHP > 0 ? (float)CurrentHP / MaxHP : 0.f;
}

void ABuildingActor::ExtinguishFire()
{
    if (IsCampfire())
    {
        bIsActive = false;
        Tags.Remove(FName("Campfire"));
        // Blueprint: stop fire particle, play sizzle sound
    }
}

// ─── ABuildingSystem ──────────────────────────────────────────────────────────

ABuildingSystem::ABuildingSystem()
{
    bReplicates = true;
}

void ABuildingSystem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ABuildingSystem, PlacedBuildings);
}

FBuildingRecipe ABuildingSystem::GetRecipe(EBuildingType Type)
{
    FBuildingRecipe R;
    R.Type = Type;

    auto MakeItem = [](FName ID, FText Name, EItemCategory Cat, int32 Qty) {
        FInventoryItem I; I.ItemID = ID; I.DisplayName = Name;
        I.Category = Cat; I.Quantity = Qty; return I;
    };

    switch (Type)
    {
    case EBuildingType::Campfire:
        R.DisplayName = FText::FromString("Afo (Campfire)");
        R.Cost = { MakeItem("hazo", FText::FromString("Bois"), EItemCategory::Wood, 5) };
        R.BuildTime = 3.f; R.HP = 100;
        R.Cost[0].ItemID = "hazo";
        Tags.Add(FName("Campfire")); // Fosa matahotra
        break;

    case EBuildingType::Shelter_L1:
        R.DisplayName = FText::FromString("Abri Urgence");
        R.Cost = {
            MakeItem("ravina", FText::FromString("Feuilles Ravinala"), EItemCategory::Misc, 8),
            MakeItem("hazo",   FText::FromString("Bambou"),            EItemCategory::Wood, 4)
        };
        R.BuildTime = 10.f; R.HP = 100;
        break;

    case EBuildingType::Shelter_L2:
        R.DisplayName = FText::FromString("Case Malagasy");
        R.Cost = {
            MakeItem("hazo",  FText::FromString("Bois"),    EItemCategory::Wood,  12),
            MakeItem("liane", FText::FromString("Liane"),   EItemCategory::Fiber, 8),
            MakeItem("ravina",FText::FromString("Feuilles"),EItemCategory::Misc,  10)
        };
        R.BuildTime = 30.f; R.HP = 300;
        break;

    case EBuildingType::Shelter_L3:
        R.DisplayName = FText::FromString("Compound Fortifié");
        R.Cost = {
            MakeItem("hazo",   FText::FromString("Bois solide"), EItemCategory::Wood,  20),
            MakeItem("pierre", FText::FromString("Pierre"),      EItemCategory::Stone, 15),
            MakeItem("liane",  FText::FromString("Liane"),       EItemCategory::Fiber, 10)
        };
        R.BuildTime = 120.f; R.HP = 600;
        break;

    case EBuildingType::Barricade:
        R.DisplayName = FText::FromString("Barricade Bois");
        R.Cost = { MakeItem("hazo", FText::FromString("Bois"), EItemCategory::Wood, 8) };
        R.BuildTime = 8.f; R.HP = 200;
        break;

    case EBuildingType::StoneWall:
        R.DisplayName = FText::FromString("Mur Pierre");
        R.Cost = { MakeItem("pierre", FText::FromString("Pierre"), EItemCategory::Stone, 12) };
        R.BuildTime = 15.f; R.HP = 500;
        break;

    case EBuildingType::Torch:
        R.DisplayName = FText::FromString("Torche");
        R.Cost = {
            MakeItem("hazo",   FText::FromString("Bois"),   EItemCategory::Wood, 1),
            MakeItem("resine", FText::FromString("Résine"), EItemCategory::Misc, 1)
        };
        R.BuildTime = 2.f; R.HP = 50;
        break;

    case EBuildingType::Trap:
        R.DisplayName = FText::FromString("Piège Fosa");
        R.Cost = {
            MakeItem("hazo",  FText::FromString("Bois"),  EItemCategory::Wood,  3),
            MakeItem("liane", FText::FromString("Liane"), EItemCategory::Fiber, 2)
        };
        R.BuildTime = 10.f; R.HP = 80;
        break;

    case EBuildingType::StorageChest:
        R.DisplayName = FText::FromString("Coffre Commun");
        R.Cost = { MakeItem("hazo", FText::FromString("Bois"), EItemCategory::Wood, 10) };
        R.BuildTime = 15.f; R.HP = 300;
        break;

    case EBuildingType::WatchTower:
        R.DisplayName = FText::FromString("Tour de Guet");
        R.Cost = {
            MakeItem("hazo",   FText::FromString("Bois"),   EItemCategory::Wood,  20),
            MakeItem("liane",  FText::FromString("Liane"),  EItemCategory::Fiber, 8),
            MakeItem("pierre", FText::FromString("Pierre"), EItemCategory::Stone, 5)
        };
        R.BuildTime = 60.f; R.HP = 400;
        break;

    default: break;
    }
    return R;
}

bool ABuildingSystem::CanAfford(EBuildingType Type, UInventoryComponent* Inventory)
{
    if (!Inventory) return false;
    FBuildingRecipe Recipe = GetRecipe(Type);
    for (const FInventoryItem& Cost : Recipe.Cost)
        if (!Inventory->HasItem(Cost.ItemID, Cost.Quantity)) return false;
    return true;
}

void ABuildingSystem::Server_PlaceBuilding_Implementation(EBuildingType Type, FVector Location, FRotator Rotation, UInventoryComponent* PlayerInventory)
{
    if (!CanAfford(Type, PlayerInventory)) return;

    FBuildingRecipe Recipe = GetRecipe(Type);

    // Deduct materials
    for (const FInventoryItem& Cost : Recipe.Cost)
        PlayerInventory->Server_RemoveItem(Cost.ItemID, Cost.Quantity);

    // Spawn building
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    ABuildingActor* Building = GetWorld()->SpawnActor<ABuildingActor>(Location, Rotation, Params);
    if (Building)
    {
        Building->BuildingType = Type;
        Building->MaxHP = Recipe.HP;
        Building->CurrentHP = Recipe.HP;

        if (Type == EBuildingType::Campfire)
            Building->Tags.Add(FName("Campfire"));

        PlacedBuildings.Add(Building);
    }
}

void ABuildingSystem::Server_RemoveBuilding_Implementation(ABuildingActor* Building, UInventoryComponent* PlayerInventory)
{
    if (!Building) return;

    // Refund 50% materials
    FBuildingRecipe Recipe = GetRecipe(Building->BuildingType);
    for (FInventoryItem Cost : Recipe.Cost)
    {
        Cost.Quantity = FMath::Max(1, Cost.Quantity / 2);
        PlayerInventory->Server_AddItem(Cost);
    }

    PlacedBuildings.Remove(Building);
    Building->Destroy();
}

void ABuildingSystem::OnRainStarted()
{
    for (ABuildingActor* B : PlacedBuildings)
        if (B && B->IsCampfire())
            B->ExtinguishFire();
}

void ABuildingSystem::OnRainStopped()
{
    // Players must manually relight fires
}
