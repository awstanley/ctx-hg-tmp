/******************************************************************************
 * vim: set ts=4 :
 ******************************************************************************
 * CTX Extension
 * Copyright (C) 2011 A.W. 'Swixel' Stanley.
 ******************************************************************************
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 *****************************************************************************/


#include "extension.h"
#include <convar.h>
#define GAME_DLL
#include <eiface.h>

CTXPatcher g_CTXPatcher;
SMEXT_LINK(&g_CTXPatcher);

ICvar *icvar = NULL;
IServerGameDLL *server = NULL;
IServerGameEnts *gameents = NULL;
IGameConfig *g_pGameConf = NULL;
IGameConfig *g_pGameMod = NULL;
CGlobalVars *gpGlobals;
IServerGameClients *gameclients = NULL;
int GameID = 0;


// Multimod ... YAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAY
bool PlayerData = false;
bool WeaponData = false;
bool ObjectData = false;
bool TwoTeams = false;
int CLASS_COUNT = 0;
int WEAPON_CNAME_OFFSET = 0;
int WEAPON_CNAME_LENGTH = 0;
int PLAYER_CNAME_OFFSET = 0;
int PLAYER_CNAME_LENGTH = 0;
int OBJECT_CNAME_OFFSET = 0;
int OBJECT_CNAME_LENGTH = 0;
bool MULTI_TEAM = false;


/* Global ConVars (enable, version) */
ConVar g_Version("sm_ctxpatcher_version", SMEXT_CONF_VERSION, FCVAR_NOTIFY|FCVAR_PRINTABLEONLY, "CTX Patcher Version");

// SourceHook to load the databases
SH_DECL_HOOK3_void(IServerGameDLL, ServerActivate, SH_NOATTRIB, 0, edict_t *, int, int);

// Databases
ValveDB *g_pPlayerClassInfoData = NULL;
ValveDB *g_pPlayerClassInfoOriginalData = NULL;
ValveDB *g_pPlayerClassInfoData2 = NULL;
ValveDB *g_pPlayerClassInfoOriginalData2 = NULL;
ValveDB *g_pWeaponInfoDatabase = NULL;
ValveDB *g_pWeaponInfoOriginalDatabase = NULL;
ValveDB *g_pObjectDatabase = NULL; 
ValveDB *g_pObjectOriginalDatabase = NULL;
size_t WEAPON_SIZE = 0;
size_t PLAYER_SIZE = 0;
size_t OBJECT_SIZE = 0;


sp_nativeinfo_t g_ExtensionNatives[] =
{
	{ "CTX_Reset",		CTX_ResetOne },
	{ "CTX_ResetAll",	CTX_ResetAll },
	{ "CTX_Get",		CTX_Get },
	{ "CTX_Set",		CTX_Set },
	{ NULL,				NULL }
};

int GetKeyValueAsInt(IGameConfig *p, char *Key)
{
	IGameConfig *GC = p;
	const char *Value = GC->GetKeyValue(Key);
	if(strlen(Value) == 0)
	{
		return 0;
	}
	else
	{
		return atoi(Value);
	}
}

bool GetKeyValueAsBool(IGameConfig *p, char *Key)
{
	IGameConfig *GC = p;
	const char *Value = GC->GetKeyValue(Key);
	if(strlen(Value) == 0)
	{
		return false;
	}
	else
	{
		return (bool)atoi(Value);
	}
}

bool GetInfo(const char *KeyBase, int &Length, int &Offset)
{
	char CLength[128];
	char COffset[128];

	sprintf(CLength, "%s_LENGTH", KeyBase); sprintf(COffset, "%s_OFFSET", KeyBase);

	if(strlen(COffset) < 8)
	{
		g_pSM->LogMessage(myself, "Offset Failed");
		return false;
	}
	else
	{
		Offset = GetKeyValueAsInt(g_pGameMod,COffset);
	}

	if(strlen(CLength) < 8)
	{
		g_pSM->LogMessage(myself, "Length Failed");
		return false;
	}
	else
	{
		Length = GetKeyValueAsInt(g_pGameMod,CLength);
	}
	return true;
}

bool DoInitialisation()
{
	if(WeaponData)
	{	
		if(!SetupWeaponDatabasePointer())
		{
			g_pSM->LogMessage(myself, "Disabled: Could not get the weapon database.");
			WeaponData = false;
			ValveDB *g_pWeaponInfoDatabase = new ValveDB();
			ValveDB *g_pWeaponInfoOriginalDatabase = new ValveDB();
		}
	}

	if(PlayerData)
	{
		if(!SetupPlayerDatabasePointer())
		{
			g_pSM->LogMessage(myself, "Disabled: Could not get the player database(s).");
			PlayerData = false;
			ValveDB *g_pPlayerClassInfoData = new ValveDB();
			ValveDB *g_pPlayerClassInfoOriginalData = new ValveDB();
			ValveDB *g_pPlayerClassInfoData2 = new ValveDB();
			ValveDB *g_pPlayerClassInfoOriginalData2 = new ValveDB();
		}
	}

	if(ObjectData)
	{
		if(!SetupObjectDatabase())
		{
			g_pSM->LogMessage(myself, "Disabled: Could not get the player database(s).");
			ObjectData = false;
			ValveDB *g_pObjectDatabase = new ValveDB(); 
			ValveDB *g_pObjectOriginalDatabase = new ValveDB();
		}
	}

	return true;
}

void Hook_ServerActivate(edict_t *pEdictList, int edictCount, int clientMax)
{
	DoInitialisation();
	SH_REMOVE_HOOK_STATICFUNC(IServerGameDLL, ServerActivate, server, Hook_ServerActivate, true);
}

bool CTXPatcher::SDK_OnLoad(char *error, size_t maxlength, bool late)
{
	char conf_error[255] = "";

	/* This mod's conf */
	if (!gameconfs->LoadGameConfigFile("ctx.all", &g_pGameConf, conf_error, sizeof(conf_error)))
	{
		if (conf_error[0])
		{
			snprintf(error, maxlength, "Could not read ctx.all.txt: %s", conf_error);
			g_pSM->LogMessage(myself, error);											  
		}
		return false;
	}

	// MULTIMOD!
	int GameID = engine->GetAppID();

	char cfgName[32];
	sprintf(cfgName, "ctx/ctx.%s", g_pSM->GetGameFolderName());

	if (!gameconfs->LoadGameConfigFile(cfgName, &g_pGameMod, conf_error, sizeof(conf_error)))
	{
		if (conf_error[0])
		{
			snprintf(error, maxlength, "Could not read %s.txt: %s", cfgName, conf_error);
			g_pSM->LogMessage(myself, error);											  
		}
		return false;
	}

	char *addr;
	int iOffset;

	WeaponData = GetKeyValueAsBool(g_pGameMod, "weapon_enabled");
	PlayerData = GetKeyValueAsBool(g_pGameMod, "player_enabled");
	ObjectData = GetKeyValueAsBool(g_pGameMod, "object_enabled");

	if(WeaponData)
	{
#ifdef _WIN32
		if(!g_pGameConf->GetMemSig("ReadWeaponDataFromFileForSlot_SubCall_01", (void **)&addr) || !addr || !g_pGameConf->GetOffset("WeaponDatabase_Offset", &iOffset))
		{
			snprintf(error, maxlength, "Game %i does not support PlayerData  modification -- Either no signature for 'ReadWeaponDataFromFileForSlot_SubCall_01' was found, or no offset for WeaponDatabase_Offset", GameID);
			g_pSM->LogMessage(myself, error);
			WeaponData = false;
		}
#else	// _APPLE and POSIX
		if(!g_pGameConf->GetMemSig("m_WeaponInfoDatabase", (void **)&addr) || !addr)
		{
			snprintf(error, maxlength, "Game %i does not support PlayerData  modification -- Either no signature for 'm_WeaponInfoDatabase', or otherwise unsupported.", GameID);
			g_pSM->LogMessage(myself, error);
			WeaponData = false;
		}
#endif
	}

	if(PlayerData)
	{
		if(!g_pGameConf->GetMemSig("GetPlayerClassData", (void **)&addr) || !addr)
		{
			snprintf(error, maxlength, "Game %i does not support PlayerData  modification -- signature missing or otherwise unsupported.", GameID);
			g_pSM->LogMessage(myself, error);
			PlayerData = false;
		}
	}

	if(ObjectData)
	{
		if(!g_pGameConf->GetMemSig("GetObjectData", (void **)&addr) || !addr)
		{
			snprintf(error, maxlength, "Game %i does not support ObjectData  modification -- signature missing or otherwise unsupported.", GameID);
			g_pSM->LogMessage(myself, error);
			ObjectData = false;
		}
	}

	// Setup data
	if(WeaponData)
	{
		WeaponData = GetInfo(g_pGameMod->GetKeyValue("weapon_key"), WEAPON_CNAME_LENGTH, WEAPON_CNAME_OFFSET);
		WEAPON_SIZE = GetKeyValueAsInt(g_pGameMod, "__weapon_size");
	}

	if(PlayerData)
	{
		PlayerData = GetInfo(g_pGameMod->GetKeyValue("player_key"), PLAYER_CNAME_LENGTH, PLAYER_CNAME_OFFSET);
		PLAYER_SIZE = GetKeyValueAsInt(g_pGameMod, "__player_size");
	}

	if(ObjectData)
	{
		char Key[80];
		ObjectData = GetInfo(g_pGameMod->GetKeyValue("object_key"), OBJECT_CNAME_LENGTH, OBJECT_CNAME_OFFSET);
		OBJECT_SIZE = GetKeyValueAsInt(g_pGameMod, "__object_size");
	}

	g_pCVar = icvar;
	ConVar_Register(0, this);

	// Late load
	if(!late)
	{
		SH_ADD_HOOK_STATICFUNC(IServerGameDLL, ServerActivate, server, Hook_ServerActivate, true);
	}
	else
	{
		if(!DoInitialisation())
		{
			snprintf(error, maxlength, "Failing due to failed attempts to access the database(s).");
			g_pSM->LogMessage(myself, error);
			return false;
		}
	}

	// Register
	sharesys->AddNatives(myself, g_ExtensionNatives);
	sharesys->RegisterLibrary(myself, "ctx");

	g_pSM->LogMessage(myself, "Loaded.");

	return true;
}

void CTXPatcher::SDK_OnAllLoaded()
{

}

void CTXPatcher::SDK_OnUnload()
{
	ValveDB *DB1; ValveDB *DB2;
	size_t size = 0;
	int NAME_LENGTH; int NAME_OFFSET;

	if(PlayerData)
	{
		DB1 = g_pPlayerClassInfoData;
		DB2 = g_pPlayerClassInfoOriginalData;
		NAME_LENGTH = PLAYER_CNAME_LENGTH;
		NAME_OFFSET = PLAYER_CNAME_OFFSET;
		size = PLAYER_SIZE;
		RestoreSet(DB1, DB2, NAME_OFFSET, NAME_LENGTH, size);
		DB1 = g_pPlayerClassInfoData2;
		DB2 = g_pPlayerClassInfoOriginalData2;
		NAME_LENGTH = PLAYER_CNAME_LENGTH;
		NAME_OFFSET = PLAYER_CNAME_OFFSET;
		RestoreSet(DB1, DB2, NAME_OFFSET, NAME_LENGTH, size);
	}
	if(WeaponData)
	{
		DB1 = g_pWeaponInfoDatabase;
		DB2 = g_pWeaponInfoOriginalDatabase;
		NAME_LENGTH = WEAPON_CNAME_LENGTH;
		NAME_OFFSET = WEAPON_CNAME_OFFSET;
		size = WEAPON_SIZE;
		RestoreSet(DB1, DB2, NAME_OFFSET, NAME_LENGTH, size);
	}
	if(ObjectData)
	{
		DB1 = g_pObjectDatabase;
		DB2 = g_pObjectOriginalDatabase;
		NAME_LENGTH = OBJECT_CNAME_LENGTH;
		NAME_OFFSET = OBJECT_CNAME_OFFSET;
		size = OBJECT_SIZE;
		RestoreSet(DB1, DB2, NAME_OFFSET, NAME_LENGTH, size);
	}

	gameconfs->CloseGameConfigFile(g_pGameConf);
	g_pSM->LogMessage(myself, "Loaded.");
}

bool CTXPatcher::RegisterConCommandBase(ConCommandBase *pCommand)
{
	META_REGCVAR(pCommand);
	return true;
}

void BackUpOneTeam(void *addr)
{
	typedef void* (*GetPlayerClassData)(int iClass);
	for(int i = 0; i < CLASS_COUNT; i++)
	{
		GetPlayerClassData Q = (GetPlayerClassData)addr;
		void *ClassData = Q(i);
		char *className = new char[PLAYER_CNAME_LENGTH];
		memcpy(className, (char *)ClassData+PLAYER_CNAME_OFFSET, PLAYER_CNAME_LENGTH);
		g_pPlayerClassInfoData->Insert(className, ClassData);
		void *OriginalClassData = malloc(PLAYER_SIZE);
		memcpy(OriginalClassData, ClassData, PLAYER_SIZE);
		g_pPlayerClassInfoOriginalData->Insert(className, OriginalClassData);
	}
}

void BackUpTwoTeams(void *addr)
{
	typedef void* (*GetPlayerClassData)(int iTeam, int iClass);
	for(int i = 0; i < CLASS_COUNT; i++)
	{
		GetPlayerClassData Q = (GetPlayerClassData)addr;
		void *ClassData = Q(2,i);
		char *className = new char[PLAYER_CNAME_LENGTH];
		memcpy(className, (char *)ClassData+PLAYER_CNAME_OFFSET, PLAYER_CNAME_LENGTH);
		g_pPlayerClassInfoData->Insert(className, ClassData);
		void *OriginalClassData = malloc(PLAYER_SIZE);
		memcpy(OriginalClassData, ClassData, PLAYER_SIZE);
		g_pPlayerClassInfoOriginalData->Insert(className, OriginalClassData);
	}
	for(int i = 0; i < CLASS_COUNT; i++)
	{
		GetPlayerClassData Q = (GetPlayerClassData)addr;
		void *ClassData = Q(3,i);
		char *className = new char[PLAYER_CNAME_LENGTH];
		memcpy(className, (char *)ClassData+PLAYER_CNAME_OFFSET, PLAYER_CNAME_LENGTH);
		g_pPlayerClassInfoData2->Insert(className, ClassData);
		void *OriginalClassData = malloc(PLAYER_SIZE);
		memcpy(OriginalClassData, ClassData, PLAYER_SIZE);
		g_pPlayerClassInfoOriginalData2->Insert(className, OriginalClassData);
	}
}


bool SetupObjectDatabase()
{
	void *addr;

	g_pObjectDatabase = new ValveDB(); 
	g_pObjectOriginalDatabase = new ValveDB();
	
	int NumObjects = GetKeyValueAsInt(g_pGameMod, "__num_objects");
	if(NumObjects == 0)
	{
		ObjectData = false;
		return false;
	}
	
	if(ObjectData)
	{
		if (!g_pGameConf->GetMemSig("GetObjectData", &addr) || !addr)
		{
			return false;
		}

		typedef void* (*GetObjectData)(int iObject);

		for(int i = 0; i < NumObjects; i++)
		{
			GetObjectData Q = (GetObjectData)addr;
			void *ObjData = Q(i);
			g_pObjectDatabase->Insert((char *)ObjData+OBJECT_CNAME_OFFSET, ObjData);
			void *OriginalClassData = malloc(OBJECT_SIZE);
			memcpy(OriginalClassData, ObjData, OBJECT_SIZE);
			g_pObjectOriginalDatabase->Insert((char *)ObjData+OBJECT_CNAME_OFFSET, OriginalClassData);
		}
	}
	return true;
}


bool SetupPlayerDatabasePointer()
{
	void *addr;

	g_pPlayerClassInfoData = new ValveDB();
	g_pPlayerClassInfoOriginalData = new ValveDB();

	// Dear DOD:S, CS:S, and HL2DM, I hate you.
	g_pPlayerClassInfoData2 = new ValveDB();
	g_pPlayerClassInfoOriginalData2 = new ValveDB();
	

	if(PlayerData)
	{
		if (!g_pGameConf->GetMemSig("GetPlayerClassData", &addr) || !addr)
		{
			return false;
		}

		int Teams = GetKeyValueAsInt(g_pGameMod, "__num_teams");
		bool QueryByTeam = GetKeyValueAsBool(g_pGameMod, "__query_team");
		CLASS_COUNT = GetKeyValueAsInt(g_pGameMod, "__num_classes");
		
		if(!QueryByTeam && Teams == 2)
		{
			CLASS_COUNT /= 2;
			QueryByTeam = true;
		}

		if(QueryByTeam)
		{
			MULTI_TEAM = true;
		}
		
		if(QueryByTeam)
		{
			BackUpTwoTeams(addr);
		}
		else
		{
			BackUpOneTeam(addr);
		}
	}
	else
	{
		return false;
	}
	return true;
}

bool SetupWeaponDatabasePointer()
{		
#ifdef _WIN32
        void *addr;
        int offset;
        if (!g_pGameConf->GetMemSig("ReadWeaponDataFromFileForSlot_SubCall_01", &addr) || !addr)
        {
            return false;
        }
        if (!g_pGameConf->GetOffset("WeaponDatabase_Offset", &offset))
        {
            return false;
        }
        g_pWeaponInfoDatabase = *(ValveDB **)((uint32)addr + offset);
#else
        void *addr;
        if (!g_pGameConf->GetMemSig("m_WeaponInfoDatabase", &addr) || !addr)
        {
            return false;
        }
        
        g_pWeaponInfoDatabase = (ValveDB *)addr;
#endif
	if(g_pWeaponInfoDatabase != NULL)
	{
		return ReplicateWpnDatabase();
	}
	else
	{
		return false;
	}
}

// Replicate the database into a local store
bool ReplicateWpnDatabase()
{
	// This can't actually fail ... unless the database isn't yet loaded(?!)
	g_pWeaponInfoOriginalDatabase = new ValveDB(); // Clear it
	if(!WeaponData) { return false; }
	for(unsigned short i = g_pWeaponInfoDatabase->First(); i != g_pWeaponInfoDatabase->InvalidIndex(); i = g_pWeaponInfoDatabase->Next(i))
	{
		// As class T says in the template classes: I pity the foo who walks into this bar.
		void *WI = NULL;
		while(WI == NULL)
		{
			WI = malloc(WEAPON_SIZE);
		}
		memcpy(WI, g_pWeaponInfoDatabase->Element(i), WEAPON_SIZE);
		char *className = new char[WEAPON_CNAME_LENGTH];
		memcpy(className, (char *)g_pWeaponInfoDatabase->Element(i)+WEAPON_CNAME_OFFSET, WEAPON_CNAME_LENGTH);
		g_pWeaponInfoOriginalDatabase->Insert(className, WI);
	}
	return true;
}

bool CTXPatcher::SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
	GET_V_IFACE_ANY(GetServerFactory, gameclients, IServerGameClients, INTERFACEVERSION_SERVERGAMECLIENTS);
	GET_V_IFACE_ANY(GetServerFactory, server, IServerGameDLL, INTERFACEVERSION_SERVERGAMEDLL);
	GET_V_IFACE_ANY(GetServerFactory, gameents, IServerGameEnts, INTERFACEVERSION_SERVERGAMEENTS);
	GET_V_IFACE_CURRENT(GetEngineFactory, engine, IVEngineServer, INTERFACEVERSION_VENGINESERVER);
	GET_V_IFACE_CURRENT(GetEngineFactory, icvar, ICvar, CVAR_INTERFACE_VERSION);
	gpGlobals = ismm->GetCGlobals();
	
	return true;
}


// ----------------------------------------------------------------------------
// Helpers
// ----------------------------------------------------------------------------

bool RestoreSingleObject(ValveDB *DB1, ValveDB *DB2, size_t size, const char *Classname)
{
	unsigned short GameIndex = DB1->Find(Classname);
	unsigned short OriginalIndex = DB2->Find(Classname);
	if(DB1->IsValidIndex(GameIndex) && DB2->IsValidIndex(OriginalIndex))
	{
		memcpy(DB1->Element(GameIndex), DB2->Element(OriginalIndex), size);
		return true;
	}
	return false;
}

// Restore weapons by name
bool RestoreSet(ValveDB *DB1, ValveDB *DB2, int NAME_OFFSET, int NAME_LENGTH, size_t size)
{
	for(unsigned short i = DB2->First(); i != DB2->InvalidIndex(); i = DB2->Next(i))
	{
		char className[128];
		memcpy(className, (char *)((unsigned char*)DB2->Element(i)+NAME_OFFSET), NAME_LENGTH);
		RestoreSingleObject(DB1, DB2, size, className);
	}
	// Should never fail, if it does, uh ... I can't think of how it did.
	return true;
}


cell_t GetUCharRtn(IPluginContext *pContext, unsigned char *uchar, int maxlength)
{
	cell_t output;
	char *OutChar = (char *)uchar;
	pContext->StringToLocal(output, sizeof(uchar), OutChar);
	return output;
}

cell_t GetCharRtn(IPluginContext *pContext, const char *achar, int maxlength)
{
	cell_t output;
	char *OutChar = (char *)achar;
	pContext->StringToLocal(output, sizeof(achar), OutChar);
	return output;
}

// ----------------------------------------------------------------------------
// NATIVES
// ----------------------------------------------------------------------------

static cell_t CTX_Set(IPluginContext *pContext, const cell_t *params)
{
	char *CTX_TYPE;
	ValveDB *DB1;
	ValveDB *DB2;

	switch(params[1])
	{
		case CTX_PLAYER:
			CTX_TYPE = "player";
			DB1 = g_pPlayerClassInfoData;
			DB2 = g_pPlayerClassInfoOriginalData;
			if(!PlayerData)
			{
				pContext->ThrowNativeError("PlayerData is not supported for this mod."); return false;
			}
			break;
		case CTX_PLAYER_TEAM2:
			CTX_TYPE = "player";
			DB1 = g_pPlayerClassInfoData2;
			DB2 = g_pPlayerClassInfoOriginalData2;
			if(!PlayerData)
			{
				pContext->ThrowNativeError("PlayerData is not supported for this mod."); return false;
			}
			break;
		case CTX_WEAPON:
			CTX_TYPE = "weapon";
			DB1 = g_pWeaponInfoDatabase;
			DB2 = g_pWeaponInfoOriginalDatabase;
			if(!WeaponData)
			{
				pContext->ThrowNativeError("WeaponData is not supported for this mod."); return false;
			}
			break;
		case CTX_OBJECT:
			CTX_TYPE = "object";
			DB1 = g_pObjectDatabase;
			DB2 = g_pObjectOriginalDatabase;
			if(!ObjectData)
			{
				pContext->ThrowNativeError("ObjectData is not supported for this mod."); return false;
			}
			break;
		default:
			pContext->ThrowNativeError("Unknown CTX_TYPE"); return false;
			return false;
	}

	char *CTX_DTYPE;
	switch(params[2])
	{
		case CTX_STRING:
			CTX_DTYPE = "string";break;
		case CTX_BOOL:
			CTX_DTYPE = "bool";break;
		case CTX_INT:
			CTX_DTYPE = "int";break;
		case CTX_FLOAT:
			CTX_DTYPE = "float";break;
		case CTX_PTR_STRING:
			CTX_DTYPE = "ptr_string";break;
		default:
			pContext->ThrowNativeError("Unknown CTX_DTYPE"); return false;
			return false;
	}

	char *Variable; pContext->LocalToString(params[3], &Variable);
	char *Target; pContext->LocalToString(params[4], &Target);
	char BaseKey[80];
	sprintf(BaseKey, "%s_%s_%s", CTX_TYPE, CTX_DTYPE, Variable);

	int Length = 0; int Offset = 0;
	if(!GetInfo(BaseKey, Length, Offset))
	{
		pContext->ThrowNativeError("Failed to get length and/or offsets (L: %i, O: %i)", Length, Offset);
		return false;
	}
	if(Length == 0)
	{
		pContext->ThrowNativeError("Length of zero detected -- invalid call.");
		return false;
	}

	unsigned short DBIndex = DB1->Find(Target);
	if(!DB1->IsValidIndex(DBIndex))
	{
		pContext->ThrowNativeError("Data was not found in the respective database; incorrect key.");
		return false;
	}

	char *NewValue; pContext->LocalToString(params[5], &NewValue);
	void *Data = DB1->Element(DBIndex);
	switch(params[2])
	{
		case CTX_STRING:
			memcpy(((char*)Data+Offset), NewValue, Length);
			return true;
			break;
		case CTX_BOOL:
			*(bool*)((char*)Data+Offset) = (bool)atoi(NewValue);
			return true;
			break;
		case CTX_INT:
			*(int*)((char*)Data+Offset) = atoi(NewValue);
			return true;
			break;
		case CTX_FLOAT:
			*(float*)((char*)Data+Offset) = atof(NewValue);
			return true;
			break;
		case CTX_PTR_STRING:
			*(char**)((char*)Data+Offset) = NewValue;
			return true;
			break;
		default:
			pContext->ThrowNativeError("Unknown CTX_DTYPE"); return false;
			return false;
	}

	pContext->ThrowNativeError("WARNING: SOMETHING WENT HORRIBLY WRONG!"); return false;
	return false; // Good cell, bad result :3
}

static cell_t CTX_Get(IPluginContext *pContext, const cell_t *params)
{
	char *CTX_TYPE;
	ValveDB *DB1;
	ValveDB *DB2;

	switch(params[1])
	{
		case CTX_PLAYER:
			CTX_TYPE = "player";
			DB1 = g_pPlayerClassInfoData;
			DB2 = g_pPlayerClassInfoOriginalData;
			if(!PlayerData)
			{
				pContext->ThrowNativeError("PlayerData is not supported for this mod."); return false;
			}
			break;
		case CTX_PLAYER_TEAM2:
			CTX_TYPE = "player";
			DB1 = g_pPlayerClassInfoData2;
			DB2 = g_pPlayerClassInfoOriginalData2;
			if(!PlayerData)
			{
				pContext->ThrowNativeError("PlayerData is not supported for this mod."); return false;
			}
			break;
		case CTX_WEAPON:
			CTX_TYPE = "weapon";
			DB1 = g_pWeaponInfoDatabase;
			DB2 = g_pWeaponInfoOriginalDatabase;
			if(!WeaponData)
			{
				pContext->ThrowNativeError("WeaponData is not supported for this mod."); return false;
			}
			break;
		case CTX_OBJECT:
			CTX_TYPE = "object";
			DB1 = g_pObjectDatabase;
			DB2 = g_pObjectOriginalDatabase;
			if(!ObjectData)
			{
				pContext->ThrowNativeError("ObjectData is not supported for this mod."); return false;
			}
			break;
		default:
			pContext->ThrowNativeError("Unknown CTX_TYPE"); return false;
			return false;
	}

	char *CTX_DTYPE;
	switch(params[2])
	{
		case CTX_STRING:
			CTX_DTYPE = "string";break;
		case CTX_BOOL:
			CTX_DTYPE = "bool";break;
		case CTX_INT:
			CTX_DTYPE = "int";break;
		case CTX_FLOAT:
			CTX_DTYPE = "float";break;
		case CTX_PTR_STRING:
			CTX_DTYPE = "ptr_string";break;
		default:
			pContext->ThrowNativeError("Unknown CTX_DTYPE"); return false;
			return false;
	}

	char *Variable; pContext->LocalToString(params[3], &Variable);
	char *Target; pContext->LocalToString(params[4], &Target);
	char BaseKey[80];
	sprintf(BaseKey, "%s_%s_%s", CTX_TYPE, CTX_DTYPE, Variable);

	int Length = 0; int Offset = 0;
	if(!GetInfo(BaseKey, Length, Offset))
	{
		pContext->ThrowNativeError("Failed to get length and/or offsets (L: %i, O: %i)", Length, Offset);
		return false;
	}
	if(Length == 0)
	{
		pContext->ThrowNativeError("Length of zero detected -- invalid call.");
		return false;
	}

	unsigned short DBIndex = DB1->Find(Target);
	if(!DB1->IsValidIndex(DBIndex))
	{
		pContext->ThrowNativeError("Data was not found in the respective database; incorrect key.");
		return false;
	}

	void *Data = DB1->Element(DBIndex);
	switch(params[2])
	{
		case CTX_STRING:
			pContext->StringToLocal(params[5], Length, ((char*)Data+Offset));
			return true;
			break;
		case CTX_BOOL:
			return *(bool*)((char*)Data+Offset);
			break;
		case CTX_INT:
			return *(int*)((char*)Data+Offset);
			break;
		case CTX_FLOAT:
			return sp_ftoc(*(float*)((char*)Data+Offset));
			break;
		case CTX_PTR_STRING:
			pContext->StringToLocal(params[5], sizeof(*((char*)Data+Offset)), ((char*)Data+Offset));
			return true;
			break;
		default:
			pContext->ThrowNativeError("Unknown CTX_DTYPE"); return false;
			return false;
	}

	pContext->ThrowNativeError("WARNING: SOMETHING WENT HORRIBLY WRONG!"); return false;
	return false; // Good cell, bad result :3
}

static cell_t CTX_ResetOne(IPluginContext *pContext, const cell_t *params)
{
	char *ItemName; pContext->LocalToString(params[2],&ItemName);

	ValveDB *DB1; ValveDB *DB2;
	size_t size = 0;
	int NAME_LENGTH; int NAME_OFFSET;
	switch(params[1])
	{
		case CTX_PLAYER:
			DB1 = g_pPlayerClassInfoData;
			DB2 = g_pPlayerClassInfoOriginalData;
			NAME_LENGTH = PLAYER_CNAME_LENGTH;
			NAME_OFFSET = PLAYER_CNAME_OFFSET;
			size = PLAYER_SIZE;
			break;
		case CTX_PLAYER_TEAM2:
			DB1 = g_pPlayerClassInfoData2;
			DB2 = g_pPlayerClassInfoOriginalData2;
			NAME_LENGTH = PLAYER_CNAME_LENGTH;
			NAME_OFFSET = PLAYER_CNAME_OFFSET;
			size = PLAYER_SIZE;
			break;
		case CTX_WEAPON:
			DB1 = g_pWeaponInfoDatabase;
			DB2 = g_pWeaponInfoOriginalDatabase;
			NAME_LENGTH = WEAPON_CNAME_LENGTH;
			NAME_OFFSET = WEAPON_CNAME_OFFSET;
			size = WEAPON_SIZE;
			break;
		case CTX_OBJECT:
			DB1 = g_pObjectDatabase;
			DB2 = g_pObjectOriginalDatabase;
			NAME_LENGTH = OBJECT_CNAME_LENGTH;
			NAME_OFFSET = OBJECT_CNAME_OFFSET;
			size = OBJECT_SIZE;
			break;
		default:
			return false;
	}
	RestoreSingleObject(DB1, DB2, size, ItemName);
	return true;
}

static cell_t CTX_ResetAll(IPluginContext *pContext, const cell_t *params)
{
	ValveDB *DB1; ValveDB *DB2;
	size_t size = 0;
	int NAME_LENGTH; int NAME_OFFSET;
	switch(params[1])
	{
		case CTX_PLAYER:
			DB1 = g_pPlayerClassInfoData;
			DB2 = g_pPlayerClassInfoOriginalData;
			NAME_LENGTH = PLAYER_CNAME_LENGTH;
			NAME_OFFSET = PLAYER_CNAME_OFFSET;
			size = PLAYER_SIZE;
			break;
		case CTX_PLAYER_TEAM2:
			DB1 = g_pPlayerClassInfoData2;
			DB2 = g_pPlayerClassInfoOriginalData2;
			NAME_LENGTH = PLAYER_CNAME_LENGTH;
			NAME_OFFSET = PLAYER_CNAME_OFFSET;
			size = PLAYER_SIZE;
			break;
		case CTX_WEAPON:
			DB1 = g_pWeaponInfoDatabase;
			DB2 = g_pWeaponInfoOriginalDatabase;
			NAME_LENGTH = WEAPON_CNAME_LENGTH;
			NAME_OFFSET = WEAPON_CNAME_OFFSET;
			size = WEAPON_SIZE;
			break;
		case CTX_OBJECT:
			DB1 = g_pObjectDatabase;
			DB2 = g_pObjectOriginalDatabase;
			NAME_LENGTH = OBJECT_CNAME_LENGTH;
			NAME_OFFSET = OBJECT_CNAME_OFFSET;
			size = OBJECT_SIZE;
			break;
		default:
			return false;
	}
	RestoreSet(DB1, DB2, NAME_OFFSET, NAME_LENGTH, size);
	return true;
}