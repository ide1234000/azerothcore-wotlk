#include "ScriptMgr.h"
#include "Configuration/Config.h"
#include "ObjectMgr.h"
#include "DatabaseEnv.h"
#include "Chat.h"
#include "Common.h"

class CS_Migrate : public CommandScript
{
public:
    CS_Migrate() : CommandScript("CS_Migrate") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> TableCommandMigrate = // .migrate
        {
            { "items",				SEC_ADMINISTRATOR, 		true, &HandleMigrateItems,             	   			"" }
        };

        static std::vector<ChatCommand> commandTable =
        {  
            { "migrate",			SEC_ADMINISTRATOR, 		true, nullptr,             	   				        "", TableCommandMigrate }
        };

        return commandTable;
    }

	static bool HandleMigrateItems(ChatHandler *handler, const char *args)
	{		
        if (sConfigMgr->GetBoolDefault("Migrate.Enable", true))
		{
			handler->SendSysMessage("System disable");
            handler->SetSentErrorMessage(true);
            return false;			
		}
		
		// .migrate items characterGuid itemcount

        char* _PlayerGuid = strtok((char*)args, " ");
        if (!_PlayerGuid)
            return false;

        uint32 PlayerGuid = (uint32)atoi(_PlayerGuid);

        char* _ItemCount = strtok(NULL, " ");
        if (!_ItemCount)
            return false;

        uint32 ItemCount = (uint32)atoi(_ItemCount);

        std::string ExecuteStr = "INSERT INTO `item_instance`(`guid`, `owner_guid`) VALUES\n";

        for (uint32 i = 1; i < ItemCount + 1; i++)
        {
            uint32 HighGUIDItem = sObjectMgr->GenerateLowGuid(HIGHGUID_ITEM);
            ExecuteStr += GetFormatString("(%u, %u)", HighGUIDItem, PlayerGuid);
            if (i < ItemCount)
                ExecuteStr += ",\n";
        }

        ExecuteStr += ";";
        CharacterDatabase.PExecute("%s", ExecuteStr.c_str());

        return true;
	}
	
	static std::string GetFormatString(const char* format, ...)
    {
        va_list ap;
        char str[2048];
        va_start(ap, format);
        vsnprintf(str, 2048, format, ap);
        va_end(ap);
        return std::string(str);
    }
};

void AddMigrateScripts() 
{
    new CS_Migrate();
}
