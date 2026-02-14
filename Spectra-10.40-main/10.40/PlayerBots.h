#pragma once
#include "framework.h"

#include "FortInventory.h"
#include "BuildingContainer.h"

namespace PlayerBots {
    enum class EBotBuildingType {
        Wall,
        Floor,
        Stair,
        Cone,
        Edit,
        Max
    };

    enum class EBotLootingFilter {
        Meds,
        BigMeds,
        Shields,
        BigShields,
        Ammo,
        Weapons,
        MAX
    };

    enum class EBotSwapItemType {
        HealthHeals,
        ShieldHeals,
        CloseRangeGun,
        MediumRangeGun,
        LongRangeGun,
        Other,
        MAX,
    };

    enum class EBotHealingType {
        Health,
        Shield,
        Both,
        MAX
    };

    enum class EBotStrafeType {
        StrafeLeft,
        StrafeRight
    };

    enum class EBotWarmupChoice {
        Emote,
        ApproachPlayersAndEmote,
        LookAtRandomPlayers,
        PickaxeOtherPlayers,
        FightOtherPlayers,
        BuildStructures,
        RunAround,
        JumpAround,
        PracticeBuilding,
        SocialGroup,
        ExploreSpawnIsland,
        DanceInGroup,
        MAX
    };

    enum class EBotPersonality {
        Aggressive,
        Builder,
        Balanced,
        Tactical,
        MAX
    };

    enum class EBotBuildPattern {
        None,
        DefensiveWall,
        DefensiveBox,
        Turtle,
        RampRush,
        Nineties,
        HighGroundRetake,
        MAX
    };

    enum class EBotSoundType {
        None,
        Gunshot,
        Building,
        Footsteps,
        Chest,
        Vehicle,
        MAX
    };

    enum class EBotCombatStance {
        Passive,
        Engaged,
        Retreating,
        Flanking,
        Holding,
        Pushing,
        MAX
    };

    enum class EBotState {
        Warmup,
        PreBus,
        Bus,
        Skydiving,
        Gliding,
        Landed,
        Looting,
        MovingToSafeZone,
        Combat,
        Healing,
        InvestigatingSound,
        MAX
    };

    enum class ELootableType {
        None = -1,
        Chest = 0,
        Pickup = 1
    };

    struct FSoundMemory {
        EBotSoundType SoundType = EBotSoundType::None;
        FVector Location = FVector();
        float DetectionTime = 0.f;
        float Confidence = 0.f;
        APawn* SourcePawn = nullptr;
        
        bool IsValid(float CurrentTime, float MaxAge = 30.f) const {
            return SoundType != EBotSoundType::None && 
                   (CurrentTime - DetectionTime) < MaxAge;
        }
        
        float GetAge(float CurrentTime) const {
            return CurrentTime - DetectionTime;
        }
    };

    std::vector<class PlayerBot*> PlayerBotArray{};
    
    struct PlayerBot
    {
    public:
        EBotWarmupChoice WarmupChoice = EBotWarmupChoice::MAX;
        EBotLootingFilter CurrentLootingFilter = EBotLootingFilter::MAX;
        EBotHealingType CurrentHealingType = EBotHealingType::MAX;
        bool bPauseTicking = false;
        uint64_t tick_counter = 0;
        AFortPlayerPawnAthena* Pawn = nullptr;
        AFortAthenaAIBotController* PC = nullptr;
        AFortPlayerStateAthena* PlayerState = nullptr;
        EBotState BotState = EBotState::MAX;
        FVector LastUpdatedBotLocation = FVector();
        bool bIsCurrentlyStrafing = false;
        EBotStrafeType StrafeType = EBotStrafeType::StrafeLeft;
        float StrafeEndTime = 0.0f;
        bool bPotentiallyUnderAttack = false;
        APawn* NearestPlayerPawn = nullptr;
        AFortPickup* NearestPickup = nullptr;
        ABuildingContainer* NearestChest = nullptr;
        FVector ClosestLootableLoc = FVector();
        ELootableType ClosestLootableType = ELootableType::None;
        APawn* CurrentlyFocusedPawn = nullptr;
        bool bHasCompletedCurrentAction = false;
        FVector TargetDropZone = FVector();
        float ClosestDistToDropZone = FLT_MAX;
        bool bHasThankedBusDriver = false;
        bool bHasJumpedFromBus = false;
        float TimeToNextAction = 0.f;
        float LootTargetStartTime = 0.f;
        AActor* TargetLootable = nullptr;
        float LastLootTargetDistance = 0.f;
        float ZoneTargetStartTime = 0.f;
        float LastZoneTargetDistance = 0.f;
        bool bIsStressed = false;
        bool bIsInSafeZone = true;
        EBotPersonality Personality = EBotPersonality::Balanced;
        int32 SkillLevel = 5;
        bool bIsBuilding = false;
        EBotBuildPattern CurrentBuildPattern = EBotBuildPattern::None;
        int32 CurrentBuildSequenceStep = 0;
        float LastBuildTime = 0.f;
        float BuildCooldown = 0.1f;
        FVector BuildTargetLocation = FVector();
        int32 WoodCount = 999;
        int32 StoneCount = 500;
        int32 MetalCount = 200;
        FVector LastWarmupMoveLocation = FVector();
        float LastWarmupActionTime = 0.f;
        EBotCombatStance CombatStance = EBotCombatStance::Passive;
        FSoundMemory LastDetectedSound;
        float LastCombatTime = 0.f;
        FVector LastKnownEnemyPosition = FVector();
        int32 ConsecutiveShotsFired = 0;
        float LastShotTime = 0.f;
        bool bIsFlanking = false;
        FVector FlankTargetPosition = FVector();
        float TimeInCurrentStance = 0.f;
        int32 DamageDealtRecently = 0;
        float LastDamageTime = 0.f;
        FVector NaturalMovementTarget = FVector();
        float NaturalMovementSpeed = 0.f;
        bool bIsMovingNaturally = false;
        float StuckCheckTime = 0.f;
        FVector LastStuckCheckPosition = FVector();
        int32 StuckCounter = 0;
        float LastNaturalMoveUpdate = 0.f;
        float TargetArrivalTime = 0.f;
        bool bHasRoofSpawned = false;

    public:
        PlayerBot(AFortPlayerPawnAthena* Pawn, AFortAthenaAIBotController* PC, AFortPlayerStateAthena* PlayerState) {
            this->Pawn = Pawn;
            this->PC = PC;
            this->PlayerState = PlayerState;
            PlayerBotArray.push_back(this);
        }

        void GiveItem(UFortItemDefinition* Def, int Count = 1, int LoadedAmmo = 0) {
            if (!Def) {
                Log("ItemDef is nullptr!");
                return;
            }
            UFortWorldItem* Item = (UFortWorldItem*)Def->CreateTemporaryItemInstanceBP(Count, 0);
            Item->OwnerInventory = PC->Inventory;
            Item->ItemEntry.LoadedAmmo = LoadedAmmo;
            PC->Inventory->Inventory.ReplicatedEntries.Add(Item->ItemEntry);
            PC->Inventory->Inventory.ItemInstances.Add(Item);
            PC->Inventory->Inventory.MarkItemDirty(Item->ItemEntry);
            PC->Inventory->HandleInventoryLocalUpdate();
        }

        void Emote() {
            auto EmoteDef = Pawn->CosmeticLoadout.Dances[UKismetMathLibrary::GetDefaultObj()->RandomIntegerInRange(0, Pawn->CosmeticLoadout.Dances.Num() - 1)];
            if (!EmoteDef) return;
            static UClass* EmoteAbilityClass = StaticLoadObject<UClass>("/Game/Abilities/Emotes/GAB_Emote_Generic.GAB_Emote_Generic_C");
            FGameplayAbilitySpec Spec{};
            AbilitySpecConstructor(&Spec, reinterpret_cast<UGameplayAbility*>(EmoteAbilityClass->DefaultObject), 1, -1, EmoteDef);
            GiveAbilityAndActivateOnce(reinterpret_cast<AFortPlayerStateAthena*>(PC->PlayerState)->AbilitySystemComponent, &Spec.Handle, Spec);
        }

        void Run() {
            for (size_t i = 0; i < PlayerState->AbilitySystemComponent->ActivatableAbilities.Items.Num(); i++) {
                if (PlayerState->AbilitySystemComponent->ActivatableAbilities.Items[i].Ability->IsA(UFortGameplayAbility_Sprint::StaticClass())) {
                    if (PlayerState->AbilitySystemComponent->ActivatableAbilities.Items[i].ActivationInfo.PredictionKeyWhenActivated.bIsStale) continue;
                    if (PlayerState->AbilitySystemComponent->CanActivateAbilityWithMatchingTag(PlayerState->AbilitySystemComponent->ActivatableAbilities.Items[i].Ability->AbilityTags)) {
                        PlayerState->AbilitySystemComponent->ServerTryActivateAbility(PlayerState->AbilitySystemComponent->ActivatableAbilities.Items[i].Handle, PlayerState->AbilitySystemComponent->ActivatableAbilities.Items[i].InputPressed, PlayerState->AbilitySystemComponent->ActivatableAbilities.Items[i].ActivationInfo.PredictionKeyWhenActivated);
                    }
                    break;
                }
            }
        }

        void LaunchIntoAir() {
            FVector BotLoc = LastUpdatedBotLocation;
            BotLoc.Z = BotLoc.Z + 15000.f;
            FHitResult HitResult;
            Pawn->K2_SetActorLocation(BotLoc, false, &HitResult, true);
            Pawn->BeginSkydiving(false);
        }

        void Pickup(AFortPickup* Pickup) {
            if (!Pickup) return;
            Pickup->PickupLocationData.bPlayPickupSound = true;
            Pickup->PickupLocationData.FlyTime = 0.3f;
            Pickup->PickupLocationData.ItemOwner = Pawn;
            Pickup->PickupLocationData.PickupGuid = Pickup->PrimaryPickupItemEntry.ItemGuid;
            Pickup->PickupLocationData.PickupTarget = Pawn;
            Pickup->OnRep_PickupLocationData();
            Pickup->bPickedUp = true;
            Pickup->OnRep_bPickedUp();
        }

        void PickupAllItemsInRange(float Range = 320.f) {
            if (!Pawn || !PC) return;
            TArray<AActor*> PickupArray;
            UGameplayStatics::GetDefaultObj()->GetAllActorsOfClass(UWorld::GetWorld(), AFortPickupAthena::StaticClass(), &PickupArray);
            for (size_t i = 0; i < PickupArray.Num(); i++) {
                if (!PickupArray[i] || PickupArray[i]->bHidden) continue;
                if (PickupArray[i]->GetDistanceTo(Pawn) < Range) Pickup((AFortPickupAthena*)PickupArray[i]);
            }
            PickupArray.Free();
        }

        FFortItemEntry* GetEntry(UFortItemDefinition* Def) {
            for (size_t i = 0; i < PC->Inventory->Inventory.ReplicatedEntries.Num(); i++) {
                if (PC->Inventory->Inventory.ReplicatedEntries[i].ItemDefinition == Def)
                    return &PC->Inventory->Inventory.ReplicatedEntries[i];
            }
            return nullptr;
        }

        void EquipPickaxe() {
            if (!Pawn) {
                Log("No Pawn");
                return;
            }
            for (size_t i = 0; i < PC->Inventory->Inventory.ReplicatedEntries.Num(); i++) {
                if (PC->Inventory->Inventory.ReplicatedEntries[i].ItemDefinition->IsA(UFortWeaponMeleeItemDefinition::StaticClass())) {
                    Pawn->EquipWeaponDefinition((UFortWeaponItemDefinition*)PC->Inventory->Inventory.ReplicatedEntries[i].ItemDefinition, PC->Inventory->Inventory.ReplicatedEntries[i].ItemGuid);
                    break;
                }
            }
        }

        ABuildingContainer* GetNearestChest() {
            static auto ChestClass = StaticLoadObject<UClass>("/Game/Building/ActorBlueprints/Containers/Tiered_Chest_Athena.Tiered_Chest_Athena_C");
            TArray<AActor*> ChestArray;
            UGameplayStatics::GetDefaultObj()->GetAllActorsOfClass(UWorld::GetWorld(), ChestClass, &ChestArray);
            ABuildingContainer* NearestChest = nullptr;
            float NearestDistance = FLT_MAX;
            for (size_t i = 0; i < ChestArray.Num(); i++) {
                ABuildingContainer* Chest = (ABuildingContainer*)ChestArray[i];
                if (Chest->bHidden || Chest->bAlreadySearched) continue;
                if (!NearestChest || Chest->GetDistanceTo(Pawn) < NearestDistance) {
                    NearestDistance = Chest->GetDistanceTo(Pawn);
                    NearestChest = Chest;
                }
            }
            ChestArray.Free();
            return (ABuildingContainer*)NearestChest;
        }

        AFortPickup* GetNearestPickup() {
            TArray<AActor*> PickupArray;
            UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), AFortPickup::StaticClass(), &PickupArray);
            AActor* NearestPickup = nullptr;
            float NearestDistance = FLT_MAX;
            for (AActor* Pickup : PickupArray) {
                if (Pickup->GetDistanceTo(Pawn) < NearestDistance) {
                    NearestDistance = Pickup->GetDistanceTo(Pawn);
                    NearestPickup = Pickup;
                }
            }
            PickupArray.Free();
            return (AFortPickup*)NearestPickup;
        }

        APawn* GetNearestPawn() {
            auto GameMode = (AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode;
            AFortGameStateAthena* GameState = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;
            APawn* NearestPlayer = nullptr;
            float NearestDistance = FLT_MAX;
            for (size_t i = 0; i < GameMode->AlivePlayers.Num(); i++) {
                if ((GameMode->AlivePlayers[i]->Pawn && GameMode->AlivePlayers[i]->Pawn->GetDistanceTo(Pawn) < NearestDistance)) {
                    AFortPlayerStateAthena* PS = (AFortPlayerStateAthena*)GameMode->AlivePlayers[i]->PlayerState;
                    AFortPlayerControllerAthena* PC = (AFortPlayerControllerAthena*)GameMode->AlivePlayers[i];
                    AFortPlayerPawnAthena* Pawn = (AFortPlayerPawnAthena*)PC->Pawn;
                    if (!PS || !PC || !Pawn) continue;
                    if (PS->TeamIndex != PlayerState->TeamIndex) {
                        if (PS->bInAircraft || Pawn->IsDead()) continue;
                        NearestDistance = GameMode->AlivePlayers[i]->Pawn->GetDistanceTo(Pawn);
                        NearestPlayer = GameMode->AlivePlayers[i]->Pawn;
                    }
                }
            }
            for (size_t i = 0; i < GameMode->AliveBots.Num(); i++) {
                if (GameMode->AliveBots[i]->Pawn != Pawn) {
                    if ((GameMode->AliveBots[i]->Pawn && GameMode->AliveBots[i]->Pawn->GetDistanceTo(Pawn) < NearestDistance)) {
                        if (GameMode->AliveBots[i]->Pawn == Pawn) continue;
                        AFortPlayerStateAthena* PS = (AFortPlayerStateAthena*)GameMode->AliveBots[i]->PlayerState;
                        if (PS->TeamIndex != PlayerState->TeamIndex) {
                            NearestDistance = GameMode->AliveBots[i]->Pawn->GetDistanceTo(Pawn);
                            NearestPlayer = GameMode->AliveBots[i]->Pawn;
                        }
                    }
                }
            }
            return NearestPlayer;
        }

        void UpdateLootableReservation(AActor* Lootable, bool RemoveReservation) {
            if (RemoveReservation && !TargetLootable) return;
            if (!RemoveReservation) {
                if (!Lootable) return;
                TargetLootable = Lootable;
                Lootable->bHidden = true;
            } else {
                TargetLootable->bHidden = false;
                TargetLootable = nullptr;
            }
        }

        APawn* GetRandomPawn() {
            auto GameMode = (AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode;
            if (GameMode->AlivePlayers.Num() == 0 || GameMode->AliveBots.Num() == 0) return nullptr;
            APawn* RandomPlayer = GameMode->AlivePlayers[UKismetMathLibrary::GetDefaultObj()->RandomIntegerInRange(0, GameMode->AlivePlayers.Num() - 1)]->Pawn;
            APawn* RandomBot = GameMode->AliveBots[UKismetMathLibrary::GetDefaultObj()->RandomIntegerInRange(0, GameMode->AliveBots.Num() - 1)]->Pawn;
            if (UKismetMathLibrary::GetDefaultObj()->RandomBool()) return RandomPlayer;
            else {
                if (RandomBot == Pawn) return nullptr;
                return RandomBot;
            }
        }

        void ForceStrafe(bool override) {
            if (!bIsCurrentlyStrafing && override) {
                bIsCurrentlyStrafing = true;
                if (UKismetMathLibrary::RandomBool()) StrafeType = EBotStrafeType::StrafeLeft;
                else StrafeType = EBotStrafeType::StrafeRight;
                StrafeEndTime = UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld()) + UKismetMathLibrary::GetDefaultObj()->RandomFloatInRange(2.0f, 5.0f);
            } else {
                if (UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld()) < StrafeEndTime) {
                    if (StrafeType == EBotStrafeType::StrafeLeft) Pawn->AddMovementInput((Pawn->GetActorRightVector() * -1.0f), 1.5f, true);
                    else Pawn->AddMovementInput(Pawn->GetActorRightVector(), 1.5f, true);
                } else bIsCurrentlyStrafing = false;
            }
        }

        void LookAt(AActor* Actor) {
            if (!Pawn || PC->GetFocusActor() == Actor) return;
            if (!Actor) {
                PC->K2_ClearFocus();
                return;
            }
            PC->K2_SetFocus(Actor);
        }

        bool IsPickaxeEquiped() {
            if (!Pawn || !Pawn->CurrentWeapon) return false;
            if (Pawn->CurrentWeapon->WeaponData->IsA(UFortWeaponMeleeItemDefinition::StaticClass())) return true;
            return false;
        }

        bool HasGun() {
            for (size_t i = 0; i < PC->Inventory->Inventory.ReplicatedEntries.Num(); i++) {
                auto& Entry = PC->Inventory->Inventory.ReplicatedEntries[i];
                if (Entry.ItemDefinition) {
                    std::string ItemName = Entry.ItemDefinition->Name.ToString();
                    if (ItemName.contains("Shotgun") || ItemName.contains("SMG") || ItemName.contains("Assault")
                        || ItemName.contains("Sniper") || ItemName.contains("Rocket") || ItemName.contains("Pistol")) return true;
                }
            }
            return false;
        }

        void SimpleSwitchToWeapon() {
            if (!Pawn || !Pawn->CurrentWeapon || !Pawn->CurrentWeapon->WeaponData || !PC || !PC->Inventory) return;
            if (Pawn->CurrentWeapon->WeaponData->IsA(UFortWeaponMeleeItemDefinition::StaticClass())) {
                for (size_t i = 0; i < PC->Inventory->Inventory.ReplicatedEntries.Num(); i++) {
                    auto& Entry = PC->Inventory->Inventory.ReplicatedEntries[i];
                    if (Entry.ItemDefinition) {
                        std::string ItemName = Entry.ItemDefinition->Name.ToString();
                        if (ItemName.contains("Shotgun") || ItemName.contains("SMG") || ItemName.contains("Assault")
                            || ItemName.contains("Sniper") || ItemName.contains("Rocket") || ItemName.contains("Pistol")) {
                            Pawn->EquipWeaponDefinition((UFortWeaponItemDefinition*)Entry.ItemDefinition, Entry.ItemGuid);
                            break;
                        }
                    }
                }
            }
        }

        void EquipBuildingItem(EBotBuildingType BuildingType) {
            if (!Pawn || !Pawn->CurrentWeapon || !PC || !PC->Inventory) return;
            for (size_t i = 0; i < PC->Inventory->Inventory.ReplicatedEntries.Num(); i++) {
                auto& Entry = PC->Inventory->Inventory.ReplicatedEntries[i];
                if (!Entry.ItemDefinition->IsA(UFortBuildingItemDefinition::StaticClass())) continue;
                if (Entry.ItemDefinition) {
                    std::string ItemName = Entry.ItemDefinition->Name.ToString();
                    switch (BuildingType) {
                    case EBotBuildingType::Wall:
                        if (ItemName.contains("Wall")) Pawn->EquipWeaponDefinition((UFortWeaponItemDefinition*)Entry.ItemDefinition, Entry.ItemGuid);
                        break;
                    case EBotBuildingType::Floor:
                        if (ItemName.contains("Floor")) Pawn->EquipWeaponDefinition((UFortWeaponItemDefinition*)Entry.ItemDefinition, Entry.ItemGuid);
                        break;
                    case EBotBuildingType::Stair:
                        if (ItemName.contains("Stair")) Pawn->EquipWeaponDefinition((UFortWeaponItemDefinition*)Entry.ItemDefinition, Entry.ItemGuid);
                        break;
                    case EBotBuildingType::Cone:
                        if (ItemName.contains("Roof")) Pawn->EquipWeaponDefinition((UFortWeaponItemDefinition*)Entry.ItemDefinition, Entry.ItemGuid);
                        break;
                    case EBotBuildingType::Edit:
                        if (ItemName.contains("EditTool")) Pawn->EquipWeaponDefinition((UFortWeaponItemDefinition*)Entry.ItemDefinition, Entry.ItemGuid);
                        break;
                    }
                }
            }
        }
    };

    class BotsBTService_InventoryManager {
    public:
        static void Update(AFortAthenaAIBotController* PC, FFortItemEntry* ItemEntry = nullptr) {
            PC->Inventory->HandleInventoryLocalUpdate();
            PC->Inventory->bRequiresLocalUpdate = true;
            PC->Inventory->ForceNetUpdate();
            if (ItemEntry == nullptr) PC->Inventory->Inventory.MarkArrayDirty();
            else PC->Inventory->Inventory.MarkItemDirty(*ItemEntry);
        }

        static UFortWorldItem* CreateNewItem(AFortAthenaAIBotController* PC, UFortItemDefinition* ItemDefinition, int Count) {
            UFortWorldItem* NewItem = (UFortWorldItem*)ItemDefinition->CreateTemporaryItemInstanceBP(Count, 1);
            return NewItem;
        }

        static UFortWorldItem* GiveItem(AFortAthenaAIBotController* PC, UFortItemDefinition* ItemDefinition, int Count, int LoadedAmmo = 0) {
            UFortWorldItem* NewInstance = CreateNewItem(PC, ItemDefinition, Count);
            NewInstance->ItemEntry.Count = Count;
            NewInstance->ItemEntry.LoadedAmmo = LoadedAmmo;
            PC->Inventory->Inventory.ReplicatedEntries.Add(NewInstance->ItemEntry);
            return NewInstance;
        }

        static UFortItemDefinition* FindItemDefinition(AFortAthenaAIBotController* PC, const FGuid& Guid) {
            if (!PC) return nullptr;
            for (int i = 0; i < PC->Inventory->Inventory.ReplicatedEntries.Num(); i++) {
                FFortItemEntry* ItemEntries = &PC->Inventory->Inventory.ReplicatedEntries[i];
                if (!ItemEntries) continue;
                if (ItemEntries->ItemGuid == Guid) return ItemEntries->ItemDefinition;
            }
            return nullptr;
        }

        FFortItemEntry* FindItemEntry(AFortAthenaAIBotController* PC, UFortItemDefinition* ItemDefinition) {
            if (!PC || !ItemDefinition) return nullptr;
            for (int i = 0; i < PC->Inventory->Inventory.ReplicatedEntries.Num(); i++) {
                FFortItemEntry* ItemEntry = &PC->Inventory->Inventory.ReplicatedEntries[i];
                if (!ItemEntry) continue;
                if (ItemEntry->ItemDefinition == ItemDefinition) return ItemEntry;
            }
            return nullptr;
        }

        void RemoveItem(AFortAthenaAIBotController* PC, UFortItemDefinition* ItemDefinition, int Count = 1) {
            if (!PC || !ItemDefinition) return;
            for (int i = 0; i < PC->Inventory->Inventory.ReplicatedEntries.Num(); i++) {
                FFortItemEntry* ItemEntry = &PC->Inventory->Inventory.ReplicatedEntries[i];
                if (!ItemEntry) continue;
                if (ItemEntry->ItemDefinition == ItemDefinition) {
                    if (ItemEntry->Count > Count) {
                        ItemEntry->Count -= Count;
                        Update(PC, ItemEntry);
                    } else {
                        PC->Inventory->Inventory.ReplicatedEntries.Remove(i);
                        Update(PC);
                    }
                    break;
                }
            }
        }

        bool IsPrimary(UFortItemDefinition* ItemDefinition) {
            if (!ItemDefinition) return false;
            if (!ItemDefinition->IsA(UFortWeaponMeleeItemDefinition::StaticClass()) && !ItemDefinition->IsA(UFortEditToolItemDefinition::StaticClass()) && !ItemDefinition->IsA(UFortBuildingItemDefinition::StaticClass()) && !ItemDefinition->IsA(UFortAmmoItemDefinition::StaticClass()) && !ItemDefinition->IsA(UFortResourceItemDefinition::StaticClass()) && !ItemDefinition->IsA(UFortTrapItemDefinition::StaticClass())) return true;
            return false;
        }

        static EFortQuickBars GetQuickBars(UFortItemDefinition* ItemDefinition) {
            if (!ItemDefinition->IsA(UFortWeaponMeleeItemDefinition::StaticClass()) &&
                !ItemDefinition->IsA(UFortEditToolItemDefinition::StaticClass()) &&
                !ItemDefinition->IsA(UFortBuildingItemDefinition::StaticClass()) &&
                !ItemDefinition->IsA(UFortAmmoItemDefinition::StaticClass()) &&
                !ItemDefinition->IsA(UFortResourceItemDefinition::StaticClass()) &&
                !ItemDefinition->IsA(UFortTrapItemDefinition::StaticClass()))
                return EFortQuickBars::Primary;
            return EFortQuickBars::Secondary;
        }

        bool IsFullInventory(AFortAthenaAIBotController* PC) {
            if (!PC || !PC->Inventory) return true;
            static constexpr int Max = 5;
            int Count = 0;
            for (int i = 0; i < PC->Inventory->Inventory.ReplicatedEntries.Num(); i++) {
                FFortItemEntry* Entries = &PC->Inventory->Inventory.ReplicatedEntries[i];
                if (!Entries) continue;
                if (!Entries->ItemDefinition) continue;
                if (GetQuickBars(Entries->ItemDefinition) == EFortQuickBars::Primary) {
                    ++Count;
                    if (Count >= Max) return true;
                }
            }
            return false;
        }

        void DropItem(PlayerBot* bot, UFortItemDefinition* ItemDef) {
            FFortItemEntry* ItemToDrop = FindItemEntry(bot->PC, ItemDef);
            if (!ItemToDrop) return;
            FVector Drop = bot->Pawn->K2_GetActorLocation() + bot->Pawn->GetActorForwardVector() * 100.f;
            SpawnPickup(ItemDef, ItemToDrop->Count, ItemToDrop->LoadedAmmo, Drop, EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::Unset, bot->Pawn);
            RemoveItem(bot->PC, ItemDef, ItemToDrop->Count);
            Update(bot->PC);
        }

        bool IsCorrectItemType(EBotLootingFilter LootingFilter, UFortItemDefinition* ItemDef) {
            if (!ItemDef) return false;
            std::string ItemName = ItemDef->Name.ToString();
            switch (LootingFilter) {
            case EBotLootingFilter::Weapons:
                if (ItemDef->ItemType == EFortItemType::WeaponRanged) return true;
            case EBotLootingFilter::Shields:
                if (ItemName.contains("Shield") || ItemName.contains("ChugJug") || ItemName.contains("Slurp")) return true;
            case EBotLootingFilter::BigShields:
                if (ItemName.contains("Shield") && !ItemName.contains("Small") && !ItemName.contains("Mini") || ItemName.contains("ChugJug") || ItemName.contains("Slurp")) return true;
            case EBotLootingFilter::Meds:
                if (ItemName.contains("Medkit") || ItemName.contains("Bandage") || ItemName.contains("ChugJug") || ItemName.contains("Slurp")) return true;
            case EBotLootingFilter::BigMeds:
                if (ItemName.contains("Medkit") || ItemName.contains("ChugJug") || ItemName.contains("Slurp")) return true;
            case EBotLootingFilter::Ammo:
                if (ItemDef->ItemType == EFortItemType::Ammo) return true;
            case EBotLootingFilter::MAX:
                return true;
            }
            return false;
        }

        EBotSwapItemType GetSwapTypeFromDef(UFortItemDefinition* ItemDef) {
            std::string ItemDefName = ItemDef->Name.ToString();
            EBotSwapItemType ItemDefSwapType = EBotSwapItemType::MAX;
            if (ItemDefName.contains("Shotgun") || ItemDefName.contains("SMG") || ItemDefName.contains("Pistol")) ItemDefSwapType = EBotSwapItemType::CloseRangeGun;
            if (ItemDefName.contains("Assault") || ItemDefName.contains("Rocket")) ItemDefSwapType = EBotSwapItemType::MediumRangeGun;
            if (ItemDefName.contains("Sniper")) ItemDefSwapType = EBotSwapItemType::LongRangeGun;
            if (ItemDefName.contains("Medkit") || ItemDefName.contains("Bandage")) ItemDefSwapType = EBotSwapItemType::HealthHeals;
            if (ItemDefName.contains("Shield")) ItemDefSwapType = EBotSwapItemType::ShieldHeals;
            return ItemDefSwapType;
        }

        bool ShouldTakeItem(PlayerBot* bot, UFortItemDefinition* ItemDef) {
            if (!ItemDef) return false;
            if (ItemDef->ItemType == EFortItemType::Ammo || ItemDef->IsA(UFortResourceItemDefinition::StaticClass())) return true;
            std::string ItemDefName = ItemDef->Name.ToString();
            EBotSwapItemType ItemDefSwapType = GetSwapTypeFromDef(ItemDef);
            int HealthHealsCount = 0;
            int ShieldHealsCount = 0;
            int ShortRangeGunCount = 0;
            int MediumRangeGunCount = 0;
            int LongRangeGunCount = 0;
            for (FFortItemEntry ItemEntry : bot->PC->Inventory->Inventory.ReplicatedEntries) {
                UFortItemDefinition* InvDef = ItemEntry.ItemDefinition;
                if (!InvDef || InvDef->ItemType == EFortItemType::Ammo || ItemDef->IsA(UFortResourceItemDefinition::StaticClass())) continue;
                EBotSwapItemType CurrentSwapItemType = GetSwapTypeFromDef(InvDef);
                if (CurrentSwapItemType == EBotSwapItemType::Other) {
                    DropItem(bot, InvDef);
                    continue;
                }
                if (CurrentSwapItemType == ItemDefSwapType && CurrentSwapItemType > EBotSwapItemType::ShieldHeals &&
                    ItemDefSwapType > EBotSwapItemType::ShieldHeals && ItemDefSwapType < EBotSwapItemType::Other) {
                    if (ItemDef->Rarity > InvDef->Rarity) return true;
                }
                switch (CurrentSwapItemType) {
                case EBotSwapItemType::HealthHeals: HealthHealsCount++; break;
                case EBotSwapItemType::ShieldHeals: ShieldHealsCount++; break;
                case EBotSwapItemType::CloseRangeGun: ShortRangeGunCount++; break;
                case EBotSwapItemType::MediumRangeGun: MediumRangeGunCount++; break;
                case EBotSwapItemType::LongRangeGun: LongRangeGunCount++; break;
                default: break;
                }
            }
            switch (ItemDefSwapType) {
            case EBotSwapItemType::HealthHeals: if (HealthHealsCount == 0) return true; break;
            case EBotSwapItemType::ShieldHeals: if (ShieldHealsCount == 0) return true; break;
            case EBotSwapItemType::CloseRangeGun: if (ShortRangeGunCount == 0) return true; break;
            case EBotSwapItemType::MediumRangeGun: if (MediumRangeGunCount == 0) return true; break;
            case EBotSwapItemType::LongRangeGun: if (LongRangeGunCount == 0) return true; break;
            default: break;
            }
            return false;
        }

        UFortItemDefinition* WantsToSwapItemFor(PlayerBot* bot, UFortItemDefinition* ItemDef) {
            if (!ItemDef) return nullptr;
            if (!IsFullInventory(bot->PC)) return nullptr;
            std::string ItemDefName = ItemDef->Name.ToString();
            UFortItemDefinition* ItemDefTargetedSwap = nullptr;
            EBotSwapItemType ItemDefSwapType = GetSwapTypeFromDef(ItemDef);
            EBotSwapItemType SwapItemType = EBotSwapItemType::MAX;
            int HealthHealsCount = 0;
            int ShieldHealsCount = 0;
            int ShortRangeGunCount = 0;
            int MediumRangeGunCount = 0;
            int LongRangeGunCount = 0;
            for (FFortItemEntry ItemEntry : bot->PC->Inventory->Inventory.ReplicatedEntries) {
                UFortItemDefinition* InvDef = ItemEntry.ItemDefinition;
                if (!InvDef || InvDef->ItemType == EFortItemType::Ammo || InvDef->IsA(UFortResourceItemDefinition::StaticClass())) continue;
                std::string InvDefName = InvDef->Name.ToString();
                EBotSwapItemType CurrentSwapItemType = GetSwapTypeFromDef(InvDef);
                if (CurrentSwapItemType == EBotSwapItemType::CloseRangeGun) {
                    ShortRangeGunCount++;
                    if (ItemDef->Rarity > InvDef->Rarity && ItemDefSwapType == CurrentSwapItemType) {
                        ItemDefTargetedSwap = InvDef;
                        SwapItemType = EBotSwapItemType::CloseRangeGun;
                    }
                    if (ShortRangeGunCount > 1) {
                        ItemDefTargetedSwap = InvDef;
                        SwapItemType = EBotSwapItemType::CloseRangeGun;
                    }
                    break;
                }
                if (CurrentSwapItemType == EBotSwapItemType::MediumRangeGun) {
                    MediumRangeGunCount++;
                    if (ItemDef->Rarity > InvDef->Rarity && ItemDefSwapType == CurrentSwapItemType) {
                        ItemDefTargetedSwap = InvDef;
                        SwapItemType = EBotSwapItemType::MediumRangeGun;
                    }
                    if (MediumRangeGunCount > 1) {
                        ItemDefTargetedSwap = InvDef;
                        SwapItemType = EBotSwapItemType::MediumRangeGun;
                    }
                    break;
                }
                if (CurrentSwapItemType == EBotSwapItemType::LongRangeGun) {
                    LongRangeGunCount++;
                    if (ItemDef->Rarity > InvDef->Rarity && ItemDefSwapType == CurrentSwapItemType) {
                        ItemDefTargetedSwap = InvDef;
                        SwapItemType = EBotSwapItemType::LongRangeGun;
                    }
                    if (LongRangeGunCount > 1) {
                        ItemDefTargetedSwap = InvDef;
                        SwapItemType = EBotSwapItemType::LongRangeGun;
                    }
                    break;
                }
                if (CurrentSwapItemType == EBotSwapItemType::HealthHeals) {
                    HealthHealsCount++;
                    if (InvDefName.contains("Bandage") && ItemDef->Name.ToString().contains("Medkit")) {
                        ItemDefTargetedSwap = InvDef;
                        SwapItemType = EBotSwapItemType::HealthHeals;
                    }
                    if (HealthHealsCount > 1) {
                        ItemDefTargetedSwap = InvDef;
                        SwapItemType = EBotSwapItemType::HealthHeals;
                    }
                    break;
                }
                if (CurrentSwapItemType == EBotSwapItemType::ShieldHeals) {
                    ShieldHealsCount++;
                    if ((InvDefName.contains("Small") || !InvDefName.contains("Mini")) &&
                        (ItemDefName.contains("Shield") && (!ItemDefName.contains("Small") || !ItemDefName.contains("Mini")))) {
                        ItemDefTargetedSwap = InvDef;
                        SwapItemType = EBotSwapItemType::ShieldHeals;
                    }
                    if (ShieldHealsCount > 1) {
                        ItemDefTargetedSwap = InvDef;
                        SwapItemType = EBotSwapItemType::ShieldHeals;
                    }
                    break;
                }
                if (InvDef->ItemType == EFortItemType::Weapon || InvDef->ItemType == EFortItemType::WeaponRanged || InvDef->ItemType == EFortItemType::Consumable) {
                    ItemDefTargetedSwap = InvDef;
                    SwapItemType = EBotSwapItemType::Other;
                }
            }
            if (ItemDefTargetedSwap) return ItemDefTargetedSwap;
            return nullptr;
        }

        AFortPickup* GetNearestFilteredPickup(PlayerBot* bot) {
            AFortGameStateAthena* GameState = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;
            TArray<AActor*> PickupArray;
            UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), AFortPickup::StaticClass(), &PickupArray);
            AFortPickup* NearestPickup = nullptr;
            float NearestDistance = FLT_MAX;
            for (AActor* PickupActor : PickupArray) {
                AFortPickup* Pickup = (AFortPickup*)PickupActor;
                if (!Pickup || Pickup->bHidden) continue;
                FFortItemEntry ItemEntry = Pickup->GetItemEntry(0);
                if (!ItemEntry.ItemDefinition) continue;
                std::string ItemName = ItemEntry.ItemDefinition->Name.ToString();
                bool bCorrectPickup = false;
                if (IsFullInventory(bot->PC)) {
                    if (WantsToSwapItemFor(bot, ItemEntry.ItemDefinition)) bCorrectPickup = true;
                } else bCorrectPickup = ShouldTakeItem(bot, ItemEntry.ItemDefinition);
                if (bCorrectPickup) {
                    float DistanceTo = Pickup->GetDistanceTo(bot->Pawn);
                    if (DistanceTo < NearestDistance) {
                        NearestDistance = DistanceTo;
                        NearestPickup = Pickup;
                    }
                }
            }
            PickupArray.Free();
            return (AFortPickup*)NearestPickup;
        }

        void CompletePickupAnimation(PlayerBot* bot, AFortPickup* Pickup) {
            FFortPickupLocationData& PickupLocationData = Pickup->PickupLocationData;
            FFortItemEntry& PickupEntry = Pickup->PrimaryPickupItemEntry;
            UFortItemDefinition* PickupItemDefinition = PickupEntry.ItemDefinition;
            if (!PickupItemDefinition) return;
            FVector Drop = bot->Pawn->K2_GetActorLocation() + bot->Pawn->GetActorForwardVector() * 100.f;
            EBotSwapItemType PickupSwapItemType = GetSwapTypeFromDef(PickupItemDefinition);
            UFortItemDefinition* ItemToSwap = WantsToSwapItemFor(bot, PickupItemDefinition);
            if (ItemToSwap) {
                FFortItemEntry* SwapItemEntry = FindItemEntry(bot->PC, ItemToSwap);
                if (SwapItemEntry && SwapItemEntry->ItemDefinition) {
                    Log("ItemToSwap: " + SwapItemEntry->ItemDefinition->Name.ToString());
                    bot->Pawn->EquipWeaponDefinition((UFortWeaponItemDefinition*)SwapItemEntry->ItemDefinition, SwapItemEntry->ItemGuid);
                }
            } else {
                if (!IsFullInventory(bot->PC)) {
                    if (!bot || !bot->PC || !bot->PC->Inventory) {
                        Log("Smth is null!");
                        SpawnPickup(PickupItemDefinition, PickupEntry.Count, PickupEntry.LoadedAmmo, Drop, EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::Unset, bot->Pawn);
                        return;
                    }
                    UFortItemDefinition* ItemToDrop = nullptr;
                    for (FFortItemEntry InvEntry : bot->PC->Inventory->Inventory.ReplicatedEntries) {
                        UFortItemDefinition* InvDef = InvEntry.ItemDefinition;
                        if (!InvDef || InvDef->ItemType == EFortItemType::Ammo || InvDef->IsA(UFortResourceItemDefinition::StaticClass())) continue;
                        if (GetSwapTypeFromDef(InvDef) == PickupSwapItemType && PickupItemDefinition->Rarity > InvDef->Rarity) {
                            ItemToDrop = InvDef;
                            break;
                        }
                    }
                    if (ItemToDrop) DropItem(bot, ItemToDrop);
                }
            }
            int PickupCount = PickupEntry.Count;
            int PickupLoadedAmmo = PickupEntry.LoadedAmmo;
            int PickupMaxStackSize = PickupItemDefinition->MaxStackSize;
            FFortItemEntry* ItemEntry = FindItemEntry(bot->PC, PickupItemDefinition);
            if (!ItemEntry && PickupItemDefinition->IsStackable()) {
                if (IsFullInventory(bot->PC)) {
                    AFortWeapon* CurrentWeapon = bot->Pawn->CurrentWeapon;
                    if (CurrentWeapon) {
                        UFortItemDefinition* OldWeaponDef = CurrentWeapon->WeaponData;
                        if (OldWeaponDef && OldWeaponDef->IsA(UFortWeaponMeleeItemDefinition::StaticClass())) {
                            if (GetQuickBars(PickupItemDefinition) == EFortQuickBars::Secondary) {
                                GiveItem(bot->PC, PickupItemDefinition, PickupCount, PickupLoadedAmmo);
                                Update(bot->PC);
                            } else {
                                SpawnPickup(PickupItemDefinition, PickupCount, PickupLoadedAmmo, Drop, EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::Unset, bot->Pawn);
                            }
                        } else {
                            if (GetQuickBars(PickupItemDefinition) == EFortQuickBars::Primary) {
                                FFortItemEntry* WeaponItemEntry = FindItemEntry(bot->PC, CurrentWeapon->WeaponData);
                                if (!WeaponItemEntry) return;
                                int OldWeaponCount = WeaponItemEntry->Count;
                                int OldWeaponAmmo = WeaponItemEntry->LoadedAmmo;
                                SpawnPickup(OldWeaponDef, OldWeaponCount, OldWeaponAmmo, Drop, EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::Unset, bot->Pawn);
                                RemoveItem(bot->PC, OldWeaponDef, OldWeaponCount);
                                GiveItem(bot->PC, PickupItemDefinition, PickupCount, PickupLoadedAmmo);
                                Update(bot->PC);
                            } else if (GetQuickBars(PickupItemDefinition) == EFortQuickBars::Secondary) {
                                GiveItem(bot->PC, PickupItemDefinition, PickupCount, PickupLoadedAmmo);
                                Update(bot->PC);
                            }
                        }
                        Pickup->K2_DestroyActor();
                        return;
                    } else {
                        SpawnPickup(PickupItemDefinition, PickupCount, PickupLoadedAmmo, Drop, EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::Unset, bot->Pawn);
                        Pickup->K2_DestroyActor();
                        return;
                    }
                } else {
                    GiveItem(bot->PC, PickupItemDefinition, PickupCount, PickupLoadedAmmo);
                    Update(bot->PC);
                    Pickup->K2_DestroyActor();
                    return;
                }
            }
            if (PickupItemDefinition->IsStackable() && ItemEntry && ItemEntry->Count < PickupMaxStackSize) {
                int Combined = ItemEntry->Count + PickupEntry.Count;
                if (Combined <= PickupMaxStackSize) {
                    ItemEntry->Count = Combined;
                    Update(bot->PC, ItemEntry);
                } else {
                    ItemEntry->Count = PickupMaxStackSize;
                    Update(bot->PC, ItemEntry);
                    int Overflow = Combined - PickupMaxStackSize;
                    GiveItem(bot->PC, PickupItemDefinition, PickupCount, PickupLoadedAmmo);
                }
                Pickup->K2_DestroyActor();
                return;
            }
            if (IsFullInventory(bot->PC)) {
                AFortWeapon* CurrentWeapon = bot->Pawn->CurrentWeapon;
                if (CurrentWeapon) {
                    UFortItemDefinition* OldWeaponDef = CurrentWeapon->WeaponData;
                    if (OldWeaponDef && OldWeaponDef->IsA(UFortWeaponMeleeItemDefinition::StaticClass())) {
                        if (GetQuickBars(PickupItemDefinition) == EFortQuickBars::Secondary) {
                            GiveItem(bot->PC, PickupItemDefinition, PickupCount, PickupLoadedAmmo);
                            Update(bot->PC);
                        } else {
                            SpawnPickup(PickupItemDefinition, PickupCount, PickupLoadedAmmo, Drop, EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::Unset, bot->Pawn);
                        }
                    } else {
                        if (GetQuickBars(PickupItemDefinition) == EFortQuickBars::Primary) {
                            FFortItemEntry* WeaponItemEntry = FindItemEntry(bot->PC, CurrentWeapon->WeaponData);
                            if (!WeaponItemEntry) return;
                            int OldWeaponCount = WeaponItemEntry->Count;
                            int OldWeaponAmmo = WeaponItemEntry->LoadedAmmo;
                            SpawnPickup(OldWeaponDef, OldWeaponCount, OldWeaponAmmo, Drop, EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::Unset, bot->Pawn);
                            RemoveItem(bot->PC, OldWeaponDef, OldWeaponCount);
                            GiveItem(bot->PC, PickupItemDefinition, PickupCount, PickupLoadedAmmo);
                            Update(bot->PC);
                        } else if (FortInventory::GetQuickBars(PickupItemDefinition) == EFortQuickBars::Secondary) {
                            GiveItem(bot->PC, PickupItemDefinition, PickupCount, PickupLoadedAmmo);
                            Update(bot->PC);
                        }
                    }
                    Pickup->K2_DestroyActor();
                    return;
                } else {
                    SpawnPickup(PickupItemDefinition, PickupCount, PickupLoadedAmmo, Drop, EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::Unset, bot->Pawn);
                    Pickup->K2_DestroyActor();
                    return;
                }
            }
            GiveItem(bot->PC, PickupItemDefinition, PickupCount, PickupLoadedAmmo);
            Update(bot->PC);
            Pickup->K2_DestroyActor();
            return;
        }
    };

    class BotsBTService_Healing {
    public:
        bool IsMaximumHealingReached(PlayerBot* bot, UFortItemDefinition* ItemDef) {
            if (!bot || !bot->Pawn || !ItemDef) return true;
            std::string ItemName = ItemDef->Name.ToString();
            float Health = bot->Pawn->GetHealth();
            float Shield = bot->Pawn->GetShield();
            bool bMaximumHealingReached = false;
            if (ItemName.contains("Medkit") && Health >= 100.f) bMaximumHealingReached = true;
            if (ItemName.contains("Bandage") && Health >= 75.f) bMaximumHealingReached = true;
            if ((ItemName.contains("Shield") && !ItemName.contains("Small") && !ItemName.contains("Mini")) && Shield >= 100.f) bMaximumHealingReached = true;
            if ((ItemName.contains("Small") || ItemName.contains("Mini")) && Shield >= 50.f) bMaximumHealingReached = true;
            return bMaximumHealingReached;
        }

        FFortItemEntry GetCurrentHealingItemEntry(PlayerBot* bot) {
            float Health = bot->Pawn->GetHealth();
            float Shield = bot->Pawn->GetShield();
            for (FFortItemEntry ItemEntry : bot->PC->Inventory->Inventory.ReplicatedEntries) {
                if (!ItemEntry.ItemDefinition) continue;
                std::string ItemName = ItemEntry.ItemDefinition->Name.ToString();
                bool bFound = false;
                if (bot->CurrentHealingType == EBotHealingType::Health || bot->CurrentHealingType == EBotHealingType::Both) {
                    if (Health >= 75.f) {
                        if (ItemName.contains("Medkit")) bFound = true;
                    } else {
                        if (ItemName.contains("Medkit") || ItemName.contains("Bandage")) bFound = true;
                    }
                }
                if (bot->CurrentHealingType == EBotHealingType::Shield || bot->CurrentHealingType == EBotHealingType::Both) {
                    if (Shield >= 75) {
                        if (ItemName.contains("Shield") || !ItemName.contains("Small") || !ItemName.contains("Mini")) bFound = true;
                    } else {
                        if (ItemName.contains("Shield")) bFound = true;
                    }
                }
                if (bFound) return ItemEntry;
            }
            return FFortItemEntry{};
        }

        bool ShouldHeal(PlayerBot* bot) {
            float Health = bot->Pawn->GetHealth();
            float Shield = bot->Pawn->GetShield();
            if (Health != 100 && Shield != 100) {
                bot->CurrentHealingType = EBotHealingType::Both;
                return true;
            } else if (Health != 100) {
                bot->CurrentHealingType = EBotHealingType::Health;
                return true;
            } else if (Shield != 100) {
                bot->CurrentHealingType = EBotHealingType::Shield;
                return true;
            } else {
                bot->CurrentHealingType = EBotHealingType::MAX;
                return false;
            }
        }

        bool CanHeal(PlayerBot* bot) {
            if (!ShouldHeal(bot)) return false;
            if (bot->bPotentiallyUnderAttack) return false;
            FFortItemEntry HealingItemEntry = GetCurrentHealingItemEntry(bot);
            if (!HealingItemEntry.ItemDefinition) return false;
            if (IsMaximumHealingReached(bot, HealingItemEntry.ItemDefinition)) return false;
            return true;
        }

        void StartHealing(PlayerBot* bot) {
            if (!CanHeal(bot)) {
                if (bot->bPotentiallyUnderAttack) {
                    if (bot->HasGun()) {
                        bot->SimpleSwitchToWeapon();
                        bot->BotState = EBotState::Combat;
                    } else {
                        bot->EquipPickaxe();
                        bot->BotState = EBotState::Looting;
                    }
                } else {
                    bot->EquipPickaxe();
                    bot->BotState = EBotState::Looting;
                }
                return;
            }
            FFortItemEntry HealingItemEntry = GetCurrentHealingItemEntry(bot);
            if (!HealingItemEntry.ItemDefinition) {
                bot->EquipPickaxe();
                bot->BotState = EBotState::Looting;
                return;
            }
            bot->Pawn->EquipWeaponDefinition((UFortWeaponItemDefinition*)HealingItemEntry.ItemDefinition, HealingItemEntry.ItemGuid);
            bot->Pawn->PawnStartFire(0);
        }
    };

    class BotsBTService_SoundDetection {
    public:
        void DetectSounds(PlayerBot* bot) {
            if (!bot || !bot->Pawn) return;
            float CurrentTime = UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld());
            auto GameMode = (AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode;
            float DetectionRadius = 5000.f;
            FVector BotLoc = bot->LastUpdatedBotLocation;
            
            // Detect gunshots from other players
            for (auto* Player : GameMode->AlivePlayers) {
                if (!Player || !Player->Pawn || Player->Pawn == bot->Pawn) continue;
                AFortPlayerPawnAthena* OtherPawn = (AFortPlayerPawnAthena*)Player->Pawn;
                if (!OtherPawn->CurrentWeapon) continue;
                float Dist = OtherPawn->GetDistanceTo(bot->Pawn);
                if (Dist < DetectionRadius ) {
                    FSoundMemory Sound;
                    Sound.SoundType = EBotSoundType::Gunshot;
                    Sound.Location = OtherPawn->K2_GetActorLocation();
                    Sound.DetectionTime = CurrentTime;
                    Sound.Confidence = 1.0f - (Dist / DetectionRadius);
                    Sound.SourcePawn = OtherPawn;
                    bot->LastDetectedSound = Sound;
                    if (bot->BotState != EBotState::Combat && bot->BotState != EBotState::InvestigatingSound) {
                        if (UKismetMathLibrary::GetDefaultObj()->RandomBoolWithWeight(0.7f)) {
                            bot->BotState = EBotState::InvestigatingSound;
                        }
                    }
                    return;
                }
            }
            
            // Detect gunshots from other bots
            for (auto* OtherBot : GameMode->AliveBots) {
                if (!OtherBot || !OtherBot->Pawn || OtherBot->Pawn == bot->Pawn) continue;
                AFortPlayerPawnAthena* OtherPawn = (AFortPlayerPawnAthena*)OtherBot->Pawn;
                if (!OtherPawn->CurrentWeapon) continue;
                float Dist = OtherPawn->GetDistanceTo(bot->Pawn);
                if (Dist < DetectionRadius ) {
                    FSoundMemory Sound;
                    Sound.SoundType = EBotSoundType::Gunshot;
                    Sound.Location = OtherPawn->K2_GetActorLocation();
                    Sound.DetectionTime = CurrentTime;
                    Sound.Confidence = 1.0f - (Dist / DetectionRadius);
                    Sound.SourcePawn = OtherPawn;
                    bot->LastDetectedSound = Sound;
                    if (bot->BotState != EBotState::Combat && bot->BotState != EBotState::InvestigatingSound) {
                        if (UKismetMathLibrary::GetDefaultObj()->RandomBoolWithWeight(0.5f)) {
                            bot->BotState = EBotState::InvestigatingSound;
                        }
                    }
                    return;
                }
            }
        }

        void InvestigateSound(PlayerBot* bot) {
            if (!bot || !bot->Pawn) return;
            float CurrentTime = UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld());
            if (!bot->LastDetectedSound.IsValid(CurrentTime, 45.f)) {
                bot->BotState = EBotState::Looting;
                return;
            }
            FVector BotLoc = bot->LastUpdatedBotLocation;
            FVector SoundLoc = bot->LastDetectedSound.Location;
            float Dist = UKismetMathLibrary::GetDefaultObj()->Vector_Distance(BotLoc, SoundLoc);
            
            // Cautious approach based on personality
            float ApproachDistance = 1500.f;
            if (bot->Personality == EBotPersonality::Aggressive) ApproachDistance = 800.f;
            else if (bot->Personality == EBotPersonality::Tactical) ApproachDistance = 2000.f;
            
            if (Dist > ApproachDistance) {
                FRotator LookAtRot = UKismetMathLibrary::GetDefaultObj()->FindLookAtRotation(BotLoc, SoundLoc);
                bot->PC->SetControlRotation(LookAtRot);
                bot->PC->K2_SetActorRotation(LookAtRot, true);
                // Move cautiously
                if (UKismetMathLibrary::GetDefaultObj()->RandomBoolWithWeight(0.3f)) bot->Pawn->Crouch(false);
                bot->PC->MoveToLocation(SoundLoc, ApproachDistance, true, false, true, true, nullptr, true);
            } else {
                // Close enough, evaluate threat
                if (bot->LastDetectedSound.SourcePawn && bot->HasGun()) {
                    float HealthPercent = bot->Pawn->GetHealth() / bot->Pawn->GetMaxHealth();
                    if (HealthPercent > 0.5f && bot->Personality != EBotPersonality::Tactical) {
                        bot->BotState = EBotState::Combat;
                        bot->NearestPlayerPawn = bot->LastDetectedSound.SourcePawn;
                    } else {
                        // Hold position or retreat
                        bot->BotState = EBotState::Looting;
                    }
                } else {
                    bot->BotState = EBotState::Looting;
                }
            }
        }
    };

    class BotsBTService_AIEvaluator {
    public:
        bool IsStressed(AFortPlayerPawnAthena* Pawn) {
            if (Pawn->GetHealth() <= 75) return true;
            return false;
        }

        EBotWarmupChoice PickWarmupChoice() {
            return (EBotWarmupChoice)UKismetMathLibrary::GetDefaultObj()->RandomIntegerInRange(0, (int)EBotWarmupChoice::MAX - 1);
        }

        EBotLootingFilter GetCurrentLootingChoice(PlayerBot* bot) {
            BotsBTService_Healing HealingManager;
            float Health = bot->Pawn->GetHealth();
            float Shield = bot->Pawn->GetShield();
            FFortItemEntry HealingItemEntry = HealingManager.GetCurrentHealingItemEntry(bot);
            if (bot->BotState == EBotState::Warmup) {
                if (!bot->HasGun()) return EBotLootingFilter::Weapons;
                else return EBotLootingFilter::MAX;
            }
            if (!bot->HasGun()) return EBotLootingFilter::Weapons;
            else if (Health != 100 && !HealingItemEntry.ItemDefinition) {
                if (Health >= 75) return EBotLootingFilter::BigMeds;
                else return EBotLootingFilter::Meds;
            } else if (Shield != 100 && !HealingItemEntry.ItemDefinition) {
                if (Shield >= 75) return EBotLootingFilter::BigShields;
                else return EBotLootingFilter::Shields;
            }
            return EBotLootingFilter::MAX;
        }

    public:
        void Tick(PlayerBot* bot) {
            BotsBTService_InventoryManager InventoryManager;
            BotsBTService_Healing HealingManager;
            BotsBTService_SoundDetection SoundDetection;

            if (!bot || !bot->Pawn || bot->PlayerState->IsPlayerDead() || bot->Pawn->IsDead()) return;
            bot->LastUpdatedBotLocation = bot->Pawn->K2_GetActorLocation();
            FVector Vel = bot->Pawn->GetVelocity();
            float Speed = 0.f;
            if (!Vel.IsZero()) Speed = sqrtf(Vel.X * Vel.X + Vel.Y * Vel.Y);

            // NEW: Natural movement - check if stuck
            float CurrentTime = UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld());
            if (CurrentTime - bot->StuckCheckTime > 3.f) {
                float DistMoved = UKismetMathLibrary::GetDefaultObj()->Vector_Distance(bot->LastStuckCheckPosition, bot->LastUpdatedBotLocation);
                if (DistMoved < 100.f && bot->BotState != EBotState::Warmup) {
                    bot->StuckCounter++;
                    if (bot->StuckCounter > 2) {
                        // Bot is stuck, try to jump or change direction
                        bot->Pawn->Jump();
                        bot->StuckCounter = 0;
                    }
                } else bot->StuckCounter = 0;
                bot->LastStuckCheckPosition = bot->LastUpdatedBotLocation;
                bot->StuckCheckTime = CurrentTime;
            }

            if (!bot->NearestChest || !bot->NearestPickup || bot->NearestChest->bDestroyed) {
                AActor* LastLootable = nullptr;
                if (bot->ClosestLootableType == ELootableType::Chest) LastLootable = bot->NearestChest;
                else LastLootable = bot->NearestPickup;
                bot->UpdateLootableReservation(LastLootable, true);
            }

            if (bot->tick_counter % 30 == 0) {
                if (UKismetMathLibrary::RandomBool()) bot->PC->K2_ClearFocus();
            }

            if (bot->tick_counter % 150 == 0) {
                bot->NearestPlayerPawn = bot->GetNearestPawn();
                if (!bot->Pawn->bIsDBNO) {
                    AActor* LastLootable = nullptr;
                    if (bot->ClosestLootableType == ELootableType::Chest) LastLootable = bot->NearestChest;
                    else LastLootable = bot->NearestPickup;
                    bot->UpdateLootableReservation(LastLootable, true);
                    bot->NearestChest = bot->GetNearestChest();
                    bot->NearestPickup = InventoryManager.GetNearestFilteredPickup(bot);
                    if (bot->NearestChest && bot->NearestPickup) {
                        if (bot->NearestChest->GetDistanceTo(bot->Pawn) > bot->NearestPickup->GetDistanceTo(bot->Pawn)) {
                            bot->ClosestLootableLoc = bot->NearestPickup->K2_GetActorLocation();
                            bot->ClosestLootableType = ELootableType::Pickup;
                            bot->UpdateLootableReservation(bot->NearestPickup, false);
                        } else {
                            bot->ClosestLootableLoc = bot->NearestChest->K2_GetActorLocation();
                            bot->ClosestLootableType = ELootableType::Chest;
                            bot->UpdateLootableReservation(bot->NearestChest, false);
                        }
                    } else if (bot->NearestChest) {
                        bot->ClosestLootableLoc = bot->NearestChest->K2_GetActorLocation();
                        bot->ClosestLootableType = ELootableType::Chest;
                        bot->UpdateLootableReservation(bot->NearestChest, false);
                    } else if (bot->NearestPickup) {
                        bot->ClosestLootableLoc = bot->NearestPickup->K2_GetActorLocation();
                        bot->ClosestLootableType = ELootableType::Pickup;
                        bot->UpdateLootableReservation(bot->NearestPickup, false);
                    }
                }
            }

            if (bot->tick_counter % 30 == 0) {
                if (bot->BotState >= EBotState::Landed) {
                    if (HealingManager.CanHeal(bot)) bot->BotState = EBotState::Healing;
                }
            }

            // NEW: Sound detection
            if (bot->tick_counter % 5 == 0 && bot->BotState >= EBotState::Landed) {
                SoundDetection.DetectSounds(bot);
            }

            if (bot->tick_counter % 2 == 0) {
                if (bot->bHasJumpedFromBus) {
                    bot->bPotentiallyUnderAttack = bot->PC->LineOfSightTo(bot->NearestPlayerPawn, bot->LastUpdatedBotLocation, true);
                    if (bot->bPotentiallyUnderAttack) {
                        if (bot->HasGun()) bot->BotState = EBotState::Combat;
                        else bot->PC->K2_SetFocus(bot->NearestPlayerPawn);
                    }
                }
            }

            if (bot->tick_counter % 10 == 0) {
                bot->CurrentLootingFilter = GetCurrentLootingChoice(bot);
                if (bot->Pawn->bIsCrouched && UKismetMathLibrary::GetDefaultObj()->RandomBoolWithWeight(0.025f)) bot->Pawn->UnCrouch(false);
                AFortGameStateAthena* GameState = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;
                bot->bHasCompletedCurrentAction = false;
                bot->bIsStressed = IsStressed(bot->Pawn);
                if (GameState->GamePhase >= EAthenaGamePhase::SafeZones) {
                    if (GameState->SafeZoneIndicator && !GameState->SafeZoneIndicator->NextCenter.IsZero()) {
                        float DistanceToZone = UKismetMathLibrary::GetDefaultObj()->Vector_Distance(bot->LastUpdatedBotLocation, GameState->SafeZoneIndicator->NextCenter);
                        bot->bIsInSafeZone = DistanceToZone < GameState->SafeZoneIndicator->NextRadius;
                    }
                }
            }

            if ((bot->tick_counter % 30 == 0) && Speed >= 100 && bot->BotState >= EBotState::Landed) bot->Run();
        }
    };

    class BotsBTService_Building {
    public:
        bool CanBuild(PlayerBot* bot, EBotBuildingType BuildType) {
            if (!bot || !bot->Pawn || !bot->PC) return false;
            float CurrentTime = UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld());
            if (CurrentTime - bot->LastBuildTime < bot->BuildCooldown) return false;
            int32 MaterialCost = 10;
            if (bot->WoodCount < MaterialCost) return false;
            return true;
        }

        void ConsumeMaterials(PlayerBot* bot, EBotBuildingType BuildType) {
            int32 MaterialCost = 10;
            bot->WoodCount -= MaterialCost;
            if (bot->WoodCount < 0) bot->WoodCount = 0;
        }

        bool PlaceBuild(PlayerBot* bot, EBotBuildingType BuildType, FVector TargetLocation) {
            if (!CanBuild(bot, BuildType)) return false;
            bot->EquipBuildingItem(BuildType);
            bot->Pawn->BuildingState = EFortBuildingState::Placement;
            bot->Pawn->PawnStartFire(0);
            Sleep(50);
            bot->Pawn->PawnStopFire(0);
            ConsumeMaterials(bot, BuildType);
            bot->LastBuildTime = UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld());
            return true;
        }

        void ExecuteDefensiveWall(PlayerBot* bot) {
            if (!bot->NearestPlayerPawn) {
                bot->bIsBuilding = false;
                return;
            }
            FVector BotLoc = bot->LastUpdatedBotLocation;
            FVector EnemyLoc = bot->NearestPlayerPawn->K2_GetActorLocation();
            FVector Direction = EnemyLoc - BotLoc;
            Direction.Normalize();
            FVector WallPos = BotLoc + (Direction * 150.0f);
            PlaceBuild(bot, EBotBuildingType::Wall, WallPos);
            bot->bIsBuilding = false;
        }

        void ExecuteDefensiveBox(PlayerBot* bot) {
            FVector BotLoc = bot->LastUpdatedBotLocation;
            if (bot->CurrentBuildSequenceStep == 0) {
                bot->BuildTargetLocation = BotLoc + FVector(150, 0, 0);
                PlaceBuild(bot, EBotBuildingType::Wall, bot->BuildTargetLocation);
                bot->CurrentBuildSequenceStep++;
            } else if (bot->CurrentBuildSequenceStep == 1) {
                bot->BuildTargetLocation = BotLoc + FVector(0, 150, 0);
                PlaceBuild(bot, EBotBuildingType::Wall, bot->BuildTargetLocation);
                bot->CurrentBuildSequenceStep++;
            } else if (bot->CurrentBuildSequenceStep == 2) {
                bot->BuildTargetLocation = BotLoc + FVector(-150, 0, 0);
                PlaceBuild(bot, EBotBuildingType::Wall, bot->BuildTargetLocation);
                bot->CurrentBuildSequenceStep++;
            } else if (bot->CurrentBuildSequenceStep == 3) {
                bot->BuildTargetLocation = BotLoc + FVector(0, -150, 0);
                PlaceBuild(bot, EBotBuildingType::Wall, bot->BuildTargetLocation);
                bot->CurrentBuildSequenceStep = 0;
                bot->bIsBuilding = false;
            }
        }

        void ExecuteTurtle(PlayerBot* bot) {
            FVector BotLoc = bot->LastUpdatedBotLocation;
            if (bot->CurrentBuildSequenceStep == 0) {
                PlaceBuild(bot, EBotBuildingType::Wall, BotLoc + FVector(150, 0, 0));
                bot->CurrentBuildSequenceStep++;
            } else if (bot->CurrentBuildSequenceStep == 1) {
                PlaceBuild(bot, EBotBuildingType::Wall, BotLoc + FVector(0, 150, 0));
                bot->CurrentBuildSequenceStep++;
            } else if (bot->CurrentBuildSequenceStep == 2) {
                PlaceBuild(bot, EBotBuildingType::Wall, BotLoc + FVector(-150, 0, 0));
                bot->CurrentBuildSequenceStep++;
            } else if (bot->CurrentBuildSequenceStep == 3) {
                PlaceBuild(bot, EBotBuildingType::Wall, BotLoc + FVector(0, -150, 0));
                bot->CurrentBuildSequenceStep++;
            } else if (bot->CurrentBuildSequenceStep == 4) {
                PlaceBuild(bot, EBotBuildingType::Floor, BotLoc);
                bot->CurrentBuildSequenceStep++;
            } else if (bot->CurrentBuildSequenceStep == 5) {
                PlaceBuild(bot, EBotBuildingType::Cone, BotLoc + FVector(0, 0, 300));
                bot->CurrentBuildSequenceStep = 0;
                bot->bIsBuilding = false;
            }
        }

        void ExecuteRampRush(PlayerBot* bot) {
            if (!bot->NearestPlayerPawn) {
                bot->bIsBuilding = false;
                return;
            }
            FVector BotLoc = bot->LastUpdatedBotLocation;
            FVector EnemyLoc = bot->NearestPlayerPawn->K2_GetActorLocation();
            FVector Direction = EnemyLoc - BotLoc;
            Direction.Normalize();
            if (bot->CurrentBuildSequenceStep == 0) {
                FVector FloorPos = BotLoc + (Direction * 150.0f);
                PlaceBuild(bot, EBotBuildingType::Floor, FloorPos);
                bot->CurrentBuildSequenceStep++;
            } else if (bot->CurrentBuildSequenceStep == 1) {
                FVector StairPos = BotLoc + (Direction * 200.0f);
                PlaceBuild(bot, EBotBuildingType::Stair, StairPos);
                bot->PC->MoveToLocation(StairPos, 50.0f, false, true, false, true, nullptr, true);
                bot->CurrentBuildSequenceStep = 0;
                bot->bIsBuilding = false;
            }
        }

        void ExecuteNineties(PlayerBot* bot) {
            FVector BotLoc = bot->LastUpdatedBotLocation;
            FVector ForwardVec = bot->Pawn->GetActorForwardVector();
            if (bot->CurrentBuildSequenceStep == 0) {
                PlaceBuild(bot, EBotBuildingType::Wall, BotLoc + ForwardVec * 100.0f);
                bot->CurrentBuildSequenceStep++;
            } else if (bot->CurrentBuildSequenceStep == 1) {
                PlaceBuild(bot, EBotBuildingType::Stair, BotLoc + ForwardVec * 50.0f);
                bot->CurrentBuildSequenceStep++;
            } else if (bot->CurrentBuildSequenceStep == 2) {
                FVector RightVec = bot->Pawn->GetActorRightVector();
                bot->PC->SetControlRotation(UKismetMathLibrary::GetDefaultObj()->MakeRotFromX(RightVec));
                PlaceBuild(bot, EBotBuildingType::Wall, BotLoc + RightVec * 100.0f);
                bot->CurrentBuildSequenceStep++;
            } else if (bot->CurrentBuildSequenceStep == 3) {
                FVector RightVec = bot->Pawn->GetActorRightVector();
                PlaceBuild(bot, EBotBuildingType::Stair, BotLoc + RightVec * 50.0f);
                bot->CurrentBuildSequenceStep = 0;
                bot->bIsBuilding = false;
            }
        }

        void ExecuteHighGroundRetake(PlayerBot* bot) {
            if (!bot->NearestPlayerPawn) {
                bot->bIsBuilding = false;
                return;
            }
            FVector BotLoc = bot->LastUpdatedBotLocation;
            FVector EnemyLoc = bot->NearestPlayerPawn->K2_GetActorLocation();
            if (bot->CurrentBuildSequenceStep == 0) {
                PlaceBuild(bot, EBotBuildingType::Wall, BotLoc + FVector(0, 0, 0));
                bot->CurrentBuildSequenceStep++;
            } else if (bot->CurrentBuildSequenceStep == 1) {
                PlaceBuild(bot, EBotBuildingType::Stair, BotLoc + FVector(0, 0, 50));
                bot->CurrentBuildSequenceStep++;
            } else if (bot->CurrentBuildSequenceStep == 2) {
                PlaceBuild(bot, EBotBuildingType::Floor, BotLoc + FVector(0, 0, 300));
                bot->CurrentBuildSequenceStep = 0;
                bot->bIsBuilding = false;
            }
        }

        void ExecuteBuildPattern(PlayerBot* bot, EBotBuildPattern Pattern) {
            if (!bot || !bot->Pawn) return;
            bot->bIsBuilding = true;
            bot->CurrentBuildPattern = Pattern;
            switch (Pattern) {
            case EBotBuildPattern::DefensiveWall: ExecuteDefensiveWall(bot); break;
            case EBotBuildPattern::DefensiveBox: ExecuteDefensiveBox(bot); break;
            case EBotBuildPattern::Turtle: ExecuteTurtle(bot); break;
            case EBotBuildPattern::RampRush: ExecuteRampRush(bot); break;
            case EBotBuildPattern::Nineties: ExecuteNineties(bot); break;
            case EBotBuildPattern::HighGroundRetake: ExecuteHighGroundRetake(bot); break;
            default: bot->bIsBuilding = false; break;
            }
        }

        EBotBuildPattern GetBuildPatternForPersonality(PlayerBot* bot) {
            if (!bot || !bot->NearestPlayerPawn) return EBotBuildPattern::None;
            FVector BotLoc = bot->LastUpdatedBotLocation;
            FVector EnemyLoc = bot->NearestPlayerPawn->K2_GetActorLocation();
            float HeightDiff = EnemyLoc.Z - BotLoc.Z;
            float Dist = UKismetMathLibrary::GetDefaultObj()->Vector_Distance(BotLoc, EnemyLoc);
            
            switch (bot->Personality) {
            case EBotPersonality::Aggressive:
                if (bot->bIsStressed && bot->Pawn->GetHealth() < 50) return EBotBuildPattern::DefensiveWall;
                if (Dist < 800 && HeightDiff < -200) return EBotBuildPattern::RampRush;
                return EBotBuildPattern::None;
            case EBotPersonality::Builder:
                if (bot->bIsStressed) return EBotBuildPattern::Turtle;
                if (HeightDiff > 200) return EBotBuildPattern::HighGroundRetake;
                if (Dist < 1500 && UKismetMathLibrary::GetDefaultObj()->RandomBoolWithWeight(0.3f)) return EBotBuildPattern::Nineties;
                return EBotBuildPattern::DefensiveBox;
            case EBotPersonality::Tactical:
                if (bot->bIsStressed || bot->Pawn->GetHealth() < 60) return EBotBuildPattern::DefensiveBox;
                if (HeightDiff > 150) return EBotBuildPattern::HighGroundRetake;
                if (Dist < 600) return EBotBuildPattern::DefensiveWall;
                return EBotBuildPattern::None;
            default: // Balanced
                if (bot->bIsStressed) return EBotBuildPattern::DefensiveBox;
                if (HeightDiff > 200) return EBotBuildPattern::RampRush;
                if (bot->bPotentiallyUnderAttack) return EBotBuildPattern::DefensiveWall;
                return EBotBuildPattern::None;
            }
        }

        void Tick(PlayerBot* bot) {
            if (!bot || !bot->Pawn || !bot->PC) return;
            if (bot->bIsBuilding && bot->CurrentBuildPattern != EBotBuildPattern::None) {
                ExecuteBuildPattern(bot, bot->CurrentBuildPattern);
                return;
            }
            if (bot->BotState == EBotState::Combat) {
                EBotBuildPattern Pattern = GetBuildPatternForPersonality(bot);
                if (Pattern != EBotBuildPattern::None) ExecuteBuildPattern(bot, Pattern);
            }
        }
    };

    class BotsBTService_Warmup {
    public:
        void Tick(PlayerBot* bot) {
            if (bot->WarmupChoice == EBotWarmupChoice::MAX) return;

            if (bot->WarmupChoice == EBotWarmupChoice::Emote) {
                if (bot->tick_counter % 300 == 0) bot->Emote();
            }
            else if (bot->WarmupChoice == EBotWarmupChoice::ApproachPlayersAndEmote) {
                if (bot->tick_counter % 200 == 0) {
                    FVector BotPos = bot->Pawn->K2_GetActorLocation();
                    if (bot->NearestPlayerPawn) {
                        FVector Nearest = bot->NearestPlayerPawn->K2_GetActorLocation();
                        if (!Nearest.IsZero()) {
                            float Dist = UKismetMathLibrary::GetDefaultObj()->Vector_Distance(BotPos, Nearest);
                            if (Dist < 200.f + rand() % 300) {
                                bot->LookAt(bot->NearestPlayerPawn);
                                if (UKismetMathLibrary::GetDefaultObj()->RandomBool()) bot->Emote();
                            } else {
                                bot->LookAt(bot->NearestPlayerPawn);
                                bot->PC->MoveToActor(bot->NearestPlayerPawn, 100, true, false, true, nullptr, true);
                            }
                        }
                    }
                }
            }
            else if (bot->WarmupChoice == EBotWarmupChoice::LookAtRandomPlayers) {
                if (bot->tick_counter % 90 == 0) {
                    APawn* RandomPawn = bot->GetRandomPawn();
                    if (RandomPawn) bot->LookAt(RandomPawn);
                }
            }
            else if (bot->WarmupChoice == EBotWarmupChoice::PickaxeOtherPlayers) {
                if (bot->tick_counter % 200 == 0) {
                    FVector BotPos = bot->Pawn->K2_GetActorLocation();
                    if (!bot->CurrentlyFocusedPawn) bot->CurrentlyFocusedPawn = bot->GetRandomPawn();
                    if (bot->CurrentlyFocusedPawn) {
                        FVector Nearest = bot->CurrentlyFocusedPawn->K2_GetActorLocation();
                        if (!Nearest.IsZero()) {
                            float Dist = UKismetMathLibrary::GetDefaultObj()->Vector_Distance(BotPos, Nearest);
                            if (Dist < 180.f) {
                                bot->LookAt(bot->CurrentlyFocusedPawn);
                                if (UKismetMathLibrary::GetDefaultObj()->RandomBool()) bot->Pawn->PawnStartFire(0);
                            } else {
                                bot->Pawn->PawnStopFire(0);
                                bot->LookAt(bot->CurrentlyFocusedPawn);
                                bot->PC->MoveToActor(bot->CurrentlyFocusedPawn, 100, true, false, true, nullptr, true);
                            }
                        }
                    }
                }
            }
            else if (bot->WarmupChoice == EBotWarmupChoice::FightOtherPlayers) {
                FVector BotPos = bot->Pawn->K2_GetActorLocation();
                if (!bot->HasGun()) {
                    bot->Pawn->PawnStopFire(0);
                    if (bot->NearestPickup) {
                        FVector Nearest = bot->NearestPickup->K2_GetActorLocation();
                        if (!Nearest.IsZero()) {
                            float Dist = UKismetMathLibrary::GetDefaultObj()->Vector_Distance(BotPos, Nearest);
                            if (Dist < 200.f) {
                                bot->LookAt(bot->CurrentlyFocusedPawn);
                                bot->Pickup(bot->NearestPickup);
                            } else {
                                bot->LookAt(bot->CurrentlyFocusedPawn);
                                bot->PC->MoveToActor(bot->CurrentlyFocusedPawn, 100, true, false, true, nullptr, true);
                            }
                        }
                    }
                } else {
                    if (bot->IsPickaxeEquiped()) bot->SimpleSwitchToWeapon();
                    if (bot->NearestPlayerPawn) {
                        FVector Nearest = bot->NearestPlayerPawn->K2_GetActorLocation();
                        if (!((AFortPlayerStateAthena*)bot->NearestPlayerPawn->PlayerState)->bInAircraft || !Nearest.IsZero()) {
                            if (bot->PC->LineOfSightTo(bot->NearestPlayerPawn, BotPos, true)) {
                                bot->Pawn->PawnStartFire(0);
                                bot->LookAt(bot->NearestPlayerPawn);
                                bot->PC->MoveToActor(bot->NearestPlayerPawn, 300, true, false, true, nullptr, true);
                                bot->ForceStrafe(true);
                            }
                        }
                    }
                }
            }
            else if (bot->WarmupChoice == EBotWarmupChoice::BuildStructures) {
                if (bot->tick_counter % 100 == 0) {
                    int RandomBuild = UKismetMathLibrary::GetDefaultObj()->RandomIntegerInRange(0, 3);
                    EBotBuildingType BuildType = (EBotBuildingType)RandomBuild;
                    bot->EquipBuildingItem(BuildType);
                    bot->Pawn->BuildingState = EFortBuildingState::Placement;
                    bot->Pawn->PawnStartFire(0);
                    Sleep(50);
                    bot->Pawn->PawnStopFire(0);
                }
            }
            else if (bot->WarmupChoice == EBotWarmupChoice::RunAround) {
                if (bot->tick_counter % 150 == 0) {
                    float RandomX = UKismetMathLibrary::GetDefaultObj()->RandomFloatInRange(-2000.f, 2000.f);
                    float RandomY = UKismetMathLibrary::GetDefaultObj()->RandomFloatInRange(-2000.f, 2000.f);
                    FVector BotLoc = bot->LastUpdatedBotLocation;
                    FVector TargetLoc = FVector(BotLoc.X + RandomX, BotLoc.Y + RandomY, BotLoc.Z);
                    bot->LastWarmupMoveLocation = TargetLoc;
                    bot->PC->MoveToLocation(TargetLoc, 50.0f, false, true, false, true, nullptr, true);
                }
                if (bot->tick_counter % 30 == 0) bot->Run();
            }
            else if (bot->WarmupChoice == EBotWarmupChoice::JumpAround) {
                if (bot->tick_counter % 150 == 0) {
                    float RandomX = UKismetMathLibrary::GetDefaultObj()->RandomFloatInRange(-1500.f, 1500.f);
                    float RandomY = UKismetMathLibrary::GetDefaultObj()->RandomFloatInRange(-1500.f, 1500.f);
                    FVector BotLoc = bot->LastUpdatedBotLocation;
                    FVector TargetLoc = FVector(BotLoc.X + RandomX, BotLoc.Y + RandomY, BotLoc.Z);
                    bot->LastWarmupMoveLocation = TargetLoc;
                    bot->PC->MoveToLocation(TargetLoc, 50.0f, false, true, false, true, nullptr, true);
                }
                if (bot->tick_counter % 45 == 0) bot->Pawn->Jump();
            }
            else if (bot->WarmupChoice == EBotWarmupChoice::PracticeBuilding) {
                if (bot->tick_counter % 120 == 0) {
                    int RandomBuild = UKismetMathLibrary::GetDefaultObj()->RandomIntegerInRange(0, 3);
                    EBotBuildingType BuildType = (EBotBuildingType)RandomBuild;
                    bot->EquipBuildingItem(BuildType);
                    bot->Pawn->BuildingState = EFortBuildingState::Placement;
                    bot->Pawn->PawnStartFire(0);
                    Sleep(50);
                    bot->Pawn->PawnStopFire(0);
                    if (UKismetMathLibrary::GetDefaultObj()->RandomBoolWithWeight(0.3f)) {
                        bot->EquipPickaxe();
                        bot->Pawn->PawnStartFire(0);
                        Sleep(100);
                        bot->Pawn->PawnStopFire(0);
                    }
                }
                if (bot->tick_counter % 60 == 0) {
                    float RandomX = UKismetMathLibrary::GetDefaultObj()->RandomFloatInRange(-500.f, 500.f);
                    float RandomY = UKismetMathLibrary::GetDefaultObj()->RandomFloatInRange(-500.f, 500.f);
                    FVector BotLoc = bot->LastUpdatedBotLocation;
                    FVector TargetLoc = FVector(BotLoc.X + RandomX, BotLoc.Y + RandomY, BotLoc.Z);
                    bot->PC->MoveToLocation(TargetLoc, 50.0f, false, true, false, true, nullptr, true);
                }
            }
            // NEW: Social grouping behavior
            else if (bot->WarmupChoice == EBotWarmupChoice::SocialGroup) {
                if (bot->tick_counter % 180 == 0) {
                    // Find a group of other bots/players
                    FVector GroupCenter = FVector(0, 0, 0);
                    int NearbyCount = 0;
                    auto GameMode = (AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode;
                    for (auto* OtherBot : GameMode->AliveBots) {
                        if (OtherBot && OtherBot->Pawn && OtherBot->Pawn != bot->Pawn) {
                            float Dist = OtherBot->Pawn->GetDistanceTo(bot->Pawn);
                            if (Dist < 1000.f) {
                                GroupCenter = GroupCenter + OtherBot->Pawn->K2_GetActorLocation();
                                NearbyCount++;
                            }
                        }
                    }
                    if (NearbyCount > 0) {
                        GroupCenter.X /= NearbyCount;
                        GroupCenter.Y /= NearbyCount;
                        GroupCenter.Z = bot->LastUpdatedBotLocation.Z;
                        // Move towards group but keep some distance
                        FVector Direction = bot->LastUpdatedBotLocation - GroupCenter;
                        Direction.Normalize();
                        FVector TargetPos = GroupCenter + Direction * 300.f;
                        bot->PC->MoveToLocation(TargetPos, 100.0f, false, true, false, true, nullptr, true);
                    } else {
                        // No group found, explore
                        float RandomX = UKismetMathLibrary::GetDefaultObj()->RandomFloatInRange(-1000.f, 1000.f);
                        float RandomY = UKismetMathLibrary::GetDefaultObj()->RandomFloatInRange(-1000.f, 1000.f);
                        FVector TargetLoc = bot->LastUpdatedBotLocation + FVector(RandomX, RandomY, 0);
                        bot->PC->MoveToLocation(TargetLoc, 100.0f, false, true, false, true, nullptr, true);
                    }
                }
                if (bot->tick_counter % 240 == 0 && UKismetMathLibrary::GetDefaultObj()->RandomBoolWithWeight(0.3f)) {
                    bot->Emote();
                }
            }
            // NEW: Explore spawn island
            else if (bot->WarmupChoice == EBotWarmupChoice::ExploreSpawnIsland) {
                if (bot->tick_counter % 200 == 0) {
                    // Pick a random direction and walk
                    float RandomAngle = UKismetMathLibrary::GetDefaultObj()->RandomFloatInRange(0.f, 360.f);
                    float Distance = UKismetMathLibrary::GetDefaultObj()->RandomFloatInRange(500.f, 2000.f);
                    float Rad = RandomAngle * 3.14159f / 180.f;
                    FVector Offset((float)cos(Rad) * Distance, (float)sin(Rad) * Distance, 0);
                    FVector TargetLoc = bot->LastUpdatedBotLocation + Offset;
                    bot->PC->MoveToLocation(TargetLoc, 50.0f, false, true, false, true, nullptr, true);
                    // Occasionally look around
                    if (UKismetMathLibrary::GetDefaultObj()->RandomBoolWithWeight(0.4f)) {
                        APawn* RandomPawn = bot->GetRandomPawn();
                        if (RandomPawn) bot->LookAt(RandomPawn);
                    }
                }
                if (bot->tick_counter % 60 == 0) bot->Run();
            }
            // NEW: Dance in group
            else if (bot->WarmupChoice == EBotWarmupChoice::DanceInGroup) {
                // Check if near other players
                bool bNearOthers = false;
                auto GameMode = (AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode;
                for (auto* OtherBot : GameMode->AliveBots) {
                    if (OtherBot && OtherBot->Pawn && OtherBot->Pawn != bot->Pawn) {
                        if (OtherBot->Pawn->GetDistanceTo(bot->Pawn) < 500.f) {
                            bNearOthers = true;
                            break;
                        }
                    }
                }
                if (bNearOthers && bot->tick_counter % 120 == 0) {
                    bot->Emote();
                } else if (!bNearOthers && bot->tick_counter % 180 == 0) {
                    // Move towards others
                    for (auto* OtherBot : GameMode->AliveBots) {
                        if (OtherBot && OtherBot->Pawn && OtherBot->Pawn != bot->Pawn) {
                            bot->PC->MoveToActor(OtherBot->Pawn, 300.f, true, false, true, nullptr, true);
                            break;
                        }
                    }
                }
            }
        }
    };

    class BotsBTService_AIDropZone {
    public:
        void ChooseDropZone(PlayerBot* bot) {
            if (BuildingFoundations.Num() == 0) return;
            AActor* DropZone = BuildingFoundations[UKismetMathLibrary::GetDefaultObj()->RandomIntegerInRange(0, BuildingFoundations.Num() - 1)];
            if (DropZone) bot->TargetDropZone = DropZone->K2_GetActorLocation();
            bot->TargetDropZone.X += UKismetMathLibrary::GetDefaultObj()->RandomFloatInRange(-800.f, 800.f);
            bot->TargetDropZone.Y += UKismetMathLibrary::GetDefaultObj()->RandomFloatInRange(-800.f, 800.f);
        }

    public:
        void Tick(PlayerBot* bot) {
            auto GameState = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;
            auto Math = (UKismetMathLibrary*)UKismetMathLibrary::StaticClass()->DefaultObject;
            auto GameMode = (AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode;
            auto Statics = (UGameplayStatics*)UGameplayStatics::StaticClass()->DefaultObject;

            if (bot->TargetDropZone.IsZero()) {
                ChooseDropZone(bot);
                return;
            }

            if (bot->BotState == EBotState::PreBus) {
                if (!bot->bHasThankedBusDriver && GameState->GamePhase == EAthenaGamePhase::Aircraft && Math->RandomBoolWithWeight(0.0005f)) {
                    bot->bHasThankedBusDriver = true;
                    bot->PlayerState->bThankedBusDriver = true;
                    bot->PlayerState->OnRep_ThankedBusDriver();
                }
            }

            if (bot->BotState == EBotState::Bus) {
                bot->Pawn->SetShield(0);
                bot->CurrentlyFocusedPawn = nullptr;
                bot->bHasCompletedCurrentAction = false;
                bot->EquipPickaxe();
                if (bot->bHasJumpedFromBus) {
                    bot->BotState = EBotState::Skydiving;
                    return;
                }
                if (!bot->bHasThankedBusDriver && GameState->GamePhase == EAthenaGamePhase::Aircraft && Math->RandomBoolWithWeight(0.0005f)) {
                    bot->bHasThankedBusDriver = true;
                    bot->PlayerState->bThankedBusDriver = true;
                    bot->PlayerState->OnRep_ThankedBusDriver();
                }
                AActor* Bus = GameState->GetAircraft(0);
                if (!Bus) return;
                FVector BusLocation = Bus->K2_GetActorLocation();
                FVector DropTarget = bot->TargetDropZone;
                DropTarget.Z = BusLocation.Z;
                if (GameState->GamePhase > EAthenaGamePhase::Aircraft) {
                    Log("Force Jump");
                    // MODIFIED: No teleport, use natural skydiving
                    bot->Pawn->BeginSkydiving(true);
                    bot->BotState = EBotState::Skydiving;
                    bot->bHasJumpedFromBus = true;
                    return;
                }
                float DistanceToDrop = Math->Vector_Distance(BusLocation, DropTarget);
                if (DistanceToDrop < bot->ClosestDistToDropZone) {
                    bot->ClosestDistToDropZone = DistanceToDrop;
                } else {
                    if (!bot->bHasThankedBusDriver && Math->RandomBoolWithWeight(0.5f)) {
                        bot->bHasThankedBusDriver = true;
                        bot->PlayerState->bThankedBusDriver = true;
                        bot->PlayerState->OnRep_ThankedBusDriver();
                    }
                    if (Math->RandomBoolWithWeight(0.75)) {
                        // MODIFIED: No teleport, use natural skydiving from current position
                        bot->Pawn->BeginSkydiving(true);
                        bot->BotState = EBotState::Skydiving;
                        bot->bHasJumpedFromBus = true;
                    }
                }
                return;
            }

            auto BotPos = bot->Pawn->K2_GetActorLocation();
            if (!bot->TargetDropZone.IsZero()) bot->TargetDropZone.Z = BotPos.Z;

            if (bot->BotState == EBotState::Skydiving) {
                if (!bot->Pawn->bIsSkydiving) bot->BotState = EBotState::Gliding;
                if (!bot->TargetDropZone.IsZero()) {
                    bot->Pawn->AddMovementInput(Math->NegateVector(bot->Pawn->GetActorUpVector()), 1, true);
                    float Dist = Math->Vector_Distance(BotPos, bot->TargetDropZone);
                    auto TestRot = Math->FindLookAtRotation(BotPos, bot->TargetDropZone);
                    bot->PC->SetControlRotation(TestRot);
                    bot->PC->K2_SetActorRotation(TestRot, true);
                    bot->PC->MoveToLocation(bot->TargetDropZone, 200.f, true, false, false, true, nullptr, true);
                }
            }
            else if (bot->BotState == EBotState::Gliding) {
                if (bot->Pawn->bIsSkydiving) bot->BotState = EBotState::Skydiving;
                FVector Vel = bot->Pawn->GetVelocity();
                float Speed = Vel.Z;
                if (Speed == 0.f || bot->Pawn->bIsInWaterVolume) {
                    bot->PC->StopMovement();
                    bot->BotState = EBotState::Landed;
                }
                if (!bot->TargetDropZone.IsZero()) {
                    float Dist = Math->Vector_Distance(BotPos, bot->TargetDropZone);
                    if (Dist < 300.f) {
                        AActor* TargetLocationToTravel = nullptr;
                        if (bot->ClosestLootableType == ELootableType::Chest) TargetLocationToTravel = bot->NearestChest;
                        else TargetLocationToTravel = bot->NearestPickup;
                        if (TargetLocationToTravel) {
                            auto TestRot = Math->FindLookAtRotation(BotPos, TargetLocationToTravel->K2_GetActorLocation());
                            bot->PC->SetControlRotation(TestRot);
                            bot->PC->K2_SetActorRotation(TestRot, true);
                            bot->LookAt(TargetLocationToTravel);
                            bot->PC->MoveToLocation(TargetLocationToTravel->K2_GetActorLocation(), 200.f, true, false, false, true, nullptr, true);
                        }
                    } else {
                        auto TestRot = Math->FindLookAtRotation(BotPos, bot->TargetDropZone);
                        bot->PC->SetControlRotation(TestRot);
                        bot->PC->K2_SetActorRotation(TestRot, true);
                        bot->PC->MoveToLocation(bot->TargetDropZone, 100.f, true, false, false, true, nullptr, true);
                    }
                }
            }
        }
    };

    class BotsBTService_Loot {
    public:
        void Tick(PlayerBot* bot, EBotLootingFilter LootingTarget = EBotLootingFilter::MAX) {
            BotsBTService_InventoryManager InventoryManager;
            auto Math = UKismetMathLibrary::GetDefaultObj();
            AActor* TargetLootable = bot->TargetLootable;
            if (TargetLootable) {
                FVector BotLoc = bot->Pawn->K2_GetActorLocation();
                if (!BotLoc.IsZero()) {
                    float Dist = Math->Vector_Distance(BotLoc, bot->ClosestLootableLoc);
                    auto TestRot = Math->FindLookAtRotation(BotLoc, bot->ClosestLootableLoc);
                    bot->PC->SetControlRotation(TestRot);
                    bot->PC->K2_SetActorRotation(TestRot, true);
                    bot->LookAt(TargetLootable);
                    float CurrentTime = UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld());
                    if (TargetLootable) {
                        if (bot->LootTargetStartTime == 0.f) {
                            bot->LootTargetStartTime = CurrentTime;
                            bot->LastLootTargetDistance = Dist;
                        }
                        float Elapsed = CurrentTime - bot->LootTargetStartTime;
                        // MODIFIED: Instead of teleporting, try to path around or jump
                        if ((Elapsed > 8.f && Dist > bot->LastLootTargetDistance - 100.f)) {
                            bot->Pawn->PawnStopFire(0);
                            bot->Pawn->Jump(); // Try jumping over obstacle
                            bot->LootTargetStartTime = 0.f;
                            return;
                        }
                        if (Dist < 300.f) bot->LootTargetStartTime = 0.f;
                        bot->LastLootTargetDistance = Dist;
                    }
                    if (Dist < 300.f) {
                        bot->PC->StopMovement();
                        bot->Pawn->PawnStopFire(0);
                        if (bot->ClosestLootableType == ELootableType::Pickup) {
                            AFortPickup* Pickup = bot->NearestPickup;
                            if (Pickup) bot->Pickup(Pickup);
                            bot->NearestPickup = nullptr;
                            bot->TimeToNextAction = 0;
                        } else if (!bot->TimeToNextAction || !bot->Pawn->bStartedInteractSearch && bot->ClosestLootableType == ELootableType::Chest) {
                            bot->TimeToNextAction = UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld());
                            bot->Pawn->bStartedInteractSearch = true;
                            bot->Pawn->OnRep_StartedInteractSearch();
                        } else if (UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld()) - bot->TimeToNextAction >= 1.5f && bot->ClosestLootableType == ELootableType::Chest) {
                            ABuildingContainer* Chest = (ABuildingContainer*)TargetLootable;
                            BuildingContainer::SpawnLoot(Chest);
                            Chest->bAlreadySearched = true;
                            bot->Pawn->bStartedInteractSearch = false;
                            bot->Pawn->OnRep_StartedInteractSearch();
                            bot->TimeToNextAction = 0;
                        }
                        TargetLootable = nullptr;
                        return;
                    } else if (Dist < 1000.f) {
                        bot->Pawn->PawnStartFire(0);
                        bot->PC->MoveToActor(TargetLootable, 50, true, false, true, nullptr, true);
                    } else {
                        bot->PC->MoveToActor(TargetLootable, 50, true, false, true, nullptr, true);
                    }
                }
            }
        }
    };

    class BotsBTService_Combat {
    public:
        void Tick(PlayerBot* bot) {
            bot->PC->K2_ClearFocus();
            auto Math = UKismetMathLibrary::GetDefaultObj();
            float CurrentTime = UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld());
            
            if (!bot->HasGun()) {
                bot->BotState = EBotState::Looting;
                return;
            }
            if (bot->IsPickaxeEquiped()) bot->SimpleSwitchToWeapon();
            
            FVector BotLoc = bot->Pawn->K2_GetActorLocation();
            if (bot->NearestPlayerPawn) {
                FVector Nearest = bot->NearestPlayerPawn->K2_GetActorLocation();
                bot->LastKnownEnemyPosition = Nearest;
                bot->LastCombatTime = CurrentTime;
                FRotator TestRot;
                FVector TargetPosmod = Nearest;
                if (!Nearest.IsZero()) {
                    float Dist = Math->Vector_Distance(BotLoc, Nearest);
                    if (bot->PC->LineOfSightTo(bot->NearestPlayerPawn, BotLoc, true)) {
                        // Aim with skill-based accuracy
                        float SkillMultiplier = (11 - bot->SkillLevel) / 10.f;
                        float RandomXmod = Math->RandomFloatInRange(-150 * SkillMultiplier, 150 * SkillMultiplier);
                        float RandomYmod = Math->RandomFloatInRange(-150 * SkillMultiplier, 150 * SkillMultiplier);
                        float RandomZmod = Math->RandomFloatInRange(-150 * SkillMultiplier, 150 * SkillMultiplier);
                        FVector TargetPosMod{ Nearest.X + RandomXmod, Nearest.Y + RandomYmod, Nearest.Z + RandomZmod };
                        FRotator Rot = Math->FindLookAtRotation(BotLoc, TargetPosMod);
                        bot->PC->SetControlRotation(Rot);
                        bot->PC->K2_SetActorRotation(Rot, true);
                        if (UKismetMathLibrary::GetDefaultObj()->RandomBoolWithWeight(0.001)) {
                            TestRot = Math->FindLookAtRotation(BotLoc, Nearest);
                            bot->PC->SetControlRotation(TestRot);
                            bot->PC->K2_SetActorRotation(TestRot, true);
                        }
                        if (!bot->Pawn->bIsCrouched && Math->RandomBoolWithWeight(0.025f)) bot->Pawn->Crouch(false);
                        
                        // Combat stance management
                        UpdateCombatStance(bot, Dist);
                        
                        // Behavior based on stance and personality
                        ExecuteCombatBehavior(bot, Dist);
                        
                        // Building integration based on personality
                        if (!bot->bIsBuilding && bot->WoodCount > 50) {
                            BotsBTService_Building BuildingService;
                            EBotBuildPattern Pattern = BuildingService.GetBuildPatternForPersonality(bot);
                            if (Pattern != EBotBuildPattern::None) BuildingService.ExecuteBuildPattern(bot, Pattern);
                        }
                    } else {
                        // Lost line of sight - try to reacquire or move to last known position
                        if (bot->Personality == EBotPersonality::Tactical && bot->SkillLevel >= 6) {
                            // Smart bots predict enemy movement
                            FVector PredictedPos = PredictEnemyPosition(bot);
                            bot->PC->MoveToLocation(PredictedPos, 100.f, true, false, true, true, nullptr, true);
                        } else {
                            bot->BotState = EBotState::MovingToSafeZone;
                        }
                    }
                }
            } else {
                // No enemy visible - check if we should investigate last known position
                if (!bot->LastKnownEnemyPosition.IsZero() && 
                    Math->Vector_Distance(BotLoc, bot->LastKnownEnemyPosition) > 500.f) {
                    bot->PC->MoveToLocation(bot->LastKnownEnemyPosition, 200.f, true, false, true, true, nullptr, true);
                } else {
                    bot->BotState = EBotState::Looting;
                }
            }
        }

    private:
        void UpdateCombatStance(PlayerBot* bot, float Distance) {
            float CurrentTime = UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld());
            bot->TimeInCurrentStance += 0.1f;
            
            // Change stance based on situation
            if (bot->bIsStressed && bot->Pawn->GetHealth() < 50) {
                bot->CombatStance = EBotCombatStance::Retreating;
            } else if (bot->Personality == EBotPersonality::Aggressive && Distance > 400) {
                bot->CombatStance = EBotCombatStance::Pushing;
            } else if (bot->Personality == EBotPersonality::Tactical && Distance < 300) {
                if (UKismetMathLibrary::GetDefaultObj()->RandomBoolWithWeight(0.3f)) {
                    bot->CombatStance = EBotCombatStance::Flanking;
                    CalculateFlankPosition(bot);
                }
            } else if (bot->TimeInCurrentStance > 5.f) {
                // Change stance periodically for variety
                bot->TimeInCurrentStance = 0.f;
                int RandomStance = UKismetMathLibrary::GetDefaultObj()->RandomIntegerInRange(0, 3);
                switch (RandomStance) {
                case 0: bot->CombatStance = EBotCombatStance::Engaged; break;
                case 1: bot->CombatStance = EBotCombatStance::Holding; break;
                case 2: bot->CombatStance = EBotCombatStance::Pushing; break;
                case 3: bot->CombatStance = (bot->Personality == EBotPersonality::Tactical) ? EBotCombatStance::Flanking : EBotCombatStance::Engaged; break;
                }
            }
        }

        void ExecuteCombatBehavior(PlayerBot* bot, float Distance) {
            switch (bot->CombatStance) {
            case EBotCombatStance::Engaged:
                bot->ForceStrafe(true);
                if (Distance < 1000) {
                    FVector BackVector = bot->Pawn->GetActorForwardVector() * -1.0f;
                    bot->Pawn->AddMovementInput(BackVector, 1.1f, true);
                }
                if (!bot->bIsStressed) {
                    bot->PC->MoveToActor(bot->NearestPlayerPawn, UKismetMathLibrary::GetDefaultObj()->RandomFloatInRange(400, 1500), true, false, true, nullptr, true);
                } else {
                    bot->Pawn->AddMovementInput((bot->Pawn->GetActorForwardVector() * -1.0f), 1.2f, true);
                }
                break;
                
            case EBotCombatStance::Pushing:
                // Aggressive push toward enemy
                if (Distance > 200) {
                    bot->PC->MoveToActor(bot->NearestPlayerPawn, 150.f, true, false, true, nullptr, true);
                    bot->Run();
                }
                break;
                
            case EBotCombatStance::Retreating:
                // Move away while still shooting
                bot->Pawn->AddMovementInput((bot->Pawn->GetActorForwardVector() * -1.0f), 1.5f, true);
                if (Distance < 1500) {
                    FVector AwayDir = bot->LastUpdatedBotLocation - bot->NearestPlayerPawn->K2_GetActorLocation();
                    AwayDir.Normalize();
                    FVector RetreatPos = bot->LastUpdatedBotLocation + AwayDir * 500.f;
                    bot->PC->MoveToLocation(RetreatPos, 100.f, true, false, true, nullptr, true);
                }
                break;
                
            case EBotCombatStance::Flanking:
                // Move to flank position
                if (!bot->FlankTargetPosition.IsZero()) {
                    bot->PC->MoveToLocation(bot->FlankTargetPosition, 100.f, true, false, true, nullptr, true);
                    float DistToFlank = UKismetMathLibrary::GetDefaultObj()->Vector_Distance(bot->LastUpdatedBotLocation, bot->FlankTargetPosition);
                    if (DistToFlank < 200.f) {
                        bot->CombatStance = EBotCombatStance::Engaged;
                        bot->bIsFlanking = false;
                    }
                }
                break;
                
            case EBotCombatStance::Holding:
                // Stay in place, crouch for accuracy
                if (!bot->Pawn->bIsCrouched) bot->Pawn->Crouch(false);
                break;
                
            default:
                bot->ForceStrafe(true);
                break;
            }
            
            if (bot->PC->LineOfSightTo(bot->NearestPlayerPawn, bot->LastUpdatedBotLocation, true)) {
                bot->Pawn->PawnStartFire(0);
            } else {
                bot->Pawn->PawnStopFire(0);
            }
        }

        void CalculateFlankPosition(PlayerBot* bot) {
            if (!bot->NearestPlayerPawn) return;
            FVector EnemyPos = bot->NearestPlayerPawn->K2_GetActorLocation();
            FVector BotPos = bot->LastUpdatedBotLocation;
            FVector EnemyToBot = BotPos - EnemyPos;
            EnemyToBot.Normalize();
            
            // Calculate perpendicular vector for flanking
            FVector PerpVector(-EnemyToBot.Y, EnemyToBot.X, 0);
            if (UKismetMathLibrary::GetDefaultObj()->RandomBool()) PerpVector = -PerpVector;
            
            float FlankDistance = UKismetMathLibrary::GetDefaultObj()->RandomFloatInRange(400.f, 800.f);
            bot->FlankTargetPosition = EnemyPos + PerpVector * FlankDistance;
            bot->bIsFlanking = true;
        }

        FVector PredictEnemyPosition(PlayerBot* bot) {
            if (!bot->NearestPlayerPawn) return bot->LastKnownEnemyPosition;
            FVector EnemyPos = bot->NearestPlayerPawn->K2_GetActorLocation();
            FVector EnemyVel = bot->NearestPlayerPawn->GetVelocity();
            // Predict where enemy will be in 0.5 seconds
            return EnemyPos + EnemyVel * 0.5f;
        }
    };

    void TickBots() {
        auto GameMode = (AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode;
        auto GameState = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;

        for (PlayerBot* Bot : PlayerBotArray) {
            if (!Bot || !Bot->PC || !Bot->Pawn || !Bot->PlayerState) continue;

            if (Bot->PlayerState->IsPlayerDead() || Bot->Pawn->IsDead()) {
                for (int i = 0; i < PlayerBotArray.size(); i++) {
                    if (PlayerBotArray[i]->Pawn == Bot->Pawn) {
                        for (int j = 0; j < GameMode->AliveBots.Num(); j++) {
                            AFortAthenaAIBotController* bot = GameMode->AliveBots[j];
                            if (Bot->PC == bot) {
                                GameMode->AliveBots.Remove(j);
                                GameState->PlayersLeft--;
                                GameState->OnRep_PlayersLeft();
                                for (int32 k = 0; k < Bot->PC->Inventory->Inventory.ReplicatedEntries.Num(); k++) {
                                    FFortItemEntry ItemEntry = Bot->PC->Inventory->Inventory.ReplicatedEntries[k];
                                    UFortItemDefinition* ItemDef = ItemEntry.ItemDefinition;
                                    if (!ItemDef) continue;
                                    if (ItemDef->IsA(UAthenaPickaxeItemDefinition::StaticClass()) || ItemDef->IsA(UFortWeaponMeleeItemDefinition::StaticClass())) continue;
                                    if (!((UFortWorldItemDefinition*)ItemDef)->bCanBeDropped) continue;
                                    SpawnPickup(ItemDef, ItemEntry.Count, ItemEntry.LoadedAmmo, Bot->LastUpdatedBotLocation, EFortPickupSourceTypeFlag::Other, EFortPickupSpawnSource::PlayerElimination);
                                }
                                break;
                            }
                        }
                        delete PlayerBotArray[i];
                        PlayerBotArray.erase(PlayerBotArray.begin() + i);
                        Log("Freed a dead bot from the array!");
                    }
                }
                continue;
            }

            if (GameState->GamePhase <= EAthenaGamePhase::Warmup) {
                if (Bot->tick_counter <= 150) {
                    Bot->tick_counter++;
                    continue;
                }
            }

            if (Bot->bPauseTicking) continue;

            BotsBTService_AIEvaluator AIEvaluator;
            AIEvaluator.Tick(Bot);
            
            BotsBTService_SoundDetection SoundDetection;

            // Initialize bot personality and skill on first tick
            if (Bot->tick_counter == 0) {
                int PersonalityRoll = UKismetMathLibrary::GetDefaultObj()->RandomIntegerInRange(0, 99);
                if (PersonalityRoll < 35) Bot->Personality = EBotPersonality::Balanced;
                else if (PersonalityRoll < 65) Bot->Personality = EBotPersonality::Aggressive;
                else if (PersonalityRoll < 85) Bot->Personality = EBotPersonality::Builder;
                else Bot->Personality = EBotPersonality::Tactical;
                Bot->SkillLevel = UKismetMathLibrary::GetDefaultObj()->RandomIntegerInRange(3, 9);
                Bot->BuildCooldown = 0.15f - (Bot->SkillLevel * 0.012f);
                Bot->WoodCount = 999;
                Bot->StoneCount = 500;
                Bot->MetalCount = 200;
            }

            auto CurrentGameMode = (AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode;
            AFortGameStateAthena* CurrentGameState = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;

            if (CurrentGameState->GamePhase == EAthenaGamePhase::Warmup) {
                Bot->BotState = EBotState::Warmup;
                if (Bot->WarmupChoice == EBotWarmupChoice::MAX) Bot->WarmupChoice = AIEvaluator.PickWarmupChoice();
                BotsBTService_Warmup WarmupService;
                WarmupService.Tick(Bot);
            }
            else if (CurrentGameState->GamePhase == EAthenaGamePhase::Aircraft && !Bot->bHasJumpedFromBus) {
                if (CurrentGameState->GamePhaseStep == EAthenaGamePhaseStep::BusFlying) Bot->BotState = EBotState::Bus;
                else Bot->BotState = EBotState::PreBus;
                BotsBTService_AIDropZone DropZoneService;
                DropZoneService.Tick(Bot);
            }
            else if (Bot->BotState == EBotState::Skydiving || Bot->BotState == EBotState::Gliding) {
                BotsBTService_AIDropZone DropZoneService;
                DropZoneService.Tick(Bot);
            }
            else if (Bot->BotState == EBotState::Landed) {
                Bot->BotState = EBotState::Looting;
            }
            else if (Bot->BotState == EBotState::Looting) {
                BotsBTService_Loot LootService;
                LootService.Tick(Bot, Bot->CurrentLootingFilter);
                if (Bot->bPotentiallyUnderAttack && Bot->HasGun()) Bot->BotState = EBotState::Combat;
                else if (!Bot->bIsInSafeZone) Bot->BotState = EBotState::MovingToSafeZone;
            }
            else if (Bot->BotState == EBotState::InvestigatingSound) {
                SoundDetection.InvestigateSound(Bot);
            }
            else if (Bot->BotState == EBotState::MovingToSafeZone) {
                Bot->Pawn->PawnStopFire(0);
                FVector BotLoc = Bot->Pawn->K2_GetActorLocation();
                if (Bot->NearestPlayerPawn) {
                    float Dist = UKismetMathLibrary::GetDefaultObj()->Vector_Distance(BotLoc, Bot->NearestPlayerPawn->K2_GetActorLocation());
                    if (Dist < 8000.f && Bot->PC->LineOfSightTo(Bot->NearestPlayerPawn, BotLoc, true) && Bot->HasGun()) {
                        Bot->BotState = EBotState::Combat;
                    }
                }
                if (Bot->bIsInSafeZone) Bot->BotState = EBotState::Looting;
                if (CurrentGameState && CurrentGameState->SafeZoneIndicator) {
                    FVector TargetLoc = CurrentGameState->SafeZoneIndicator->NextCenter;
                    float TargetRadius = CurrentGameState->SafeZoneIndicator->NextRadius;
                    float CurrentTime = UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld());
                    float DistToZone = UKismetMathLibrary::GetDefaultObj()->Vector_Distance(Bot->LastUpdatedBotLocation, TargetLoc);
                    FRotator LookAtZone = UKismetMathLibrary::GetDefaultObj()->FindLookAtRotation(Bot->Pawn->K2_GetActorLocation(), TargetLoc);
                    Bot->PC->SetControlRotation(LookAtZone);
                    Bot->PC->K2_SetActorRotation(LookAtZone, true);
                    if (Bot->ZoneTargetStartTime == 0.f) {
                        Bot->ZoneTargetStartTime = CurrentTime;
                        Bot->LastZoneTargetDistance = DistToZone;
                    }
                    float Elapsed = CurrentTime - Bot->ZoneTargetStartTime;
                    // MODIFIED: No teleport, use LaunchIntoAir as fallback
                    if (Elapsed > 10.f && DistToZone > Bot->LastZoneTargetDistance - 100.f) {
                        Bot->Pawn->PawnStopFire(0);
                        if (UKismetMathLibrary::RandomBool()) Bot->LaunchIntoAir();
                        Bot->ZoneTargetStartTime = 0.f;
                        continue;
                    }
                    if (DistToZone <= TargetRadius + 150.f) Bot->ZoneTargetStartTime = 0.f;
                    Bot->LastZoneTargetDistance = DistToZone;
                    Bot->PC->MoveToLocation(TargetLoc, TargetRadius, true, false, false, true, nullptr, true);
                }
            }
            else if (Bot->BotState == EBotState::Combat) {
                FVector BotLoc = Bot->Pawn->K2_GetActorLocation();
                if (Bot->NearestPlayerPawn) {
                    float Dist = UKismetMathLibrary::GetDefaultObj()->Vector_Distance(BotLoc, Bot->NearestPlayerPawn->K2_GetActorLocation());
                    if (Dist < 8000.f) {
                        if (!Bot->HasGun()) Bot->BotState = EBotState::Looting;
                        else if (Bot->PC->LineOfSightTo(Bot->NearestPlayerPawn, BotLoc, true)) Bot->BotState = EBotState::Combat;
                    } else Bot->BotState = EBotState::MovingToSafeZone;
                }
                BotsBTService_Combat CombatService;
                CombatService.Tick(Bot);
            }
            else if (Bot->BotState == EBotState::Healing) {
                if (Bot->bPotentiallyUnderAttack) {
                    if (Bot->HasGun()) Bot->BotState = EBotState::Combat;
                    else Bot->BotState = EBotState::Looting;
                }
                BotsBTService_Healing HealingManager;
                HealingManager.StartHealing(Bot);
            }

            Bot->tick_counter++;
        }
    }
}
