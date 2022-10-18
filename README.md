# mod-profspecs
Profession Spec module for AzerothCore

Adds an NPC that will teach characters additional specilizations for tradeskills (such are Armorsmithing and Weaponsmithing).

Features:
- Toggle on and off for each tradeskill.
- Add a gold cost to learning specilizations. 

Planned:
- Toggle for subspecs (Swordsmith, Hammersmith, Axesmith)

## How to install
1. Put the moudule under the modules folder in your AzerothCore source folder.
2. Rerun Cmake and launch a clean build of AzerothCore.
3. Import SQL into the world database.
4. Place the NPC using `.npc add 197761` command.

