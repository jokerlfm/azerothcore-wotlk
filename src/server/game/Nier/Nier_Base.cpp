#include "Nier_Base.h"
#include "World/World.h"
#include "Player.h"
#include "Pet.h"
#include "CreatureAI.h"
#include "Spell.h"
#include "GridNotifiers.h"
#include "Map.h"
#include "SpellMgr.h"
#include "SpellAuras.h"
#include "Bag.h"
#include "TargetedMovementGenerator.h"
#include "WorldSessionMgr.h"

Nier_Base::Nier_Base()
{
    nier_id = 0;
    master_character_id = 0;
    account_id = 0;
    account_name = "";
    character_id = 0;
    target_level = 0;
    target_specialty = 0;
    target_race = 0;
    target_class = 0;

    me = nullptr;
    actionDuration = 0;
    actionTimeLimit = 0;
    actionTargetUnit = nullptr;
    actionTargetSpell = 0;

    followDistance = 0.0f;

    specialty = 0;
    accountState = NierAccountState::NierAccountState_None;
    actionState = NierActionState::NierActionState_None;

    timeValue = 0;
    checkDelay = 0;
}

void Nier_Base::ClearAction()
{
    actionState = NierActionState::NierActionState_None;
    actionDuration = 0;
    actionTimeLimit = 0;
    actionTargetUnit = nullptr;
    if (me)
    {
        actionTargetPos = me->GetPosition();
    }
    actionTargetSpell = 0;
}

void Nier_Base::Prepare()
{
    if (me)
    {
        me->SetPvP(true);
        me->UpdatePvP(true);
        me->DurabilityRepairAll(false, 0, false);
        if (me->GetMap()->Instanceable())
        {
            me->TeleportTo(me->m_homebindMapId, me->m_homebindX, me->m_homebindY, me->m_homebindZ, me->GetOrientation());
        }
        followDistance = frand(1.0f, 10.0f);
    }
}

void Nier_Base::Update(uint64 pTimeValue)
{
    uint64 elapsed = pTimeValue - timeValue;
    timeValue = pTimeValue;
    checkDelay -= elapsed;
    actionDuration += elapsed;
    if (checkDelay > 0)
    {
        return;
    }

    if (UpdateAccount())
    {
        return;
    }
    if (UpdateAction())
    {
        return;
    }
    if (UpdateMind())
    {
        return;
    }
}

bool Nier_Base::UpdateAccount()
{
    bool accountResult = true;

    std::ostringstream replyStream;
    switch (accountState)
    {
    case NierAccountState_None:
    {
        checkDelay = urand(5 * MINUTE * IN_MILLISECONDS, 10 * MINUTE * IN_MILLISECONDS);
        break;
    }
    case NierAccountState_OffLine:
    {
        checkDelay = urand(5 * MINUTE * IN_MILLISECONDS, 10 * MINUTE * IN_MILLISECONDS);
        break;
    }
    case NierAccountState_Enter:
    {
        accountState = NierAccountState::NierAccountState_CheckAccount;
        replyStream << "Nier enter " << nier_id << " - " << account_id << " - " << character_id << " - ";
        sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, replyStream.str());
        break;
    }
    case NierAccountState_CheckAccount:
    {
        if (account_name.empty())
        {
            accountState = NierAccountState::NierAccountState_None;
            checkDelay = urand(5 * MINUTE * IN_MILLISECONDS, 10 * MINUTE * IN_MILLISECONDS);
            replyStream << "invalid nier";
            sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, replyStream.str());
            break;
        }
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
            replyStream << "Nier account is ready - " << account_name;
            accountState = NierAccountState::NierAccountState_CheckCharacter;
        }
        else
        {
            replyStream << "Nier account is not ready - " << account_name;
            accountState = NierAccountState::NierAccountState_CreateAccount;
        }
        sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, replyStream.str());
        break;
    }
    case NierAccountState_CreateAccount:
    {
        if (account_name.empty())
        {
            accountState = NierAccountState::NierAccountState_None;
            checkDelay = urand(5 * MINUTE * IN_MILLISECONDS, 10 * MINUTE * IN_MILLISECONDS);
            replyStream << "invalid nier";
            sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, replyStream.str());
            break;
        }
        if (AccountMgr::CreateAccount(account_name, NIER_MARK) == AccountOpResult::AOR_OK)
        {
            accountState = NierAccountState::NierAccountState_CheckAccount;
        }
        else
        {
            replyStream << "Nier account can not be created - " << nier_id;
            sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, replyStream.str());
            accountState = NierAccountState::NierAccountState_None;
            checkDelay = urand(5 * MINUTE * IN_MILLISECONDS, 10 * MINUTE * IN_MILLISECONDS);
        }
        break;
    }
    case NierAccountState_CheckCharacter:
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
                std::ostringstream sqlStream;
                sqlStream << "update nier set character_id = " << character_id << " where nier_id = " << nier_id;
                std::string sql = sqlStream.str();
                CharacterDatabase.DirectExecute(sql.c_str());
                replyStream << "Nier is ready - " << account_id << " - " << character_id;
                sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, replyStream.str());
                accountState = NierAccountState::NierAccountState_DoEnum;
                //accountState = NierAccountState::NierAccountState_DoLogin;
                checkDelay = urand(2 * IN_MILLISECONDS, 5 * IN_MILLISECONDS);
                break;
            }
        }
        replyStream << "Nier is not ready - " << account_id;
        sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, replyStream.str());
        accountState = NierAccountState::NierAccountState_CreateCharacter;
        checkDelay = urand(2 * IN_MILLISECONDS, 5 * IN_MILLISECONDS);
        break;
    }
    case NierAccountState_CreateCharacter:
    {
        std::string currentName = "";
        bool nameValid = false;
        while (sNierManager->nierNameMap.find(sNierManager->nameIndex) != sNierManager->nierNameMap.end())
        {
            currentName = sNierManager->nierNameMap[sNierManager->nameIndex];
            std::ostringstream queryStream;
            queryStream << "SELECT count(*) FROM characters where name = '" << currentName << "'";
            QueryResult checkNameQR = CharacterDatabase.Query(queryStream.str().c_str());
            if (!checkNameQR)
            {
                replyStream << "Name is available - " << currentName;
                sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, replyStream.str());
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
            sNierManager->nameIndex++;
            if (nameValid)
            {
                break;
            }
        }
        if (!nameValid)
        {
            replyStream << "No available names";
            sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, replyStream.str());
            accountState = NierAccountState::NierAccountState_None;
            checkDelay = urand(5 * MINUTE * IN_MILLISECONDS, 10 * MINUTE * IN_MILLISECONDS);
            return false;
        }
        uint8 gender = 0, skin = 0, face = 0, hairStyle = 0, hairColor = 0, facialHair = 0;
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

        if (WorldSession* createSession = new WorldSession(account_id, std::move(account_name), NULL, AccountTypes::SEC_PLAYER, 2, 0, LocaleConstant::LOCALE_enUS, 0, false, true, 0))
        {
            uint32 newCharacterId = sObjectMgr->GetGenerator<HighGuid::Player>().Generate();
            if (Player* newPlayer = new Player(createSession))
            {
                if (newPlayer->Create(sObjectMgr->GetGenerator<HighGuid::Player>().Generate(), cci))
                {
                    character_id = newCharacterId;
                    newPlayer->SaveToDB(true, true);
                    std::ostringstream replyStream;
                    replyStream << "nier character created : " << account_id << " - " << character_id << " - " << currentName;
                    sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, replyStream.str());
                    accountState = NierAccountState::NierAccountState_CheckCharacter;
                    checkDelay = urand(2 * IN_MILLISECONDS, 5 * IN_MILLISECONDS);
                }
                else
                {
                    newPlayer->CleanupsBeforeDelete();
                    delete newPlayer;
                    replyStream << "create character failed - " << currentName << " - " << target_race << " - " << target_class;
                    sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, replyStream.str());
                    accountState = NierAccountState::NierAccountState_None;
                    checkDelay = urand(5 * MINUTE * IN_MILLISECONDS, 10 * MINUTE * IN_MILLISECONDS);
                }
            }
            else
            {
                replyStream << "create character failed - " << currentName << " - " << target_race << " - " << target_class;
                sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, replyStream.str());
                accountState = NierAccountState::NierAccountState_None;
                checkDelay = urand(5 * MINUTE * IN_MILLISECONDS, 10 * MINUTE * IN_MILLISECONDS);
            }
            delete createSession;
        }
        else
        {
            replyStream << "create session failed - " << nier_id;
            sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, replyStream.str());
            accountState = NierAccountState::NierAccountState_None;
            checkDelay = urand(5 * MINUTE * IN_MILLISECONDS, 10 * MINUTE * IN_MILLISECONDS);
        }
        break;
    }
    case NierAccountState_CheckLogin:
    {
        checkDelay = urand(2 * IN_MILLISECONDS, 5 * IN_MILLISECONDS);
        ObjectGuid playerGuid = ObjectGuid(HighGuid::Player, character_id);
        if (Player* targetPlayer = ObjectAccessor::FindPlayer(playerGuid))
        {
            if (targetPlayer->IsInWorld())
            {
                sCharacterCache->RefreshCacheEntry(character_id);
                targetPlayer->nier = this;
                me = targetPlayer;
                replyStream << "nier character logged in - " << account_id << " - " << character_id;
                sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, replyStream.str());
                accountState = NierAccountState::NierAccountState_Initialize;
                break;
            }
        }
        break;
    }
    case NierAccountState_DoLogin:
    {
        WorldSession* loginSession = sWorldSessionMgr->FindSession(account_id);
        if (!loginSession)
        {
            loginSession = new WorldSession(account_id, std::move(account_name), NULL, AccountTypes::SEC_PLAYER, 2, 0, LocaleConstant::LOCALE_enUS, 0, false, true, 0);
            sWorldSessionMgr->AddSession(loginSession);
        }
        loginSession->nier_id = nier_id;
        ObjectGuid playerGuid = ObjectGuid(HighGuid::Player, character_id);
        WorldPacket wpLogin(CMSG_PLAYER_LOGIN, 16);
        wpLogin << playerGuid;
        loginSession->HandlePlayerLoginOpcode(wpLogin);
        replyStream << "log in character - " << account_id << " - " << character_id;
        sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, replyStream.str());
        accountState = NierAccountState::NierAccountState_CheckLogin;
        checkDelay = urand(2 * IN_MILLISECONDS, 5 * IN_MILLISECONDS);
        break;
    }
    case NierAccountState_DoEnum:
    {
        WorldSession* loginSession = sWorldSessionMgr->FindSession(account_id);
        if (!loginSession)
        {
            loginSession = new WorldSession(account_id, std::move(account_name), NULL, AccountTypes::SEC_PLAYER, 2, 0, LocaleConstant::LOCALE_enUS, 0, false, true, 0);
            sWorldSessionMgr->AddSession(loginSession);
        }
        loginSession->nier_id = nier_id;
        WorldPacket wpEnum(CMSG_CHAR_ENUM, 4);
        loginSession->HandleCharEnumOpcode(wpEnum);
        replyStream << "enum character - " << account_id << " - " << character_id;
        sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, replyStream.str());
        checkDelay = urand(5 * IN_MILLISECONDS, 10 * IN_MILLISECONDS);
        accountState = NierAccountState::NierAccountState_CheckEnum;
        break;
    }
    case NierAccountState_CheckEnum:
    {
        checkDelay = urand(5 * IN_MILLISECONDS, 10 * IN_MILLISECONDS);
        break;
    }
    case NierAccountState_Initialize:
    {
        if (me->IsInWorld())
        {
            ObjectGuid masterGuid = ObjectGuid(HighGuid::Player, master_character_id);
            if (Player* master = ObjectAccessor::FindPlayer(masterGuid))
            {
                if (master->IsInWorld())
                {
                    InitializeCharacter(master->GetLevel());
                    replyStream << "nier initialized : " << account_id << " - " << character_id << " - " << me->GetName();
                    sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, replyStream.str());

                    // role initialize
                    uint32 meClass = me->getClass();
                    if (meClass == Classes::CLASS_WARRIOR)
                    {
                        me->nierGroupRole = NierGroupRole::NierGroupRole_Tank;
                    }
                    else if (meClass == Classes::CLASS_PRIEST)
                    {
                        me->nierGroupRole = NierGroupRole::NierGroupRole_Healer;
                    }
                    else
                    {
                        me->nierGroupRole = NierGroupRole::NierGroupRole_DPS;
                    }

                    accountState = NierAccountState::NierAccountState_Equip;
                    checkDelay = urand(2 * IN_MILLISECONDS, 5 * IN_MILLISECONDS);
                    break;
                }
            }
        }
        checkDelay = urand(10 * IN_MILLISECONDS, 20 * IN_MILLISECONDS);
        break;
    }
    case NierAccountState_Equip:
    {
        if (me->IsInWorld())
        {
            for (uint32 equipSlot = EquipmentSlots::EQUIPMENT_SLOT_HEAD; equipSlot < EquipmentSlots::EQUIPMENT_SLOT_TABARD; equipSlot++)
            {
                EquipRandomItem(equipSlot);
            }
            replyStream << "nier equipped : " << account_id << " - " << character_id << " - " << me->GetName();
            sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, replyStream.str());
            Prepare();
            accountState = NierAccountState::NierAccountState_Online;
            checkDelay = urand(1 * IN_MILLISECONDS, 3 * IN_MILLISECONDS);
        }
        break;
    }
    case NierAccountState_Online:
    {
        accountResult = false;
        break;
    }
    case NierAccountState_Exit:
    {
        break;
    }
    case NierAccountState_CheckLogoff:
    {
        break;
    }
    case NierAccountState_DoLogoff:
    {
        break;
    }
    default:
    {
        break;
    }
    }

    return accountResult;
}

bool Nier_Base::UpdateAction()
{
    bool actionResult = true;

    switch (actionState)
    {
    case NierActionState_None:
    {
        actionResult = false;
        break;
    }
    case NierActionState_Idle:
    {
        break;
    }
    case NierActionState_Wander:
    {
        break;
    }
    case NierActionState_Follow:
    {
        Follow();
        break;
    }
    case NierActionState_Tank:
    {
        break;
    }
    case NierActionState_Attack:
    {
        if (!Attack(actionTargetUnit))
        {
            ClearAction();
            actionResult = false;
        }
        break;
    }
    case NierActionState_Heal:
    {
        if (!Heal(actionTargetUnit))
        {
            ClearAction();
            actionResult = false;
        }
        break;
    }
    case NierActionState_Cast:
    {
        if (!CastSpell(actionTargetUnit, actionTargetSpell))
        {
            ClearAction();
            actionResult = false;
        }
        break;
    }
    case NierActionState_Assist:
    {
        break;
    }
    case NierActionState_Rest:
    {
        if (me->IsInCombat())
        {
            ClearAction();
            actionResult = false;
        }
        else
        {
            if (Eat())
            {
                break;
            }
            if (Drink())
            {
                break;
            }
        }
        break;
    }
    case NierActionState::NierActionState_Freeze:
    {
        if (me->isMoving())
        {
            me->StopMoving();
        }
        actionTimeLimit = 1800000;
        break;
    }
    case NierActionState::NierActionState_Assemble:
    {
        if (actionDuration > actionTimeLimit)
        {
            if (actionTargetUnit)
            {
                if (Player* targetPlayer = actionTargetUnit->ToPlayer())
                {
                    me->TeleportTo(actionTargetUnit->GetWorldLocation());
                    sNierManager->WhisperTo(targetPlayer, "coming", me);
                    if (!me->IsAlive())
                    {
                        actionState = NierActionState::NierActionState_Corpse;
                        actionDuration = 0;
                        actionTimeLimit = urand(1000, 3000);
                        actionTargetSpell = 0;
                    }
                }
            }
        }
        actionResult = false;
        break;
    }
    case NierActionState::NierActionState_Bunch:
    {
        if (!me->isMoving())
        {
            if (actionTargetUnit)
            {
                if (me->GetDistance(actionTargetUnit) > CONTACT_DISTANCE)
                {
                    MoveToPosition(actionTargetUnit->GetPosition());
                }
            }
        }
        break;
    }
    case NierActionState::NierActionState_Corpse:
    {
        if (actionDuration > actionTimeLimit)
        {
            if (!me->IsAlive())
            {
                me->ResurrectPlayer(10.0f);
                me->Say("revived", Language::LANG_UNIVERSAL);
            }
        }
        actionResult = false;
        break;
    }
    case NierActionState::NierActionState_Revive:
    {
        break;
    }
    default:
    {
        break;
    }
    }

    if (actionDuration > actionTimeLimit)
    {
        ClearAction();
        actionResult = false;
    }

    return actionResult;
}

bool Nier_Base::UpdateMind()
{
    if (!me)
    {
        return false;
    }
    if (!me->IsInWorld())
    {
        return false;
    }
    if (!me->IsAlive())
    {
        return false;
    }
    if (!me->CanFreeMove())
    {
        return true;
    }
    if (Group* meGroup = me->GetGroup())
    {
        // grouping
        ObjectGuid ogSkull = meGroup->GetGuidByTargetIcon(7);
        if (!ogSkull.IsEmpty())
        {
            if (Unit* enemy = ObjectAccessor::GetCreature(*me, ogSkull))
            {
                if (enemy->IsInCombat())
                {
                    float enemyDistance = me->GetDistance(enemy);
                    if (enemyDistance < VISIBILITY_DISTANCE_NORMAL)
                    {
                        switch (me->nierGroupRole)
                        {
                        case NierGroupRole::NierGroupRole_Tank:
                        {
                            if (Tank(enemy))
                            {
                                return true;
                            }
                            break;
                        }
                        case NierGroupRole::NierGroupRole_Healer:
                        {
                            ObjectGuid ogTank = meGroup->GetGuidByTargetIcon(0);
                            if (!ogTank.IsEmpty())
                            {
                                if (Player* tank = ObjectAccessor::FindPlayer(ogTank))
                                {
                                    float tankDistance = me->GetDistance(tank);
                                    if (tankDistance < VISIBILITY_DISTANCE_NORMAL)
                                    {
                                        if (Heal(tank))
                                        {
                                            return true;
                                        }
                                    }
                                }
                            }
                            break;
                        }
                        case NierGroupRole::NierGroupRole_DPS:
                        {
                            if (me->IsInCombat())
                            {
                                if (Attack(enemy))
                                {
                                    return true;
                                }
                            }
                            break;
                        }
                        default:
                        {
                            break;
                        }
                        }
                    }
                }
            }
        }
        if (Rest())
        {
            return true;
        }
        for (GroupReference* groupRef = meGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
        {
            if (Player* member = groupRef->GetSource())
            {
                if (Buff(member))
                {
                    return true;
                }
                if (Revive(member))
                {
                    actionState = NierActionState::NierActionState_Revive;
                    actionTargetUnit = member;
                    actionDuration = 0;
                    actionTimeLimit = 12000;
                    return true;
                }
            }
        }

        if (Follow())
        {
            return true;
        }
    }
    else
    {
        // solo
        if (me->IsInCombat())
        {
            Unit* enemy = nullptr;
            for (auto const& pAttacker : me->getAttackers())
            {
                float attackerDistance = me->GetDistance(pAttacker);
                if (attackerDistance < DEFAULT_VISIBILITY_DISTANCE)
                {
                    enemy = pAttacker;
                    if (pAttacker->GetTypeId() == TypeID::TYPEID_PLAYER)
                    {
                        break;
                    }
                }
            }
            Attack(enemy);
        }
        else
        {
            if (Rest())
            {
                return true;
            }
            uint32 actionRate = urand(0, 100);
            if (actionRate < 50)
            {
                if (Wander())
                {
                    return true;
                }
            }
            else if (actionRate < 70)
            {
                if (PVE())
                {
                    return true;
                }
            }
            else if (actionRate < 90)
            {
                if (PVP())
                {
                    return true;
                }
            }
            else
            {
                if (Idle())
                {
                    return true;
                }
            }
        }
    }

    return false;
}

bool Nier_Base::Rest()
{
    float hpp = me->GetHealthPct();
    float mpp = 100.0f;
    uint32 meClass = me->getClass();
    if (meClass == Classes::CLASS_HUNTER || meClass == Classes::CLASS_SHAMAN || meClass == Classes::CLASS_PALADIN || meClass == Classes::CLASS_PRIEST || meClass == Classes::CLASS_WARLOCK || meClass == Classes::CLASS_MAGE || meClass == Classes::CLASS_DRUID)
    {
        mpp = me->GetPowerPct(Powers::POWER_MANA);
    }
    if (hpp < 50.0f || mpp < 50.0f)
    {
        actionState = NierActionState::NierActionState_Rest;
        actionTimeLimit = 15000;
        return true;
    }

    return false;
}

bool Nier_Base::PVP()
{
    if (Player* target = GetNearbyHostilePlayer())
    {
        if (Attack(target))
        {
            ClearAction();
            actionState = NierActionState::NierActionState_Attack;
            actionTimeLimit = 20000;
            actionTargetUnit = target;
            actionTargetPos = target->GetPosition();

            return true;
        }
    }

    return false;
}

bool Nier_Base::PVE()
{
    if (Unit* target = GetNearbyHostileUnit())
    {
        if (Attack(target))
        {
            ClearAction();
            actionState = NierActionState::NierActionState_Attack;
            actionTimeLimit = 20000;
            actionTargetUnit = target;
            actionTargetPos = target->GetPosition();

            return true;
        }
    }

    return false;
}

bool Nier_Base::Wander()
{
    ClearAction();
    float distance = frand(VISIBILITY_DISTANCE_TINY, VISIBILITY_DISTANCE_SMALL);
    float angle = frand(0.0f, 2 * M_PI);
    me->GetNearPoint(me, actionTargetPos.m_positionX, actionTargetPos.m_positionY, actionTargetPos.m_positionZ, me->GetObjectSize(), distance, angle);
    MoveToPosition(actionTargetPos, false);
    actionState = NierActionState::NierActionState_Wander;
    actionTimeLimit = urand(5000, 10000);

    return true;
}

bool Nier_Base::Idle()
{
    ClearAction();
    actionState = NierActionState::NierActionState_Idle;
    actionTimeLimit = urand(5000, 10000);

    return true;
}

bool Nier_Base::Attack(Unit* pTarget)
{
    if (!me)
    {
        return false;
    }
    if (!me->IsAlive())
    {
        return false;
    }
    if (!pTarget)
    {
        return false;
    }
    if (!me->IsValidAttackTarget(pTarget))
    {
        return false;
    }

    return true;
}

bool Nier_Base::Tank(Unit* pTarget)
{
    if (!me)
    {
        return false;
    }
    if (me->nierGroupRole != NierGroupRole::NierGroupRole_Tank)
    {
        return false;
    }
    if (!me->IsAlive())
    {
        return false;
    }
    if (!pTarget)
    {
        return false;
    }
    if (!me->IsValidAttackTarget(pTarget))
    {
        return false;
    }

    return true;
}

bool Nier_Base::Heal(Unit* pTarget)
{
    if (!me)
    {
        return false;
    }
    if (!me->IsAlive())
    {
        return false;
    }
    if (!pTarget)
    {
        return false;
    }
    if (!pTarget->IsAlive())
    {
        return false;
    }

    return true;
}

bool Nier_Base::Follow()
{
    if (!me)
    {
        return false;
    }
    else if (!me->IsAlive())
    {
        return false;
    }
    if (me->IsNonMeleeSpellCast(false, false, true))
    {
        return true;
    }
    if (Group* meGroup = me->GetGroup())
    {
        if (Player* leader = meGroup->GetLeader())
        {
            ChooseTarget(leader);

            if (me->GetDistance(leader) < followDistance)
            {
                if (me->isMoving())
                {
                    me->StopMoving();
                }
                if (!me->isInFront(leader, M_PI_2))
                {
                    me->SetFacingToObject(leader);
                }
            }
            else
            {
                float destPosTargetDist = leader->GetDistance(actionTargetPos);
                if (destPosTargetDist > followDistance + 1.0f)
                {
                    leader->GetNearPoint(leader, actionTargetPos.m_positionX, actionTargetPos.m_positionY, actionTargetPos.m_positionZ, 0.0f, followDistance - 1.0f, leader->GetAbsoluteAngle(me));
                    me->GetMotionMaster()->MovePoint(0, actionTargetPos);
                }
                else
                {
                    float destPosMeDist = me->GetDistance(actionTargetPos);
                    if (destPosMeDist > CONTACT_DISTANCE)
                    {
                        if (!me->isMoving())
                        {
                            me->GetMotionMaster()->MovePoint(0, actionTargetPos);
                        }
                    }
                }
            }
        }

        return true;
    }

    return false;
}

bool Nier_Base::Cure(Unit* pTarget)
{
    return false;
}

bool Nier_Base::Buff(Unit* pTarget)
{
    if (!me)
    {
        return false;
    }
    if (!me->IsAlive())
    {
        return false;
    }
    if (!pTarget)
    {
        return false;
    }
    if (!pTarget->IsAlive())
    {
        return false;
    }

    return true;
}

bool Nier_Base::Revive(Unit* pTarget)
{
    if (!me)
    {
        return false;
    }
    if (!me->IsAlive())
    {
        return false;
    }
    if (pTarget->GetTypeId() != TypeID::TYPEID_PLAYER)
    {
        return false;
    }
    if (pTarget->IsAlive())
    {
        return false;
    }
    if (me->GetDistance(pTarget) > VISIBILITY_DISTANCE_TINY)
    {
        return false;
    }

    return true;
}

bool Nier_Base::InitializeCharacter(uint32 pTargetLevel)
{
    if (!me)
    {
        return false;
    }

    return true;
}

bool Nier_Base::ResetTalentsAndSpells()
{
    if (!me)
    {
        return false;
    }
    me->resetTalents(true);
    me->SendTalentsInfoData(false);

    return true;
}

void Nier_Base::RemoveEquipments()
{
    if (!me)
    {
        return;
    }
    for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; ++slot)
    {
        if (Item* inventoryItem = me->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
        {
            me->DestroyItem(INVENTORY_SLOT_BAG_0, slot, true);
        }
    }
    for (uint32 checkEquipSlot = EquipmentSlots::EQUIPMENT_SLOT_HEAD; checkEquipSlot < EquipmentSlots::EQUIPMENT_SLOT_TABARD; checkEquipSlot++)
    {
        if (Item* currentEquip = me->GetItemByPos(INVENTORY_SLOT_BAG_0, checkEquipSlot))
        {
            me->DestroyItem(INVENTORY_SLOT_BAG_0, checkEquipSlot, true);
        }
    }
}

void Nier_Base::LearnTalent(uint32 pTalentId, uint32 pMaxRank)
{
    if (!me)
    {
        return;
    }
    uint32 checkRank = 0;
    while (checkRank < pMaxRank)
    {
        me->LearnTalent(pTalentId, checkRank);
        checkRank++;
    }
}

void Nier_Base::TrainSpells(uint32 pTrainerEntry)
{
    TrainerSpellData const* trainer_spells = sObjectMgr->GetNpcTrainerSpells(pTrainerEntry);
    bool hasNew = false;
    while (true)
    {
        hasNew = false;
        for (TrainerSpellMap::const_iterator itr = trainer_spells->spellList.begin(); itr != trainer_spells->spellList.end(); ++itr)
        {
            TrainerSpell const* tSpell = &itr->second;
            if (me->HasSpell(tSpell->spell))
            {
                continue;
            }
            if (tSpell->reqSpell && !me->HasSpell(tSpell->reqSpell))
            {
                continue;
            }
            TrainerSpellState state = me->GetTrainerSpellState(tSpell);
            if (state == TrainerSpellState::TRAINER_SPELL_GREEN)
            {
                me->learnSpell(tSpell->spell);
                hasNew = true;
            }
        }
        if (!hasNew)
        {
            break;
        }
    }
}

void Nier_Base::EquipRandomItem(uint32 pEquipSlot)
{

}

void Nier_Base::PetAttack(Unit* pmTarget)
{
    if (me)
    {
        if (Pet* myPet = me->GetPet())
        {
            if (myPet->IsAlive())
            {
                if (CreatureAI* cai = myPet->AI())
                {
                    cai->AttackStart(pmTarget);
                }
            }
        }
    }
}

void Nier_Base::PetStop()
{
    if (me)
    {
        if (Pet* myPet = me->GetPet())
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
    }
}

bool Nier_Base::UseItem(Item* pItem, Unit* pTarget)
{
    if (!me)
    {
        return false;
    }
    if (me->CanUseItem(pItem) != EQUIP_ERR_OK)
    {
        return false;
    }

    if (me->IsNonMeleeSpellCast(false, false, true))
    {
        return false;
    }

    if (ItemTemplate const* proto = pItem->GetTemplate())
    {
        for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
        {
            _Spell const& spellData = proto->Spells[i];
            if (sNierManager->HasAura(me, spellData.SpellId))
            {
                return false;
            }
        }
    }
    if (const ItemTemplate* proto = pItem->GetTemplate())
    {
        ChooseTarget(pTarget);
        SpellCastTargets targets;
        targets.Update(pTarget);
        me->CastItemUseSpell(pItem, targets, 1, 0);
        return true;
    }

    return false;
}

bool Nier_Base::UseItem(Item* pItem, Item* pTarget)
{
    if (!me)
    {
        return false;
    }
    if (me->CanUseItem(pItem) != EQUIP_ERR_OK)
    {
        return false;
    }
    if (me->IsNonMeleeSpellCast(false, false, true))
    {
        return false;
    }
    if (!pTarget)
    {
        return false;
    }

    if (const ItemTemplate* proto = pItem->GetTemplate())
    {
        SpellCastTargets targets;
        targets.SetItemTarget(pTarget);
        me->CastItemUseSpell(pItem, targets, 1, 0);
        return true;
    }

    return false;
}

bool Nier_Base::CastSpell(Unit* pmTarget, uint32 pmSpellId, bool pmCheckAura, bool pmOnlyMyAura, bool pmClearShapeShift, uint32 pmMaxAuraStack)
{
    if (!SpellValid(pmSpellId))
    {
        return false;
    }
    if (!me)
    {
        return false;
    }
    if (me->IsNonMeleeSpellCast(false, false, true))
    {
        return true;
    }
    if (pmClearShapeShift)
    {
        me->RemoveAurasByType(SPELL_AURA_MOD_SHAPESHIFT);
    }
    if (const SpellInfo* pS = sSpellMgr->GetSpellInfo(pmSpellId))
    {
        if (pmTarget)
        {
            if (!me->IsWithinLOSInMap(pmTarget))
            {
                return false;
            }
            if (pmTarget->IsImmunedToSpell(pS))
            {
                return false;
            }
            if (pmCheckAura)
            {
                Aura* targetAura = nullptr;
                if (pmOnlyMyAura)
                {
                    targetAura = pmTarget->GetAura(pmSpellId, me->GetGUID());
                }
                else
                {
                    targetAura = pmTarget->GetAura(pmSpellId);
                }
                if (targetAura)
                {
                    if (targetAura->GetStackAmount() >= pmMaxAuraStack)
                    {
                        return false;
                    }
                }
            }
            if (!me->isInFront(pmTarget, M_PI_2))
            {
                me->SetFacingToObject(pmTarget);
            }
            if (me->GetTarget() != pmTarget->GetGUID())
            {
                ChooseTarget(pmTarget);
            }
        }
        for (size_t i = 0; i < MAX_SPELL_REAGENTS; i++)
        {
            if (pS->Reagent[i] > 0)
            {
                if (!me->HasItemCount(pS->Reagent[i], pS->ReagentCount[i]))
                {
                    me->StoreNewItemInBestSlots(pS->Reagent[i], pS->ReagentCount[i] * 10);
                }
            }
        }
        if (me->getStandState() != UnitStandStateType::UNIT_STAND_STATE_STAND)
        {
            me->SetStandState(UNIT_STAND_STATE_STAND);
        }
        //me->CastSpell(pmTarget, pS, TriggerCastFlags::TRIGGERED_NONE);
        //return true;

        SpellCastResult scr = me->CastSpell(pmTarget, pS->Id, TriggerCastFlags::TRIGGERED_NONE);
        if (scr == SpellCastResult::SPELL_CAST_OK)
        {
            return true;
        }
    }

    return false;
}

void Nier_Base::CancelAura(uint32 pmSpellID)
{
    if (pmSpellID == 0)
    {
        return;
    }
    if (!me)
    {
        return;
    }
    me->RemoveAurasDueToSpell(pmSpellID);
}

bool Nier_Base::Eat()
{
    uint32 foodEntry = 0;
    uint32 myLevel = me->GetLevel();
    if (myLevel >= 75)
    {
        foodEntry = 35950;
    }
    else if (myLevel >= 65)
    {
        foodEntry = 33449;
    }
    else if (myLevel >= 55)
    {
        foodEntry = 21023;
    }
    else if (myLevel >= 45)
    {
        foodEntry = 8950;
    }
    else if (myLevel >= 35)
    {
        foodEntry = 4601;
    }
    else if (myLevel >= 25)
    {
        foodEntry = 4544;
    }
    else if (myLevel >= 15)
    {
        foodEntry = 4542;
    }
    else if (myLevel >= 5)
    {
        foodEntry = 4541;
    }
    else
    {
        foodEntry = 4540;
    }
    if (!me->HasItemCount(foodEntry, 1))
    {
        me->StoreNewItemInBestSlots(foodEntry, 20);
    }
    if (Item* pFood = GetItemInInventory(foodEntry))
    {
        if (UseItem(pFood, me))
        {
            return true;
        }
    }

    return false;
}

bool Nier_Base::Drink()
{
    uint32 drinkEntry = 0;
    uint32 myLevel = me->GetLevel();
    if (myLevel >= 75)
    {
        drinkEntry = 33445;
    }
    else if (myLevel >= 70)
    {
        drinkEntry = 33444;
    }
    else if (myLevel >= 65)
    {
        drinkEntry = 27860;
    }
    else if (myLevel >= 60)
    {
        drinkEntry = 28399;
    }
    else if (myLevel >= 55)
    {
        drinkEntry = 18300;
    }
    else if (myLevel >= 45)
    {
        drinkEntry = 8766;
    }
    else if (myLevel >= 35)
    {
        drinkEntry = 1645;
    }
    else if (myLevel >= 25)
    {
        drinkEntry = 1708;
    }
    else if (myLevel >= 15)
    {
        drinkEntry = 1205;
    }
    else if (myLevel >= 5)
    {
        drinkEntry = 1179;
    }
    else
    {
        drinkEntry = 159;
    }

    if (!me->HasItemCount(drinkEntry, 1))
    {
        me->StoreNewItemInBestSlots(drinkEntry, 20);
    }
    uint32 itemSpellId = 0;
    if (Item* pDrink = GetItemInInventory(drinkEntry))
    {
        if (UseItem(pDrink, me))
        {
            return true;
        }
    }

    return false;
}

bool Nier_Base::HealthPotion()
{
    if (!me)
    {
        return false;
    }
    else if (!me->IsAlive())
    {
        return false;
    }
    else if (!me->IsInCombat())
    {
        return false;
    }
    uint32 potionEntry = 0;
    uint32 myLevel = me->GetLevel();
    if (myLevel >= 70)
    {
        potionEntry = 33447;
    }
    else if (myLevel >= 55)
    {
        potionEntry = 22829;
    }
    else if (myLevel >= 45)
    {
        potionEntry = 13446;
    }
    else if (myLevel >= 35)
    {
        potionEntry = 3928;
    }
    else if (myLevel >= 21)
    {
        potionEntry = 1710;
    }
    else if (myLevel >= 12)
    {
        potionEntry = 929;
    }
    else if (myLevel >= 3)
    {
        potionEntry = 858;
    }
    else
    {
        potionEntry = 118;
    }
    if (!me->HasItemCount(potionEntry, 1))
    {
        me->StoreNewItemInBestSlots(potionEntry, 20);
    }
    if (Item* pPotion = GetItemInInventory(potionEntry))
    {
        if (UseItem(pPotion, me))
        {
            return true;
        }
    }
    return false;
}

bool Nier_Base::ManaPotion()
{
    if (!me)
    {
        return false;
    }
    else if (!me->IsAlive())
    {
        return false;
    }
    else if (!me->IsInCombat())
    {
        return false;
    }
    uint32 potionEntry = 0;
    uint32 myLevel = me->GetLevel();
    if (myLevel >= 70)
    {
        potionEntry = 33448;
    }
    else if (myLevel >= 55)
    {
        potionEntry = 22832;
    }
    else if (myLevel >= 49)
    {
        potionEntry = 13444;
    }
    else if (myLevel >= 41)
    {
        potionEntry = 13443;
    }
    else if (myLevel >= 31)
    {
        potionEntry = 6149;
    }
    else if (myLevel >= 22)
    {
        potionEntry = 3827;
    }
    else if (myLevel >= 14)
    {
        potionEntry = 3385;
    }
    else if (myLevel >= 5)
    {
        potionEntry = 2455;
    }
    if (potionEntry > 0)
    {
        if (!me->HasItemCount(potionEntry, 1))
        {
            me->StoreNewItemInBestSlots(potionEntry, 20);
        };
        if (Item* pPotion = GetItemInInventory(potionEntry))
        {
            if (UseItem(pPotion, me))
            {
                return true;
            }
        }
    }

    return false;
}

void Nier_Base::ChooseTarget(Unit* pTarget)
{
    if (pTarget)
    {
        if (me)
        {
            me->SetSelection(pTarget->GetGUID());
            me->SetTarget(pTarget->GetGUID());
        }
    }
}

void Nier_Base::ClearTarget()
{
    if (me)
    {
        me->SetSelection(ObjectGuid::Empty);
        me->SetTarget(ObjectGuid::Empty);
        me->AttackStop();
        me->InterruptNonMeleeSpells(true);
    }
}

bool Nier_Base::SpellValid(uint32 pSpellID)
{
    if (pSpellID == 0)
    {
        return false;
    }
    if (!me)
    {
        return false;
    }
    if (me->HasSpellCooldown(pSpellID))
    {
        return false;
    }

    return true;
}

Item* Nier_Base::GetItemInInventory(uint32 pmEntry)
{
    if (!me)
    {
        return NULL;
    }
    for (uint8 i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; i++)
    {
        Item* pItem = me->GetItemByPos(INVENTORY_SLOT_BAG_0, i);
        if (pItem)
        {
            if (pItem->GetEntry() == pmEntry)
            {
                return pItem;
            }
        }
    }

    for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++)
    {
        if (Bag* pBag = (Bag*)me->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
        {
            for (uint32 j = 0; j < pBag->GetBagSize(); j++)
            {
                Item* pItem = me->GetItemByPos(i, j);
                if (pItem)
                {
                    if (pItem->GetEntry() == pmEntry)
                    {
                        return pItem;
                    }
                }
            }
        }
    }

    return NULL;
}

Player* Nier_Base::GetNearbyHostilePlayer()
{
    std::list<Player*> players;
    Acore::AnyPlayerInObjectRangeCheck checker(me, VISIBILITY_DISTANCE_NORMAL);
    Acore::PlayerListSearcher<Acore::AnyPlayerInObjectRangeCheck> searcher(me, players, checker);
    Cell::VisitWorldObjects(me, searcher, VISIBILITY_DISTANCE_NORMAL);
    for (std::list<Player*>::iterator itr = players.begin(); itr != players.end(); ++itr)
    {
        if (Player* eachPlayer = *itr)
        {
            if (eachPlayer->IsAlive())
            {
                if (me->IsValidAttackTarget(eachPlayer))
                {
                    if (eachPlayer->CanSeeOrDetect(me))
                    {
                        return eachPlayer;
                    }
                }
            }
        }
    }

    return nullptr;
}

Unit* Nier_Base::GetNearbyHostileUnit()
{
    std::list<Creature*> creatureList;
    me->GetCreatureListWithEntryInGrid(creatureList, 0, VISIBILITY_DISTANCE_TINY);
    if (!creatureList.empty())
    {
        for (std::list<Creature*>::iterator itr = creatureList.begin(); itr != creatureList.end(); ++itr)
        {
            if (Creature* hostileCreature = *itr)
            {
                if (hostileCreature->IsAlive())
                {
                    if (!hostileCreature->IsCivilian())
                    {
                        if (me->IsValidAttackTarget(hostileCreature))
                        {
                            return hostileCreature;
                        }
                    }
                }
            }
        }
    }

    return nullptr;
}

void Nier_Base::MoveToPosition(Position pTargetpos, bool pRun)
{
    me->SetStandState(UnitStandStateType::UNIT_STAND_STATE_STAND);
    if (pRun)
    {
        me->SetWalk(false);
    }
    else
    {
        me->SetWalk(true);
    }
    me->GetMotionMaster()->MovePoint(0, pTargetpos);
}

bool Nier_Base::Chase(Unit* pTarget, float pDistance)
{
    bool inPosition = false;

    if (pDistance < ATTACK_DISTANCE)
    {
        if (me->IsWithinMeleeRange(pTarget))
        {
            inPosition = true;
        }
        else
        {
            float destPosTargetDist = pTarget->GetDistance(actionTargetPos);
            if (destPosTargetDist > DEFAULT_COMBAT_REACH)
            {
                pTarget->GetNearPoint(pTarget, actionTargetPos.m_positionX, actionTargetPos.m_positionY, actionTargetPos.m_positionZ, 0.0f, CONTACT_DISTANCE, pTarget->GetAbsoluteAngle(me));
                me->GetMotionMaster()->MovePoint(0, actionTargetPos);
            }
            else
            {
                float destPosMeDist = me->GetDistance(actionTargetPos);
                if (destPosMeDist > CONTACT_DISTANCE)
                {
                    if (!me->isMoving())
                    {
                        me->GetMotionMaster()->MovePoint(0, actionTargetPos);
                    }
                }
            }
        }
    }
    else
    {
        if (me->GetDistance(pTarget) < pDistance)
        {
            inPosition = true;
        }
        else
        {
            float destPosTargetDist = pTarget->GetDistance(actionTargetPos);
            if (destPosTargetDist > pDistance)
            {
                pTarget->GetNearPoint(pTarget, actionTargetPos.m_positionX, actionTargetPos.m_positionY, actionTargetPos.m_positionZ, 0.0f, pDistance - 1.0f, pTarget->GetAbsoluteAngle(me));
                me->GetMotionMaster()->MovePoint(0, actionTargetPos);
            }
            else
            {
                float destPosMeDist = me->GetDistance(actionTargetPos);
                if (destPosMeDist > CONTACT_DISTANCE)
                {
                    if (!me->isMoving())
                    {
                        me->GetMotionMaster()->MovePoint(0, actionTargetPos);
                    }
                }
            }
        }
    }

    if (inPosition)
    {
        if (me->isMoving())
        {
            me->StopMoving();
        }
        if (!me->isInFront(pTarget, M_PI_2))
        {
            me->SetFacingToObject(pTarget);
        }
    }

    return inPosition;
}
