/*
** Made by Rochet2(Eluna)
** Rewritten by Poszer & Talamortis https://github.com/poszer/ & https://github.com/talamortis/
** AzerothCore 2019 http://www.azerothcore.org/
** Cleaned and made into a module by Micrah https://github.com/milestorme/
*/

/*
    --Import this SQL first:
    CREATE TABLE `custom_item_enchant_visuals` (
        `iguid` INT(10) UNSIGNED NOT NULL COMMENT 'item DB guid',
        `display` INT(10) UNSIGNED NOT NULL COMMENT 'enchantID',
		`PlayerName` varchar(50) NOT NULL,
        PRIMARY KEY(`iguid`)
        )
        COMMENT = 'stores the enchant IDs for the visuals'
        COLLATE = 'latin1_swedish_ci'
        ENGINE = InnoDB;
*/
#include "ScriptMgr.h"
#include "Cell.h"
#include "CellImpl.h"
#include "GameEventMgr.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Unit.h"
#include "GameObject.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "InstanceScript.h"
#include "CombatAI.h"
#include "PassiveAI.h"
#include "Chat.h"
#include "DBCStructure.h"
#include "DBCStores.h"
#include "ObjectMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"

using namespace std;

#define DEFAULT_MESSAGE 907

struct VisualData
{
    uint32 Menu;
    uint32 Submenu;
    uint32 Icon;
    uint32 Id;
    string Name;
};

VisualData vData[] =
{
    { 1, 0, GOSSIP_ICON_BATTLE, 3789, "狂暴" },
    { 1, 0, GOSSIP_ICON_BATTLE, 3854, "法术强度" },
    { 1, 0, GOSSIP_ICON_BATTLE, 3273, "死亡霜冻" },
    { 1, 0, GOSSIP_ICON_BATTLE, 3225, "刽子手" },
    { 1, 0, GOSSIP_ICON_BATTLE, 3870, "吸血" },
    { 1, 0, GOSSIP_ICON_BATTLE, 1899, "邪恶武器" },
    { 1, 0, GOSSIP_ICON_BATTLE, 2674, "法术浪涌" },
    { 1, 0, GOSSIP_ICON_BATTLE, 2675, "战斗大师" },
    { 1, 0, GOSSIP_ICON_BATTLE, 2671, "奥术与火焰能量" },
    { 1, 0, GOSSIP_ICON_BATTLE, 2672, "暗影和冰霜能量" },
    { 1, 0, GOSSIP_ICON_BATTLE, 3365, "符文剑" },
    { 1, 0, GOSSIP_ICON_BATTLE, 2673, "猫鼬" },
    { 1, 0, GOSSIP_ICON_BATTLE, 2343, "法术强度" },
    { 1, 2, GOSSIP_ICON_TALK, 0, "下一页.." },

    { 2, 0, GOSSIP_ICON_BATTLE, 425, "黑暗神殿" },
    { 2, 0, GOSSIP_ICON_BATTLE, 3855, "法术力量 III" },
    { 2, 0, GOSSIP_ICON_BATTLE, 1894, "寒冰武器" },
    { 2, 0, GOSSIP_ICON_BATTLE, 1103, "敏捷性" },
    { 2, 0, GOSSIP_ICON_BATTLE, 1898, "吸取生命" },
    { 2, 0, GOSSIP_ICON_BATTLE, 3345, "大地生命 I" },
    { 2, 0, GOSSIP_ICON_BATTLE, 1743, "魔法属性" },
    { 2, 0, GOSSIP_ICON_BATTLE, 3093, "亡灵与恶魔" },
    { 2, 0, GOSSIP_ICON_BATTLE, 1900, "十字军战士" },
    { 2, 0, GOSSIP_ICON_BATTLE, 3846, "魔法力量 II" },
    { 2, 0, GOSSIP_ICON_BATTLE, 1606, "攻击力" },
    { 2, 0, GOSSIP_ICON_BATTLE, 283, "风怒 I" },
    { 2, 0, GOSSIP_ICON_BATTLE, 1, "石齿 III" },
    { 2, 3, GOSSIP_ICON_TALK, 0, "下一页.." },
    { 2, 1, GOSSIP_ICON_TALK, 0, "..返回" },

    { 3, 0, GOSSIP_ICON_BATTLE, 3265, "祝福" },
    { 3, 0, GOSSIP_ICON_BATTLE, 2, "冰封 I" },
    { 3, 0, GOSSIP_ICON_BATTLE, 3, "焰舌 III" },
    { 3, 0, GOSSIP_ICON_BATTLE, 3266, "正义" },
    { 3, 0, GOSSIP_ICON_BATTLE, 1903, "精神" },
    { 3, 0, GOSSIP_ICON_BATTLE, 13, "锐化" },
    { 3, 0, GOSSIP_ICON_BATTLE, 26, "霜油" },
    { 3, 0, GOSSIP_ICON_BATTLE, 7, "致命毒药" },
    { 3, 0, GOSSIP_ICON_BATTLE, 803, "火焰" },
    { 3, 0, GOSSIP_ICON_BATTLE, 1896, "武器伤害" },
    { 3, 0, GOSSIP_ICON_BATTLE, 2666, "智力" },
    { 3, 0, GOSSIP_ICON_BATTLE, 25, "暗影" },
    { 3, 2, GOSSIP_ICON_TALK, 0, "..返回" },
};



class npc_visualweapon : public CreatureScript
{
public:
    npc_visualweapon() : CreatureScript("npc_visualweapon") { }

    bool MainHand;

    void SetVisual(Player* player, uint32 visual)
    {
        uint8 slot = MainHand ? EQUIPMENT_SLOT_MAINHAND : EQUIPMENT_SLOT_OFFHAND;

        Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);

        if (!item)
        {
            ChatHandler(player->GetSession()).PSendSysMessage("没有装备可以更改显示特效。");
            return;
        }

        const ItemTemplate* itemTemplate = item->GetTemplate();

        if (itemTemplate->SubClass == ITEM_SUBCLASS_ARMOR_SHIELD ||
            itemTemplate->SubClass == ITEM_SUBCLASS_ARMOR_BUCKLER ||
            itemTemplate->SubClass == ITEM_SUBCLASS_WEAPON_SPEAR ||
            itemTemplate->SubClass == ITEM_SUBCLASS_WEAPON_BOW ||
            itemTemplate->SubClass == ITEM_SUBCLASS_WEAPON_GUN ||
            itemTemplate->SubClass == ITEM_SUBCLASS_WEAPON_obsolete ||
            itemTemplate->SubClass == ITEM_SUBCLASS_WEAPON_EXOTIC ||
            itemTemplate->SubClass == ITEM_SUBCLASS_WEAPON_EXOTIC2 ||
            itemTemplate->SubClass == ITEM_SUBCLASS_WEAPON_MISC ||
            itemTemplate->SubClass == ITEM_SUBCLASS_WEAPON_THROWN ||
            itemTemplate->SubClass == ITEM_SUBCLASS_WEAPON_CROSSBOW ||
            itemTemplate->SubClass == ITEM_SUBCLASS_WEAPON_WAND ||
            itemTemplate->SubClass == ITEM_SUBCLASS_WEAPON_FISHING_POLE ||
            itemTemplate->SubClass == ITEM_SUBCLASS_WEAPON_obsolete)
            return;

        player->SetUInt16Value(PLAYER_VISIBLE_ITEM_1_ENCHANTMENT + (item->GetSlot() * 2), 0, visual);
        CharacterDatabase.PExecute("REPLACE into custom_item_enchant_visuals VALUES ('%u', '%u', '%s')", item->GetGUIDLow(), visual, player->GetName().c_str());
    }

    void GetMenu(Player* player, Creature* creature, uint32 menuId)
    {
        for (uint8 i = 0; i < (sizeof(vData) / sizeof(*vData)); i++)
        {
            if (vData[i].Menu == menuId)
                player->ADD_GOSSIP_ITEM(vData[i].Icon, vData[i].Name, GOSSIP_SENDER_MAIN, i);
        }

        player->SEND_GOSSIP_MENU(DEFAULT_MESSAGE, creature->GetGUID());
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "|TInterface/PaperDoll/UI-PaperDoll-Slot-MainHand:40:40:-18|t主手", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "|TInterface/PaperDoll/UI-PaperDoll-Slot-SecondaryHand:40:40:-18|t副手", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "|TInterface/PaperDollInfoFrame/UI-GearManager-Undo:40:40:-18|t离开", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);

        player->SEND_GOSSIP_MENU(DEFAULT_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        switch (action)
        {
        case GOSSIP_ACTION_INFO_DEF + 1:
            MainHand = true;
            GetMenu(player, creature, 1);
            return false;

        case GOSSIP_ACTION_INFO_DEF + 2:
            MainHand = false;
            GetMenu(player, creature, 1);
            return false;

        case GOSSIP_ACTION_INFO_DEF + 3:
            player->CLOSE_GOSSIP_MENU();
            return false;
        }

        uint32 menuData = vData[action].Submenu;

        if (menuData == 0)
        {
            SetVisual(player, vData[action].Id);
            menuData = vData[action].Menu;
        }

        GetMenu(player, creature, menuData);
        return true;
    }
};

class player_visualweapon : public PlayerScript
{
public:
    player_visualweapon() : PlayerScript("player_visualweapons") {}

    void GetVisual(Player* player)
    {
        if (!player)
            return;

        Item* pItem;

        // We need to query the DB to get item
        QueryResult result = CharacterDatabase.PQuery("SELECT iguid, display FROM custom_item_enchant_visuals WHERE iguid IN(SELECT guid FROM item_instance WHERE owner_guid = %u)", player->GetGUIDLow());

        if (!result)
            return;

        // Lets loop to check item by pos
        for (int i = EQUIPMENT_SLOT_MAINHAND; i < EQUIPMENT_SLOT_OFFHAND; ++i)
        {
            pItem = player->GetItemByPos(255, i);
        }

        // Now we have query the DB we need to get the fields.
        do
        {
            Field* fields = result->Fetch();
            uint32 iguid = fields[0].GetUInt32();
            uint32 visual = fields[1].GetUInt32();
        if (!pItem)
        return;
            // Lets make sure our item guid matches the guid in the DB
            if (pItem->GetGUIDLow() == iguid)
            {
                player->SetUInt16Value(PLAYER_VISIBLE_ITEM_1_ENCHANTMENT + (pItem->GetSlot() * 2), 0, visual);
            }

        } while (result->NextRow());
    }

    // if Player has item in bag and re-equip it lets check for enchant
    void OnEquip(Player* player, Item* /*item*/, uint8 /*bag*/, uint8 /*slot*/, bool /*update*/) override
    {
        GetVisual(player);
    }

    void OnLogin(Player* p) override
    {
        GetVisual(p);
    }
};

void AddSC_Npc_VisualWeaponScripts()
{
    new npc_visualweapon;
    new player_visualweapon;
}
