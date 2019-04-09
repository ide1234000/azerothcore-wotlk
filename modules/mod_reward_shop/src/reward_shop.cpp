/*

Database Actions:

1 = item
2 = gold
3 = name change
4 = faction change
5 = race change

script made by talamortis

*/

#include "Configuration/Config.h"
#include "Player.h"
#include "Creature.h"
#include "AccountMgr.h"
#include "ScriptMgr.h"
#include "Define.h"
#include "GossipDef.h"

class reward_shop : public CreatureScript
{
public:
    reward_shop()   :   CreatureScript("reward_shop"){  }

    bool failedcode;

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->IsInCombat())
            return false;

        if (!sConfigMgr->GetBoolDefault("RewardShopEnable", 0))
            return false;
        

        std::string text = "输入代码并按下【接受】";
        player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, "我想兑换我的代码。", GOSSIP_SENDER_MAIN, 1, text, 0, true);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "我该如何获得代码？", GOSSIP_SENDER_MAIN, 2);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "我没有代码，打扰了！", GOSSIP_SENDER_MAIN, 3);
        if (sConfigMgr->GetBoolDefault("AllowGM", 1))
            if (player->IsGameMaster())
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "[GM] 我要生成一个代码。", GOSSIP_SENDER_MAIN, 4);
        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }


    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        std::string info = sConfigMgr->GetStringDefault("WebsiteAddress", "目前你只能这样获得代码 (website address)");
        uint32 rnd1 = urand(10000, 90000);
        uint32 rnd2 = urand(10000, 90000);
        uint32 rnd3 = urand(10000, 90000);
        uint32 rnd4 = urand(10000, 90000);
        uint32 rnd5 = urand(10000, 90000);

        std::string CreatedBy = player->GetName();
        std::ostringstream randomcode;
        randomcode << "AZ-" << rnd1 << "-" << rnd2 << "-" << rnd3 << "-" << rnd4 << "-" << rnd5;

        switch (action)
        {
        case 2:
            creature->MonsterWhisper(info.c_str(), player, false);
            player->CLOSE_GOSSIP_MENU();
            break;
        case 3:
            player->CLOSE_GOSSIP_MENU();
            break;
        case 4:
            player->PlayerTalkClass->ClearMenus();
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "我想生成一个名称更改代码。", GOSSIP_SENDER_MAIN, 6);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "我想要生成一个阵营更改代码。", GOSSIP_SENDER_MAIN, 7);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "我想生成一个外观更改代码。", GOSSIP_SENDER_MAIN, 8);
            player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
            break;
        case 6:
            CharacterDatabase.PQuery("INSERT INTO `reward_shop` (`action`, `action_data`, `quantity`, `code`, `status`, `PlayerGUID`, `PlayerIP`, `CreatedBy`) VALUES(3, 0, 0, '%s', 0, 0, '0', '%s')", randomcode.str().c_str(), CreatedBy.c_str());
            ChatHandler(player->GetSession()).PSendSysMessage("Code was successfully created your code is %s", randomcode.str().c_str());
            break;
        case 7:
            CharacterDatabase.PQuery("INSERT INTO `reward_shop` (`action`, `action_data`, `quantity`, `code`, `status`, `PlayerGUID`, `PlayerIP`,`CreatedBy`) VALUES(4, 0, 0, '%s', 0, 0, '0', '%s')", randomcode.str().c_str(), CreatedBy.c_str());
            ChatHandler(player->GetSession()).PSendSysMessage("Code was successfully created your code is %s", randomcode.str().c_str());
            break;
        case 8:
            CharacterDatabase.PQuery("INSERT INTO `reward_shop` (`action`, `action_data`, `quantity`, `code`, `status`, `PlayerGUID`, `PlayerIP`, `CreatedBy`) VALUES(5, 0, 0, '%s', 0, 0, '0', '%s')", randomcode.str().c_str(), CreatedBy.c_str());
            ChatHandler(player->GetSession()).PSendSysMessage("Code was successfully created your code is %s", randomcode.str().c_str());
            break;
        }
        return true;
    }

    bool OnGossipSelectCode(Player* player, Creature* creature, uint32 sender, uint32, const char* code)
    {
        uint32 playerguid = player->GetGUID();
        std::string playerIP = player->GetSession()->GetRemoteAddress();
        std::string rewardcode = code;
        std::ostringstream messageCode;
        messageCode << "对不起 " << player->GetName() << "，这不是一个有效的代码或已经被兑换！";

        //check for code
        QueryResult result = CharacterDatabase.PQuery("SELECT id, action, action_data, quantity, status FROM reward_shop WHERE code = '%s'", rewardcode.c_str());

        std::size_t found = rewardcode.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890-");

        if (found != std::string::npos)
            return false;

        if (!result)
        {
            player->PlayDirectSound(9638); // No
            creature->MonsterWhisper(messageCode.str().c_str(), player);
            creature->HandleEmoteCommand(EMOTE_ONESHOT_QUESTION);
            player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
            return false;
        }

        std::transform(rewardcode.begin(), rewardcode.end(), rewardcode.begin(), ::toupper);

        do
        {
                Field* fields = result->Fetch();
                uint32 id = fields[0].GetUInt32();
                uint32 action = fields[1].GetUInt32();
                uint32 action_data = fields[2].GetUInt32();
                uint32 quantity = fields[3].GetUInt32();
                uint32 status = fields[4].GetInt32();
                uint32 nospace = 0;
                int count = 1;
                uint32 noSpaceForCount = 0;
                ItemPosCountVec dest;
                InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, action_data, quantity, &noSpaceForCount);


                if (status == 1)
                {
                    player->PlayDirectSound(9638); // No
                    creature->MonsterWhisper(messageCode.str().c_str(), player);
                    creature->HandleEmoteCommand(EMOTE_ONESHOT_QUESTION);
                    player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
                    return false;
                }
                switch (action)
                {

                case 1: /* Item */
                    if (msg != EQUIP_ERR_OK)
                        count -= noSpaceForCount;

                    if (count == 0 || dest.empty())
                    {
                        ChatHandler(player->GetSession()).PSendSysMessage("无法创建项目或者您没有足够的空间");
                        ChatHandler(player->GetSession()).SetSentErrorMessage(true);
                        return false;
                    }

                    if (count > 0 && action_data)
                    {
                        player->AddItem(action_data, quantity);
                    }
                    break;
                case 2: /* Gold */
                    player->ModifyMoney(action_data * 10000);
                    ChatHandler(player->GetSession()).PSendSysMessage("已成功兑换 [%u 金]", action_data);
                    break;
                case 3: /* Name Change */
                    player->SetAtLoginFlag(AT_LOGIN_RENAME);
                    ChatHandler(player->GetSession()).PSendSysMessage("兑换成功！请登出到角色选择页面进行名称变更！");
                    break;
                case 4: /* Faction Change */
                    player->SetAtLoginFlag(AT_LOGIN_CHANGE_FACTION);
                    ChatHandler(player->GetSession()).PSendSysMessage("兑换成功！请登出到角色选择页面进行阵营变更！");
                    break;
                case 5: /* Race Change */
                    player->SetAtLoginFlag(AT_LOGIN_CHANGE_RACE);
                    ChatHandler(player->GetSession()).PSendSysMessage("兑换成功！请登出到角色选择页面进行外观变更！");
                    break;
                }
            
        } while (result->NextRow());

        CharacterDatabase.PQuery("UPDATE reward_shop SET status = 1, PlayerGUID = '%u', PlayerIP = '%s' WHERE code = '%s'", playerguid, playerIP.c_str(), rewardcode.c_str());
        return true;
    }


    struct npc_reward_shopAI: public ScriptedAI
    {
        npc_reward_shopAI(Creature* creature)   :   ScriptedAI(creature) { }
        uint32 say_timer;
        bool canSay;

        void Reset()
        {
            say_timer = 1000;
            canSay = false;
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (me->IsWithinDist(who, 5.0f) && who->GetTypeId() == TYPEID_PLAYER)
            {
                canSay = true;
            }
            else
                canSay = false;
        }

        void UpdateAI(uint32 diff)
        {
            if (say_timer <= diff)
            {
                if (canSay)
                {
                    me->MonsterSay("嗨~ 你有获得过一段神秘代码吗？快来这里偷偷告诉我！", LANG_UNIVERSAL, NULL);
                    me->HandleEmoteCommand(EMOTE_ONESHOT_EXCLAMATION);
                    say_timer = 61000;
                }
            }
            else
                say_timer -= diff;
        }

    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_reward_shopAI(creature);
    }
};

class reward_shop_conf : public WorldScript
{
public:
    reward_shop_conf() : WorldScript("reward_shop_conf") { }

    void OnBeforeConfigLoad(bool reload) override
    {
        if (!reload) {
            std::string conf_path = _CONF_DIR;
            std::string cfg_file = conf_path + "/reward_shop.conf";
#ifdef WIN32
            cfg_file = "reward_shop.conf";
#endif
            std::string cfg_def_file = cfg_file + ".dist";

            sConfigMgr->LoadMore(cfg_def_file.c_str());

            sConfigMgr->LoadMore(cfg_file.c_str());
        }
    }
};

void AddRewardShopScripts()
{
    new reward_shop();
    new reward_shop_conf();
}
