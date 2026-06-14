# GAME DESIGN DOCUMENT
# 🌿 SURVIVAL MADAGASCAR ONLINE
**Version:** 1.0  
**Date:** 2026-06-14  
**Genre:** Survival / Exploration / Co-op Multiplayer  
**Engine:** Unreal Engine 5  
**Players:** 1–3 (Co-op Online)  
**Platform:** PC (Windows/Linux), Console (futur)

---

## 1. VISION DU JEU

> Trois joueurs sont perdus dans la forêt de Madagascar. Sans technologie, sans nourriture — ils doivent survivre ensemble en utilisant les ressources naturelles de l'île, affronter les prédateurs de la nuit, et explorer un monde vivant et dangereux pour retrouver la civilisation.

**Piliers du jeu :**
- 🌿 **Exploration** — Monde ouvert inspiré de Madagasikara réel
- 🤝 **Co-op** — Survie en équipe de 3 joueurs
- 🌙 **Tension nocturne** — Prédateurs la nuit, calme le jour
- 🌍 **Authenticité** — Faune, flore, culture Malagasy réelles

---

## 2. GAMEPLAY CORE LOOP

```
MATIN:
└── Réveiller → Vérifier besoins (faim, soif, santé)
    └── Explorer → Collecter ressources (nourriture, bois, herbes)
        └── Construire / Améliorer base
            └── Préparer pour la NUIT

NUIT:
└── Allumer feu → Construire barricades
    └── Faire la garde (un joueur veille)
        └── Survivre aux prédateurs
            └── MATIN → Recommencer
```

**Objectif final :** Survivre **30 nuits** → Trouver la civilisation et s'échapper de la forêt.

---

## 3. MAP — ZONES DE MADAGASIKARA

### 3.1 Zones Disponibles

| Zone | Description | Danger | Ressources |
|------|-------------|--------|------------|
| 🌿 **Forêt Tropicale** | Zone de départ, dense | Moyen | Bois, fruits, herbes |
| 🏖️ **Plage / Côte** | Bord de mer, ouvert | Faible | Poissons, coquillages, sable |
| 💧 **Rivière / Lac** | Eau douce, marécages | Élevé (Krokodily) | Eau, poissons, argile |
| 🏔️ **Tendrombohitra** | Montagnes, rochers | Moyen | Pierre, plantes rares |
| 🏜️ **Tsingy** | Rochers pointus, labyrinthe | Très élevé | Minéraux, serpents, lémurs rares |
| 🌾 **Savane** | Herbes hautes, terrain ouvert | Moyen | Lambo gidro, herbes médicinales |
| 🏚️ **Village Abandonné** | Ruines, secrets | Inconnu | Outils anciens, indices carte |

### 3.2 Points d'Intérêt

- **Manambola River** — Rivière principale (Krokodily zone)
- **Avenue des Baobabs** — Lieu iconique, ressources rares
- **Grotte de Tsingy** — Cachette sûre mais difficile d'accès
- **Village Malagasy** — NPC, commerce, quêtes
- **Épave de Bateau** — Loot rare sur la côte

---

## 4. SYSTÈME ANDRO/ALINA (Cycle Jour/Nuit)

```
⏱️ 1 heure réelle = 3 jours in-game
⏱️ 1 jour in-game = 20 minutes réelles
    ├── Jour (12 min) : 06:00 → 18:00
    └── Nuit (8 min)  : 18:00 → 06:00
```

### Phases de la Journée :

| Heure | Phase | Événement |
|-------|-------|-----------|
| 06:00 | 🌅 Maraina | Réveil, oiseaux chantent, calme |
| 09:00 | ☀️ Andro | Exploration libre, ressources abondantes |
| 12:00 | 🌞 Mitataovovonana | Chaleur max, faim augmente vite |
| 15:00 | 🌤️ Hariva kely | Meilleur moment pour construire |
| 17:30 | ⚠️ **WARNING** | "Ny alina manakaiky... miorena!" |
| 18:00 | 🌆 Hariva | Lumière diminue, sons changent |
| 19:00 | 🌙 **ALINA** | Fosa réveillée, DANGER commence |
| 00:00 | 🌑 Misasaka alina | Danger maximum, visibilité nulle |
| 04:00 | 🌃 Vaovaom-parihy | Tension diminue doucement |
| 06:00 | 🌅 Maraina | Nouveau jour, sécurité |

---

## 5. SYSTÈME DE SURVIE

### 5.1 Besoins du Joueur (Needs System)

```
❤️  FAHASALAMANA (HP)    : 0–100
🍖  HANINA (Faim)        : 0–100 (diminue -2/min)
💧  RANO (Soif)          : 0–100 (diminue -3/min)
😴  TORIMASO (Fatigue)   : 0–100 (augmente +1/min)
🌡️  HAFANANA (Chaleur)   : 0–100 (variable selon météo)
```

**Conséquences si besoins non satisfaits :**
- Faim = 0 → HP -5/min
- Soif = 0 → HP -8/min (plus dangereux)
- Fatigue = 100 → Vitesse -50%, vision floue
- Chaleur trop haute → Déshydratation rapide

### 5.2 Maladies et Poisons

| Maladie | Cause | Symptômes | Remède |
|---------|-------|-----------|--------|
| 🦟 **Malaria** | Piqûre moustique (alina) | Fatigue, fièvre, HP -3/min | Quinquina (écorce rare) |
| 🐍 **Poizina Menarana** | Morsure serpent | HP -10/min, vision floue | Herbe Fanafody (forêt) |
| 🕷️ **Poizina Hainteny** | Morsure araignée | Paralysie temporaire | Rano mangotraka + citron |
| 🤢 **Infeksiona** | Blessure non soignée | HP -2/min lentement | Écorce antibiotique |
| 💧 **Rano maloto** | Boire eau non purifiée | Vomissements, soif augmente | Repos + rano madio |

### 5.3 Fanafody (Remèdes Naturels)

```
🌿 Herbes médicinales:
├── Kinina (Quinquina)     → Anti-malaria
├── Ravintsara             → Anti-infection  
├── Papaye verte           → Cicatrisant
├── Gingembre sauvage      → Anti-nausée
├── Aloès sauvage          → Brûlures, coupures
└── Baobab (écorce)        → Énergie, anti-fatigue
```

---

## 6. BIBY (FAUNE DE MADAGASIKARA)

### 6.1 ☠️ Prédateurs Dangereux

#### 🦁 FOSA (Cryptoprocta ferox) — *Prédateur Principal*
```
Statut      : Actif ALINA uniquement (19:00–06:00)
HP          : 300
Dégâts      : -25 HP par attaque
Vitesse     : Très rapide (sprint player +20%)
Intelligence: Haute (contourne les obstacles)

Comportement AI:
├── Cherche les joueurs isolés en priorité
├── Évite les grands feux (rayon 10m)
├── Tourne autour du camp → cherche une faille
├── Rugit avant d'attaquer → warning sonore
├── Nuit de pluie : plus agressif, feu s'éteint
├── Si 3 joueurs ensemble + feu : recule
└── Si joueur seul : attaque immédiatement

Sons:
├── Grognement lointain : Fosa dans la zone
├── Grognement proche  : Fosa à 30m
└── Rugissement        : Attaque imminente!
```

#### 🐊 KROKODILY (Crocodylus niloticus)
```
Statut      : Actif rano sy berge (jour + nuit)
HP          : 500
Dégâts      : -40 HP (mort quasi immédiate)
Zone        : Rivières, lacs, marécages

Comportement AI:
├── Immobile sous l'eau (invisible si eau trouble)
├── Attaque éclair si joueur entre dans l'eau
├── Sur berge: attaque si < 5m
├── Ne sort pas loin de l'eau (max 15m)
└── Vulnérable: yeux, ventre (faiblesse)

Zones dangereuses:
├── Manambola River ★★★
├── Tsiribihina River ★★★
├── Lac Alaotra ★★
└── Tout marécage ★★
```

#### 🐍 MENARANA (Serpents)
```
Karazana 1: Boa de Madagascar
├── HP: 200, Dégâts: -30 HP (constriction)
├── Taille: 3–5 mètres
├── Lent mais fort
└── Forêt dense, nuit

Karazana 2: Langaha (venimeux)
├── HP: 50, Dégâts: Poison (HP -8/min pendant 5min)
├── Taille: petit (difficile à voir!)
├── Herbes hautes, bois pourri
└── Jour + nuit

Karazana 3: Ithycyphus (venimeux modéré)
├── HP: 80, Dégâts: Poison (HP -4/min pendant 3min)
├── Dans les arbres (tombe sur le joueur!)
└── Forêt canopée
```

#### 🕷️ HAINTENY sy KALALAO (Araignées)
```
Hainteny (Nephila):
├── Toile géante dans forêt
├── Ralentit le joueur (piégé dans toile)
└── Morsure: paralysie 10 secondes

Kalalao venimeux:
├── Caché dans base si mal construite
├── Morsure nocturne pendant sommeil
└── Poison lent: HP -2/min
```

#### 🦟 MOKA (Moustiques)
```
Actif     : Alina uniquement
Danger    : Malaria (probabilité 30% par nuit sans protection)
Protection:
├── Feu (éloigne les moustiques)
├── Moustiquaire craftée
└── Herbes répulsives (brûlées)
```

#### 🐗 LAMBO GIDRO (Potamochère / Sanglier)
```
HP      : 250
Dégâts  : -20 HP
Actif   : Jour (savane, forêt)
Neutre  : Ignore les joueurs
Agressif: Si approche < 5m, ou attaque un petit
Loot    : Viande (x5), os, peau
```

---

### 6.2 🟢 Biby Sakaiza (Animaux Alliés)

#### 🐒 LÉMUR — *Compagnon et Indicateur*
```
Karazana:
├── Lémur Catta    — Savane, familier, apprivoisable
├── Indri          — Forêt, cri fort, indicateur danger
└── Aye-aye        — Nuit, rare, porte-bonheur

Utility:
├── Indri crie fort si Fosa à moins de 100m → WARNING!
├── Lémur Catta apprivoisé → suit le joueur, trouve fruits
├── Lémur apprivoisé → peut distraire la Fosa
└── Aye-aye trouvé → bonus chance (moins de malaria cette nuit)

Comment apprivoiser:
├── Offrir fruits pendant 3 jours
├── Ne pas effrayer
└── Construire petite maison pour lui
```

#### 🦎 CAMÉLÉON — *Indicateur Météo*
```
Couleurs:
├── Vert vif    → Tout va bien
├── Jaune       → Pluie dans 5 min
├── Rouge       → DANGER proche (Fosa, serpent)
├── Marron      → Température trop chaude
└── Bleu        → Nuit calme, pas de danger

Apprivoisable: Oui (nourrir insectes)
Utilité: Prédicteur météo naturel
```

#### 🦜 VORONA MALAGASY (Oiseaux)
```
Coua coucou    → Chante le matin = aucun danger la nuit passée
Vanga          → Silence soudain = prédateur proche!
Drongo         → Imite sons → peut tromper la Fosa
Perroquet gris → Apprivoisable → dit "FOSA!" si danger
```

#### 🐟 POISSONS (Rivière / Mer)
```
Tilapia       → Rivières, facile à pêcher
Crevettes     → Estuaires
Poissons mer  → Requiert pirogue
Anguille      → Marécages, choc électrique si touchée!
```

---

## 7. SYSTÈME DE CONSTRUCTION

### 7.1 Niveaux de Base

```
NIVEAU 1 — Abri d'urgence (Nuit 1–3)
├── Matériaux: Feuilles ravinala, bambou
├── Protection: Pluie uniquement
├── Fosa peut forcer l'entrée!
└── Temps: 10 minutes

NIVEAU 2 — Case Malagasy (Nuit 4–10)
├── Matériaux: Bois, liane, feuilles
├── Protection: Pluie + petits animaux
├── Fosa ne peut pas entrer (si barricade)
└── Temps: 30 minutes

NIVEAU 3 — Compound Fortifié (Nuit 10+)
├── Matériaux: Bois solide, pierre, bambou renforcé
├── Protection: Tous animaux
├── Tours de guet possible
└── Temps: 2 heures (équipe)

NIVEAU 4 — Village (Nuit 20+)
├── Matériaux: Pierre, bois traité, métal récupéré
├── Plusieurs bâtiments
├── Stockage sécurisé
└── Base permanente
```

### 7.2 Défenses Disponibles

| Défense | Matériaux | Effet |
|---------|-----------|-------|
| 🔥 Feu de camp | Bois x5 | Éloigne Fosa (rayon 10m) |
| 🔥 Torche | Bois x1 + résine | Éclairage + protection locale |
| 🪵 Barricade bois | Bois x8 | Bloque passage (HP: 200) |
| 🪨 Mur pierre | Pierre x12 | Bloque passage (HP: 500) |
| ⚡ Piège | Bois x3 + liane | Dommages -50 HP Fosa |
| 🪤 Filet | Liane x10 | Ralentit Fosa 30 secondes |
| 🔔 Alarme | Bambou x5 | Son si Fosa touche |

---

## 8. CRAFTING SYSTÈME

### 8.1 Ressources Principales

```
🪵 BOIS          → Hazo (partout)
🌿 LIANE          → Ala (remplace corde)
🪨 PIERRE         → Tendrombohitra, rivière  
🍃 FEUILLES       → Ravinala (partout)
🦴 OS             → Chasse (lambo, biby)
🐟 POISSONS       → Rivière, mer
🌾 FIBRES         → Herbes hautes
🌺 HERBES MED.    → Ala (rare)
🔥 RÉSINE         → Arbres spéciaux
💎 MINERAIS       → Tsingy, tendrombohitra
```

### 8.2 Crafting Recettes Importantes

```
OUTILS:
├── Hache: Pierre x2 + Bois x1 + Liane x1
├── Couteau: Pierre x1 + Liane x1  
├── Lance: Bois x2 + Pierre x1
├── Arc: Bois courbe x1 + Liane x2
└── Flèche: Bois x1 + Plume x1 + Pierre x1

NOURRITURE:
├── Viande grillée: Viande + Feu
├── Eau purifiée: Rano + Feu + Récipient
├── Soupe herbes: Rano + Herbes + Récipient + Feu
└── Conservation: Viande + Sel (bord de mer)

MÉDECINE:
├── Pansement: Feuilles + Fibres
├── Antidote basique: Herbe x3 + Rano
├── Anti-malaria: Kinina x2 + Rano chaud
└── Désinfectant: Résine + Alcool plante

PROTECTION:
├── Moustiquaire: Fibres x10 + Liane x5
├── Répulsif: Herbes spéciales brûlées
└── Armure légère: Peau x5 + Liane x3
```

---

## 9. SYSTÈME MULTIJOUEUR

### 9.1 Configuration

```
Mode         : Co-op Online
Joueurs      : 1–3 (recommandé: 3)
Session type : Public ou Privé (code invitation)
Serveur      : Dedicated Server (EOS)
Sync         : Position, HP, Inventaire, World State
```

### 9.2 Système d'Équipe

```
RÔLES DISPONIBLES (libre choix):
├── 🗡️  Chasseur    → Spécialisé combat et chasse
├── 🌿 Soigneur    → Spécialisé médecine et herbes
└── 🔨 Bâtisseur   → Spécialisé construction et défense

COMMUNICATION:
├── 🎙️ Voice Chat (proximity: entend si < 50m)
├── 📍 Ping système (marquer danger, ressources)
├── 💬 Chat texte
└── 🚨 Signal d'urgence (SOS rouge sur map)

REVIVE SYSTÈME:
├── Joueur mort → Tombé au sol (peut encore parler)
├── Fenêtre revive: 60 secondes
├── Revive: Approcher + maintenir E (10 secondes)
├── Après revive: HP 30%, vulnérable
└── Si personne revive en 60s → Respawn au camp
```

### 9.3 Partage de Ressources

```
INVENTAIRE:
├── Inventaire personnel (24 slots)
├── Coffre commun au camp (illimité)
└── Transfert rapide: maintenir F près d'un joueur

PARTAGE AUTOMATIQUE:
├── Map explorée → partagée avec équipe
├── Danger détecté → notification équipe
└── Mort d'un joueur → notification + localisation
```

---

## 10. PROGRESSION & OBJECTIFS

### 10.1 Objectif Principal

```
SURVIE 30 NUITS → ÉCHAPPER DE LA FORÊT

Étapes:
├── Nuit 1–5   : Survivre les premières nuits (le plus difficile)
├── Nuit 6–10  : Établir une base solide
├── Nuit 11–15 : Explorer les zones dangereuses
├── Nuit 16–20 : Trouver indices pour la sortie
├── Nuit 21–25 : Construire moyen de transport (pirogue/radeau)
├── Nuit 26–29 : Préparer l'évasion
└── Nuit 30    : S'échapper → FIN / VICTOIRE!
```

### 10.2 Quêtes Secondaires

```
📜 QUÊTES HISTOIRE:
├── Trouver les autres survivants (NPC)
├── Découvrir pourquoi le village est abandonné
├── Décoder la carte mystérieuse
└── Construire un signal SOS (radio artisanale)

🏆 DÉFIS:
├── Survivre une nuit de tempête
├── Traverser le Tsingy
├── Apprivoiser 3 lémurs
├── Tuer une Fosa (très difficile)
└── Pêcher dans la zone Krokodily
```

### 10.3 Système de Connaissance

```
JOURNAL DU SURVIVANT (débloqué progressivement):
├── Biby: Fiches sur chaque animal rencontré
├── Plantes: Propriétés découvertes par expérience
├── Carte: Se remplit en explorant
├── Recettes: Découvertes par craft ou NPC
└── Langue: Mots Malagasy appris via NPC
```

---

## 11. AUDIO ET ATMOSPHÈRE

### 11.1 Musique

```
JOUR:
├── Musique douce, instruments Malagasy (valiha, sodina)
├── Sons de forêt tropicale
└── Ambiance paisible et belle

HARIVA (Crépuscule):
├── Musique commence à changer
├── Tension qui monte doucement
└── Sons d'animaux nocturnes qui commencent

ALINA:
├── Musique sombre, tension permanente
├── Grognements, bruits de pas dans les feuilles
└── Silence soudain = DANGER IMMINENT

DANGER:
├── Musique de combat intense
├── Heartbeat du joueur (rythme cardiaque rapide)
└── Son directionnel (entend d'où vient la Fosa)
```

### 11.2 Sons Importants

```
🦁 Fosa:
├── Grognement lointain (zone: alerte)
├── Grognement proche (< 30m: danger)
├── Rugissement (attaque: 3 secondes!)
└── Pas dans feuilles mortes (nuit silencieuse)

🐒 Indri (Lémur):
└── Cri fort = Fosa détectée dans la zone!

🦜 Oiseaux:
└── Silence soudain = quelque chose approche

🌧️ Pluie:
├── Couvre les sons (ne peut plus entendre la Fosa!)
└── Éteint les feux (danger x2)
```

---

## 12. GRAPHISMES ET DIRECTION ARTISTIQUE

### 12.1 Style Visuel

```
Style: Réaliste (pas cartoon)
Référence: Green Hell, The Forest (niveau graphique)
Spécificité: Couleurs chaudes Madagascar (rouge, vert, ocre)
```

### 12.2 Assets Prioritaires UE5

```
ENVIRONNEMENT:
├── Megascans: Terre rouge Madagascar, rochers
├── Forêt tropicale dense (Lumen éclairage)
├── Baobabs (asset iconique)
├── Rivières avec Krokodily (eau UE5)
└── Village Malagasy traditionnel

PERSONNAGES:
├── 3 personnages jouables (diversité)
├── NPC Malagasy (MetaHuman)
└── Animations réalistes (survie, fatigue)

BIBY:
├── Fosa (modèle custom, animation IA)
├── Krokodily (animation eau/terre)
├── Lémur (animation social, apprivoisement)
├── Serpents (animation sinueuse)
└── Caméléon (changement couleur shader)
```

### 12.3 Effets Visuels

```
🌅 Lever soleil: Ciel orange/rouge sur baobabs
🌙 Nuit: Ciel étoilé Madagascar (pas de pollution)
🌧️ Pluie: Effets volumétriques, boue
🔥 Feu: Flammes Lumen, ombres dynamiques
🌫️ Brouillard: Matin dans la forêt
```

---

## 13. INTERFACE UTILISATEUR (HUD)

```
HUD MINIMAL (immersif):

Coin bas-gauche:
├── ❤️ HP Bar
├── 🍖 Faim
├── 💧 Soif
└── 😴 Fatigue

Coin bas-droite:
├── 🎒 Inventaire rapide (8 slots)
└── 🔧 Outil actif

Centre-bas:
└── Boussole simple

Coin haut-droite:
└── ⏰ Heure in-game + icône lune/soleil

Map:
└── Touche M → Map complète (zones explorées)
    ├── Position équipe (icônes joueurs)
    ├── Camp marqué
    └── Points d'intérêt découverts
```

---

## 14. TECHNOLOGIES ET ARCHITECTURE

### 14.1 Stack Technique

```
ENGINE:
└── Unreal Engine 5.4+
    ├── Lumen (éclairage global dynamique)
    ├── Nanite (géométrie haute densité)
    ├── World Partition (map ouverte infinie)
    ├── Chaos Physics (destruction, ragdoll)
    └── MetaSounds (audio procédural)

MULTIJOUEUR:
└── Epic Online Services (EOS)
    ├── Sessions (lobby, matchmaking)
    ├── Voice Chat intégré
    ├── Replication (sync état monde)
    └── Dedicated Server (Linux)

INFRASTRUCTURE:
└── AWS EC2 (serveur dédié)
    ├── Instance: t3.medium (~$30/mois)
    ├── OS: Ubuntu Server 22.04
    ├── Région: EU (latence Africa/Madagascar)
    └── Auto-scaling selon joueurs

LANGAGE:
├── C++ (systèmes core: IA, physique, réseau)
└── Blueprints (gameplay, UI, événements)
```

### 14.2 Architecture Réseau

```
CLIENT (Joueur):
├── Rendu graphique (UE5)
├── Input joueur
├── Prédiction locale (mouvement fluide)
└── UI / HUD

SERVEUR DÉDIÉ:
├── Logique jeu (autorité)
├── Position tous joueurs
├── État du monde (bâtiments, ressources)
├── IA des animaux
├── Cycle jour/nuit
└── Sauvegarde session

REPLICATION (Serveur → Clients):
├── Position joueurs (60 fois/seconde)
├── État animaux (30 fois/seconde)
├── Événements (mort, craft, danger)
└── World state (changements terrain)
```

---

## 15. PLANNING DE DÉVELOPPEMENT

### Phase 1 — Prototype (3 mois)
```
✅ Tontolo 3D basique (forêt simple)
✅ Character controller (marche, course, saut)
✅ Cycle jour/nuit
✅ Multiplayer 3 joueurs (position sync)
✅ Feu de camp basique
```

### Phase 2 — Core Survival (3 mois)
```
✅ Système besoins (faim, soif, santé)
✅ Inventaire + crafting basique
✅ Fosa IA simple (patruille + attaque)
✅ Construction base niveau 1-2
✅ Sons + musique basique
```

### Phase 3 — Monde Vivant (3 mois)
```
✅ Map complète Madagascar
✅ Tous les animaux + IA
✅ Système maladies + remèdes
✅ NPC Malagasy
✅ Quêtes principales
```

### Phase 4 — Polish & Release (3 mois)
```
✅ Graphismes finaux (Megascans, Lumen)
✅ Audio complet (musique Malagasy)
✅ Optimisation performance
✅ Tests multijoueur intensifs
✅ Beta test → corrections
✅ LAUNCH! 🚀
```

**TOTAL: ~12 mois (équipe 2–5 personnes)**

---

## 16. ÉQUIPE RECOMMANDÉE

| Rôle | Nombre | Outils |
|------|--------|--------|
| Game Designer | 1 | (toi!) |
| UE5 Developer | 1–2 | C++ + Blueprints |
| 3D Artist | 1 | Blender + Megascans |
| Sound Designer | 1 | FMOD + DAW |
| Level Designer | 1 | UE5 World Builder |

---

## 17. BUDGET ESTIMATIF

```
DÉVELOPPEMENT:
├── Unreal Engine 5        : GRATUIT
├── Epic Online Services   : GRATUIT (jusqu'à 1M users)
├── Megascans Assets       : GRATUIT (avec UE5)
├── Serveur AWS            : ~$30–50/mois
└── Assets supplémentaires : ~$500–2000

LANCEMENT:
├── Steam (PC)             : $100 frais uniques
└── Marketing              : Variable

TOTAL MINIMUM: ~$1000–3000 pour MVP
```

---

*Document créé le 2026-06-14*  
*Projet: Survival Madagascar Online*  
*Ny ala Malagasy miandry anao... 🌿*
