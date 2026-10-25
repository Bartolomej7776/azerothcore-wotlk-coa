# Skin Peeler Quest + Apex Mini Bosses

> **Status:** Stormfang is complete and locked. Azshara and Feralas remain design-only and will be added in later updates.

### Core concept

Replace the Skin-Peeler 9000's Bazaar Token purchase with a challenging, one-time endgame hunting questline. Players must earn the tool by doing what it automates: hunting beasts and personally skinning their carcasses.

### Quest flow

- **Level 58 — The Apex Contract:** Visit your faction's Huntmaster and inspect the bounty board to unlock three contracts.

- **Level 60 — Three parallel hunts:** Complete the **Azshara**, **Hinterlands**, and **Feralas** bounties in any order. Inspect tracks in each zone to locate the beast, kill it, and personally skin its carcass.

- **Level 60 — Proof of Mastery:** Return all three trophy hides to the Huntmaster to unlock the Skin-Peeler 9000 appearance account-wide.

### The three apex hunts

Each boss is a level 60 elite, shared open-world spawn with a one-hour respawn, a small patrol area, and tracks that lead players to its lair.

- **Azshara — The Drowned Oracle:** A tide-twisted oracle lurking in the Ruins of Eldarath, attended by the Spitelash naga. Uses **Tidebound Venom** to stack poison, **Undertow** to knock players back, **Call the Deep** to summon Spitelash adds, and **Drowned Fury** to enrage. Players must manage poison, control adds, and avoid the brine pools.

- **Hinterlands — Stormfang, the Crackling Fury (locked):** A lightning-charged worg stalking the Overlook and the Aerie Peak foothills, drawn by the Wildhammer gryphons and the storms that never break. Stormfang is a level-60 elite and stays passive until attacked. **Thunder Pounce** leaps at a non-tank target within 30 yards and knocks it back; **Storm Howl** summons two level-60 non-elite Spirit Wolves; **Stormcall** roots him for seven seconds while seven Lightning Calls strike every player within 50 yards for 100 nature damage each (700 total); **Thunderclap** and **Rake** pressure the active tank. He yells at the pull, 65%, and 30%. The locked encounter has no fear, Chain Lightning, or Overcharge enrage. Players must spread for Pounce, control the wolves, and survive the Stormcall channel.

- **Feralas — The Scarred Matriarch:** A scarred owlcat matriarch haunting the Frayfeather Highlands and the Ruins of Isildien, driven mad by years of Gordunni ogre hunting. Uses **Pounce** to apply bleeds, **Screech** to fear, **Owlcat Pack** to summon adds, **Scarred Hide** for temporary damage reduction, and a **Berserk** timer. Players must break fear, control adds, and coordinate burst damage during the Scarred Hide windows.

### Stormfang technical reference

**Encounter identity**

- Creature entry `9916212`; name **Stormfang**, title **the Crackling Fury**; level 60, elite (`rank 1`).
- Spawn GUID `9999267`; map `0` (Hinterlands); 3,600-second respawn; waypoint movement enabled; defensive/passive until first attacked.
- Stormfang Spirit Wolf entry `9916211`; level 60, non-elite (`rank 0`); timed summon with a 25-second duration.

**Ability contract**

| Ability | Spell | Timing | Behavior |
| --- | --- | --- | --- |
| **Thunder Pounce** | `1004112`, impact `1004116` | First at 10s; every 16s above 70% health, 14s at 40–70%, 12s below 40% | Targets a non-tank within 30 yards, then lands with a 600-nature impact, 15-yard knockback, and 2-second stun. Retries every 5s if no valid non-tank exists. |
| **Storm Howl** | Summons entry `9916211` | First at 20s; every 45s above 40% health, 35s below 40% | Summons two wolves 5 yards to either side of Stormfang. Wolves target non-tanks within 40 yards; with one non-tank both use that target, otherwise they fall back to the tank. Each wolf has 100 initial threat, so a tank can pull them back. |
| **Stormcall** | `1004117`, tick `1004118` | First at 30s; every 60s above 70% health, 50s at 40–70%, 40s below 40% | Seven-second self-root and channel. Seven Lightning Calls fire at 0–6 seconds, each dealing 100 nature damage to every living hostile player within 50 yards, for 700 total per player. Blocked while another non-melee cast is active. |
| **Thunderclap** | `100377` | Every 12s | Hits Stormfang’s current victim; suppressed during Stormcall. |
| **Rake** | `100331` | Every 8s | Applies to Stormfang’s current victim; suppressed during Stormcall. |

**Technical notes**

- **Phase pressure:** health thresholds only accelerate ability timers; there is no separate enrage. The 65% and 30% transitions are red yell events.
- **Yells:** encounter start, 65%, and 30%; all use `CHAT_MSG_MONSTER_YELL` (`Type 14`). Death uses sound `394`.
- **Pounce targeting:** the most recent non-tank is excluded when another non-tank is available. Pounce does not fall back to the tank.
- **Stormcall visual/audio:** the root uses client emote state `122` (`Eat No Sheathe`) for the channel; CreatureSpecial is disabled with `ChannelKit = 0`, and the Stormcall howl is intentionally removed.
- **Lightning Call combat log:** `1004118` is a real 100-nature damage effect rather than a dummy aura, so each pulse produces a damage line instead of an aura-refresh line.
- **Wolf separation:** while two wolves attack the same target within 2.5 yards, the lower-GUID wolf shifts to a side angle using chase movement. This keeps both wolves targetable without leaving one in a stopped movement state.

### Trophy hides

- **Tide-Warped Hide** — The Drowned Oracle
- **Stormfang Pelt** — Stormfang
- **Scarred Owlcat Pelt** — The Scarred Matriarch

### Group design and anti-exploit rules

- Tuned for **3–5 players**, with mechanics that reward a healer, cleanses, fear breaks, add control, and coordinated damage. Soloing is possible for heavily overgeared players, but isn't the intended experience.

- Every eligible quest holder in the killing group receives group credit and can interact with the same carcass for their own personal hide. One kill can advance up to five players, avoiding competition over a single spawn.

- The carcass remains skinable for five minutes. Hides are **Bind on Pickup, unique, and quest-only**, and can only be earned through the personal skinning interaction — never purchased, traded, or farmed repeatedly.

- The contracts require **Skinning 300**, are one-time only, and cannot be repeated. If a boss is on cooldown, players can complete the other hunts and return later.
