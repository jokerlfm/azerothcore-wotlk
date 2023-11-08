#include "LingerManager.h"
#include "LingerConfig.h"
#include "AuctionHouseMgr.h"
#include "GameTime.h"

LingerManager::LingerManager()
{
    materialEntrySet.clear();
    updateDelay = 0;
}

void LingerManager::InitializeManager()
{
    if (sLingerConfig->Enable == 0)
    {
        return;
    }

    sLog->outMessage(LINGER_MARK, LogLevel::LOG_LEVEL_INFO, "Initialize linger");

    materialEntrySet.clear();
    materialEntrySet.insert(39682);

    sLog->outMessage(LINGER_MARK, LogLevel::LOG_LEVEL_INFO, "linger initialized");
}

LingerManager* LingerManager::instance()
{
    static LingerManager instance;
    return &instance;
}

void LingerManager::UpdateManager(uint32 pmDiff)
{
    if (sLingerConfig->Enable == 0)
    {
        return;
    }

    if (updateDelay > 0)
    {
        updateDelay -= pmDiff;
        return;
    }
    else
    {
        updateDelay = LINGER_UPDATE_DELAY;
    }
    for (std::unordered_set<uint32>::iterator entryIt = materialEntrySet.begin(); entryIt != materialEntrySet.end(); entryIt++)
    {
        AddAuction(AuctionHouses::AUCTIONHOUSE_ALLIANCE, *entryIt);
        AddAuction(AuctionHouses::AUCTIONHOUSE_HORDE, *entryIt);
        AddAuction(AuctionHouses::AUCTIONHOUSE_NEUTRAL, *entryIt);
    }
}

void LingerManager::AddAuction(uint32 pmHouse, uint32 pmItemEntry)
{
    if (AuctionHouseObject* aho = sAuctionMgr->GetAuctionsMapByHouseId(pmHouse))
    {
        if (const AuctionHouseEntry* ahe = sAuctionMgr->GetAuctionHouseEntryFromHouse(pmHouse))
        {
            if (Item* item = Item::CreateItem(pmItemEntry, 1))
            {
                if (const ItemTemplate* proto = item->GetTemplate())
                {
                    AuctionEntry* ae = new AuctionEntry;
                    ae->Id = sObjectMgr->GenerateAuctionID();
                    ae->houseId = ahe->houseId;
                    uint32 basePrice = proto->BuyPrice;
                    if (basePrice == 0)
                    {
                        basePrice = proto->SellPrice * 5;
                    }
                    if (basePrice == 0)
                    {
                        basePrice = proto->ItemLevel * proto->Quality * 500;
                    }
                    ae->item_guid = item->GetGUID();
                    ae->item_template = item->GetEntry();
                    ae->itemCount = item->GetCount();
                    ae->owner = ObjectGuid::Empty;
                    ae->startbid = urand(basePrice * 4, basePrice * 8);
                    ae->bidder = ObjectGuid::Empty;
                    ae->bid = 0;
                    ae->buyout = urand(basePrice * 10, basePrice * 20);
                    uint32 etime = GameTime::GetGameTime().count() + urand(1, 2) * HOUR;
                    ae->expire_time = etime;
                    ae->deposit = sAuctionMgr->GetAuctionDeposit(ahe, etime, item, 1);;
                    ae->auctionHouseEntry = ahe;
                    sAuctionMgr->AddAItem(item);
                    aho->AddAuction(ae);
                    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
                    item->SaveToDB(trans);
                    ae->SaveToDB(trans);
                    CharacterDatabase.CommitTransaction(trans);
                }
            }
        }
    }
}
