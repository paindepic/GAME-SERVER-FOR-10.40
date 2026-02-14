#pragma once
#include "pch.h"

#include "ServerBotManager.h"
#include "BotNames.h"

namespace Bots {
    // Helper function to filter large building foundations for roof spawning
    TArray<AActor*> GetLargeBuildingFoundations(float MinSize = 10000.0f) {
        TArray<AActor*> LargeFoundations;
        
        for (AActor* Foundation : BuildingFoundations) {
            if (!Foundation) continue;
            
            // Get bounding box to determine building size
            FVector Origin, Extent;
            Foundation->GetActorBounds(false, &Origin, &Extent);
            
            // Calculate approximate footprint (X * Y dimensions)
            float Footprint = Extent.X * Extent.Y * 4.0f; // *4 because Extent is half-size
            
            if (Footprint >= MinSize) {
                LargeFoundations.Add(Foundation);
            }
        }
        
        Log(std::format("[ROOF SPAWN] Found {} large building foundations out of {}", 
            LargeFoundations.Num(), BuildingFoundations.Num()).c_str());
        
        return LargeFoundations;
    }

    void SpawnPlayerBot(AActor* OverrideSpawnLoc = nullptr, PlayerBots::EBotState StartingState = PlayerBots::EBotState::Warmup, bool bSpawnOnRoof = false) {
        static UBehaviorTree* PhoebeBehaviorTree = StaticLoadObject<UBehaviorTree>("Game/Athena/AI/Phoebe/BehaviorTrees/BT_Phoebe.BT_Phoebe");
        static UClass* PhoebePawnClass = StaticLoadObject<UClass>("/Game/Athena/AI/Phoebe/BP_PlayerPawn_Athena_Phoebe.BP_PlayerPawn_Athena_Phoebe_C");

        static int MaxSpawnLocations = PlayerStarts.Num();
        static int BotSpawnLocationIndex = 0;

        auto GameMode = (AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode;
        AFortGameStateAthena* GameState = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;

        // Null checks for critical objects
        if (!GameMode) {
            Log("[CRASH FIX] GameMode is null in SpawnPlayerBot!");
            return;
        }
        if (!GameState) {
            Log("[CRASH FIX] GameState is null in SpawnPlayerBot!");
            return;
        }

        UFortAthenaAIBotCustomizationData* BotCustomizationData = (UFortAthenaAIBotCustomizationData*)UGameplayStatics::SpawnObject(UFortAthenaAIBotCustomizationData::StaticClass(), GameMode);
        BotCustomizationData->BehaviorTree = PhoebeBehaviorTree;
        BotCustomizationData->bOverrideBehaviorTree = true;
        BotCustomizationData->bOverrideCharacterCustomization = false;
        BotCustomizationData->bOverrideDBNOPlayStyle = false;
        BotCustomizationData->bOverrideSkillLevel = false;
        BotCustomizationData->bOverrideSkillSets = false;
        BotCustomizationData->bOverrideStartupInventory = false;
        BotCustomizationData->CharacterCustomization = {};
        BotCustomizationData->DBNOPlayStyle = EDBNOPlayStyle::Default;
        BotCustomizationData->PawnClass = PhoebePawnClass;
        BotCustomizationData->SkillLevel = 1;
        BotCustomizationData->SkillSets = {};
        BotCustomizationData->StartupInventory = (UFortAthenaAIBotInventoryItems*)UGameplayStatics::SpawnObject(UFortAthenaAIBotInventoryItems::StaticClass(), GameMode);

        static UFortWeaponMeleeItemDefinition* PickDef = StaticLoadObject<UFortWeaponMeleeItemDefinition>("/Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01");;
        if (!Pickaxes.empty()) {
            PickDef = Pickaxes[UKismetMathLibrary::GetDefaultObj()->RandomIntegerInRange(0, Pickaxes.size() - 1)]->WeaponDefinition;
        }

        if (PickDef) {
            BotCustomizationData->StartupInventory->Items.Add(PickDef);
        }

        if (Floor && Roof && Stair && Wall && EditTool) {
            BotCustomizationData->StartupInventory->Items.Add(Floor);
            BotCustomizationData->StartupInventory->Items.Add(Roof);
            BotCustomizationData->StartupInventory->Items.Add(Stair);
            BotCustomizationData->StartupInventory->Items.Add(Wall);
            BotCustomizationData->StartupInventory->Items.Add(EditTool);
        }
        else
        {
            Log("BuildingItem Null!");
        }

        if (BotSpawnLocationIndex >= MaxSpawnLocations) {
            BotSpawnLocationIndex = 0;
        }
        AActor* BotSpawn = PlayerStarts[BotSpawnLocationIndex];
        BotSpawnLocationIndex++;
        if (OverrideSpawnLoc) {
            BotSpawn = OverrideSpawnLoc;
        }

        // Support roof spawning - use large building foundations for major cities
        FVector SpawnLocation = BotSpawn->K2_GetActorLocation();
        bool bActuallySpawnedOnRoof = false;
        
        if (bSpawnOnRoof && BuildingFoundations.Num() > 0) {
            // Get large buildings only (cities like Tilted, Pleasant, Retail)
            static TArray<AActor*> LargeBuildingCache;
            static bool bCacheInitialized = false;
            
            if (!bCacheInitialized) {
                LargeBuildingCache = GetLargeBuildingFoundations(8000.0f); // Minimum 8k footprint for major buildings
                bCacheInitialized = true;
            }
            
            if (LargeBuildingCache.Num() > 0) {
                AActor* RoofFoundation = LargeBuildingCache[UKismetMathLibrary::GetDefaultObj()->RandomIntegerInRange(0, LargeBuildingCache.Num() - 1)];
                if (RoofFoundation) {
                    FVector RoofLoc = RoofFoundation->K2_GetActorLocation();
                    
                    // Get building height for better roof placement
                    FVector Origin, Extent;
                    RoofFoundation->GetActorBounds(false, &Origin, &Extent);
                    float BuildingHeight = Extent.Z * 2.0f;
                    
                    RoofLoc.Z += (BuildingHeight > 300.0f ? BuildingHeight : 800.0f); // Use actual height or default
                    RoofLoc.X += UKismetMathLibrary::GetDefaultObj()->RandomFloatInRange(-200.f, 200.f);
                    RoofLoc.Y += UKismetMathLibrary::GetDefaultObj()->RandomFloatInRange(-200.f, 200.f);
                    SpawnLocation = RoofLoc;
                    bActuallySpawnedOnRoof = true;
                    
                    Log(std::format("[ROOF SPAWN] Bot spawning on roof at X={:.1f}, Y={:.1f}, Z={:.1f} (Building Height: {:.1f})",
                        RoofLoc.X, RoofLoc.Y, RoofLoc.Z, BuildingHeight).c_str());
                }
            } else {
                Log("[ROOF SPAWN] No large buildings found, using normal spawn!");
            }
        }

        int PrecomputedTeamIndex = Globals::NextBotTeamIndex;
        ++Globals::CurrentBotsOnTeam;

        if (Globals::CurrentBotsOnTeam == Globals::MaxPlayersPerTeam)
        {
            Globals::NextBotTeamIndex++;
            Globals::CurrentBotsOnTeam = 0;
        }

        int PrecomputedSquadId = PrecomputedTeamIndex - 3;

        AFortPlayerPawnAthena* Pawn = GameMode->ServerBotManager->SpawnBot(SpawnLocation, BotSpawn->K2_GetActorRotation(), BotCustomizationData);
        if (!Pawn) {
            Log("[CRASH FIX] Failed to spawn bot pawn!");
            return;
        }
        AFortAthenaAIBotController* PC = (AFortAthenaAIBotController*)Pawn->Controller;
        if (!PC) {
            Log("[CRASH FIX] Failed to get bot controller!");
            return;
        }
        AFortPlayerStateAthena* PlayerState = (AFortPlayerStateAthena*)PC->PlayerState;
        if (!PlayerState) {
            Log("[CRASH FIX] Failed to get bot player state!");
            return;
        }

        if (BotDisplayNames.size() != 0) {
            std::srand(static_cast<unsigned int>(std::time(0)));
            int randomIndex = std::rand() % BotDisplayNames.size();
            std::string rdName = BotDisplayNames[randomIndex];
            BotDisplayNames.erase(BotDisplayNames.begin() + randomIndex);

            int size_needed = MultiByteToWideChar(CP_UTF8, 0, rdName.c_str(), (int)rdName.size(), NULL, 0);
            std::wstring wideString(size_needed, 0);
            MultiByteToWideChar(CP_UTF8, 0, rdName.c_str(), (int)rdName.size(), &wideString[0], size_needed);

            FString CVName = FString(wideString.c_str());
            GameMode->ChangeName(PC, CVName, true);
            PlayerState->OnRep_PlayerName();
        }

        if (StartingState != PlayerBots::EBotState::Warmup) {
            for (PlayerBots::PlayerBot* Bot : PlayerBots::PlayerBotArray) {
                if (Bot->Pawn == Pawn) {
                    Bot->BotState = StartingState;
                    Bot->bHasRoofSpawned = bActuallySpawnedOnRoof;
                    if (bActuallySpawnedOnRoof) {
                        Log("[ROOF SPAWN] Bot marked for roof-based drop zone targeting!");
                    }
                }
            }
        }

        PlayerState->TeamIndex = PrecomputedTeamIndex;
        PlayerState->OnRep_TeamIndex(0);

        PlayerState->SquadId = PrecomputedSquadId;
        PlayerState->OnRep_SquadId();

        PlayerState->OnRep_PlayerTeam();
        PlayerState->OnRep_PlayerTeamPrivate();

        FGameMemberInfo NewMemberInfo{ -1,-1,-1 };
        NewMemberInfo.MemberUniqueId = PlayerState->GetUniqueID();
        NewMemberInfo.SquadId = PlayerState->SquadId;
        NewMemberInfo.TeamIndex = PlayerState->TeamIndex;

        GameState->GameMemberInfoArray.Members.Add(NewMemberInfo);
        GameState->GameMemberInfoArray.MarkItemDirty(NewMemberInfo);
    }
}
