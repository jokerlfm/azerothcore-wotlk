#include "NingerManager.h"
#include "AccountMgr.h"
#include "NingerStrategy_Base.h"
#include "NingerConfig.h"
#include "Group.h"
#include "NingerAction_Base.h"
#include "Chat.h"
#include "SpellMgr.h"
#include "SpellAuras.h"
#include "MotionMaster.h"
#include "Map.h"
#include "Pet.h"
#include "GridNotifiers.h"

// lfm ninger 
#include <boost/chrono/duration.hpp>

NingerManager::NingerManager()
{
    checkDelay = 0;
    allianceRaces.clear();
    hordeRaces.clear();
    nameIndex = 0;
    ningerNameMap.clear();
    ningerEntitySet.clear();
    characterTalentTabNameMap.clear();
}

void NingerManager::InitializeManager()
{
    sLog->outMessage(NINGER_MARK, LogLevel::LOG_LEVEL_INFO, "Initialize ninger");

    checkDelay = sNingerConfig->ManagerCheckDelay;

    allianceRaces.clear();
    allianceRaces[CLASS_WARRIOR][allianceRaces[CLASS_WARRIOR].size()] = RACE_HUMAN;
    allianceRaces[CLASS_WARRIOR][allianceRaces[CLASS_WARRIOR].size()] = RACE_NIGHTELF;
    allianceRaces[CLASS_WARRIOR][allianceRaces[CLASS_WARRIOR].size()] = RACE_GNOME;
    allianceRaces[CLASS_WARRIOR][allianceRaces[CLASS_WARRIOR].size()] = RACE_DWARF;
    hordeRaces[CLASS_WARRIOR][hordeRaces[CLASS_WARRIOR].size()] = RACE_ORC;
    hordeRaces[CLASS_WARRIOR][hordeRaces[CLASS_WARRIOR].size()] = Races::RACE_UNDEAD_PLAYER;
    hordeRaces[CLASS_WARRIOR][hordeRaces[CLASS_WARRIOR].size()] = RACE_TAUREN;
    hordeRaces[CLASS_WARRIOR][hordeRaces[CLASS_WARRIOR].size()] = RACE_TROLL;

    allianceRaces[CLASS_PALADIN][allianceRaces[CLASS_PALADIN].size()] = RACE_HUMAN;
    allianceRaces[CLASS_PALADIN][allianceRaces[CLASS_PALADIN].size()] = RACE_DWARF;
    hordeRaces[CLASS_PALADIN][hordeRaces[CLASS_PALADIN].size()] = RACE_BLOODELF;

    allianceRaces[CLASS_ROGUE][allianceRaces[CLASS_ROGUE].size()] = RACE_HUMAN;
    allianceRaces[CLASS_ROGUE][allianceRaces[CLASS_ROGUE].size()] = RACE_DWARF;
    allianceRaces[CLASS_ROGUE][allianceRaces[CLASS_ROGUE].size()] = RACE_NIGHTELF;
    allianceRaces[CLASS_ROGUE][allianceRaces[CLASS_ROGUE].size()] = RACE_GNOME;
    hordeRaces[CLASS_ROGUE][hordeRaces[CLASS_ROGUE].size()] = RACE_ORC;
    hordeRaces[CLASS_ROGUE][hordeRaces[CLASS_ROGUE].size()] = RACE_TROLL;
    hordeRaces[CLASS_ROGUE][hordeRaces[CLASS_ROGUE].size()] = Races::RACE_UNDEAD_PLAYER;

    allianceRaces[CLASS_PRIEST][allianceRaces[CLASS_PRIEST].size()] = RACE_HUMAN;
    allianceRaces[CLASS_PRIEST][allianceRaces[CLASS_PRIEST].size()] = RACE_DWARF;
    allianceRaces[CLASS_PRIEST][allianceRaces[CLASS_PRIEST].size()] = RACE_NIGHTELF;
    hordeRaces[CLASS_PRIEST][hordeRaces[CLASS_PRIEST].size()] = RACE_TROLL;
    hordeRaces[CLASS_PRIEST][hordeRaces[CLASS_PRIEST].size()] = Races::RACE_UNDEAD_PLAYER;

    allianceRaces[CLASS_MAGE][allianceRaces[CLASS_MAGE].size()] = RACE_HUMAN;
    allianceRaces[CLASS_MAGE][allianceRaces[CLASS_MAGE].size()] = RACE_GNOME;
    hordeRaces[CLASS_MAGE][hordeRaces[CLASS_MAGE].size()] = Races::RACE_UNDEAD_PLAYER;
    hordeRaces[CLASS_MAGE][hordeRaces[CLASS_MAGE].size()] = RACE_TROLL;

    allianceRaces[CLASS_WARLOCK][allianceRaces[CLASS_WARLOCK].size()] = RACE_HUMAN;
    allianceRaces[CLASS_WARLOCK][allianceRaces[CLASS_WARLOCK].size()] = RACE_GNOME;
    hordeRaces[CLASS_WARLOCK][hordeRaces[CLASS_WARLOCK].size()] = Races::RACE_UNDEAD_PLAYER;
    hordeRaces[CLASS_WARLOCK][hordeRaces[CLASS_WARLOCK].size()] = RACE_ORC;

    allianceRaces[CLASS_SHAMAN][allianceRaces[CLASS_SHAMAN].size()] = RACE_DRAENEI;
    hordeRaces[CLASS_SHAMAN][hordeRaces[CLASS_SHAMAN].size()] = RACE_ORC;
    hordeRaces[CLASS_SHAMAN][hordeRaces[CLASS_SHAMAN].size()] = RACE_TAUREN;
    hordeRaces[CLASS_SHAMAN][hordeRaces[CLASS_SHAMAN].size()] = RACE_TROLL;

    allianceRaces[CLASS_HUNTER][allianceRaces[CLASS_HUNTER].size()] = RACE_DWARF;
    allianceRaces[CLASS_HUNTER][allianceRaces[CLASS_HUNTER].size()] = RACE_NIGHTELF;
    hordeRaces[CLASS_HUNTER][hordeRaces[CLASS_HUNTER].size()] = RACE_ORC;
    hordeRaces[CLASS_HUNTER][hordeRaces[CLASS_HUNTER].size()] = RACE_TAUREN;
    hordeRaces[CLASS_HUNTER][hordeRaces[CLASS_HUNTER].size()] = RACE_TROLL;

    allianceRaces[CLASS_DRUID][allianceRaces[CLASS_DRUID].size()] = RACE_NIGHTELF;
    hordeRaces[CLASS_DRUID][hordeRaces[CLASS_DRUID].size()] = RACE_TAUREN;

    characterTalentTabNameMap.clear();
    characterTalentTabNameMap[Classes::CLASS_WARRIOR][0] = "Arms";
    characterTalentTabNameMap[Classes::CLASS_WARRIOR][1] = "Fury";
    characterTalentTabNameMap[Classes::CLASS_WARRIOR][2] = "Protection";

    characterTalentTabNameMap[Classes::CLASS_HUNTER][0] = "Beast Mastery";
    characterTalentTabNameMap[Classes::CLASS_HUNTER][1] = "Marksmanship";
    characterTalentTabNameMap[Classes::CLASS_HUNTER][2] = "Survival";

    characterTalentTabNameMap[Classes::CLASS_SHAMAN][0] = "Elemental";
    characterTalentTabNameMap[Classes::CLASS_SHAMAN][1] = "Enhancement";
    characterTalentTabNameMap[Classes::CLASS_SHAMAN][2] = "Restoration";

    characterTalentTabNameMap[Classes::CLASS_PALADIN][0] = "Holy";
    characterTalentTabNameMap[Classes::CLASS_PALADIN][1] = "Protection";
    characterTalentTabNameMap[Classes::CLASS_PALADIN][2] = "Retribution";

    characterTalentTabNameMap[Classes::CLASS_WARLOCK][0] = "Affliction";
    characterTalentTabNameMap[Classes::CLASS_WARLOCK][1] = "Demonology";
    characterTalentTabNameMap[Classes::CLASS_WARLOCK][2] = "Destruction";

    characterTalentTabNameMap[Classes::CLASS_PRIEST][0] = "Discipline";
    characterTalentTabNameMap[Classes::CLASS_PRIEST][1] = "Holy";
    characterTalentTabNameMap[Classes::CLASS_PRIEST][2] = "Shadow";

    characterTalentTabNameMap[Classes::CLASS_ROGUE][0] = "Assassination";
    characterTalentTabNameMap[Classes::CLASS_ROGUE][1] = "Combat";
    characterTalentTabNameMap[Classes::CLASS_ROGUE][2] = "subtlety";

    characterTalentTabNameMap[Classes::CLASS_MAGE][0] = "Arcane";
    characterTalentTabNameMap[Classes::CLASS_MAGE][1] = "Fire";
    characterTalentTabNameMap[Classes::CLASS_MAGE][2] = "Frost";

    characterTalentTabNameMap[Classes::CLASS_DRUID][0] = "Balance";
    characterTalentTabNameMap[Classes::CLASS_DRUID][1] = "Feral";
    characterTalentTabNameMap[Classes::CLASS_DRUID][2] = "Restoration";

    if (sNingerConfig->Reset)
    {
        DeleteNingers();
    }

    QueryResult ningerQR = CharacterDatabase.Query("SELECT ninger_id, account_name, character_id, target_level, target_race, target_class, target_specialty FROM ninger");
    if (ningerQR)
    {
        do
        {
            Field* fields = ningerQR->Fetch();
            NingerEntity* re = new NingerEntity();
            re->ninger_id = fields[0].Get<uint32>();
            re->account_name = fields[1].Get<std::string>();
            re->character_id = fields[2].Get<uint32>();
            re->target_level = fields[3].Get<uint32>();
            re->target_race = fields[4].Get<uint32>();
            re->target_class = fields[5].Get<uint32>();
            re->target_specialty = fields[6].Get<uint32>();
            ningerEntitySet.insert(re);
        } while (ningerQR->NextRow());
    }

    nameIndex = 0;
    QueryResult ningerNameQR = WorldDatabase.Query("SELECT name FROM ninger_names order by rand()");
    if (ningerNameQR)
    {
        do
        {
            Field* fields = ningerNameQR->Fetch();
            std::string eachName = fields[0].Get<std::string>();
            ningerNameMap[ningerNameMap.size()] = eachName;
        } while (ningerNameQR->NextRow());
    }

    sLog->outMessage(NINGER_MARK, LogLevel::LOG_LEVEL_INFO, "ninger initialized");
}

NingerManager* NingerManager::instance()
{
    static NingerManager instance;
    return &instance;
}

void NingerManager::UpdateNingerManager(uint32 pmDiff)
{
    if (sNingerConfig->Enable == 0)
    {
        return;
    }

    if (checkDelay >= 0)
    {
        checkDelay -= pmDiff;
    }
    if (checkDelay < 0)
    {
        checkDelay = sNingerConfig->ManagerCheckDelay;
        std::unordered_set<uint32> onlinePlayerLevelSet;
        onlinePlayerLevelSet.clear();
        std::unordered_map<uint32, WorldSession*> allSessions = sWorld->GetAllSessions();
        for (std::unordered_map<uint32, WorldSession*>::iterator wsIT = allSessions.begin(); wsIT != allSessions.end(); wsIT++)
        {
            if (WorldSession* eachWS = wsIT->second)
            {
                if (!eachWS->isNinger)
                {
                    if (Player* eachPlayer = eachWS->GetPlayer())
                    {
                        uint32 eachLevel = eachPlayer->getLevel();
                        if (onlinePlayerLevelSet.find(eachLevel) == onlinePlayerLevelSet.end())
                        {
                            onlinePlayerLevelSet.insert(eachLevel);
                        }
                    }
                }
            }
        }
        for (std::unordered_set<uint32>::iterator levelIT = onlinePlayerLevelSet.begin(); levelIT != onlinePlayerLevelSet.end(); levelIT++)
        {
            uint32 eachLevel = *levelIT;
            LoginNinger(eachLevel);
        }
    }
}

void NingerManager::UpdateNingerEntities(uint32 pmDiff)
{
    for (std::unordered_set<NingerEntity*>::iterator reIT = ningerEntitySet.begin(); reIT != ningerEntitySet.end(); reIT++)
    {
        (*reIT)->Update(pmDiff);
    }
}

void NingerManager::DeleteNingers()
{
    QueryResult ningerQR = CharacterDatabase.Query("SELECT account_id FROM ninger");
    if (ningerQR)
    {
        do
        {
            Field* fields = ningerQR->Fetch();
            uint32 eachAccountId = fields[0].Get<uint32>();
            if (eachAccountId > 0)
            {
                AccountMgr::DeleteAccount(eachAccountId);
            }
        } while (ningerQR->NextRow());
    }
    CharacterDatabase.DirectExecute("delete from ninger");

    std::ostringstream accountQueryStream;
    accountQueryStream << "SELECT id FROM account where username like '" << NINGER_MARK << "%'";
    std::string accountQuerySQL = accountQueryStream.str();
    QueryResult ningerAccountQR = LoginDatabase.Query(accountQuerySQL);
    if (ningerAccountQR)
    {
        do
        {
            Field* fields = ningerAccountQR->Fetch();
            uint32 eachAccountId = fields[0].Get<uint32>();
            if (eachAccountId > 0)
            {
                AccountMgr::DeleteAccount(eachAccountId);
            }
        } while (ningerAccountQR->NextRow());
    }
}

bool NingerManager::LoginNinger(uint32 pmLevel)
{
    if (pmLevel >= 20)
    {
        uint32 currentCount = 0;
        QueryResult levelNingerQR = CharacterDatabase.Query("SELECT count(*) FROM ninger where target_level = {}", pmLevel);
        if (levelNingerQR)
        {
            Field* fields = levelNingerQR->Fetch();
            currentCount = fields[0].Get<uint32>();
        }
        if (currentCount < sNingerConfig->NingerCountEachLevel)
        {
            uint32 addCount = sNingerConfig->NingerCountEachLevel - currentCount;
            uint32 allianceCount = addCount / 2;
            uint32 hordeCount = addCount - allianceCount;
            int checkCount = allianceCount;
            while (checkCount > 0)
            {
                CreateNinger(pmLevel, true);
                checkCount--;
            }
            checkCount = hordeCount;
            while (checkCount > 0)
            {
                CreateNinger(pmLevel, false);
                checkCount--;
            }
        }
        uint32 onlineCount = 0;
        for (std::unordered_set<NingerEntity*>::iterator reIT = ningerEntitySet.begin(); reIT != ningerEntitySet.end(); reIT++)
        {
            if (NingerEntity* eachRE = *reIT)
            {
                if (eachRE->target_level == pmLevel)
                {
                    if (eachRE->entityState != NingerEntityState::NingerEntityState_OffLine && eachRE->entityState != NingerEntityState::NingerEntityState_None)
                    {
                        onlineCount++;
                    }
                }
            }
        }
        int toOnline = 0;
        if (sNingerConfig->NingerCountEachLevel > onlineCount)
        {
            toOnline = sNingerConfig->NingerCountEachLevel - onlineCount;
        }
        if (toOnline > 0)
        {
            for (std::unordered_set<NingerEntity*>::iterator reIT = ningerEntitySet.begin(); reIT != ningerEntitySet.end(); reIT++)
            {
                if (NingerEntity* eachRE = *reIT)
                {
                    if (eachRE->target_level == pmLevel)
                    {
                        if (eachRE->entityState == NingerEntityState::NingerEntityState_OffLine)
                        {
                            eachRE->entityState = NingerEntityState::NingerEntityState_Enter;
                            toOnline--;
                            if (toOnline <= 0)
                            {
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    return true;
}

void NingerManager::CreateNinger(uint32 pmLevel, bool pmAlliance)
{
    uint32 currentNingerCount = 0;
    QueryResult ningerQR = CharacterDatabase.Query("SELECT count(*) FROM ninger");
    if (ningerQR)
    {
        Field* fields = ningerQR->Fetch();
        currentNingerCount = fields[0].Get<uint32>();
    }
    std::ostringstream accountNameStream;
    accountNameStream << NINGER_MARK << currentNingerCount;
    std::string checkAccountName = accountNameStream.str();
    std::ostringstream querySQLStream;
    querySQLStream << "SELECT * FROM account where username ='" << checkAccountName << "'";
    std::string querySQL = querySQLStream.str();
    QueryResult accountNameQR = LoginDatabase.Query(querySQL.c_str());
    if (accountNameQR)
    {
        sLog->outMessage(NINGER_MARK, LogLevel::LOG_LEVEL_INFO, "Account already %s exists.", checkAccountName.c_str());
    }
    else
    {
        uint32 target_class = Classes::CLASS_PRIEST;
        uint32 target_race = 0;
        if (pmAlliance)
        {
            uint32 raceIndex = urand(0, allianceRaces[target_class].size() - 1);
            target_race = allianceRaces[target_class][raceIndex];
        }
        else
        {
            uint32 raceIndex = urand(0, hordeRaces[target_class].size() - 1);
            target_race = hordeRaces[target_class][raceIndex];
        }
        uint32 target_specialty = 0;
        std::ostringstream sqlStream;
        sqlStream << "INSERT INTO ninger (ninger_id,account_id, account_name, character_id, target_level, target_race, target_class, target_specialty) VALUES (" << currentNingerCount << ", 0, '" << checkAccountName << "', 0, " << pmLevel << ", " << target_race << ", " << target_class << ", " << target_specialty << ")";
        std::string sql = sqlStream.str();
        CharacterDatabase.DirectExecute(sql.c_str());
        NingerEntity* re = new NingerEntity();
        re->ninger_id = currentNingerCount;
        re->account_id = 0;
        re->account_name = checkAccountName;
        re->character_id = 0;
        re->target_level = pmLevel;
        re->target_race = target_race;
        re->target_class = target_class;
        re->target_specialty = target_specialty;
        ningerEntitySet.insert(re);
        std::ostringstream replyStream;
        replyStream << "ninger " << checkAccountName << " created";
        sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, replyStream.str().c_str());
    }
}

void NingerManager::HandlePlayerSay(Player* pmPlayer, std::string pmContent)
{
    //if (!pmPlayer)
    //{
    //    return;
    //}
    //std::vector<std::string> commandVector = SplitString(pmContent, " ", true);
    //std::string commandName = commandVector.at(0);
    //if (commandName == "role")
    //{
    //    if (Awareness_Base* playerAI = pmPlayer->awarenessMap[pmPlayer->activeAwarenessIndex])
    //    {
    //        if (commandVector.size() > 1)
    //        {
    //            std::string newRole = commandVector.at(1);
    //            playerAI->SetGroupRole(newRole);
    //        }
    //        std::ostringstream replyStream;
    //        replyStream << "Your group role is ";
    //        replyStream << playerAI->GetGroupRoleName();
    //        sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, replyStream.str().c_str(), pmPlayer);
    //    }
    //}
    //else if (commandName == "arrangement")
    //{
    //    std::ostringstream replyStream;
    //    if (Group* myGroup = pmPlayer->GetGroup())
    //    {
    //        if (myGroup->GetLeaderGUID() == pmPlayer->GetGUID())
    //        {
    //            bool paladinAura_concentration = false;
    //            bool paladinAura_devotion = false;
    //            bool paladinAura_retribution = false;
    //            bool paladinAura_fire = false;
    //            bool paladinAura_frost = false;
    //            bool paladinAura_shadow = false;

    //            bool paladinBlessing_kings = false;
    //            bool paladinBlessing_might = false;
    //            bool paladinBlessing_wisdom = false;
    //            bool paladinBlessing_salvation = false;

    //            bool paladinSeal_Justice = false;

    //            bool warlockCurse_Weakness = false;
    //            bool warlockCurse_Tongues = false;
    //            bool warlockCurse_Element = false;

    //            int rtiIndex = 0;

    //            bool hunterAspect_wild = false;

    //            for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
    //            {
    //                Player* member = groupRef->GetSource();
    //                if (member)
    //                {
    //                    if (member->GetMapId() == 619)
    //                    {
    //                        member->activeAwarenessIndex = 619;
    //                    }
    //                    else if (member->GetMapId() == 555)
    //                    {
    //                        member->activeAwarenessIndex = 555;
    //                    }
    //                    else if (member->GetMapId() == 585)
    //                    {
    //                        member->activeAwarenessIndex = 585;
    //                    }
    //                    else
    //                    {
    //                        member->activeAwarenessIndex = 0;
    //                    }
    //                    if (Awareness_Base* memberAwareness = member->awarenessMap[member->activeAwarenessIndex])
    //                    {
    //                        memberAwareness->Reset();
    //                        if (Script_Base* sb = memberAwareness->sb)
    //                        {
    //                            switch (member->getClass())
    //                            {
    //                            case Classes::CLASS_WARRIOR:
    //                            {
    //                                memberAwareness->groupRole = GroupRole::GroupRole_Tank;
    //                                break;
    //                            }
    //                            case Classes::CLASS_SHAMAN:
    //                            {
    //                                memberAwareness->groupRole = GroupRole::GroupRole_Healer;
    //                                break;
    //                            }
    //                            case Classes::CLASS_PALADIN:
    //                            {
    //                                memberAwareness->groupRole = GroupRole::GroupRole_Healer;
    //                                break;
    //                            }
    //                            case Classes::CLASS_PRIEST:
    //                            {
    //                                memberAwareness->groupRole = GroupRole::GroupRole_Healer;
    //                                break;
    //                            }
    //                            case Classes::CLASS_DRUID:
    //                            {
    //                                memberAwareness->groupRole = GroupRole::GroupRole_Tank;
    //                                break;
    //                            }
    //                            default:
    //                            {
    //                                break;
    //                            }
    //                            }
    //                            if (member->getClass() == Classes::CLASS_PALADIN)
    //                            {
    //                                if (Script_Paladin* sp = (Script_Paladin*)sb)
    //                                {
    //                                    if (memberAwareness->groupRole != GroupRole::GroupRole_Healer)
    //                                    {
    //                                        if (!paladinSeal_Justice)
    //                                        {
    //                                            sp->sealType = PaladinSealType::PaladinSealType_Justice;
    //                                            paladinSeal_Justice = true;
    //                                        }
    //                                        else
    //                                        {
    //                                            sp->sealType = PaladinSealType::PaladinSealType_Righteousness;
    //                                        }
    //                                    }
    //                                    if (!paladinBlessing_salvation)
    //                                    {
    //                                        sp->blessingType = PaladinBlessingType::PaladinBlessingType_Salvation;
    //                                        paladinBlessing_salvation = true;
    //                                    }
    //                                    else if (!paladinBlessing_might)
    //                                    {
    //                                        sp->blessingType = PaladinBlessingType::PaladinBlessingType_Might;
    //                                        paladinBlessing_might = true;
    //                                    }
    //                                    else if (!paladinBlessing_wisdom)
    //                                    {
    //                                        sp->blessingType = PaladinBlessingType::PaladinBlessingType_Wisdom;
    //                                        paladinBlessing_wisdom = true;
    //                                    }
    //                                    else if (!paladinBlessing_kings)
    //                                    {
    //                                        sp->blessingType = PaladinBlessingType::PaladinBlessingType_Kings;
    //                                        paladinBlessing_kings = true;
    //                                    }
    //                                    else
    //                                    {
    //                                        sp->blessingType = PaladinBlessingType::PaladinBlessingType_Might;
    //                                        paladinBlessing_might = true;
    //                                    }

    //                                    if (!paladinAura_devotion)
    //                                    {
    //                                        sp->auraType = PaladinAuraType::PaladinAuraType_Devotion;
    //                                        paladinAura_devotion = true;
    //                                    }
    //                                    else if (!paladinAura_retribution)
    //                                    {
    //                                        sp->auraType = PaladinAuraType::PaladinAuraType_Retribution;
    //                                        paladinAura_retribution = true;
    //                                    }
    //                                    else if (!paladinAura_concentration)
    //                                    {
    //                                        sp->auraType = PaladinAuraType::PaladinAuraType_Concentration;
    //                                        paladinAura_concentration = true;
    //                                    }
    //                                    else if (!paladinAura_fire)
    //                                    {
    //                                        sp->auraType = PaladinAuraType::PaladinAuraType_FireResistant;
    //                                        paladinAura_fire = true;
    //                                    }
    //                                    else if (!paladinAura_frost)
    //                                    {
    //                                        sp->auraType = PaladinAuraType::PaladinAuraType_FrostResistant;
    //                                        paladinAura_frost = true;
    //                                    }
    //                                    else if (!paladinAura_shadow)
    //                                    {
    //                                        sp->auraType = PaladinAuraType::PaladinAuraType_ShadowResistant;
    //                                        paladinAura_shadow = true;
    //                                    }
    //                                    else
    //                                    {
    //                                        sp->auraType = PaladinAuraType::PaladinAuraType_Devotion;
    //                                        paladinAura_devotion = true;
    //                                    }
    //                                }
    //                            }
    //                            if (member->getClass() == Classes::CLASS_MAGE)
    //                            {
    //                                if (rtiIndex >= 0 && rtiIndex < TARGETICONCOUNT)
    //                                {
    //                                    sb->rti = rtiIndex;
    //                                    rtiIndex++;
    //                                }
    //                            }
    //                            if (member->getClass() == Classes::CLASS_HUNTER)
    //                            {
    //                                if (Script_Hunter* sh = (Script_Hunter*)sb)
    //                                {
    //                                    if (hunterAspect_wild)
    //                                    {
    //                                        sh->aspectType = HunterAspectType::HunterAspectType_Hawk;
    //                                    }
    //                                    else
    //                                    {
    //                                        sh->aspectType = HunterAspectType::HunterAspectType_Wild;
    //                                        hunterAspect_wild = true;
    //                                    }
    //                                }
    //                            }
    //                            if (member->getClass() == Classes::CLASS_WARLOCK)
    //                            {
    //                                if (Script_Warlock* swl = (Script_Warlock*)sb)
    //                                {
    //                                    if (!warlockCurse_Weakness)
    //                                    {
    //                                        swl->curseType = WarlockCurseType::WarlockCurseType_Weakness;
    //                                        warlockCurse_Weakness = true;
    //                                    }
    //                                    else if (!warlockCurse_Tongues)
    //                                    {
    //                                        swl->curseType = WarlockCurseType::WarlockCurseType_Tongues;
    //                                        warlockCurse_Tongues = true;
    //                                    }
    //                                    else if (!warlockCurse_Element)
    //                                    {
    //                                        swl->curseType = WarlockCurseType::WarlockCurseType_Element;
    //                                        warlockCurse_Element = true;
    //                                    }
    //                                    else
    //                                    {
    //                                        swl->curseType = WarlockCurseType::WarlockCurseType_Weakness;
    //                                    }
    //                                }
    //                            }
    //                        }
    //                        memberAwareness->Report();
    //                    }
    //                }
    //            }
    //            replyStream << "Arranged";
    //        }
    //        else
    //        {
    //            replyStream << "You are not leader";
    //        }
    //    }
    //    else
    //    {
    //        replyStream << "Not in a group";
    //    }
    //    sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, replyStream.str().c_str(), pmPlayer);
    //}
    //else if (commandName == "join")
    //{
    //    std::ostringstream replyStream;
    //    Group* myGroup = pmPlayer->GetGroup();
    //    if (myGroup)
    //    {
    //        ObjectGuid targetGUID = pmPlayer->GetTarget();
    //        if (!targetGUID.IsEmpty())
    //        {
    //            bool validTarget = false;
    //            for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
    //            {
    //                Player* member = groupRef->GetSource();
    //                if (member)
    //                {
    //                    if (member->GetGUID() != pmPlayer->GetGUID())
    //                    {
    //                        if (member->GetGUID() == targetGUID)
    //                        {
    //                            validTarget = true;
    //                            replyStream << "Joining " << member->GetName();
    //                            pmPlayer->TeleportTo(member->GetMapId(), member->GetPositionX(), member->GetPositionY(), member->GetPositionZ(), member->GetOrientation());
    //                        }
    //                    }
    //                }
    //            }
    //            if (!validTarget)
    //            {
    //                replyStream << "Target is no group member";
    //            }
    //        }
    //        else
    //        {
    //            replyStream << "You have no target";
    //        }
    //    }
    //    else
    //    {
    //        replyStream << "You are not in a group";
    //    }
    //    sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, replyStream.str().c_str(), pmPlayer);
    //}
    //else if (commandName == "leader")
    //{
    //    if (Group* myGroup = pmPlayer->GetGroup())
    //    {
    //        if (myGroup->GetLeaderGUID() != pmPlayer->GetGUID())
    //        {
    //            bool change = true;
    //            if (Player* leader = ObjectAccessor::FindPlayer(myGroup->GetLeaderGUID()))
    //            {
    //                if (WorldSession* leaderSession = leader->GetSession())
    //                {
    //                    if (!leaderSession->isNinger)
    //                    {
    //                        change = false;
    //                    }
    //                }
    //            }
    //            if (change)
    //            {
    //                myGroup->ChangeLeader(pmPlayer->GetGUID());
    //            }
    //            else
    //            {
    //                sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, "Leader is valid", pmPlayer);
    //            }
    //        }
    //        else
    //        {
    //            sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, "You are the leader", pmPlayer);
    //        }
    //    }
    //    else
    //    {
    //        sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, "You are not in a group", pmPlayer);
    //    }
    //}
    //else if (commandName == "ninger")
    //{
    //    if (commandVector.size() > 1)
    //    {
    //        std::string ningerAction = commandVector.at(1);
    //        if (ningerAction == "delete")
    //        {
    //            std::ostringstream replyStream;
    //            bool allOffline = true;
    //            for (std::unordered_map<std::string, NingerEntity*>::iterator reIT = ningerEntityMap.begin(); reIT != ningerEntityMap.end(); reIT++)
    //            {
    //                NingerEntity* eachRE = reIT->second;
    //                if (eachRE->entityState != NingerEntityState::NingerEntityState_None && eachRE->entityState != NingerEntityState::NingerEntityState_OffLine)
    //                {
    //                    allOffline = false;
    //                    replyStream << "Not all ningers are offline. Going offline first";
    //                    LogoutNingers();
    //                    break;
    //                }
    //            }
    //            if (allOffline)
    //            {
    //                replyStream << "All ningers are offline. Ready to delete";
    //                Deleteningers();
    //            }
    //            sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, replyStream.str().c_str(), pmPlayer);
    //        }
    //        else if (ningerAction == "offline")
    //        {
    //            std::ostringstream replyStream;
    //            replyStream << "All ningers are going offline";
    //            LogoutNingers();
    //            sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, replyStream.str().c_str(), pmPlayer);
    //        }
    //        else if (ningerAction == "online")
    //        {
    //            uint32 playerLevel = pmPlayer->getLevel();
    //            if (playerLevel < 10)
    //            {
    //                std::ostringstream replyStream;
    //                replyStream << "You level is too low";
    //                sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, replyStream.str().c_str(), pmPlayer);
    //            }
    //            else
    //            {
    //                uint32 ningerCount = sNingerConfig->NingerCountEachLevel;
    //                if (commandVector.size() > 2)
    //                {
    //                    ningerCount = atoi(commandVector.at(2).c_str());
    //                }
    //                if (ningerCount > 0)
    //                {
    //                    std::ostringstream replyTitleStream;
    //                    replyTitleStream << "ninger count to go online : " << ningerCount;
    //                    sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, replyTitleStream.str().c_str(), pmPlayer);
    //                    LoginNingers(playerLevel, ningerCount);
    //                }
    //            }
    //        }
    //        else if (ningerAction == "relocate")
    //        {
    //            std::unordered_map<uint32, WorldSession*> allSessions = sWorld->GetAllSessions();
    //            for (std::unordered_map<uint32, WorldSession*>::iterator wsIT = allSessions.begin(); wsIT != allSessions.end(); wsIT++)
    //            {
    //                if (WorldSession* eachWS = wsIT->second)
    //                {
    //                    if (eachWS->isNinger)
    //                    {
    //                        if (Player* eachNinger = eachWS->GetPlayer())
    //                        {
    //                            if (eachNinger->IsInWorld())
    //                            {
    //                                eachNinger->TeleportTo(eachNinger->m_homebindMapId, eachNinger->m_homebindX, eachNinger->m_homebindY, eachNinger->m_homebindZ, 0.0f);

    //                                std::ostringstream replyTitleStream;
    //                                replyTitleStream << "Teleport ninger to homebind : " << eachNinger->GetName();
    //                                sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, replyTitleStream.str().c_str(), pmPlayer);
    //                            }
    //                        }
    //                    }
    //                }
    //            }
    //        }
    //        else if (ningerAction == "equip")
    //        {
    //            std::unordered_map<uint32, WorldSession*> allSessions = sWorld->GetAllSessions();
    //            for (std::unordered_map<uint32, WorldSession*>::iterator wsIT = allSessions.begin(); wsIT != allSessions.end(); wsIT++)
    //            {
    //                if (WorldSession* eachWS = wsIT->second)
    //                {
    //                    if (eachWS->isNinger)
    //                    {
    //                        if (Player* eachNinger = eachWS->GetPlayer())
    //                        {
    //                            if (eachNinger->IsInWorld())
    //                            {
    //                                InitializeEquipments(eachNinger, true);
    //                                std::ostringstream replyTitleStream;
    //                                replyTitleStream << "Reset equipments for ninger : " << eachNinger->GetName();
    //                                sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, replyTitleStream.str().c_str(), pmPlayer);
    //                            }
    //                        }
    //                    }
    //                }
    //            }
    //        }
    //    }
    //}
    //else if (commandName == "train")
    //{
    //    LearnPlayerSpells(pmPlayer);
    //}
    //else if (commandName == "talent")
    //{
    //    if (LearnPlayerTalents(pmPlayer))
    //    {
    //        if (WorldSession* ws = pmPlayer->GetSession())
    //        {
    //            ws->LogoutPlayer(true);
    //        }
    //    }
    //}
    //else if (commandName == "equip")
    //{
    //    InitializeEquipments(pmPlayer, true);
    //}
    //else if (commandName == "tcast")
    //{
    //    if (commandVector.size() > 1)
    //    {
    //        if (Unit* myTarget = pmPlayer->GetSelectedUnit())
    //        {
    //            std::string spellIDStr = commandVector.at(1);
    //            int32 spellID = atoi(spellIDStr.c_str());
    //            Unit* castTarget = nullptr;
    //            myTarget->CastSpell(castTarget, spellID, true);
    //        }
    //    }
    //}
    //else if (commandName == "poison")
    //{
    //    if (Awareness_Base* ab = pmPlayer->awarenessMap[pmPlayer->activeAwarenessIndex])
    //    {
    //        if (Script_Base* sb = ab->sb)
    //        {
    //            uint32 maxInstancePoisonLevel = 0;
    //            for (std::unordered_map<uint32, uint32>::iterator ipIT = sNingerManager->instantPoisonEntryMap.begin(); ipIT != sNingerManager->instantPoisonEntryMap.end(); ipIT++)
    //            {
    //                if (ipIT->first <= pmPlayer->getLevel())
    //                {
    //                    if (ipIT->first > maxInstancePoisonLevel)
    //                    {
    //                        maxInstancePoisonLevel = ipIT->first;
    //                    }
    //                }
    //            }
    //            if (maxInstancePoisonLevel > 0)
    //            {
    //                uint32 instancePoisonEntry = sNingerManager->instantPoisonEntryMap[maxInstancePoisonLevel];
    //                if (!pmPlayer->HasItemCount(instancePoisonEntry, 1))
    //                {
    //                    pmPlayer->StoreNewItemInBestSlots(instancePoisonEntry, 20);
    //                }
    //                Item* instancePoison = sb->GetItemInInventory(instancePoisonEntry);
    //                if (instancePoison && !instancePoison->IsInTrade())
    //                {
    //                    if (Item* weapon_mh = pmPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EquipmentSlots::EQUIPMENT_SLOT_MAINHAND))
    //                    {
    //                        if (sb->UseItem(instancePoison, weapon_mh))
    //                        {
    //                            if (Item* weapon_oh = pmPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, EquipmentSlots::EQUIPMENT_SLOT_OFFHAND))
    //                            {
    //                                if (sb->UseItem(instancePoison, weapon_oh))
    //                                {
    //                                    sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, "Added instance poison to weapons", pmPlayer);
    //                                }
    //                            }
    //                        }
    //                    }
    //                }
    //            }
    //        }
    //    }
    //}
    //else if (commandName == "aura")
    //{
    //    Unit* checkTarget = pmPlayer->GetSelectedUnit();
    //    if (commandVector.size() > 1)
    //    {
    //        std::string checkType = commandVector.at(1);
    //        if (checkType == "has")
    //        {
    //            if (commandVector.size() > 2)
    //            {
    //                std::string spellIDStr = commandVector.at(2);
    //                uint32 spellID = atoi(spellIDStr.c_str());
    //                if (!checkTarget)
    //                {
    //                    checkTarget = pmPlayer;
    //                }
    //                std::ostringstream replyStream;
    //                if (checkTarget->HasAura(spellID))
    //                {
    //                    replyStream << "has aura " << spellID;
    //                }
    //                else
    //                {
    //                    replyStream << "no aura " << spellID;
    //                }
    //                sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, replyStream.str().c_str(), pmPlayer);
    //            }
    //        }
    //    }
    //}
}

void NingerManager::HandleChatCommand(Player* pmSender, std::string pmCMD, Player* pmReceiver)
{
    //    if (!pmSender)
    //    {
    //        return;
    //    }
    //    std::vector<std::string> commandVector = SplitString(pmCMD, " ", true);
    //    std::string commandName = commandVector.at(0);
    //    if (pmReceiver)
    //    {
    //        if (WorldSession* receiverSession = pmReceiver->GetSession())
    //        {
    //            if (receiverSession->isNinger)
    //            {
    //                if (Awareness_Base* receiverAI = pmReceiver->awarenessMap[pmReceiver->activeAwarenessIndex])
    //                {
    //#pragma region command handling
    //                    if (commandName == "role")
    //                    {
    //                        std::ostringstream replyStream;
    //                        if (commandVector.size() > 1)
    //                        {
    //                            std::string newRole = commandVector.at(1);
    //                            receiverAI->SetGroupRole(newRole);
    //                        }
    //                        replyStream << "My group role is ";
    //                        replyStream << receiverAI->GetGroupRoleName();
    //                        WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
    //                    }
    //                    else if (commandName == "engage")
    //                    {
    //                        receiverAI->staying = false;
    //                        if (Unit* target = pmSender->GetSelectedUnit())
    //                        {
    //                            if (receiverAI->Engage(target))
    //                            {
    //                                if (Group* myGroup = pmReceiver->GetGroup())
    //                                {
    //                                    if (myGroup->GetTargetIconByGuid(target->GetGUID()) == -1)
    //                                    {
    //                                        myGroup->SetTargetIcon(7, pmSender->GetGUID(), target->GetGUID());
    //                                    }
    //                                }
    //                                receiverAI->ogEngageTarget = target->GetGUID();
    //                                int engageDelay = 5000;
    //                                if (commandVector.size() > 1)
    //                                {
    //                                    std::string checkStr = commandVector.at(1);
    //                                    engageDelay = atoi(checkStr.c_str());
    //                                }
    //                                receiverAI->engageDelay = engageDelay;
    //                                std::ostringstream replyStream;
    //                                replyStream << "Try to engage " << target->GetName();
    //                                WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
    //                            }
    //                        }
    //                    }
    //                    else if (commandName == "tank")
    //                    {
    //                        if (Unit* target = pmSender->GetSelectedUnit())
    //                        {
    //                            if (receiverAI->groupRole == GroupRole::GroupRole_Tank)
    //                            {
    //                                if (receiverAI->Tank(target))
    //                                {
    //                                    if (Group* myGroup = pmReceiver->GetGroup())
    //                                    {
    //                                        if (myGroup->GetTargetIconByGuid(target->GetGUID()) == -1)
    //                                        {
    //                                            myGroup->SetTargetIcon(7, pmSender->GetGUID(), target->GetGUID());
    //                                        }
    //                                    }
    //                                    receiverAI->staying = false;
    //                                    receiverAI->ogEngageTarget = target->GetGUID();
    //                                    int engageDelay = 5000;
    //                                    if (commandVector.size() > 1)
    //                                    {
    //                                        std::string checkStr = commandVector.at(1);
    //                                        engageDelay = atoi(checkStr.c_str());
    //                                    }
    //                                    receiverAI->engageDelay = engageDelay;
    //                                    std::ostringstream replyStream;
    //                                    replyStream << "Try to tank " << target->GetName();
    //                                    WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
    //                                }
    //                            }
    //                            else
    //                            {
    //                                receiverAI->staying = false;
    //                            }
    //                        }
    //                    }
    //                    else if (commandName == "revive")
    //                    {
    //                        if (pmReceiver->IsAlive())
    //                        {
    //                            receiverAI->reviveDelay = 2000;
    //                            if (Script_Base* sb = receiverAI->sb)
    //                            {
    //                                if (NingerMovement* rm = sb->rm)
    //                                {
    //                                    rm->ResetMovement();
    //                                }
    //                            }
    //                        }
    //                    }
    //                    else if (commandName == "follow")
    //                    {
    //                        std::ostringstream replyStream;
    //                        bool takeAction = true;
    //                        if (commandVector.size() > 1)
    //                        {
    //                            std::string cmdDistanceStr = commandVector.at(1);
    //                            float cmdDistance = atof(cmdDistanceStr.c_str());
    //                            if (cmdDistance == 0.0f)
    //                            {
    //                                receiverAI->following = false;
    //                                replyStream << "Stop following. ";
    //                                takeAction = false;
    //                            }
    //                            else if (cmdDistance >= FOLLOW_MIN_DISTANCE && cmdDistance <= FOLLOW_MAX_DISTANCE)
    //                            {
    //                                receiverAI->followDistance = cmdDistance;
    //                                replyStream << "Distance updated. ";
    //                            }
    //                            else
    //                            {
    //                                replyStream << "Distance is not valid. ";
    //                                takeAction = false;
    //                            }
    //                        }
    //                        if (takeAction)
    //                        {
    //                            receiverAI->eatDelay = 0;
    //                            receiverAI->drinkDelay = 0;
    //                            receiverAI->staying = false;
    //                            receiverAI->holding = false;
    //                            receiverAI->following = true;
    //                            if (receiverAI->Follow())
    //                            {
    //                                replyStream << "Following " << receiverAI->followDistance;
    //                            }
    //                            else
    //                            {
    //                                replyStream << "can not follow";
    //                            }
    //                        }
    //                        WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
    //                    }
    //                    else if (commandName == "chase")
    //                    {
    //                        std::ostringstream replyStream;
    //                        bool takeAction = true;
    //                        if (commandVector.size() > 2)
    //                        {
    //                            std::string cmdMinDistanceStr = commandVector.at(1);
    //                            float cmdMinDistance = atof(cmdMinDistanceStr.c_str());
    //                            if (cmdMinDistance < MELEE_MIN_DISTANCE)
    //                            {
    //                                cmdMinDistance = MELEE_MIN_DISTANCE;
    //                            }
    //                            std::string cmdMaxDistanceStr = commandVector.at(2);
    //                            float cmdMaxDistance = atof(cmdMaxDistanceStr.c_str());
    //                            if (cmdMaxDistance > RANGE_HEAL_DISTANCE)
    //                            {
    //                                cmdMaxDistance = RANGE_HEAL_DISTANCE;
    //                            }
    //                            else if (cmdMaxDistance < MELEE_MAX_DISTANCE)
    //                            {
    //                                cmdMaxDistance = MELEE_MAX_DISTANCE;
    //                            }
    //                            if (cmdMinDistance > cmdMaxDistance)
    //                            {
    //                                cmdMinDistance = cmdMaxDistance - MELEE_MIN_DISTANCE;
    //                            }
    //                            receiverAI->chaseDistanceMin = cmdMinDistance;
    //                            receiverAI->chaseDistanceMax = cmdMaxDistance;
    //                            replyStream << "Chase distance range updated. " << receiverAI->chaseDistanceMin << " " << receiverAI->chaseDistanceMax;
    //                        }
    //                        else
    //                        {
    //                            replyStream << "Chase distance range is " << receiverAI->chaseDistanceMin << " " << receiverAI->chaseDistanceMax;
    //                        }
    //                        WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
    //                    }
    //                    else if (commandName == "stay")
    //                    {
    //                        std::string targetGroupRole = "";
    //                        if (commandVector.size() > 1)
    //                        {
    //                            targetGroupRole = commandVector.at(1);
    //                        }
    //                        if (receiverAI->Stay(targetGroupRole))
    //                        {
    //                            WhisperTo(pmSender, "Staying", Language::LANG_UNIVERSAL, pmReceiver);
    //                        }
    //                    }
    //                    else if (commandName == "hold")
    //                    {
    //                        std::string targetGroupRole = "";
    //                        if (commandVector.size() > 1)
    //                        {
    //                            targetGroupRole = commandVector.at(1);
    //                        }
    //                        if (receiverAI->Hold(targetGroupRole))
    //                        {
    //                            WhisperTo(pmReceiver, "Holding", Language::LANG_UNIVERSAL, pmSender);
    //                        }
    //                    }
    //                    else if (commandName == "rest")
    //                    {
    //                        std::ostringstream replyStream;
    //                        if (receiverAI->sb->Eat(true))
    //                        {
    //                            receiverAI->eatDelay = DEFAULT_REST_DELAY;
    //                            receiverAI->drinkDelay = 1000;
    //                            replyStream << "Resting";
    //                        }
    //                        else
    //                        {
    //                            replyStream << "Can not rest";
    //                        }
    //                        WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
    //                    }
    //                    else if (commandName == "who")
    //                    {
    //                        if (Awareness_Base* ningerAI = pmReceiver->awarenessMap[pmReceiver->activeAwarenessIndex])
    //                        {
    //                            if (Script_Base* sb = ningerAI->sb)
    //                            {
    //                                WhisperTo(pmSender, characterTalentTabNameMap[pmReceiver->getClass()][sb->maxTalentTab], Language::LANG_UNIVERSAL, pmReceiver);
    //                            }
    //                        }
    //                    }
    //                    else if (commandName == "assemble")
    //                    {
    //                        std::ostringstream replyStream;
    //                        if (receiverAI->teleportAssembleDelay > 0)
    //                        {
    //                            replyStream << "I am on the way";
    //                        }
    //                        else
    //                        {
    //                            if (pmReceiver->IsAlive())
    //                            {
    //                                if (pmReceiver->GetDistance(pmSender) < VISIBILITY_DISTANCE_TINY)
    //                                {
    //                                    receiverAI->teleportAssembleDelay = urand(10 * IN_MILLISECONDS, 15 * IN_MILLISECONDS);
    //                                    replyStream << "We are close, I will teleport to you in " << receiverAI->teleportAssembleDelay / 1000 << " seconds";
    //                                }
    //                                else
    //                                {
    //                                    receiverAI->teleportAssembleDelay = urand(30 * IN_MILLISECONDS, 1 * MINUTE * IN_MILLISECONDS);
    //                                    replyStream << "I will teleport to you in " << receiverAI->teleportAssembleDelay / 1000 << " seconds";
    //                                }
    //                            }
    //                            else
    //                            {
    //                                receiverAI->teleportAssembleDelay = urand(1 * MINUTE * IN_MILLISECONDS, 2 * MINUTE * IN_MILLISECONDS);
    //                                replyStream << "I will teleport to you and revive in " << receiverAI->teleportAssembleDelay / 1000 << " seconds";
    //                            }
    //                        }
    //                        WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
    //                    }
    //                    else if (commandName == "gather")
    //                    {
    //                        std::ostringstream replyStream;
    //                        if (receiverAI->moveDelay > 0)
    //                        {
    //                            replyStream << "I am on the way";
    //                        }
    //                        else
    //                        {
    //                            if (pmReceiver->IsAlive())
    //                            {
    //                                if (pmReceiver->GetDistance(pmSender) < RANGE_HEAL_DISTANCE)
    //                                {
    //                                    if (pmReceiver->IsNonMeleeSpellCast(false))
    //                                    {
    //                                        pmReceiver->InterruptSpell(CurrentSpellTypes::CURRENT_GENERIC_SPELL);
    //                                        pmReceiver->InterruptSpell(CurrentSpellTypes::CURRENT_CHANNELED_SPELL);
    //                                    }
    //                                    pmReceiver->GetMotionMaster()->Initialize();
    //                                    pmReceiver->StopMoving();
    //                                    receiverAI->eatDelay = 0;
    //                                    receiverAI->drinkDelay = 0;
    //                                    int moveDelay = 1000;
    //                                    if (commandVector.size() > 1)
    //                                    {
    //                                        std::string moveDelayStr = commandVector.at(1);
    //                                        moveDelay = atoi(moveDelayStr.c_str());
    //                                        if (moveDelay < 1000 || moveDelay > 6000)
    //                                        {
    //                                            moveDelay = 1000;
    //                                        }
    //                                    }
    //                                    receiverAI->moveDelay = moveDelay;
    //                                    receiverAI->sb->rm->MovePoint(pmSender->GetPosition(), moveDelay);
    //                                    replyStream << "I will move to you";
    //                                }
    //                                else
    //                                {
    //                                    replyStream << "too far away";
    //                                }
    //                            }
    //                            else
    //                            {
    //                                replyStream << "I am dead ";
    //                            }
    //                        }
    //                        WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
    //                    }
    //                    else if (commandName == "cast")
    //                    {
    //                        std::ostringstream replyStream;
    //                        if (pmReceiver->IsAlive())
    //                        {
    //                            if (commandVector.size() > 1)
    //                            {
    //                                std::ostringstream targetStream;
    //                                uint8 arrayCount = 0;
    //                                for (std::vector<std::string>::iterator it = commandVector.begin(); it != commandVector.end(); it++)
    //                                {
    //                                    if (arrayCount > 0)
    //                                    {
    //                                        targetStream << (*it) << " ";
    //                                    }
    //                                    arrayCount++;
    //                                }
    //                                std::string spellName = TrimString(targetStream.str());
    //                                Unit* senderTarget = pmSender->GetSelectedUnit();
    //                                if (!senderTarget)
    //                                {
    //                                    senderTarget = pmReceiver;
    //                                }
    //                                if (receiverAI->sb->CastSpell(senderTarget, spellName, VISIBILITY_DISTANCE_NORMAL))
    //                                {
    //                                    replyStream << "Cast spell " << spellName << " on " << senderTarget->GetName();
    //                                }
    //                                else
    //                                {
    //                                    replyStream << "Can not cast spell " << spellName << " on " << senderTarget->GetName();
    //                                }
    //                            }
    //                        }
    //                        else
    //                        {
    //                            replyStream << "I am dead";
    //                        }
    //                        WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
    //                    }
    //                    else if (commandName == "cancel")
    //                    {
    //                        std::ostringstream replyStream;
    //                        if (pmReceiver->IsAlive())
    //                        {
    //                            if (commandVector.size() > 1)
    //                            {
    //                                std::ostringstream targetStream;
    //                                uint8 arrayCount = 0;
    //                                for (std::vector<std::string>::iterator it = commandVector.begin(); it != commandVector.end(); it++)
    //                                {
    //                                    if (arrayCount > 0)
    //                                    {
    //                                        targetStream << (*it) << " ";
    //                                    }
    //                                    arrayCount++;
    //                                }
    //                                std::string spellName = TrimString(targetStream.str());
    //                                if (receiverAI->sb->CancelAura(spellName))
    //                                {
    //                                    replyStream << "Aura canceled " << spellName;
    //                                }
    //                                else
    //                                {
    //                                    replyStream << "Can not cancel aura " << spellName;
    //                                }
    //                            }
    //                        }
    //                        else
    //                        {
    //                            replyStream << "I am dead";
    //                        }
    //                        WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
    //                    }
    //                    else if (commandName == "use")
    //                    {
    //                        std::ostringstream replyStream;
    //                        if (pmReceiver->IsAlive())
    //                        {
    //                            if (commandVector.size() > 1)
    //                            {
    //                                std::string useType = commandVector.at(1);
    //                                if (useType == "go")
    //                                {
    //                                    if (commandVector.size() > 2)
    //                                    {
    //                                        std::ostringstream goNameStream;
    //                                        uint32 checkPos = 2;
    //                                        while (checkPos < commandVector.size())
    //                                        {
    //                                            goNameStream << commandVector.at(checkPos) << " ";
    //                                            checkPos++;
    //                                        }
    //                                        std::string goName = TrimString(goNameStream.str());
    //                                        bool validToUse = false;
    //                                        std::list<GameObject*> nearGOList;
    //                                        pmReceiver->GetGameObjectListWithEntryInGrid(nearGOList, 0, MELEE_MAX_DISTANCE);
    //                                        for (std::list<GameObject*>::iterator it = nearGOList.begin(); it != nearGOList.end(); it++)
    //                                        {
    //                                            if ((*it)->GetName() == goName)
    //                                            {
    //                                                pmReceiver->SetFacingToObject((*it));
    //                                                pmReceiver->StopMoving();
    //                                                pmReceiver->GetMotionMaster()->Initialize();
    //                                                (*it)->Use(pmReceiver);
    //                                                replyStream << "Use game object : " << goName;
    //                                                validToUse = true;
    //                                                break;
    //                                            }
    //                                        }
    //                                        if (!validToUse)
    //                                        {
    //                                            replyStream << "No go with name " << goName << " nearby";
    //                                        }
    //                                    }
    //                                    else
    //                                    {
    //                                        replyStream << "No go name";
    //                                    }
    //                                }
    //                                else if (useType == "item")
    //                                {
    //
    //                                }
    //                                else
    //                                {
    //                                    replyStream << "Unknown type";
    //                                }
    //                            }
    //                            else
    //                            {
    //                                replyStream << "Use what?";
    //                            }
    //                        }
    //                        else
    //                        {
    //                            replyStream << "I am dead";
    //                        }
    //                        WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
    //                    }
    //                    else if (commandName == "stop")
    //                    {
    //                        std::ostringstream replyStream;
    //                        if (pmReceiver->IsAlive())
    //                        {
    //                            pmReceiver->StopMoving();
    //                            pmReceiver->InterruptSpell(CurrentSpellTypes::CURRENT_AUTOREPEAT_SPELL);
    //                            pmReceiver->InterruptSpell(CurrentSpellTypes::CURRENT_CHANNELED_SPELL);
    //                            pmReceiver->InterruptSpell(CurrentSpellTypes::CURRENT_GENERIC_SPELL);
    //                            pmReceiver->InterruptSpell(CurrentSpellTypes::CURRENT_MELEE_SPELL);
    //                            pmReceiver->AttackStop();
    //                            if (Script_Base* sb = receiverAI->sb)
    //                            {
    //                                sb->PetStop();
    //                                sb->ClearTarget();
    //                            }
    //                            receiverAI->moveDelay = 2000;
    //                            replyStream << "Stopped";
    //                        }
    //                        else
    //                        {
    //                            replyStream << "I am dead";
    //                        }
    //                        WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
    //                    }
    //                    else if (commandName == "delay")
    //                    {
    //                        std::ostringstream replyStream;
    //                        if (commandVector.size() > 1)
    //                        {
    //                            int delayMS = std::stoi(commandVector.at(1));
    //                            receiverAI->dpsDelay = delayMS;
    //                            replyStream << "DPS delay set to : " << delayMS;
    //                        }
    //                        else
    //                        {
    //                            replyStream << "DPS delay is : " << receiverAI->dpsDelay;
    //                        }
    //                        WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
    //                    }
    //                    else if (commandName == "attackers")
    //                    {
    //                        std::ostringstream replyStream;
    //                        if (pmReceiver->IsAlive())
    //                        {
    //                            replyStream << "attackers list : ";
    //                            std::unordered_set<Unit*> attackers = pmReceiver->getAttackers();
    //                            for (std::unordered_set<Unit*>::iterator aIT = attackers.begin(); aIT != attackers.end(); aIT++)
    //                            {
    //                                if (Unit* eachAttacker = *aIT)
    //                                {
    //                                    replyStream << eachAttacker->GetName() << ", ";
    //                                }
    //                            }
    //                        }
    //                        else
    //                        {
    //                            replyStream << "I am dead";
    //                        }
    //                        WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
    //                    }
    //                    else if (commandName == "cure")
    //                    {
    //                        std::ostringstream replyStream;
    //                        if (commandVector.size() > 1)
    //                        {
    //                            std::string cureCMD = commandVector.at(1);
    //                            if (cureCMD == "on")
    //                            {
    //                                receiverAI->cure = true;
    //                            }
    //                            else if (cureCMD == "off")
    //                            {
    //                                receiverAI->cure = false;
    //                            }
    //                            else
    //                            {
    //                                replyStream << "Unknown state";
    //                            }
    //                        }
    //                        if (receiverAI->cure)
    //                        {
    //                            replyStream << "auto cure is on";
    //                        }
    //                        else
    //                        {
    //                            replyStream << "auto cure is off";
    //                        }
    //                        WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
    //                    }
    //                    else if (commandName == "petting")
    //                    {
    //                        std::ostringstream replyStream;
    //                        if (commandVector.size() > 1)
    //                        {
    //                            std::string cureCMD = commandVector.at(1);
    //                            if (cureCMD == "on")
    //                            {
    //                                receiverAI->petting = true;
    //                            }
    //                            else if (cureCMD == "off")
    //                            {
    //                                receiverAI->petting = false;
    //                            }
    //                            else
    //                            {
    //                                replyStream << "Unknown state";
    //                            }
    //                        }
    //                        if (receiverAI->petting)
    //                        {
    //                            replyStream << "petting is on";
    //                        }
    //                        else
    //                        {
    //                            replyStream << "petting is off";
    //                        }
    //                        WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
    //                    }
    //                    else if (commandName == "aoe")
    //                    {
    //                        std::ostringstream replyStream;
    //                        if (commandVector.size() > 1)
    //                        {
    //                            std::string on = commandVector.at(1);
    //                            if (on == "on")
    //                            {
    //                                receiverAI->aoe = true;
    //                            }
    //                            else if (on == "off")
    //                            {
    //                                receiverAI->aoe = false;
    //                            }
    //                        }
    //                        if (receiverAI->aoe)
    //                        {
    //                            replyStream << "AOE is on";
    //                        }
    //                        else
    //                        {
    //                            replyStream << "AOE is off";
    //                        }
    //                        WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
    //                    }
    //                    else if (commandName == "mark")
    //                    {
    //                        std::ostringstream replyStream;
    //                        if (commandVector.size() > 1)
    //                        {
    //                            std::string markCMD = commandVector.at(1);
    //                            if (markCMD == "on")
    //                            {
    //                                receiverAI->mark = true;
    //                            }
    //                            else if (markCMD == "off")
    //                            {
    //                                receiverAI->mark = false;
    //                            }
    //                            else
    //                            {
    //                                replyStream << "Unknown state";
    //                            }
    //                        }
    //                        if (receiverAI->mark)
    //                        {
    //                            replyStream << "Mark is on";
    //                        }
    //                        else
    //                        {
    //                            replyStream << "Mark is off";
    //                        }
    //                        WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
    //                    }
    //                    else if (commandName == "emote")
    //                    {
    //                        if (pmReceiver->IsAlive())
    //                        {
    //                            if (commandVector.size() > 1)
    //                            {
    //                                int emoteCMD = std::stoi(commandVector.at(1));
    //                                pmReceiver->HandleEmoteCommand((Emote)emoteCMD);
    //                            }
    //                            else
    //                            {
    //                                pmReceiver->AttackStop();
    //                                pmReceiver->CombatStop();
    //                            }
    //                        }
    //                        else
    //                        {
    //                            WhisperTo(pmSender, "I am dead", Language::LANG_UNIVERSAL, pmReceiver);
    //                        }
    //                    }
    //                    else if (commandName == "pa")
    //                    {
    //                        if (pmReceiver->getClass() == Classes::CLASS_PALADIN)
    //                        {
    //                            std::ostringstream replyStream;
    //                            if (Script_Paladin* sp = (Script_Paladin*)receiverAI->sb)
    //                            {
    //                                if (commandVector.size() > 1)
    //                                {
    //                                    std::string auratypeName = commandVector.at(1);
    //                                    if (auratypeName == "concentration")
    //                                    {
    //                                        sp->auraType = PaladinAuraType::PaladinAuraType_Concentration;
    //                                    }
    //                                    else if (auratypeName == "devotion")
    //                                    {
    //                                        sp->auraType = PaladinAuraType::PaladinAuraType_Devotion;
    //                                    }
    //                                    else if (auratypeName == "retribution")
    //                                    {
    //                                        sp->auraType = PaladinAuraType::PaladinAuraType_Retribution;
    //                                    }
    //                                    else if (auratypeName == "fire")
    //                                    {
    //                                        sp->auraType = PaladinAuraType::PaladinAuraType_FireResistant;
    //                                    }
    //                                    else if (auratypeName == "frost")
    //                                    {
    //                                        sp->auraType = PaladinAuraType::PaladinAuraType_FrostResistant;
    //                                    }
    //                                    else if (auratypeName == "shadow")
    //                                    {
    //                                        sp->auraType = PaladinAuraType::PaladinAuraType_ShadowResistant;
    //                                    }
    //                                    else
    //                                    {
    //                                        replyStream << "Unknown type";
    //                                    }
    //                                }
    //                                switch (sp->auraType)
    //                                {
    //                                case PaladinAuraType::PaladinAuraType_Concentration:
    //                                {
    //                                    replyStream << "concentration";
    //                                    break;
    //                                }
    //                                case PaladinAuraType::PaladinAuraType_Devotion:
    //                                {
    //                                    replyStream << "devotion";
    //                                    break;
    //                                }
    //                                case PaladinAuraType::PaladinAuraType_Retribution:
    //                                {
    //                                    replyStream << "retribution";
    //                                    break;
    //                                }
    //                                case PaladinAuraType::PaladinAuraType_FireResistant:
    //                                {
    //                                    replyStream << "fire";
    //                                    break;
    //                                }
    //                                case PaladinAuraType::PaladinAuraType_FrostResistant:
    //                                {
    //                                    replyStream << "frost";
    //                                    break;
    //                                }
    //                                case PaladinAuraType::PaladinAuraType_ShadowResistant:
    //                                {
    //                                    replyStream << "shadow";
    //                                    break;
    //                                }
    //                                default:
    //                                {
    //                                    break;
    //                                }
    //                                }
    //                            }
    //                            WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
    //                        }
    //                    }
    //                    else if (commandName == "pb")
    //                    {
    //                        if (pmReceiver->getClass() == Classes::CLASS_PALADIN)
    //                        {
    //                            std::ostringstream replyStream;
    //                            if (Script_Paladin* sp = (Script_Paladin*)receiverAI->sb)
    //                            {
    //                                if (commandVector.size() > 1)
    //                                {
    //                                    std::string blessingTypeName = commandVector.at(1);
    //                                    if (blessingTypeName == "kings")
    //                                    {
    //                                        sp->blessingType = PaladinBlessingType::PaladinBlessingType_Kings;
    //                                    }
    //                                    else if (blessingTypeName == "might")
    //                                    {
    //                                        sp->blessingType = PaladinBlessingType::PaladinBlessingType_Might;
    //                                    }
    //                                    else if (blessingTypeName == "wisdom")
    //                                    {
    //                                        sp->blessingType = PaladinBlessingType::PaladinBlessingType_Wisdom;
    //                                    }
    //                                    else if (blessingTypeName == "salvation")
    //                                    {
    //                                        sp->blessingType = PaladinBlessingType::PaladinBlessingType_Salvation;
    //                                    }
    //                                    else
    //                                    {
    //                                        replyStream << "Unknown type";
    //                                    }
    //                                }
    //                                switch (sp->blessingType)
    //                                {
    //                                case PaladinBlessingType::PaladinBlessingType_Kings:
    //                                {
    //                                    replyStream << "kings";
    //                                    break;
    //                                }
    //                                case PaladinBlessingType::PaladinBlessingType_Might:
    //                                {
    //                                    replyStream << "might";
    //                                    break;
    //                                }
    //                                case PaladinBlessingType::PaladinBlessingType_Wisdom:
    //                                {
    //                                    replyStream << "wisdom";
    //                                    break;
    //                                }
    //                                case PaladinBlessingType::PaladinBlessingType_Salvation:
    //                                {
    //                                    replyStream << "salvation";
    //                                    break;
    //                                }
    //                                default:
    //                                {
    //                                    break;
    //                                }
    //                                }
    //                            }
    //                            WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
    //                        }
    //                    }
    //                    else if (commandName == "ps")
    //                    {
    //                        if (pmReceiver->getClass() == Classes::CLASS_PALADIN)
    //                        {
    //                            std::ostringstream replyStream;
    //                            if (Script_Paladin* sp = (Script_Paladin*)receiverAI->sb)
    //                            {
    //                                if (commandVector.size() > 1)
    //                                {
    //                                    std::string sealTypeName = commandVector.at(1);
    //                                    if (sealTypeName == "righteousness")
    //                                    {
    //                                        sp->sealType = PaladinSealType::PaladinSealType_Righteousness;
    //                                    }
    //                                    else if (sealTypeName == "justice")
    //                                    {
    //                                        sp->sealType = PaladinSealType::PaladinSealType_Justice;
    //                                    }
    //                                    else if (sealTypeName == "command")
    //                                    {
    //                                        sp->sealType = PaladinSealType::PaladinSealType_Command;
    //                                    }
    //                                    else
    //                                    {
    //                                        replyStream << "Unknown type";
    //                                    }
    //                                }
    //                                switch (sp->sealType)
    //                                {
    //                                case PaladinSealType::PaladinSealType_Righteousness:
    //                                {
    //                                    replyStream << "righteousness";
    //                                    break;
    //                                }
    //                                case PaladinSealType::PaladinSealType_Justice:
    //                                {
    //                                    replyStream << "justice";
    //                                    break;
    //                                }
    //                                case PaladinSealType::PaladinSealType_Command:
    //                                {
    //                                    replyStream << "command";
    //                                    break;
    //                                }
    //                                default:
    //                                {
    //                                    break;
    //                                }
    //                                }
    //                            }
    //                            WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
    //                        }
    //                    }
    //                    else if (commandName == "ha")
    //                    {
    //                        if (pmReceiver->getClass() == Classes::CLASS_HUNTER)
    //                        {
    //                            std::ostringstream replyStream;
    //                            if (Script_Hunter* sh = (Script_Hunter*)receiverAI->sb)
    //                            {
    //                                if (commandVector.size() > 1)
    //                                {
    //                                    std::string aspectName = commandVector.at(1);
    //                                    if (aspectName == "hawk")
    //                                    {
    //                                        sh->aspectType = HunterAspectType::HunterAspectType_Hawk;
    //                                    }
    //                                    else if (aspectName == "monkey")
    //                                    {
    //                                        sh->aspectType = HunterAspectType::HunterAspectType_Monkey;
    //                                    }
    //                                    else if (aspectName == "wild")
    //                                    {
    //                                        sh->aspectType = HunterAspectType::HunterAspectType_Wild;
    //                                    }
    //                                    else if (aspectName == "pack")
    //                                    {
    //                                        sh->aspectType = HunterAspectType::HunterAspectType_Pack;
    //                                    }
    //                                    else
    //                                    {
    //                                        replyStream << "Unknown type";
    //                                    }
    //                                }
    //                                switch (sh->aspectType)
    //                                {
    //                                case HunterAspectType::HunterAspectType_Hawk:
    //                                {
    //                                    replyStream << "hawk";
    //                                    break;
    //                                }
    //                                case HunterAspectType::HunterAspectType_Monkey:
    //                                {
    //                                    replyStream << "monkey";
    //                                    break;
    //                                }
    //                                case HunterAspectType::HunterAspectType_Wild:
    //                                {
    //                                    replyStream << "wild";
    //                                    break;
    //                                }
    //                                case HunterAspectType::HunterAspectType_Pack:
    //                                {
    //                                    replyStream << "pack";
    //                                    break;
    //                                }
    //                                default:
    //                                {
    //                                    break;
    //                                }
    //                                }
    //                            }
    //                            WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
    //                        }
    //                    }
    //                    else if (commandName == "equip")
    //                    {
    //                        if (commandVector.size() > 1)
    //                        {
    //                            std::string equipType = commandVector.at(1);
    //                            if (equipType == "molten core")
    //                            {
    //                                if (pmReceiver->getClass() == Classes::CLASS_DRUID)
    //                                {
    //                                    for (uint32 checkEquipSlot = EquipmentSlots::EQUIPMENT_SLOT_HEAD; checkEquipSlot < EquipmentSlots::EQUIPMENT_SLOT_TABARD; checkEquipSlot++)
    //                                    {
    //                                        if (Item* currentEquip = pmReceiver->GetItemByPos(INVENTORY_SLOT_BAG_0, checkEquipSlot))
    //                                        {
    //                                            pmReceiver->DestroyItem(INVENTORY_SLOT_BAG_0, checkEquipSlot, true);
    //                                        }
    //                                    }
    //                                    EquipNewItem(pmReceiver, 16983, EquipmentSlots::EQUIPMENT_SLOT_HEAD);
    //                                    EquipNewItem(pmReceiver, 19139, EquipmentSlots::EQUIPMENT_SLOT_SHOULDERS);
    //                                    EquipNewItem(pmReceiver, 16833, EquipmentSlots::EQUIPMENT_SLOT_CHEST);
    //                                    EquipNewItem(pmReceiver, 11764, EquipmentSlots::EQUIPMENT_SLOT_WRISTS);
    //                                    EquipNewItem(pmReceiver, 16831, EquipmentSlots::EQUIPMENT_SLOT_HANDS);
    //                                    EquipNewItem(pmReceiver, 19149, EquipmentSlots::EQUIPMENT_SLOT_WAIST);
    //                                    EquipNewItem(pmReceiver, 15054, EquipmentSlots::EQUIPMENT_SLOT_LEGS);
    //                                    EquipNewItem(pmReceiver, 16982, EquipmentSlots::EQUIPMENT_SLOT_FEET);
    //                                    EquipNewItem(pmReceiver, 18803, EquipmentSlots::EQUIPMENT_SLOT_MAINHAND);
    //                                    EquipNewItem(pmReceiver, 2802, EquipmentSlots::EQUIPMENT_SLOT_TRINKET1);
    //                                    EquipNewItem(pmReceiver, 18406, EquipmentSlots::EQUIPMENT_SLOT_TRINKET2);
    //                                    EquipNewItem(pmReceiver, 18398, EquipmentSlots::EQUIPMENT_SLOT_FINGER1);
    //                                    EquipNewItem(pmReceiver, 18813, EquipmentSlots::EQUIPMENT_SLOT_FINGER2);
    //                                    EquipNewItem(pmReceiver, 18811, EquipmentSlots::EQUIPMENT_SLOT_BACK);
    //                                    EquipNewItem(pmReceiver, 16309, EquipmentSlots::EQUIPMENT_SLOT_NECK);
    //                                    std::ostringstream replyStream;
    //                                    replyStream << "Equip all fire resistance gears.";
    //                                    WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
    //                                }
    //                            }
    //                            else if (equipType == "reset")
    //                            {
    //                                InitializeEquipments(pmReceiver, true);
    //                                std::ostringstream replyStream;
    //                                replyStream << "All my equipments are reset.";
    //                                WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
    //                            }
    //                        }
    //                    }
    //                    else if (commandName == "rti")
    //                    {
    //                        int targetIcon = -1;
    //                        if (commandVector.size() > 1)
    //                        {
    //                            std::string iconIndex = commandVector.at(1);
    //                            targetIcon = atoi(iconIndex.c_str());
    //                        }
    //                        if (targetIcon >= 0 && targetIcon < TARGETICONCOUNT)
    //                        {
    //                            receiverAI->sb->rti = targetIcon;
    //                        }
    //                        std::ostringstream replyStream;
    //                        replyStream << "RTI is " << receiverAI->sb->rti;
    //                        WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
    //                    }
    //                    else if (commandName == "assist")
    //                    {
    //                        if (receiverAI->sb->Assist(nullptr))
    //                        {
    //                            receiverAI->assistDelay = 5000;
    //                            std::ostringstream replyStream;
    //                            replyStream << "Try to pin down my RTI : " << receiverAI->sb->rti;
    //                            WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
    //                        }
    //                    }
    //                    else if (commandName == "prepare")
    //                    {
    //                        sNingerManager->PrepareNinger(pmReceiver);
    //                    }
    //                    else if (commandName == "wlc")
    //                    {
    //                        if (pmReceiver->getClass() == Classes::CLASS_WARLOCK)
    //                        {
    //                            std::ostringstream replyStream;
    //                            if (Script_Warlock* swl = (Script_Warlock*)receiverAI->sb)
    //                            {
    //                                if (commandVector.size() > 1)
    //                                {
    //                                    std::string curseName = commandVector.at(1);
    //                                    if (curseName == "none")
    //                                    {
    //                                        swl->curseType = WarlockCurseType::WarlockCurseType_None;
    //                                    }
    //                                    else if (curseName == "element")
    //                                    {
    //                                        swl->curseType = WarlockCurseType::WarlockCurseType_Element;
    //                                    }
    //                                    else if (curseName == "weakness")
    //                                    {
    //                                        swl->curseType = WarlockCurseType::WarlockCurseType_Weakness;
    //                                    }
    //                                    else if (curseName == "tongues")
    //                                    {
    //                                        swl->curseType = WarlockCurseType::WarlockCurseType_Tongues;
    //                                    }
    //                                    else
    //                                    {
    //                                        replyStream << "Unknown type";
    //                                    }
    //                                }
    //                                switch (swl->curseType)
    //                                {
    //                                case WarlockCurseType::WarlockCurseType_None:
    //                                {
    //                                    replyStream << "none";
    //                                    break;
    //                                }
    //                                case WarlockCurseType::WarlockCurseType_Element:
    //                                {
    //                                    replyStream << "element";
    //                                    break;
    //                                }
    //                                case WarlockCurseType::WarlockCurseType_Weakness:
    //                                {
    //                                    replyStream << "weakness";
    //                                    break;
    //                                }
    //                                case WarlockCurseType::WarlockCurseType_Tongues:
    //                                {
    //                                    replyStream << "tongues";
    //                                    break;
    //                                }
    //                                default:
    //                                {
    //                                    break;
    //                                }
    //                                }
    //                            }
    //                            WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
    //                        }
    //                    }
    //#pragma endregion
    //                }
    //            }
    //        }
    //    }
    //    else
    //    {
    //        if (Group* myGroup = pmSender->GetGroup())
    //        {
    //            if (commandName == "revive")
    //            {
    //                std::unordered_set<ObjectGuid> deadOGSet;
    //                for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
    //                {
    //                    if (Player* member = groupRef->GetSource())
    //                    {
    //                        if (!member->IsAlive())
    //                        {
    //                            deadOGSet.insert(member->GetGUID());
    //                        }
    //                    }
    //                }
    //                std::unordered_set<ObjectGuid> revivingOGSet;
    //                for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
    //                {
    //                    if (Player* member = groupRef->GetSource())
    //                    {
    //                        if (member->IsAlive())
    //                        {
    //                            if (!member->IsInCombat())
    //                            {
    //                                if (member->getClass() == Classes::CLASS_DRUID || member->getClass() == Classes::CLASS_PRIEST || member->getClass() == Classes::CLASS_PALADIN || member->getClass() == Classes::CLASS_SHAMAN)
    //                                {
    //                                    if (WorldSession* memberSession = member->GetSession())
    //                                    {
    //                                        if (memberSession->isNinger)
    //                                        {
    //                                            float manaRate = member->GetPower(Powers::POWER_MANA) * 100 / member->GetMaxPower(Powers::POWER_MANA);
    //                                            if (manaRate > 40)
    //                                            {
    //                                                for (std::unordered_set<ObjectGuid>::iterator dIT = deadOGSet.begin(); dIT != deadOGSet.end(); dIT++)
    //                                                {
    //                                                    if (ObjectGuid ogEachDead = *dIT)
    //                                                    {
    //                                                        if (revivingOGSet.find(ogEachDead) == revivingOGSet.end())
    //                                                        {
    //                                                            if (Player* eachDead = ObjectAccessor::FindPlayer(ogEachDead))
    //                                                            {
    //                                                                if (member->GetDistance(eachDead) < RANGE_HEAL_DISTANCE)
    //                                                                {
    //                                                                    if (Awareness_Base* memberAwareness = member->awarenessMap[member->activeAwarenessIndex])
    //                                                                    {
    //                                                                        if (Script_Base* sb = memberAwareness->sb)
    //                                                                        {
    //                                                                            sb->ogReviveTarget = eachDead->GetGUID();
    //                                                                            HandleChatCommand(pmSender, pmCMD, member);
    //                                                                            revivingOGSet.insert(eachDead->GetGUID());
    //                                                                            std::ostringstream replyStream;
    //                                                                            replyStream << "Try to revive ";
    //                                                                            replyStream << eachDead->GetName();
    //                                                                            WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, member);
    //                                                                            break;
    //                                                                        }
    //                                                                    }
    //                                                                }
    //                                                            }
    //                                                        }
    //                                                    }
    //                                                }
    //                                            }
    //                                        }
    //                                    }
    //                                }
    //                            }
    //                        }
    //                    }
    //                }
    //            }
    //            else
    //            {
    //                for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
    //                {
    //                    if (Player* member = groupRef->GetSource())
    //                    {
    //                        HandleChatCommand(pmSender, pmCMD, member);
    //                    }
    //                }
    //            }
    //        }
    //    }
}

bool NingerManager::StringEndWith(const std::string& str, const std::string& tail)
{
    return str.compare(str.size() - tail.size(), tail.size(), tail) == 0;
}

bool NingerManager::StringStartWith(const std::string& str, const std::string& head)
{
    return str.compare(0, head.size(), head) == 0;
}

std::vector<std::string> NingerManager::SplitString(std::string srcStr, std::string delimStr, bool repeatedCharIgnored)
{
    std::vector<std::string> resultStringVector;
    std::replace_if(srcStr.begin(), srcStr.end(), [&](const char& c) {if (delimStr.find(c) != std::string::npos) { return true; } else { return false; }}, delimStr.at(0));
    size_t pos = srcStr.find(delimStr.at(0));
    std::string addedString = "";
    while (pos != std::string::npos) {
        addedString = srcStr.substr(0, pos);
        if (!addedString.empty() || !repeatedCharIgnored) {
            resultStringVector.push_back(addedString);
        }
        srcStr.erase(srcStr.begin(), srcStr.begin() + pos + 1);
        pos = srcStr.find(delimStr.at(0));
    }
    addedString = srcStr;
    if (!addedString.empty() || !repeatedCharIgnored) {
        resultStringVector.push_back(addedString);
    }
    return resultStringVector;
}

std::string NingerManager::TrimString(std::string srcStr)
{
    std::string result = srcStr;
    if (!result.empty())
    {
        result.erase(0, result.find_first_not_of(" "));
        result.erase(result.find_last_not_of(" ") + 1);
    }

    return result;
}
