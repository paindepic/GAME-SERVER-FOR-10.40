#pragma once
#include "pch.h"

#include "PlayerBots.h"
#include "BotNames.h"

namespace Bots {
    // City coordinates for Fortnite Season 10.40 map (approximate centers)
    struct FCityLocation {
        FString Name;
        FVector Center;
        float Radius;
    };
    
    static TArray<FCityLocation> GetMajorCityLocations() {
        TArray<FCityLocation> Cities;
        
        // Major POIs on Athena map (Season 10.40)
        Cities.Add({FString(L"Tilted Towers"), FVector(-143000, -212000, 0), 5000.f});
        Cities.Add({FString(L"Pleasant Park"), FVector(-50000, -225000, 0), 4000.f});
        Cities.Add({FString(L"Retail Row"), FVector(205000, -213000, 0), 4000.f});
        Cities.Add({FString(L"Salty Springs"), FVector(27000, -60000, 0), 3000.f});
        Cities.Add({FString(L"Paradise Palms"), FVector(219000, 70000, 0), 4500.f});
        Cities.Add({FString(L"Fatal Fields"), FVector(-19000, 119000, 0), 3500.f});
        Cities.Add({FString(L"Lazy Lagoon"), FVector(166000, -290000, 0), 3500.f});
        Cities.Add({FString(L"Mega Mall"), FVector(15000, -172000, 0), 4000.f});
        
        return Cities;
    }
    
    // Helper function to filter large building foundations for roof spawning
    // Now supports city-specific targeting for better distribution
    TArray<AActor*> GetLargeBuildingFoundations(float MinSize = 10000.0f, bool bCitiesOnly = false) {
        TArray<AActor*> LargeFoundations;
        TArray<FCityLocation> Cities = GetMajorCityLocations();
        
        for (AActor* Foundation : BuildingFoundations) {
            if (!Foundation) continue;
            
            // Get bounding box to determine building size
            FVector Origin, Extent;
            Foundation->GetActorBounds(false, &Origin, &Extent);
            FVector BuildingLoc = Foundation->K2_GetActorLocation();
            
            // Calculate approximate footprint (X * Y dimensions)
            float Footprint = Extent.X * Extent.Y * 4.0f; // *4 because Extent is half-size
            
            if (Footprint < MinSize) continue;
            
            // If city-only mode, check if building is in a major city
            if (bCitiesOnly) {
                bool bInCity = false;
                FString CityName = FString(L"Unknown");
                
                for (const FCityLocation& City : Cities) {
                    float Distance = UKismetMathLibrary::GetDefaultObj()->Vector_Distance(BuildingLoc, City.Center);
                    if (Distance < City.Radius) {
                        bInCity = true;
                        CityName = City.Name;
                        std::string cityNameStr = CityName.ToString();
                        std::stringstream ss;
                        ss << "[ROOF SPAWN] Found building in " << cityNameStr << " (footprint: " << static_cast<int>(Footprint) << ")";
                        Log(ss.str());
                        break;
                    }
                }
                
                if (bInCity) {
                    LargeFoundations.Add(Foundation);
                }
            } else {
                // Add all large buildings regardless of location
                LargeFoundations.Add(Foundation);
            }
        }
        
        Log(std::string("[ROOF SPAWN] Found ") + std::to_string(LargeFoundations.Num())
            + " large buildings (cities only: " + std::string(bCitiesOnly ? "YES" : "NO") + ")");
        
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

        // ENHANCED: Support roof spawning with city-specific targeting
        FVector SpawnLocation = BotSpawn->K2_GetActorLocation();
        bool bActuallySpawnedOnRoof = false;
        
        if (bSpawnOnRoof && BuildingFoundations.Num() > 0) {
            // Get large buildings in major cities (Tilted, Pleasant, Retail, etc.)
            static TArray<AActor*> CityBuildingCache;
            static TArray<AActor*> AllLargeBuildingCache;
            static bool bCacheInitialized = false;
            
            if (!bCacheInitialized) {
                CityBuildingCache = GetLargeBuildingFoundations(8000.0f, true); // Cities only
                AllLargeBuildingCache = GetLargeBuildingFoundations(8000.0f, false); // All large buildings
                bCacheInitialized = true;

                {
                    std::stringstream ss;
                    ss << "[ROOF SPAWN] Cached " << CityBuildingCache.Num() << " city buildings, " << AllLargeBuildingCache.Num() << " total large buildings";
                    Log(ss.str());
                }
            }
            
            // Prefer city buildings (70% chance) for better distribution
            bool bUseCity = UKismetMathLibrary::GetDefaultObj()->RandomBoolWithWeight(0.7f);
            TArray<AActor*>& BuildingCache = (bUseCity && CityBuildingCache.Num() > 0) ? 
                CityBuildingCache : AllLargeBuildingCache;
            
            if (BuildingCache.Num() > 0) {
                AActor* RoofFoundation = BuildingCache[UKismetMathLibrary::GetDefaultObj()->RandomIntegerInRange(0, BuildingCache.Num() - 1)];
                if (RoofFoundation) {
                    FVector RoofLoc = RoofFoundation->K2_GetActorLocation();
                    
                    // Get building height for better roof placement
                    FVector Origin, Extent;
                    RoofFoundation->GetActorBounds(false, &Origin, &Extent);
                    float BuildingHeight = Extent.Z * 2.0f;
                    float BuildingFootprint = Extent.X * Extent.Y * 4.0f;
                    
                    // Spawn on top of building with slight randomization
                    RoofLoc.Z += (BuildingHeight > 300.0f ? BuildingHeight * 0.9f : 800.0f);
                    RoofLoc.X += UKismetMathLibrary::GetDefaultObj()->RandomFloatInRange(-150.f, 150.f);
                    RoofLoc.Y += UKismetMathLibrary::GetDefaultObj()->RandomFloatInRange(-150.f, 150.f);
                    SpawnLocation = RoofLoc;
                    bActuallySpawnedOnRoof = true;

                    Log(std::string("[ROOF SPAWN] Bot spawning on ") + (bUseCity ? "CITY" : "large building")
                        + " roof at X=" + std::to_string(RoofLoc.X)
                        + ", Y=" + std::to_string(RoofLoc.Y)
                        + ", Z=" + std::to_string(RoofLoc.Z)
                        + " (Height: " + std::to_string(BuildingHeight)
                        + ", Footprint: " + std::to_string(BuildingFootprint) + ")");
                }
            } else {
                Log("[ROOF SPAWN] No suitable buildings found, using normal spawn!");
            }
        }

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

        if (true)
        {
            int Ret = Globals::NextBotTeamIndex;

            ++Globals::CurrentBotsOnTeam;

            if (Globals::CurrentBotsOnTeam == Globals::MaxPlayersPerTeam)
            {
                Globals::NextBotTeamIndex++;
                Globals::CurrentBotsOnTeam = 0;
            }

            PlayerState->TeamIndex = Ret;
            PlayerState->OnRep_TeamIndex(0);

            PlayerState->SquadId = PlayerState->TeamIndex - 3;
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
}
