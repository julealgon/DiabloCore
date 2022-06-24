# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

Note that the changes described below are on top of DevilutionX's version 1.3.0. You can check [the original DevilutionX changelog](https://github.com/diasurgical/devilutionX/blob/master/docs/CHANGELOG.md#devilutionx-130) for changes done prior to this).

## Unreleased

## v0.1.0

### ✨ Features
- Removed attack animation skipping: heroes now always attack using all animation frames. This makes the game substantially harder but results in a smoother and more consistent flow (#8)
- Unified all Diablo and Hellfire behavior into a single consistent behavior (#19)
  - The game will always present itself as "Diablo" now (Hellfire UI, logos, videos, etc won't show)
  - All spells are available, including Diablo and Hellfire ones
  - Monk, Barbarian and Bard now always available as a playable classes
  - Removed velocity bonus for arrows when using "Fast Attack" equipment
  - Damage piercing will always use the "Enemy AC % reduction" logic
  - Spells always cost half for sorcerer
  - All save files now end in `.sv` extension
  - Diablo (the monster) now always uses all his animation frames and has no HP nerfs applied
  - Pepin will not consider hero stats for selling elixirs
  - Shortcuts to town will only be open if the hero has visited them at least once from inside the dungeon
  - Monsters will keep attacking the hero's body when you die
  - Special-case resistances against holy bolt have been removed
  - Fire arrows do not stop monster regeneration for monsters
  - Apocalypse spell will only hit monsters in hero's line of sight
  - The sorcerer will always start with a charged bolt staff and 2 mana potions
  - Blocked missiles will always be redirected/deflected on impact with shield
  - Vitality and Magic elixirs will not heal HP or mana respectively
  - Griswold will always sell 6 Premium items
  - Items sold by Wirt will always cost 50% more than normal

### 💥 Breaking changes
- Removed multiplatform support (#1)
- Removed alternative control methods (#6)
- Removed translation support (#7)
- Removed all multi-player related networking logic (#10, #12)
- Removed "spawn" installation option (#11)
- Removed hero name auto-fill functionality (#13)
- Removed time-demo recording and playback functionality (#14)
- Removed support for compiling/running the game without sound (#16)
- Removed Nightmare and Hell game difficulties (#17)
- Removed "hold mouse button to attack constantly" functionality (#24)
- Removed various configuration options from the `.ini` and standardized their behavior (#20)
  - Networking settings cannot be set (no networking anymore)
  - Chat options cannot be set (no multiplayer anymore)
  - The 'Disable Crippling Shrines' option has been removed: all shrines are now always clickable
  - The 'Randomize Quests' option has been removed: quests will always be random now
  - The 'Adria Refills Mana' option has been removed: Adria will never refill mana now
  - The 'Experience Bar' option has been removed: the XP bar will always show now
  - The 'Friendly Fire' option has been removed (not applicable to single-player)
  - The 'Test Bard' option has been removed: the bard is now always selectable as a hero class
  - The 'Test Barbarian' option has been removed: the barbarian is now always selectable as a hero class
  - The 'Cow Quest' option has been removed: the cow quest has been removed from the game
  - The 'Theo Quest' option has been removed: the theo quest has been removed from the game
  - The 'Color Cycling' option has been removed: color cycling is now always turned on
  - The 'Blended Transparency' option has been removed: blended transparency is now always turned on
  - The 'Hardware Cursor' option has been removed: hardware cursor is now always turned on
  - The 'Walking Sound' sound option has been removed: walking sounds will now always play
  - The 'Sample Rate' sound option has been removed: sound will now always play using 22khz
  - The 'Channels' sound option has been removed: sound will now always play in stereo
  - The 'BufferSize' sound option has been removed: sound buffer is now always set to 2KiB internally
  - The 'Resampling Quality' sound option has been removed: audio resampling quality is now set to 5 internally
  - The 'Auto Equip Sound' sound option has been removed: auto-equiping items will now always play the equipment sound
  - The 'Auto Gold Pickup' option has been removed: gold is never picked up automatically now

### ♻️ Other (refactoring, architecture, etc)
- Removed tests from project (#2)
- Added `Damage` valuetype to abstract damage values (#4)
- Removed GPERF instrumentation (#15)
- Removed partial screen rendering optimization (#21)
- Moved hero attack cancellation when target monster dies to the monster death event (#23)
- Cleaned up and unified configuration settings models from Diablo and Hellfire into a single entity (#20)
