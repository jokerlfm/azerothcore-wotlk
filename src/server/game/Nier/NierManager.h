#ifndef NIER_MANAGER_H
#define NIER_MANAGER_H

#include "NierEntity.h"

#include <string>
#include <iostream>
#include <sstream>

#include "NierConfig.h"

class pvpZonePosition
{
public:
    pvpZonePosition()
    {
        mapID = 0;
        minLevel = 0;
        maxLevel = 0;
        spawnDistance = 5.0f;
        flagPointMap.clear();
    }

    uint32 mapID;
    uint32 minLevel;
    uint32 maxLevel;
    float spawnDistance;
    std::unordered_map<uint32, Position> flagPointMap;
};

enum ShapeshiftSpell :uint32
{
    CatForm = 768,
    DireBearForm = 9634,
    BearForm = 5487,
    MoonkinForm = 24858
};

class NierManager
{
    NierManager();
    NierManager(NierManager const&) = delete;
    NierManager& operator=(NierManager const&) = delete;
    ~NierManager() = default;

public:
    void InitializeManager();
    void UpdateNierManager(uint32 pmDiff);
    void UpdateNierEntities(uint32 pmDiff);
    void LogoutNiers(bool pmInstant = false);
    void DeleteNiers();
    bool LoginNier(uint32 pmLevel, uint32 pmCount);
    bool LoginOneNier(uint32 pmLevel);

    bool IsPolymorphed(Unit* pmTarget);

    Position PredictPosition(Unit* target);

    void HandleChatCommand(Player* pmCommander, std::string pmContent, Player* pmTargetPlayer = nullptr, Group* pmTargetGroup = nullptr);
    void HandlePacket(const WorldSession* pmSession, WorldPacket pmPacket);

    static NierManager* instance();

public:
    std::unordered_map<uint32, std::unordered_map<uint32, uint32>> allianceRaces;
    std::unordered_map<uint32, std::unordered_map<uint32, uint32>> hordeRaces;
    uint32 nameIndex;
    std::unordered_map<uint32, std::string> nierNameMap;
    std::unordered_set<NierEntity*> nierEntitySet;
    std::unordered_map<uint32, std::unordered_map<uint32, std::string>> characterTalentTabNameMap;
    std::unordered_set<uint32> instanceEncounterEntrySet;

private:
    void CreateNier(uint32 pmLevel, bool pmAlliance, uint32 pmGroupRole);
    int checkDelay;
};

#define sNierManager NierManager::instance()

#endif
