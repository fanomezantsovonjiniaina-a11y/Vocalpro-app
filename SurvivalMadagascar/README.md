# 🌿 Survival Madagascar Online

**Co-op Survival Game | Unreal Engine 5 | 3 Players**

> Olona 3 very tao amin'ny ala Malagasy. Tsy maintsy mahavelona 30 alina miaraka!

---

## Fomba fampiasana ny projet

### Requirements
- Unreal Engine 5.4+
- Visual Studio 2022 (Windows) na Xcode 14+ (Mac)
- Git

### Fanombohana

```bash
# 1. Clone
git clone https://github.com/fanomezantsovonjiniaina-a11y/vocalpro-app.git
cd vocalpro-app/SurvivalMadagascar

# 2. Generate project files
# Windows:
UnrealBuildTool.exe -projectfiles -project="SurvivalMadagascar.uproject" -game -rocket -progress

# Mac:
/Users/Shared/Epic Games/UE_5.4/Engine/Build/BatchFiles/Mac/GenerateProjectFiles.sh \
  -project="$(pwd)/SurvivalMadagascar.uproject" -game

# 3. Double-click SurvivalMadagascar.uproject
```

---

## Structure du Code

```
Source/SurvivalMadagascar/
├── Animals/
│   ├── FosaAI          — Prédateur principal (alina)
│   ├── KrokodilAI      — Prédateur rano
│   ├── LemurAI         — Companion (Catta/Indri/Aye-aye)
│   ├── MenaranaAI      — Serpents (Boa/Langaha/Ithycyphus)
│   └── ChameleonIndicator — Indicator météo/danger
├── Characters/
│   └── SurvivalCharacter  — Player (revive, rôle, vitesse)
├── Survival/
│   ├── SurvivalComponent  — HP/Faim/Soif/Fatigue/Poizina
│   ├── InventoryComponent — 24 slots, poids, transfer
│   └── CraftingLibrary    — 15 recettes
├── World/
│   ├── DayNightCycle   — 20min/jour, orana, warning
│   ├── BuildingSystem  — 12 bâtiments
│   └── MalagasyNPC     — 5 rôles, dialogue MG+FR, trade
├── Multiplayer/
│   └── SurvivalGameMode — 3 players, 30 alina, Fosa spawn
└── UI/
    └── SurvivalHUD     — Bars, warning, revive, teammate
```

---

## Gameplay

| Feature | Statut |
|---------|--------|
| Cycle Andro/Alina | ✅ |
| Survival (HP/Faim/Soif) | ✅ |
| Maladies & Poizina | ✅ |
| Fosa AI | ✅ |
| Krokodily AI | ✅ |
| Lémur Companion | ✅ |
| Menarana AI | ✅ |
| Caméléon Indicator | ✅ |
| Inventory (24 slots) | ✅ |
| Crafting (15 recettes) | ✅ |
| Construction (12 bâtiments) | ✅ |
| NPC Malagasy + Trade | ✅ |
| Multiplayer 3 joueurs | ✅ |
| Revive système | ✅ |
| HUD | ✅ |
| Map / Level Design | 🔲 |
| Animations | 🔲 |
| Sons & Musique | 🔲 |
| 3D Models | 🔲 |

---

## Objectif
Survivre **30 alina** → Mamorina pirogue → Miala amin'ny ala!

*Ny ala Malagasy miandry anao... 🌿*
