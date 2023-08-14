#include "NierManager.h"
#include "NierStrategies/NierStrategy_Base.h"
#include "NierConfig.h"
#include "NierActions/NierAction_Base.h"
#include "NierActions/NierAction_Druid.h"
#include "NierActions/NierAction_Hunter.h"
#include "NierActions/NierAction_Mage.h"
#include "NierActions/NierAction_Paladin.h"
#include "NierActions/NierAction_Priest.h"
#include "NierActions/NierAction_Rogue.h"
#include "NierActions/NierAction_Shaman.h"
#include "NierActions/NierAction_Warlock.h"
#include "NierActions/NierAction_Warrior.h"

#include "MingManager.h"

#include "GameTime.h"
#include "Item.h"
#include "InstanceSaveMgr.h"
#include "Player.h"
#include "Group.h"
#include "World/World.h"
#include "Accounts/AccountMgr.h"
#include <boost/chrono/duration.hpp>
#include "MoveSplineInit.h"

NierManager::NierManager()
{
    checkDelay = 0;
    allianceRaces.clear();
    hordeRaces.clear();
    nameIndex = 0;
    nierNameMap.clear();
    nierEntityMap.clear();
    characterTalentTabNameMap.clear();
    equipsMap.clear();
}

void NierManager::InitializeManager()
{
    if (sNierConfig->Enable == 0)
    {
        return;
    }

    sLog->outMessage(NIER_MARK, LogLevel::LOG_LEVEL_DEBUG, "Initialize nier");

    checkDelay = sNierConfig->ManagerCheckDelay;

    allianceRaces.clear();
    allianceRaces[CLASS_WARRIOR][allianceRaces[CLASS_WARRIOR].size()] = RACE_HUMAN;
    allianceRaces[CLASS_WARRIOR][allianceRaces[CLASS_WARRIOR].size()] = RACE_NIGHTELF;
    allianceRaces[CLASS_WARRIOR][allianceRaces[CLASS_WARRIOR].size()] = RACE_GNOME;
    allianceRaces[CLASS_WARRIOR][allianceRaces[CLASS_WARRIOR].size()] = RACE_DWARF;
    hordeRaces[CLASS_WARRIOR][hordeRaces[CLASS_WARRIOR].size()] = RACE_ORC;
    hordeRaces[CLASS_WARRIOR][hordeRaces[CLASS_WARRIOR].size()] = Races::RACE_UNDEAD_PLAYER;
    hordeRaces[CLASS_WARRIOR][hordeRaces[CLASS_WARRIOR].size()] = RACE_TAUREN;
    hordeRaces[CLASS_WARRIOR][hordeRaces[CLASS_WARRIOR].size()] = RACE_TROLL;
    hordeRaces[CLASS_WARRIOR][hordeRaces[CLASS_WARRIOR].size()] = Races::RACE_DRAENEI;

    allianceRaces[CLASS_PALADIN][allianceRaces[CLASS_PALADIN].size()] = RACE_HUMAN;
    allianceRaces[CLASS_PALADIN][allianceRaces[CLASS_PALADIN].size()] = RACE_DWARF;
    hordeRaces[CLASS_PALADIN][hordeRaces[CLASS_PALADIN].size()] = RACE_BLOODELF;
    hordeRaces[Classes::CLASS_PALADIN][hordeRaces[Classes::CLASS_PALADIN].size()] = Races::RACE_DRAENEI;

    allianceRaces[CLASS_ROGUE][allianceRaces[CLASS_ROGUE].size()] = RACE_HUMAN;
    allianceRaces[CLASS_ROGUE][allianceRaces[CLASS_ROGUE].size()] = RACE_DWARF;
    allianceRaces[CLASS_ROGUE][allianceRaces[CLASS_ROGUE].size()] = RACE_NIGHTELF;
    allianceRaces[CLASS_ROGUE][allianceRaces[CLASS_ROGUE].size()] = RACE_GNOME;
    hordeRaces[CLASS_ROGUE][hordeRaces[CLASS_ROGUE].size()] = RACE_ORC;
    hordeRaces[CLASS_ROGUE][hordeRaces[CLASS_ROGUE].size()] = RACE_TROLL;
    hordeRaces[CLASS_ROGUE][hordeRaces[CLASS_ROGUE].size()] = Races::RACE_UNDEAD_PLAYER;
    hordeRaces[Classes::CLASS_ROGUE][hordeRaces[Classes::CLASS_ROGUE].size()] = Races::RACE_BLOODELF;

    allianceRaces[CLASS_PRIEST][allianceRaces[CLASS_PRIEST].size()] = RACE_HUMAN;
    allianceRaces[CLASS_PRIEST][allianceRaces[CLASS_PRIEST].size()] = RACE_DWARF;
    allianceRaces[CLASS_PRIEST][allianceRaces[CLASS_PRIEST].size()] = RACE_NIGHTELF;
    hordeRaces[CLASS_PRIEST][hordeRaces[CLASS_PRIEST].size()] = RACE_TROLL;
    hordeRaces[CLASS_PRIEST][hordeRaces[CLASS_PRIEST].size()] = Races::RACE_UNDEAD_PLAYER;
    hordeRaces[Classes::CLASS_PRIEST][hordeRaces[Classes::CLASS_PRIEST].size()] = Races::RACE_DRAENEI;
    hordeRaces[Classes::CLASS_PRIEST][hordeRaces[Classes::CLASS_PRIEST].size()] = Races::RACE_BLOODELF;

    allianceRaces[CLASS_MAGE][allianceRaces[CLASS_MAGE].size()] = RACE_HUMAN;
    allianceRaces[CLASS_MAGE][allianceRaces[CLASS_MAGE].size()] = RACE_GNOME;
    hordeRaces[CLASS_MAGE][hordeRaces[CLASS_MAGE].size()] = Races::RACE_UNDEAD_PLAYER;
    hordeRaces[CLASS_MAGE][hordeRaces[CLASS_MAGE].size()] = RACE_TROLL;
    hordeRaces[Classes::CLASS_MAGE][hordeRaces[Classes::CLASS_MAGE].size()] = Races::RACE_DRAENEI;
    hordeRaces[Classes::CLASS_MAGE][hordeRaces[Classes::CLASS_MAGE].size()] = Races::RACE_BLOODELF;

    allianceRaces[CLASS_WARLOCK][allianceRaces[CLASS_WARLOCK].size()] = RACE_HUMAN;
    allianceRaces[CLASS_WARLOCK][allianceRaces[CLASS_WARLOCK].size()] = RACE_GNOME;
    hordeRaces[CLASS_WARLOCK][hordeRaces[CLASS_WARLOCK].size()] = Races::RACE_UNDEAD_PLAYER;
    hordeRaces[CLASS_WARLOCK][hordeRaces[CLASS_WARLOCK].size()] = RACE_ORC;
    hordeRaces[Classes::CLASS_WARLOCK][hordeRaces[Classes::CLASS_WARLOCK].size()] = Races::RACE_BLOODELF;

    allianceRaces[CLASS_SHAMAN][allianceRaces[CLASS_SHAMAN].size()] = RACE_DRAENEI;
    hordeRaces[CLASS_SHAMAN][hordeRaces[CLASS_SHAMAN].size()] = RACE_ORC;
    hordeRaces[CLASS_SHAMAN][hordeRaces[CLASS_SHAMAN].size()] = RACE_TAUREN;
    hordeRaces[CLASS_SHAMAN][hordeRaces[CLASS_SHAMAN].size()] = RACE_TROLL;
    hordeRaces[Classes::CLASS_SHAMAN][hordeRaces[Classes::CLASS_SHAMAN].size()] = Races::RACE_DRAENEI;

    allianceRaces[CLASS_HUNTER][allianceRaces[CLASS_HUNTER].size()] = RACE_DWARF;
    allianceRaces[CLASS_HUNTER][allianceRaces[CLASS_HUNTER].size()] = RACE_NIGHTELF;
    hordeRaces[CLASS_HUNTER][hordeRaces[CLASS_HUNTER].size()] = RACE_ORC;
    hordeRaces[CLASS_HUNTER][hordeRaces[CLASS_HUNTER].size()] = RACE_TAUREN;
    hordeRaces[CLASS_HUNTER][hordeRaces[CLASS_HUNTER].size()] = RACE_TROLL;
    hordeRaces[Classes::CLASS_HUNTER][hordeRaces[Classes::CLASS_HUNTER].size()] = Races::RACE_DRAENEI;
    hordeRaces[Classes::CLASS_HUNTER][hordeRaces[Classes::CLASS_HUNTER].size()] = Races::RACE_BLOODELF;

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

    if (sNierConfig->Reset)
    {
        DeleteNiers();
    }

    QueryResult nierQR = CharacterDatabase.Query("SELECT entry, master_id, account_name, account_id, character_id, race, career, specialty, role FROM nier");
    if (nierQR)
    {
        do
        {
            Field* fields = nierQR->Fetch();
            NierEntity* re = new NierEntity();
            re->entry = fields[0].Get<uint32>();
            re->master_id = fields[1].Get<uint32>();
            re->account_name = fields[2].Get<std::string>();
            re->account_id = fields[3].Get<uint32>();
            re->character_id = fields[4].Get<uint32>();
            re->target_race = fields[5].Get<uint32>();
            re->target_class = fields[6].Get<uint32>();
            re->target_specialty = fields[7].Get<uint32>();
            nierEntityMap[re->master_id].insert(re);
        } while (nierQR->NextRow());
    }
    nameIndex = 0;
    QueryResult nierNameQR = WorldDatabase.Query("SELECT name FROM nier_names order by rand()");
    if (nierNameQR)
    {
        do
        {
            Field* fields = nierNameQR->Fetch();
            std::string eachName = fields[0].Get<std::string>();
            nierNameMap[nierNameMap.size()] = eachName;
        } while (nierNameQR->NextRow());
    }

    sLog->outMessage(NIER_MARK, LogLevel::LOG_LEVEL_DEBUG, "nier initialized");
}

NierManager* NierManager::instance()
{
    static NierManager instance;
    return &instance;
}

void NierManager::UpdateNierManager(uint32 pmDiff)
{
    if (sNierConfig->Enable == 0)
    {
        return;
    }

    if (checkDelay >= 0)
    {
        checkDelay -= pmDiff;
    }
    if (checkDelay < 0)
    {
        checkDelay = sNierConfig->ManagerCheckDelay;
        std::unordered_map<uint32, WorldSession*> allSessions = sWorld->GetAllSessions();
        for (std::unordered_map<uint32, WorldSession*>::iterator wsIT = allSessions.begin(); wsIT != allSessions.end(); wsIT++)
        {
            if (WorldSession* eachWS = wsIT->second)
            {
                if (!eachWS->isNier)
                {
                    if (Player* eachPlayer = eachWS->GetPlayer())
                    {
                        LoginNierPartners(eachPlayer->GetGUID().GetCounter());
                    }
                }
            }
        }
    }
}

void NierManager::UpdateNierEntities(uint32 pmDiff)
{
    for (std::unordered_map<uint32, std::unordered_set<NierEntity*>>::iterator nierGroupIT = nierEntityMap.begin(); nierGroupIT != nierEntityMap.end(); nierGroupIT++)
    {
        for (std::unordered_set<NierEntity*>::iterator reIT = nierGroupIT->second.begin(); reIT != nierGroupIT->second.end(); reIT++)
        {
            (*reIT)->Update(pmDiff);
        }
    }
}

void NierManager::LogoutNiers(bool pmInstant)
{
    for (std::unordered_map<uint32, std::unordered_set<NierEntity*>>::iterator nierGroupIT = nierEntityMap.begin(); nierGroupIT != nierEntityMap.end(); nierGroupIT++)
    {
        for (std::unordered_set<NierEntity*>::iterator reIT = nierGroupIT->second.begin(); reIT != nierGroupIT->second.end(); reIT++)
        {
            if (NierEntity* eachNier = *reIT)
            {
                if (pmInstant)
                {
                    ObjectGuid playerGuid = ObjectGuid(HighGuid::Player, eachNier->character_id);
                    if (Player* eachPlayer = ObjectAccessor::FindPlayer(playerGuid))
                    {
                        std::ostringstream logStream;
                        logStream << "Logout nier : " << eachPlayer->GetName();
                        sLog->outMessage(NIER_MARK, LogLevel::LOG_LEVEL_DEBUG, logStream.str().c_str());
                        eachPlayer->GetSession()->LogoutPlayer(true);
                    }
                }
                else
                {
                    eachNier->entityState = NierEntityState::NierEntityState_DoLogoff;
                    eachNier->checkDelay = 10;
                }
            }
        }
    }
}

void NierManager::DeleteNiers()
{
    QueryResult nierQR = CharacterDatabase.Query("SELECT account_id FROM nier");
    if (nierQR)
    {
        do
        {
            Field* fields = nierQR->Fetch();
            uint32 eachAccountId = fields[0].Get<uint32>();
            if (eachAccountId > 0)
            {
                AccountMgr::DeleteAccount(eachAccountId);
            }
        } while (nierQR->NextRow());
    }
    CharacterDatabase.DirectExecute("delete from nier");

    std::ostringstream accountQueryStream;
    accountQueryStream << "SELECT id FROM account where username like '" << NIER_MARK << "%'";
    std::string accountQuerySQL = accountQueryStream.str();
    QueryResult nierAccountQR = LoginDatabase.Query(accountQuerySQL.c_str());
    if (nierAccountQR)
    {
        do
        {
            Field* fields = nierAccountQR->Fetch();
            uint32 eachAccountId = fields[0].Get<uint32>();
            if (eachAccountId > 0)
            {
                AccountMgr::DeleteAccount(eachAccountId);
            }
        } while (nierAccountQR->NextRow());
    }
}

bool NierManager::LoginNierPartners(uint32 pmMasterId)
{
    ObjectGuid masterGuid = ObjectGuid(HighGuid::Player, pmMasterId);
    if (Player* master = ObjectAccessor::FindConnectedPlayer(masterGuid))
    {
        int tankCount = 1;
        int healerCount = 0;
        int dpsCount = 3;
        std::unordered_map<uint32, uint32> partnerMap;
        QueryResult nierQR = CharacterDatabase.Query("SELECT master_id, role FROM nier where master_id = {}", pmMasterId);
        if (nierQR)
        {
            do
            {
                Field* fields = nierQR->Fetch();
                uint32 character_id = fields[0].Get<uint32>();
                uint32 role = fields[1].Get<uint32>();
                partnerMap[character_id] = role;
                if (role == GroupRole::GroupRole_Tank)
                {
                    tankCount -= 1;
                }
                else if (role == GroupRole::GroupRole_Healer)
                {
                    healerCount -= 1;
                }
                else if (role == GroupRole::GroupRole_DPS)
                {
                    dpsCount -= 1;
                }
            } while (nierQR->NextRow());
        }
        while (tankCount > 0)
        {
            uint32 career = Classes::CLASS_WARRIOR;
            uint32 specialty = 2;

            career = Classes::CLASS_PRIEST;
            specialty = 0;

            uint32 role = GroupRole::GroupRole_Tank;
            uint32 race = urand(0, 4);
            uint32 masterRace = master->getRace();
            if (masterRace == Races::RACE_HUMAN || masterRace == Races::RACE_DWARF || masterRace == Races::RACE_NIGHTELF || masterRace == Races::RACE_DRAENEI || masterRace == Races::RACE_GNOME)
            {
                uint32 raceIndex = urand(0, allianceRaces[career].size() - 1);
                race = allianceRaces[career][raceIndex];
            }
            else
            {
                uint32 raceIndex = urand(0, hordeRaces[career].size() - 1);
                race = hordeRaces[career][raceIndex];
            }
            // debug
            if (race == 0)
            {
                bool breakPoint = true;
            }
            CreateNier(pmMasterId, race, career, specialty, role);
            tankCount -= 1;
        }
        while (healerCount > 0)
        {
            uint32 career = Classes::CLASS_PRIEST;
            uint32 specialty = 1;
            uint32 role = GroupRole::GroupRole_Healer;
            uint32 race = urand(0, 4);
            uint32 masterRace = master->getRace();
            if (masterRace == Races::RACE_HUMAN || masterRace == Races::RACE_DWARF || masterRace == Races::RACE_NIGHTELF || masterRace == Races::RACE_DRAENEI || masterRace == Races::RACE_GNOME)
            {
                uint32 raceIndex = urand(0, allianceRaces[career].size() - 1);
                race = allianceRaces[career][raceIndex];
            }
            else
            {
                uint32 raceIndex = urand(0, hordeRaces[career].size() - 1);
                race = hordeRaces[career][raceIndex];
            }
            // debug
            if (race == 0)
            {
                bool breakPoint = true;
            }
            CreateNier(pmMasterId, race, career, specialty, role);
            healerCount -= 1;
        }
        while (dpsCount > 0)
        {
            uint32 career = Classes::CLASS_PRIEST;
            uint32 specialty = 2;
            uint32 role = GroupRole::GroupRole_DPS;
            uint32 race = urand(0, 4);
            uint32 masterRace = master->getRace();
            if (masterRace == Races::RACE_HUMAN || masterRace == Races::RACE_DWARF || masterRace == Races::RACE_NIGHTELF || masterRace == Races::RACE_DRAENEI || masterRace == Races::RACE_GNOME)
            {
                uint32 raceIndex = urand(0, allianceRaces[career].size() - 1);
                race = allianceRaces[career][raceIndex];
            }
            else
            {
                uint32 raceIndex = urand(0, hordeRaces[career].size() - 1);
                race = hordeRaces[career][raceIndex];
            }
            // debug
            if (race == 0)
            {
                bool breakPoint = true;
            }
            CreateNier(pmMasterId, race, career, specialty, role);
            dpsCount -= 1;
        }

        // nier todo login partners
        if (nierEntityMap.find(pmMasterId) != nierEntityMap.end())
        {
            for (std::unordered_set<NierEntity*>::iterator reIT = nierEntityMap[pmMasterId].begin(); reIT != nierEntityMap[pmMasterId].end(); reIT++)
            {
                if (NierEntity* re = *reIT)
                {
                    if (re->entityState == NierEntityState::NierEntityState_OffLine)
                    {
                        re->entityState = NierEntityState::NierEntityState_Enter;
                        re->checkDelay = urand(1000, 10000);
                    }
                }
            }
        }
    }

    return true;
}

void NierManager::CreateNier(uint32 pmMasterId, uint32 pmRace, uint32 pmCareer, uint32 pmSpecialty, uint32 pmGroupRole)
{
    uint32 currentNierCount = 0;
    QueryResult nierQR = CharacterDatabase.Query("SELECT count(*) FROM nier");
    if (nierQR)
    {
        Field* fields = nierQR->Fetch();
        currentNierCount = fields[0].Get<uint32>();
    }
    std::ostringstream accountNameStream;
    accountNameStream << NIER_MARK << currentNierCount;
    std::string checkAccountName = accountNameStream.str();
    std::ostringstream querySQLStream;
    querySQLStream << "SELECT * FROM account where username ='" << checkAccountName << "'";
    std::string querySQL = querySQLStream.str();
    QueryResult accountNameQR = LoginDatabase.Query(querySQL.c_str());
    if (accountNameQR)
    {
        sLog->outMessage(NIER_MARK, LogLevel::LOG_LEVEL_DEBUG, "Account already {} exists.", checkAccountName);
    }
    else
    {
        uint32 nierEntry = 0;
        nierQR = CharacterDatabase.Query("SELECT entry FROM nier order by entry desc");
        if (nierQR)
        {
            Field* fields = nierQR->Fetch();
            nierEntry = fields[0].Get<uint32>();
        }
        nierEntry = nierEntry + 1;

        std::ostringstream sqlStream;
        sqlStream << "INSERT INTO nier (entry, master_id, account_name, account_id, character_id, race, career, specialty, role) VALUES (" << nierEntry << ", " << pmMasterId << ", '" << checkAccountName << "', 0, 0, " << pmRace << ", " << pmCareer << ", " << pmSpecialty << ", " << pmGroupRole << ")";
        std::string sql = sqlStream.str();
        CharacterDatabase.DirectExecute(sql.c_str());
        std::ostringstream replyStream;
        replyStream << "nier " << checkAccountName << " created";
        sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, replyStream.str().c_str());

        NierEntity* ne = new NierEntity();
        ne->account_id = 0;
        ne->account_name = checkAccountName;
        ne->character_id = 0;
        ne->checkDelay = 1000;
        ne->entityState = NierEntityState_OffLine;
        ne->master_id = pmMasterId;
        ne->entry = nierEntry;
        ne->offlineDelay = 0;
        ne->target_class = pmCareer;
        ne->target_level = 0;
        ne->target_race = pmRace;
        ne->target_specialty = pmSpecialty;
        nierEntityMap[pmMasterId].insert(ne);
    }
}

bool NierManager::IsPolymorphed(Unit* pmTarget)
{
    if (pmTarget)
    {
        if (pmTarget->HasAura(118) || pmTarget->HasAura(12824) || pmTarget->HasAura(12825) || pmTarget->HasAura(12826))
        {
            return true;
        }
        if (pmTarget->HasAura(2637) || pmTarget->HasAura(18657) || pmTarget->HasAura(18658))
        {
            return true;
        }
        if (pmTarget->HasAura(339) || pmTarget->HasAura(1062) || pmTarget->HasAura(5195) || pmTarget->HasAura(5196) || pmTarget->HasAura(9852) || pmTarget->HasAura(9853) || pmTarget->HasAura(26989) || pmTarget->HasAura(53308))
        {
            return true;
        }
    }

    return false;
}

Position NierManager::PredictPosition(Unit* target)
{
    Position pos = target->GetPosition();

    float speed = target->GetSpeed(UnitMoveType::MOVE_RUN);
    float orientation = target->GetOrientation();

    if (target->m_movementInfo.HasMovementFlag(MOVEMENTFLAG_FORWARD))
    {
        pos.m_positionX += cos(orientation) * speed;
        pos.m_positionY += std::sin(orientation) * speed;
    }
    else if (target->m_movementInfo.HasMovementFlag(MOVEMENTFLAG_BACKWARD))
    {
        pos.m_positionX -= cos(orientation) * speed;
        pos.m_positionY -= std::sin(orientation) * speed;
    }

    if (target->m_movementInfo.HasMovementFlag(MOVEMENTFLAG_STRAFE_LEFT))
    {
        pos.m_positionX += cos(orientation + M_PI / 2.f) * speed;
        pos.m_positionY += std::sin(orientation + M_PI / 2.f) * speed;
    }
    else if (target->m_movementInfo.HasMovementFlag(MOVEMENTFLAG_STRAFE_RIGHT))
    {
        pos.m_positionX += cos(orientation - M_PI / 2.f) * speed;
        pos.m_positionY += std::sin(orientation - M_PI / 2.f) * speed;
    }

    return pos;
}

void NierManager::HandleChatCommand(Player* pmCommander, std::string pmContent, Player* pmTargetPlayer, Group* pmTargetGroup)
{
    if (!pmCommander)
    {
        return;
    }
    std::vector<std::string> commandVector = sMingManager->SplitString(pmContent, " ", true);
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
                    if (member->GetGUID() != pmCommander->GetGUID())
                    {
                        HandleChatCommand(pmCommander, pmContent, member);
                    }
                }
            }
        }
        else
        {
            Player* targetPlayer = pmTargetPlayer;
            if (!targetPlayer)
            {
                targetPlayer = pmCommander;
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
                pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmCommander);
            }
            else
            {
                sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, replyStream.str().c_str(), pmCommander);
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
                    if (member->GetGUID() != pmCommander->GetGUID())
                    {
                        HandleChatCommand(pmCommander, pmContent, member);
                    }
                }
            }
        }
        else if (pmTargetPlayer)
        {
            std::ostringstream replyStream;
            replyStream << characterTalentTabNameMap[pmTargetPlayer->getClass()][pmTargetPlayer->nierAction->specialty];
            pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmCommander);
        }
    }
    else if (commandName == "emote")
    {
        if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmCommander->GetGUID())
                    {
                        HandleChatCommand(pmCommander, pmContent, member);
                    }
                }
            }
        }
        else if (pmTargetPlayer)
        {
            Player* targetPlayer = pmTargetPlayer;
            if (!targetPlayer)
            {
                targetPlayer = pmCommander;
            }
            if (commandVector.size() > 1)
            {
                std::string emoteContents = commandVector.at(1);
                int emoteNumber = atoi(emoteContents.c_str());
                targetPlayer->HandleEmoteCommand(emoteNumber);
            }
        }
        else
        {
            if (commandVector.size() > 1)
            {
                std::string emoteContents = commandVector.at(1);
                int emoteNumber = atoi(emoteContents.c_str());
                pmCommander->HandleEmoteCommand(emoteNumber);
            }
        }
    }
    else if (commandName == "join")
    {
        if (ObjectGuid selectionGUID = pmCommander->GetGuidValue(UNIT_FIELD_TARGET))
        {
            if (Player* targetPlayer = ObjectAccessor::FindPlayer(selectionGUID))
            {
                if (targetPlayer->IsInSameGroupWith(pmCommander))
                {
                    pmCommander->TeleportTo(targetPlayer->GetWorldLocation());
                    sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, "joined", pmCommander);
                }
            }
        }
    }
    else if (commandName == "hover")
    {
        if (Unit* targetUnit = pmCommander->GetSelectedUnit())
        {
            if (targetUnit->m_movementInfo.HasMovementFlag(MOVEMENTFLAG_ASCENDING))
            {
                targetUnit->m_movementInfo.RemoveMovementFlag(MOVEMENTFLAG_ASCENDING);
            }
            else
            {
                targetUnit->m_movementInfo.AddMovementFlag(MOVEMENTFLAG_ASCENDING);
            }
        }
    }
    else if (commandName == "turn")
    {
        if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmCommander->GetGUID())
                    {
                        HandleChatCommand(pmCommander, pmContent, member);
                    }
                }
            }
        }
        else if (pmTargetPlayer)
        {
            Player* targetPlayer = pmTargetPlayer;
            if (!targetPlayer)
            {
                targetPlayer = pmCommander;
            }
            if (commandVector.size() > 1)
            {
                std::string emoteContents = commandVector.at(1);
                int emoteNumber = atoi(emoteContents.c_str());
                targetPlayer->HandleEmoteCommand(emoteNumber);
            }
        }
        else
        {
            if (commandVector.size() > 1)
            {
                std::string turnContents = commandVector.at(1);
                float turnAngle = M_PI / 8;
                if (turnContents == "left")
                {
                    turnAngle = M_PI / 8;
                }
                else
                {
                    turnAngle = -M_PI / 8;
                }
                float destX = 0.0f, destY = 0.0f, destZ = 0.0f;
                pmCommander->GetNearPoint(nullptr, destX, destY, destZ, 0.0f, 0.05f, pmCommander->GetOrientation() + turnAngle + M_PI);
                float angle = pmCommander->GetOrientation() + turnAngle;
                Movement::MoveSplineInit init(pmCommander);
                init.MoveTo(destX, destY, destZ, false);
                init.SetFacing(angle);
                init.SetOrientationInversed();
                init.Launch();
            }
        }
    }
    else if (commandName == "arrangement")
    {
        std::ostringstream replyStream;
        if (pmTargetGroup)
        {
            if (pmTargetGroup->GetLeaderGUID() == pmCommander->GetGUID())
            {
                ObjectGuid ogTank = ObjectGuid();
                ObjectGuid ogHealer = ObjectGuid();
                int rti = 0;
                for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
                {
                    if (Player* member = groupRef->GetSource())
                    {
                        if (member->GetGUID() == pmCommander->GetGUID())
                        {
                            continue;
                        }
                        uint32 mapId = member->GetMapId();
                        if (mapId == StrategyIndex::StrategyIndex_The_Underbog)
                        {
                            member->activeStrategyIndex = StrategyIndex::StrategyIndex_The_Underbog;
                        }
                        else if (mapId == 269)
                        {
                            member->activeStrategyIndex = StrategyIndex::StrategyIndex_The_Black_Morass;
                        }
                        else if (mapId == 585)
                        {
                            member->activeStrategyIndex = 585;
                        }
                        else if (mapId == 601)
                        {
                            member->activeStrategyIndex = 601;
                        }
                        if (NierStrategy_Base* ns = member->nierStrategyMap[member->activeStrategyIndex])
                        {
                            ns->Reset();
                            member->groupRole = GroupRole::GroupRole_DPS;
                            switch (member->getClass())
                            {
                            case Classes::CLASS_WARRIOR:
                            {
                                member->groupRole = GroupRole::GroupRole_Tank;
                                ogTank = member->GetGUID();
                                break;
                            }
                            case Classes::CLASS_HUNTER:
                            {
                                break;
                            }
                            case Classes::CLASS_MAGE:
                            {
                                ns->rti = rti;
                                rti++;
                                break;
                            }
                            case Classes::CLASS_DRUID:
                            {
                                ns->rti = rti;
                                rti++;
                                break;
                            }
                            case Classes::CLASS_ROGUE:
                            {
                                ns->forceBack = true;
                                break;
                            }
                            case Classes::CLASS_PRIEST:
                            {
                                if (member->nierAction->specialty == 0)
                                {
                                    member->groupRole = GroupRole::GroupRole_Tank;
                                    ogTank = member->GetGUID();
                                }
                                else if (member->nierAction->specialty == 1)
                                {
                                    member->groupRole = GroupRole::GroupRole_Healer;
                                    ogHealer = member->GetGUID();
                                }
                                if (member->nierAction->specialty == 2)
                                {
                                    member->groupRole = GroupRole::GroupRole_DPS;
                                }
                                break;
                            }
                            default:
                            {
                                break;
                            }
                            }
                        }
                        member->nierAction->Prepare();
                    }
                }
                for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
                {
                    if (Player* member = groupRef->GetSource())
                    {
                        if (member->GetGUID() == pmCommander->GetGUID())
                        {
                            continue;
                        }
                        if (NierStrategy_Base* ns = member->nierStrategyMap[member->activeStrategyIndex])
                        {
                            ns->ogTank = ogTank;
                            ns->ogHealer = ogHealer;
                            ns->Report();
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
        sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, replyStream.str().c_str(), pmCommander);
    }
    else if (commandName == "instance")
    {
        std::ostringstream replyStream;
        if (pmTargetPlayer)
        {
            if (pmTargetPlayer->GetSession()->isNier)
            {
                for (uint8 i = 0; i < MAX_DIFFICULTY; ++i)
                {
                    BoundInstancesMap const& m_boundInstances = sInstanceSaveMgr->PlayerGetBoundInstances(pmTargetPlayer->GetGUID(), Difficulty(i));
                    for (BoundInstancesMap::const_iterator itr = m_boundInstances.begin(); itr != m_boundInstances.end(); itr++)
                    {
                        InstanceSave const* save = itr->second.save;
                        uint32 resetTime = itr->second.extended ? save->GetExtendedResetTime() : save->GetResetTime();
                        uint32 ttr = (resetTime >= GameTime::GetGameTime().count() ? resetTime - GameTime::GetGameTime().count() : 0);
                        sInstanceSaveMgr->PlayerUnbindInstance(pmTargetPlayer->GetGUID(), itr->first, Difficulty(i), true, pmTargetPlayer);
                        itr = m_boundInstances.begin();
                    }
                }
                replyStream << "Instance unbond";
                pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmCommander);
            }
        }
        else if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmCommander->GetGUID())
                    {
                        HandleChatCommand(pmCommander, pmContent, member, pmTargetGroup);
                    }
                }
            }
        }
    }
    else if (commandName == "updateentry")
    {
        if (Unit* target = pmCommander->GetSelectedUnit())
        {
            if (Creature* targetC = (Creature*)target)
            {
                if (commandVector.size() > 1)
                {
                    std::string checkEntry = commandVector.at(1);
                    uint32 entry = atoi(checkEntry.c_str());
                    targetC->UpdateEntry(entry);
                }
            }
        }
    }
    else if (commandName == "vimgol")
    {
        if (pmTargetGroup)
        {
            std::list<Creature*> visualList;
            pmCommander->GetCreatureListWithEntryInGrid(visualList, 23040, VISIBILITY_DISTANCE_NORMAL);
            std::unordered_set<ObjectGuid> ogGoingSet;
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                if (Player* member = groupRef->GetSource())
                {
                    if (member->GetGUID() != pmCommander->GetGUID())
                    {
                        for (auto& eachVisual : visualList)
                        {
                            if (ogGoingSet.find(eachVisual->GetGUID()) == ogGoingSet.end())
                            {
                                if (NierStrategy_Base* ns = member->nierStrategyMap[member->activeStrategyIndex])
                                {
                                    ns->basicStrategyType = BasicStrategyType::BasicStrategyType_Hold;
                                    ns->restLimit = 0;
                                    ns->actionType = ActionType::ActionType_Move;
                                    ns->actionLimit = 30000;
                                    member->nierAction->nm->Point(eachVisual->GetPosition(), ns->actionLimit);
                                    //std::ostringstream replyStream;
                                    //replyStream << "Goting to visual : " << eachVisual->GetGUID().GetCounter();
                                    //member->Say(replyStream.str(), Language::LANG_UNIVERSAL);
                                    ogGoingSet.insert(eachVisual->GetGUID());
                                }
                                break;
                            }
                        }
                    }
                }
            }
        }
        else
        {
            HandleChatCommand(pmCommander, pmContent, pmCommander, pmTargetGroup);
        }
    }
    else if (commandName == "distance")
    {
        if (pmTargetPlayer)
        {
            if (Unit* distanceTarget = pmCommander->GetSelectedUnit())
            {
                float distance = pmTargetPlayer->GetDistance(distanceTarget);
                std::ostringstream replyStream0;
                replyStream0 << "base : " << distance;
                pmTargetPlayer->Say(replyStream0.str(), Language::LANG_UNIVERSAL);
            }
            else
            {
                std::ostringstream replyStream;
                replyStream << "No target";
                pmTargetPlayer->Say(replyStream.str(), Language::LANG_UNIVERSAL);
            }
        }
        else if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmCommander->GetGUID())
                    {
                        HandleChatCommand(pmCommander, pmContent, member, pmTargetGroup);
                    }
                }
            }
        }
        else
        {
            HandleChatCommand(pmCommander, pmContent, pmCommander, pmTargetGroup);
        }
    }
    else if (commandName == "apexis")
    {
        std::ostringstream replyStream;
        if (pmTargetPlayer)
        {
            if (commandVector.size() > 1)
            {
                std::string color = commandVector.at(1);
                uint32 entry = 0;
                if (color == "red")
                {
                    entry = 185794;
                }
                else if (color == "yellow")
                {
                    entry = 185792;
                }
                else if (color == "blue")
                {
                    entry = 185795;
                }
                else if (color == "green")
                {
                    entry = 185793;
                }
                if (entry > 0)
                {
                    if (GameObject* pGo = pmTargetPlayer->FindNearestGameObject(entry, INTERACTION_DISTANCE))
                    {
                        pGo->Use(pmTargetPlayer);
                        replyStream << "Use GO : " << pGo->GetGUID().GetCounter();
                    }
                }
                else
                {
                    replyStream << "No color";
                }
            }
            else
            {
                replyStream << "No color";
            }
            pmTargetPlayer->Say(replyStream.str(), Language::LANG_UNIVERSAL);
        }
        else if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmCommander->GetGUID())
                    {
                        HandleChatCommand(pmCommander, pmContent, member, pmTargetGroup);
                    }
                }
            }
        }
        else
        {
            HandleChatCommand(pmCommander, pmContent, pmCommander);
        }
    }
    else if (commandName == "display")
    {
        if (Unit* target = pmCommander->GetSelectedUnit())
        {
            if (commandVector.size() > 1)
            {
                std::string idStr = commandVector.at(1);
                uint32 displayId = atoi(idStr.c_str());
                target->SetDisplayId(displayId);
            }
        }
    }
    else if (commandName == "ndisplay")
    {
        if (Unit* target = pmCommander->GetSelectedUnit())
        {
            if (commandVector.size() > 1)
            {
                std::string idStr = commandVector.at(1);
                uint32 displayId = atoi(idStr.c_str());
                target->SetNativeDisplayId(displayId);
            }
        }
    }
    else if (commandName == "vip")
    {
        if (pmTargetPlayer)
        {
            if (NierStrategy_Base* ns = pmTargetPlayer->nierStrategyMap[pmTargetPlayer->activeStrategyIndex])
            {
                if (commandVector.size() > 1)
                {
                    std::string nameStr = commandVector.at(1);
                    CreatureTemplateContainer const* ctc = sObjectMgr->GetCreatureTemplates();
                    for (CreatureTemplateContainer::const_iterator itr = ctc->begin(); itr != ctc->end(); ++itr)
                    {
                        if (itr->second.Name == nameStr)
                        {
                            ns->vipEntry = itr->second.Entry;
                        }
                    }
                }
                std::ostringstream replyStream;
                replyStream << "vip - " << ns->vipEntry;
                pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmCommander);
            }
        }
        else if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmCommander->GetGUID())
                    {
                        HandleChatCommand(pmCommander, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "nearpoint")
    {
        std::ostringstream replyStream;
        if (pmTargetPlayer)
        {
            if (Unit* distanceTarget = pmCommander->GetSelectedUnit())
            {
                float distance = 10.0f;
                if (commandVector.size() > 1)
                {
                    std::string checkStr = commandVector.at(1);
                    distance = atof(checkStr.c_str());
                }
                pmTargetPlayer->nierAction->nm->Chase(distanceTarget, distance, 0.0f, false, false);
                replyStream << "chasing : " << distanceTarget->GetName();
                pmTargetPlayer->Say(replyStream.str(), Language::LANG_UNIVERSAL);
            }
            else
            {
                replyStream << "No target";
            }
            pmTargetPlayer->Say(replyStream.str(), Language::LANG_UNIVERSAL);
        }
        else if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmCommander->GetGUID())
                    {
                        HandleChatCommand(pmCommander, pmContent, member, pmTargetGroup);
                    }
                }
            }
        }
        else
        {
            HandleChatCommand(pmCommander, pmContent, pmCommander);
        }
    }
    else if (commandName == "assemble")
    {
        std::ostringstream replyStream;
        if (pmTargetPlayer)
        {
            if (pmTargetPlayer->isNier)
            {
                if (NierStrategy_Base* ns = pmTargetPlayer->nierStrategyMap[pmTargetPlayer->activeStrategyIndex])
                {
                    if (!pmTargetPlayer->IsAlive())
                    {
                        ns->assembleDelay = 120000;
                        replyStream << "Corpserun 120 seconds";
                    }
                    else
                    {
                        ns->assembleDelay = 60000;
                        replyStream << "Assemble in 60 seconds";
                    }
                    pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmCommander);
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
                    if (member->GetGUID() != pmCommander->GetGUID())
                    {
                        HandleChatCommand(pmCommander, pmContent, member, pmTargetGroup);
                    }
                }
            }
        }
    }
    else if (commandName == "leader")
    {
        if (pmTargetGroup)
        {
            if (pmTargetGroup->GetLeaderGUID() != pmCommander->GetGUID())
            {
                bool change = true;
                if (Player* leader = ObjectAccessor::FindPlayer(pmTargetGroup->GetLeaderGUID()))
                {
                    if (WorldSession* leaderSession = leader->GetSession())
                    {
                        if (!leaderSession->isNier)
                        {
                            change = false;
                        }
                    }
                }
                if (change)
                {
                    pmTargetGroup->ChangeLeader(pmCommander->GetGUID());
                }
                else
                {
                    sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, "Leader is valid", pmCommander);
                }
            }
            else
            {
                sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, "You are the leader", pmCommander);
            }
        }
        else
        {
            sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, "You are not in a group", pmCommander);
        }
    }
    else if (commandName == "debug")
    {
        if (commandVector.size() > 1)
        {
            std::string debugType = commandVector.at(1);
            if (debugType == "jingzhe1")
            {
                //pmCommander->StoreNewItemInBestSlots(6948, 1);
            }
        }
    }
    else if (commandName == "nier")
    {
        if (commandVector.size() > 1)
        {
            std::string nierAction = commandVector.at(1);
            if (nierAction == "offline")
            {
                std::ostringstream replyStream;
                replyStream << "All niers are going offline";
                LogoutNiers();
                sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, replyStream.str().c_str(), pmCommander);
            }
            else if (nierAction == "relocate")
            {
                std::unordered_map<uint32, WorldSession*> allSessions = sWorld->GetAllSessions();
                for (std::unordered_map<uint32, WorldSession*>::iterator wsIT = allSessions.begin(); wsIT != allSessions.end(); wsIT++)
                {
                    if (WorldSession* eachWS = wsIT->second)
                    {
                        if (eachWS->isNier)
                        {
                            if (Player* eachNier = eachWS->GetPlayer())
                            {
                                if (eachNier->IsInWorld())
                                {
                                    eachNier->TeleportTo(eachNier->m_homebindMapId, eachNier->m_homebindX, eachNier->m_homebindY, eachNier->m_homebindZ, 0);

                                    std::ostringstream replyTitleStream;
                                    replyTitleStream << "Teleport nier to homebind : " << eachNier->GetName();
                                    sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, replyTitleStream.str().c_str(), pmCommander);
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
            if (NierStrategy_Base* ns = pmTargetPlayer->nierStrategyMap[pmTargetPlayer->activeStrategyIndex])
            {
                if (ns->basicStrategyType == BasicStrategyType::BasicStrategyType_Freeze || ns->basicStrategyType == BasicStrategyType::BasicStrategyType_Glue)
                {
                    ns->basicStrategyType = BasicStrategyType::BasicStrategyType_Normal;
                }
                if (Unit* target = pmCommander->GetSelectedUnit())
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
                        pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmCommander);
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
                    if (member->GetGUID() != pmCommander->GetGUID())
                    {
                        HandleChatCommand(pmCommander, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "tank")
    {
        if (pmTargetPlayer)
        {
            if (NierStrategy_Base* ns = pmTargetPlayer->nierStrategyMap[pmTargetPlayer->activeStrategyIndex])
            {
                if (Unit* target = pmCommander->GetSelectedUnit())
                {
                    if (ns->basicStrategyType == BasicStrategyType::BasicStrategyType_Freeze || ns->basicStrategyType == BasicStrategyType::BasicStrategyType_Glue)
                    {
                        ns->basicStrategyType = BasicStrategyType::BasicStrategyType_Normal;
                    }
                    if (pmTargetPlayer->groupRole == GroupRole::GroupRole_Tank)
                    {
                        if (ns->DoTank(target))
                        {
                            if (Group* tankGroup = pmTargetPlayer->GetGroup())
                            {
                                tankGroup->SetTargetIcon(7, pmTargetPlayer->GetGUID(), target->GetGUID());
                                for (GroupReference* groupRef = tankGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
                                {
                                    if (Player* member = groupRef->GetSource())
                                    {
                                        if (member->GetGUID() != pmCommander->GetGUID())
                                        {
                                            if (member->GetSession()->isNier)
                                            {
                                                if (member->nierAction->ReadyTank(pmTargetPlayer))
                                                {
                                                    if (NierStrategy_Base* nsAction = member->nierStrategyMap[pmTargetPlayer->activeStrategyIndex])
                                                    {
                                                        nsAction->ogActionTarget = pmTargetPlayer->GetGUID();
                                                        nsAction->actionType = ActionType::ActionType_ReadyTank;
                                                        nsAction->actionLimit = 2000;
                                                    }
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
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
                            pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmCommander);
                        }
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
                    if (member->GetGUID() != pmCommander->GetGUID())
                    {
                        HandleChatCommand(pmCommander, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "hold")
    {
        if (pmTargetPlayer)
        {
            bool doAction = true;
            if (commandVector.size() > 1)
            {
                std::string role = commandVector.at(1);
                if (role == "tank")
                {
                    if (pmTargetPlayer->groupRole != GroupRole::GroupRole_Tank)
                    {
                        doAction = false;
                    }
                }
                else if (role == "healer")
                {
                    if (pmTargetPlayer->groupRole != GroupRole::GroupRole_Healer)
                    {
                        doAction = false;
                    }
                }
                else if (role == "dps")
                {
                    if (pmTargetPlayer->groupRole != GroupRole::GroupRole_DPS)
                    {
                        doAction = false;
                    }
                }
                else if (role == "melee")
                {
                    uint32 myClass = pmTargetPlayer->getClass();
                    if (myClass != Classes::CLASS_WARRIOR && myClass != Classes::CLASS_ROGUE && myClass != Classes::CLASS_PALADIN)
                    {
                        doAction = false;
                    }
                }
            }
            if (doAction)
            {
                if (NierStrategy_Base* ns = pmTargetPlayer->nierStrategyMap[pmTargetPlayer->activeStrategyIndex])
                {
                    ns->basicStrategyType = BasicStrategyType::BasicStrategyType_Hold;
                    pmTargetPlayer->StopMoving();
                    pmTargetPlayer->GetMotionMaster()->Clear();
                    std::ostringstream replyStream;
                    replyStream << "Holding";
                    pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmCommander);
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
                    if (member->GetGUID() != pmCommander->GetGUID())
                    {
                        HandleChatCommand(pmCommander, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "dev0")
    {
        if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmCommander->GetGUID())
                    {
                        member->SetSelection(ObjectGuid::Empty);
                        member->SetTarget(ObjectGuid::Empty);
                        member->CombatStop(true);
                        member->StopMoving();
                        member->GetMotionMaster()->Clear();
                        member->AttackStop();
                        member->InterruptNonMeleeSpells(true);
                        member->ResurrectPlayer(1.0f);
                        member->TeleportTo(pmCommander->GetMapId(), pmCommander->GetPositionX(), pmCommander->GetPositionY(), pmCommander->GetPositionZ(), pmCommander->GetOrientation());
                    }
                }
            }
        }
    }
    else if (commandName == "caution")
    {
        if (pmTargetPlayer)
        {
            if (NierStrategy_Base* ns = pmTargetPlayer->nierStrategyMap[pmTargetPlayer->activeStrategyIndex])
            {
                if (commandVector.size() > 1)
                {
                    std::string cautionTypeStr = commandVector.at(1);
                    if (commandVector.size() > 2)
                    {
                        std::ostringstream nameStream;
                        nameStream << commandVector.at(2);
                        for (int nameIndex = 3; nameIndex < commandVector.size(); nameIndex++)
                        {
                            nameStream << " " << commandVector.at(nameIndex);
                        }
                        std::string spellNameStr = nameStream.str();
                        if (cautionTypeStr == "add")
                        {
                            std::ostringstream queryStream;
                            queryStream << "SELECT Id FROM spell_template where SpellName = '" << spellNameStr << "'";
                            QueryResult spellQR = WorldDatabase.Query(queryStream.str().c_str());
                            if (spellQR)
                            {
                                do
                                {
                                    Field* fields = spellQR->Fetch();
                                    uint32 eachId = fields[0].Get<uint32>();
                                    if (ns->cautionSpellMap.find(spellNameStr) == ns->cautionSpellMap.end())
                                    {
                                        ns->cautionSpellMap[spellNameStr].insert(eachId);
                                    }
                                    else
                                    {
                                        std::unordered_set<uint32> eachIDSet = ns->cautionSpellMap.find(spellNameStr)->second;
                                        if (eachIDSet.find(eachId) == eachIDSet.end())
                                        {
                                            ns->cautionSpellMap[spellNameStr].insert(eachId);
                                        }
                                    }
                                } while (spellQR->NextRow());
                            }
                        }
                        else if (cautionTypeStr == "remove")
                        {
                            ns->cautionSpellMap.erase(spellNameStr);
                        }
                    }
                    else if (cautionTypeStr == "clear")
                    {
                        ns->cautionSpellMap.clear();
                    }
                }
                for (std::unordered_map<std::string, std::unordered_set<uint32>>::iterator nameIT = ns->cautionSpellMap.begin(); nameIT != ns->cautionSpellMap.end(); nameIT++)
                {
                    for (std::unordered_set<uint32>::iterator idIT = nameIT->second.begin(); idIT != nameIT->second.end(); idIT++)
                    {
                        std::ostringstream detailsStream;
                        detailsStream << "Caution : " << nameIT->first << " - " << *idIT;
                        pmTargetPlayer->Whisper(detailsStream.str(), Language::LANG_UNIVERSAL, pmCommander);
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
                    if (member->GetGUID() != pmCommander->GetGUID())
                    {
                        HandleChatCommand(pmCommander, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "freeze")
    {
        if (pmTargetPlayer)
        {
            if (NierStrategy_Base* ns = pmTargetPlayer->nierStrategyMap[pmTargetPlayer->activeStrategyIndex])
            {
                ns->basicStrategyType = BasicStrategyType::BasicStrategyType_Freeze;
                pmTargetPlayer->nierAction->nm->ResetMovement();
                pmTargetPlayer->InterruptNonMeleeSpells(true);
                pmTargetPlayer->AttackStop();
                pmTargetPlayer->nierAction->PetStop();
                std::ostringstream replyStream;
                replyStream << "Freezed";
                pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmCommander);
            }
        }
        else if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmCommander->GetGUID())
                    {
                        HandleChatCommand(pmCommander, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "prepare")
    {
        if (pmTargetPlayer)
        {
            pmTargetPlayer->nierAction->Prepare();
        }
        else if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmCommander->GetGUID())
                    {
                        HandleChatCommand(pmCommander, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "rest")
    {
        if (pmTargetPlayer)
        {
            if (NierStrategy_Base* ns = pmTargetPlayer->nierStrategyMap[pmTargetPlayer->activeStrategyIndex])
            {
                ns->Rest(true);
                pmTargetPlayer->Whisper("Resting", Language::LANG_UNIVERSAL, pmCommander);
            }
        }
        else if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmCommander->GetGUID())
                    {
                        HandleChatCommand(pmCommander, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "revive")
    {
        if (pmTargetPlayer)
        {
            if (NierStrategy_Base* ns = pmTargetPlayer->nierStrategyMap[pmTargetPlayer->activeStrategyIndex])
            {
                if (Unit* target = pmCommander->GetSelectedUnit())
                {
                    if (ns->Revive(target))
                    {
                        ns->actionLimit = DEFAULT_ACTION_LIMIT_DELAY;
                        ns->ogActionTarget = target->GetGUID();
                        ns->actionType = ActionType::ActionType_Revive;
                        std::ostringstream replyStream;
                        replyStream << "Try to do reviving";
                        pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmCommander);
                    }
                }
                else if (ns->Revive())
                {
                    std::ostringstream replyStream;
                    replyStream << "Try to do reviving";
                    pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmCommander);
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
                    if (member->GetGUID() != pmCommander->GetGUID())
                    {
                        HandleChatCommand(pmCommander, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "follow")
    {
        if (pmTargetPlayer)
        {
            if (NierStrategy_Base* ns = pmTargetPlayer->nierStrategyMap[pmTargetPlayer->activeStrategyIndex])
            {
                std::ostringstream replyStream;
                if (commandVector.size() > 1)
                {
                    std::string cmdDistanceStr = commandVector.at(1);
                    float cmdDistance = atof(cmdDistanceStr.c_str());
                    if (cmdDistance >= CONTACT_DISTANCE && cmdDistance <= NIER_MAX_DISTANCE)
                    {
                        ns->followDistance = cmdDistance;
                        replyStream << "Follow distance - " << ns->followDistance;
                    }
                    else
                    {
                        replyStream << "Distance is not valid";
                    }
                }
                else
                {
                    ns->basicStrategyType = BasicStrategyType::BasicStrategyType_Normal;
                    ns->restLimit = 0;
                    ns->actionLimit = 0;
                    replyStream << "Following - " << ns->followDistance;
                }
                pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmCommander);
            }
        }
        else if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmCommander->GetGUID())
                    {
                        HandleChatCommand(pmCommander, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "chase")
    {
        if (pmTargetPlayer)
        {
            if (NierStrategy_Base* ns = pmTargetPlayer->nierStrategyMap[pmTargetPlayer->activeStrategyIndex])
            {
                std::ostringstream replyStream;
                if (commandVector.size() > 1)
                {
                    std::string cmdDistanceStr = commandVector.at(1);
                    float cmdDistance = atof(cmdDistanceStr.c_str());
                    if (cmdDistance >= CONTACT_DISTANCE && cmdDistance <= NIER_MAX_DISTANCE)
                    {
                        ns->dpsDistance = cmdDistance;
                    }
                    else
                    {
                        replyStream << "Distance is not valid";
                    }
                }
                replyStream << "Chase distance - " << ns->dpsDistance;
                pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmCommander);
            }
        }
        else if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmCommander->GetGUID())
                    {
                        HandleChatCommand(pmCommander, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "gather")
    {
        if (pmTargetPlayer)
        {
            if (NierStrategy_Base* ns = pmTargetPlayer->nierStrategyMap[pmTargetPlayer->activeStrategyIndex])
            {
                std::ostringstream replyStream;
                if (pmTargetPlayer->IsAlive())
                {
                    if (pmCommander->GetDistance(pmTargetPlayer) < VISIBILITY_DISTANCE_LARGE)
                    {
                        ns->restLimit = 0;
                        ns->actionType = ActionType::ActionType_Move;
                        ns->actionLimit = 3 * IN_MILLISECONDS;
                        pmTargetPlayer->nierAction->nm->Point(pmCommander->GetPosition(), ns->actionLimit);
                        replyStream << "Moving to you";
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
                pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmCommander);
            }
        }
        else if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmCommander->GetGUID())
                    {
                        HandleChatCommand(pmCommander, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "move")
    {
        if (pmTargetPlayer)
        {
            std::ostringstream replyStream;
            if (pmTargetPlayer->IsAlive())
            {
                if (commandVector.size() > 1)
                {
                    bool doAction = true;
                    std::string direction = commandVector.at(1);
                    uint32 directionType = NierMovementDirection::NierMovementDirection_Forward;
                    if (direction == "forward")
                    {
                        directionType = NierMovementDirection::NierMovementDirection_Forward;
                    }
                    else if (direction == "back")
                    {
                        directionType = NierMovementDirection::NierMovementDirection_Back;
                    }
                    else if (direction == "left")
                    {
                        directionType = NierMovementDirection::NierMovementDirection_Left;
                    }
                    else if (direction == "right")
                    {
                        directionType = NierMovementDirection::NierMovementDirection_Right;
                    }
                    else
                    {
                        doAction = false;
                        replyStream << "Direction invalid";
                    }
                    if (doAction)
                    {
                        if (commandVector.size() > 2)
                        {
                            std::string role = commandVector.at(2);
                            if (role == "all")
                            {
                                doAction = true;
                            }
                            else if (role == "tank")
                            {
                                if (pmTargetPlayer->groupRole != GroupRole::GroupRole_Tank)
                                {
                                    doAction = false;
                                }
                            }
                            else if (role == "healer")
                            {
                                if (pmTargetPlayer->groupRole != GroupRole::GroupRole_Healer)
                                {
                                    doAction = false;
                                }
                            }
                            else if (role == "dps")
                            {
                                if (pmTargetPlayer->groupRole != GroupRole::GroupRole_DPS)
                                {
                                    doAction = false;
                                }
                            }
                            else if (role == "melee")
                            {
                                uint32 myClass = pmTargetPlayer->getClass();
                                if (myClass != Classes::CLASS_WARRIOR && myClass != Classes::CLASS_ROGUE && myClass != Classes::CLASS_PALADIN)
                                {
                                    doAction = false;
                                }
                            }
                            else
                            {
                                doAction = false;
                                replyStream << "Role invalid";
                            }
                        }
                        else
                        {
                            doAction = false;
                            replyStream << "No role";
                        }
                        if (doAction)
                        {
                            if (NierStrategy_Base* ns = pmTargetPlayer->nierStrategyMap[pmTargetPlayer->activeStrategyIndex])
                            {
                                uint32 distance = 5;
                                if (commandVector.size() > 3)
                                {
                                    std::string distanceStr = commandVector.at(3);
                                    distance = std::atoi(distanceStr.c_str());
                                }
                                uint32 actionLimit = distance / 5;
                                actionLimit = actionLimit * IN_MILLISECONDS;
                                if (pmTargetPlayer->nierAction->nm->Direction(pmCommander, directionType, actionLimit, distance))
                                {
                                    ns->basicStrategyType = BasicStrategyType::BasicStrategyType_Hold;
                                    ns->restLimit = 0;
                                    ns->actionType = ActionType::ActionType_Move;
                                    ns->actionLimit = actionLimit;
                                    replyStream << direction;
                                }
                                pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmCommander);
                            }
                        }
                    }
                }
                else
                {
                    replyStream << "No direction";
                }
            }
            else
            {
                replyStream << "I am dead";
            }
        }
        else if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmCommander->GetGUID())
                    {
                        HandleChatCommand(pmCommander, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "cast")
    {
        if (pmTargetPlayer)
        {
            std::ostringstream replyStream;
            if (Unit* targetU = pmCommander->GetSelectedUnit())
            {
                if (commandVector.size() > 1)
                {
                    uint32 spellId = std::stoi(commandVector.at(1));
                    if (pmTargetPlayer->HasActiveSpell(spellId))
                    {
                        pmTargetPlayer->InterruptNonMeleeSpells(true);
                        SpellCastResult scr = pmTargetPlayer->CastSpell(targetU, spellId, TriggerCastFlags::TRIGGERED_NONE);
                        replyStream << "Cast spell : " << scr;
                    }
                }
                else
                {
                    replyStream << "Spell name missing";
                }
            }
            else
            {
                replyStream << "No target";
            }
            pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmCommander);
        }
        else if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmCommander->GetGUID())
                    {
                        HandleChatCommand(pmCommander, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "unaura")
    {
        if (Unit* targetU = pmCommander->GetSelectedUnit())
        {
            if (commandVector.size() > 1)
            {
                uint32 spellId = std::stoi(commandVector.at(1));
                targetU->RemoveAurasDueToSpell(spellId);
            }
        }
    }
    else if (commandName == "aura")
    {
        std::ostringstream replyStream;
        if (Unit* targetU = pmCommander->GetSelectedUnit())
        {
            if (commandVector.size() > 1)
            {
                uint32 spellId = std::stoi(commandVector.at(1));
                if (targetU->HasAura(spellId))
                {
                    replyStream << "Yes";
                }
                else
                {
                    replyStream << "No";
                }
            }
            else
            {
                replyStream << "No id";
            }
        }
        else
        {
            replyStream << "No target";
        }
        sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, replyStream.str().c_str(), pmCommander);
    }
    else if (commandName == "tcast")
    {
        std::ostringstream replyStream;
        if (Unit* targetU = pmCommander->GetSelectedUnit())
        {
            if (commandVector.size() > 1)
            {
                uint32 spellId = std::stoi(commandVector.at(1));
                SpellCastResult scr = targetU->CastSpell(targetU, spellId, TriggerCastFlags::TRIGGERED_NONE);
                replyStream << "Cast result : " << scr;
            }
            else
            {
                replyStream << "No spell";
            }
        }
        else
        {
            replyStream << "No t";
        }
        sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, replyStream.str().c_str(), pmCommander);
    }
    else if (commandName == "castt")
    {
        std::ostringstream replyStream;
        if (Unit* targetU = pmCommander->GetSelectedUnit())
        {
            if (commandVector.size() > 1)
            {
                uint32 spellId = std::stoi(commandVector.at(1));
                SpellCastResult scr = pmCommander->CastSpell(targetU, spellId, TriggerCastFlags::TRIGGERED_NONE);
                replyStream << "Cast result : " << scr;
            }
            else
            {
                replyStream << "No spell";
            }
        }
        else
        {
            replyStream << "No t";
        }
        sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, replyStream.str().c_str(), pmCommander);
    }
    else if (commandName == "delay")
    {
        if (pmTargetPlayer)
        {
            if (NierStrategy_Base* ns = pmTargetPlayer->nierStrategyMap[pmTargetPlayer->activeStrategyIndex])
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
                pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmCommander);
            }
        }
        else if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmCommander->GetGUID())
                    {
                        HandleChatCommand(pmCommander, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "cure")
    {
        if (pmTargetPlayer)
        {
            if (NierStrategy_Base* ns = pmTargetPlayer->nierStrategyMap[pmTargetPlayer->activeStrategyIndex])
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
                pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmCommander);
            }
        }
        else if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmCommander->GetGUID())
                    {
                        HandleChatCommand(pmCommander, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "petting")
    {
        if (pmTargetPlayer)
        {
            if (NierStrategy_Base* ns = pmTargetPlayer->nierStrategyMap[pmTargetPlayer->activeStrategyIndex])
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
                            if (NierAction_Hunter* naHunter = (NierAction_Hunter*)pmTargetPlayer->nierAction)
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
                pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmCommander);
            }
        }
        else if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmCommander->GetGUID())
                    {
                        HandleChatCommand(pmCommander, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "aoe")
    {
        if (pmTargetPlayer)
        {
            if (NierStrategy_Base* ns = pmTargetPlayer->nierStrategyMap[pmTargetPlayer->activeStrategyIndex])
            {
                std::ostringstream replyStream;
                if (commandVector.size() > 1)
                {
                    std::string roleCMD = commandVector.at(1);
                    bool onoff = false;
                    if (commandVector.size() > 2)
                    {
                        std::string switchCMD = commandVector.at(2);
                        if (switchCMD == "on")
                        {
                            onoff = true;
                        }
                        if (roleCMD == "tank")
                        {
                            if (pmTargetPlayer->groupRole == GroupRole::GroupRole_Tank)
                            {
                                ns->aoe = onoff;
                            }
                        }
                        else if (roleCMD == "dps")
                        {
                            if (pmTargetPlayer->groupRole == GroupRole::GroupRole_DPS)
                            {
                                ns->aoe = onoff;
                            }
                        }
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
                pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmCommander);
            }
        }
        else if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmCommander->GetGUID())
                    {
                        HandleChatCommand(pmCommander, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "rti")
    {
        if (pmTargetPlayer)
        {
            if (NierStrategy_Base* ns = pmTargetPlayer->nierStrategyMap[pmTargetPlayer->activeStrategyIndex])
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
                    ns->rti = targetIcon;
                }
                replyStream << "RTI is " << ns->rti;
                pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmCommander);
            }
        }
        else if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmCommander->GetGUID())
                    {
                        HandleChatCommand(pmCommander, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "portal")
    {
        if (pmTargetPlayer)
        {
            if (pmTargetPlayer->getClass() == Classes::CLASS_MAGE)
            {
                if (pmTargetPlayer->GetSession()->isNier)
                {
                    if (commandVector.size() > 1)
                    {
                        std::string city = commandVector.at(1);
                        if (NierAction_Mage* nam = (NierAction_Mage*)pmTargetPlayer->nierAction)
                        {
                            uint32 portalSpell = 0;
                            if (city == "Dalaran")
                            {
                                portalSpell = nam->spell_Portal_Dalaran;
                            }
                            else
                            {
                                uint32 playerRace = pmTargetPlayer->getRace();
                                if (playerRace == Races::RACE_DRAENEI || playerRace == Races::RACE_DWARF || playerRace == Races::RACE_GNOME || playerRace == Races::RACE_HUMAN || playerRace == Races::RACE_NIGHTELF)
                                {
                                    if (city == "exodar")
                                    {
                                        portalSpell = nam->spell_Portal_Exodar;
                                    }
                                    else if (city == "stormwind")
                                    {
                                        portalSpell = nam->spell_Portal_Stormwind;
                                    }
                                    else if (city == "darnassus")
                                    {
                                        portalSpell = nam->spell_Portal_Darnassus;
                                    }
                                    else if (city == "ironforge")
                                    {
                                        portalSpell = nam->spell_Portal_Ironforge;
                                    }
                                    else if (city == "shattrath")
                                    {
                                        portalSpell = nam->spell_Portal_Shattrath_A;
                                    }
                                }
                                else
                                {
                                    if (city == "orgrimmar")
                                    {
                                        portalSpell = nam->spell_Portal_Orgrimmar;
                                    }
                                    else if (city == "thunderbluff")
                                    {
                                        portalSpell = nam->spell_Portal_ThunderBluff;
                                    }
                                    else if (city == "silvermoon")
                                    {
                                        portalSpell = nam->spell_Portal_Silvermoon;
                                    }
                                    else if (city == "undercity")
                                    {
                                        portalSpell = nam->spell_Portal_Shattrath_H;
                                    }
                                }
                            }
                            if (portalSpell > 0)
                            {
                                std::ostringstream replyStream;
                                replyStream << "Portal to " << city;
                                pmTargetPlayer->Say(replyStream.str(), Language::LANG_UNIVERSAL);
                                nam->CastSpell(pmTargetPlayer, portalSpell);
                            }
                        }
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
                    if (member->GetGUID() != pmCommander->GetGUID())
                    {
                        HandleChatCommand(pmCommander, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "home")
    {
        if (pmTargetPlayer)
        {
            if (pmTargetPlayer->GetSession()->isNier)
            {
                pmTargetPlayer->Whisper("Going home", Language::LANG_UNIVERSAL, pmCommander);
                pmTargetPlayer->TeleportTo(pmTargetPlayer->m_homebindMapId, pmTargetPlayer->m_homebindX, pmTargetPlayer->m_homebindY, pmTargetPlayer->m_homebindZ, 0);
            }
        }
        else if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmCommander->GetGUID())
                    {
                        HandleChatCommand(pmCommander, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "instant")
    {
        if (pmTargetPlayer)
        {
            if (NierStrategy_Base* ns = pmTargetPlayer->nierStrategyMap[pmTargetPlayer->activeStrategyIndex])
            {
                std::ostringstream replyStream;
                if (commandVector.size() > 1)
                {
                    std::string instantType = commandVector.at(1);
                    if (instantType == "on")
                    {
                        ns->instantOnly = true;
                    }
                    else
                    {
                        ns->instantOnly = false;
                    }
                }
                replyStream << "Instant type is : ";
                if (ns->instantOnly)
                {
                    replyStream << "on";
                }
                else
                {
                    replyStream << "off";
                }
                pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmCommander);
            }
        }
        else if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmCommander->GetGUID())
                    {
                        HandleChatCommand(pmCommander, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "glue")
    {
        if (pmTargetPlayer)
        {
            if (NierStrategy_Base* ns = pmTargetPlayer->nierStrategyMap[pmTargetPlayer->activeStrategyIndex])
            {
                std::ostringstream replyStream;
                ns->basicStrategyType = BasicStrategyType::BasicStrategyType_Glue;
                pmTargetPlayer->nierAction->nm->ResetMovement();
                replyStream << "bst : " << ns->basicStrategyType;
                pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmCommander);
            }
        }
        else if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmCommander->GetGUID())
                    {
                        HandleChatCommand(pmCommander, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "innervate")
    {
        if (pmTargetPlayer)
        {
            if (pmTargetPlayer->getClass() == Classes::CLASS_DRUID)
            {
                if (pmTargetPlayer->GetSession()->isNier)
                {
                    if (NierAction_Druid* nad = (NierAction_Druid*)pmTargetPlayer->nierAction)
                    {
                        if (nad->spell_Innervate > 0)
                        {
                            if (commandVector.size() > 1)
                            {
                                std::string roleName = commandVector.at(1);
                                if (roleName == "self")
                                {
                                    //if (nad->spell_MoonkinForm > 0)
                                    //{
                                    //    nad->CancelAura(nad->spell_MoonkinForm);
                                    //}
                                    if (nad->CastSpell(pmTargetPlayer, nad->spell_Innervate, true, false, true))
                                    {
                                        std::ostringstream replyStream;
                                        replyStream << "Innervate - " << pmTargetPlayer->GetName();
                                        pmTargetPlayer->Yell(replyStream.str(), Language::LANG_UNIVERSAL);
                                    }
                                }
                                else if (roleName == "healer")
                                {
                                    if (Group* targetGroup = pmTargetPlayer->GetGroup())
                                    {
                                        for (GroupReference* groupRef = targetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
                                        {
                                            if (Player* member = groupRef->GetSource())
                                            {
                                                if (member->groupRole == GroupRole::GroupRole_Healer)
                                                {
                                                    if (member->IsAlive())
                                                    {
                                                        if (pmTargetPlayer->GetDistance(member) < NIER_FAR_DISTANCE)
                                                        {
                                                            //if (nad->spell_MoonkinForm > 0)
                                                            //{
                                                            //    nad->CancelAura(nad->spell_MoonkinForm);
                                                            //}
                                                            if (nad->CastSpell(member, nad->spell_Innervate, true, false, true))
                                                            {
                                                                std::ostringstream replyStream;
                                                                replyStream << "Innervate - " << member->GetName();
                                                                pmTargetPlayer->Yell(replyStream.str(), Language::LANG_UNIVERSAL);
                                                                break;
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
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
                    if (member->GetGUID() != pmCommander->GetGUID())
                    {
                        HandleChatCommand(pmCommander, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "assist")
    {
        if (pmTargetPlayer)
        {
            if (NierStrategy_Base* ns = pmTargetPlayer->nierStrategyMap[pmTargetPlayer->activeStrategyIndex])
            {
                if (ns->rti >= 0)
                {
                    if (Group* targetGroup = pmTargetPlayer->GetGroup())
                    {
                        ObjectGuid ogRTI = targetGroup->GetGuidByTargetIcon(ns->rti);
                        Unit* rtiTarget = nullptr;
                        if (!ogRTI.IsEmpty())
                        {
                            rtiTarget = ObjectAccessor::GetUnit(*pmTargetPlayer, ogRTI);
                        }
                        else
                        {
                            if (pmTargetPlayer->IsInCombat())
                            {
                                bool marked = false;
                                std::unordered_set<Unit*> checkAttackers = pmTargetPlayer->getAttackers();
                                for (std::unordered_set<Unit*>::iterator ait = checkAttackers.begin(); ait != checkAttackers.end(); ++ait)
                                {
                                    if (Unit* eachAttacker = *ait)
                                    {
                                        if (pmTargetPlayer->IsValidAttackTarget(eachAttacker))
                                        {
                                            if (targetGroup->GetTargetIconByGuid(eachAttacker->GetGUID()) < 0)
                                            {
                                                if (pmTargetPlayer->nierAction->Mark(eachAttacker, ns->rti))
                                                {
                                                    marked = true;
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                }
                                if (!marked)
                                {
                                    std::list<Creature*> creatureList;
                                    pmTargetPlayer->GetCreatureListWithEntryInGrid(creatureList, 0, VISIBILITY_DISTANCE_NORMAL);
                                    if (!creatureList.empty())
                                    {
                                        for (std::list<Creature*>::iterator itr = creatureList.begin(); itr != creatureList.end(); ++itr)
                                        {
                                            if (Creature* hostileCreature = *itr)
                                            {
                                                if (pmTargetPlayer->IsValidAttackTarget(hostileCreature))
                                                {
                                                    if (targetGroup->GetTargetIconByGuid(hostileCreature->GetGUID()) < 0)
                                                    {
                                                        if (hostileCreature->IsInCombat())
                                                        {
                                                            if (pmTargetPlayer->nierAction->Mark(hostileCreature, ns->rti))
                                                            {
                                                                marked = true;
                                                                break;
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        if (rtiTarget)
                        {
                            pmTargetPlayer->nierAction->Assist(ns->rti);
                        }
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
                    if (member->GetGUID() != pmCommander->GetGUID())
                    {
                        HandleChatCommand(pmCommander, pmContent, member);
                    }
                }
            }
        }
    }
    else if (commandName == "equip")
    {
        if (pmTargetPlayer)
        {
            if (pmTargetPlayer->isNier)
            {
                std::ostringstream replyStream;
                if (commandVector.size() > 1)
                {
                    uint32 entry = 0;
                    std::string entryStr = commandVector.at(2);
                    entry = std::atoi(entryStr.c_str());
                    if (entry > 0)
                    {
                        std::ostringstream queryStream;
                        queryStream << "SELECT entry FROM item_template where entry = " << entry << "";
                        QueryResult itemQR = WorldDatabase.Query(queryStream.str().c_str());
                        if (itemQR)
                        {
                            do
                            {
                                Field* fields = itemQR->Fetch();
                                uint32 entry = fields[0].Get<uint32>();
                                if (const ItemTemplate* proto = sObjectMgr->GetItemTemplate(entry))
                                {
                                    if (Item* pItem = Item::CreateItem(entry, 1))
                                    {
                                        uint32 equipSlot = 0;
                                        switch (proto->InventoryType)
                                        {
                                        case INVTYPE_HEAD:
                                        {
                                            equipSlot = EQUIPMENT_SLOT_HEAD;
                                            break;
                                        }
                                        case INVTYPE_NECK:
                                        {
                                            equipSlot = EQUIPMENT_SLOT_NECK;
                                            break;
                                        }
                                        case INVTYPE_SHOULDERS:
                                        {
                                            equipSlot = EQUIPMENT_SLOT_SHOULDERS;
                                            break;
                                        }
                                        case INVTYPE_BODY:
                                        {
                                            equipSlot = EQUIPMENT_SLOT_BODY;
                                            break;
                                        }
                                        case INVTYPE_CHEST:
                                        {
                                            equipSlot = EQUIPMENT_SLOT_CHEST;
                                            break;
                                        }
                                        case INVTYPE_ROBE:
                                        {
                                            equipSlot = EQUIPMENT_SLOT_CHEST;
                                            break;
                                        }
                                        case INVTYPE_WAIST:
                                        {
                                            equipSlot = EQUIPMENT_SLOT_WAIST;
                                            break;
                                        }
                                        case INVTYPE_LEGS:
                                        {
                                            equipSlot = EQUIPMENT_SLOT_LEGS;
                                            break;
                                        }
                                        case INVTYPE_FEET:
                                        {
                                            equipSlot = EQUIPMENT_SLOT_FEET;
                                            break;
                                        }
                                        case INVTYPE_WRISTS:
                                        {
                                            equipSlot = EQUIPMENT_SLOT_WRISTS;
                                            break;
                                        }
                                        case INVTYPE_HANDS:
                                        {
                                            equipSlot = EQUIPMENT_SLOT_HANDS;
                                            break;
                                        }
                                        case INVTYPE_FINGER:
                                        {
                                            equipSlot = EQUIPMENT_SLOT_FINGER1;
                                            //viable_slots[1] = EQUIPMENT_SLOT_FINGER2;
                                            break;
                                        }
                                        case INVTYPE_TRINKET:
                                        {
                                            equipSlot = EQUIPMENT_SLOT_TRINKET1;
                                            //viable_slots[1] = EQUIPMENT_SLOT_TRINKET2;
                                            break;
                                        }
                                        case INVTYPE_CLOAK:
                                        {
                                            equipSlot = EQUIPMENT_SLOT_BACK;
                                            break;
                                        }
                                        case INVTYPE_WEAPON:
                                        {
                                            equipSlot = EQUIPMENT_SLOT_MAINHAND;
                                            //if (CanDualWield())
                                            //{										
                                            //	viable_slots[1] = EQUIPMENT_SLOT_OFFHAND;
                                            //}
                                            break;
                                        }
                                        case INVTYPE_SHIELD:
                                        {
                                            equipSlot = EQUIPMENT_SLOT_OFFHAND;
                                            break;
                                        }
                                        case INVTYPE_RANGED:
                                        {
                                            equipSlot = EQUIPMENT_SLOT_RANGED;
                                            break;
                                        }
                                        case INVTYPE_2HWEAPON:
                                        {
                                            equipSlot = EQUIPMENT_SLOT_MAINHAND;
                                            //if (CanDualWield() && CanTitanGrip())
                                            //{										
                                            //	viable_slots[1] = EQUIPMENT_SLOT_OFFHAND;
                                            //}
                                            break;
                                        }
                                        case INVTYPE_TABARD:
                                        {
                                            equipSlot = EQUIPMENT_SLOT_TABARD;
                                            break;
                                        }
                                        case INVTYPE_WEAPONMAINHAND:
                                        {
                                            equipSlot = EQUIPMENT_SLOT_MAINHAND;
                                            break;
                                        }
                                        case INVTYPE_WEAPONOFFHAND:
                                        {
                                            equipSlot = EQUIPMENT_SLOT_OFFHAND;
                                            break;
                                        }
                                        case INVTYPE_HOLDABLE:
                                        {
                                            equipSlot = EQUIPMENT_SLOT_OFFHAND;
                                            break;
                                        }
                                        case INVTYPE_THROWN:
                                        {
                                            equipSlot = EQUIPMENT_SLOT_RANGED;
                                            break;
                                        }
                                        case INVTYPE_RANGEDRIGHT:
                                        {
                                            equipSlot = EQUIPMENT_SLOT_RANGED;
                                            break;
                                        }
                                        case INVTYPE_BAG:
                                        {
                                            equipSlot = INVENTORY_SLOT_BAG_START + 0;
                                            //viable_slots[1] = INVENTORY_SLOT_BAG_START + 1;
                                            //viable_slots[2] = INVENTORY_SLOT_BAG_START + 2;
                                            //viable_slots[3] = INVENTORY_SLOT_BAG_START + 3;
                                            break;
                                        }
                                        case INVTYPE_RELIC:
                                        {
                                            //pClass = getClass();
                                            //if (pClass)
                                            //{
                                            //	switch (proto->SubClass)
                                            //	{
                                            //	case ITEM_SUBCLASS_ARMOR_LIBRAM:											
                                            //		if (pClass == CLASS_PALADIN)
                                            //		{
                                            //			equipSlot = EQUIPMENT_SLOT_RANGED;
                                            //		}
                                            //		break;
                                            //	case ITEM_SUBCLASS_ARMOR_IDOL:											
                                            //		if (pClass == CLASS_DRUID)
                                            //		{
                                            //			equipSlot = EQUIPMENT_SLOT_RANGED;
                                            //		}
                                            //		break;
                                            //	case ITEM_SUBCLASS_ARMOR_TOTEM:											
                                            //		if (pClass == CLASS_SHAMAN)
                                            //		{
                                            //			equipSlot = EQUIPMENT_SLOT_RANGED;
                                            //		}
                                            //		break;
                                            //	case ITEM_SUBCLASS_ARMOR_MISC:											
                                            //		if (pClass == CLASS_WARLOCK)
                                            //		{
                                            //			equipSlot = EQUIPMENT_SLOT_RANGED;
                                            //		}
                                            //		break;
                                            //	case ITEM_SUBCLASS_ARMOR_SIGIL:											
                                            //		if (pClass == CLASS_DEATH_KNIGHT)
                                            //		{
                                            //			equipSlot = EQUIPMENT_SLOT_RANGED;
                                            //		}
                                            //		break;
                                            //	default:											
                                            //		break;
                                            //	}
                                            //}
                                            break;
                                        }
                                        default:
                                        {
                                            break;
                                        }
                                        }
                                        if (Item* currentEquip = pmTargetPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, equipSlot))
                                        {
                                            if (const ItemTemplate* checkIT = currentEquip->GetTemplate())
                                            {
                                                pmTargetPlayer->DestroyItem(INVENTORY_SLOT_BAG_0, equipSlot, true);
                                            }
                                        }
                                        uint16 dest = 0;
                                        if (pmTargetPlayer->CanEquipItem(equipSlot, dest, pItem, false) == InventoryResult::EQUIP_ERR_OK)
                                        {
                                            pmTargetPlayer->EquipItem(dest, pItem, true);
                                            replyStream << "Equiped " << pItem->GetTemplate()->Name1;
                                        }
                                        else
                                        {
                                            replyStream << "Can not equip " << pItem->GetTemplate()->Name1;
                                        }
                                    }
                                }
                                break;
                            } while (itemQR->NextRow());
                        }
                        else
                        {
                            replyStream << "Item not found : " << entry;
                        }
                    }
                    else
                    {
                        replyStream << "Item entry not found : " << entry;
                    }
                }
                else
                {
                    pmTargetPlayer->nierAction->InitializeEquipments(true);
                }
                pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmCommander);
            }
        }
        else if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmCommander->GetGUID())
                    {
                        HandleChatCommand(pmCommander, pmContent, member);
                    }
                }
            }
        }
        else
        {
            //HandleChatCommand(pmCommander, pmContent, pmCommander);
        }
    }
    else if (commandName == "rush")
    {
        if (pmTargetPlayer)
        {
            if (NierStrategy_Base* ns = pmTargetPlayer->nierStrategyMap[pmTargetPlayer->activeStrategyIndex])
            {
                if (ns->basicStrategyType == BasicStrategyType::BasicStrategyType_Freeze || ns->basicStrategyType == BasicStrategyType::BasicStrategyType_Glue)
                {
                    ns->basicStrategyType = BasicStrategyType::BasicStrategyType_Normal;
                }
                if (Unit* target = pmCommander->GetSelectedUnit())
                {
                    if (ns->Engage(target))
                    {
                        int engageLimit = DEFAULT_ACTION_LIMIT_DELAY;
                        if (commandVector.size() > 1)
                        {
                            std::string checkStr = commandVector.at(1);
                            engageLimit = atoi(checkStr.c_str());
                        }
                        ns->rushing = true;
                        ns->actionLimit = engageLimit;
                        ns->ogActionTarget = target->GetGUID();
                        ns->actionType = ActionType::ActionType_Engage;
                        std::ostringstream replyStream;
                        replyStream << "Rush " << target->GetName();
                        pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmCommander);
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
                    if (member->GetGUID() != pmCommander->GetGUID())
                    {
                        HandleChatCommand(pmCommander, pmContent, member);
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
            pmTargetPlayer->nierAction->ResetTalent();
            replyStream << "Talents reset";
            pmTargetPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pmCommander);
        }
        else if (pmTargetGroup)
        {
            for (GroupReference* groupRef = pmTargetGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                Player* member = groupRef->GetSource();
                if (member)
                {
                    if (member->GetGUID() != pmCommander->GetGUID())
                    {
                        HandleChatCommand(pmCommander, pmContent, member);
                    }
                }
            }
        }
    }
}

void NierManager::HandlePacket(const WorldSession* pmSession, WorldPacket pmPacket)
{
    switch (pmPacket.GetOpcode())
    {
    case SMSG_CHAR_ENUM:
    {
        std::unordered_set<uint32> myCharacterIdSet;
        QueryResult characterQR = CharacterDatabase.Query("SELECT guid FROM characters where account = {}", pmSession->GetAccountId());
        if (characterQR)
        {
            Field* characterFields = characterQR->Fetch();
            uint32 character_id = characterFields[0].Get<uint32>();
            if (character_id > 0)
            {
                myCharacterIdSet.insert(character_id);
            }
        }
        for (std::unordered_map<uint32, std::unordered_set<NierEntity*>>::iterator nierGroupIT = nierEntityMap.begin(); nierGroupIT != nierEntityMap.end(); nierGroupIT++)
        {
            for (std::unordered_set<NierEntity*>::iterator reIT = nierGroupIT->second.begin(); reIT != nierGroupIT->second.end(); reIT++)
            {
                if (NierEntity* re = *reIT)
                {
                    if (myCharacterIdSet.find(re->character_id) != myCharacterIdSet.end())
                    {
                        re->entityState = NierEntityState::NierEntityState_DoLogin;
                    }
                }
            }
        }
        break;
    }
    case SMSG_SPELL_FAILURE:
    {
        break;
    }
    case SMSG_SPELL_DELAYED:
    {
        break;
    }
    case SMSG_GROUP_INVITE:
    {
        if (Player* receiver = pmSession->GetPlayer())
        {
            if (!pmSession->GetPlayer())
            {
                break;
            }
            else if (!receiver->IsInWorld())
            {
                break;
            }
            else if (!receiver->nierAction)
            {
                break;
            }
            if (Group* myGroup = receiver->GetGroup())
            {
                receiver->RemoveFromGroup();
                for (uint8 i = 0; i < MAX_DIFFICULTY; ++i)
                {
                    BoundInstancesMap const& m_boundInstances = sInstanceSaveMgr->PlayerGetBoundInstances(receiver->GetGUID(), Difficulty(i));
                    for (BoundInstancesMap::const_iterator itr = m_boundInstances.begin(); itr != m_boundInstances.end(); itr++)
                    {
                        InstanceSave const* save = itr->second.save;
                        uint32 resetTime = itr->second.extended ? save->GetExtendedResetTime() : save->GetResetTime();
                        uint32 ttr = (resetTime >= GameTime::GetGameTime().count() ? resetTime - GameTime::GetGameTime().count() : 0);
                        sInstanceSaveMgr->PlayerUnbindInstance(receiver->GetGUID(), itr->first, Difficulty(i), true, receiver);
                        itr = m_boundInstances.begin();
                    }
                }
            }
            if (Group* grp = receiver->GetGroupInvite())
            {
                for (uint8 i = 0; i < MAX_DIFFICULTY; ++i)
                {
                    BoundInstancesMap const& m_boundInstances = sInstanceSaveMgr->PlayerGetBoundInstances(receiver->GetGUID(), Difficulty(i));
                    for (BoundInstancesMap::const_iterator itr = m_boundInstances.begin(); itr != m_boundInstances.end(); itr++)
                    {
                        InstanceSave const* save = itr->second.save;
                        uint32 resetTime = itr->second.extended ? save->GetExtendedResetTime() : save->GetResetTime();
                        uint32 ttr = (resetTime >= GameTime::GetGameTime().count() ? resetTime - GameTime::GetGameTime().count() : 0);
                        sInstanceSaveMgr->PlayerUnbindInstance(receiver->GetGUID(), itr->first, Difficulty(i), true, receiver);
                        itr = m_boundInstances.begin();
                    }
                }
                WorldPacket wpAccept(CMSG_GROUP_ACCEPT, 4);
                wpAccept << uint32(0);
                receiver->GetSession()->HandleGroupAcceptOpcode(wpAccept);
                std::ostringstream replyStream_Talent;
                receiver->nierAction->Reset();
                replyStream_Talent << sNierManager->characterTalentTabNameMap[receiver->getClass()][receiver->nierAction->specialty];
                if (Player* inviter = ObjectAccessor::FindConnectedPlayer(grp->GetLeaderGUID()))
                {
                    receiver->Whisper(replyStream_Talent.str(), Language::LANG_UNIVERSAL, inviter);
                }
                if (receiver->getClass() == Classes::CLASS_ROGUE)
                {
                    if (NierAction_Rogue* nar = (NierAction_Rogue*)receiver->nierAction)
                    {
                        nar->CancelAura(nar->spell_Stealth);
                    }
                }
            }
        }
        break;
    }
    case BUY_ERR_NOT_ENOUGHT_MONEY:
    {
        break;
    }
    case BUY_ERR_REPUTATION_REQUIRE:
    {
        break;
    }
    case MSG_RAID_READY_CHECK:
    {
        break;
    }
    case SMSG_GROUP_SET_LEADER:
    {
        //std::string leaderName = "";
        //pmPacket >> leaderName;
        //Player* newLeader = ObjectAccessor::FindPlayerByName(leaderName);
        //if (newLeader)
        //{
        //    if (newLeader->GetGUID() == me->GetGUID())
        //    {
        //        WorldPacket data(CMSG_GROUP_SET_LEADER, 8);
        //        data << master->GetGUID().WriteAsPacked();
        //        me->GetSession()->HandleGroupSetLeaderOpcode(data);
        //    }
        //    else
        //    {
        //        if (!newLeader->isnier)
        //        {
        //            master = newLeader;
        //        }
        //    }
        //}
        break;
    }
    case SMSG_RESURRECT_REQUEST:
    {
        if (Player* receiver = pmSession->GetPlayer())
        {
            if (!receiver)
            {
                break;
            }
            else if (!receiver->IsInWorld())
            {
                break;
            }
            else if (!receiver->nierAction)
            {
                break;
            }
            if (receiver->isResurrectRequested())
            {
                receiver->ResurectUsingRequestData();
                receiver->ClearInCombat();
                receiver->nierAction->ClearTarget();
                receiver->nierAction->nm->ResetMovement();
            }
        }
        break;
    }
    case SMSG_INVENTORY_CHANGE_FAILURE:
    {
        break;
    }
    case SMSG_TRADE_STATUS:
    {
        break;
    }
    case SMSG_LOOT_RESPONSE:
    {
        break;
    }
    case SMSG_ITEM_PUSH_RESULT:
    {
        break;
    }
    case SMSG_PARTY_COMMAND_RESULT:
    {
        break;
    }
    case SMSG_DUEL_REQUESTED:
    {
        if (Player* receiver = pmSession->GetPlayer())
        {
            if (!receiver)
            {
                break;
            }
            if (!receiver->duel)
            {
                break;
            }
            receiver->DuelComplete(DuelCompleteType::DUEL_INTERRUPTED);
            receiver->Whisper("Not interested", Language::LANG_UNIVERSAL, receiver->duel->Opponent);
        }
        break;
    }
    default:
    {
        break;
    }
    }
}
