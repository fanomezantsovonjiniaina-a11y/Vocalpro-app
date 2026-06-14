#include "MalagasyNPC.h"
#include "Net/UnrealNetwork.h"
#include "../Characters/SurvivalCharacter.h"

AMalagasyNPC::AMalagasyNPC()
{
    bReplicates = true;
}

void AMalagasyNPC::BeginPlay()
{
    Super::BeginPlay();
    BuildDefaultDialogues();
    BuildDefaultTrades();
}

void AMalagasyNPC::BuildDefaultDialogues()
{
    if (!Dialogues.IsEmpty()) return; // Already set in editor

    switch (Role)
    {
    case ENPCRole::Villager:
        Dialogues.Add({ FText::FromString("Tonga soa, mpandeha! Mitandrema amin'ny alina..."),
                        FText::FromString("Bienvenue, voyageur ! Fais attention la nuit...") });
        Dialogues.Add({ FText::FromString("Ny Fosa dia mifoha rehefa maizina. Aza misaraka!"),
                        FText::FromString("La Fosa se réveille quand il fait nuit. Ne vous séparez pas!"),
                        true });
        Dialogues.Add({ FText::FromString("Mitadiava ny Kinina ao amin'ny ala — hanasitrana ny tazomoka izy."),
                        FText::FromString("Cherchez le Quinquina dans la forêt — il guérit le paludisme.") });
        break;

    case ENPCRole::Healer:
        Dialogues.Add({ FText::FromString("Miarahaba! Izaho no mpanasitrana eto. Misy fery ve?"),
                        FText::FromString("Bonjour! Je suis le guérisseur ici. Avez-vous des blessures?") });
        Dialogues.Add({ FText::FromString("Ny Ravintsara dia tsara amin'ny infeksiona. Mitadiava azy!"),
                        FText::FromString("Le Ravintsara est bon contre les infections. Cherchez-en!") });
        Dialogues.Add({ FText::FromString("Poizina menarana? Haingana! Mila Herbe Fanafody ianao!"),
                        FText::FromString("Venin de serpent? Vite! Vous avez besoin d'Herbe Fanafody!"),
                        true });
        break;

    case ENPCRole::Hunter:
        Dialogues.Add({ FText::FromString("Ny Fosa dia matahotra afo. Aza avela maty ny afo amin'ny alina!"),
                        FText::FromString("La Fosa craint le feu. Ne laissez jamais le feu s'éteindre la nuit!"),
                        true });
        Dialogues.Add({ FText::FromString("Ny Krokodily dia miandry ao anaty rano. Tsy hitanao izy!"),
                        FText::FromString("Le crocodile attend dans l'eau. Vous ne le verrez pas!"),
                        true });
        Dialogues.Add({ FText::FromString("Ampiasao ny Lance raha misy Lambo Gidro. Mikapoka haingana izy!"),
                        FText::FromString("Utilisez la Lance contre le Lambo Gidro. Il charge vite!") });
        break;

    case ENPCRole::Elder:
        Dialogues.Add({ FText::FromString("Taloha, ny razantsika dia niaina tao amin'ity ala ity. Izahay nahalala ny tsiambaratelo rehetra."),
                        FText::FromString("Autrefois, nos ancêtres vivaient dans cette forêt. Nous en connaissions tous les secrets.") });
        Dialogues.Add({ FText::FromString("Raha misy Indri mihakakaka — mihazakazaka! Ny Fosa akaiky!"),
                        FText::FromString("Si l'Indri crie — courez! La Fosa est proche!"),
                        true });
        Dialogues.Add({ FText::FromString("Ny Caméléon mena dia milaza loza. Ny manga dia midika hoe miadana ny alina."),
                        FText::FromString("Le caméléon rouge annonce le danger. Le bleu signifie une nuit tranquille.") });
        Dialogues.Add({ FText::FromString("Hiala amin'ity ala ity? Azonao atao — fa tsy maintsy manao pirogue ianao aloha."),
                        FText::FromString("Quitter cette forêt? C'est possible — mais vous devez d'abord construire une pirogue."),
                        false, true }); // bGivesQuest
        break;

    case ENPCRole::Merchant:
        Dialogues.Add({ FText::FromString("Misy zavatra tianao hividy? Mivarotra zavatra tsara aho!"),
                        FText::FromString("Vous voulez acheter quelque chose? J'ai de bonnes choses à vendre!") });
        break;
    }
}

void AMalagasyNPC::BuildDefaultTrades()
{
    if (!TradeOffers.IsEmpty()) return;

    auto MakeItem = [](FName ID, FText Name, EItemCategory Cat, int32 Qty) {
        FInventoryItem I; I.ItemID = ID; I.DisplayName = Name;
        I.Category = Cat; I.Quantity = Qty; return I;
    };

    switch (Role)
    {
    case ENPCRole::Healer:
        // Fanafody an-tsena
        TradeOffers.Add({
            MakeItem("kinina", FText::FromString("Kinina x2"), EItemCategory::Medicine, 2),
            MakeItem("remede_malaria", FText::FromString("Remède Malaria"), EItemCategory::Medicine, 1),
            FText::FromString("Kinina x2 → Remède Malaria")
        });
        TradeOffers.Add({
            MakeItem("hazo", FText::FromString("Bois x5"), EItemCategory::Wood, 5),
            MakeItem("pansement", FText::FromString("Pansement x3"), EItemCategory::Medicine, 3),
            FText::FromString("Bois x5 → Pansement x3")
        });
        TradeOffers.Add({
            MakeItem("viande_grillee", FText::FromString("Viande x2"), EItemCategory::Food, 2),
            MakeItem("antidote", FText::FromString("Antidote"), EItemCategory::Medicine, 1),
            FText::FromString("Viande x2 → Antidote")
        });
        break;

    case ENPCRole::Merchant:
        TradeOffers.Add({
            MakeItem("pierre", FText::FromString("Pierre x5"), EItemCategory::Stone, 5),
            MakeItem("couteau", FText::FromString("Couteau"), EItemCategory::Tool, 1),
            FText::FromString("Pierre x5 → Couteau")
        });
        TradeOffers.Add({
            MakeItem("peau", FText::FromString("Peau x3"), EItemCategory::Misc, 3),
            MakeItem("moustiquaire", FText::FromString("Moustiquaire"), EItemCategory::Misc, 1),
            FText::FromString("Peau x3 → Moustiquaire")
        });
        TradeOffers.Add({
            MakeItem("voankazo", FText::FromString("Voankazo x5"), EItemCategory::Food, 5),
            MakeItem("recipient", FText::FromString("Récipient"), EItemCategory::Misc, 2),
            FText::FromString("Voankazo x5 → Récipient x2")
        });
        break;

    case ENPCRole::Hunter:
        TradeOffers.Add({
            MakeItem("hazo", FText::FromString("Bois x3"), EItemCategory::Wood, 3),
            MakeItem("fleche", FText::FromString("Flèches x10"), EItemCategory::Weapon, 10),
            FText::FromString("Bois x3 → Flèches x10")
        });
        break;

    default: break;
    }
}

void AMalagasyNPC::Server_Interact_Implementation(AActor* Player)
{
    if (!Player) return;

    float Dist = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
    if (Dist > InteractRadius * 100.f) return;

    FNPCDialogue Dialogue = GetRandomDialogue();
    OnDialogue.Broadcast(Dialogue);
}

void AMalagasyNPC::Server_Trade_Implementation(AActor* Player, int32 OfferIndex)
{
    if (!Player || !TradeOffers.IsValidIndex(OfferIndex)) return;

    ASurvivalCharacter* PlayerChar = Cast<ASurvivalCharacter>(Player);
    if (!PlayerChar) return;

    UInventoryComponent* Inv = PlayerChar->FindComponentByClass<UInventoryComponent>();
    if (!Inv) return;

    const FTradeOffer& Offer = TradeOffers[OfferIndex];

    if (!Inv->HasItem(Offer.WantItem.ItemID, Offer.WantItem.Quantity)) return;

    Inv->Server_RemoveItem(Offer.WantItem.ItemID, Offer.WantItem.Quantity);
    Inv->Server_AddItem(Offer.GiveItem);
    OnTrade.Broadcast(Offer.GiveItem);
}

TArray<FTradeOffer> AMalagasyNPC::GetAvailableTrades(UInventoryComponent* PlayerInv) const
{
    TArray<FTradeOffer> Available;
    if (!PlayerInv) return Available;
    for (const FTradeOffer& Offer : TradeOffers)
        if (PlayerInv->HasItem(Offer.WantItem.ItemID, Offer.WantItem.Quantity))
            Available.Add(Offer);
    return Available;
}

FNPCDialogue AMalagasyNPC::GetRandomDialogue() const
{
    if (Dialogues.IsEmpty()) return FNPCDialogue();
    return Dialogues[FMath::RandRange(0, Dialogues.Num() - 1)];
}
