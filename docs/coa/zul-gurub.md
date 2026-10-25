# Zul'Gurub restoration — state of play and plan

This documents the state of Zul'Gurub (map 309) on CoA before `feat/zg-restoration`, compared with the live
Ascension CoA raid, and what the branch is to change. As with [Molten Core](molten-core.md), every row is tagged with
the evidence it rests on, and every open point stays open rather than guessed.

## 1. Summary

Live CoA ran Zul'Gurub on four difficulties — Normal, Heroic, Mythic and Ascended — with per-boss achievements for
each (Normal, Heroic, Mythic, Ascended 10/25, Nightmare). The client still offers all four: `MapDifficulty` 67-70
(difficulty 0-3) and `LFGDungeons` 420/42/435/450 are present and byte-identical to the server's DBC.

The server only has Normal:

- `dungeon_access_template` holds one row for map 309 (id 20, difficulty 0, level 50). There are no 1xxxxx, 2xxxxx
  or 3xxxxx creature templates, every `difficulty_entry_1..3` is 0 and every spawn is `spawnMask` 1, so Heroic,
  Mythic and Ascended cannot be entered.
- Boss health in `coa_boss_flex` is "built, not measured" and runs above the one live clear on record.
- Several boss timers differ from that clear, and some boss, add and trash spells cast live are missing.
- Hakkar's Normal loot lacks the raid cache, the Offering Key and the sigil. Quest reputation, 57 quests and 27
  Zandalar vendor items are missing or wrong.

## 2. Evidence

| tag | source | what it holds for ZG |
|---|---|---|
| **log** | CoA combat log `2026-08-02-18.28.55 WoWCombatLog.txt` (db.exil.es archive, old-logs) | the only full live clear: 12 players, 11 from Hakkar on. No Jin'do, Gahz'ranka, Hazza'rah, Renataki or Wushoolay |
| **wdb** | client caches (AscensionDB, client captures) | creature ranks, HealthModifier, ManaModifier, displays; quests; items |
| **exiles** | db.exil.es export 2026-09-13 | boss kits (spell lists), absolute health, loot |
| **sub** | db.exil.es dump 2026-10-04 | loot per difficulty for all 13 bosses, spawns, quests, vendors |
| **dbc** | installed game client vs `data/dbc` | Map, MapDifficulty, LFGDungeons, DungeonEncounter, Spell, SpellDifficulty: identical |
| **tavern** | coatavern.com | items, spells and achievements only; NPC and quest pages return 404 |
| **descension** | Project Descension (78dcaeeb) | stock ZG scripts; WDB and Exiles staging rows for the Loa Avatars and Heart of Hakkar |

A value found only in an archive (exiles, sub, or a lone wdb row that contradicts other sources) needs approval before
it goes into a fix. Health in exiles and sub does not scale consistently against the log (ratios 5.4x-10.2x), so
neither is used as a health target.

## 3. Access and difficulties

| item | server | expected | evidence |
|---|---|---|---|
| `dungeon_access_template` | id 20, difficulty 0, level 50 | difficulties 0-3, level 60 (LFG target 57-60) | dbc |
| variant templates | none | entry + 100000 / 200000 / 300000 for bosses and trash, `difficulty_entry_1..3` | dbc, sub, tavern |
| attunement | none | unknown | — |
| `lfg_dungeon_template` | none | unknown; nothing shows live ZG in RDF | — |
| release gate | `StageZulGurub` 4, dist `ReleaseStage` 3: closed by default | config decision | — |

## 4. Bosses

Measured health per player is net damage taken in the log divided by the group size; flex is assumed linear.

| boss | server `hp_d0` per player | measured per player | timers to retune (script → log) | missing live spells |
|---|---|---|---|---|
| 14517 Jeklik | 497,928 | 370,178 | Mass Fear 48 → 44 s, Greater Heal first 29 → 21 s, Call of Hir'eek first 20 → 22 s | — |
| 14507 Venoxis | 497,928 | 370,221 | Renew 24 → 28 s, Holy Nova 18 → 17 s | — |
| 14510 Mar'li | 497,928 | 370,327 | Web Spray 55 → 45 s, Poison Shock 25 → 45 s | 2118140 Burrow, 2118150 Poison Nova |
| 14509 Thekal | 248,964 | 188,435 | Disrupting Shout → 25.8 s, War Stomp and Speed Slash → 40 s | — |
| 14515 Arlokk | 497,928 | 367,800 | Claw Storm 20 → 55 s | — |
| 11382 Mandokir | 622,364 | 510,568 | Mortal Strike 10 → 12 s | — |
| 14834 Hakkar | 647,307 | 582,793 | Blood Scythe 12 → 15 s, Blood Siphon 90 → 59.5 s first, 79.5 s repeat | 2118607 Pool of Corrupted Blood |
| 15082 Gri'lek | 108,341 | above 133k (not killed at 11) | Ground Slam 13 → 24 s first, 25 s repeat | — |
| 11380 Jin'do, 15114 Gahz'ranka, 15083-15085 | — | no log | unknown | unknown |

All bosses and adds: Fierce Blow 975011 and Knockdown 520871 are cast live and absent on the server (Molten Core
precedent: `rev_20261005_10`/`_11`).

Adds still on stock AzerothCore kits, CoA kits seen in the log:

| entry | add | live kit |
|---|---|---|
| 11347 | Zealot Lor'Khan | 2118260 Greater Heal, 2118261-2118263 Storm Shield, 2118267, 2760204 Holy Ward |
| 11348 | Zealot Zath | 2118250 Sinister Strike, 2118252 Blind, 2118255 Disarm, 2118263, 2760204 |
| 14988 | Ohgan | 2106616 Shred Armor, 2106617 Dash, 570147 (role unknown) |
| 15117 | Chained Spirit | 2118426 Resurrection |
| 14750 | Gurubashi Bat Rider | 350048 Infected Bite in place of 16128 |
| 15041 | Spawn of Mar'li | 2118117 Growth |
| 15101 | Zulian Prowler | 2118350 Prowl, 2118351 Shred, 2118352 Shadowstep |
| 11357 | Son of Hakkar | 2118675 Pool of Poisoned Blood |

Archive-only boss points (approval needed):

- Loa Avatars 666988-666992 die at each priest's 50 % phase change in the log; they are absent from `coa_world`.
  Templates exist only in wdb/exiles.
- Heart of Hakkar: the log has 92005 (wdb rank 3, model 28887, HM 17.55); the script uses 9780022
  (`boss_hakkar_coa.cpp`), an id that is "Netherstorm Fel Quests" in sub.
- Ohgan rank 1 / display 215271, Spawn of Mar'li display 959, Powerful Healing Ward named "Healing Stream Totem"
  (wdb).
- Exiles-only spells never cast in the log: 2106125 Bat-tering Charge, Venoxis 2118081, 2118085, 2118045.

## 5. Trash and spawns

HealthModifier matches wdb on every trash entry and stays. Live kits from the log (all ids present in the client and
server Spell.dbc):

| entry | trash | change |
|---|---|---|
| 11338 | Hakkari Shadowcaster | 15232 → 350383, 20741 → 350386 |
| 11350 | Gurubashi Axe Thrower | add 2118900, 2118901, 2118905; 8269 → 340021 |
| 11351 | Gurubashi Headhunter | 15708 → 340009 |
| 11352 | Gurubashi Berserker | 15588 → 340080, 11130 → 340164, 8269 → 340021; add 2118910 |
| 11359 | Soulflayer | 8269 → 340021 |
| 11361, 15068 | Zulian Tiger, Zulian Guardian | no AI: add 24332 Rake, 24333 Ravage |
| 11368 | Bloodseeker Bat | add 2106151 Vampiric Bite, 2106156 Frenzied Dive |
| 11373, 14884 | Razzashi Cobra, Parasitic Serpent | 24097 → 2118075 |
| 11830 | Hakkari Priest | 22883 → 85309 |
| 14532 | Razzashi Venombrood | add 2118914 |
| 14821 | Razzashi Raptor | 8599 → 340031, 24339 → 350048 |
| 15043 | Zulian Crocolisk | 13445 → 340516 |
| 15111 | Mad Servant | 24611 → 350395, 24612 → 350399 |

Fierce Blow 975011 and Berserking 1126297 are cast by almost every trash type live.

wdb corrections: ManaModifier 11339, 11340, 11830, 11831, 14825 → 1.35; 11388 → 0.3375; 14883, 15111 → 0.65.
14826 Sacrificed Troll display 14391 → 27651.

Spawns: server 927, sub 807, 792 guids shared. The gap is AzerothCore pools and random-entry rolls, not extra
creatures. Open, archive-only: entry swaps on 51391 (→ 11338), 51389, 51392, 51394, 91457, 49139, 49195-49197;
sub-only guid 200616 Hakkari Priest; possible stacked duplicates 49149 and 52265-52271. The 37 gameobject templates
match sub.

## 6. Loot, items, reputation, quests, vendors

Tier item ids are confirmed by wdb and tavern: Heroic base + 300000, Mythic base + 1300000, Ascended base + 200000.
All 520 item ids that sub lists in ZG boss loot exist in `item_template`.

| item | server | expected | evidence |
|---|---|---|---|
| Hakkar Normal | no cache, key or sigil | 1400023 Cache of the Blood God 100 %, 97725 Offering Key 100 %, 60108 sigil 1 % | wdb, exiles, sub |
| boss sigils (13) | absent | 1 % each | exiles, sub |
| Edge of Madness blood items 19939-19942 | 20 % | 100 % | sub, TBC data |
| variant loot | unreachable | tiered ids per difficulty | sub only |
| Hakkar caches per tier | — | Heroic 1400039, Mythic 2400039, Ascended 2400039 (sub) or 2500039 (tavern, server item) | uncertain |
| quest reputation, 81 ZG quests | `RewardFactionOverride` → +100/150/200 | override 0 → +250/350/500 | wdb, exiles |
| MinLevel 8048, 8184-8192, 9208-9210 / 8183 | 58 / 58 | 10 / 60 | wdb, sub |
| missing quests | — | Zanza 1508184-1508189, [HEROIC] Paragons 208053-208079, Heart of Hakkar 208183/308183/408183, Inscriptions 208184-208198, Hero's Call 77878 | wdb, questcache; givers unknown |
| Rin'wosho 14921 | 22 items | plus 27 reputation-gated CoA items 195065-195097 | wdb, sub |

Archive-only loot (approval needed): Zandalar set and Battleweaver pieces as 3.333 % boss drops (sub only; quest
rewards in vanilla and live AtlasLoot), 0 %-chance CoA pieces on bosses and 43 trash greens, Edge 12504, Gahz'ranka
12503 and 1022739.

Left as is unless decided otherwise: quests 580290 and 580295 carry local Path to Ascension titles and MinLevel 8.

## 7. Plan

Confirmed items first, then the variants once their scale is decided:

- [ ] Hakkar Normal cache, Offering Key and sigil; boss sigils; Edge of Madness blood items at 100 %
- [ ] Quest reputation and MinLevel; missing quests (givers to be settled)
- [ ] Rin'wosho reputation items
- [ ] Boss timers and missing live spells; Fierce Blow and Knockdown on bosses and adds; add kits
- [ ] Trash kits, Fierce Blow and Berserking; ManaModifier and display corrections
- [ ] Normal boss health from the log
- [ ] Access rows for difficulties 1-3 at level 60
- [ ] Variant templates, health and loot for Heroic, Mythic and Ascended

## 8. Open decisions

1. Normal health: adopt the measured per-player values, linear in group size, from one 11-12 player clear?
2. Variant health scale: sub ×2/×3/×4, Molten Core ×1.44/×1.88/×2.32, or the single wdb row 311374 (×1.544)?
3. Heart of Hakkar 92005 in place of 9780022; Loa Avatar templates 666988-666992.
4. Zandalar set as a boss drop, and the 0 %-chance CoA pieces.
5. Ascended cache 2400039 or 2500039.
6. Rank 1 (wdb) or normal (pfQuest, exiles) for 11360, 11368, 11387-11391, 14758, 14826.
7. Archive-only spawn swaps and the sub-only Hakkari Priest.
