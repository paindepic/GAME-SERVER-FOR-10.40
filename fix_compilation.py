#!/usr/bin/env python3
import re

# Read the file
with open('/home/engine/project/Spectra-10.40-main/10.40/PlayerBots.h', 'r', encoding='utf-8') as f:
    content = f.read()

# Apply fixes
# 1. Replace GetHealth() with Health (but not when it's a method call on something other than Pawn)
content = re.sub(r'(\w+)\->GetHealth\(\)', r'\1->Health', content)
# 2. Replace GetShield() with Shield
content = re.sub(r'(\w+)\->GetShield\(\)', r'\1->Shield', content)
# 3. Replace GetMaxHealth() with MaxHealth
content = re.sub(r'(\w+)\->GetMaxHealth\(\)', r'\1->MaxHealth', content)
# 4. Replace IsDead() with Health <= 0 (for Pawn objects)
content = re.sub(r'(\w+)\->IsDead\(\)', r'\1->Health <= 0', content)

# Write the fixed file
with open('/home/engine/project/Spectra-10.40-main/10.40/PlayerBots.h', 'w', encoding='utf-8') as f:
    f.write(content)

print("Compilation fixes applied successfully!")
