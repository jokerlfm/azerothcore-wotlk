#ifndef NIER_ENTITY_H
#define NIER_ENTITY_H

enum NierEntityState :uint32
{
    NierEntityState_None = 0,
    NierEntityState_OffLine,
    NierEntityState_Enter,
    NierEntityState_CheckAccount,
    NierEntityState_CreateAccount,
    NierEntityState_CheckCharacter,
    NierEntityState_CreateCharacter,
    NierEntityState_CheckLogin,
    NierEntityState_DoLogin,
    NierEntityState_DoEnum,
    NierEntityState_CheckEnum,
    NierEntityState_Initialize,
    NierEntityState_Equip,
    NierEntityState_Online,
    NierEntityState_Exit,
    NierEntityState_CheckLogoff,
    NierEntityState_DoLogoff,
    NierEntityState_RedoLogin,
    NierEntityState_CheckRedoLogin,
};

class NierEntity
{
public:
    NierEntity();
    void Update(uint32 pmDiff);

public:
    uint32 master_id;
    std::string account_name;
    uint32 account_id;
    std::string character_name;
    uint32 character_id;
    uint32 target_team;
    uint32 target_level;
    uint32 target_race;
    uint32 target_class;
    uint32 target_specialty;
    uint32 nier_type;
    int checkDelay;
    uint32 entityState;
};
#endif
