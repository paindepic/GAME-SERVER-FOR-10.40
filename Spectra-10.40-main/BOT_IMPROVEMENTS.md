# Améliorations des Bots Joueurs - Spectra 10.40

## Résumé des Modifications

Ce document décrit les améliorations significatives apportées aux bots joueurs dans Spectra 10.40.

---

## 1. Suppression de la Téléportation

### Problème
Les bots utilisaient auparavant `K2_TeleportTo` pour se déplacer instantanément, ce qui créait un comportement artificiel et prévisible.

### Solution
- **Remplacement par des mouvements naturels** : Les bots utilisent désormais `MoveToLocation` et `MoveToActor` pour se déplacer de manière réaliste
- **Système anti-blocage** : Détection automatique quand un bot est bloqué avec tentative de saut pour contourner les obstacles
- **Mouvements fluides** : Les bots utilisent `AddMovementInput` pour des déplacements plus naturels

### Fichiers Modifiés
- `PlayerBots.h` : Classes `BotsBTService_AIDropZone`, `BotsBTService_Loot`, `BotsBTService_AIEvaluator`

---

## 2. Système de Détection Sonore

### Nouvelles fonctionnalités
- **Détection des tirs** : Les bots peuvent détecter les tirs dans un rayon de 5000 unités
- **Mémorisation des sons** : Structure `FSoundMemory` pour stocker les informations sur les sons détectés
- **Investigation** : Nouvel état `InvestigatingSound` où les bots se déplacent prudemment vers la source du son
- **Réaction basée sur la personnalité** :
  - Aggressif : Approche rapide
  - Tactique : Approche prudente avec distance augmentée
  - Autres : Approche standard

### Types de Sons Détectés
```cpp
enum class EBotSoundType {
    None,
    Gunshot,      // Tirs d'armes
    Building,     // Construction
    Footsteps,    // Pas
    Chest,        // Coffres
    Vehicle,      // Véhicules
    MAX
};
```

### Classe Principale
- `BotsBTService_SoundDetection` : Détection et investigation des sons

---

## 3. Stratégies de Combat Améliorées

### Nouveau Système de Stances
```cpp
enum class EBotCombatStance {
    Passive,      // Non engagé
    Engaged,      // En combat
    Retreating,   // Recul
    Flanking,     // Flanc
    Holding,      // Position défensive
    Pushing,      // Poussee agressive
    MAX
};
```

### Comportements de Combat

#### 1. Stance Engaged (Standard)
- Strafe latéral
- Recul si ennemi trop proche (< 1000)
- Maintien de la distance optimale

#### 2. Stance Pushing (Agressive)
- Course vers l'ennemi
- Réduction de la distance à 150 unités
- Priorité à l'attaque

#### 3. Stance Retreating (Défensive)
- Recul tout en tirant
- Recherche de couvert
- Activée quand santé < 50%

#### 4. Stance Flanking (Tactique)
- Calcul de position de flanc
- Approche perpendiculaire
- Disponible uniquement pour personnalité Tactical

#### 5. Stance Holding
- Accroupissement pour précision
- Maintien de position
- Utilisé périodiquement

### Prédiction de Position
- Les bots tactiques (Skill >= 6) prédisent la position ennemie
- Basé sur la vélocité actuelle
- Prédiction à 0.5 secondes

### Tir Amélioré
- Précision basée sur le niveau de skill
- Multiplicateur d'erreur : `(11 - SkillLevel) / 10`
- Skills 3-9 (plus précis = meilleur)

---

## 4. Nouvelle Personnalité : Tactique

### Distribution des Personnalités
- Balanced : 35%
- Aggressive : 30%
- Builder : 20%
- Tactical : 15% (NOUVEAU)

### Caractéristiques Tactique
- Utilise le flanking
- Approche prudente des sons
- Prédiction de position
- Construction défensive proactive
- Maintien des distances

---

## 5. Comportements de Lobby Réalistes

### Nouveaux Choix de Warmup

#### SocialGroup
- Regroupement avec autres bots
- Formation de groupes naturels
- Distance de 300 unités entre bots

#### ExploreSpawnIsland
- Exploration de l'île
- Mouvements directionnels aléatoires (500-2000 unités)
- Regards autour pendant l'exploration

#### DanceInGroup
- Danse quand proche d'autres joueurs (< 500 unités)
- Déplacement vers les groupes si seul
- Interactions sociales réalistes

### Améliorations des Comportements Existants
- **RunAround** : Sprints plus fréquents
- **JumpAround** : Sauts pendant les mouvements
- **PracticeBuilding** : Construction + destruction

---

## 6. Spawn sur les Toits

### Implémentation
Le fichier `Bots.h` contient déjà la logique de spawn sur toits :

```cpp
void SpawnPlayerBot(AActor* OverrideSpawnLoc = nullptr, 
                    PlayerBots::EBotState StartingState = PlayerBots::EBotState::Warmup, 
                    bool bSpawnOnRoof = false);
```

### Utilisation
```cpp
// Spawn sur toit
Bots::SpawnPlayerBot(nullptr, PlayerBots::EBotState::Warmup, true);

// Spawn normal
Bots::SpawnPlayerBot(nullptr, PlayerBots::EBotState::Warmup, false);
```

### Logique
- Utilise le tableau `BuildingFoundations`
- Offset Z de +800 unités
- Randomisation X/Y de ±300 unités
- Fallback sur PlayerStarts si BuildingFoundations vide

---

## 7. Système de Construction Amélioré

### Patterns de Construction
- **DefensiveWall** : Mur simple
- **DefensiveBox** : 4 murs de protection
- **Turtle** : Boîte complète avec sol et toit
- **RampRush** : Escalier + mur pour poussée
- **Nineties** : Séquence mur + escalier rapide
- **HighGroundRetake** : Récupération hauteur

### Sélection Basée sur Personnalité
```cpp
// Aggressive
if (bIsStressed && Health < 50) -> DefensiveWall
if (Dist < 800 && HeightDiff < -200) -> RampRush

// Builder
if (bIsStressed) -> Turtle
if (HeightDiff > 200) -> HighGroundRetake

// Tactical (NOUVEAU)
if (bIsStressed || Health < 60) -> DefensiveBox
if (HeightDiff > 150) -> HighGroundRetake
if (Dist < 600) -> DefensiveWall

// Balanced
if (bIsStressed) -> DefensiveBox
if (HeightDiff > 200) -> RampRush
```

---

## 8. Détection Anti-Blocage

### Système de Détection
- Vérification toutes les 3 secondes
- Seuil de mouvement : 100 unités
- Compteur de blocages

### Réaction au Blocage
1. Tentative de saut
2. Réinitialisation du compteur
3. Changement de direction

### Implémentation
```cpp
if (CurrentTime - bot->StuckCheckTime > 3.f) {
    float DistMoved = Vector_Distance(bot->LastStuckCheckPosition, bot->LastUpdatedBotLocation);
    if (DistMoved < 100.f) {
        bot->StuckCounter++;
        if (bot->StuckCounter > 2) {
            bot->Pawn->Jump();
            bot->StuckCounter = 0;
        }
    }
}
```

---

## 9. Champs de Données Ajoutés au PlayerBot

### Combat et IA
```cpp
EBotCombatStance CombatStance = EBotCombatStance::Passive;
FSoundMemory LastDetectedSound;
float LastCombatTime = 0.f;
FVector LastKnownEnemyPosition = FVector::ZeroVector;
int32 ConsecutiveShotsFired = 0;
float LastShotTime = 0.f;
bool bIsFlanking = false;
FVector FlankTargetPosition = FVector::ZeroVector;
float TimeInCurrentStance = 0.f;
```

### Mouvement Naturel
```cpp
FVector NaturalMovementTarget = FVector::ZeroVector;
float NaturalMovementSpeed = 0.f;
bool bIsMovingNaturally = false;
float StuckCheckTime = 0.f;
FVector LastStuckCheckPosition = FVector::ZeroVector();
int32 StuckCounter = 0;
float LastNaturalMoveUpdate = 0.f;
float TargetArrivalTime = 0.f;
```

---

## 10. Statistiques du Code

### Lignes de Code
- Fichier original : ~2230 lignes
- Fichier amélioré : ~2207 lignes
- Code optimisé et mieux organisé

### Classes Ajoutées/Modifiées
1. `BotsBTService_SoundDetection` (NOUVEAU)
2. `BotsBTService_AIEvaluator` (AMÉLIORÉ)
3. `BotsBTService_Combat` (AMÉLIORÉ)
4. `BotsBTService_Warmup` (AMÉLIORÉ)
5. `BotsBTService_Building` (EXISTANT)
6. `BotsBTService_Healing` (EXISTANT)
7. `BotsBTService_Loot` (MODIFIÉ - sans téléport)
8. `BotsBTService_AIDropZone` (MODIFIÉ - sans téléport)

### Enums Ajoutés
- `EBotSoundType`
- `EBotCombatStance`

### Enums Modifiés
- `EBotPersonality` : Ajout de Tactical
- `EBotWarmupChoice` : Ajout de SocialGroup, ExploreSpawnIsland, DanceInGroup
- `EBotState` : Ajout de InvestigatingSound

---

## Utilisation

### Spawn de Bots
```cpp
// Spawn standard
Bots::SpawnPlayerBot();

// Spawn sur toit
Bots::SpawnPlayerBot(nullptr, PlayerBots::EBotState::Warmup, true);

// Spawn avec état spécifique
Bots::SpawnPlayerBot(CustomLocation, PlayerBots::EBotState::Landed, false);
```

### Distribution Recommandée
```cpp
for (int i = 0; i < 10; i++) {
    bool onRoof = (i % 3 == 0);  // 33% sur toits
    Bots::SpawnPlayerBot(nullptr, PlayerBots::EBotState::Warmup, onRoof);
}
```

---

## Notes Techniques

### Performance
- Détection sonore toutes les 5 ticks (optimisé)
- Vérification de blocage toutes les 3 secondes
- Cooldown de construction basé sur skill (0.05s - 0.15s)

### Compatibilité
- Rétrocompatible avec les systèmes existants
- Pas de modifications requises dans d'autres fichiers
- Les bots existants continuent de fonctionner

---

## Future Améliorations Possibles

1. **Système d'Apprentissage** : Bots qui apprennent des patterns joueurs
2. **Communication d'Équipe** : Coordination entre bots alliés
3. **Édition de Construction** : Utilisation avancée de l'outil d'édition
4. **Gestion des Ressources** : Collecte automatique quand stocks bas
5. **Adaptation Dynamique** : Ajustement du comportement selon la phase de jeu

---

**Date de Mise à Jour** : Février 2026
**Version** : 2.0
**Auteur** : Système d'Amélioration IA
