/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

 /* ScriptData
 SDName: Instance_Wailing_Caverns
 SD%Complete: 99
 SDComment: Everything seems to work, still need some checking
 SDCategory: Wailing Caverns
 EndScriptData */

#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "ScriptedEscortAI.h"
#include "ScriptedGossip.h"
#include "TemporarySummon.h"
#include "wailing_caverns_lfm.h"
#include "ScriptedEscortAI.h"
#include "Creature.h"
#include "Map.h"
#include "wailing_caverns_lfm.h"

enum Enums
{
    SAY_AT_LAST = 0,
    SAY_MAKE_PREPARATIONS = 1,
    SAY_TEMPLE_OF_PROMISE = 2,
    SAY_MUST_CONTINUE = 3,
    SAY_BANISH_THE_SPIRITS = 4,
    SAY_CAVERNS_PURIFIED = 5,
    SAY_BEYOND_THIS_CORRIDOR = 6,
    SAY_EMERALD_DREAM = 7,
    EMOTE_AWAKENING_RITUAL = 8,
    EMOTE_TROUBLED_SLEEP = 0,
    EMOTE_WRITHE_IN_AGONY = 1,
    EMOTE_HORRENDOUS_VISION = 2,
    SAY_MUTANUS_THE_DEVOURER = 9,
    SAY_I_AM_AWAKE = 3,
    SAY_NARALEX_AWAKES = 10,
    SAY_THANK_YOU = 4,
    SAY_FAREWELL = 5,
    SAY_ATTACKED = 11,

    GOSSIP_OPTION_LET_EVENT_BEGIN = 201,
    NPC_TEXT_NARALEX_SLEEPS_AGAIN = 698,
    NPC_TEXT_FANGLORDS_ARE_DEAD = 699,

    SPELL_MARK_OF_THE_WILD_RANK_2 = 5232,
    SPELL_SERPENTINE_CLEANSING = 6270,
    SPELL_NARALEXS_AWAKENING = 6271,
    SPELL_FLIGHT_FORM = 33943,

    NPC_DEVIATE_RAVAGER = 3636,
    NPC_DEVIATE_VIPER = 5755,
    NPC_DEVIATE_MOCCASIN = 5762,
    NPC_NIGHTMARE_ECTOPLASM = 5763,
    NPC_MUTANUS_THE_DEVOURER = 3654,

    PATH_ESCORT_NARALEX_DISCIPLE = 29426,
};

class instance_wailing_caverns : public InstanceMapScript
{
public:
    instance_wailing_caverns() : InstanceMapScript(WCScriptName, 43) { }

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_wailing_caverns_InstanceMapScript(map);
    }

    struct instance_wailing_caverns_InstanceMapScript : public InstanceScript
    {
        instance_wailing_caverns_InstanceMapScript(InstanceMap* map) : InstanceScript(map)
        {
            SetHeaders(DataHeader);
            SetBossNumber(MAX_ENCOUNTER);

            yelled = false;
        }

        bool yelled;
        ObjectGuid NaralexGUID;
        ObjectGuid DiscipleOfNaralexGUID;

        void OnCreatureCreate(Creature* creature) override
        {
            uint32 creatureEntry = creature->GetEntry();
            if (creatureEntry == DATA_NARALEX)
            {
                NaralexGUID = creature->GetGUID();
            }
            else if (creatureEntry == 3678)
            {
                DiscipleOfNaralexGUID = creature->GetGUID();
            }
        }

        void OnUnitDeath(Unit* unit) override
        {
            switch (unit->GetEntry())
            {
            case NPC_KRESH:                 SetBossState(BOSS_KRESH, DONE); break;
            case NPC_SKUM:                  SetBossState(BOSS_SKUM, DONE); break;
            case NPC_VERDAN_THE_EVERLIVING: SetBossState(BOSS_VERDAN_THE_EVERLIVING, DONE); break;
            case NPC_Lady_Anacondra:
            {
                SetBossState(BOSS_LADY_ANACONDRA, DONE);
                DiscipleYell();
                break;
            }
            case NPC_Lord_Cobrahn:
            {
                SetBossState(BOSS_LORD_COBRAHN, DONE);
                DiscipleYell();
                break;
            }
            case NPC_Lord_Pythas:
            {
                SetBossState(BOSS_LORD_PYTHAS, DONE);
                DiscipleYell();
                break;
            }
            case NPC_Lord_Serpentis:
            {
                SetBossState(BOSS_LORD_SERPENTIS, DONE);
                DiscipleYell();
                break;
            }
            default:
                break;
            }
        }

        void DiscipleYell()
        {
            SetBossState(BOSS_LADY_ANACONDRA, DONE);
            if ((GetData(TYPE_LORD_COBRAHN) == DONE) && (GetData(TYPE_LORD_PYTHAS) == DONE) &&
                (GetData(TYPE_LADY_ANACONDRA) == DONE) && (GetData(TYPE_LORD_SERPENTIS) == DONE))
            {
                if (Creature* discipleOfNaralex = instance->GetCreature(DiscipleOfNaralexGUID))
                {
                    if (!GetData(TYPE_NARALEX_YELLED))
                    {
                        discipleOfNaralex->Talk(SAY_AT_LAST, ChatMsg::CHAT_MSG_YELL, 1000.0f, nullptr);
                        SetData(TYPE_NARALEX_YELLED, 1);
                    }
                }
            }
        }

        void SetData(uint32 type, uint32 data) override
        {
            switch (type)
            {
            case TYPE_LORD_COBRAHN:         SetBossState(BOSS_LORD_COBRAHN, EncounterState(data)); break;
            case TYPE_LORD_PYTHAS:          SetBossState(BOSS_LORD_PYTHAS, EncounterState(data)); break;
            case TYPE_LADY_ANACONDRA:       SetBossState(BOSS_LADY_ANACONDRA, EncounterState(data)); break;
            case TYPE_LORD_SERPENTIS:       SetBossState(BOSS_LORD_SERPENTIS, EncounterState(data)); break;
            case TYPE_NARALEX_EVENT:        SetBossState(4, EncounterState(data)); break;
            case TYPE_NARALEX_PART1:        SetBossState(5, EncounterState(data)); break;
            case TYPE_NARALEX_PART2:        SetBossState(6, EncounterState(data)); break;
            case TYPE_NARALEX_PART3:        SetBossState(7, EncounterState(data)); break;
            case TYPE_MUTANUS_THE_DEVOURER: SetBossState(BOSS_MUTANUS_THE_DEVOURER, EncounterState(data)); break;
            case TYPE_NARALEX_YELLED:       yelled = true;      break;
            default:                        break;
            }
        }

        uint32 GetData(uint32 type) const override
        {
            switch (type)
            {
            case TYPE_LORD_COBRAHN:         return GetBossState(BOSS_LORD_COBRAHN);
            case TYPE_LORD_PYTHAS:          return GetBossState(BOSS_LORD_PYTHAS);
            case TYPE_LADY_ANACONDRA:       return GetBossState(BOSS_LADY_ANACONDRA);
            case TYPE_LORD_SERPENTIS:       return GetBossState(BOSS_LORD_SERPENTIS);
            case TYPE_NARALEX_EVENT:        return GetBossState(4);
            case TYPE_NARALEX_PART1:        return GetBossState(5);
            case TYPE_NARALEX_PART2:        return GetBossState(6);
            case TYPE_NARALEX_PART3:        return GetBossState(7);
            case TYPE_MUTANUS_THE_DEVOURER: return GetBossState(BOSS_MUTANUS_THE_DEVOURER);
            case TYPE_NARALEX_YELLED:       return yelled;
            default:                        break;
            }
            return 0;
        }

        ObjectGuid GetGuidData(uint32 data) const override
        {
            if (data == DATA_NARALEX)return NaralexGUID;
            return ObjectGuid::Empty;
        }
    };

};

class npc_disciple_of_naralex : public CreatureScript
{
public:
    npc_disciple_of_naralex() : CreatureScript("npc_disciple_of_naralex") { }

    struct npc_disciple_of_naralexAI : public npc_escortAI
    {
        npc_disciple_of_naralexAI(Creature* creature) : npc_escortAI(creature)
        {
            instance = creature->GetInstanceScript();
            eventTimer = 0;
            currentEvent = 0;
            eventProgress = 0;
            me->setActive(true);
            me->SetVisibilityDistanceOverride(VisibilityDistanceType::Gigantic);
            me->SetImmuneToPC(false);
        }

        uint32 eventTimer;
        uint32 currentEvent;
        uint32 eventProgress;
        InstanceScript* instance;

        void WaypointReached(uint32 waypointId) override
        {
            switch (waypointId)
            {
            case 4:
                eventProgress = 1;
                currentEvent = TYPE_NARALEX_PART1;
                instance->SetData(TYPE_NARALEX_PART1, IN_PROGRESS);
                break;
            case 5:
                Talk(SAY_MUST_CONTINUE);
                instance->SetData(TYPE_NARALEX_PART1, DONE);
                break;
            case 11:
                eventProgress = 1;
                currentEvent = TYPE_NARALEX_PART2;
                instance->SetData(TYPE_NARALEX_PART2, IN_PROGRESS);
                break;
            case 19:
                Talk(SAY_BEYOND_THIS_CORRIDOR);
                break;
            case 24:
                eventProgress = 1;
                currentEvent = TYPE_NARALEX_PART3;
                instance->SetData(TYPE_NARALEX_PART3, IN_PROGRESS);
                break;
            }
        }

        void Reset() override
        {

        }

        void JustEngagedWith(Unit* who) override
        {
            Talk(SAY_ATTACKED, who);
        }

        void JustDied(Unit* /*killer*/) override
        {
            instance->SetData(TYPE_NARALEX_EVENT, FAIL);
            instance->SetData(TYPE_NARALEX_PART1, FAIL);
            instance->SetData(TYPE_NARALEX_PART2, FAIL);
            instance->SetData(TYPE_NARALEX_PART3, FAIL);
        }

        void JustSummoned(Creature* summoned) override
        {
            summoned->AI()->AttackStart(me);
        }

        void UpdateAI(uint32 diff) override
        {
            if (currentEvent != TYPE_NARALEX_PART3)
                npc_escortAI::UpdateAI(diff);

            if (eventTimer <= diff)
            {
                eventTimer = 0;
                if (instance->GetData(currentEvent) == IN_PROGRESS)
                {
                    switch (currentEvent)
                    {
                    case TYPE_NARALEX_PART1:
                        if (eventProgress == 1)
                        {
                            ++eventProgress;
                            Talk(SAY_TEMPLE_OF_PROMISE);
                            me->SummonCreature(NPC_DEVIATE_RAVAGER, -82.1763f, 227.874f, -93.3233f, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);
                            me->SummonCreature(NPC_DEVIATE_RAVAGER, -72.9506f, 216.645f, -93.6756f, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);
                        }
                        break;
                    case TYPE_NARALEX_PART2:
                        if (eventProgress == 1)
                        {
                            ++eventProgress;
                            Talk(SAY_BANISH_THE_SPIRITS);
                            DoCast(me, SPELL_SERPENTINE_CLEANSING);
                            //CAST_AI(EscortAI, me->AI())->SetCanDefend(false);
                            eventTimer = 30000;
                            me->SummonCreature(NPC_DEVIATE_VIPER, -61.5261f, 273.676f, -92.8442f, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);
                            me->SummonCreature(NPC_DEVIATE_VIPER, -58.4658f, 280.799f, -92.8393f, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);
                            me->SummonCreature(NPC_DEVIATE_VIPER, -50.002f, 278.578f, -92.8442f, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);
                        }
                        else
                            if (eventProgress == 2)
                            {
                                //CAST_AI(EscortAI, me->AI())->SetCanDefend(true);
                                Talk(SAY_CAVERNS_PURIFIED);
                                instance->SetData(TYPE_NARALEX_PART2, DONE);
                                if (me->HasAura(SPELL_SERPENTINE_CLEANSING))
                                    me->RemoveAura(SPELL_SERPENTINE_CLEANSING);
                            }
                        break;
                    case TYPE_NARALEX_PART3:
                        if (eventProgress == 1)
                        {
                            ++eventProgress;
                            eventTimer = 4000;
                            me->SetStandState(UNIT_STAND_STATE_KNEEL);
                            Talk(SAY_EMERALD_DREAM);
                        }
                        else
                            if (eventProgress == 2)
                            {
                                ++eventProgress;
                                eventTimer = 15000;
                                //CAST_AI(EscortAI, me->AI())->SetCanDefend(false);
                                if (Creature* naralex = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_NARALEX)))
                                    DoCast(naralex, SPELL_NARALEXS_AWAKENING, true);
                                Talk(EMOTE_AWAKENING_RITUAL);
                            }
                            else
                                if (eventProgress == 3)
                                {
                                    ++eventProgress;
                                    eventTimer = 15000;
                                    if (Creature* naralex = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_NARALEX)))
                                    {
                                        naralex->AI()->Talk(EMOTE_TROUBLED_SLEEP);
                                        if (Creature* evils = me->SummonCreature(NPC_DEVIATE_MOCCASIN, 135.943f, 199.701f, -103.529f, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 15000))
                                        {
                                            evils->GetMotionMaster()->MovePoint(0, naralex->GetPosition());
                                        }
                                        if (Creature* evils = me->SummonCreature(NPC_DEVIATE_MOCCASIN, 151.08f, 221.13f, -103.609f, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 15000))
                                        {
                                            evils->GetMotionMaster()->MovePoint(0, naralex->GetPosition());
                                        }
                                        if (Creature* evils = me->SummonCreature(NPC_DEVIATE_MOCCASIN, 128.007f, 227.428f, -97.421f, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 15000))
                                        {
                                            evils->GetMotionMaster()->MovePoint(0, naralex->GetPosition());
                                        }
                                    }
                                }
                                else
                                    if (eventProgress == 4)
                                    {
                                        ++eventProgress;
                                        eventTimer = 30000;
                                        if (Creature* naralex = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_NARALEX)))
                                        {
                                            naralex->AI()->Talk(EMOTE_WRITHE_IN_AGONY);
                                            if (Creature* evils = me->SummonCreature(NPC_NIGHTMARE_ECTOPLASM, 133.413f, 207.188f, -102.469f, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 15000))
                                            {
                                                evils->GetMotionMaster()->MovePoint(0, naralex->GetPosition());
                                            }
                                            if (Creature* evils = me->SummonCreature(NPC_NIGHTMARE_ECTOPLASM, 142.857f, 218.645f, -102.905f, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 15000))
                                            {
                                                evils->GetMotionMaster()->MovePoint(0, naralex->GetPosition());
                                            }
                                            if (Creature* evils = me->SummonCreature(NPC_NIGHTMARE_ECTOPLASM, 105.102f, 227.211f, -102.752f, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 15000))
                                            {
                                                evils->GetMotionMaster()->MovePoint(0, naralex->GetPosition());
                                            }
                                            if (Creature* evils = me->SummonCreature(NPC_NIGHTMARE_ECTOPLASM, 153.372f, 235.149f, -102.826f, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 15000))
                                            {
                                                evils->GetMotionMaster()->MovePoint(0, naralex->GetPosition());
                                            }
                                            if (Creature* evils = me->SummonCreature(NPC_NIGHTMARE_ECTOPLASM, 149.524f, 251.113f, -102.558f, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 15000))
                                            {
                                                evils->GetMotionMaster()->MovePoint(0, naralex->GetPosition());
                                            }
                                            if (Creature* evils = me->SummonCreature(NPC_NIGHTMARE_ECTOPLASM, 136.208f, 266.466f, -102.977f, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 15000))
                                            {
                                                evils->GetMotionMaster()->MovePoint(0, naralex->GetPosition());
                                            }
                                            if (Creature* evils = me->SummonCreature(NPC_NIGHTMARE_ECTOPLASM, 126.167f, 274.759f, -102.962f, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 15000))
                                            {
                                                evils->GetMotionMaster()->MovePoint(0, naralex->GetPosition());
                                            }
                                        }
                                    }
                                    else
                                        if (eventProgress == 5)
                                        {
                                            ++eventProgress;
                                            if (Creature* naralex = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_NARALEX)))
                                            {
                                                naralex->AI()->Talk(EMOTE_HORRENDOUS_VISION);
                                                if (Creature* mutanus = me->SummonCreature(NPC_MUTANUS_THE_DEVOURER, 150.872f, 262.905f, -103.503f, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000000))
                                                {
                                                    mutanus->GetMotionMaster()->MovePoint(0, naralex->GetPosition());
                                                    Talk(SAY_MUTANUS_THE_DEVOURER, mutanus);
                                                    instance->SetData(TYPE_MUTANUS_THE_DEVOURER, IN_PROGRESS);
                                                }
                                            }
                                        }
                                        else
                                            if (eventProgress == 6 && instance->GetData(TYPE_MUTANUS_THE_DEVOURER) == DONE)
                                            {
                                                ++eventProgress;
                                                eventTimer = 3000;
                                                if (Creature* naralex = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_NARALEX)))
                                                {
                                                    if (me->HasAura(SPELL_NARALEXS_AWAKENING))
                                                        me->RemoveAura(SPELL_NARALEXS_AWAKENING);
                                                    naralex->SetStandState(UNIT_STAND_STATE_STAND);
                                                    naralex->AI()->Talk(SAY_I_AM_AWAKE);
                                                }
                                                Talk(SAY_NARALEX_AWAKES);
                                            }
                                            else
                                                if (eventProgress == 7)
                                                {
                                                    ++eventProgress;
                                                    eventTimer = 6000;
                                                    if (Creature* naralex = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_NARALEX)))
                                                        naralex->AI()->Talk(SAY_THANK_YOU);
                                                }
                                                else
                                                    if (eventProgress == 8)
                                                    {
                                                        ++eventProgress;
                                                        eventTimer = 8000;
                                                        if (Creature* naralex = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_NARALEX)))
                                                        {
                                                            naralex->AI()->Talk(SAY_FAREWELL);
                                                            naralex->AddAura(SPELL_FLIGHT_FORM, naralex);
                                                        }
                                                        me->SetStandState(UNIT_STAND_STATE_STAND);
                                                        me->AddAura(SPELL_FLIGHT_FORM, me);
                                                    }
                                                    else
                                                        if (eventProgress == 9)
                                                        {
                                                            ++eventProgress;
                                                            eventTimer = 1500;
                                                            if (Creature* naralex = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_NARALEX)))
                                                                naralex->GetMotionMaster()->MovePoint(25, naralex->GetPositionX(), naralex->GetPositionY(), naralex->GetPositionZ());
                                                        }
                                                        else
                                                            if (eventProgress == 10)
                                                            {
                                                                ++eventProgress;
                                                                eventTimer = 2500;
                                                                if (Creature* naralex = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_NARALEX)))
                                                                {
                                                                    naralex->GetMotionMaster()->MovePoint(0, 117.095512f, 247.107971f, -96.167870f);
                                                                    naralex->GetMotionMaster()->MovePoint(1, 90.388809f, 276.135406f, -83.389801f);
                                                                }
                                                                me->GetMotionMaster()->MovePoint(0, 117.095512f, 247.107971f, -96.167870f);
                                                                me->GetMotionMaster()->MovePoint(1, 144.375443f, 281.045837f, -82.477135f);
                                                            }
                                                            else
                                                                if (eventProgress == 11)
                                                                {
                                                                    if (Creature* naralex = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_NARALEX)))
                                                                        naralex->SetVisible(false);
                                                                    me->SetVisible(false);
                                                                    instance->SetData(TYPE_NARALEX_PART3, DONE);
                                                                }
                        break;
                    }
                }
            }
            else eventTimer -= diff;
        }

        void sGossipSelect(Player* player, uint32 sender, uint32 action) override
        {
            ClearGossipMenuFor(player);
            if (action == GOSSIP_ACTION_INFO_DEF + 1 || action == 0)
            {
                CloseGossipMenuFor(player);
                if (instance)
                    instance->SetData(TYPE_NARALEX_EVENT, IN_PROGRESS);

                Talk(SAY_MAKE_PREPARATIONS);

                me->SetFaction(FACTION_ESCORTEE_N_NEUTRAL_ACTIVE);
                me->SetImmuneToPC(false);

                Start(true, false, player->GetGUID(), nullptr, true, false, true);
                SetDespawnAtFar(false);
                SetDespawnAtEnd(false);
            }
        }

        void sGossipHello(Player* player) override
        {
            DoCast(player, SPELL_MARK_OF_THE_WILD_RANK_2, true);
            if ((instance->GetData(TYPE_LORD_COBRAHN) == DONE) && (instance->GetData(TYPE_LORD_PYTHAS) == DONE) &&
                (instance->GetData(TYPE_LADY_ANACONDRA) == DONE) && (instance->GetData(TYPE_LORD_SERPENTIS) == DONE))
            {
                ClearGossipMenuFor(player);
                AddGossipItemFor(player, GOSSIP_OPTION_LET_EVENT_BEGIN, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                SendGossipMenuFor(player, NPC_TEXT_FANGLORDS_ARE_DEAD, me->GetGUID());
            }
            else
            {
                SendGossipMenuFor(player, NPC_TEXT_NARALEX_SLEEPS_AGAIN, me->GetGUID());
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return GetWailingCavernsAI<npc_disciple_of_naralexAI>(creature);
    }
};

void AddSC_instance_wailing_caverns()
{
    new npc_disciple_of_naralex();
    new instance_wailing_caverns();
}
