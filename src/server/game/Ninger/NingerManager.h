#ifndef NINGER_MANAGER_H
#define NINGER_MANAGER_H

#define enum_to_string(x) #x

#ifndef NINGER_PASSWORD
# define NINGER_PASSWORD "ninger"
#endif

#include "NingerEntity.h"
#include "Player.h"

#include <string>
#include <iostream>
#include <sstream>

#include "NingerConfig.h"

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

enum BaseSpell :uint32
{
    SunderArmor = 7386,
    CatForm = 768,
    DireBearForm = 9634,
    BearForm = 5487,
    MoonkinForm = 24858
};

class NingerManager
{
    NingerManager();
    NingerManager(NingerManager const&) = delete;
    NingerManager& operator=(NingerManager const&) = delete;
    ~NingerManager() = default;

public:
    void InitializeManager();
    void UpdateNingerManager(uint32 pmDiff);
    bool Deleteningers();

    bool LoginNinger(uint32 pmLevel);
    void HandlePlayerSay(Player* pmPlayer, std::string pmContent);    
    bool StringEndWith(const std::string& str, const std::string& tail);
    bool StringStartWith(const std::string& str, const std::string& head);
    std::vector<std::string> SplitString(std::string srcStr, std::string delimStr, bool repeatedCharIgnored);
    std::string TrimString(std::string srcStr);
    static NingerManager* instance();    
    void WhisperTo(Player* pmTarget, std::string pmContent, Language pmLanguage, Player* pmSender);

    void HandleChatCommand(Player* pmSender, std::string pmCMD, Player* pmReceiver = NULL);
    void HandlePacket(WorldSession* pmSession, WorldPacket pmPacket);

public:
    std::unordered_map<uint32, std::unordered_map<uint32, uint32>> allianceRaces;
    std::unordered_map<uint32, std::unordered_map<uint32, uint32>> hordeRaces;    
    std::unordered_map<uint32, std::unordered_map<uint32, NingerEntity*>> ningerEntityMap;
    std::unordered_map<uint32, std::unordered_map<uint32, std::string>> characterTalentTabNameMap;   

private:
    int checkDelay;
};

#define sNingerManager NingerManager::instance()

#endif
