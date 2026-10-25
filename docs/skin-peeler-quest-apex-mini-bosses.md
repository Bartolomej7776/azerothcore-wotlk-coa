# Skin Peeler Quest + Apex Mini Bosses

> **Status:** Stormfang is complete and locked. Torvald Greyfeather and Zul'khar are present as the two faction huntmasters. Azshara and Feralas remain design-only and will be added in later updates.

### Core concept

Replace the Skin-Peeler 9000's Bazaar Token purchase with a challenging, one-time endgame hunting questline. Players must earn the tool by doing what it automates: hunting beasts and personally skinning their carcasses.

### Quest flow

- **Level 58 — The Apex Contract:** Visit **Torvald Greyfeather** in Stormwind or **Zul'khar** in Orgrimmar, then inspect the bounty board to unlock three contracts.

- **Level 60 — Three parallel hunts:** Complete the **Azshara**, **Hinterlands**, and **Feralas** bounties in any order. Inspect tracks in each zone to locate the beast, kill it, and personally skin its carcass.

- **Level 60 — Proof of Mastery:** Return all three trophy hides to the Huntmaster to unlock the Skin-Peeler 9000 appearance account-wide.

### The huntmasters

- **Torvald Greyfeather — Alliance:** Dwarf male hunter in the Stormwind Dwarven District. Entry `9916113`, display `53`, spawn GUID `9999266` at map `0`, `-9168.72, 768.735, 154.275`, 300-second respawn. Uses the Moonlit Great Lynx appearance preset.

- **Zul'khar — Horde:** Troll male hunter in the Orgrimmar Valley of Honor. Entry `9916114`, display `1478`, spawn GUID `9999265` at map `1`, `2003.64, -4645.85, 25.9132`, 300-second respawn. Uses the Emberlit Great Lynx appearance preset.

Both are level-60 friendly questgivers with `npcflag = 3`, faction `35`, `type_flags = 134217728`, and `creature_display_preset` rows. Their appearance is delivered through the stock mirror-image exchange: the client requests `CMSG_GET_MIRRORIMAGE_DATA` (`0x0401`) and the server replies with `SMSG_MIRRORIMAGE_DATA` (`0x0402`) from the preset row. No custom model is required.

The contract quest and gossip content is a later content pass. This delivery provides the huntmasters' complete world presence, faction, questgiver flags and appearance; it does not invent quest relations that are not implemented.

### The three apex hunts

Each boss is a level 60 elite, shared open-world spawn with a one-hour respawn, a small patrol area, and tracks that lead players to its lair.

- **Azshara — The Drowned Oracle:** A tide-twisted oracle lurking in the Ruins of Eldarath, attended by the Spitelash naga. Uses **Tidebound Venom** to stack poison, **Undertow** to knock players back, **Call the Deep** to summon Spitelash adds, and **Drowned Fury** to enrage. Players must manage poison, control adds, and avoid the brine pools.

- **Hinterlands — Stormfang, the Crackling Fury (locked):** A lightning-charged worg stalking the Overlook and the Aerie Peak foothills, drawn by the Wildhammer gryphons and the storms that never break. Stormfang is a level-60 elite and stays passive until attacked. **Thunder Pounce** leaps at a non-tank target within 30 yards and knocks it back; **Storm Howl** summons two level-60 non-elite Spirit Wolves; **Stormcall** roots him for seven seconds while seven Lightning Calls strike every player within 50 yards for 100 nature damage each (700 total); **Thunderclap** and **Rake** pressure the active tank. He yells at the pull, 65%, and 30%. The locked encounter has no fear, Chain Lightning, or Overcharge enrage. Players must spread for Pounce, control the wolves, and survive the Stormcall channel.

- **Feralas — The Scarred Matriarch:** A scarred owlcat matriarch haunting the Frayfeather Highlands and the Ruins of Isildien, driven mad by years of Gordunni ogre hunting. Uses **Pounce** to apply bleeds, **Screech** to fear, **Owlcat Pack** to summon adds, **Scarred Hide** for temporary damage reduction, and a **Berserk** timer. Players must break fear, control adds, and coordinate burst damage during the Scarred Hide windows.

### Huntmaster technical reference

| Field | Torvald Greyfeather | Zul'khar |
| --- | --- | --- |
| Entry | `9916113` | `9916114` |
| Faction | `35` | `35` |
| Level / rank | `60` / normal | `60` / normal |
| NPC flags | `3` (gossip + questgiver) | `3` (gossip + questgiver) |
| Gossip menu | `0` (no menu in this pass) | `0` (no menu in this pass) |
| Base display | `53` (dwarf male) | `1478` (troll male) |
| Display scale | `1` | `1` |
| Type flags | `134217728` | `134217728` |
| Unit flags 2 | `2048` | `2048` |
| Spawn GUID | `9999266` | `9999265` |
| Map / position | `0`, `-9168.72, 768.735, 154.275` | `1`, `2003.64, -4645.85, 25.9132` |
| Respawn | `300` seconds | `300` seconds |
| Movement | stationary (`MovementType = 0`) | stationary (`MovementType = 0`) |

**Appearance preset rows**

| Entry | Display | Race | Gender | Class | Skin / face / hair / color / facial | Head | Shoulders | Chest | Waist | Legs | Feet | Wrists | Hands | Back |
| --- | ---: | ---: | ---: | ---: | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| `9916113` | `53` | `3` | `0` | `3` | `1 / 2 / 3 / 5 / 4` | `140207` | `140213` | `140225` | `140231` | `140243` | `140201` | `140255` | `140249` | `140195` |
| `9916114` | `1478` | `8` | `0` | `3` | `1 / 2 / 3 / 0 / 0` | `140205` | `140211` | `140223` | `140229` | `140241` | `140199` | `140253` | `140247` | `140193` |

**Implementation notes**

- `creature_display_preset` is the appearance source; the base `creature_template_model` display is only the client-buildable placeholder.
- The core sets `UNIT_FLAG2_MIRROR_IMAGE` for creatures with a loaded preset (`AscensionCompat.cpp`), so the client requests the paper-doll on visibility.
- `type_flags = 134217728` belongs to the preset/NPC path.
- `npcflag = 3` marks the units as gossip-capable questgivers. No `gossip_menu`, `gossip_menu_option`, `npc_text` or `creature_queststarter` rows are part of this pass; those belong to the contract content pass.
- The final rows are supplied by `modules/mod-skinpeelerquest/sql/mod_skinpeelerquest_huntmasters.sql`.

### Stormfang technical reference

**Encounter identity and spawn**

- Creature entry `9916212`; name **Stormfang**, title **the Crackling Fury**; level 60, elite (`rank 1`), faction `16`, beast/wolf, experience class `2`.
- Template: `AIName = SmartAI`, `MovementType = 0`, `HealthModifier = 12`, `DamageModifier = 1.2`, `RegenHealth = 1`.
- Spawn GUID `9999267`; map `0` (Hinterlands); position `146.714, -4587.26, 119.685`; 3,600-second respawn; spawn `MovementType = 2`; `creature_addon` path id `99992670`.
- State: defensive/passive on spawn and after reset; it does not initiate combat until attacked.
- Spirit Wolf entry `9916211`; level 60, non-elite (`rank 0`), display `21114`, scale `1`; timed summon with a 25-second lifetime.

**Ability contract**

| Ability | Spell IDs | First use / repeat | Exact behavior |
| --- | --- | --- | --- |
| **Thunder Pounce** | `1004112`, impact `1004116` | 10 seconds; 16 seconds above 70%, 14 seconds at 40–70%, 12 seconds below 40% | Selects a non-tank within 30 yards. The impact is scheduled after `distance / 24 + 150 ms`, clamped to 350–1,500 ms. The impact deals 600 nature damage, a 15-yard knockback and a 2-second stun. Retries every 5 seconds if no valid non-tank exists. |
| **Storm Howl** | Summons `9916211` | 20 seconds; 45 seconds above 40%, 35 seconds below 40% | Summons two wolves 5 yards to either side of Stormfang. Wolves choose non-tanks within 40 yards; with one non-tank both use that target; with none they fall back to the tank. Each receives 100 initial threat. |
| **Stormcall** | `1004117`; tick `1004118` | 30 seconds; 60 seconds above 70%, 50 seconds at 40–70%, 40 seconds below 40% | Seven-second self-root and channel. Pulses fire at 0, 1, 2, 3, 4, 5 and 6 seconds. Each pulse deals 100 nature damage to every living hostile player in the same phase within 50 yards, for 700 total per player. The cast is refused while another non-melee cast is active. |
| **Thunderclap** | `100377` | Every 12 seconds | Hits Stormfang’s current victim; deferred while Stormcall is active. |
| **Rake** | `100331` | Every 8 seconds | Hits Stormfang’s current victim; deferred while Stormcall is active. |
| **Melee audio** | direct sound `4069` | Every 2 seconds | Plays only while Stormfang has a victim and is not channelling Stormcall. |

**Phase, timers and yells**

- Health does not add a separate enrage. The 70% and 40% breakpoints only shorten Pounce, Storm Howl and Stormcall repeat timers.
- The 65% and 30% transitions are yell events, not additional spell phases.
- Encounter start is SmartAI event type 4; 65% and 30% are health events with phase mask `0`.
- All three lines use `CHAT_MSG_MONSTER_YELL` (`creature_text.Type = 14`).
- Death sound is `394`.
- Every timer map is keyed by the creature GUID counter and resets when Stormfang leaves combat; this prevents a stale timer from surviving a reset or respawn.

**Spells, visuals and data**

- `1004111` Stormfang Rake; `1004112` Thunder Pounce; `1004113` Overcharged Bite; `1004114` Thunderclap; `1004115` Static Charge; `1004116` Thunder Pounce Impact; `1004117` Stormfang Stormcall; `1004118` Lightning Call; `1004119` Frenzy.
- `1004118` is a real school-damage effect: `Effect_1 = 2`, `EffectBasePoints_1 = 99`, `SchoolMask = 8`, target `6`. It is not a dummy aura, so the combat log receives damage lines instead of refresh lines.
- Stormcall duration is `SpellDuration` row `165`, which resolves to 7,000 ms.
- `SpellVisual` `99028` has `ChannelKit = 0`; `spellvisualkit_dbc` rows `791` and `99028` are present.
- Stormcall uses emote state `122` (`Eat No Sheathe`) and an explicit `HandleEmoteCommand(122)`. The root and emote are cleared at 7,000 ms.
- Stormcall intentionally has no werewolf/CreatureSpecial channel animation and no `PlayDirectSound(9036)` howl.
- Custom spell IDs are registered through the existing `Ascension::ClientSpellPatches` interface in `SkinpeelerQuest.cpp:462-469`.
- No new opcodes, packet layouts, serialization formats or client DLL exports are introduced.

**Wolf and anti-exploit contract**

- Wolves spawn at ±5 yards from Stormfang and use normal level-60 non-elite health and damage.
- A wolf is assigned to a non-tank when one exists; the tank retains normal threat reacquisition through the 100-point initial link.
- Two wolves attacking the same target within 2.5 yards trigger the separation pass. Only the lower GUID moves, and it uses `MoveChase` with a side angle, so it remains in combat and targetable.
- Pounce never falls back to the tank. The previous non-tank is excluded when another non-tank is available.
- Stormcall only affects hostile players in the same phase, alive, within 50 yards; it does not fear, chain, reflect or apply a player control aura.
- `StartStormcall` is called only after `SPELL_CAST_OK`; there is no second `OnSpellCast` scheduling path.
- The encounter data is supplied by `data/sql/updates/pending_db_world/rev_20261025_01_apex_stormfang.sql`.

### Trophy hides

- **Tide-Warped Hide** — The Drowned Oracle
- **Stormfang Pelt** — Stormfang
- **Scarred Owlcat Pelt** — The Scarred Matriarch

### Group design and anti-exploit rules

- Tuned for **3–5 players**, with mechanics that reward a healer, cleanses, fear breaks, add control, and coordinated damage. Soloing is possible for heavily overgeared players, but isn't the intended experience.

- Every eligible quest holder in the killing group receives group credit and can interact with the same carcass for their own personal hide. One kill can advance up to five players, avoiding competition over a single spawn.

- The carcass remains skinable for five minutes. Hides are **Bind on Pickup, unique, and quest-only**, and can only be earned through the personal skinning interaction — never purchased, traded, or farmed repeatedly.

- The contracts require **Skinning 300**, are one-time only, and cannot be repeated. If a boss is on cooldown, players can complete the other hunts and return later.
