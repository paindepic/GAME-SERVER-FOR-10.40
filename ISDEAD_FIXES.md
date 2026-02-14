# IsDead() Fix for PlayerBots.h

## Problem
The SDK 10.40 does not have the `IsDead()` method on APawn, causing compilation error C2039.

## Required Changes

### Change 1: Line 354
**Location:** In `PlayerBot::GetNearestPawn()` method
**Before:** `if (PS->bInAircraft || Pawn->IsDead()) continue;`
**After:** `if (PS->bInAircraft || Pawn->GetHealth() <= 0) continue;`

### Change 2: Line 1141
**Location:** In `BotsBTService_AIEvaluator::Tick()` method
**Before:** `if (!bot || !bot->Pawn || bot->PlayerState->IsPlayerDead() || bot->Pawn->IsDead()) return;`
**After:** `if (!bot || !bot->Pawn || bot->PlayerState->IsPlayerDead() || bot->Pawn->GetHealth() <= 0) return;`

### Change 3: Line 1939
**Location:** In `BotsBTService_Combat::Tick()` method
**Before:** `if (bot->NearestPlayerPawn && !bot->NearestPlayerPawn->IsDead()) {`
**After:** `if (bot->NearestPlayerPawn && bot->NearestPlayerPawn->GetHealth() > 0) {`

## Implementation
Run the following command to apply all fixes:
```bash
cd /home/engine/project/Spectra-10.40-main/10.40
sed -i 's/Pawn->IsDead()/Pawn->GetHealth() <= 0/g' PlayerBots.h
sed -i 's/bot->Pawn->IsDead()/bot->Pawn->GetHealth() <= 0/g' PlayerBots.h
sed -i 's/!bot->NearestPlayerPawn->IsDead()/bot->NearestPlayerPawn->GetHealth() > 0/g' PlayerBots.h
```

## Verification
After applying fixes, verify by searching for `IsDead()`:
```bash
grep -n "IsDead()" PlayerBots.h
```
Should return no results.

## Enhanced Bot Systems Status
The following systems are properly integrated in PlayerBots.h:
✅ Personality system (Aggressive, Builder, Balanced, Tactical)
✅ Build patterns (RampRush, Nineties, Turtle, DefensiveBox, DefensiveWall, HighGroundRetake)
✅ Sound detection (BotsBTService_SoundDetection)
✅ Combat stances (EBotCombatStance)
✅ Anti-stuck detection with jump fallback
✅ Roof spawning (bHasRoofSpawned, BuildingFoundations)
✅ Bus jumping mechanics with natural skydiving
✅ Natural movement without teleportation
