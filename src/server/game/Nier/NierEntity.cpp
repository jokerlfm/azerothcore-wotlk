#include "NierEntity.h"
#include "NierManager.h"
#include "NierConfig.h"
#include "MotionMaster.h"
#include "NierStrategies/NierStrategy_Base.h"
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

#include "Accounts/AccountMgr.h"
#include "Player.h"
#include "Groups/Group.h"
#include "World/World.h"

NierEntity::NierEntity()
{
    master_id = 0;
    account_name = "";
    account_id = 0;
    character_name = "";
    character_id = 0;
    target_team = 0;
    target_level = 0;
    target_race = 0;
    target_class = 0;
    target_specialty = 0;
    nier_type = 0;
    checkDelay = 2 * IN_MILLISECONDS;
    entityState = NierEntityState::NierEntityState_OffLine;
}

void NierEntity::Update(uint32 pmDiff)
{
    if (checkDelay >= 0)
    {
        checkDelay -= pmDiff;
    }
    if (checkDelay < 0)
    {
        checkDelay = urand(2 * IN_MILLISECONDS, 10 * IN_MILLISECONDS);
        switch (entityState)
        {
        case NierEntityState::NierEntityState_None:
        {
            checkDelay = urand(5 * MINUTE * IN_MILLISECONDS, 10 * MINUTE * IN_MILLISECONDS);
            break;
        }
        case NierEntityState::NierEntityState_OffLine:
        {
            break;
        }
        case NierEntityState::NierEntityState_Enter:
        {
            entityState = NierEntityState::NierEntityState_CheckAccount;
            sLog->outMessage(NIER_MARK, LogLevel::LOG_LEVEL_INFO, "Nier {} is ready to go online.", account_id);
            break;
        }
        case NierEntityState::NierEntityState_CheckAccount:
        {
            if (account_name.empty())
            {
                checkDelay = urand(600000, 1200000);
                entityState = NierEntityState::NierEntityState_None;
            }
            else
            {
                uint32 queryAccountId = 0;
                std::ostringstream accountQueryStream;
                accountQueryStream << "SELECT id FROM account where username = '" << account_name << "'";
                QueryResult nierAccountQR = LoginDatabase.Query(accountQueryStream.str().c_str());
                if (nierAccountQR)
                {
                    Field* fields = nierAccountQR->Fetch();
                    queryAccountId = fields[0].Get<uint32>();
                }
                if (queryAccountId > 0)
                {
                    account_id = queryAccountId;
                    std::ostringstream sqlStream;
                    sqlStream << "update nier set account_id = " << account_id << " where account_name = '" << account_name << "'";
                    std::string sql = sqlStream.str();
                    CharacterDatabase.DirectExecute(sql.c_str());
                    sLog->outMessage(NIER_MARK, LogLevel::LOG_LEVEL_INFO, "Nier {} is ready.", account_name.c_str());
                    entityState = NierEntityState::NierEntityState_CheckCharacter;
                }
                else
                {
                    sLog->outMessage(NIER_MARK, LogLevel::LOG_LEVEL_INFO, "Nier {} is not ready.", account_name.c_str());
                    entityState = NierEntityState::NierEntityState_CreateAccount;
                }
            }
            break;
        }
        case NierEntityState::NierEntityState_CreateAccount:
        {
            if (account_name.empty())
            {
                entityState = NierEntityState::NierEntityState_None;
            }
            else
            {
                if (AccountMgr::CreateAccount(account_name, NIER_MARK) == AccountOpResult::AOR_OK)
                {
                    entityState = NierEntityState::NierEntityState_CheckAccount;
                }
                else
                {
                    sLog->outMessage(NIER_MARK, LogLevel::LOG_LEVEL_INFO, "Nier {} account creation failed.", account_name);
                    entityState = NierEntityState::NierEntityState_None;
                }
            }
            break;
        }
        case NierEntityState::NierEntityState_CheckCharacter:
        {
            std::ostringstream queryStream;
            queryStream << "SELECT guid FROM characters where account = " << account_id;
            QueryResult characterQR = CharacterDatabase.Query(queryStream.str().c_str());
            if (characterQR)
            {
                Field* characterFields = characterQR->Fetch();
                character_id = characterFields[0].Get<uint32>();
                if (character_id > 0)
                {
                    sLog->outMessage(NIER_MARK, LogLevel::LOG_LEVEL_INFO, "Nier account_id {} character_id {} is ready.", account_id, character_id);
                    entityState = NierEntityState::NierEntityState_DoEnum;
                    //entityState = NierEntityState::NierEntityState_DoLogin;
                    break;
                }
            }
            sLog->outMessage(NIER_MARK, LogLevel::LOG_LEVEL_INFO, "Nier account_id {} character_id is not ready.", account_id);
            entityState = NierEntityState::NierEntityState_CreateCharacter;
            break;
        }
        case NierEntityState::NierEntityState_CreateCharacter:
        {
            uint8 gender = 0, skin = 0, face = 0, hairStyle = 0, hairColor = 0, facialHair = 0;
            while (true)
            {
                gender = urand(0, 100);
                if (gender < 50)
                {
                    gender = 0;
                }
                else
                {
                    gender = 1;
                }
                face = urand(0, 5);
                hairStyle = urand(0, 5);
                hairColor = urand(0, 5);
                facialHair = urand(0, 5);

                CharacterCreateInfo* cci = new CharacterCreateInfo();
                cci->Name = character_name;
                cci->Race = target_race;
                cci->Class = target_class;
                cci->Gender = gender;
                cci->Skin = skin;
                cci->Face = face;
                cci->HairStyle = hairStyle;
                cci->HairColor = hairColor;
                cci->FacialHair = facialHair;
                cci->OutfitId = 0;

                WorldSession* createSession = new WorldSession(account_id, std::move(account_name), NULL, AccountTypes::SEC_PLAYER, 2, 0, LocaleConstant::LOCALE_enUS, 0, false, true, 0);
                Player* newPlayer = new Player(createSession);
                if (!newPlayer->Create(sObjectMgr->GetGenerator<HighGuid::Player>().Generate(), cci))
                {
                    newPlayer->CleanupsBeforeDelete();
                    delete createSession;
                    delete newPlayer;
                    sLog->outMessage(NIER_MARK, LogLevel::LOG_LEVEL_ERROR, "Character create failed, {} {} {} ", character_name.c_str(), target_race, target_class);
                    sLog->outMessage(NIER_MARK, LogLevel::LOG_LEVEL_INFO, "Try again");
                    continue;
                }
                newPlayer->GetMotionMaster()->Initialize();
                newPlayer->setCinematic(2);
                newPlayer->SetAtLoginFlag(AT_LOGIN_NONE);
                newPlayer->SaveToDB(true, true);
                character_id = newPlayer->GetGUID().GetCounter();
                if (character_id > 0)
                {
                    createSession->isNier = true;
                    sWorld->AddSession(createSession);
                    std::ostringstream replyStream;
                    replyStream << "nier character created : account - " << account_id << " character - " << newPlayer->GetGUID().GetCounter() << " " << character_name;
                    std::string replyString = replyStream.str();
                    sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, replyString.c_str());
                    sLog->outMessage(NIER_MARK, LogLevel::LOG_LEVEL_INFO, replyString.c_str());
                    break;
                }
            }
            if (character_id > 0)
            {
                std::ostringstream sqlStream;
                sqlStream << "update nier set character_id = " << character_id << " where account_id = " << account_id << " and character_name = '" << character_name << "'";
                std::string sql = sqlStream.str();
                CharacterDatabase.DirectExecute(sql.c_str());
                entityState = NierEntityState::NierEntityState_CheckCharacter;
                break;
            }
            entityState = NierEntityState::NierEntityState_None;
            break;
        }
        case NierEntityState::NierEntityState_DoEnum:
        {
            WorldSession* loginSession = sWorld->FindSession(account_id);
            if (!loginSession)
            {
                loginSession = new WorldSession(account_id, std::move(account_name), NULL, AccountTypes::SEC_PLAYER, 2, 0, LocaleConstant::LOCALE_enUS, 0, false, true, 0);
                sWorld->AddSession(loginSession);
            }
            loginSession->isNier = true;
            WorldPacket wpEnum(CMSG_CHAR_ENUM, 4);
            loginSession->HandleCharEnumOpcode(wpEnum);
            sLog->outMessage(NIER_MARK, LogLevel::LOG_LEVEL_INFO, "Enum character {} {} ", account_id, character_id);
            checkDelay = urand(5 * IN_MILLISECONDS, 10 * IN_MILLISECONDS);
            entityState = NierEntityState::NierEntityState_CheckEnum;
            break;
        }
        case NierEntityState::NierEntityState_CheckEnum:
        {
            checkDelay = urand(5 * IN_MILLISECONDS, 10 * IN_MILLISECONDS);
            break;
        }
        case NierEntityState::NierEntityState_DoLogin:
        {
            WorldSession* loginSession = sWorld->FindSession(account_id);
            if (!loginSession)
            {
                loginSession = new WorldSession(account_id, std::move(account_name), NULL, AccountTypes::SEC_PLAYER, 2, 0, LocaleConstant::LOCALE_enUS, 0, false, true, 0);
                sWorld->AddSession(loginSession);
            }
            loginSession->isNier = true;
            WorldPacket wpLogin(CMSG_PLAYER_LOGIN, 16);
            ObjectGuid playerGuid = ObjectGuid(HighGuid::Player, character_id);
            wpLogin << playerGuid;
            loginSession->HandlePlayerLoginOpcode(wpLogin);
            std::ostringstream replyStream;
            replyStream << "log in character : account - " << account_id << " character - " << character_id;
            std::string replyString = replyStream.str();
            sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, replyString.c_str());
            sLog->outMessage(NIER_MARK, LogLevel::LOG_LEVEL_INFO, replyString.c_str());
            checkDelay = urand(5 * IN_MILLISECONDS, 10 * IN_MILLISECONDS);
            entityState = NierEntityState::NierEntityState_CheckLogin;
            break;
        }
        case NierEntityState::NierEntityState_CheckLogin:
        {
            ObjectGuid playerGuid = ObjectGuid(HighGuid::Player, character_id);
            if (Player* me = ObjectAccessor::FindConnectedPlayer(playerGuid))
            {
                if (me->IsInWorld())
                {
                    me->isNier = true;
                    sCharacterCache->AddCharacterCacheEntry(me->GetGUID(), account_id, me->GetName(), me->getGender(), me->getRace(), me->getClass(), me->getLevel());
                    std::ostringstream replyStream;
                    replyStream << "nier character logged in : account - " << account_id << " character - " << character_id;
                    std::string replyString = replyStream.str();
                    sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, replyString.c_str());
                    sLog->outMessage(NIER_MARK, LogLevel::LOG_LEVEL_INFO, replyString.c_str());
                    entityState = NierEntityState::NierEntityState_Initialize;
                    break;
                }
            }
            checkDelay = urand(5 * IN_MILLISECONDS, 10 * IN_MILLISECONDS);
            break;
        }
        case NierEntityState::NierEntityState_Initialize:
        {
            ObjectGuid playerGuid = ObjectGuid(HighGuid::Player, character_id);
            if (Player* me = ObjectAccessor::FindConnectedPlayer(playerGuid))
            {
                if (me->IsInWorld())
                {
                    me->activeStrategyIndex = 0;
                    me->nierStrategyMap.clear();
                    me->nierStrategyMap[me->activeStrategyIndex] = new NierStrategy_Base();
                    me->nierStrategyMap[me->activeStrategyIndex]->masterId = master_id;
                    me->nierStrategyMap[me->activeStrategyIndex]->me = me;
                    me->nierStrategyMap[me->activeStrategyIndex]->Reset();
                    me->nierStrategyMap[StrategyIndex::StrategyIndex_The_Underbog] = new NierStrategy_The_Underbog();
                    me->nierStrategyMap[StrategyIndex::StrategyIndex_The_Underbog]->masterId = master_id;
                    me->nierStrategyMap[StrategyIndex::StrategyIndex_The_Underbog]->me = me;
                    me->nierStrategyMap[StrategyIndex::StrategyIndex_The_Underbog]->Reset();
                    me->nierStrategyMap[StrategyIndex::StrategyIndex_The_Black_Morass] = new NierStrategy_The_Black_Morass();
                    me->nierStrategyMap[StrategyIndex::StrategyIndex_The_Black_Morass]->masterId = master_id;
                    me->nierStrategyMap[StrategyIndex::StrategyIndex_The_Black_Morass]->me = me;
                    me->nierStrategyMap[StrategyIndex::StrategyIndex_The_Black_Morass]->Reset();
                    me->nierStrategyMap[585] = new NierStrategy_Magisters_Terrace();
                    me->nierStrategyMap[585]->masterId = master_id;
                    me->nierStrategyMap[585]->me = me;
                    me->nierStrategyMap[585]->Reset();
                    me->nierStrategyMap[601] = new NierStrategy_Azjol_Nerub();
                    me->nierStrategyMap[585]->masterId = master_id;
                    me->nierStrategyMap[601]->me = me;
                    me->nierStrategyMap[601]->Reset();
                    me->nierStrategyMap[me->activeStrategyIndex]->relocateDelay = 0;
                    switch (target_class)
                    {
                    case Classes::CLASS_DEATH_KNIGHT:
                    {
                        me->nierAction = new NierAction_Base(me);
                        break;
                    }
                    case Classes::CLASS_DRUID:
                    {
                        me->nierAction = new NierAction_Druid(me);
                        break;
                    }
                    case Classes::CLASS_HUNTER:
                    {
                        me->nierAction = new NierAction_Hunter(me);
                        break;
                    }
                    case Classes::CLASS_MAGE:
                    {
                        me->nierAction = new NierAction_Mage(me);
                        break;
                    }
                    case Classes::CLASS_PALADIN:
                    {
                        me->nierAction = new NierAction_Paladin(me);
                        break;
                    }
                    case Classes::CLASS_PRIEST:
                    {
                        me->nierAction = new NierAction_Priest(me);
                        break;
                    }
                    case Classes::CLASS_ROGUE:
                    {
                        me->nierAction = new NierAction_Rogue(me);
                        break;
                    }
                    case Classes::CLASS_SHAMAN:
                    {
                        me->nierAction = new NierAction_Shaman(me);
                        break;
                    }
                    case Classes::CLASS_WARLOCK:
                    {
                        me->nierAction = new NierAction_Warlock(me);
                        break;
                    }
                    case Classes::CLASS_WARRIOR:
                    {
                        me->nierAction = new NierAction_Warrior(me);
                        break;
                    }
                    default:
                    {
                        me->nierAction = new NierAction_Base(me);
                        break;
                    }
                    }
                    target_level = 20;
                    ObjectGuid masterGuid = ObjectGuid(HighGuid::Player, master_id);
                    if (Player* master = ObjectAccessor::FindConnectedPlayer(masterGuid))
                    {
                        target_level = master->GetLevel();
                    }
                    me->nierAction->InitializeCharacter(target_level, target_specialty);
                    std::ostringstream replyStream;
                    replyStream << "nier initialized : account - " << account_id << " character - " << character_id << " " << me->GetName();
                    std::string replyString = replyStream.str();
                    sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, replyString.c_str());
                    sLog->outMessage(NIER_MARK, LogLevel::LOG_LEVEL_INFO, replyString.c_str());
                    entityState = NierEntityState::NierEntityState_Equip;
                    break;
                }
            }
            entityState = NierEntityState::NierEntityState_OffLine;
            break;
        }
        case NierEntityState::NierEntityState_Equip:
        {
            ObjectGuid playerGuid = ObjectGuid(HighGuid::Player, character_id);
            if (Player* me = ObjectAccessor::FindConnectedPlayer(playerGuid))
            {
                if (me->IsInWorld())
                {
                    me->nierAction->InitializeEquipments();
                    for (std::unordered_map<uint32, NierStrategy_Base*>::iterator nsbIt = me->nierStrategyMap.begin(); nsbIt != me->nierStrategyMap.end(); nsbIt++)
                    {
                        if (NierStrategy_Base* nsb = nsbIt->second)
                        {
                            nsb->me = me;
                            nsb->nierInitialized = true;
                        }
                    }
                    entityState = NierEntityState::NierEntityState_Online;
                    std::ostringstream msgStream;
                    msgStream << me->GetName() << " Equiped";
                    sWorld->SendServerMessage(ServerMessageType::SERVER_MSG_STRING, msgStream.str().c_str());
                    break;
                }
            }
            break;
        }
        case NierEntityState::NierEntityState_Online:
        {
            checkDelay = urand(60000, 1200000);
            ObjectGuid playerGuid = ObjectGuid(HighGuid::Player, character_id);
            if (Player* me = ObjectAccessor::FindConnectedPlayer(playerGuid))
            {
                if (me->IsInWorld())
                {
                    me->nierAction->Prepare();
                    ObjectGuid masterGuid = ObjectGuid(HighGuid::Player, master_id);
                    if (Player* master = ObjectAccessor::FindConnectedPlayer(masterGuid))
                    {
                        if (me->GetLevel() != master->GetLevel())
                        {
                            if (me->IsAlive())
                            {
                                target_level = master->GetLevel();
                                for (std::unordered_map<uint32, NierStrategy_Base*>::iterator nsbIt = me->nierStrategyMap.begin(); nsbIt != me->nierStrategyMap.end(); nsbIt++)
                                {
                                    if (NierStrategy_Base* nsb = nsbIt->second)
                                    {
                                        nsb->nierInitialized = false;
                                    }
                                }
                                me->nierAction->InitializeCharacter(target_level, target_specialty);
                                entityState = NierEntityState::NierEntityState_Equip;
                            }
                        }
                    }
                    if (Group* myGroup = me->GetGroup())
                    {
                        if (Player* leader = ObjectAccessor::FindPlayer(myGroup->GetLeaderGUID()))
                        {
                            if (leader->GetSession()->isNier)
                            {
                                me->RemoveFromGroup();
                            }
                        }
                    }
                }
                else
                {
                    entityState = NierEntityState::NierEntityState_Exit;
                }
            }
            break;
        }
        case NierEntityState::NierEntityState_Exit:
        {
            ObjectGuid playerGuid = ObjectGuid(HighGuid::Player, character_id);
            if (Player* me = ObjectAccessor::FindConnectedPlayer(playerGuid))
            {
                if (me->IsInWorld())
                {
                    if (me->GetGroup())
                    {
                        entityState = NierEntityState::NierEntityState_Online;
                        break;
                    }
                }
            }
            sLog->outMessage(NIER_MARK, LogLevel::LOG_LEVEL_INFO, "Nier {} is ready to go offline.", character_name);
            entityState = NierEntityState::NierEntityState_DoLogoff;
            break;
        }
        case NierEntityState::NierEntityState_DoLogoff:
        {
            checkDelay = urand(10 * IN_MILLISECONDS, 20 * IN_MILLISECONDS);
            ObjectGuid playerGuid = ObjectGuid(HighGuid::Player, character_id);
            if (Player* me = ObjectAccessor::FindConnectedPlayer(playerGuid))
            {
                if (me->IsInWorld())
                {
                    if (WorldSession* ws = me->GetSession())
                    {
                        me->RemoveFromGroup();
                        ws->LogoutPlayer(true);
                        entityState = NierEntityState::NierEntityState_CheckLogoff;
                        break;
                    }
                }
            }
            entityState = NierEntityState::NierEntityState_OffLine;
            break;
        }
        case NierEntityState::NierEntityState_CheckLogoff:
        {
            ObjectGuid playerGuid = ObjectGuid(HighGuid::Player, character_id);
            if (Player* me = ObjectAccessor::FindConnectedPlayer(playerGuid))
            {
                sLog->outMessage(NIER_MARK, LogLevel::LOG_LEVEL_ERROR, "Log out nier {} failed", me->GetName());
                entityState = NierEntityState::NierEntityState_None;
                break;
            }
            entityState = NierEntityState::NierEntityState_OffLine;
            break;
        }
        case NierEntityState::NierEntityState_RedoLogin:
        {
            entityState = NierEntityState::NierEntityState_OffLine;
            break;
        }
        case NierEntityState::NierEntityState_CheckRedoLogin:
        {
            entityState = NierEntityState::NierEntityState_OffLine;
            break;
        }
        default:
        {
            checkDelay = urand(5 * MINUTE * IN_MILLISECONDS, 10 * MINUTE * IN_MILLISECONDS);
            break;
        }
        }
    }
}
