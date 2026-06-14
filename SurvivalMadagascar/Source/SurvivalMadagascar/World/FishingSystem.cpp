#include "FishingSystem.h"
#include "Net/UnrealNetwork.h"
#include "../Characters/SurvivalCharacter.h"

AFishingSystem::AFishingSystem() { PrimaryActorTick.bCanEverTick = true; bReplicates = true; }

void AFishingSystem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AFishingSystem, bIsFishing);
    DOREPLIFETIME(AFishingSystem, FishingProgress);
    DOREPLIFETIME(AFishingSystem, PirogueState);
    DOREPLIFETIME(AFishingSystem, PirogueHP);
}

void AFishingSystem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if(!HasAuthority() || !bIsFishing || !FishingPlayer) return;

    FishingTimer += DeltaTime;
    FishingProgress = FMath::Min(1.f, FishingTimer / FishWaitTime);

    if(FishingTimer >= FishWaitTime)
    {
        GiveFishToPlayer();
        bIsFishing = false;
        FishingTimer = 0.f;
        FishingProgress = 0.f;
    }
}

void AFishingSystem::Server_StartFishing_Implementation(AActor* Player, bool bInMaraisZone)
{
    if(bIsFishing) return;
    bIsFishing = true;
    FishingPlayer = Player;
    bInMarais = bInMaraisZone;
    FishWaitTime = FMath::RandRange(5.f, 20.f);
    FishingTimer = 0.f;
}

void AFishingSystem::Server_StopFishing_Implementation()
{
    bIsFishing = false;
    FishingTimer = 0.f;
    FishingProgress = 0.f;
    FishingPlayer = nullptr;
}

EFishType AFishingSystem::RollFish(bool bMarais, bool bAtSea) const
{
    if(bAtSea)
    {
        int32 R = FMath::RandRange(0,100);
        if(R < 50) return EFishType::PoissonMer;
        if(R < 80) return EFishType::Crevette;
        return EFishType::Nothing;
    }
    if(bMarais)
    {
        int32 R = FMath::RandRange(0,100);
        if(R < 30) return EFishType::Anguille; // Dangerous!
        if(R < 70) return EFishType::Tilapia;
        return EFishType::Nothing;
    }
    // River
    int32 R = FMath::RandRange(0,100);
    if(R < 60) return EFishType::Tilapia;
    if(R < 80) return EFishType::Crevette;
    return EFishType::Nothing;
}

void AFishingSystem::GiveFishToPlayer()
{
    bool bAtSea = PirogueState == EPirogueState::AtSea;
    EFishType Fish = RollFish(bInMarais, bAtSea);
    OnFishCaught.Broadcast(Fish);

    if(Fish == EFishType::Nothing) return;

    // Electric shock from anguille!
    if(Fish == EFishType::Anguille)
    {
        OnElectricShock.Broadcast();
        if(ASurvivalCharacter* PC = Cast<ASurvivalCharacter>(FishingPlayer))
            PC->GetSurvivalComponent()->Server_ApplyDamage(15.f);
    }

    // Add fish to player inventory
    if(ASurvivalCharacter* PC = Cast<ASurvivalCharacter>(FishingPlayer))
    {
        UInventoryComponent* Inv = PC->FindComponentByClass<UInventoryComponent>();
        if(!Inv) return;

        FInventoryItem FishItem;
        FishItem.Category = EItemCategory::Food;
        FishItem.Quantity = 1;
        FishItem.Weight = 0.3f;

        switch(Fish)
        {
        case EFishType::Tilapia:
            FishItem.ItemID = "tilapia_cru";
            FishItem.DisplayName = FText::FromString("Tilapia cru");
            break;
        case EFishType::Crevette:
            FishItem.ItemID = "crevette";
            FishItem.DisplayName = FText::FromString("Crevettes");
            FishItem.Quantity = 3;
            break;
        case EFishType::Anguille:
            FishItem.ItemID = "anguille_crue";
            FishItem.DisplayName = FText::FromString("Anguille (attention!)");
            break;
        case EFishType::PoissonMer:
            FishItem.ItemID = "poisson_mer";
            FishItem.DisplayName = FText::FromString("Poisson de mer");
            FishItem.Quantity = 2;
            break;
        default: return;
        }
        Inv->Server_AddItem(FishItem);
    }
}

void AFishingSystem::Server_BuildPirogue_Implementation(UInventoryComponent* Inv)
{
    if(!Inv || PirogueState != EPirogueState::NotBuilt) return;
    if(!Inv->HasItem("hazo", 20) || !Inv->HasItem("liane", 10) || !Inv->HasItem("pierre", 5)) return;

    Inv->Server_RemoveItem("hazo",  20);
    Inv->Server_RemoveItem("liane", 10);
    Inv->Server_RemoveItem("pierre", 5);

    PirogueState = EPirogueState::Built;
    PirogueHP = 100.f;
}

void AFishingSystem::Server_RepairPirogue_Implementation(UInventoryComponent* Inv)
{
    if(!Inv || PirogueState != EPirogueState::Damaged) return;
    if(!Inv->HasItem("hazo", 5)) return;
    Inv->Server_RemoveItem("hazo", 5);
    PirogueHP = 100.f;
    PirogueState = EPirogueState::Built;
}
