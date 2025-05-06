#ifndef NIER_MANAGER_H
#define NIER_MANAGER_H

#include"Nier_Base.h"

#include <string>
#include <iostream>
#include <sstream>

#include "NierConfig.h"

class Nier_Base;

class NierManager
{
    NierManager();
    NierManager(NierManager const&) = delete;
    NierManager& operator=(NierManager const&) = delete;
    ~NierManager() = default;

public:
    void InitializeManager();
    void Update(uint32 pDiff);
    void LogoutNiers(bool pmInstant = false);
    void DeleteNiers();
    bool CreateNier(uint32 pMasterId, uint32 pClass, uint32 pRace);
    bool LoginNiers(uint32 pMasterId);

    Nier_Base* GetNier(uint32 pNierId);

    bool IsPolymorphed(Unit* pmTarget);

    bool HasAura(Unit* pTarget, uint32 pSpellId, Unit* pCaster = NULL);
    uint32 GetAuraDuration(Unit* pTarget, uint32 pSpellId, Unit* pCaster = NULL);
    uint32 GetAuraStack(Unit* pTarget, uint32 pSpellId, Unit* pCaster = NULL);

    void HandleChatCommand(Player* pCommander, std::string pCommand, Player* pTarget);
    void HandlePacket(const WorldSession* pSession, WorldPacket pPacket);

    void WhisperTo(Player* pTarget, std::string pContent, Player* pSender);

    std::vector<std::string> SplitString(std::string srcStr, std::string delimStr, bool repeatedCharIgnored);
    std::string TrimString(std::string srcStr);

    static NierManager* instance();

public:
    std::unordered_map<uint32, Nier_Base*> nierMap;
    std::unordered_map<uint32, std::unordered_map<uint32, uint32>> allianceRaces;
    std::unordered_map<uint32, std::unordered_map<uint32, uint32>> hordeRaces;
    uint32 nameIndex;
    std::unordered_map<uint32, std::string> nierNameMap;
    std::unordered_map<uint32, std::unordered_map<uint32, std::string>> characterTalentTabNameMap;
    std::unordered_set<uint32> instanceEncounterEntrySet;

private:
    uint64 timeValue;
};

class AllCreaturesOfEntryInRange_Nier
{
public:
    AllCreaturesOfEntryInRange_Nier(WorldObject const* pObject, uint32 uiEntry, float fMaxRange) : m_pObject(pObject), m_uiEntry(uiEntry), m_fRange(fMaxRange) {}
    bool operator() (Unit* pUnit)
    {
        if (m_uiEntry > 0)
        {
            if (pUnit->GetEntry() != m_uiEntry)
            {
                return false;
            }
        }
        if (!m_pObject->IsWithinDist(pUnit, m_fRange, false))
        {
            return false;
        }

        return true;
    }

private:
    WorldObject const* m_pObject;
    uint32 m_uiEntry;
    float m_fRange;
};

#define sNierManager NierManager::instance()

#endif
