#ifndef MING_MANAGER_H
#define MING_MANAGER_H

#ifndef MING_MARK
# define MING_MARK "ming"
#endif

#define enum_to_string(x) #x

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <mutex>
#include <string>
#include <unordered_map>

#include "Common.h"

class SpawnedObject
{
public:
    SpawnedObject()
    {
        guid = 0;
        mapId = 0;
    }

public:
    uint32 guid;
    uint32 mapId;
    Position pos;
};

class MingManager
{
    MingManager();
    MingManager(MingManager const&) = delete;
    MingManager& operator=(MingManager const&) = delete;
    ~MingManager() = default;

public:
    void InitializeManager();
    bool StringEndWith(const std::string& str, const std::string& tail);
    bool StringStartWith(const std::string& str, const std::string& head);
    bool IsMingerExceptionEntry(uint32 pmEntry);
    bool IsHerb(uint32 pmEntry);
    bool NearHerb(uint32 pmMapId, Position pmPos, float pmDistance);
    bool AddHerb(uint32 pmGuid, uint32 pmMapId, Position pmPos, float pmDistance);
    bool IsVein(uint32 pmEntry);    
    std::vector<std::string> SplitString(std::string srcStr, std::string delimStr, bool repeatedCharIgnored);
    std::string TrimString(std::string srcStr);
    static MingManager* instance();    

public:
    std::unordered_set<uint32> herbGOEntrySet;
    std::unordered_set<uint32> veinGOEntrySet;
    std::unordered_set<uint32> instanceEncounterEntrySet;
    std::unordered_set<SpawnedObject*> spawnedHerbSet;
    std::unordered_set<SpawnedObject*> spawnedVeinSet;
    std::unordered_map<uint32, SpawnedObject*> veinsMap;
};

#define sMingManager MingManager::instance()

#endif
