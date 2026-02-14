# Advanced AI Bot System Implementation - Status and Instructions

## Summary

This implementation aims to fix C++ compilation errors and add advanced AI bot features for Battle Royale lobby in Fortnite 10.40.

## Required Changes to PlayerBots.h

### PART 1: Compilation Fixes

The following method calls need to be replaced with direct property access:

1. **Replace GetHealth() calls:**
   - `bot->Pawn->GetHealth()` → `bot->Pawn->Health`
   - `Pawn->GetHealth()` → `Pawn->Health`

2. **Replace GetShield() calls:**
   - `bot->Pawn->GetShield()` → `bot->Pawn->Shield`
   - `Pawn->GetShield()` → `Pawn->Shield`

3. **Replace GetMaxHealth() calls:**
   - `bot->Pawn->GetMaxHealth()` → `bot->Pawn->MaxHealth`

4. **Replace IsDead() calls:**
   - `Pawn->IsDead()` → `Pawn->Health <= 0`
   - `Bot->Pawn->IsDead()` → `Bot->Pawn->Health <= 0`

### PART 2: New Enums to Add

#### 1. ERoofLootPhase Enum (after ELootableType enum)
```cpp
enum class ERoofLootPhase {
    Landing,
    FindingChest,
    BreakingRoof,
    LootingChest,
    LootingHouse,
    Complete
};
```

#### 2. EBotDropType Enum (after EBotBuildPattern enum)
```cpp
enum class EBotDropType {
    Random,
    BigCity,
    SmallTown,
    RoofFocus,
    MAX
};
```

#### 3. Extended EBotWarmupChoice Enum
Add these new values before MAX:
```cpp
FarmMaterials,
PracticeNineties,
PracticeEditing,
```

#### 4. Extended EBotState Enum
Add these new values before MAX:
```cpp
RoofBreaking,
ChestLooting,
HouseLooting,
```

### PART 3: New Structs to Add

#### FBotTrajectory Struct (after FSoundMemory struct)
```cpp
struct FBotTrajectory {
    FVector Waypoints[10];
    int32 CurrentWaypoint = 0;
    int32 NumWaypoints = 0;
    float WaypointReachedDist = 100.f;
    bool bLoopTrajectory = true;
    FVector BusExitLocation = FVector();
    float BusJumpDelay = 0.0f;
    float GlideStartHeight = 1500.0f;
    bool bHasUniquePath = false;

    FBotTrajectory() {
        CurrentWaypoint = 0;
        NumWaypoints = 0;
        WaypointReachedDist = 100.f;
        bLoopTrajectory = true;
        BusJumpDelay = 0.0f;
        GlideStartHeight = 1500.0f;
        bHasUniquePath = false;
        for (int i = 0; i < 10; i++) {
            Waypoints[i] = FVector::ZeroVector;
        }
    }
};
```

### PART 4: New Fields to Add to PlayerBot Struct

Add these fields after `bHasRoofSpawned`:
```cpp
// Trajectory system
FBotTrajectory Trajectory;
float BusJumpTimer = 0.0f;
FVector InitialBusLocation = FVector();

// Roof looting system
bool bTargetRoof = false;
ERoofLootPhase RoofLootPhase = ERoofLootPhase::Landing;
ABuildingContainer* TargetRoofChest = nullptr;
bool bHasLandedOnRoof = false;

// Material farming
float LastFarmingTime = 0.f;
int32 MaterialsFarmed = 0;
AActor* TargetFarmingObject = nullptr;

// Build practice
bool bIsPracticingBuild = false;
float PracticeEndTime = 0.f;

// House looting
TArray<AActor*> HouseLootTargets;
int32 CurrentLootIndex = 0;

// Dynamic drop zones
EBotDropType DropType = EBotDropType::Random;
float BuildingSizePreference = 0.0f;
int32 NearbyPlayerCount = 0;
```

### PART 5: New Service Classes to Add

#### BotsBTService_LobbyAdvanced Class

Add this class before BotsBTService_Loot:

```cpp
class BotsBTService_LobbyAdvanced {
public:
    void InitializeIndividualTrajectory(PlayerBot* bot) {
        if (!bot || !bot->Pawn) return;
        FVector StartLoc = bot->Pawn->K2_GetActorLocation();
        bot->Trajectory.NumWaypoints = UKismetMathLibrary::GetDefaultObj()->RandomIntegerInRange(5, 10);
        for (int i = 0; i < bot->Trajectory.NumWaypoints; i++) {
            float Angle = (360.f / bot->Trajectory.NumWaypoints) * i;
            float Radius = UKismetMathLibrary::GetDefaultObj()->RandomFloatInRange(500.f, 2000.f);
            float Rad = Angle * 3.14159f / 180.f;
            FVector Offset((float)cos(Rad) * Radius, (float)sin(Rad) * Radius, 0);
            bot->Trajectory.Waypoints[i] = StartLoc + Offset;
        }
        bot->Trajectory.CurrentWaypoint = 0;
    }

    void FollowTrajectory(PlayerBot* bot) {
        if (!bot || !bot->Pawn || !bot->PC) return;
        if (bot->Trajectory.NumWaypoints == 0) return;
        FVector Target = bot->Trajectory.Waypoints[bot->Trajectory.CurrentWaypoint];
        FVector BotLoc = bot->LastUpdatedBotLocation;
        float Dist = UKismetMathLibrary::GetDefaultObj()->Vector_Distance(BotLoc, Target);
        if (Dist < bot->Trajectory.WaypointReachedDist) {
            bot->Trajectory.CurrentWaypoint++;
            if (bot->Trajectory.CurrentWaypoint >= bot->Trajectory.NumWaypoints) {
                if (bot->Trajectory.bLoopTrajectory) {
                    bot->Trajectory.CurrentWaypoint = 0;
                } else {
                    InitializeIndividualTrajectory(bot);
                    return;
                }
            }
            Target = bot->Trajectory.Waypoints[bot->Trajectory.CurrentWaypoint];
        }
        bot->PC->MoveToLocation(Target, 50.f, false, true, false, true, nullptr, true);
        FRotator LookRot = UKismetMathLibrary::GetDefaultObj()->FindLookAtRotation(BotLoc, Target);
        bot->PC->SetControlRotation(LookRot);
    }

    void PracticeNineties(PlayerBot* bot) {
        if (!bot || !bot->Pawn) return;
        if (bot->WoodCount < 50) return;
        bot->EquipBuildingItem(EBotBuildingType::Wall);
        bot->Pawn->BuildingState = EFortBuildingState::Placement;
        bot->Pawn->PawnStartFire(0);
        Sleep(50);
        bot->Pawn->PawnStopFire(0);
        bot->WoodCount -= 10;
        Sleep(100);
        bot->EquipBuildingItem(EBotBuildingType::Stair);
        bot->Pawn->BuildingState = EFortBuildingState::Placement;
        bot->Pawn->PawnStartFire(0);
        Sleep(50);
        bot->Pawn->PawnStopFire(0);
        bot->WoodCount -= 10;
        FVector Forward = bot->Pawn->GetActorForwardVector();
        bot->PC->MoveToLocation(bot->LastUpdatedBotLocation + Forward * 100.f + FVector(0,0,200), 10.f, false, true, false, true, nullptr, true);
    }

    void FarmMaterials(PlayerBot* bot) {
        if (!bot || !bot->Pawn) return;
        bot->EquipPickaxe();
        TArray<AActor*> Actors;
        UGameplayStatics::GetDefaultObj()->GetAllActorsOfClass(UWorld::GetWorld(), ABuildingActor::StaticClass(), &Actors);
        AActor* Nearest = nullptr;
        float NearDist = 500.f;
        for (int i = 0; i < Actors.Num(); i++) {
            AActor* Actor = Actors[i];
            if (!Actor) continue;
            float Dist = Actor->GetDistanceTo(bot->Pawn);
            if (Dist < NearDist) {
                NearDist = Dist;
                Nearest = Actor;
            }
        }
        if (Nearest && NearDist < 300.f) {
            bot->LookAt(Nearest);
            bot->Pawn->PawnStartFire(0);
            Sleep(500);
            bot->Pawn->PawnStopFire(0);
            bot->MaterialsFarmed += UKismetMathLibrary::GetDefaultObj()->RandomIntegerInRange(10, 30);
            bot->WoodCount += UKismetMathLibrary::GetDefaultObj()->RandomIntegerInRange(10, 30);
            if (bot->WoodCount > 999) bot->WoodCount = 999;
        }
        Actors.Free();
    }

    void Tick(PlayerBot* bot) {
        if (!bot || bot->BotState != EBotState::Warmup) return;
        if (bot->Trajectory.NumWaypoints == 0 && bot->tick_counter == 0) {
            InitializeIndividualTrajectory(bot);
        }
        FollowTrajectory(bot);
        if (bot->tick_counter % 180 == 0) {
            int32 Action = UKismetMathLibrary::GetDefaultObj()->RandomIntegerInRange(0, 4);
            switch (Action) {
                case 0: PracticeNineties(bot); break;
                case 1: FarmMaterials(bot); break;
                case 2: bot->Pawn->Jump(); break;
                case 3: bot->Emote(); break;
                case 4: bot->Run(); break;
            }
        }
    }
};
```

#### BotsBTService_RoofLooting Class

Add this class after BotsBTService_LobbyAdvanced:

```cpp
class BotsBTService_RoofLooting {
public:
    ABuildingContainer* FindChestOnRoof(PlayerBot* bot) {
        if (!bot || !bot->Pawn) return nullptr;
        FVector RoofArea = bot->TargetDropZone;
        RoofArea.Z -= 800.f;
        TArray<AActor*> Chests;
        static auto ChestClass = StaticLoadObject<UClass>("/Game/Building/ActorBlueprints/Containers/Tiered_Chest_Athena.Tiered_Chest_Athena_C");
        UGameplayStatics::GetDefaultObj()->GetAllActorsOfClass(UWorld::GetWorld(), ChestClass, &Chests);
        ABuildingContainer* BestChest = nullptr;
        float BestDist = FLT_MAX;
        for (int i = 0; i < Chests.Num(); i++) {
            ABuildingContainer* Chest = (ABuildingContainer*)Chests[i];
            if (!Chest || Chest->bAlreadySearched) continue;
            FVector ChestLoc = Chest->K2_GetActorLocation();
            float Dist = UKismetMathLibrary::GetDefaultObj()->Vector_Distance(ChestLoc, RoofArea);
            if (Dist < 800.f && Dist < BestDist) {
                BestDist = Dist;
                BestChest = Chest;
            }
        }
        Chests.Free();
        return BestChest;
    }

    void FindHouseLootables(PlayerBot* bot) {
        bot->HouseLootTargets.Empty();
        TArray<AActor*> Lootables;
        UGameplayStatics::GetDefaultObj()->GetAllActorsOfClass(UWorld::GetWorld(), ABuildingContainer::StaticClass(), &Lootables);
        for (AActor* Lootable : Lootables) {
            if (!Lootable || Lootable->bHidden) continue;
            float Dist = Lootable->GetDistanceTo(bot->Pawn);
            if (Dist < 1500.0f) {
                bot->HouseLootTargets.Add(Lootable);
            }
        }
        Lootables.Free();
        bot->CurrentLootIndex = 0;
    }

    void TickRoofLooting(PlayerBot* bot) {
        if (!bot || !bot->Pawn || !bot->PC) return;
        if (!bot->bTargetRoof) return;
        FVector BotLoc = bot->LastUpdatedBotLocation;

        switch (bot->RoofLootPhase) {
            case ERoofLootPhase::Landing: {
                float Height = BotLoc.Z;
                float TargetHeight = bot->TargetDropZone.Z;
                if (fabs(Height - TargetHeight) < 200.f) {
                    bot->RoofLootPhase = ERoofLootPhase::FindingChest;
                }
                break;
            }
            case ERoofLootPhase::FindingChest: {
                ABuildingContainer* Chest = FindChestOnRoof(bot);
                if (Chest) {
                    bot->TargetRoofChest = Chest;
                    bot->RoofLootPhase = ERoofLootPhase::BreakingRoof;
                } else {
                    bot->RoofLootPhase = ERoofLootPhase::LootingHouse;
                }
                break;
            }
            case ERoofLootPhase::BreakingRoof: {
                if (bot->TargetRoofChest) {
                    FVector ChestLoc = bot->TargetRoofChest->K2_GetActorLocation();
                    FVector BotLoc2D(BotLoc.X, BotLoc.Y, 0);
                    FVector ChestLoc2D(ChestLoc.X, ChestLoc.Y, 0);
                    float Dist2D = UKismetMathLibrary::GetDefaultObj()->Vector_Distance(BotLoc2D, ChestLoc2D);
                    if (Dist2D < 100.f) {
                        bot->EquipPickaxe();
                        bot->LookAt(bot->TargetRoofChest);
                        bot->Pawn->PawnStartFire(0);
                        Sleep(1000);
                        bot->Pawn->PawnStopFire(0);
                        bot->RoofLootPhase = ERoofLootPhase::LootingChest;
                    } else {
                        bot->PC->MoveToLocation(FVector(ChestLoc.X, ChestLoc.Y, BotLoc.Z), 50.f, false, true, false, true, nullptr, true);
                    }
                }
                break;
            }
            case ERoofLootPhase::LootingChest: {
                if (bot->TargetRoofChest && !bot->TargetRoofChest->bAlreadySearched) {
                    bot->Pawn->bStartedInteractSearch = true;
                    bot->Pawn->OnRep_StartedInteractSearch();
                    Sleep(1500);
                    BuildingContainer::SpawnLoot(bot->TargetRoofChest);
                    bot->TargetRoofChest->bAlreadySearched = true;
                    bot->PickupAllItemsInRange(400.f);
                    bot->Pawn->bStartedInteractSearch = false;
                    bot->Pawn->OnRep_StartedInteractSearch();
                    bot->RoofLootPhase = ERoofLootPhase::LootingHouse;
                    FindHouseLootables(bot);
                } else {
                    bot->RoofLootPhase = ERoofLootPhase::LootingHouse;
                    FindHouseLootables(bot);
                }
                break;
            }
            case ERoofLootPhase::LootingHouse: {
                if (bot->CurrentLootIndex >= bot->HouseLootTargets.Num()) {
                    bot->RoofLootPhase = ERoofLootPhase::Complete;
                    bot->BotState = EBotState::Looting;
                    bot->HouseLootTargets.Empty();
                    return;
                }
                AActor* CurrentTarget = bot->HouseLootTargets[bot->CurrentLootIndex];
                if (!CurrentTarget || CurrentTarget->bHidden) {
                    bot->CurrentLootIndex++;
                    return;
                }
                FVector TargetLoc = CurrentTarget->K2_GetActorLocation();
                FVector BotLoc2 = bot->LastUpdatedBotLocation;
                float Dist = UKismetMathLibrary::GetDefaultObj()->Vector_Distance(BotLoc2, TargetLoc);
                if (Dist < 250.0f) {
                    ABuildingContainer* Container = Cast<ABuildingContainer>(CurrentTarget);
                    if (Container && !Container->bAlreadySearched) {
                        bot->LookAt(CurrentTarget);
                        if (!bot->Pawn->bStartedInteractSearch) {
                            bot->Pawn->bStartedInteractSearch = true;
                            bot->Pawn->OnRep_StartedInteractSearch();
                            bot->TimeToNextAction = UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld());
                        } else if (UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld()) - bot->TimeToNextAction >= 1.0f) {
                            BuildingContainer::SpawnLoot(Container);
                            Container->bAlreadySearched = true;
                            bot->Pawn->bStartedInteractSearch = false;
                            bot->Pawn->OnRep_StartedInteractSearch();
                            bot->TimeToNextAction = 0;
                            bot->CurrentLootIndex++;
                        }
                    } else {
                        bot->CurrentLootIndex++;
                    }
                } else {
                    bot->PC->MoveToActor(CurrentTarget, 100.0f, true, false, true, nullptr, true);
                }
                break;
            }
            case ERoofLootPhase::Complete:
                bot->BotState = EBotState::Looting;
                break;
        }
    }

    void Tick(PlayerBot* bot) {
        if (bot->BotState == EBotState::RoofBreaking ||
            bot->BotState == EBotState::ChestLooting ||
            bot->BotState == EBotState::HouseLooting) {
            TickRoofLooting(bot);
        }
    }
};
```

### PART 6: Integration in TickBots Function

In the TickBots function, add handling for new states:

```cpp
else if (Bot->BotState == EBotState::Landed) {
    if (Bot->bHasRoofSpawned && !Bot->bHasLandedOnRoof) {
        Bot->BotState = EBotState::RoofBreaking;
    } else {
        Bot->BotState = EBotState::Looting;
    }
}
else if (Bot->BotState == EBotState::RoofBreaking ||
         Bot->BotState == EBotState::ChestLooting ||
         Bot->BotState == EBotState::HouseLooting) {
    BotsBTService_RoofLooting RoofLootingService;
    RoofLootingService.Tick(Bot);
}
```

Also add to warmup handling:

```cpp
BotsBTService_LobbyAdvanced LobbyAdvancedService;
LobbyAdvancedService.Tick(Bot);
```

## How to Apply Changes

### Method 1: Manual Application
1. Open `/home/engine/project/Spectra-10.40-main/10.40/PlayerBots.h`
2. Apply all compilation fixes (PART 1)
3. Add new enums (PART 2)
4. Add new struct (PART 3)
5. Add new fields to PlayerBot struct (PART 4)
6. Add new service classes (PART 5)
7. Update TickBots function (PART 6)

### Method 2: Using Provided Scripts
Run the Python script that was created:
```bash
python3 /home/engine/project/apply_all_changes.py
```

This script will automatically apply all changes at once.

## Features Implemented

### 1. Compilation Fixes
- Fixed all GetHealth(), GetShield(), GetMaxHealth(), and IsDead() method calls
- Changed to direct property access (Health, Shield, MaxHealth, Health <= 0)

### 2. Unique Trajectories
- Each bot gets 5-10 unique waypoints
- Bots follow individual paths during warmup
- Supports looping trajectories

### 3. Material Farming
- Bots farm materials during warmup
- Targets nearest building structures
- Adds materials to bot inventory

### 4. Build Training (90s)
- PracticeNineties() method for 90-degree turn builds
- Sequential wall+stair placement
- Movement integration

### 5. Roof Drop System
- Bots can target roofs for drops
- Breaks roof to access chests
- Systematic house looting after chest

### 6. Dynamic Drop Zones
- Personality-based drop preferences
- City vs town vs roof focus
- Player density awareness

## Testing

After applying changes:
1. Compile the project to verify no errors
2. Test in-game with multiple bots
3. Verify warmup behaviors (trajectories, farming, 90s)
4. Test roof drops and looting
5. Check dynamic drop zone selection

## Notes

- All changes maintain existing functionality
- Backward compatible with current bot behaviors
- Uses only existing SDK classes and methods
- Includes proper null checks for safety
