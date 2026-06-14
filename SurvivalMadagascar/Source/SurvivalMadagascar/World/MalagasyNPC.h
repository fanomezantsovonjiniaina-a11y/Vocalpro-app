#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Survival/InventoryComponent.h"
#include "MalagasyNPC.generated.h"

UENUM(BlueprintType)
enum class ENPCRole : uint8
{
    Villager,   // Olona tsotra, manolotra vaovao
    Healer,     // Mpanasitrana — mivarotra fanafody
    Hunter,     // Mpihaza — mampianatra fiadiana
    Elder,      // Anti — mahafantatra ny tantara
    Merchant    // Mpivarotra — exchange items
};

USTRUCT(BlueprintType)
struct FTradeOffer
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite) FInventoryItem WantItem;   // Ilay izy mila
    UPROPERTY(BlueprintReadWrite) FInventoryItem GiveItem;   // Ilay izy manolotra
    UPROPERTY(BlueprintReadWrite) FText Description;
};

USTRUCT(BlueprintType)
struct FNPCDialogue
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite) FText MalagasyText;
    UPROPERTY(BlueprintReadWrite) FText FrenchTranslation;
    UPROPERTY(BlueprintReadWrite) bool bIsWarning = false;
    UPROPERTY(BlueprintReadWrite) bool bGivesQuest = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueTriggered, FNPCDialogue, Dialogue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTradeCompleted, FInventoryItem, ReceivedItem);

UCLASS()
class SURVIVALMADAGASCAR_API AMalagasyNPC : public ACharacter
{
    GENERATED_BODY()

public:
    AMalagasyNPC();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) ENPCRole Role = ENPCRole::Villager;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FText NPCName;
    UPROPERTY(EditDefaultsOnly) TArray<FNPCDialogue> Dialogues;
    UPROPERTY(EditDefaultsOnly) TArray<FTradeOffer> TradeOffers;
    UPROPERTY(EditDefaultsOnly) float InteractRadius = 3.f; // 3m

    UPROPERTY(BlueprintAssignable) FOnDialogueTriggered OnDialogue;
    UPROPERTY(BlueprintAssignable) FOnTradeCompleted OnTrade;

    UFUNCTION(BlueprintCallable, Server, Reliable) void Server_Interact(AActor* Player);
    UFUNCTION(BlueprintCallable, Server, Reliable) void Server_Trade(AActor* Player, int32 OfferIndex);
    UFUNCTION(BlueprintPure) TArray<FTradeOffer> GetAvailableTrades(UInventoryComponent* PlayerInv) const;
    UFUNCTION(BlueprintPure) FNPCDialogue GetRandomDialogue() const;

protected:
    virtual void BeginPlay() override;

private:
    void BuildDefaultDialogues();
    void BuildDefaultTrades();
    int32 DialogueIndex = 0;
};
