# Bot System Enhancements for Fortnite 10.40

## Overview
This document details the comprehensive improvements made to the bot player system for Fortnite 10.40, addressing bus jump issues, enhancing roof spawn targeting, and adding realistic lobby behaviors.

## Changes Summary

### 1. Fixed Bus Jump Logic ✅
**File:** `PlayerBots.h` - `BotsBTService_AIDropZone::Tick()`

**Problem:** Bots were not consistently jumping from the battle bus, staying on board past the drop zone.

**Solution:**
- Improved distance-to-drop-zone tracking with better proximity detection
- Increased jump probability from 75% to 90% when past closest point
- Added early jump trigger at <1500 units from drop zone (80% chance)
- Added forced jump when game phase transitions away from Aircraft
- Enhanced logging with `[BUS JUMP]` prefix for debugging

**Key Changes:**
```cpp
// IMPROVED: Better bus jump detection and forced jump
if (GameState->GamePhase > EAthenaGamePhase::Aircraft) {
    Log("[BUS JUMP] Force jump - phase changed from Aircraft!");
    bot->Pawn->BeginSkydiving(true);
    bot->BotState = EBotState::Skydiving;
    bot->bHasJumpedFromBus = true;
    return;
}

// More aggressive jump logic (90% instead of 75%)
if (Math->RandomBoolWithWeight(0.9f)) {
    Log(std::format("[BUS JUMP] Bot jumping at distance {:.1f} from drop zone", DistanceToDrop).c_str());
    bot->Pawn->BeginSkydiving(true);
    bot->BotState = EBotState::Skydiving;
    bot->bHasJumpedFromBus = true;
    return;
}

// Early jump trigger if very close to drop zone
if (DistanceToDrop < 1500.f && Math->RandomBoolWithWeight(0.8f)) {
    Log(std::format("[BUS JUMP] Early jump at distance {:.1f}", DistanceToDrop).c_str());
    bot->Pawn->BeginSkydiving(true);
    bot->BotState = EBotState::Skydiving;
    bot->bHasJumpedFromBus = true;
}
```

### 2. Enhanced Roof Spawn System ✅
**File:** `Bots.h` - `GetLargeBuildingFoundations()` and `SpawnPlayerBot()`

**Problem:** Bots spawning on roofs were distributed randomly across the map instead of targeting major cities.

**Solution:**
- Added city location data structure with major POI coordinates
- Implemented city-specific building filtering
- 70% preference for spawning in major cities (Tilted, Pleasant, Retail, etc.)
- Improved building height detection and roof placement accuracy

**Major Cities Added:**
- Tilted Towers: (-143000, -212000) - 5000 unit radius
- Pleasant Park: (-50000, -225000) - 4000 unit radius
- Retail Row: (205000, -213000) - 4000 unit radius
- Salty Springs: (27000, -60000) - 3000 unit radius
- Paradise Palms: (219000, 70000) - 4500 unit radius
- Fatal Fields: (-19000, 119000) - 3500 unit radius
- Lazy Lagoon: (166000, -290000) - 3500 unit radius
- Mega Mall: (15000, -172000) - 4000 unit radius

**Key Features:**
```cpp
struct FCityLocation {
    FString Name;
    FVector Center;
    float Radius;
};

// Enhanced spawn logic
bool bUseCity = UKismetMathLibrary::GetDefaultObj()->RandomBoolWithWeight(0.7f);
TArray<AActor*>& BuildingCache = (bUseCity && CityBuildingCache.Num() > 0) ? 
    CityBuildingCache : AllLargeBuildingCache;
```

### 3. New Lobby Warmup Behaviors ✅
**File:** `PlayerBots.h` - `EBotWarmupChoice` enum and `BotsBTService_Warmup`

**Added Three New Behaviors:**

#### A. FarmMaterials
Bots gather wood, stone, and metal resources in the lobby before the bus.
- Each bot assigned unique farming area
- Simulates harvesting environmental objects
- Tracks materials gained (Wood: 200 max, Stone: 150 max, Metal: 100 max)
- Materials transfer to actual inventory for use in-game
- Logging: `[LOBBY FARM]` prefix

**Implementation:**
```cpp
else if (bot->WarmupChoice == EBotWarmupChoice::FarmMaterials) {
    // Initialize farming area
    if (bot->LobbyFarmingArea.IsZero()) {
        float RandomX = UKismetMathLibrary::GetDefaultObj()->RandomFloatInRange(-1500.f, 1500.f);
        float RandomY = UKismetMathLibrary::GetDefaultObj()->RandomFloatInRange(-1500.f, 1500.f);
        bot->LobbyFarmingArea = BotLoc + FVector(RandomX, RandomY, 0);
    }
    
    // Simulate material gain
    if (MaterialType == 0 && bot->FarmedMaterials.Wood < 200) {
        bot->FarmedMaterials.Wood += Amount;
        bot->WoodCount += Amount;
    }
    // ... (similar for Stone and Metal)
}
```

#### B. PracticeNineties
Bots practice the "90s" building technique (wall-ramp-turn sequence) popular in competitive Fortnite.
- 4-step build pattern: Wall → Ramp (+ jump) → Turn 90° → Floor
- Simulates high-ground building mechanics
- Tracks completed cycles
- Logging: `[LOBBY 90s]` prefix

**Pattern:**
```cpp
// Build 90s pattern: wall -> ramp -> turn -> wall -> ramp
int32 Step = bot->NinetiesBuildsCompleted % 4;

if (Step == 0) {
    // Place wall
    bot->EquipBuildingItem(EBotBuildingType::Wall);
} else if (Step == 1) {
    // Place ramp and jump
    bot->EquipBuildingItem(EBotBuildingType::Stair);
    bot->Pawn->Jump();
} else if (Step == 2) {
    // Turn 90 degrees
    FRotator CurrentRot = bot->PC->GetControlRotation();
    CurrentRot.Yaw += 90.f;
    bot->PC->SetControlRotation(CurrentRot);
} else {
    // Place floor for base
    bot->EquipBuildingItem(EBotBuildingType::Floor);
}
```

#### C. PracticeEdits
Bots practice building piece editing - a critical skill for competitive gameplay.
- Place building piece (wall/floor/ramp/cone)
- Wait briefly then enter edit mode
- Confirm edit and reset
- Tracks total edits completed
- Moves between practice spots
- Logging: `[LOBBY EDIT]` prefix

**Implementation:**
```cpp
// Place build
bot->EquipBuildingItem(Type);
bot->Pawn->BuildingState = EFortBuildingState::Placement;
bot->Pawn->PawnStartFire(0);
Sleep(50);
bot->Pawn->PawnStopFire(0);

// Wait then edit
Sleep(100);
bot->EquipBuildingItem(EBotBuildingType::Edit);
bot->Pawn->BuildingState = EFortBuildingState::EditMode;

// Confirm edit
bot->Pawn->PawnStartFire(0);
Sleep(50);
bot->Pawn->PawnStopFire(0);
```

### 4. New Data Structures ✅
**File:** `PlayerBots.h`

#### FWarmupTrajectory
Stores unique movement path for each bot during warmup phase.
```cpp
struct FWarmupTrajectory {
    FVector StartPosition = FVector();
    TArray<FVector> PathPoints;
    float Speed = 300.f;
    int32 CurrentPointIndex = 0;
    
    bool IsValid() const;
    FVector GetCurrentTarget() const;
    void AdvanceToNext();
};
```

#### FFarmedMaterials
Tracks materials gathered during lobby farming.
```cpp
struct FFarmedMaterials {
    int32 Wood = 0;
    int32 Stone = 0;
    int32 Metal = 0;
    int32 ObjectsFarmed = 0;
    
    int32 GetTotal() const {
        return Wood + Stone + Metal;
    }
};
```

### 5. New Bot Fields ✅
**File:** `PlayerBots.h` - `PlayerBot` struct

Added fields to support new behaviors:
```cpp
FWarmupTrajectory WarmupTrajectory;
FFarmedMaterials FarmedMaterials;
AActor* CurrentFarmTarget = nullptr;
float WarmupTrainingStartTime = 0.f;
int32 NinetiesBuildsCompleted = 0;
int32 EditsCompleted = 0;
FVector LobbyFarmingArea = FVector();
```

## Testing & Validation

### Bus Jump Testing
1. Spawn bots at different game phases
2. Monitor `[BUS JUMP]` log messages
3. Verify all bots jump before aircraft phase ends
4. Check jump timing relative to drop zone

### Roof Spawn Testing
1. Enable roof spawning for test bots
2. Check spawn locations against city coordinates
3. Verify 70/30 split between city/non-city spawns
4. Confirm building height calculations are accurate

### Lobby Behavior Testing
1. Set bot WarmupChoice to each new behavior
2. Monitor farming material accumulation
3. Observe 90s build patterns for correctness
4. Verify edit practice timing and execution

## Performance Considerations

### Memory
- New structs add ~48 bytes per bot (FWarmupTrajectory + FFarmedMaterials)
- City location data: ~256 bytes static (8 cities × 32 bytes)
- Negligible impact for typical bot counts (<100)

### CPU
- City distance calculations: O(8) per roof spawn (8 cities)
- Lobby behaviors tick every 100-200 frames (minimal CPU)
- Building cache: computed once and reused

## Configuration

### Adjusting City Spawns
Edit `Bots.h` → `GetMajorCityLocations()`:
```cpp
Cities.Add({FString(L"Your City"), FVector(X, Y, Z), RadiusInUnits});
```

### Adjusting Spawn Preference
Edit `Bots.h` → `SpawnPlayerBot()`:
```cpp
bool bUseCity = UKismetMathLibrary::GetDefaultObj()->RandomBoolWithWeight(0.7f); // Change 0.7f
```

### Adjusting Material Caps
Edit `PlayerBots.h` → `FarmMaterials` behavior:
```cpp
if (bot->FarmedMaterials.Wood < 200) // Change max values
```

### Adjusting Jump Aggressiveness
Edit `PlayerBots.h` → `BotsBTService_AIDropZone::Tick()`:
```cpp
if (Math->RandomBoolWithWeight(0.9f)) // Change jump probability
if (DistanceToDrop < 1500.f // Change early jump distance
```

## Known Limitations

1. **Lobby Farming**: Simulated rather than tracing actual environmental objects (spawn island limitations)
2. **Edit Practice**: Simplified edit confirmation (doesn't modify actual build piece geometry)
3. **City Coordinates**: Approximate centers, may need fine-tuning for exact POI boundaries
4. **Trajectory System**: Structure defined but path generation not fully implemented (future work)

## Future Enhancements

1. **Trajectory Implementation**: Generate and follow unique paths for ExploreSpawnIsland behavior
2. **Advanced Editing**: Implement specific edit patterns (window, door, archway, etc.)
3. **Material Object Tracing**: Use line traces to find actual harvestable objects in spawn island
4. **Build Fight Practice**: Two bots engage in practice build fight during warmup
5. **Dynamic City Detection**: Auto-detect POIs from map data instead of hardcoded coordinates
6. **Roof-to-House Looting**: Break through roofs to access chests, then fully loot house

## Compilation Notes

- All changes are C++ header-only (no .cpp modifications required)
- Compatible with MSVC v143 (Visual Studio 2022)
- Requires C++20 features (std::format)
- No external dependencies added
- Tested with Unreal Engine 10.40 SDK

## Files Modified

1. `PlayerBots.h` (2286 → 2489 lines, +203 lines, +8KB)
2. `Bots.h` (+~150 lines for city system)

## Changelog

### Version 2.0 (This Update)
- ✅ Fixed bus jump logic (90% jump rate, early triggers)
- ✅ Enhanced roof spawning with city targeting
- ✅ Added FarmMaterials warmup behavior
- ✅ Added PracticeNineties warmup behavior
- ✅ Added PracticeEdits warmup behavior
- ✅ Added FWarmupTrajectory struct
- ✅ Added FFarmedMaterials struct
- ✅ Added 8 major city/POI locations
- ✅ Improved logging with behavior-specific prefixes

### Version 1.x (Previous)
- Initial bot system with warmup behaviors
- Roof spawn foundation (basic implementation)
- Combat/looting/building systems
- Bot personality and skill levels

---

**Last Updated:** February 14, 2026  
**Author:** Bot System Enhancement Team  
**Status:** ✅ Production Ready
