# Stormfang Delivery — Skin Peeler Quest + Apex Mini Bosses

This document is the implementation manifest for the completed Stormfang encounter. It is intentionally limited to Stormfang; the Azshara and Feralas encounters remain design content in the companion draft.

## Delivery scope

- Runtime encounter: `modules/mod-skinpeelerquest`
- Canonical world-data migration: `data/sql/updates/pending_db_world/rev_20261025_01_apex_stormfang.sql`
- Gameplay coverage: ten Stormfang scenarios under `apps/coa-gameplay-test/scenarios/`
- Design and encounter contract: `docs/skin-peeler-quest-apex-mini-bosses.md`

## Source manifest

| File | Lines | Responsibility |
| --- | ---: | --- |
| `modules/mod-skinpeelerquest/src/SkinpeelerQuest.cpp` | 1–469 | Complete encounter implementation |
| `modules/mod-skinpeelerquest/CMakeLists.txt` | 1–9 | Module source registration |
| `data/sql/updates/pending_db_world/rev_20261025_01_apex_stormfang.sql` | 1–87 | Canonical templates, spawn, text, SmartAI, spell and visual rows |
| `docs/skin-peeler-quest-apex-mini-bosses.md` | 1–68 | Encounter design and technical reference |
| `apps/coa-gameplay-test/scenarios/skinpeeler-stormfang-*.json` | 29–109 each | Gameplay regression scenarios |

### `SkinpeelerQuest.cpp` line index

| Lines | Implementation |
| --- | --- |
| 1–19 | Server, player, spell, map, summon and container includes |
| 21–50 | Creature entries, spell IDs, radii, timers and emote state |
| 52–74 | `NearbyPlayers` phase, hostility, alive and radius filtering |
| 71–75 | Random target shuffling |
| 77–97 | `SelectNonTankTarget`, including previous-target exclusion |
| 96–118 | `SummonSpiritWolf`, timed summon and initial threat |
| 116–148 | `SummonStormHowlWolves`, two-wolf composition and target assignment |
| 148–185 | `StartStormcall`, seven damage pulses, self-root, emote and cleanup |
| 187–219 | `KeepWolfSeparation`, same-victim detection and chase-angle separation |
| 221–266 | `SkinpeelerStormfangSpellScript`, Thunder Pounce landing event |
| 268–460 | `SkinpeelerStormfangCreatureScript` |
| 276–280 | Spawn-time defensive state |
| 282–458 | Combat timers, health scaling, Pounce, Storm Howl, Stormcall and melee sound |
| 448–457 | Melee sound throttle outside Stormcall |
| 462–469 | Client spell registration and script construction |

### Database rows

| Table | Keys | Content |
| --- | --- | --- |
| `creature_template` | `9916212` | Stormfang, the Crackling Fury; level 60 elite; SmartAI; passive/defensive setup |
| `creature_template` | `9916211` | Stormfang Spirit Wolf; level 60 non-elite; normal creature template |
| `creature_template_model` | `9916212`, `9916211` | Display 68899 at scale 3; display 21114 at scale 1 |
| `creature` | `9999267` | Hinterlands spawn, 3,600-second respawn, movement type 2 |
| `creature_addon` | `9999267` | Spawn path state |
| `creature_text` | `9916212` groups `0`, `1`, `2` | Start, 65% and 30% red yells; type 14 |
| `smart_scripts` | `9916212` IDs `1`, `17`, `20`, `40`, `100` | Start yell, death sound, phase yells and defensive-on-spawn |
| `spell_dbc` | `100331`, `1004111`–`1004119` | Custom Rake and Stormfang spell data, including Lightning Call damage |
| `spellvisual_dbc` | `99028` | Stormcall visual with `ChannelKit = 0` |
| `spellvisualkit_dbc` | `791`, `99028` | Stormcall visual kit data |
| `spellvisualkit_dbc` | table definition | Created only when absent |

The migration removes obsolete pre-final entries (`9916201`, `9916202`, `9916210`), their spawn, text, scripts, and earlier spell/visual rows before inserting the final rows.

### Runtime behavior contract

- Stormfang is level 60 elite, entry `9916212`, name **Stormfang**, title **the Crackling Fury**.
- He remains defensive/passive until attacked.
- **Thunder Pounce** targets a non-tank within 30 yards. The landing hook calculates a distance-based delay and casts `1004116` on the same target.
- **Storm Howl** creates two `9916211` wolves with 25-second timed despawns. They receive 100 initial threat and are assigned non-tank targets; a tank can reacquire them through normal threat.
- **Stormcall** is a seven-second self-root. `StartStormcall` schedules pulses at 0–6 seconds and casts `1004118` on every living hostile player in phase within 50 yards.
- **Lightning Call** is a 100-nature damage spell, not a dummy aura. Seven pulses produce seven combat-log damage events and 700 total damage per player before mitigation.
- **Stormcall animation** uses client emote state `122` (`Eat No Sheathe`) and `HandleEmoteCommand(122)`; the state and root are cleared after seven seconds. CreatureSpecial/ChannelKit animation is disabled.
- **Thunderclap** and **Rake** are current-victim abilities and are suppressed while Stormcall is active.
- **Phase pressure** accelerates Pounce, Storm Howl and Stormcall timers at 70% and 40%; 65% and 30% are yell transitions, not additional spell phases.
- **Wolf spacing** uses chase movement rather than a one-shot point move, so the lower-GUID wolf remains engaged while separating from the other wolf.

### Client and DLL boundary

- The module registers its custom spell IDs through the existing `Ascension::ClientSpellPatches` interface in `SkinpeelerQuest.cpp:462-469`.
- No new opcodes, packet structures, GUID formats, or DLL exports are introduced.
- World-data spell and visual rows are supplied by the canonical SQL migration and are streamed through the existing custom-spell path.
- No client DLL source changes are part of this delivery.

### Gameplay scenarios

| Scenario | Lines | Coverage |
| --- | ---: | --- |
| `skinpeeler-stormfang-precombat.json` | 30 | Passive/defensive spawn and pre-pull behavior |
| `skinpeeler-stormfang-solo-tank.json` | 32 | Solo tank pressure and ability cadence |
| `skinpeeler-stormfang-one-nontank.json` | 36 | Pounce selection with one non-tank |
| `skinpeeler-stormfang-one-nontank-long.json` | 29 | Extended one-non-tank cycle |
| `skinpeeler-stormfang-two-nontanks.json` | 41 | Alternating non-tank Pounce targets |
| `skinpeeler-stormfang-full-timeline.json` | 45 | Full ability-timeline pass |
| `skinpeeler-stormfang-complete-one-nontank.json` | 76 | Complete encounter with one non-tank |
| `skinpeeler-stormfang-complete-target-death.json` | 78 | Target death and retargeting |
| `skinpeeler-stormfang-complete-group.json` | 109 | Sustained three-player fight through boss death |
| `skinpeeler-stormfang-stormcall-root-cycle.json` | 36 | Seven Stormcall pulses, damage and root expiry |

### Verification performed

- `worldserver` target built with the module present; the module compiled and linked without errors.
- `skinpeeler-stormfang-stormcall-root-cycle` passed after the Lightning Call damage effect was added.
- `skinpeeler-stormfang-complete-group` passed on the same encounter implementation before the final debug-log-only cleanup.
- Local runtime confirmed seven `100` damage applications per player per Stormcall in the temporary verification log.
- The temporary verification logging was removed before this delivery.

### Known boundaries

- The gameplay harness cannot render the client emote, channel bar, audio, or verify exact pathing/position spacing visually.
- The companion design document reserves Azshara and Feralas for later implementation.
