#!/usr/bin/env python3
import os

file_path = '/home/engine/project/Spectra-10.40-main/10.40/PlayerBots.h'

# Read the file
with open(file_path, 'r', encoding='utf-8') as f:
    content = f.read()

# Replace GetHealth() with Health
content = content.replace('bot->Pawn->GetHealth()', 'bot->Pawn->Health')
content = content.replace('Pawn->GetHealth()', 'Pawn->Health')

# Replace GetShield() with Shield
content = content.replace('bot->Pawn->GetShield()', 'bot->Pawn->Shield')
content = content.replace('Pawn->GetShield()', 'Pawn->Shield')

# Replace GetMaxHealth() with MaxHealth
content = content.replace('bot->Pawn->GetMaxHealth()', 'bot->Pawn->MaxHealth')
content = content.replace('Pawn->GetMaxHealth()', 'Pawn->MaxHealth')

# Write back to the file
with open(file_path, 'w', encoding='utf-8') as f:
    f.write(content)

print("Successfully fixed GetHealth/GetShield/GetMaxHealth in PlayerBots.h")
