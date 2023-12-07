#include "NierManager.h"

#include "NierConfig.h"
#include "Nier_Base.h"
#include "MingManager.h"
#include "GameTime.h"
#include "Item.h"
#include "InstanceSaveMgr.h"
#include "Player.h"
#include "Group.h"
#include "World.h"
#include "AccountMgr.h"
#include "MoveSplineInit.h"
#include "GridNotifiers.h"
#include "Pet.h"

#include <boost/chrono/duration.hpp>

NierManager::NierManager()
{
    allianceRaces.clear();
    hordeRaces.clear();
    nierNameMap.clear();
    characterTalentTabNameMap.clear();
    trainerMap.clear();
}

void NierManager::InitializeManager()
{
    if (sNierConfig->Enable == 0)
    {
        return;
    }

    sLog->outMessage(NIER_MARK, LogLevel::LOG_LEVEL_DEBUG, "Initialize nier");

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

    trainerMap.clear();
    trainerMap[Classes::CLASS_WARRIOR] = 914;
    trainerMap[Classes::CLASS_PALADIN] = 928;
    trainerMap[Classes::CLASS_HUNTER] = 5115;
    trainerMap[Classes::CLASS_ROGUE] = 4214;
    trainerMap[Classes::CLASS_PRIEST] = 3046;
    trainerMap[Classes::CLASS_SHAMAN] = 3032;
    trainerMap[Classes::CLASS_MAGE] = 3047;
    trainerMap[Classes::CLASS_WARLOCK] = 5173;
    trainerMap[Classes::CLASS_DRUID] = 4217;

    groupRoleNameMap.clear();
    groupRoleNameMap[GroupRole::GroupRole_Tank] = "tank";
    groupRoleNameMap[GroupRole::GroupRole_Healer] = "healer";
    groupRoleNameMap[GroupRole::GroupRole_DPS] = "dps";

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

    if (sNierConfig->Reset == 1)
    {
        DeleteNiers();
    }

    sLog->outMessage(NIER_MARK, LogLevel::LOG_LEVEL_DEBUG, "nier system initialized");
}

NierManager* NierManager::instance()
{
    static NierManager instance;
    return &instance;
}

void NierManager::LogoutNiers(bool pmInstant)
{
    sWorld->KickAll();
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

void NierManager::HandleChatCommand(Player* pCommander, std::string pContent, Player* pTargetPlayer)
{
    if (!pCommander)
    {
        return;
    }
    std::vector<std::string> commandVector = sMingManager->SplitString(pContent, " ", true);
    std::string commandName = commandVector.at(0);

    if (commandName == "nier")
    {
        if (commandVector.size() > 1)
        {
            std::string partnerAction = commandVector.at(1);
            if (partnerAction == "online")
            {
                if (commandVector.size() > 2)
                {
                    std::string partnerAction = commandVector.at(2);
                    if (partnerAction == "partner")
                    {
                        uint32 rawLevel = pCommander->GetLevel();
                        if (rawLevel < 10)
                        {
                            sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, "Level lower than 10", pCommander);
                            return;
                        }
                        if (pCommander->partners.empty())
                        {
                            uint32 meId = pCommander->GetGUID().GetCounter();
                            std::ostringstream nierQueryStream;
                            nierQueryStream << "SELECT entry, master_id, account_id, character_id, account_name, race, career, specialty, role, nier_type FROM nier where master_id = " << meId << " and nier_type = 0";
                            while (true)
                            {
                                QueryResult nierQR = CharacterDatabase.Query(nierQueryStream.str().c_str());
                                if (nierQR)
                                {
                                    std::ostringstream replyStream;
                                    replyStream << "Partner loaded : ";
                                    do
                                    {
                                        Nier_Base* nier = nullptr;
                                        Field* fields = nierQR->Fetch();
                                        uint32 targetCareer = fields[6].Get<uint32>();
                                        switch (targetCareer)
                                        {
                                        case 1:
                                        {
                                            nier = new Nier_Warrior();
                                            break;
                                        }
                                        case 2:
                                        {
                                            nier = new Nier_Paladin();
                                            break;
                                        }
                                        case 3:
                                        {
                                            nier = new Nier_Hunter();
                                            break;
                                        }
                                        case 4:
                                        {
                                            nier = new Nier_Rogue();
                                            break;
                                        }
                                        case 5:
                                        {
                                            nier = new Nier_Priest();
                                            break;
                                        }
                                        case 7:
                                        {
                                            nier = new Nier_Shaman();
                                            break;
                                        }
                                        case 8:
                                        {
                                            nier = new Nier_Mage();
                                            break;
                                        }
                                        case 9:
                                        {
                                            nier = new Nier_Warlock();
                                            break;
                                        }
                                        case 11:
                                        {
                                            nier = new Nier_Druid();
                                            break;
                                        }
                                        default:
                                        {
                                            nier = new Nier_Base();
                                            break;
                                        }
                                        }
                                        nier->entry = fields[0].Get<uint32>();
                                        nier->master_id = fields[1].Get<uint32>();
                                        nier->account_id = fields[2].Get<uint32>();
                                        nier->character_id = fields[3].Get<uint32>();
                                        nier->target_race = fields[5].Get<uint32>();
                                        nier->target_specialty = fields[7].Get<uint32>();
                                        nier->target_class = targetCareer;
                                        pCommander->partners.insert(nier);
                                        replyStream << targetCareer << " ";
                                    } while (nierQR->NextRow());
                                    sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, replyStream.str(), pCommander);
                                    break;
                                }
                                else
                                {
                                    std::ostringstream replyStream;
                                    replyStream << "Nier added : ";
                                    uint32 career = Classes::CLASS_PRIEST;
                                    AddNier(pCommander, career);
                                    replyStream << career << " ";
                                    career = Classes::CLASS_WARLOCK;
                                    AddNier(pCommander, career);
                                    replyStream << career << " ";
                                    career = Classes::CLASS_MAGE;
                                    AddNier(pCommander, career);
                                    replyStream << career << " ";
                                    career = Classes::CLASS_ROGUE;
                                    AddNier(pCommander, career);
                                    replyStream << career << " ";
                                    sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, replyStream.str(), pCommander);
                                }
                            }
                        }
                        for (std::unordered_set<Nier_Base*>::iterator nit = pCommander->partners.begin(); nit != pCommander->partners.end(); nit++)
                        {
                            if (Nier_Base* nb = *nit)
                            {
                                if (nb->entityState == NierState::NierState_OffLine)
                                {
                                    nb->entityState = NierState::NierState_Enter;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else if (commandName == "role")
    {
        if (commandVector.size() > 1)
        {
            std::string role = commandVector.at(1);
            if (role == "tank")
            {
                pCommander->groupRole = GroupRole::GroupRole_Tank;
            }
            else if (role == "healer")
            {
                pCommander->groupRole = GroupRole::GroupRole_Healer;
            }
            else if (role == "dps")
            {
                pCommander->groupRole = GroupRole::GroupRole_DPS;
            }
        }
        std::ostringstream replyStream;
        replyStream << "Role is : " << groupRoleNameMap[pCommander->groupRole];
        sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, replyStream.str(), pCommander);
    }
    else if (commandName == "equip")
    {
        for (std::unordered_set<Nier_Base*>::iterator nit = pCommander->partners.begin(); nit != pCommander->partners.end(); nit++)
        {
            if (Nier_Base* nb = *nit)
            {
                if (Player* eachPlayer = nb->me)
                {
                    if (pTargetPlayer)
                    {
                        if (pTargetPlayer->GetGUID() == eachPlayer->GetGUID())
                        {
                            nb->InitializeEquipments(true);
                            eachPlayer->Whisper("Equipped", Language::LANG_UNIVERSAL, pCommander);
                            break;
                        }
                    }
                    else
                    {
                        nb->InitializeEquipments(true);
                        eachPlayer->Whisper("Equipped", Language::LANG_UNIVERSAL, pCommander);
                    }
                }
            }
        }
    }
    else if (commandName == "assemble")
    {
        for (std::unordered_set<Nier_Base*>::iterator nit = pCommander->partners.begin(); nit != pCommander->partners.end(); nit++)
        {
            if (Nier_Base* nb = *nit)
            {
                if (Player* eachPlayer = nb->me)
                {
                    if (pTargetPlayer)
                    {
                        if (pTargetPlayer->GetGUID() == eachPlayer->GetGUID())
                        {
                            nb->assembleDelay = 60000;
                            eachPlayer->Whisper("Assemble in 60 seconds", Language::LANG_UNIVERSAL, pCommander);
                            break;
                        }
                    }
                    else
                    {
                        nb->assembleDelay = 60000;
                        eachPlayer->Whisper("Assemble in 60 seconds", Language::LANG_UNIVERSAL, pCommander);
                    }
                }
            }
        }
    }
    else if (commandName == "who")
    {
        for (std::unordered_set<Nier_Base*>::iterator nit = pCommander->partners.begin(); nit != pCommander->partners.end(); nit++)
        {
            if (Nier_Base* nb = *nit)
            {
                if (Player* eachPlayer = nb->me)
                {
                    if (pTargetPlayer)
                    {
                        if (pTargetPlayer->GetGUID() == eachPlayer->GetGUID())
                        {
                            std::ostringstream replyStream;
                            replyStream << "Role is : " << groupRoleNameMap[eachPlayer->groupRole];
                            replyStream << " Specialty is : " << characterTalentTabNameMap[eachPlayer->getClass()][nb->target_specialty];
                            eachPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pCommander);
                            break;
                        }
                    }
                    else
                    {
                        std::ostringstream replyStream;
                        replyStream << "Role is : " << groupRoleNameMap[eachPlayer->groupRole];
                        replyStream << " Specialty is : " << characterTalentTabNameMap[eachPlayer->getClass()][nb->target_specialty];
                        eachPlayer->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, pCommander);
                    }
                }
            }
        }
    }
    else if (commandName == "freeze")
    {
        for (std::unordered_set<Nier_Base*>::iterator nit = pCommander->partners.begin(); nit != pCommander->partners.end(); nit++)
        {
            if (Nier_Base* nb = *nit)
            {
                if (Player* eachPlayer = nb->me)
                {
                    if (pTargetPlayer)
                    {
                        if (pTargetPlayer->GetGUID() == eachPlayer->GetGUID())
                        {
                            nb->freezing = true;
                            eachPlayer->InterruptSpell(CurrentSpellTypes::CURRENT_AUTOREPEAT_SPELL);
                            eachPlayer->InterruptSpell(CurrentSpellTypes::CURRENT_CHANNELED_SPELL);
                            eachPlayer->InterruptSpell(CurrentSpellTypes::CURRENT_GENERIC_SPELL);
                            eachPlayer->InterruptSpell(CurrentSpellTypes::CURRENT_MELEE_SPELL);
                            eachPlayer->AttackStop();
                            if (Pet* myPet = eachPlayer->GetPet())
                            {
                                myPet->AttackStop();
                                if (CharmInfo* pci = myPet->GetCharmInfo())
                                {
                                    if (pci->IsCommandAttack())
                                    {
                                        pci->SetIsCommandAttack(false);
                                    }
                                    if (!pci->IsCommandFollow())
                                    {
                                        pci->SetIsCommandFollow(true);
                                    }
                                }
                            }
                            eachPlayer->StopMoving();
                            eachPlayer->GetMotionMaster()->Clear();
                            eachPlayer->Whisper("Freezed", Language::LANG_UNIVERSAL, pCommander);
                            break;
                        }
                    }
                    else
                    {
                        nb->freezing = true;
                        eachPlayer->InterruptSpell(CurrentSpellTypes::CURRENT_AUTOREPEAT_SPELL);
                        eachPlayer->InterruptSpell(CurrentSpellTypes::CURRENT_CHANNELED_SPELL);
                        eachPlayer->InterruptSpell(CurrentSpellTypes::CURRENT_GENERIC_SPELL);
                        eachPlayer->InterruptSpell(CurrentSpellTypes::CURRENT_MELEE_SPELL);
                        eachPlayer->StopMoving();
                        eachPlayer->GetMotionMaster()->Clear();
                        eachPlayer->Whisper("Freezed", Language::LANG_UNIVERSAL, pCommander);
                    }
                }
            }
        }
    }
    else if (commandName == "follow")
    {
        for (std::unordered_set<Nier_Base*>::iterator nit = pCommander->partners.begin(); nit != pCommander->partners.end(); nit++)
        {
            if (Nier_Base* nb = *nit)
            {
                if (Player* eachPlayer = nb->me)
                {
                    if (pTargetPlayer)
                    {
                        if (pTargetPlayer->GetGUID() == eachPlayer->GetGUID())
                        {
                            nb->freezing = false;
                            eachPlayer->Whisper("Following", Language::LANG_UNIVERSAL, pCommander);
                            break;
                        }
                    }
                    else
                    {
                        nb->freezing = false;
                        eachPlayer->Whisper("Following", Language::LANG_UNIVERSAL, pCommander);
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

                ObjectGuid masterGuid = ObjectGuid(HighGuid::Player, receiver->masterId);
                Player* master = ObjectAccessor::FindConnectedPlayer(masterGuid);

                for (std::unordered_set<Nier_Base*>::iterator nit = master->partners.begin(); nit != master->partners.end(); nit++)
                {
                    if (Nier_Base* nb = *nit)
                    {
                        if (Player* eachPlayer = nb->me)
                        {
                            if (receiver->GetGUID() == eachPlayer->GetGUID())
                            {
                                std::ostringstream replyStream;
                                replyStream << "Role is : " << groupRoleNameMap[receiver->groupRole];
                                replyStream << " Specialty is : " << characterTalentTabNameMap[receiver->getClass()][nb->target_specialty];
                                receiver->Whisper(replyStream.str(), Language::LANG_UNIVERSAL, master);
                                break;
                            }
                        }
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
            if (receiver->isResurrectRequested())
            {
                receiver->ResurectUsingRequestData();
                receiver->ClearInCombat();
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

void NierManager::RandomTeleport(Player* me, Player* target)
{
    int myLevel = me->getLevel();
    std::unordered_map<uint32, ObjectGuid> samePlayersMap;
    std::unordered_map<uint32, WorldSession*> allSessions = sWorld->GetAllSessions();
    for (std::unordered_map<uint32, WorldSession*>::iterator wsIT = allSessions.begin(); wsIT != allSessions.end(); wsIT++)
    {
        if (WorldSession* eachWS = wsIT->second)
        {
            if (!eachWS->isNier)
            {
                if (Player* eachPlayer = eachWS->GetPlayer())
                {
                    if (Map* map = eachPlayer->GetMap())
                    {
                        if (!map->Instanceable())
                        {
                            int eachLevel = eachPlayer->getLevel();
                            if (myLevel > eachLevel - 5 && myLevel < eachLevel + 5)
                            {
                                //if (me->IsHostileTo(eachPlayer))
                                //{
                                //    samePlayersMap[samePlayersMap.size()] = eachPlayer->GetGUID();
                                //}
                                samePlayersMap[samePlayersMap.size()] = eachPlayer->GetGUID();
                            }
                        }
                    }
                }
            }
        }
    }
    uint32 targetPlayerIndex = urand(0, samePlayersMap.size());
    if (Player* targetPlayer = ObjectAccessor::FindPlayer(samePlayersMap[targetPlayerIndex]))
    {
        float nearX = 0.0f;
        float nearY = 0.0f;
        float nearZ = 0.0f;
        float nearDistance = frand(200.0f, 400.0f);
        float nearAngle = frand(0.0f, M_PI * 2);
        targetPlayer->GetNearPoint(targetPlayer, nearX, nearY, nearZ, 0.0f, nearDistance, nearAngle);
        me->ClearInCombat();
        me->TeleportTo(me->GetMapId(), nearX, nearY, nearZ, 0);
    }
}

void NierManager::AddNier(Player* pMaster, uint32 pCareer)
{
    uint32 meTeamId = pMaster->GetTeamId();
    uint32 race = 0;
    if (meTeamId == TeamId::TEAM_ALLIANCE)
    {
        race = sNierManager->allianceRaces[pCareer].size();
        race = urand(1, race);
        if (race >= sNierManager->allianceRaces.size())
        {
            race = sNierManager->allianceRaces.size() - 1;
        }
        race = sNierManager->allianceRaces[pCareer][race];
    }
    else
    {
        race = sNierManager->hordeRaces[pCareer].size();
        race = urand(1, race);
        if (race >= sNierManager->hordeRaces.size())
        {
            race = sNierManager->hordeRaces.size() - 1;
        }
        race = sNierManager->hordeRaces[pCareer][race];
    }

    std::ostringstream nierInsertStream;
    nierInsertStream << "INSERT INTO `nier` (`master_id`, `account_id`, `character_id`, `account_name`, `race`, `career`, `specialty`, `role`, `nier_type`) VALUES (" << pMaster->GetGUID().GetCounter() << ", 0, 0, '', " << race << ", " << pCareer << ", 1, 1, 0)";
    CharacterDatabase.DirectExecute(nierInsertStream.str().c_str());
    sLog->outMessage(NIER_MARK, LogLevel::LOG_LEVEL_DEBUG, "nier added : {}, {}", race, pCareer);
}
