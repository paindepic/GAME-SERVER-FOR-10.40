# PlayerBots.h Compilation Fixes and New Features Implementation Plan

## PART 1: C++ Compilation Fixes Required

All instances found that need to be corrected:

### Replace GetHealth() with Health:
1. Line ~917: `float Health = bot->Pawn->GetHealth();` → `bot->Pawn->Health`
2. Line ~928: `float Health = bot->Pawn->GetHealth();` → `bot->Pawn->Health`
3. Line ~954: `float Health = bot->Pawn->GetHealth();` → `bot->Pawn->Health`
4. Line ~1089: `float HealthPercent = bot->Pawn->GetHealth() / bot->Pawn->GetMaxHealth();` → `bot->Pawn->Health / bot->Pawn->MaxHealth`
5. Line ~1107: `if (Pawn->GetHealth() <= 75)` → `if (Pawn->Health <= 75)`
6. Line ~1117: `float Health = bot->Pawn->GetHealth();` → `bot->Pawn->Health`
7. Line ~1421: `if (bot->bIsStressed && bot->Pawn->GetHealth() < 50)` → `bot->Pawn->Health < 50`
8. Line ~1430: `if (bot->bIsStressed || bot->Pawn->GetHealth() < 60)` → `bot->Pawn->Health < 60`
9. Line ~2004: `if (bot->bIsStressed && bot->Pawn->GetHealth() < 50)` → `bot->Pawn->Health < 50`

### Replace GetShield() with Shield:
1. Line ~918: `float Shield = bot->Pawn->GetShield();` → `bot->Pawn->Shield`
2. Line ~929: `float Shield = bot->Pawn->GetShield();` → `bot->Pawn->Shield`
3. Line ~955: `float Shield = bot->Pawn->GetShield();` → `bot->Pawn->Shield`
4. Line ~1118: `float Shield = bot->Pawn->GetShield();` → `bot->Pawn->Shield`

### Replace GetMaxHealth() with MaxHealth:
1. Line ~1089: `bot->Pawn->GetMaxHealth()` → `bot->Pawn->MaxHealth`

### Replace IsDead() with Health <= 0:
1. Line ~354: `if (PS->bInAircraft || Pawn->IsDead())` → `Pawn->Health <= 0`
2. Line ~2136: `if (Bot->PlayerState->IsPlayerDead() || Bot->Pawn->IsDead())` → `Bot->Pawn->Health <= 0`

## PART 2: New Enums and Structures

### 1. Add ERoofLootPhase enum (after line ~124, after ELootableType enum):
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

### 2. Add FBotTrajectory struct (after ERoofLootPhase enum):
```cpp
struct FBotTrajectory {
    FVector Waypoints[10];
    int32 CurrentWaypoint = 0;
    int32 NumWaypoints = 0;
    float WaypointReachedDist = 100.f;
    bool bLoopTrajectory = true;
    
    FBotTrajectory() {
        CurrentWaypoint = 0;
        NumWaypoints = 0;
        WaypointReachedDist = 100.f;
        bLoopTrajectory = true;
        for (int i = 0; i < 10; i++) {
            Waypoints[i] = FVector::ZeroVector;
        }
    }
};
```

### 3. Add new fields to PlayerBot struct (after line ~212, after bHasRoofSpawned):
```cpp
// NOUVEAUX CHAMPS pour système avancé lobby
FBotTrajectory Trajectory;
bool bTargetRoof = false;
ERoofLootPhase RoofLootPhase = ERoofLootPhase::Landing;
ABuildingContainer* TargetRoofChest = nullptr;
float LastFarmingTime = 0.f;
bool bIsPracticingBuild = false;
float PracticeEndTime = 0.f;
```

## PART 3: New Classes

### 1. Add BotsBTService_LobbyAdvanced class (before TickBots function):
```cpp
class BotsBTService_LobbyAdvanced {
public:
    void InitializeIndividualTrajectory(PlayerBot* bot) {
        if (!bot || !bot->Pawn) return;
        
        FVector StartLoc = bot->Pawn->K2_GetActorLocation();
        
        // Générer 5-10 waypoints aléatoires uniques pour ce bot
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

### 2. Add BotsBTService_RoofLooting class (before TickBots function):
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
                }
                bot->RoofLootPhase = ERoofLootPhase::LootingHouse;
                break;
            }
            
            case ERoofLootPhase::LootingHouse: {
                if (bot->tick_counter % 60 == 0) {
                    ABuildingContainer* NextChest = bot->GetNearestChest();
                    if (NextChest && !NextChest->bAlreadySearched) {
                        bot->NearestChest = NextChest;
                        bot->BotState = EBotState::Looting;
                    } else {
                        bot->RoofLootPhase = ERoofLootPhase::Complete;
                        bot->bTargetRoof = false;
                    }
                }
                break;
            }
            
            case ERoofLootPhase::Complete:
                bot->bTargetRoof = false;
                break;
        }
    }
};
```

## PART 4: Modify TickBots() Function

Add these lines at the beginning of TickBots() function (after line ~2117, after GameState null checks):

```cpp
// NOUVEAU: Services avancés
BotsBTService_LobbyAdvanced LobbyService;
BotsBTService_RoofLooting RoofLootService;
```

Add in the main for loop (after line ~2130, in the bot checking loop):

```cpp
if (GameState->GamePhase == EAthenaGamePhase::Warmup) {
    // NOUVEAU: Comportement lobby avancé
    LobbyService.Tick(Bot);
}
// ... autres états ...
else if (Bot->BotState == EBotState::Landed && Bot->bTargetRoof) {
    // NOUVEAU: Loot de toit
    RoofLootService.TickRoofLooting(Bot);
}
```

## PART 5: Modify Bots.h

In SpawnPlayerBot function, enhance the bSpawnOnRoof logic (after SpawnLocation calculation):

```cpp
// Support roof spawning amélioré
FVector SpawnLocation = BotSpawn->K2_GetActorLocation();
if (bSpawnOnRoof && BuildingFoundations.Num() > 0) {
    // Choisir un bâtiment aléatoire
    AActor* RoofFoundation = BuildingFoundations[UKismetMathLibrary::GetDefaultObj()->RandomIntegerInRange(0, BuildingFoundations.Num() - 1)];
    if (RoofFoundation) {
        FVector RoofLoc = RoofFoundation->K2_GetActorLocation();
        RoofLoc.Z += 800.0f;
        RoofLoc.X += UKismetMathLibrary::GetDefaultObj()->RandomFloatInRange(-300.f, 300.f);
        RoofLoc.Y += UKismetMathLibrary::GetDefaultObj()->RandomFloatInRange(-300.f, 300.f);
        SpawnLocation = RoofLoc;
        Log("[Bots] Spawning bot on roof at: " + std::to_string(RoofLoc.X) + ", " + std::to_string(RoofLoc.Y) + ", " + std::to_string(RoofLoc.Z));
    }
}
```

## PART 6: Modify GameMode.h

In ReadyToStartMatch, after BotManager initialization (around where it initializes bots):

```cpp
// Spawn progressif des bots en warmup
if (Globals::bBotsEnabled && GameState->GamePhase == EAthenaGamePhase::Warmup) {
    static int32 BotsSpawned = 0;
    static int32 TotalBots = 50; // Configurable
    
    if (BotsSpawned < TotalBots && GameState->GetServerWorldTimeSeconds() > BotsSpawned * 2.0f) {
        Bots::SpawnPlayerBot(nullptr, PlayerBots::EBotState::Warmup, false);
        BotsSpawned++;
        Log("[Lobby] Spawned bot " + std::to_string(BotsSpawned) + "/" + std::to_string(TotalBots));
    }
}
```

## Implementation Notes

Due to persistent technical issues with the EditFile tool and bash commands not responding to input, the following approach should be taken:

1. Manual text replacement using a text editor or IDE
2. Apply all 16 compilation fixes for GetHealth()/GetShield()/GetMaxHealth()/IsDead()
3. Add the new ERoofLootPhase enum after ELootableType enum
4. Add the FBotTrajectory struct after the new enum
5. Add 7 new fields to PlayerBot struct after bHasRoofSpawned
6. Add BotsBTService_LobbyAdvanced class before TickBots function
7. Add BotsBTService_RoofLooting class before TickBots function  
8. Modify TickBots() to integrate both new services
9. Modify Bots.h SpawnPlayerBot for enhanced roof spawning
10. Modify GameMode.h ReadyToStartMatch for progressive bot spawning

All changes maintain existing code style, use C++ only, and follow the existing patterns in the codebase.
