#include "NingerEntity.h"
#include "NingerManager.h"
#include "NingerConfig.h"
#include "NingerStrategy_Base.h"
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

#include "Group.h"

NingerEntity::NingerEntity()
{
    ninger_id = 0;
    account_id = 0;
    account_name = "";
    character_id = 0;
    target_level = 0;
    target_specialty = 0;
    checkDelay = 5 * IN_MILLISECONDS;
    entityState = NingerEntityState::NingerEntityState_OffLine;
    onlineDelay = 0;
    offlineDelay = 0;
}

void NingerEntity::Update(uint32 pmDiff)
{
    checkDelay -= pmDiff;
    if (checkDelay < 0)
    {
        checkDelay = urand(2 * IN_MILLISECONDS, 10 * IN_MILLISECONDS);
        switch (entityState)
        {
        case NingerEntityState::NingerEntityState_None:
        {
            checkDelay = urand(5 * MINUTE * IN_MILLISECONDS, 10 * MINUTE * IN_MILLISECONDS);
            break;
        }
        case NingerEntityState::NingerEntityState_OffLine:
        {
            checkDelay = urand(5 * MINUTE * IN_MILLISECONDS, 10 * MINUTE * IN_MILLISECONDS);
            break;
        }
        case NingerEntityState::NingerEntityState_Enter:
        {
            entityState = NingerEntityState::NingerEntityState_CheckAccount;
            sLog->outMessage("ninger", LogLevel::LOG_LEVEL_INFO, "Ninger %s is ready to go online.", account_name.c_str());
            break;
        }
        case NingerEntityState::NingerEntityState_CheckAccount:
        {
            if (account_name.empty())
            {
                entityState = NingerEntityState::NingerEntityState_None;
            }
            else
            {
                account_id = AccountMgr::GetId(account_name);
                if (account_id > 0)
                {
                    sLog->outMessage("ninger", LogLevel::LOG_LEVEL_INFO, "Ninger %s is ready.", account_name.c_str());
                    entityState = NingerEntityState::NingerEntityState_CheckCharacter;
                }
                else
                {
                    sLog->outMessage("ninger", LogLevel::LOG_LEVEL_INFO, "Ninger %s is not ready.", account_name.c_str());
                    entityState = NingerEntityState::NingerEntityState_CreateAccount;
                }
            }
            break;
        }
        case NingerEntityState::NingerEntityState_CreateAccount:
        {
            if (account_name.empty())
            {
                entityState = NingerEntityState::NingerEntityState_None;
            }
            else
            {
                if (AccountMgr::CreateAccount(account_name, "ninger") == AccountOpResult::AOR_OK)
                {
                    sLog->outMessage("ninger", LogLevel::LOG_LEVEL_INFO, "Ninger id %d account creation failed.", ninger_id);
                    entityState = NingerEntityState::NingerEntityState_None;
                }
                else
                {
                    entityState = NingerEntityState::NingerEntityState_CheckAccount;
                }
            }
            break;
        }
        case NingerEntityState::NingerEntityState_CheckCharacter:
        {
            QueryResult characterQR = CharacterDatabase.Query("SELECT guid FROM characters where account = {}", account_id);
            if (characterQR)
            {
                Field* characterFields = characterQR->Fetch();
                character_id = characterFields[0].Get<uint32>();
                if (character_id > 0)
                {
                    sLog->outMessage("ninger", LogLevel::LOG_LEVEL_INFO, "Ninger account_id %d character_id %d is ready.", account_id, character_id);
                    entityState = NingerEntityState::NingerEntityState_DoLogin;
                    break;
                }
            }
            sLog->outMessage("ninger", LogLevel::LOG_LEVEL_INFO, "Ninger account_id %d character_id is not ready.", account_id);
            entityState = NingerEntityState::NingerEntityState_CreateCharacter;
            break;
        }
        case NingerEntityState::NingerEntityState_CreateCharacter:
        {
            std::string currentName = "";
            bool nameValid = false;
            uint32 nameIndex = 0;
            while (sNingerManager->ningerNameMap.find(sNingerManager->nameIndex) != sNingerManager->ningerNameMap.end())
            {
                currentName = sNingerManager->ningerNameMap[sNingerManager->nameIndex];
                QueryResult checkNameQR = CharacterDatabase.Query("SELECT count(*) FROM characters where name = '{}'", currentName.c_str());
                if (!checkNameQR)
                {
                    sLog->outMessage("ninger", LogLevel::LOG_LEVEL_INFO, "Name %s is available", currentName.c_str());
                    nameValid = true;
                }
                else
                {
                    Field* nameCountFields = checkNameQR->Fetch();
                    uint32 nameCount = nameCountFields[0].Get<uint32>();
                    if (nameCount == 0)
                    {
                        nameValid = true;
                    }
                }

                nameIndex++;
                if (nameValid)
                {
                    break;
                }
            }
            if (!nameValid)
            {
                sLog->outMessage("ninger", LogLevel::LOG_LEVEL_ERROR, "No available names");
                entityState = NingerEntityState::NingerEntityState_None;
                return;
            }
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
                cci->Name = currentName;
                cci->Race = target_race;
                cci->Class = target_class;
                cci->Gender = gender;
                cci->Skin = skin;
                cci->Face = face;
                cci->HairStyle = hairStyle;
                cci->HairColor = hairColor;
                cci->FacialHair = facialHair;
                cci->OutfitId = 0;

                WorldSession* createSession = new WorldSession(account_id, account_name.c_str(), NULL, SEC_PLAYER, 2, 0, LOCALE_enUS, 0, false, true, 0);
                Player* newPlayer = new Player(createSession);
                if (!newPlayer->Create(sObjectMgr->GetGenerator<HighGuid::Player>().Generate(), cci))
                {
                    newPlayer->CleanupsBeforeDelete();
                    delete createSession;
                    delete newPlayer;
                    sLog->outMessage("ninger", LogLevel::LOG_LEVEL_ERROR, "Character create failed, %s %d %d", currentName.c_str(), target_race, target_class);
                    sLog->outMessage("ninger", LogLevel::LOG_LEVEL_INFO, "Try again");
                    continue;
                }
                newPlayer->GetMotionMaster()->Initialize();
                newPlayer->setCinematic(2);
                newPlayer->SetAtLoginFlag(AT_LOGIN_NONE);
                newPlayer->SaveToDB(true, true);
                character_id = newPlayer->GetGUID().GetCounter();
                if (character_id > 0)
                {
                    createSession->isNinger = true;
                    sWorld->AddSession(createSession);
                    sLog->outMessage("ninger", LogLevel::LOG_LEVEL_INFO, "Create character %d - %s for account %d", newPlayer->GetGUID().GetCounter(), currentName.c_str(), account_id);
                    break;
                }
            }
            if (character_id > 0)
            {
                std::ostringstream sqlStream;
                sqlStream << "update ninger set character_id = " << character_id << " where ninger_id = " << ninger_id;
                std::string sql = sqlStream.str();
                CharacterDatabase.DirectExecute(sql.c_str());
                entityState = NingerEntityState::NingerEntityState_CheckCharacter;
                break;
            }
            entityState = NingerEntityState::NingerEntityState_None;
            break;
        }
        case NingerEntityState::NingerEntityState_DoLogin:
        {
            WorldSession* loginSession = sWorld->FindSession(account_id);
            if (!loginSession)
            {
                loginSession = new WorldSession(account_id, account_name.c_str(), NULL, SEC_PLAYER, 2, 0, LOCALE_enUS, 0, false, true, 0);
                sWorld->AddSession(loginSession);
            }
            loginSession->isNinger = true;
            WorldPacket wpLogin(CMSG_PLAYER_LOGIN, 8);
            wpLogin << character_id;
            loginSession->HandleAcceptTradeOpcode(wpLogin);
            sLog->outMessage("ninger", LogLevel::LOG_LEVEL_INFO, "Log in character %d %d", account_id, character_id);
            checkDelay = urand(5 * IN_MILLISECONDS, 10 * IN_MILLISECONDS);
            entityState = NingerEntityState::NingerEntityState_CheckLogin;
            break;
        }
        case NingerEntityState::NingerEntityState_CheckLogin:
        {
            if (Player* me = ObjectAccessor::FindPlayerByLowGUID(character_id))
            {
                if (me->IsInWorld())
                {
                    entityState = NingerEntityState::NingerEntityState_Initialize;
                    break;
                }
            }
            entityState = NingerEntityState::NingerEntityState_OffLine;
            break;
        }
        case NingerEntityState::NingerEntityState_Initialize:
        {
            if (Player* me = ObjectAccessor::FindPlayerByLowGUID(character_id))
            {
                if (me->IsInWorld())
                {
                    me->strategyMap.clear();
                    me->strategyMap[0] = new NingerStrategy_Base(me);
                    me->activeStrategyIndex = 0;
                    switch (target_class)
                    {
                    case Classes::CLASS_DEATH_KNIGHT:
                    {
                        me->ningerAction = new NingerAction_Base(me);
                        break;
                    }
                    case Classes::CLASS_DRUID:
                    {
                        me->ningerAction = new NingerAction_Druid(me);
                        break;
                    }
                    case Classes::CLASS_HUNTER:
                    {
                        me->ningerAction = new NingerAction_Hunter(me);
                        break;
                    }
                    case Classes::CLASS_MAGE:
                    {
                        me->ningerAction = new NingerAction_Mage(me);
                        break;
                    }
                    case Classes::CLASS_PALADIN:
                    {
                        me->ningerAction = new NingerAction_Paladin(me);
                        break;
                    }
                    case Classes::CLASS_PRIEST:
                    {
                        me->ningerAction = new NingerAction_Priest(me);
                        break;
                    }
                    case Classes::CLASS_ROGUE:
                    {
                        me->ningerAction = new NingerAction_Rogue(me);
                        break;
                    }
                    case Classes::CLASS_SHAMAN:
                    {
                        me->ningerAction = new NingerAction_Shaman(me);
                        break;
                    }
                    case Classes::CLASS_WARLOCK:
                    {
                        me->ningerAction = new NingerAction_Warlock(me);
                        break;
                    }
                    case Classes::CLASS_WARRIOR:
                    {
                        me->ningerAction = new NingerAction_Warrior(me);
                        break;
                    }
                    default:
                    {
                        me->ningerAction = new NingerAction_Base(me);
                        break;
                    }
                    }
                    me->ningerAction->InitializeCharacter(target_level, target_specialty);
                    entityState = NingerEntityState::NingerEntityState_Online;
                    break;
                }
            }
            entityState = NingerEntityState::NingerEntityState_OffLine;
            break;
        }
        case NingerEntityState::NingerEntityState_Online:
        {
            checkDelay = urand(10 * MINUTE * IN_MILLISECONDS, 20 * MINUTE * IN_MILLISECONDS);
            if (Player* me = ObjectAccessor::FindPlayerByLowGUID(character_id))
            {
                if (me->IsInWorld())
                {
                    me->ningerAction->Prepare();
                }
            }
            break;
        }
        case NingerEntityState::NingerEntityState_Exit:
        {
            if (Player* me = ObjectAccessor::FindPlayerByLowGUID(character_id))
            {
                if (me->IsInWorld())
                {
                    if (me->GetGroup())
                    {
                        entityState = NingerEntityState::NingerEntityState_Online;
                        break;
                    }
                }
            }
            sLog->outMessage("ninger", LogLevel::LOG_LEVEL_INFO, "Ninger %d is ready to go offline.", ninger_id);
            entityState = NingerEntityState::NingerEntityState_DoLogoff;
            break;
        }
        case NingerEntityState::NingerEntityState_DoLogoff:
        {
            checkDelay = urand(10 * IN_MILLISECONDS, 20 * IN_MILLISECONDS);
            if (Player* me = ObjectAccessor::FindPlayerByLowGUID(character_id))
            {
                if (me->IsInWorld())
                {
                    if (WorldSession* ws = me->GetSession())
                    {
                        ws->LogoutPlayer(true);
                        entityState = NingerEntityState::NingerEntityState_CheckLogoff;
                        break;
                    }
                }
            }
            entityState = NingerEntityState::NingerEntityState_OffLine;
            break;
        }
        case NingerEntityState::NingerEntityState_CheckLogoff:
        {
            ObjectGuid guid = ObjectGuid(HighGuid::Player, character_id);
            if (Player* me = ObjectAccessor::FindPlayer(guid))
            {
                sLog->outMessage("ninger", LogLevel::LOG_LEVEL_ERROR, "Log out ninger %s failed", me->GetName());
                entityState = NingerEntityState::NingerEntityState_None;
                break;
            }
            entityState = NingerEntityState::NingerEntityState_OffLine;
            break;
        }
        case NingerEntityState::NingerEntityState_RedoLogin:
        {
            entityState = NingerEntityState::NingerEntityState_OffLine;
            break;
        }
        case NingerEntityState::NingerEntityState_CheckRedoLogin:
        {
            entityState = NingerEntityState::NingerEntityState_OffLine;
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
