#include "AllCreatureScript.h"
#include "AllSpellScript.h"
#include "AscensionClientSpellPatches.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "Log.h"
#include "Map.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "Spell.h"
#include "TemporarySummon.h"
#include "Timer.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <random>
#include <unordered_map>
#include <vector>

namespace
{
    constexpr uint32 APEX_STORMSFANG_ENTRY = 9916212;
    constexpr uint32 APEX_SPIRIT_WOLF_ENTRY = 9916211;
    constexpr uint32 APEX_THUNDER_POUNCE = 1004112;
    constexpr uint32 APEX_POUNCE_IMPACT = 1004116;
    constexpr uint32 APEX_STORMCALL = 1004117;
    constexpr uint32 APEX_LIGHTNING_CALL = 1004118;
    constexpr uint32 APEX_THUNDERCLAP = 100377;
    constexpr uint32 APEX_RAKE = 100331;
    constexpr uint32 APEX_EAT_NO_SHEATHE_EMOTE = 122;
    constexpr float APEX_WOLF_SEPARATION_RADIUS = 2.5f;

    constexpr uint32 APEX_POUNCE_FIRST_DELAY_MS = 10000;
    constexpr uint32 APEX_POUNCE_RETRY_MS = 5000;
    constexpr uint32 APEX_STORMCALL_FIRST_DELAY_MS = 30000;
    constexpr uint32 APEX_STORMCALL_RETRY_MS = 5000;
    constexpr uint32 APEX_STORM_HOWL_FIRST_DELAY_MS = 20000;
    constexpr uint32 APEX_STORM_HOWL_RETRY_MS = 5000;
    constexpr uint32 APEX_STORM_HOWL_DURATION_MS = 25000;
    constexpr uint32 APEX_SPIRIT_WOLF_RETRY_MS = 5000;
    constexpr uint32 APEX_POUNCE_MIN_DELAY_MS = 350;
    constexpr uint32 APEX_POUNCE_MAX_DELAY_MS = 1500;
    constexpr float APEX_POUNCE_RADIUS = 30.0f;
    constexpr float APEX_STORM_HOWL_RADIUS = 40.0f;
    constexpr float APEX_STORMCALL_RADIUS = 50.0f;

    constexpr std::array<uint32, 10> APEX_CLIENT_SPELLS = {
        100331, 1004111, 1004112, 1004113, 1004114,
        1004115, 1004116, 1004117, 1004118, 1004119};

    std::vector<Player*> NearbyPlayers(Creature* creature, float radius,
        Player* excluded = nullptr)
    {
        std::vector<Player*> players;
        Map::PlayerList const& list = creature->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator itr = list.begin();
             itr != list.end(); ++itr)
        {
            Player* player = itr->GetSource();
            if (!player || !player->IsAlive() || player == excluded ||
                !player->InSamePhase(creature) ||
                !player->IsHostileTo(creature) ||
                !creature->IsWithinDistInMap(player, radius))
                continue;
            players.push_back(player);
        }
        return players;
    }

    void Shuffle(std::vector<Player*>& players)
    {
        std::mt19937 rng(static_cast<uint32>(getMSTime()));
        std::shuffle(players.begin(), players.end(), rng);
    }

    Player* SelectNonTankTarget(Creature* creature, Player* tank,
        float radius, ObjectGuid excluded = ObjectGuid::Empty)
    {
        if (!tank)
            return nullptr;

        std::vector<Player*> players = NearbyPlayers(creature, radius, tank);
        if (players.size() > 1)
            players.erase(std::remove_if(players.begin(), players.end(),
                [excluded](Player* player)
                {
                    return excluded && player->GetGUID() == excluded;
                }), players.end());
        if (players.empty())
            return nullptr;
        Shuffle(players);
        return players.front();
    }

    void SummonSpiritWolf(Creature* summoner, Player* target, float angleOffset)
    {
        if (!summoner || !target)
            return;

        float angle = summoner->GetOrientation() + angleOffset;
        float x = summoner->GetPositionX() + 5.0f * std::cos(angle);
        float y = summoner->GetPositionY() + 5.0f * std::sin(angle);

        if (TempSummon* wolf = summoner->SummonCreature(
                APEX_SPIRIT_WOLF_ENTRY, x, y, summoner->GetPositionZ(),
                target->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN,
                APEX_STORM_HOWL_DURATION_MS))
        {
            wolf->SetFaction(summoner->GetFaction());
            wolf->GetThreatMgr().AddThreat(target, 100.0f);
            wolf->AI()->AttackStart(target);
        }
    }

    void SummonStormHowlWolves(Creature* creature)
    {
        Unit* victim = creature->GetVictim();
        Player* tank = victim ? victim->ToPlayer() : nullptr;
        std::vector<Player*> nonTanks = NearbyPlayers(creature, APEX_STORM_HOWL_RADIUS, tank);
        Shuffle(nonTanks);

        Player* first = nullptr;
        Player* second = nullptr;
        if (nonTanks.size() >= 2)
        {
            first = nonTanks[0];
            second = nonTanks[1];
        }
        else if (nonTanks.size() == 1)
        {
            first = nonTanks[0];
            second = nonTanks[0];
        }
        else if (tank)
        {
            first = tank;
            second = tank;
        }

        if (!first || !second)
            return;

        SummonSpiritWolf(creature, first, 0.0f);
        SummonSpiritWolf(creature, second, 3.14159265f);
    }

    void StartStormcall(Creature* caster)
    {
        if (!caster)
            return;

        for (uint32 delay : {0u, 1000u, 2000u, 3000u, 4000u, 5000u, 6000u})
        {
            caster->m_Events.AddEventAtOffset([caster]()
            {
                if (!caster || !caster->IsAlive())
                    return;

                std::vector<Player*> players =
                    NearbyPlayers(caster, APEX_STORMCALL_RADIUS);
                for (Player* player : players)
                {
                    if (!player->IsAlive())
                        continue;

                    caster->CastSpell(player, APEX_LIGHTNING_CALL, true);
                }
            }, Milliseconds(delay));
        }

        caster->SetControlled(true, UNIT_STATE_ROOT);
        caster->SetEmoteState(Emote(APEX_EAT_NO_SHEATHE_EMOTE));
        caster->HandleEmoteCommand(APEX_EAT_NO_SHEATHE_EMOTE);

        caster->m_Events.AddEventAtOffset([caster]()
        {
            if (caster)
            {
                caster->SetControlled(false, UNIT_STATE_ROOT);
                caster->ClearEmoteState();
                caster->HandleEmoteCommand(0);
            }
        }, 7000ms);
    }

    void KeepWolfSeparation(Creature* wolf)
    {
        Unit* victim = wolf->GetVictim();
        if (!wolf->IsInCombat() || !victim)
            return;

        static std::unordered_map<uint32, uint32> nextMove;
        uint32 const key = wolf->GetGUID().GetCounter();
        uint32 const now = getMSTime();
        auto cooldown = nextMove.find(key);
        if (cooldown != nextMove.end() && now < cooldown->second)
            return;

        std::list<Creature*> wolves;
        wolf->GetCreatureListWithEntryInGrid(wolves, APEX_SPIRIT_WOLF_ENTRY, 8.0f);
        for (Creature* other : wolves)
        {
            if (other == wolf || !other->IsAlive() || !wolf->InSamePhase(other) ||
                other->GetVictim() != victim)
                continue;

            float const distance = wolf->GetExactDist2d(
                other->GetPositionX(), other->GetPositionY());
            if (distance >= APEX_WOLF_SEPARATION_RADIUS ||
                wolf->GetGUID().GetCounter() > other->GetGUID().GetCounter())
                continue;

            wolf->GetMotionMaster()->MoveChase(
                victim, 0.0f, static_cast<float>(M_PI) * 0.5f);
            nextMove[key] = now + 1000;
            break;
        }
    }

    class SkinpeelerStormfangSpellScript : public AllSpellScript
    {
    public:
        SkinpeelerStormfangSpellScript()
            : AllSpellScript("SkinpeelerStormfangSpellScript",
                  {ALLSPELLHOOK_ON_CAST})
        {
        }

        void OnSpellCast(Spell* spell, Unit*,
            SpellInfo const*, bool) override
        {
            if (!spell || !spell->GetCaster() ||
                spell->GetCaster()->GetEntry() != APEX_STORMSFANG_ENTRY)
                return;

            if (spell->m_spellInfo->Id == APEX_THUNDER_POUNCE)
            {
                Unit* caster = spell->GetCaster();
                Unit* target = spell->m_targets.GetUnitTarget();
                if (!caster || !target)
                    return;

                ObjectGuid targetGuid = target->GetGUID();
                uint32 delayMs = static_cast<uint32>(
                    caster->GetExactDist2d(target) * 1000.0f / 24.0f) + 150u;
                if (delayMs < APEX_POUNCE_MIN_DELAY_MS)
                    delayMs = APEX_POUNCE_MIN_DELAY_MS;
                if (delayMs > APEX_POUNCE_MAX_DELAY_MS)
                    delayMs = APEX_POUNCE_MAX_DELAY_MS;

                caster->m_Events.AddEventAtOffset([caster, targetGuid]()
                {
                    if (!caster || !caster->IsAlive())
                        return;

                    if (Unit* landingTarget =
                            ObjectAccessor::GetUnit(*caster, targetGuid))
                    {
                        caster->PlayDirectSound(36559);
                        caster->CastSpell(landingTarget, APEX_POUNCE_IMPACT, true);
                    }
                }, Milliseconds(delayMs));
            }
        }
    };

    class SkinpeelerStormfangCreatureScript : public AllCreatureScript
    {
    public:
        SkinpeelerStormfangCreatureScript()
            : AllCreatureScript("SkinpeelerStormfangCreatureScript")
        {
        }

        void OnCreatureAddWorld(Creature* creature) override
        {
            if (creature && creature->GetEntry() == APEX_STORMSFANG_ENTRY)
                creature->SetReactState(REACT_DEFENSIVE);
        }

        void OnAllCreatureUpdate(Creature* creature, uint32) override
        {
            if (!creature || creature->GetEntry() != APEX_STORMSFANG_ENTRY)
            {
                if (creature && creature->GetEntry() == APEX_SPIRIT_WOLF_ENTRY)
                    KeepWolfSeparation(creature);
                return;
            }

            if (creature->GetReactState() == REACT_AGGRESSIVE && !creature->IsInCombat())
                creature->SetReactState(REACT_DEFENSIVE);

            static std::unordered_map<uint32, uint32> nextPounce;
            static std::unordered_map<uint32, uint32> nextStormcall;
            static std::unordered_map<uint32, uint32> nextStormHowl;
            static std::unordered_map<uint32, uint32> nextMeleeSound;
            static std::unordered_map<uint32, uint32> nextThunderclap;
            static std::unordered_map<uint32, uint32> nextRake;
            static std::unordered_map<uint32, ObjectGuid> lastPounce;
            uint32 const key = creature->GetGUID().GetCounter();
            uint32 const now = getMSTime();

            if (!creature->IsAlive())
            {
                nextPounce.erase(key);
                nextStormcall.erase(key);
                nextStormHowl.erase(key);
                nextMeleeSound.erase(key);
                nextThunderclap.erase(key);
                nextRake.erase(key);
                lastPounce.erase(key);
                return;
            }

            if (!creature->IsInCombat())
            {
                nextPounce[key] = now + APEX_POUNCE_FIRST_DELAY_MS;
                nextStormcall[key] = now + APEX_STORMCALL_FIRST_DELAY_MS;
                nextStormHowl[key] = now + APEX_STORM_HOWL_FIRST_DELAY_MS;
                nextMeleeSound[key] = now;
                nextThunderclap[key] = now + 3000;
                nextRake[key] = now + 8000;
                lastPounce.erase(key);
                return;
            }

            float const healthPct = creature->GetHealthPct();
            bool const stormcallActive = creature->HasAura(APEX_STORMCALL);

            auto clap = nextThunderclap.find(key);
            if (clap == nextThunderclap.end())
            {
                nextThunderclap.emplace(key, now + 3000);
            }
            else if (now >= clap->second)
            {
                Unit* victim = creature->GetVictim();
                if (!stormcallActive && victim &&
                    creature->CastSpell(victim, APEX_THUNDERCLAP, true) == SPELL_CAST_OK)
                    clap->second = now + 12000;
                else
                    clap->second = now + 1000;
            }

            auto rake = nextRake.find(key);
            if (rake == nextRake.end())
            {
                nextRake.emplace(key, now + 8000);
            }
            else if (now >= rake->second)
            {
                Unit* victim = creature->GetVictim();
                if (!stormcallActive && victim &&
                    creature->CastSpell(victim, APEX_RAKE, true) == SPELL_CAST_OK)
                    rake->second = now + 8000;
                else
                    rake->second = now + 1000;
            }

            auto pounce = nextPounce.find(key);
            if (pounce == nextPounce.end())
            {
                nextPounce.emplace(key, now + APEX_POUNCE_FIRST_DELAY_MS);
            }
            else if (now >= pounce->second)
            {
                if (stormcallActive)
                {
                    pounce->second = now + 1000;
                }
                else
                {
                    Unit* victim = creature->GetVictim();
                    Player* tank = victim ? victim->ToPlayer() : nullptr;
                    ObjectGuid previous = lastPounce.count(key)
                        ? lastPounce[key] : ObjectGuid::Empty;
                    Player* target = SelectNonTankTarget(
                        creature, tank, APEX_POUNCE_RADIUS, previous);
                    if (target)
                    {
                        creature->CastSpell(target, APEX_THUNDER_POUNCE, true);
                        lastPounce[key] = target->GetGUID();
                        uint32 interval = healthPct < 40.0f ? 12000 :
                            (healthPct < 70.0f ? 14000 : 16000);
                        pounce->second = now + interval;
                    }
                    else
                    {
                        pounce->second = now + APEX_POUNCE_RETRY_MS;
                    }
                }
            }

            auto howl = nextStormHowl.find(key);
            if (howl == nextStormHowl.end())
            {
                nextStormHowl.emplace(key, now + APEX_STORM_HOWL_FIRST_DELAY_MS);
            }
            else if (now >= howl->second)
            {
                if (stormcallActive)
                {
                    howl->second = now + 1000;
                }
                else if (creature->GetVictim() ||
                         !NearbyPlayers(creature, APEX_STORM_HOWL_RADIUS).empty())
                {
                    SummonStormHowlWolves(creature);
                    uint32 interval = healthPct < 40.0f ? 35000 : 45000;
                    howl->second = now + interval;
                }
                else
                {
                    howl->second = now + APEX_STORM_HOWL_RETRY_MS;
                }
            }

            auto call = nextStormcall.find(key);
            if (call == nextStormcall.end())
            {
                nextStormcall.emplace(key, now + APEX_STORMCALL_FIRST_DELAY_MS);
            }
            else if (now >= call->second)
            {
                if (!NearbyPlayers(creature, APEX_STORMCALL_RADIUS).empty() &&
                    !creature->IsNonMeleeSpellCast(false))
                {
                    SpellCastResult result = creature->CastSpell(creature, APEX_STORMCALL, true);
                    if (result != SPELL_CAST_OK)
                    {
                        LOG_WARN("coa", "Stormfang Stormcall cast failed {}", uint32(result));
                        call->second = now + APEX_STORMCALL_RETRY_MS;
                        return;
                    }

                    StartStormcall(creature);
                    uint32 interval = healthPct < 40.0f ? 40000 :
                        (healthPct < 70.0f ? 50000 : 60000);
                    call->second = now + interval;
                }
                else
                {
                    call->second = now + APEX_STORMCALL_RETRY_MS;
                }
            }

            auto meleeSound = nextMeleeSound.find(key);
            if (meleeSound == nextMeleeSound.end())
                nextMeleeSound.emplace(key, now);
            else if (now >= meleeSound->second &&
                     creature->GetVictim() &&
                     !creature->HasAura(APEX_STORMCALL))
            {
                creature->PlayDirectSound(4069);
                meleeSound->second = now + 2000;
            }
        }
    };
}

void Addmod_skinpeelerquestScripts()
{
    for (uint32 spellId : APEX_CLIENT_SPELLS)
        Ascension::ClientSpellPatches::Instance().Register(spellId);

    new SkinpeelerStormfangSpellScript();
    new SkinpeelerStormfangCreatureScript();
}
