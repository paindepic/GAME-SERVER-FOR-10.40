#pragma once
#include "pch.h"

namespace FortInventory
{
    static EFortQuickBars GetQuickBars(UFortItemDefinition* ItemDefinition)
    {
        if (!ItemDefinition) return EFortQuickBars::Secondary;

        if (!ItemDefinition->IsA(UFortWeaponMeleeItemDefinition::StaticClass()) &&
            !ItemDefinition->IsA(UFortEditToolItemDefinition::StaticClass()) &&
            !ItemDefinition->IsA(UFortBuildingItemDefinition::StaticClass()) &&
            !ItemDefinition->IsA(UFortAmmoItemDefinition::StaticClass()) &&
            !ItemDefinition->IsA(UFortResourceItemDefinition::StaticClass()) &&
            !ItemDefinition->IsA(UFortTrapItemDefinition::StaticClass()))
            return EFortQuickBars::Primary;
        return EFortQuickBars::Secondary;
    }

    static int32 FindEmptyQuickBarSlot(AFortPlayerController* PC, EFortQuickBars QuickBarType)
    {
        if (!PC || !PC->WorldInventory) return -1;

        // Count items in this quickbar type and track the highest OrderIndex used
        int32 MaxOrderIndex = -1;
        for (int32 i = 0; i < PC->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
        {
            FFortItemEntry& Entry = PC->WorldInventory->Inventory.ReplicatedEntries[i];
            UFortItemDefinition* Def = Entry.ItemDefinition;
            if (Def && GetQuickBars(Def) == QuickBarType)
            {
                // Track the highest OrderIndex for this quickbar type
                if (Entry.OrderIndex > MaxOrderIndex) {
                    MaxOrderIndex = Entry.OrderIndex;
                }
            }
        }

        // The next available slot is MaxOrderIndex + 1
        int32 NextSlot = MaxOrderIndex + 1;

        // Check if we have room for another slot
        int32 MaxSlots = (QuickBarType == EFortQuickBars::Primary) ? 5 : 2;
        if (NextSlot < MaxSlots)
        {
            return NextSlot;
        }

        return -1;
    }

    static void Update(AFortPlayerController* PC, FFortItemEntry* ItemEntry = nullptr)
    {
        PC->HandleWorldInventoryLocalUpdate();
        PC->WorldInventory->HandleInventoryLocalUpdate();
        PC->WorldInventory->bRequiresLocalUpdate = true;
        PC->WorldInventory->ForceNetUpdate();
        if (ItemEntry == nullptr)
            PC->WorldInventory->Inventory.MarkArrayDirty();
        else
            PC->WorldInventory->Inventory.MarkItemDirty(*ItemEntry);
    }

    static UFortWorldItem* CreateNewItem(AFortPlayerController* PC, UFortItemDefinition* ItemDefinition, int Count)
    {
        UFortWorldItem* NewItem = (UFortWorldItem*)ItemDefinition->CreateTemporaryItemInstanceBP(Count, 1);
        NewItem->SetOwningControllerForTemporaryItem(PC);
        return NewItem;
    }

    void GiveItem(AFortPlayerController* PC, UFortItemDefinition* Def, int Count = 1, int LoadedAmmo = 1, bool bShouldAddToExistingStack = false)
    {
        if (!PC || !PC->WorldInventory || !Def) return;

        if (bShouldAddToExistingStack) {
            for (FFortItemEntry& ItemEntry : PC->WorldInventory->Inventory.ReplicatedEntries) {
                if (Def == ItemEntry.ItemDefinition) {
                    ItemEntry.Count += Count;

                    PC->WorldInventory->Inventory.MarkItemDirty(ItemEntry);
                    Update(PC);
                    break;
                }
            }
            return;
        }

        UFortWorldItem* Item = Cast<UFortWorldItem>(Def->CreateTemporaryItemInstanceBP(Count, 0));
        if (!Item) return;
        Item->SetOwningControllerForTemporaryItem(PC);
        Item->ItemEntry.LoadedAmmo = LoadedAmmo;

        // Set OrderIndex for proper quickbar slot assignment in SDK 10.40
        // OrderIndex determines which slot the item goes in (0-4 for primary, 0-1 for secondary)
        Item->ItemEntry.OrderIndex = PC->WorldInventory->Inventory.ReplicatedEntries.Num();

        /*if (Item && Item->ItemEntry.ItemDefinition) {
            FFortItemEntryStateValue Value{};
            Value.IntValue = true;
            Value.StateType = EFortItemEntryState::ShouldShowItemToast;
            Item->ItemEntry.StateValues.Add(Value);
        }*/

        PC->WorldInventory->Inventory.ReplicatedEntries.Add(Item->ItemEntry);
        PC->WorldInventory->Inventory.ItemInstances.Add(Item);
        PC->WorldInventory->Inventory.MarkItemDirty(Item->ItemEntry);
        PC->WorldInventory->HandleInventoryLocalUpdate();
    }

    static UFortItemDefinition* FindItemDefinition(AFortPlayerController* PC, const FGuid& Guid)
    {
        if (!PC) return nullptr;

        for (int i = 0; i < PC->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
        {
            FFortItemEntry* ItemEntries = &PC->WorldInventory->Inventory.ReplicatedEntries[i];
            if (!ItemEntries) continue;
            if (ItemEntries->ItemGuid == Guid) return ItemEntries->ItemDefinition;
        }

        return nullptr;
    }

    static FFortItemEntry* FindItemEntry(AFortPlayerController* PC, UFortItemDefinition* ItemDefinition)
    {
        if (!PC || !ItemDefinition) return nullptr;
        if (!PC->WorldInventory) return nullptr;

        for (int i = 0; i < PC->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
        {
            FFortItemEntry* ItemEntry = &PC->WorldInventory->Inventory.ReplicatedEntries[i];
            if (!ItemEntry) continue;
            if (ItemEntry->ItemDefinition == ItemDefinition) return ItemEntry;
        }

        return nullptr;
    }

    static FFortItemEntry* FindItemEntry(AFortPlayerController* PC, const FGuid& Guid)
    {
        if (!PC) return nullptr;

        for (int i = 0; i < PC->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
        {
            FFortItemEntry* ItemEntry = &PC->WorldInventory->Inventory.ReplicatedEntries[i];
            if (!ItemEntry) continue;
            if (ItemEntry->ItemGuid == Guid) return ItemEntry;
        }

        return nullptr;
    }

    static void RemoveItem(AFortPlayerController* PC, UFortItemDefinition* ItemDefinition, int Count = 1)
    {
        if (!PC || !ItemDefinition) return;

        for (int i = 0; i < PC->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
        {
            FFortItemEntry* ItemEntry = &PC->WorldInventory->Inventory.ReplicatedEntries[i];
            if (!ItemEntry) continue;

            if (ItemEntry->ItemDefinition == ItemDefinition)
            {
                if (ItemEntry->Count > Count)
                {
                    ItemEntry->Count -= Count;
                    Update(PC, ItemEntry);
                }
                else
                {
                    PC->WorldInventory->Inventory.ReplicatedEntries.Remove(i);
                    Update(PC);
                }

                break;
            }
        }
    }

    static bool IsFullInventory(AFortPlayerController* PC)
    {
        if (!PC || !PC->WorldInventory) return true;

        static constexpr int Max = 5;
        int Count = 0;

        for (int i = 0; i < PC->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
        {
            FFortItemEntry* Entries = &PC->WorldInventory->Inventory.ReplicatedEntries[i];
            if (!Entries) continue;
            if (!Entries->ItemDefinition) continue;

            if (GetQuickBars(Entries->ItemDefinition) == EFortQuickBars::Primary)
            {
                ++Count;
                //Log(std::string("Count=") + std::to_string(Count));

                    return true;
            }
        }

        return false;
    }
}
