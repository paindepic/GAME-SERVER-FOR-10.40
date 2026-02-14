# 🧪 Testing Guide - Crash Fixes & Roof Spawn System

## Quick Start Test

### 1. Basic Compilation Test
```bash
# Build the DLL with Visual Studio or your build system
# Expected: Zero compilation errors
# Expected: Zero warnings related to null pointer usage
```

### 2. Immediate Server Stability Test
```cpp
// In your server initialization:
for (int i = 0; i < 20; i++) {
    Bots::SpawnPlayerBot(nullptr, PlayerBots::EBotState::Warmup, false);
}
// Expected: All 20 bots spawn successfully
// Expected: No crashes during warmup phase
```

### 3. Roof Spawn Visual Test
```cpp
// Spawn bots on roofs:
for (int i = 0; i < 10; i++) {
    Bots::SpawnPlayerBot(nullptr, PlayerBots::EBotState::Bus, true); // bSpawnOnRoof = true
}

// Check logs for:
// [ROOF SPAWN] Found X large building foundations out of Y
// [ROOF SPAWN] Bot spawning on roof at X=..., Y=..., Z=...
```

### 4. Bus & Drop Zone Test
```cpp
// Start match with bots
// Wait for aircraft phase
// Expected: Bots jump from bus without crashes
// Check logs for:
// [BOT DROP] Bot jumped from bus forcefully!
// [DROP ZONE] Roof bot targeting large building roof at Z=...
```

---

## Comprehensive Test Suite

### Test 1: Null Safety - GameMode

**Objective**: Verify GameMode null checks prevent crashes

**Steps**:
1. Start server
2. Monitor logs during match start
3. Check for premature GameState/GameMode access

**Expected Results**:
- No crashes if GameState is temporarily null
- Logs show: `[CRASH FIX] GameState is null in...` (should be rare/never)
- Server continues running

**Pass Criteria**: ✅ Zero crashes from null GameMode/GameState

---

### Test 2: Null Safety - Bot Components

**Objective**: Ensure bot component validation works

**Steps**:
1. Spawn 50+ bots
2. Force kill some bots mid-tick
3. Monitor for crashes during death handling

**Expected Results**:
- Logs show: `[CRASH FIX] Bot has null components - skipping tick!`
- Dead bots removed from PlayerBotArray cleanly
- No EXCEPTION_ACCESS_VIOLATION

**Pass Criteria**: ✅ Bots die gracefully without crashes

---

### Test 3: Aircraft Null Check

**Objective**: Verify aircraft validation prevents crashes

**Steps**:
1. Start match with 30 bots
2. Monitor OnAircraftExitedDropZone execution
3. Check for aircraft access errors

**Expected Results**:
- Logs show: `OnAircraftExitedDropZone!`
- No crashes when aircraft exits drop zone
- All bots jump successfully or log: `[CRASH FIX] Aircraft is null`

**Pass Criteria**: ✅ Zero crashes during bus exit phase

---

### Test 4: Large Building Detection

**Objective**: Verify roof spawn system finds appropriate buildings

**Steps**:
1. Load Athena map (default map)
2. Trigger match start (initializes BuildingFoundations)
3. Check logs immediately after:

**Expected Results**:
```
[ROOF SPAWN] Found 8-15 large building foundations out of 300-600
```

**Analysis**:
- If **0 found**: Map may not have large buildings or footprint threshold too high
- If **8-15 found**: ✅ Correct (Tilted, Pleasant, Retail, etc.)
- If **50+ found**: Threshold too low, adjust to 10000.0f

**Pass Criteria**: ✅ 5-20 large buildings detected

---

### Test 5: Roof Spawn Placement

**Objective**: Bots spawn on building roofs, not ground

**Steps**:
1. Spawn 10 bots with `bSpawnOnRoof = true`
2. Use spectator camera to view spawn locations
3. Check bot Z-coordinates in logs

**Expected Results**:
- Logs show: `[ROOF SPAWN] Bot spawning on roof at ... Z=800-1500`
- Visual confirmation: Bots standing on rooftops
- Bots not falling through buildings

**Pass Criteria**: ✅ Bots physically on roofs (Z > 700)

---

### Test 6: Drop Zone Intelligence

**Objective**: Roof-spawned bots target large buildings from bus

**Steps**:
1. Spawn bots with `bSpawnOnRoof = true`
2. Start match (bots enter bus)
3. Watch bots jump and glide

**Expected Results**:
- Logs show: `[DROP ZONE] Roof bot targeting large building roof at Z=...`
- Bots glide toward tall structures (Tilted, Pleasant)
- Landing locations concentrate in major cities

**Pass Criteria**: ✅ 70%+ of bots land in/near major cities

---

### Test 7: Combat System Stability

**Objective**: Combat service doesn't crash on null enemies

**Steps**:
1. Spawn 40 bots (20 roof, 20 ground)
2. Let bots engage in combat naturally
3. Monitor for 5+ minutes of combat

**Expected Results**:
- No crashes during combat
- Logs may show: `[CRASH FIX] Null bot components in Combat::Tick!` (rare)
- Bots shoot, build, and fight normally

**Pass Criteria**: ✅ Zero combat-related crashes over 5 minutes

---

### Test 8: Building System (90s, Build Fighting)

**Objective**: Verify personalities and build patterns still work

**Steps**:
1. Spawn bots with roof spawn enabled
2. Force combat between bots
3. Observe building behavior

**Expected Results**:
- Builder bots execute 90s (rapid stair-turn-stair pattern)
- Aggressive bots ramp rush
- Tactical bots build defensive boxes
- No crashes during build execution

**Pass Criteria**: ✅ All build patterns execute without crashes

---

### Test 9: Long-Running Stability

**Objective**: Server runs for extended period without memory leaks or crashes

**Steps**:
1. Start server with 60 bots (30 roof, 30 ground)
2. Run 3 full matches (30+ minutes)
3. Monitor memory usage and performance

**Expected Results**:
- Memory usage stable (no leaks from building cache)
- No crashes over multiple matches
- Performance consistent (no degradation)

**Pass Criteria**: ✅ 3 matches complete without crashes

---

### Test 10: Mixed Spawn Modes

**Objective**: Roof and ground bots coexist and interact correctly

**Steps**:
1. Spawn 25 bots with `bSpawnOnRoof = true`
2. Spawn 25 bots with `bSpawnOnRoof = false`
3. Let both groups land and engage

**Expected Results**:
- Roof bots land in cities
- Ground bots land in spread-out locations
- Both groups find loot and engage in combat
- No behavioral conflicts

**Pass Criteria**: ✅ Mixed spawn works seamlessly

---

## Performance Benchmarks

### Memory Usage
**Baseline**: ~200-300 MB with 50 bots  
**With Caching**: +5-10 MB for LargeBuildingCache  
**Expected**: Negligible impact (<5% increase)

### CPU Usage
**Roof Detection**: ~1-2ms one-time cost at match start  
**Drop Zone Logic**: +0.1ms per bot (only during Bus phase)  
**Expected**: <1% overall performance impact

---

## Log Analysis

### Healthy Server Logs
```
[SPECTRA]: bSetupPlaylist = 1
[SPECTRA]: Playlist = Playlist_DefaultDuo
[SPECTRA]: Initialised Bots!
[SPECTRA]: [ROOF SPAWN] Found 12 large building foundations out of 487
[SPECTRA]: [ROOF SPAWN] Bot spawning on roof at X=1234.5, Y=5678.9, Z=1200.2 (Building Height: 800.0)
[SPECTRA]: [DROP ZONE] Roof bot targeting large building roof at Z=960.0
[SPECTRA]: OnAircraftEnteredDropZone Called!
[SPECTRA]: OnAircraftExitedDropZone!
[SPECTRA]: [BOT DROP] Bot jumped from bus forcefully!
[SPECTRA]: Freed a dead bot from the array!
```

### Problem Indicators
```
❌ [CRASH FIX] GameMode is null in TickBots!
   → Investigate: GameMode destroyed prematurely?

❌ [CRASH FIX] Null bot components in Combat::Tick!
   → Investigate: Bot death not cleaning up properly?

❌ [ROOF SPAWN] Found 0 large building foundations out of 500
   → Investigate: Footprint threshold too high OR map has no large buildings
```

---

## Debugging Tips

### Issue: Bots not spawning on roofs

**Diagnosis**:
```cpp
// Add debug log in Bots.h:
Log(std::format("BuildingFoundations.Num() = {}", BuildingFoundations.Num()).c_str());
Log(std::format("LargeBuildingCache.Num() = {}", LargeBuildingCache.Num()).c_str());
```

**Fixes**:
1. Lower footprint threshold: `GetLargeBuildingFoundations(5000.0f)`
2. Check `ReadyToStartMatch` populates `BuildingFoundations`
3. Verify map has `ABuildingFoundation` actors

---

### Issue: Crashes still occurring

**Diagnosis**:
1. Check crash log for exception address
2. Match address to function using `ImageBase + Offset`
3. Look for last `[CRASH FIX]` log before crash

**Fixes**:
- Add more null checks in identified function
- Validate pointers before `GetActorBounds()` calls
- Check array bounds before indexing

---

### Issue: Bots landing in wrong locations

**Diagnosis**:
```cpp
// Add debug logs in BotsBTService_AIDropZone::ChooseDropZone:
Log(std::format("bot->bHasRoofSpawned = {}", bot->bHasRoofSpawned).c_str());
Log(std::format("TargetDropZone: X={}, Y={}, Z={}", 
    bot->TargetDropZone.X, bot->TargetDropZone.Y, bot->TargetDropZone.Z).c_str());
```

**Fixes**:
- Verify `bHasRoofSpawned` flag set correctly in `SpawnPlayerBot`
- Check drop zone not overwritten in `Tick()`
- Increase target Z offset if bots landing below roofs

---

## Regression Testing

After each code change, re-run:
1. ✅ Test 1 (GameMode null safety)
2. ✅ Test 3 (Aircraft null check)
3. ✅ Test 7 (Combat stability)
4. ✅ Test 9 (Long-running stability)

**Full Suite**: Run all 10 tests before production deployment

---

## Acceptance Criteria

### Minimum for Production:
- ✅ Zero crashes over 3 consecutive matches (Test 9)
- ✅ All null checks functioning (Tests 1-3, 7)
- ✅ Roof spawn working (Tests 4-6)
- ✅ Build patterns preserved (Test 8)

### Gold Standard:
- ✅ 10/10 tests passing
- ✅ Clean logs (no unexpected CRASH FIX messages)
- ✅ Performance within 5% of baseline
- ✅ Bots landing in major cities 70%+ of time

---

## Quick Test Script

```cpp
// Paste this into your server initialization:

void TestCrashFixesAndRoofSpawn() {
    Log("========== STARTING COMPREHENSIVE TEST ==========");
    
    // Test 1: Normal spawns (should work)
    for (int i = 0; i < 10; i++) {
        Bots::SpawnPlayerBot(nullptr, PlayerBots::EBotState::Warmup, false);
    }
    Log("[TEST] Spawned 10 normal bots - checking for crashes...");
    
    // Test 2: Roof spawns (should work if large buildings exist)
    for (int i = 0; i < 10; i++) {
        Bots::SpawnPlayerBot(nullptr, PlayerBots::EBotState::Bus, true);
    }
    Log("[TEST] Spawned 10 roof bots - check logs for ROOF SPAWN messages");
    
    // Test 3: Mixed (should work)
    for (int i = 0; i < 20; i++) {
        bool bRoof = (i % 2 == 0);
        Bots::SpawnPlayerBot(nullptr, PlayerBots::EBotState::Bus, bRoof);
    }
    Log("[TEST] Spawned 20 mixed bots - verify no crashes");
    
    Log("========== TEST COMPLETE - CHECK LOGS ==========");
}
```

**Usage**: Call `TestCrashFixesAndRoofSpawn()` after `ReadyToStartMatch` completes

---

## Support

If tests fail or crashes persist:
1. Capture full `Spectra_log.txt`
2. Note which test failed
3. Include exception address if crash occurred
4. Check Discord/GitHub issues for similar problems

**Last Updated**: February 14, 2026  
**Version**: 1.0.0
