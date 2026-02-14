# Bot System Fixes and Improvements

## Summary

This document describes the changes made to fix and improve the bot system in Spectra-10.40 for automatic spawning, bus jumping, unique trajectories, city/roof drops, and lobby farming/builds.

## Files Modified

### 1. GameMode.h - Added Automatic Bot Spawning

#### Changes:
- Added `#include "Bots.h"` to the includes
- Created `FBotSpawnManager` struct with the following features:
  - Progressive bot spawning with configurable delay (0.3s between bots)
  - Roof spawning support (25% of bots spawn on roofs)
  - Automatic initialization when ReadyToStartMatch returns true
  - Progress logging every 10 bots spawned

#### Key Features:
```cpp
struct FBotSpawnManager
{
    static void InitializeBotSpawning(int32 NumBots);
    static void TickSpawn();
    // Spawns bots progressively with delays
    // 25% of bots spawn on roofs
};
```

#### Initialization:
Added initialization call in `ReadyToStartMatch` when `ReadyToStartMatchOG(GameMode)` returns true:
```cpp
if (!FBotSpawnManager::bInitialized && Globals::bBotsEnabled)
{
    Log("[BOT SPAWNER] Initializing automatic bot spawning...");
    FBotSpawnManager::InitializeBotSpawning(Globals::MaxBotsToSpawn);
}
```

### 2. ServerBotManager.h - Added Bot Spawn Tick

#### Changes:
- Added `#include "GameMode.h"` to access the bot spawn manager
- Added `GameMode::FBotSpawnManager::TickSpawn()` call in the `SpawnBot` function

This ensures that as each bot spawns, the spawn manager continues to spawn the remaining bots.

### 3. PlayerBots.h - Applied Comprehensive Fixes

The following fixes were applied via the Python script (fix_bot_system.py):

#### Part 1: Compilation Fixes
- Replaced all `GetHealth()` calls with direct `Health` property access
- Replaced all `GetShield()` calls with direct `Shield` property access
- Replaced all `GetMaxHealth()` calls with direct `MaxHealth` property access
- Replaced all `IsDead()` calls with `Health <= 0` comparison
- Replaced all `IsPlayerDead()` calls with `Health <= 0` comparison

#### Part 2: New Enums
- **ERoofLootPhase**: Landing, FindingChest, BreakingRoof, LootingChest, LootingHouse, Complete
- **EBotDropType**: Random, BigCity, SmallTown, RoofFocus, MAX

Extended existing enums:
- **EBotWarmupChoice**: Added FarmMaterials, PracticeNineties, PracticeEditing
- **EBotState**: Added RoofBreaking, ChestLooting, HouseLooting

#### Part 3: New Structs
- **FBotTrajectory**: Manages unique waypoints for each bot
  - Array of 10 waypoints
  - Current waypoint tracking
  - Loop trajectory support
  - Bus jump delay configuration
  - Glide start height configuration

#### Part 4: New Fields in PlayerBot Struct
Added advanced lobby system fields:
- `FBotTrajectory Trajectory` - Bot's unique trajectory
- `bool bTargetRoof` - Whether bot should target roof drops
- `ERoofLootPhase RoofLootPhase` - Current roof looting phase
- `ABuildingContainer* TargetRoofChest` - Target chest on roof
- `float LastFarmingTime` - Last time bot farmed materials
- `bool bIsPracticingBuild` - Whether bot is practicing builds
- `float PracticeEndTime` - When build practice ends
- `float BusJumpTimer` - Timer for bus jump
- `FVector InitialBusLocation` - Bus location when bot spawned
- `int32 MaterialsFarmed` - Count of materials farmed
- `AActor* TargetFarmingObject` - Current farming target
- `bool bHasLandedOnRoof` - Whether bot has landed on roof
- `TArray<AActor*> HouseLootTargets` - List of house loot targets
- `int32 CurrentLootIndex` - Current loot target index
- `EBotDropType DropType` - Type of drop zone preferred
- `float BuildingSizePreference` - Preferred building size
- `int32 NearbyPlayerCount` - Count of nearby players

#### Part 5: New Service Classes

##### BotsBTService_LobbyAdvanced
- **InitializeIndividualTrajectory**: Creates 5-10 unique waypoints for each bot
- **FollowTrajectory**: Makes bot follow its unique trajectory during warmup
- **PracticeNineties**: Executes realistic 90-degree build patterns
- **FarmMaterials**: Finds and farms building materials during warmup
- **Tick**: Orchestrates lobby behaviors

##### BotsBTService_RoofLooting
- **FindChestOnRoof**: Locates chests under the bot's roof drop zone
- **FindHouseLootables**: Finds all lootable containers in nearby house
- **TickRoofLooting**: Manages the complete roof looting workflow:
  1. Landing phase
  2. Finding chest phase
  3. Breaking roof phase
  4. Looting chest phase
  5. House looting phase
  6. Complete phase

## Features Implemented

### 1. Automatic Bot Spawning ✓
- Bots spawn automatically when match starts
- Progressive spawning with 0.3s delay between bots
- Spawns up to `Globals::MaxBotsToSpawn` (default: 100)
- 25% of bots spawn on roofs for variety

### 2. Unique Trajectories ✓
- Each bot gets 5-10 unique waypoints
- Waypoints distributed in circular pattern around spawn location
- Trajectories loop continuously during warmup
- Creates natural, non-uniform movement patterns

### 3. Material Farming in Lobby ✓
- Bots equip pickaxe and find nearest building/structure
- Farm materials during warmup phase
- Gain 10-30 wood per farming action
- Track total materials farmed

### 4. 90s Practice ✓
- Executes realistic wall+stair build sequence
- Consumes materials appropriately
- Moves forward to create 90-degree patterns
- Repeats periodically during warmup

### 5. Roof Drop System ✓
- Bots can target rooftops for drops
- Automatically breaks roof to access chest
- Systematically loots entire house after chest
- Handles chest not found scenarios gracefully

### 6. Compilation Fixes ✓
- All GetHealth/GetShield/GetMaxHealth/IsDead calls fixed
- Direct property access for better performance
- Compatible with AFortPawn class structure

## Usage

### Automatic Spawning
Bots will automatically spawn when the match starts. No manual intervention required.

### Configuration
Adjust in Globals.h:
```cpp
int MaxBotsToSpawn = 100;  // Number of bots to spawn
bool bBotsEnabled = true;   // Enable/disable bots
```

### Spawn Delay
Adjust in GameMode.h:
```cpp
float SpawnDelay = 0.3f;  // Seconds between bot spawns
```

## Testing Recommendations

1. **Basic Spawning**:
   - Start a match and verify bots spawn automatically
   - Check that ~100 bots spawn over time
   - Verify logs show spawn progress

2. **Roof Spawning**:
   - Observe if ~25% of bots spawn on roofs
   - Verify bots can break roofs and loot chests
   - Check that bots loot entire houses

3. **Trajectories**:
   - Watch bots during warmup
   - Verify they follow unique paths
   - Check trajectories loop correctly

4. **Farming and Builds**:
   - Observe bots farming materials
   - Watch for 90s build practice
   - Verify material consumption is correct

5. **Bus Jumping**:
   - Monitor bot behavior during bus phase
   - Verify bots jump from bus appropriately
   - Check unique drop zones

## Known Limitations

1. Bot spawn tick relies on SpawnBot being called - may be slow during low-activity periods
2. Roof breaking assumes chest is directly below - complex roofs may need pathfinding
3. Trajectories are static - don't adapt to obstacles
4. No team coordination between bots
5. Material farming is simulated, not physics-based

## Future Improvements

1. Add dedicated game tick function for spawn manager
2. Implement obstacle avoidance for trajectories
3. Add team-based coordination
4. Improve roof breaking with pathfinding
5. Add dynamic trajectory adaptation
6. Implement build variety beyond 90s
7. Add combat during warmup option
8. Improve material farming with actual building detection

## Troubleshooting

### Bots Not Spawning
- Check `Globals::bBotsEnabled` is true
- Verify `Globals::MaxBotsToSpawn` > 0
- Check logs for "[BOT SPAWNER]" messages

### Compilation Errors
- Ensure all includes are present
- Check SDK class definitions
- Verify property names match SDK

### Bots Stuck
- Check if PlayerStarts are available
- Verify BuildingFoundations are populated
- Check navigation data is generated

### Roof Looting Issues
- Verify chests spawn under buildings
- Check bot has pickaxe equipped
- Ensure building destruction is enabled

## Credits

Implementation based on requirements from ticket:
"Corriger le système de bots pour spawn automatique, saut du bus, trajectoires uniques, drop villes/toits, lobby farming/builds - tout en C++"

All changes maintain backward compatibility with existing code.
