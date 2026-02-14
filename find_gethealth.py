#!/usr/bin/env python3
import re

with open('/home/engine/project/Spectra-10.40-main/10.40/PlayerBots.h', 'r', encoding='utf-8') as f:
    lines = f.readlines()

print("GetHealth() occurrences:")
for i, line in enumerate(lines, 1):
    if 'GetHealth()' in line:
        print(f"Line {i}: {line.rstrip()}")

print("\nGetShield() occurrences:")
for i, line in enumerate(lines, 1):
    if 'GetShield()' in line:
        print(f"Line {i}: {line.rstrip()}")

print("\nGetMaxHealth() occurrences:")
for i, line in enumerate(lines, 1):
    if 'GetMaxHealth()' in line:
        print(f"Line {i}: {line.rstrip()}")
