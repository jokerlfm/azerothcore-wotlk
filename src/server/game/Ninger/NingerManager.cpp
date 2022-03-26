#include "NingerManager.h"
#include "AccountMgr.h"
#include "NingerStrategy_Base.h"
#include "NingerConfig.h"
#include "Group.h"
#include "Chat.h"
#include "SpellMgr.h"
#include "SpellAuras.h"
#include "MotionMaster.h"
#include "Map.h"
#include "Pet.h"
#include "GridNotifiers.h"
#include "NingerAction_Base.h"
#include "NingerAction_Druid.h"
#include "NingerAction_Hunter.h"
#include "NingerAction_Mage.h"
#include "NingerAction_Paladin.h"
#include "NingerAction_Priest.h"
#include "NingerAction_Rogue.h"
#include "NingerAction_Shaman.h"
#include "NingerAction_Warlock.h"
#include "NingerAction_Warrior.h"

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
            LoginNinger(eachLevel, sNingerConfig->NingerCountEachLevel);
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

void NingerManager::LogoutNingers()
{
    for (std::unordered_set<NingerEntity*>::iterator reIT = ningerEntitySet.begin(); reIT != ningerEntitySet.end(); reIT++)
    {
        if (NingerEntity* eachNinger = *reIT)
        {
            eachNinger->entityState = NingerEntityState::NingerEntityState_DoLogoff;
            eachNinger->checkDelay = 10;
        }
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

bool NingerManager::LoginNinger(uint32 pmLevel, uint32 pmCount)
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
        if (currentCount < pmCount)
        {
            uint32 addCount = pmCount - currentCount;
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
        if (pmCount > onlineCount)
        {
            toOnline = pmCount - onlineCount;
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
        uint32 target_class = Classes::CLASS_HUNTER;
        uint32 target_specialty = 1;
        uint32 classRand = urand(1, 100);
        if (classRand < 25)
        {
            target_class = Classes::CLASS_PRIEST;
            target_specialty = 0;
        }
        else if (classRand < 50)
        {
            target_class = Classes::CLASS_WARRIOR;
            target_specialty = 2;
        }
        else if (classRand < 75)
        {
            target_class = Classes::CLASS_ROGUE;
            target_specialty = 1;
        }
        else
        {
            target_class = Classes::CLASS_HUNTER;
            target_specialty = 1;
        }

        // lfm debug
        //target_class = Classes::CLASS_ROGUE;
        //target_specialty = 1;

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

void NingerManager::HandleChatCommand(Player* pmPlayer, std::string pmContent, Player* pmTargetPlayer, Group* pmTargetGroup)
{
    if (!pmPlayer)
    {
        return;
    }
    std::vector<std::string> commandVector = SplitString(pmContent, " ", true);
    std::string commandName = commandVector.at(0);
    if (commandName == "role")
    {
        if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmPlayer->GetGUID())
                    {
                        HandleChatCommand(pmPlayer, pmContent, member);
                    }
                }
            }
        }
        else
        {
            Player* targetPlayer = pmTargetPlayer;
            if (!targetPlayer)
            {
                targetPlayer = pmPlayer;
            }
            std::ostringstream replyStream;
            if (commandVector.size() > 1)
            {
                std::string newRole = commandVector.at(1);
                if (newRole == "tank")
                {
                    targetPlayer->groupRole = GroupRole::GroupRole_Tank;
                }
                else if (newRole == "healer")
                {
                    targetPlayer->groupRole = GroupRole::GroupRole_Healer;
                }
                else if (newRole == "dps")
                {
                    targetPlayer->groupRole = GroupRole::GroupRole_DPS;
                }
            }
            if (targetPlayer->groupRole == GroupRole::GroupRole_Tank)
            {
                replyStream << "Role is tank";
            }
            else if (targetPlayer->groupRole == GroupRole::GroupRole_Healer)
            {
                replyStream << "Role is healer";
            }
            else if (targetPlayer->groupRole == GroupRole::GroupRole_DPS)
            {
                replyStream << "Role is dps";
            }
            if (pmTargetPlayer)
            {
                pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmPlayer);
            }
            else
            {
                sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, replyStream.str().c_str(), pmPlayer);
            }
        }
    }
    else if (commandName == "who")
    {
        if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmPlayer->GetGUID())
                    {
                        HandleChatCommand(pmPlayer, pmContent, member);
                    }
                }
            }
        }
        else if (pmTargetPlayer)
        {
            std::ostringstream replyStream;
            replyStream << characterTalentTabNameMap[pmTargetPlayer->getClass()][pmTargetPlayer->ningerAction->specialty];
            pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmPlayer);
        }
    }
    else if (commandName == "arrangement")
    {
        std::ostringstream replyStream;
        if (pmTargetGroup)
        {
            if (pmTargetGroup->GetLeaderGUID() == pmPlayer->GetGUID())
            {
                bool paladinAura_concentration = false;
                bool paladinAura_devotion = false;
                bool paladinAura_retribution = false;
                bool paladinAura_fire = false;
                bool paladinAura_frost = false;
                bool paladinAura_shadow = false;

                bool paladinBlessing_kings = false;
                bool paladinBlessing_might = false;
                bool paladinBlessing_wisdom = false;
                bool paladinBlessing_salvation = false;

                bool paladinSeal_Justice = false;

                bool warlockCurse_Weakness = false;
                bool warlockCurse_Tongues = false;
                bool warlockCurse_Element = false;

                int rtiIndex = 0;

                bool hunterAspect_wild = false;

                for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
                {
                    if (Player* member = groupRef->GetSource())
                    {
                        //if (member->GetMapId() == 619)
                        //{
                        //    member->activeStrategyIndex = 619;
                        //}
                        //else if (member->GetMapId() == 555)
                        //{
                        //    member->activeStrategyIndex = 555;
                        //}
                        //else if (member->GetMapId() == 585)
                        //{
                        //    member->activeStrategyIndex = 585;
                        //}
                        //else
                        //{
                        //    member->activeStrategyIndex = 0;
                        //}
                        if (member->GetGUID() == pmPlayer->GetGUID())
                        {
                            continue;
                        }
                        if (NingerStrategy_Base* ns = member->strategyMap[member->activeStrategyIndex])
                        {
                            ns->Reset();
                        }
                        if (NingerAction_Base* na = member->ningerAction)
                        {
                            switch (member->getClass())
                            {
                            case Classes::CLASS_WARRIOR:
                            {
                                member->groupRole = GroupRole::GroupRole_Tank;
                                break;
                            }
                            case Classes::CLASS_SHAMAN:
                            {
                                member->groupRole = GroupRole::GroupRole_DPS;
                                break;
                            }
                            case Classes::CLASS_PALADIN:
                            {
                                member->groupRole = GroupRole::GroupRole_DPS;
                                break;
                            }
                            case Classes::CLASS_PRIEST:
                            {
                                member->groupRole = GroupRole::GroupRole_Healer;
                                break;
                            }
                            case Classes::CLASS_DRUID:
                            {
                                member->groupRole = GroupRole::GroupRole_DPS;
                                break;
                            }
                            default:
                            {
                                member->groupRole = GroupRole::GroupRole_DPS;
                                break;
                            }
                            }
                            if (member->getClass() == Classes::CLASS_PALADIN)
                            {
                                if (NingerAction_Paladin* nap = (NingerAction_Paladin*)na)
                                {
                                    if (!paladinBlessing_salvation)
                                    {
                                        nap->blessingType = PaladinBlessingType::PaladinBlessingType_Salvation;
                                        paladinBlessing_salvation = true;
                                    }
                                    else if (!paladinBlessing_might)
                                    {
                                        nap->blessingType = PaladinBlessingType::PaladinBlessingType_Might;
                                        paladinBlessing_might = true;
                                    }
                                    else if (!paladinBlessing_wisdom)
                                    {
                                        nap->blessingType = PaladinBlessingType::PaladinBlessingType_Wisdom;
                                        paladinBlessing_wisdom = true;
                                    }
                                    else if (!paladinBlessing_kings)
                                    {
                                        nap->blessingType = PaladinBlessingType::PaladinBlessingType_Kings;
                                        paladinBlessing_kings = true;
                                    }
                                    else
                                    {
                                        nap->blessingType = PaladinBlessingType::PaladinBlessingType_Might;
                                        paladinBlessing_might = true;
                                    }

                                    if (!paladinAura_devotion)
                                    {
                                        nap->auraType = PaladinAuraType::PaladinAuraType_Devotion;
                                        paladinAura_devotion = true;
                                    }
                                    else if (!paladinAura_retribution)
                                    {
                                        nap->auraType = PaladinAuraType::PaladinAuraType_Retribution;
                                        paladinAura_retribution = true;
                                    }
                                    else if (!paladinAura_concentration)
                                    {
                                        nap->auraType = PaladinAuraType::PaladinAuraType_Concentration;
                                        paladinAura_concentration = true;
                                    }
                                    else if (!paladinAura_fire)
                                    {
                                        nap->auraType = PaladinAuraType::PaladinAuraType_FireResistant;
                                        paladinAura_fire = true;
                                    }
                                    else if (!paladinAura_frost)
                                    {
                                        nap->auraType = PaladinAuraType::PaladinAuraType_FrostResistant;
                                        paladinAura_frost = true;
                                    }
                                    else if (!paladinAura_shadow)
                                    {
                                        nap->auraType = PaladinAuraType::PaladinAuraType_ShadowResistant;
                                        paladinAura_shadow = true;
                                    }
                                    else
                                    {
                                        nap->auraType = PaladinAuraType::PaladinAuraType_Devotion;
                                        paladinAura_devotion = true;
                                    }
                                }
                            }
                            if (member->getClass() == Classes::CLASS_MAGE)
                            {
                                if (rtiIndex >= 0 && rtiIndex < TARGETICONCOUNT)
                                {
                                    na->rti = rtiIndex;
                                    rtiIndex++;
                                }
                            }
                            if (member->getClass() == Classes::CLASS_HUNTER)
                            {
                                if (NingerAction_Hunter* nah = (NingerAction_Hunter*)na)
                                {
                                    if (hunterAspect_wild)
                                    {
                                        nah->aspectType = HunterAspectType::HunterAspectType_Hawk;
                                    }
                                    else
                                    {
                                        nah->aspectType = HunterAspectType::HunterAspectType_Wild;
                                        hunterAspect_wild = true;
                                    }
                                }
                            }
                            if (member->getClass() == Classes::CLASS_WARLOCK)
                            {
                                if (NingerAction_Warlock* naWarlock = (NingerAction_Warlock*)na)
                                {
                                    if (!warlockCurse_Weakness)
                                    {
                                        naWarlock->curseType = WarlockCurseType::WarlockCurseType_Weakness;
                                        warlockCurse_Weakness = true;
                                    }
                                    else if (!warlockCurse_Tongues)
                                    {
                                        naWarlock->curseType = WarlockCurseType::WarlockCurseType_Tongues;
                                        warlockCurse_Tongues = true;
                                    }
                                    else if (!warlockCurse_Element)
                                    {
                                        naWarlock->curseType = WarlockCurseType::WarlockCurseType_Element;
                                        warlockCurse_Element = true;
                                    }
                                    else
                                    {
                                        naWarlock->curseType = WarlockCurseType::WarlockCurseType_Weakness;
                                    }
                                }
                            }
                        }
                    }
                }
                replyStream << "Arranged";
            }
            else
            {
                replyStream << "You are not leader";
            }
        }
        else
        {
            replyStream << "Not in a group";
        }
        sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, replyStream.str().c_str(), pmPlayer);
    }
    else if (commandName == "assemble")
    {
        std::ostringstream replyStream;
        if (Player* teleportTarget = ObjectAccessor::FindPlayer(pmPlayer->GetTarget()))
        {
            if (pmTargetPlayer)
            {
                if (teleportTarget->GetGUID() != pmTargetPlayer->GetGUID())
                {
                    pmTargetPlayer->teleportTargetGuid = teleportTarget->GetGUID().GetCounter();
                    if (!pmTargetPlayer->IsAlive())
                    {
                        pmTargetPlayer->teleportDelay = sNingerConfig->CorpseRunDelay;
                        pmTargetPlayer->reviveDelay = sNingerConfig->CorpseRunDelay + 5000;
                        replyStream << "I am dead, Prepare to do corpserun to " << teleportTarget->GetName() << " in " << pmTargetPlayer->teleportDelay / 1000 << " seconds";
                    }
                    else
                    {
                        pmTargetPlayer->teleportDelay = sNingerConfig->TeleportDelay;
                        replyStream << "Prepare to teleport to " << teleportTarget->GetName() << " in " << pmTargetPlayer->teleportDelay / 1000 << " seconds";
                    }
                    if (pmTargetPlayer->GetGUID() == pmPlayer->GetGUID())
                    {
                        sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, replyStream.str(), pmPlayer);
                    }
                    else
                    {
                        pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmPlayer);
                    }
                }
            }
            else if (pmTargetGroup)
            {
                for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
                {
                    Player* member = groupRef->GetSource();
                    if (member)
                    {
                        if (member->GetGUID() != teleportTarget->GetGUID())
                        {
                            HandleChatCommand(pmPlayer, pmContent, member, pmTargetGroup);
                        }
                    }
                }
            }
            else
            {
                HandleChatCommand(pmPlayer, pmContent, pmPlayer);
            }
        }
        else
        {
            sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, "No target", pmPlayer);
        }
    }
    else if (commandName == "leader")
    {
        if (pmTargetGroup)
        {
            if (pmTargetGroup->GetLeaderGUID() != pmPlayer->GetGUID())
            {
                bool change = true;
                if (Player* leader = ObjectAccessor::FindPlayer(pmTargetGroup->GetLeaderGUID()))
                {
                    if (WorldSession* leaderSession = leader->GetSession())
                    {
                        if (!leaderSession->isNinger)
                        {
                            change = false;
                        }
                    }
                }
                if (change)
                {
                    pmTargetGroup->ChangeLeader(pmPlayer->GetGUID());
                }
                else
                {
                    sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, "Leader is valid", pmPlayer);
                }
            }
            else
            {
                sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, "You are the leader", pmPlayer);
            }
        }
        else
        {
            sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, "You are not in a group", pmPlayer);
        }
    }
    else if (commandName == "ninger")
    {
        if (commandVector.size() > 1)
        {
            std::string ningerAction = commandVector.at(1);
            if (ningerAction == "reset")
            {
                std::ostringstream replyStream;
                bool allOffline = true;
                for (std::unordered_set<NingerEntity*>::iterator reIT = ningerEntitySet.begin(); reIT != ningerEntitySet.end(); reIT++)
                {
                    if (NingerEntity* eachRE = *reIT)
                    {
                        if (eachRE->entityState != NingerEntityState::NingerEntityState_None && eachRE->entityState != NingerEntityState::NingerEntityState_OffLine)
                        {
                            allOffline = false;
                            replyStream << "Not all ningers are offline. Going offline first";
                            LogoutNingers();
                            break;
                        }
                    }
                }
                if (allOffline)
                {
                    replyStream << "All ningers are offline. Ready to delete";
                    DeleteNingers();
                }
                sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, replyStream.str().c_str(), pmPlayer);
            }
            else if (ningerAction == "offline")
            {
                std::ostringstream replyStream;
                replyStream << "All ningers are going offline";
                LogoutNingers();
                sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, replyStream.str().c_str(), pmPlayer);
            }
            else if (ningerAction == "online")
            {
                uint32 playerLevel = pmPlayer->getLevel();
                if (playerLevel < 10)
                {
                    std::ostringstream replyStream;
                    replyStream << "You level is too low";
                    sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, replyStream.str().c_str(), pmPlayer);
                }
                else
                {
                    uint32 ningerCount = sNingerConfig->NingerCountEachLevel;
                    if (commandVector.size() > 2)
                    {
                        ningerCount = atoi(commandVector.at(2).c_str());
                    }
                    if (ningerCount > 0)
                    {
                        std::ostringstream replyTitleStream;
                        replyTitleStream << "ninger count to go online : " << ningerCount;
                        sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, replyTitleStream.str().c_str(), pmPlayer);
                        LoginNinger(playerLevel, ningerCount);
                    }
                }
            }
            else if (ningerAction == "relocate")
            {
                std::unordered_map<uint32, WorldSession*> allSessions = sWorld->GetAllSessions();
                for (std::unordered_map<uint32, WorldSession*>::iterator wsIT = allSessions.begin(); wsIT != allSessions.end(); wsIT++)
                {
                    if (WorldSession* eachWS = wsIT->second)
                    {
                        if (eachWS->isNinger)
                        {
                            if (Player* eachNinger = eachWS->GetPlayer())
                            {
                                if (eachNinger->IsInWorld())
                                {
                                    eachNinger->TeleportTo(eachNinger->m_homebindMapId, eachNinger->m_homebindX, eachNinger->m_homebindY, eachNinger->m_homebindZ, 0.0f);

                                    std::ostringstream replyTitleStream;
                                    replyTitleStream << "Teleport ninger to homebind : " << eachNinger->GetName();
                                    sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, replyTitleStream.str().c_str(), pmPlayer);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else if (commandName == "engage")
    {
        if (pmTargetPlayer)
        {
            if (NingerStrategy_Base* ns = pmTargetPlayer->strategyMap[pmTargetPlayer->activeStrategyIndex])
            {
                ns->freeze = false;
                if (Unit* target = pmPlayer->GetSelectedUnit())
                {
                    if (ns->Engage(target))
                    {
                        int engageLimit = DEFAULT_ACTION_LIMIT_DELAY;
                        if (commandVector.size() > 1)
                        {
                            std::string checkStr = commandVector.at(1);
                            engageLimit = atoi(checkStr.c_str());
                        }
                        ns->actionLimit = engageLimit;
                        ns->ogActionTarget = target->GetGUID();
                        ns->actionType = ActionType::ActionType_Engage;
                        std::ostringstream replyStream;
                        replyStream << "Try to engage " << target->GetName();
                        pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmPlayer);
                    }
                }
            }
        }
        else if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmPlayer->GetGUID())
                    {
                        HandleChatCommand(pmPlayer, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "tank")
    {
        if (pmTargetPlayer)
        {
            if (NingerStrategy_Base* ns = pmTargetPlayer->strategyMap[pmTargetPlayer->activeStrategyIndex])
            {
                ns->freeze = false;
                if (Unit* target = pmPlayer->GetSelectedUnit())
                {
                    if (ns->Tank(target))
                    {
                        int engageLimit = DEFAULT_ACTION_LIMIT_DELAY;
                        if (commandVector.size() > 1)
                        {
                            std::string checkStr = commandVector.at(1);
                            engageLimit = atoi(checkStr.c_str());
                        }
                        ns->actionLimit = engageLimit;
                        ns->ogActionTarget = target->GetGUID();
                        ns->actionType = ActionType::ActionType_Engage;
                        std::ostringstream replyStream;
                        replyStream << "Try to engage " << target->GetName();
                        pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmPlayer);
                    }
                }
            }
        }
        else if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmPlayer->GetGUID())
                    {
                        HandleChatCommand(pmPlayer, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "freeze")
    {
        if (pmTargetPlayer)
        {
            if (NingerStrategy_Base* ns = pmTargetPlayer->strategyMap[pmTargetPlayer->activeStrategyIndex])
            {
                ns->freeze = true;
                pmTargetPlayer->StopMoving();
                pmTargetPlayer->InterruptNonMeleeSpells(true);
                pmTargetPlayer->AttackStop();
                pmTargetPlayer->ningerAction->PetStop();
                std::ostringstream replyStream;
                replyStream << "Freezed";
                pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmPlayer);
            }
        }
        else if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmPlayer->GetGUID())
                    {
                        HandleChatCommand(pmPlayer, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "prepare")
    {
        if (pmTargetPlayer)
        {
            pmTargetPlayer->ningerAction->Prepare();
        }
        else if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmPlayer->GetGUID())
                    {
                        HandleChatCommand(pmPlayer, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "rest")
    {
        if (pmTargetPlayer)
        {
            if (NingerStrategy_Base* ns = pmTargetPlayer->strategyMap[pmTargetPlayer->activeStrategyIndex])
            {
                ns->Rest(true);
                pmTargetPlayer->Whisper("Resting", Language::LANG_UNIVERSAL, pmPlayer);
            }
        }
        else if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmPlayer->GetGUID())
                    {
                        HandleChatCommand(pmPlayer, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "revive")
    {
        if (pmTargetPlayer)
        {
            if (NingerStrategy_Base* ns = pmTargetPlayer->strategyMap[pmTargetPlayer->activeStrategyIndex])
            {
                ns->freeze = false;
                if (Player* target = pmPlayer->GetSelectedPlayer())
                {
                    if (ns->Revive(target))
                    {
                        ns->actionLimit = DEFAULT_ACTION_LIMIT_DELAY;
                        ns->ogActionTarget = target->GetGUID();
                        ns->actionType = ActionType::ActionType_Revive;
                        std::ostringstream replyStream;
                        replyStream << "Try to do reviving";
                        pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmPlayer);
                    }
                }
                else if (ns->Revive())
                {
                    std::ostringstream replyStream;
                    replyStream << "Try to do reviving";
                    pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmPlayer);
                }
            }
        }
        else if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmPlayer->GetGUID())
                    {
                        HandleChatCommand(pmPlayer, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "follow")
    {
        if (pmTargetPlayer)
        {
            if (NingerStrategy_Base* ns = pmTargetPlayer->strategyMap[pmTargetPlayer->activeStrategyIndex])
            {
                std::ostringstream replyStream;
                if (commandVector.size() > 1)
                {
                    std::string cmdDistanceStr = commandVector.at(1);
                    float cmdDistance = atof(cmdDistanceStr.c_str());
                    if (cmdDistance >= FOLLOW_MIN_DISTANCE && cmdDistance <= FOLLOW_MAX_DISTANCE && cmdDistance > ns->followDistanceMin)
                    {
                        ns->followDistanceMax = cmdDistance;
                        if (commandVector.size() > 2)
                        {
                            cmdDistanceStr = commandVector.at(2);
                            cmdDistance = atof(cmdDistanceStr.c_str());
                            if (cmdDistance >= 0 && cmdDistance <= FOLLOW_MAX_DISTANCE && cmdDistance < ns->followDistanceMax)
                            {
                                ns->followDistanceMin = cmdDistance;
                            }
                            else
                            {
                                replyStream << "Distance is not valid";
                            }
                        }
                        replyStream << "Follow distance - " << ns->followDistanceMax << " " << ns->followDistanceMin;
                    }
                    else
                    {
                        replyStream << "Distance is not valid";
                    }
                }
                else
                {
                    if (ns->Follow())
                    {
                        ns->freeze = false;
                        ns->restLimit = 0;
                        ns->actionLimit = 0;
                        replyStream << "Following - " << ns->followDistanceMax << " " << ns->followDistanceMin;
                    }
                    else
                    {
                        replyStream << "can not follow";
                    }
                }
                pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmPlayer);
            }
        }
        else if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmPlayer->GetGUID())
                    {
                        HandleChatCommand(pmPlayer, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "chase")
    {
        if (pmTargetPlayer)
        {
            if (NingerStrategy_Base* ns = pmTargetPlayer->strategyMap[pmTargetPlayer->activeStrategyIndex])
            {
                std::ostringstream replyStream;
                if (commandVector.size() > 1)
                {
                    std::string cmdDistanceStr = commandVector.at(1);
                    float cmdDistance = atof(cmdDistanceStr.c_str());
                    if (cmdDistance >= MELEE_MIN_DISTANCE && cmdDistance <= RANGE_MAX_DISTANCE && cmdDistance > ns->chaseDistanceMin)
                    {
                        ns->chaseDistanceMax = cmdDistance;
                        if (commandVector.size() > 2)
                        {
                            cmdDistanceStr = commandVector.at(2);
                            cmdDistance = atof(cmdDistanceStr.c_str());
                            if (cmdDistance >= MELEE_MIN_DISTANCE && cmdDistance <= RANGE_MAX_DISTANCE && cmdDistance < ns->chaseDistanceMax)
                            {
                                ns->chaseDistanceMin = cmdDistance;
                            }
                            else
                            {
                                replyStream << "Distance is not valid";
                            }
                        }
                    }
                    else
                    {
                        replyStream << "Distance is not valid";
                    }
                }
                replyStream << "Chase distance - " << ns->chaseDistanceMax << " " << ns->chaseDistanceMin;
                pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmPlayer);
            }
        }
        else if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmPlayer->GetGUID())
                    {
                        HandleChatCommand(pmPlayer, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "gather")
    {
        if (pmTargetPlayer)
        {
            if (NingerStrategy_Base* ns = pmTargetPlayer->strategyMap[pmTargetPlayer->activeStrategyIndex])
            {
                std::ostringstream replyStream;
                if (pmTargetPlayer->IsAlive())
                {
                    if (pmPlayer->GetDistance(pmTargetPlayer) < FOLLOW_MAX_DISTANCE)
                    {
                        if (ns->gatherDelay > 0)
                        {
                            replyStream << "I am on the way";
                        }
                        else
                        {
                            ns->freeze = false;
                            ns->restLimit = 0;
                            ns->actionLimit = 0;
                            ns->gatherDelay = IN_MILLISECONDS;
                            if (pmTargetPlayer->IsNonMeleeSpellCast(false))
                            {
                                pmTargetPlayer->InterruptSpell(CurrentSpellTypes::CURRENT_GENERIC_SPELL);
                                pmTargetPlayer->InterruptSpell(CurrentSpellTypes::CURRENT_CHANNELED_SPELL);
                            }
                            pmTargetPlayer->ningerAction->rm->MovePoint(pmPlayer->GetPosition(), ns->gatherDelay);
                            replyStream << "I will move to you";
                        }
                    }
                    else
                    {
                        replyStream << "I am too far away";
                    }
                }
                else
                {
                    replyStream << "I am dead";
                }
                pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmPlayer);
            }
        }
        else if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmPlayer->GetGUID())
                    {
                        HandleChatCommand(pmPlayer, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "cast")
    {

    }
    else if (commandName == "unaura")
    {

    }
    else if (commandName == "delay")
    {
        if (pmTargetPlayer)
        {
            if (NingerStrategy_Base* ns = pmTargetPlayer->strategyMap[pmTargetPlayer->activeStrategyIndex])
            {
                std::ostringstream replyStream;
                if (commandVector.size() > 1)
                {
                    int delayMS = std::stoi(commandVector.at(1));
                    ns->dpsDelay = delayMS;
                    replyStream << "DPS delay set to : " << delayMS;
                }
                else
                {
                    replyStream << "DPS delay is : " << ns->dpsDelay;
                }
                pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmPlayer);
            }
        }
        else if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmPlayer->GetGUID())
                    {
                        HandleChatCommand(pmPlayer, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "cure")
    {
        if (pmTargetPlayer)
        {
            if (NingerStrategy_Base* ns = pmTargetPlayer->strategyMap[pmTargetPlayer->activeStrategyIndex])
            {
                std::ostringstream replyStream;
                if (commandVector.size() > 1)
                {
                    std::string cureCMD = commandVector.at(1);
                    if (cureCMD == "on")
                    {
                        ns->cure = true;
                    }
                    else if (cureCMD == "off")
                    {
                        ns->cure = false;
                    }
                    else
                    {
                        replyStream << "Unknown state";
                    }
                }
                if (ns->cure)
                {
                    replyStream << "auto cure is on";
                }
                else
                {
                    replyStream << "auto cure is off";
                }
                pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmPlayer);
            }
        }
        else if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmPlayer->GetGUID())
                    {
                        HandleChatCommand(pmPlayer, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "petting")
    {
        if (pmTargetPlayer)
        {
            if (NingerStrategy_Base* ns = pmTargetPlayer->strategyMap[pmTargetPlayer->activeStrategyIndex])
            {
                std::ostringstream replyStream;
                if (commandVector.size() > 1)
                {
                    std::string switchCMD = commandVector.at(1);
                    if (switchCMD == "on")
                    {
                        ns->petting = true;
                    }
                    else if (switchCMD == "off")
                    {
                        ns->petting = false;
                    }
                    else if (switchCMD == "reset")
                    {
                        if (pmTargetPlayer->getClass() == Classes::CLASS_HUNTER)
                        {
                            if (NingerAction_Hunter* naHunter = (NingerAction_Hunter*)pmTargetPlayer->ningerAction)
                            {
                                if (naHunter->Petting(ns->petting, true))
                                {
                                    replyStream << "Pet abandoned. ";
                                }
                                else
                                {
                                    replyStream << "reset failed";
                                }
                            }
                        }
                    }
                    else
                    {
                        replyStream << "Unknown state";
                    }
                }
                if (ns->petting)
                {
                    replyStream << "Petting is on";
                }
                else
                {
                    replyStream << "Petting is off";
                }
                pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmPlayer);
            }
        }
        else if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmPlayer->GetGUID())
                    {
                        HandleChatCommand(pmPlayer, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "aoe")
    {
        if (pmTargetPlayer)
        {
            if (NingerStrategy_Base* ns = pmTargetPlayer->strategyMap[pmTargetPlayer->activeStrategyIndex])
            {
                std::ostringstream replyStream;
                if (commandVector.size() > 1)
                {
                    std::string switchCMD = commandVector.at(1);
                    if (switchCMD == "on")
                    {
                        ns->aoe = true;
                    }
                    else if (switchCMD == "off")
                    {
                        ns->aoe = false;
                    }
                    else
                    {
                        replyStream << "Unknown state";
                    }
                }
                if (ns->aoe)
                {
                    replyStream << "AOE is on";
                }
                else
                {
                    replyStream << "AOE is off";
                }
                pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmPlayer);
            }
        }
        else if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmPlayer->GetGUID())
                    {
                        HandleChatCommand(pmPlayer, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "rti")
    {
        if (pmTargetPlayer)
        {
            if (NingerStrategy_Base* ns = pmTargetPlayer->strategyMap[pmTargetPlayer->activeStrategyIndex])
            {
                std::ostringstream replyStream;
                int targetIcon = -1;
                if (commandVector.size() > 1)
                {
                    std::string iconIndex = commandVector.at(1);
                    targetIcon = atoi(iconIndex.c_str());
                }
                if (targetIcon >= 0 && targetIcon < TARGETICONCOUNT)
                {
                    pmTargetPlayer->ningerAction->rti = targetIcon;
                }
                replyStream << "RTI is " << pmTargetPlayer->ningerAction->rti;
                pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmPlayer);
            }
        }
        else if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmPlayer->GetGUID())
                    {
                        HandleChatCommand(pmPlayer, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "equip")
    {
        if (pmTargetPlayer)
        {
            std::ostringstream replyStream;
            pmTargetPlayer->ningerAction->InitializeEquipments(true);
            replyStream << "Equipments reset";
            pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmPlayer);
        }
        else if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmPlayer->GetGUID())
                    {
                        HandleChatCommand(pmPlayer, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "talent")
    {
        if (pmTargetPlayer)
        {
            std::ostringstream replyStream;
            pmTargetPlayer->ningerAction->ResetTalent();
            replyStream << "Talents reset";
            pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmPlayer);
        }
        else if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmPlayer->GetGUID())
                    {
                        HandleChatCommand(pmPlayer, pmContent, member);
                    }
                }
            }
        }
    }
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
