#include "MapLayout.h"

AMapLayout::AMapLayout() { PrimaryActorTick.bCanEverTick = false; }

void AMapLayout::BeginPlay() { Super::BeginPlay(); InitializeZones(); }

void AMapLayout::InitializeZones()
{
    float S = WORLD_SIZE;

    // Zone: ForetTropicale — départ, centre de la map
    { FZoneData Z; Z.Zone = EMapZone::ForetTropicale;
      Z.Center = FVector(S*0.5f, S*0.5f, 0); Z.Radius = S*0.2f; Z.DangerLevel = 2;
      Z.Name = FText::FromString("Ala Tropicale");
      Z.Description = FText::FromString("Zone départ. Bois, fruits, herbes. Fosa la nuit.");
      Z.Resources = {"hazo","ravina","liane","voankazo","herbe","herbe_fanafody"};
      Z.Predators = {"Fosa","Menarana_Langaha","Kalanoro"};
      Zones.Add(Z); }

    // Zone: Plage — bord de mer, ouest
    { FZoneData Z; Z.Zone = EMapZone::Plage;
      Z.Center = FVector(S*0.05f, S*0.5f, 0); Z.Radius = S*0.12f; Z.DangerLevel = 1;
      Z.Name = FText::FromString("Moron-tsiraka");
      Z.Description = FText::FromString("Bord de mer. Poissons, sel, coquillages. Peu dangereux.");
      Z.Resources = {"sel","coquillage","pierre","sable","poisson_mer"};
      Z.Predators = {};
      Zones.Add(Z); }

    // Zone: Riviere — Manambola, nord
    { FZoneData Z; Z.Zone = EMapZone::Riviere;
      Z.Center = FVector(S*0.5f, S*0.1f, 0); Z.Radius = S*0.08f; Z.DangerLevel = 4;
      Z.Name = FText::FromString("Renirano Manambola");
      Z.Description = FText::FromString("KROKODILY! Eau douce, poissons. Très dangereux.");
      Z.Resources = {"rano_madio","tilapia_cru","pierre","argile"};
      Z.Predators = {"Krokodily","Menarana_Boa"};
      Zones.Add(Z); }

    // Zone: Lac Alaotra — est
    { FZoneData Z; Z.Zone = EMapZone::Lac;
      Z.Center = FVector(S*0.85f, S*0.3f, 0); Z.Radius = S*0.1f; Z.DangerLevel = 3;
      Z.Name = FText::FromString("Farihy Alaotra");
      Z.Description = FText::FromString("Grand lac. Pêche abondante. Krokodily présents.");
      Z.Resources = {"tilapia_cru","crevette","rano_madio","plante_aquatique"};
      Z.Predators = {"Krokodily"};
      Zones.Add(Z); }

    // Zone: Tendrombohitra — nord-est
    { FZoneData Z; Z.Zone = EMapZone::Tendrombohitra;
      Z.Center = FVector(S*0.8f, S*0.1f, 0); Z.Radius = S*0.12f; Z.DangerLevel = 3;
      Z.Name = FText::FromString("Tendrombohitra");
      Z.Description = FText::FromString("Montagnes. Pierre, minerais, plantes rares. Lambo gidro.");
      Z.Resources = {"pierre","minerai","kinina","hazo_mafy","cristal"};
      Z.Predators = {"LamboGidro","Menarana_Ithycyphus"};
      Zones.Add(Z); }

    // Zone: Tsingy — centre-nord — TRÈS DANGEREUX
    { FZoneData Z; Z.Zone = EMapZone::Tsingy;
      Z.Center = FVector(S*0.5f, S*0.15f, 0); Z.Radius = S*0.1f; Z.DangerLevel = 5;
      Z.Name = FText::FromString("Tsingy");
      Z.Description = FText::FromString("⚠️ TENA MAMPIDI-DOZA! Vato maranitra, Menarana maro, Kalanoro.");
      Z.Resources = {"minerai_rare","pierre","cristal","kinina_tsara","lémur_rare"};
      Z.Predators = {"Menarana_Langaha","Menarana_Ithycyphus","Kalanoro","Angatra"};
      Zones.Add(Z); }

    // Zone: Savane — sud
    { FZoneData Z; Z.Zone = EMapZone::Savane;
      Z.Center = FVector(S*0.5f, S*0.85f, 0); Z.Radius = S*0.15f; Z.DangerLevel = 2;
      Z.Name = FText::FromString("Savane");
      Z.Description = FText::FromString("Herbes hautes. Lambo Gidro, herbes médicinales.");
      Z.Resources = {"fibres","herbe_med","viande_crue","peau","plume"};
      Z.Predators = {"LamboGidro"};
      Zones.Add(Z); }

    // Zone: Village Abandonné — sud-est
    { FZoneData Z; Z.Zone = EMapZone::VillageAbandonne;
      Z.Center = FVector(S*0.75f, S*0.75f, 0); Z.Radius = S*0.08f; Z.DangerLevel = 3;
      Z.Name = FText::FromString("Tanàna Foana");
      Z.Description = FText::FromString("Village abandonné. NPC, loot rare, secrets. Kinoly la nuit!");
      Z.Resources = {"outil_ancien","tissu","recipient","carte_mystere","hazo"};
      Z.Predators = {"Kinoly","Fosa"};
      Zones.Add(Z); }

    // Zone: Marais — ouest-nord
    { FZoneData Z; Z.Zone = EMapZone::Marais;
      Z.Center = FVector(S*0.15f, S*0.2f, 0); Z.Radius = S*0.1f; Z.DangerLevel = 4;
      Z.Name = FText::FromString("Manandriana (Marais)");
      Z.Description = FText::FromString("Marécage. Anguille électrique, Moka, Menarana. Danger!");
      Z.Resources = {"anguille_crue","rano_maloto","herbe_marais","fibres","grenouille"};
      Z.Predators = {"Anguille","Moka","Menarana_Boa","Krokodily"};
      Zones.Add(Z); }

    // Zone: Estuaire — objectif final!
    { FZoneData Z; Z.Zone = EMapZone::Estuaire;
      Z.Center = FVector(S*0.05f, S*0.8f, 0); Z.Radius = S*0.08f; Z.DangerLevel = 2;
      Z.Name = FText::FromString("Estuaire — Point de Départ Pirogue");
      Z.Description = FText::FromString("🏆 OBJECTIF FINAL! Construire pirogue ici → Liberté!");
      Z.Resources = {"crevette","bois_derive","sel","coquillage"};
      Z.Predators = {};
      Zones.Add(Z); }
}

EMapZone AMapLayout::GetZoneAtLocation(FVector Location) const
{
    EMapZone Nearest = EMapZone::ForetTropicale;
    float NearestDist = FLT_MAX;
    for(const FZoneData& Z : Zones)
    {
        float D = FVector::Dist2D(Location, Z.Center);
        if(D < Z.Radius && D < NearestDist) { NearestDist = D; Nearest = Z.Zone; }
    }
    return Nearest;
}

FZoneData AMapLayout::GetZoneData(EMapZone Zone) const
{
    for(const FZoneData& Z : Zones) if(Z.Zone == Zone) return Z;
    return FZoneData();
}

int32 AMapLayout::GetDangerLevel(FVector Location) const
{
    return GetZoneData(GetZoneAtLocation(Location)).DangerLevel;
}

TArray<FName> AMapLayout::GetResourcesAt(FVector Location) const
{
    return GetZoneData(GetZoneAtLocation(Location)).Resources;
}

void AMapLayout::RevealZone(EMapZone Zone, APlayerController* PC)
{
    RevealedZones.Add(Zone);
    // Notify UI to show zone on map — handled in Blueprint
}

FVector AMapLayout::GetStartLocation() const
{
    return FVector(WORLD_SIZE*0.5f, WORLD_SIZE*0.5f, 100.f);
}

FVector AMapLayout::GetVictoryLocation() const
{
    return FVector(WORLD_SIZE*0.05f, WORLD_SIZE*0.8f, 100.f);
}
