#!/usr/bin/env python3

with open('/home/engine/project/Spectra-10.40-main/10.40/PlayerBots.h', 'r', encoding='utf-8') as f:
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

with open('/home/engine/project/Spectra-10.40-main/10.40/PlayerBots.h', 'w', encoding='utf-8') as f:
    f.write(content)

print("Fixed all GetHealth/GetShield/GetMaxHealth occurrences in PlayerBots.h")
