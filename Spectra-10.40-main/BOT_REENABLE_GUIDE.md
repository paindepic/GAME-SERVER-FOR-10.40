# Quick Guide: Re-enabling Bots After Testing

## Current Status
✅ Bots are DISABLED for initial crash testing
✅ All crash fixes have been implemented
✅ Safety checks are in place

## How to Re-enable Bots

### Option 1: Re-enable with Full Bot Count (Recommended if Step 1 succeeds)

**File:** `10.40/Globals.h`
**Line:** 10

```cpp
// Change from:
bool bBotsEnabled = false;

// To:
bool bBotsEnabled = true;
```

**Expected behavior:**
- Up to 100 bots will spawn (MaxBotsToSpawn in Globals.h)
- Spawn delay: 0.5 seconds per bot
- Progressive spawning with roof spawns enabled

---

### Option 2: Re-enable with Reduced Bot Count (For cautious testing)

**Step 1:** Enable bots in `10.40/Globals.h`
```cpp
bool bBotsEnabled = true;
```

**Step 2:** Limit bot count in `10.40/GameMode.h`
**Line:** 27 (in InitializeBotSpawning function)

```cpp
// Change from:
BotsToSpawn = NumBots;

// To:
BotsToSpawn = std::min(NumBots, 20);  // Limit to 20 bots for stability testing
```

**Step 3:** Gradually increase if stable
```cpp
BotsToSpawn = std::min(NumBots, 50);  // Test with 50 bots
// Then:
BotsToSpawn = NumBots;  // Full count when confirmed stable
```

---

## Configuration Options

### Adjust Spawn Rate (if needed)
**File:** `10.40/GameMode.h`
**Lines:** 29 and 79

```cpp
// Slower spawning (more stable, but slower match start):
SpawnDelay = 1.0f;  // 1 second between spawns

// Current setting:
SpawnDelay = 0.5f;  // 0.5 seconds between spawns

// Faster spawning (less stable, quicker match start):
SpawnDelay = 0.3f;  // 0.3 seconds between spawns
```

### Adjust Total Bot Count
**File:** `10.40/Globals.h`
**Line:** 12

```cpp
int MaxBotsToSpawn = 100;  // Default (adjust as needed)
```

---

## What to Monitor

### Successful Operation
Look for these log messages:
```
[BOT SPAWNER] Initialized spawning 100 bots
[BOT SPAWNER] Spawned 10/100 bots
[BOT SPAWNER] Spawned 20/100 bots
...
[BOT SPAWNER] All 100 bots spawned successfully!
[BOT DROP] Bot jumped from bus forcefully!
```

### Issues to Watch For
```
[CRASH FIX] GameMode is null!
[CRASH FIX] PlayerBot is null in OnAircraftExitedDropZone!
[CRASH FIX] PlayerBot->Pawn is null in OnAircraftExitedDropZone!
```

If you see these, the safety checks are working - preventing crashes.

---

## Testing Checklist

- [ ] **Test 1:** Run server with `bBotsEnabled = false`
  - Server starts successfully ✓
  - Player can join ✓
  - Match starts without crashing ✓

- [ ] **Test 2:** Run server with `bBotsEnabled = true` and 20 bots
  - Server starts successfully ✓
  - Bots spawn progressively ✓
  - Bots jump from bus when exiting drop zone ✓
  - No crashes during match ✓

- [ ] **Test 3:** Run server with `bBotsEnabled = true` and 50 bots
  - (Same checks as Test 2)

- [ ] **Test 4:** Run server with `bBotsEnabled = true` and 100 bots
  - (Same checks as Test 2)

---

## Troubleshooting

### If server still crashes with bots enabled:

1. **Check the crash point:**
   - Look at the last log message before the crash
   - Check which safety check triggered

2. **Reduce bot count:**
   - Try with only 10 bots first
   - Gradually increase

3. **Increase spawn delay:**
   - Set `SpawnDelay = 1.0f` or higher

4. **Review SDK definitions:**
   - Verify `bIsSkydiving` field in AFortPlayerPawnAthena
   - Verify `bInAircraft` field in AFortPlayerStateAthena

5. **Check for memory issues:**
   - System may not handle 100 bots well
   - Consider reducing to 50-75 bots

### If bots don't jump from bus:

1. Check if aircraft is properly detected
2. Verify bots are actually in the aircraft (bInAircraft)
3. Check logs for "[BOT DROP] No bots to process"

### If bots spawn but don't behave correctly:

This is likely not related to the crash fixes and may require:
- AI behavior tree adjustments
- Skill level tuning
- Additional bot logic improvements

---

## Summary

The crash fixes focus on:
1. ✅ Null pointer checks (prevents dereferencing invalid pointers)
2. ✅ State validation (only acts on bots in valid states)
3. ✅ Aircraft membership check (only teleports bots actually in aircraft)
4. ✅ Increased spawn delay (reduces race conditions)
5. ✅ Debug logging (helps identify issues)

Once you confirm the server works without bots, re-enable them and test progressively!
