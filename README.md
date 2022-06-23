# What is Diablo Core

Diablo Core is a mod of the original Diablo and Hellfire games built using DevilutionX 1.3.0 as a base.

Contrary to DevilutionX, this mod is a heavily opinionated modification of the original games and attempts to improve and innovate on several areas by drastically altering some of the base game mechanics. You should not expect a similar experience compared to the original game here.

Additionally, keep in mind that I'm not an artist myself, and since I'll be doing this alone, I'll probably not be able to add a ton more _assets_ to the game: instead, I'll do my best to repurpose existing content (used and unused) for the mod, and will focus more on other aspects like balance, mechanics, randomization, etc.

If you want to see what ideas I have in mind for the mod, check the [Diablo Core Idea index](https://github.com/julealgon/DiabloCore/issues/26) issue.
 
For a full list of changes see the [changelog](docs/CHANGELOG.md).

# Design Principles

Here I'll share a brief overview of some of the ideas I have regarding various aspects of the game and what the mod will try to go for.

## Popularity

Every aspect of this mod will be developed without regard to how "popular" the change/feature is in the Diablo community or playerbase: I'll primarily be making a mod that _I'd like to play_, and hope that other people will potentially enjoy it as well. If nobody else besides me finds the mod fun, that's ok: I'm not here to make a "popular" or "successful" mod. If it ever becomes popular or successful, then great, but those are absolutely not actual targets and won't be influencing any of my decisions when coding.

This means I'll consider mechanics from scratch for the most part. "It was done like this in D2" is not an argument that I'll ever consider for implementing something here: what I'll be looking at is the underlying value that the feature has, and only that.

In the end of the day, this probably means I'll be implementing a few mechanics that probably already exist in other games, but in a way that feels different to play.

## Difficulty

While many people still consider vanilla Diablo to be a challenging game, I'm not one of those people: the game is ridden with subtle bugs, AI exploits and severe balancing issues that completely break the game for any intermediate-to-advanced player.

This mod will increase the overall difficulty of the game and will try to do so not by simply bumping numbers and creating damage spikes, but by changing several mechanics, fixing and improving AI bugs, preventing exploits and rebalancing the game around more intuitive but deeper systems.

Expect the normal game to be more inline to actual rogue-like games, without the hability to load back in time (loading will only be available to "continue your session", so won't be exploitable), and also a gameplay loop that is closer to an Ironman mode by default.

I'm also thinking about ways to encourage players to move forward constantly without big pauses and AI exploit stalling, by introducing some sort of "gotta-go-fast" system: players who try to abuse the AI with weak but 100% weapons/tile positions should be punished by this new mechanic.

## Scope

The vanilla game was originally a single-player-only game, that got multi-player added in the last minute by pressure from business. Similarly, the content in the game was artificially expanded by the addition of "difficulties", playable only in multi-player games or via a bug in single-player.

This mod will not support either of these initially: it will focus on the single-player experience only, and will rebalance and modify the game to be enjoyable and replayable without the need for Nightmare and Hell difficulties.

Similarly, I will not be targetting every platform with this mod: it will initially only run on Windows, and I may (though unlikely) end up exposing it for other platforms at a later point.

And last but not least, no "endgame" is planned: replayability will come strictly from starting new runs from scratch.

## Itemization

One of the goals of this project is to revamp the way items work, by hopefully creating a more engaging and balanced system. The concept of "unique items" will be dropped in favor of less "fixed" stats, and the idea is to encourage more variation during a play session and less "best in slot" configurations.

I also believe that while quite fun, the original's item system is too limiting and simple. On that front, I intend to introduce more components like weapon range, damage types, item weight, item buffs and debuffs, attribute-based attack speed, more item bases and more variability between items.

Regarding the potion system, it will also be changed dramatically to avoid being exploitable like in the original: no more filling the inventory with potions and potion-spamming your way out of danger. The new system will be designed in a way to discourage spam, promote strategical use and will not be exploitable with enough gold.

## Battle mechanics

Out of all systems in vanilla, the one I personaly consider the most shallow is the way combat works. Admitedly, D1 does promote a lot of strategic placement and positioning compared to D2, but other than that, the combat itself is too simplistic and repetitive.

Perhaps one of the main goals of this mod will be to drasticaly increase the options one has in combat by introducing several new mechanics as well as changing existing mechanics. Here is a non-exaustive list of things that should be changing in the mod:

- Chance-to-hit and armor mechanics will be completely redesigned
- Different weapon ranges will be introduced
- A new stamina system will be added to discourage spam and promote strategic play
- Weapon damage types will be expanded from the shallow (sword/mace/neutral) to a more fully fledged damage-type system with monster resistances. Planned damage types are `slash`, `blunt` and `piercing`
- Animation durations will be dynamic and adjust to attributes
- Facing direction will start playing a role in some mechanics, like blocking chance

## Spells

Diablo has a fairly interesting spell system, where any character can use any spell provided he has the required magic to do so. I don't necessarily intent to _change_ that part of the system, but I _may_ introduce some more class-specific skills besides the single one each class has today. This will be in addition to the "learnable" skills.

The way current spells work however will be dramatically rebalanced. Spells in D1 are widely considered unbalanced and unfair, and make the sorcerer class extremely overpowered. Spells will be changed in such a way as to discourage builds like "dump everything into magic", which is what basically happens today since the sorcerer is in most situations able to kill everything from very far and never have to worry about survivability, especially because of mana shield: new spell balance will at least:

1. drastically scale down spell damage
2. drastically reduce attack spells range
3. rework spells like mana shield to be deeper and have more of a risk/reward component
4. promote a more balanced use of skills that are currently almost useless (inferno, flash, bone spirit, etc)

## Randomization

While the original game is highly random today, I want to expand upon that even more by randomizing some other aspects of the game.

For example, I expect to have random area order per game (i.e. you first visit catacombs, then caves, then cathedral, then hell in a game, and in another, you visit caves first, then cathedral, then catacombs, then hell).

I also want to make it so that monsters and monster categories can vary a bit more than in the original. For example, getting a high level zombie enemy to appear on a very late dungeon level, or having a weaker version of vipers appearing in cathedral etc.

## Formulas and numbers

I'm personally a huge fan of simple, intuitive systems backed by small, reasonable number scaling. Any weird, hidden bonus or quirk in Diablo's formulas will probably be removed completely and reworked as a more orthogonal system. Here is a non-exaustive list of some of the things I intend to change:
- Class-specific "bonuses", like the Warrior's "bonus blocking" factor: instead of being assigned to a class and completely hidden to the player, blocking chance will be reworked and based on character stats
- Character level (and monster level) will be removed from all formulas in the game: clvl will only exist as a display value, a way to judge progress, and nothing more
- Similarly, "on-level-up" bonuses (extra HP and mana) will be removed. HP and mana will instead directly reflect your stats: want more mana? add points to energy. So on and so forth.
- Hit recovery will dynamically scale based on the strength of the hit: stronger hits => longer hit recovery frames
- Same with blocking: blocking weak arrows will not result in the same blocking animation as blocking a charging Hork Demon for example
- Armor will work in the opposite way of damage: as damage reduction. Chance to hit will be reserved for ranged attacks and moving targets only, as melee hits against stationary targets will "always hit"
- I'll make an effort to have most if not all formulas be simple differences of values with linear progression: 10 damage against 6 armor deals 4 HP damage.

I hope these changes will make the overall game more intuitive and deeper at the same time.

## Architecture and Technical aspects

I'll be moving the whole project from C++ to C# as fast as possible. I don't like the restrictions C++ imposes me and how messy the code looks, and my familiarity with C# will allow me to expand on ideas much faster than I otherwise would.

I'll also give it a shot to writing very decoupled modules for the game. Ideally, the game logic core will be separate from the presentation layer in such a way that would allow multiple UIs to be written independently.

Once the game is fully refactored, I expect a significant portion of the logic to be softcoded. The original game had basically _every_ aspect hardcoded in the game logic making it really hard to extend and modify.

As part of this mod, I'll look into creating a dedicated "data-API" that can be used not only by the game itself, but also by websites/wikis/etc. I want to make it so that there is a single source-of-truth for game data that is consistently consumed by everything else that needs it.

# Contributing

I'm not accepting any contributions at this time as the mod is in its infancy. You are free to use GitHub's 'Discussions' feature to ask any questions however and I'll try my best to answer them.

# Credits

- The original DevilutionX project/team [DevilutionX](https://github.com/diasurgical/devilutionX). Without their efforts, I'd never be able to seriously start my own mod.
