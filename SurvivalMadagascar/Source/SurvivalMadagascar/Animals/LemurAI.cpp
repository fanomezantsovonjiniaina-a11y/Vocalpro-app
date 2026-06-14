#include "LemurAI.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Characters/SurvivalCharacter.h"
#include "../Animals/FosaAI.h"
#include "../Survival/InventoryComponent.h"

ALemurAI::ALemurAI()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
}

void ALemurAI::BeginPlay()
{
    Super::BeginPlay();
}

void ALemurAI::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (!HasAuthority()) return;

    AlertCooldown = FMath::Max(0.f, AlertCooldown - DeltaTime);
    FruitSearchCooldown = FMath::Max(0.f, FruitSearchCooldown - DeltaTime);

    UpdateAI(DeltaTime);
}

void ALemurAI::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ALemurAI, CurrentState);
    DOREPLIFETIME(ALemurAI, Owner);
    DOREPLIFETIME(ALemurAI, TamingDaysLeft);
}

void ALemurAI::UpdateAI(float DeltaTime)
{
    // Indri: always checking for Fosa regardless of state
    if (LemurType == ELemurType::Indri)
        CheckForFosa();

    switch (CurrentState)
    {
    case ELemurState::Wild:
    {
        // Manalavitra player raha akaiky loatra
        TArray<AActor*> Players;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASurvivalCharacter::StaticClass(), Players);
        for (AActor* P : Players)
        {
            float Dist = FVector::Dist(GetActorLocation(), P->GetActorLocation());
            if (Dist < 500.f) // 5m — fuit!
            {
                FVector Away = (GetActorLocation() - P->GetActorLocation()).GetSafeNormal();
                AddMovementInput(Away);
            }
        }
        break;
    }

    case ELemurState::Curious:
        // Miandry — manantona moramora ny player nandefa sakafo
        if (FoodGiver)
        {
            float Dist = FVector::Dist(GetActorLocation(), FoodGiver->GetActorLocation());
            if (Dist > 300.f)
            {
                FVector Dir = (FoodGiver->GetActorLocation() - GetActorLocation()).GetSafeNormal();
                AddMovementInput(Dir * 0.5f);
            }
        }
        break;

    case ELemurState::Tamed:
    {
        if (!Owner) { CurrentState = ELemurState::Wild; break; }

        // Manaraka ny owner amin'ny 8m
        float Dist = FVector::Dist(GetActorLocation(), Owner->GetActorLocation());
        if (Dist > 800.f)
        {
            FVector Dir = (Owner->GetActorLocation() - GetActorLocation()).GetSafeNormal();
            AddMovementInput(Dir);
        }

        // Catta: mitady voankazo ho an'ny owner
        if (LemurType == ELemurType::Catta && FruitSearchCooldown <= 0.f)
        {
            FindFruitForOwner();
            FruitSearchCooldown = 60.f; // Isaky ny 1 minitra
        }

        // Aye-aye: bonus fiarovana malaria (handled in GameMode)
        break;
    }

    case ELemurState::Alerting:
    {
        // Indri: mihakakaka mafy — warning ho an'ny players rehetra
        // Sound played in Blueprint
        if (AlertCooldown <= 0.f)
        {
            CurrentState = ELemurState::Fleeing;
        }
        break;
    }

    case ELemurState::Fleeing:
    {
        // Mihazakazaka manalavitra ny Fosa
        TArray<AActor*> Fosas;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFosaAI::StaticClass(), Fosas);
        if (!Fosas.IsEmpty())
        {
            FVector Away = (GetActorLocation() - Fosas[0]->GetActorLocation()).GetSafeNormal();
            GetCharacterMovement()->MaxWalkSpeed = 600.f;
            AddMovementInput(Away);
        }
        else
        {
            CurrentState = IsTamed() ? ELemurState::Tamed : ELemurState::Wild;
        }
        break;
    }

    default: break;
    }
}

void ALemurAI::OfferFood(AActor* Player, FName FoodItemID)
{
    // Voankazo ihany no ekena
    static TArray<FName> AcceptedFoods = {
        "voankazo", "akondro", "voatabia", "papaye", "anana"
    };

    if (!AcceptedFoods.Contains(FoodItemID)) return;

    FoodGiver = Player;

    if (CurrentState == ELemurState::Wild)
    {
        CurrentState = ELemurState::Curious;
    }
    else if (CurrentState == ELemurState::Curious)
    {
        // Already curious — taming progresses on new day
    }
}

void ALemurAI::OnNewDay()
{
    if (CurrentState != ELemurState::Curious && CurrentState != ELemurState::Taming) return;

    CurrentState = ELemurState::Taming;
    TamingDaysLeft = FMath::Max(0, TamingDaysLeft - 1);

    if (TamingDaysLeft <= 0)
    {
        CurrentState = ELemurState::Tamed;
        Owner = FoodGiver;
        OnTamed.Broadcast();
    }
}

void ALemurAI::CheckForFosa()
{
    if (AlertCooldown > 0.f) return;

    if (IsFosaNearby(10000.f)) // 100m
    {
        CurrentState = ELemurState::Alerting;
        AlertCooldown = 10.f;
        OnFosaDetected.Broadcast();
        // Blueprint: play Indri scream sound
    }
}

bool ALemurAI::IsFosaNearby(float Radius) const
{
    TArray<AActor*> Fosas;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFosaAI::StaticClass(), Fosas);
    for (AActor* F : Fosas)
    {
        AFosaAI* Fosa = Cast<AFosaAI>(F);
        if (Fosa && Fosa->IsActive())
        {
            if (FVector::Dist(GetActorLocation(), F->GetActorLocation()) < Radius)
                return true;
        }
    }
    return false;
}

void ALemurAI::FindFruitForOwner()
{
    if (!Owner) return;

    // Find nearest fruit actor and bring it to owner
    TArray<AActor*> Fruits;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Fruit"), Fruits);

    AActor* NearestFruit = nullptr;
    float NearestDist = 3000.f; // 30m max search

    for (AActor* F : Fruits)
    {
        float Dist = FVector::Dist(GetActorLocation(), F->GetActorLocation());
        if (Dist < NearestDist) { NearestDist = Dist; NearestFruit = F; }
    }

    if (NearestFruit)
    {
        // Give fruit to owner inventory
        if (ASurvivalCharacter* PlayerChar = Cast<ASurvivalCharacter>(Owner))
        {
            FInventoryItem Fruit;
            Fruit.ItemID = "voankazo";
            Fruit.DisplayName = FText::FromString("Voankazo (Lémur)");
            Fruit.Category = EItemCategory::Food;
            Fruit.Quantity = 1;

            UInventoryComponent* Inv = PlayerChar->FindComponentByClass<UInventoryComponent>();
            if (Inv) Inv->Server_AddItem(Fruit);

            NearestFruit->Destroy();
        }
    }
}
