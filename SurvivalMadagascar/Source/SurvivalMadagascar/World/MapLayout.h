#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapLayout.generated.h"

UENUM(BlueprintType)
enum class EMapZone : uint8
{
    ForetTropicale,   // Zone départ — ala afovoany
    Plage,            // Bord de mer — sécurité relative
    Riviere,          // Manambola — KROKODILY!
    Lac,              // Lac Alaotra — pêche
    Tendrombohitra,   // Montagnes — ressources rares
    Tsingy,           // Rochers pointus — très dangereux
    Savane,           // Herbes hautes — Lambo Gidro
    VillageAbandonne, // Ruines — NPC, loot rare
    Marais,           // Swamp — Anguille, Moka
    Estuaire          // Embouchure — Crevettes, Pirogue
};

USTRUCT(BlueprintType)
struct FZoneData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly) EMapZone Zone;
    UPROPERTY(BlueprintReadOnly) FVector Center;
    UPROPERTY(BlueprintReadOnly) float Radius;
    UPROPERTY(BlueprintReadOnly) int32 DangerLevel; // 1-5
    UPROPERTY(BlueprintReadOnly) FText Name;
    UPROPERTY(BlueprintReadOnly) FText Description;
    UPROPERTY(BlueprintReadOnly) TArray<FName> Resources;
    UPROPERTY(BlueprintReadOnly) TArray<FName> Predators;
};

UCLASS()
class SURVIVALMADAGASCAR_API AMapLayout : public AActor
{
    GENERATED_BODY()
public:
    AMapLayout();

    // World size: 4km x 4km
    static constexpr float WORLD_SIZE = 400000.f; // 4km in cm

    UPROPERTY(BlueprintReadOnly) TArray<FZoneData> Zones;

    UFUNCTION(BlueprintPure) EMapZone GetZoneAtLocation(FVector Location) const;
    UFUNCTION(BlueprintPure) FZoneData GetZoneData(EMapZone Zone) const;
    UFUNCTION(BlueprintPure) int32 GetDangerLevel(FVector Location) const;
    UFUNCTION(BlueprintPure) TArray<FName> GetResourcesAt(FVector Location) const;
    UFUNCTION(BlueprintCallable) void RevealZone(EMapZone Zone, APlayerController* PC);

    // Starting location: center of ForetTropicale
    UFUNCTION(BlueprintPure) FVector GetStartLocation() const;
    // Victory location: Estuaire (pirogue launch point)
    UFUNCTION(BlueprintPure) FVector GetVictoryLocation() const;

protected:
    virtual void BeginPlay() override;

private:
    void InitializeZones();
    TSet<EMapZone> RevealedZones;
};
