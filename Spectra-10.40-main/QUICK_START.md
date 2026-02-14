# Quick Start Guide - Enhanced Player Bots

## What's New

✅ **Bot Personalities**: Aggressive, Builder, and Balanced playstyles  
✅ **Building System**: Full build fighting with 90s, ramp rushing, box fighting  
✅ **Active Lobbies**: Bots run around, jump, and practice building  
✅ **Roof Spawning**: Spawn bots on rooftops of buildings  
✅ **Resource Management**: Bots track and consume materials  

## Usage

### Spawning Bots

```cpp
// Normal spawn
Bots::SpawnPlayerBot();

// Spawn on roof
Bots::SpawnPlayerBot(nullptr, PlayerBots::EBotState::Warmup, true);

// Spawn at specific location
Bots::SpawnPlayerBot(CustomLocation, PlayerBots::EBotState::Landed);
```

### Bot Personalities

Bots automatically choose personalities on spawn:
- **40%** Balanced (situational building)
- **35%** Aggressive (minimal building, push-focused)
- **25%** Builder (extensive building, high ground control)

### Build Patterns

Bots will automatically:
- Build defensive walls when shot at
- Ramp rush when enemy has height advantage
- Build 90s to gain high ground (Builder personality)
- Box up when stressed/low health
- Turtle with floor and cone when critically injured

### Lobby Behaviors

Bots now perform varied warmup activities:
- Run around with sprint
- Jump while moving
- Practice building and harvesting
- Fight other players if they find weapons
- Emote and interact socially

## Configuration

All features work out of the box with sensible defaults:

- **Starting Materials**: 999 Wood, 500 Stone, 200 Metal
- **Skill Levels**: 3-8 (affects build speed)
- **Build Cooldown**: 0.05-0.15s between builds
- **Personality Distribution**: 40/35/25 split

## Performance

The system is optimized for server performance:
- Build cooldowns prevent spam
- Resource checks before building
- Maximum ~10 builds per second per bot
- Building patterns interrupt when conditions change

## Troubleshooting

**Bots not building?**
- Check they have materials (WoodCount > 50)
- Ensure they're in Combat state
- Verify BuildingFoundations array is populated

**Roof spawning not working?**
- BuildingFoundations must be populated by GameMode
- Falls back to PlayerStarts if empty
- Check Z offset is appropriate for your map

**Build placement issues?**
- Building depends on game's placement system
- Check building cooldowns aren't too aggressive
- Verify building items are in bot inventory

## Examples

### Spawn 10 Bots with Mix of Normal and Roof Spawns

```cpp
for (int i = 0; i < 10; i++) {
    bool spawnOnRoof = (i % 3 == 0); // Every 3rd bot on roof
    Bots::SpawnPlayerBot(nullptr, PlayerBots::EBotState::Warmup, spawnOnRoof);
}
```

### Force All Bots to Be Builders (for testing)

Modify line 2475 in PlayerBots.h:
```cpp
// Change from random to always Builder
Bot->Personality = EBotPersonality::Builder;
```

## What to Test

Priority testing checklist:
1. ✅ Spawn bots in lobby - verify warmup behaviors
2. ✅ Start match - verify bots jump from bus
3. ✅ Land near bots - verify looting behavior
4. ✅ Engage in combat - verify building activates
5. ✅ Different personalities - observe varied playstyles
6. ✅ Roof spawning - use spawn parameter

## Future Enhancements

Planned improvements:
- Edit tool patterns for peek shots
- Material harvesting when low
- Advanced build sequences (cone jumps, tunneling)
- Team-based coordinated building
- Dynamic difficulty adjustment

## Support

For issues or questions:
1. Check IMPROVEMENTS.md for detailed technical documentation
2. Verify .gitignore is properly configured
3. Ensure all SDK dependencies are present
4. Test with original Spectra codebase first

---

**Version**: 1.0  
**Compatible With**: Spectra 10.40  
**Last Updated**: February 2026
