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
	{ "CTX_Reset",				CTX_ResetOne },
	{ "CTX_ResetAll",			CTX_ResetAll },
	{ "CTX_SetBool",			CTX_SetBool },
	{ "CTX_GetBool",			CTX_GetBool },
	{ "CTX_SetFloat",			CTX_SetFloat },
	{ "CTX_GetFloat",			CTX_GetFloat },
	{ "CTX_SetInt",				CTX_SetInt },
	{ "CTX_GetInt",				CTX_GetInt },
	{ "CTX_SetString",			CTX_SetString },
	{ "CTX_GetString",			CTX_GetString },
	{ "CTX_SetStringPtr",		CTX_SetStringPtr },
	{ "CTX_GetStringPtr",		CTX_GetStringPtr },
	{ NULL,				NULL }
};

int GetKeyValueAsInt(IGameConfig *p, char *Key)
{
	IGameConfig *GC = p;
	const char *Value = GC->GetKeyValue(Key);
	if(Value == NULL)
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
	if(Value == NULL)
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
		Offset = GetKeyValueAsInt(g_pGameConf,COffset);
	}

	if(strlen(CLength) < 8)
	{
		g_pSM->LogMessage(myself, "Length Failed");
		return false;
	}
	else
	{
		Length = GetKeyValueAsInt(g_pGameConf,CLength);
	}
	return true;
}

bool DoInitialisation()
{
	// Initialise to guard
	g_pPlayerClassInfoData = new ValveDB();
	g_pPlayerClassInfoOriginalData = new ValveDB();
	g_pPlayerClassInfoData2 = new ValveDB();
	g_pPlayerClassInfoOriginalData2 = new ValveDB();
	g_pWeaponInfoDatabase = new ValveDB();
	g_pWeaponInfoOriginalDatabase = new ValveDB();
	g_pObjectDatabase = new ValveDB(); 
	g_pObjectOriginalDatabase = new ValveDB();

	if(WeaponData)
	{	
		if(!SetupWeaponDatabasePointer())
		{
			g_pSM->LogMessage(myself, "Disabled: Could not get the weapon database.");
			WeaponData = false;
		}
	}

	if(PlayerData)
	{
		if(!SetupPlayerDatabasePointer())
		{
			g_pSM->LogMessage(myself, "Disabled: Could not get the player database(s).");
			PlayerData = false;
		}
	}

	if(ObjectData)
	{
		if(!SetupObjectDatabase())
		{
			g_pSM->LogMessage(myself, "Disabled: Could not get the player database(s).");
			ObjectData = false;
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
	// Register first
	sharesys->AddNatives(myself, g_ExtensionNatives);
	sharesys->RegisterLibrary(myself, "ctx");


	char conf_error[255] = "";

	/* This mod's conf */
	if (!gameconfs->LoadGameConfigFile("ctx.games", &g_pGameConf, conf_error, sizeof(conf_error)))
	{
		if (conf_error[0])
		{
			snprintf(error, maxlength, "Could not read ctx/master.games.txt: %s", conf_error);
			g_pSM->LogMessage(myself, error);											  
		}
		return false;
	}

	char *addr;
	int iOffset;

	// TODO: registration

	WeaponData = GetKeyValueAsBool(g_pGameConf, "weapon_enabled");
	PlayerData = GetKeyValueAsBool(g_pGameConf, "player_enabled");
	ObjectData = GetKeyValueAsBool(g_pGameConf, "object_enabled");

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
		WeaponData = GetInfo(g_pGameConf->GetKeyValue("weapon_key"), WEAPON_CNAME_LENGTH, WEAPON_CNAME_OFFSET);
		WEAPON_SIZE = GetKeyValueAsInt(g_pGameConf, "__weapon_size");
	}

	if(PlayerData)
	{
		PlayerData = GetInfo(g_pGameConf->GetKeyValue("player_key"), PLAYER_CNAME_LENGTH, PLAYER_CNAME_OFFSET);
		PLAYER_SIZE = GetKeyValueAsInt(g_pGameConf, "__player_size");
	}

	if(ObjectData)
	{
		ObjectData = GetInfo(g_pGameConf->GetKeyValue("object_key"), OBJECT_CNAME_LENGTH, OBJECT_CNAME_OFFSET);
		OBJECT_SIZE = GetKeyValueAsInt(g_pGameConf, "__object_size");
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
	
	int NumObjects = GetKeyValueAsInt(g_pGameConf, "__num_objects");
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

		int Teams = GetKeyValueAsInt(g_pGameConf, "__num_teams");
		bool QueryByTeam = GetKeyValueAsBool(g_pGameConf, "__query_team");
		CLASS_COUNT = GetKeyValueAsInt(g_pGameConf, "__num_classes");
		
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
	if(DB1 == NULL || DB2 == NULL)
	{
		g_pSM->LogMessage(myself, "Database is null -- unsupported feature or serious error.");
		return false;
	}
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
	if(DB1 == NULL || DB2 == NULL)
	{
		g_pSM->LogMessage(myself, "Database is null -- unsupported feature or serious error.");
		return false;
	}
	for(unsigned short i = DB2->First(); i != DB2->InvalidIndex(); i = DB2->Next(i))
	{
		char className[128];
		memcpy(className, (char *)((unsigned char*)DB2->Element(i)+NAME_OFFSET), NAME_LENGTH);
		RestoreSingleObject(DB1, DB2, size, className);
	}
	// Should never fail, if it does, uh ... I can't think of how it did.
	return true;
}


void *GetOffset(int iCTX_TYPE, char *CTX_DTYPE, char *Target, char *Variable, int &Length)
{
	const char *CTX_TYPE;
	ValveDB *DB;
	switch(iCTX_TYPE)
	{
		case CTX_PLAYER:
			CTX_TYPE = "player";
			DB = g_pPlayerClassInfoData;
			if(!PlayerData)
			{
				g_pSM->LogMessage(myself, "PlayerData is not supported for this mod."); return NULL;
			}
			break;
		case CTX_PLAYER_TEAM2:
			CTX_TYPE = "player";
			DB = g_pPlayerClassInfoData2;
			if(!PlayerData)
			{
				g_pSM->LogMessage(myself, "PlayerData is not supported for this mod."); return NULL;
			}
			break;
		case CTX_WEAPON:
			CTX_TYPE = "weapon";
			DB = g_pWeaponInfoDatabase;
			if(!WeaponData)
			{
				g_pSM->LogMessage(myself, "WeaponData is not supported for this mod."); return NULL;
			}
			break;
		case CTX_OBJECT:
			CTX_TYPE = "object";
			DB = g_pObjectDatabase;
			if(!ObjectData)
			{
				g_pSM->LogMessage(myself, "ObjectData is not supported for this mod."); return NULL;
			}
			break;
		default:
			g_pSM->LogMessage(myself, "Unknown CTX_TYPE"); return NULL;
	}

	char BaseKey[80];
	sprintf(BaseKey, "%s_%s_%s", CTX_TYPE, CTX_DTYPE, Variable);

	int Offset = 0;
	if(!GetInfo(BaseKey, Length, Offset))
	{
		g_pSM->LogMessage(myself, "Failed to get length and/or offsets (L: %i, O: %i)", Length, Offset);
		return NULL;
	}
	if(Length == 0)
	{
		g_pSM->LogMessage(myself, "Length of zero detected -- invalid call.");
		return NULL;
	}

	unsigned short DBIndex = DB->Find(Target);
	if(!DB->IsValidIndex(DBIndex))
	{
		g_pSM->LogMessage(myself, "Invalid item name.");
		return NULL;
	}
	return (void*)((char*)DB->Element(DBIndex)+Offset);
}

// ----------------------------------------------------------------------------
// NATIVES
// ----------------------------------------------------------------------------

// native bool:CTX_SetBool(CTX_TYPE, String:Variable[], String:strClassName[], bool:NewValue);
static cell_t CTX_SetBool(IPluginContext *pContext, const cell_t *params)
{
	int Length; char *Target; pContext->LocalToString(params[3], &Target); char *Variable; pContext->LocalToString(params[2], &Variable);
	void *Data = GetOffset(params[1], "bool", Target, Variable, Length);
	if(Data == NULL) return false;
	*(bool*)(Data) = params[4];
	return true;
}

// native bool:CTX_GetBool(CTX_TYPE, String:Variable[], String:strClassName[], bool:ReturnedBool)
static cell_t CTX_GetBool(IPluginContext *pContext, const cell_t *params)
{
	int Length; char *Target; pContext->LocalToString(params[3], &Target); char *Variable; pContext->LocalToString(params[2], &Variable);
	void *Data = GetOffset(params[1], "bool", Target, Variable, Length);
	if(Data == NULL) return false;
	cell_t *addr;
	pContext->LocalToPhysAddr(params[4], &addr); 
	*addr = *(bool*)(Data);
	return true;
}

// native bool:CTX_SetFloat(CTX_TYPE, String:Variable[], String:strClassName[], Float:NewValue);
static cell_t CTX_SetFloat(IPluginContext *pContext, const cell_t *params)
{
	int Length; char *Target; pContext->LocalToString(params[3], &Target); char *Variable; pContext->LocalToString(params[2], &Variable);
	void *Data = GetOffset(params[1], "float", Target, Variable, Length);
	if(Data == NULL) return false;
	*(float*)(Data) = sp_ctof(params[4]);
	return true;
}

// native bool:CTX_GetFloat(CTX_TYPE, String:Variable[], String:strClassName[], Float:ReturnedFloat)
static cell_t CTX_GetFloat(IPluginContext *pContext, const cell_t *params)
{
	int Length; char *Target; pContext->LocalToString(params[3], &Target); char *Variable; pContext->LocalToString(params[2], &Variable);
	void *Data = GetOffset(params[1], "float", Target, Variable, Length);
	if(Data == NULL) return false;
	cell_t *addr;
	pContext->LocalToPhysAddr(params[4], &addr); 
	*addr = sp_ftoc(*(float*)(Data));
	return true;
}

// native bool:CTX_SetInt(CTX_TYPE, String:Variable[], String:strClassName[], NewValue);
static cell_t CTX_SetInt(IPluginContext *pContext, const cell_t *params)
{
	int Length; char *Target; pContext->LocalToString(params[3], &Target); char *Variable; pContext->LocalToString(params[2], &Variable);
	void *Data = GetOffset(params[1], "int", Target, Variable, Length);
	if(Data == NULL) return false;
	*(int*)(Data) = params[4];
	return true;
}

// native bool:CTX_GetInt(CTX_TYPE, String:Variable[], String:strClassName[], ReturnedInt)
static cell_t CTX_GetInt(IPluginContext *pContext, const cell_t *params)
{
	int Length; char *Target; pContext->LocalToString(params[3], &Target); char *Variable; pContext->LocalToString(params[2], &Variable);
	void *Data = GetOffset(params[1], "int", Target, Variable, Length);
	if(Data == NULL) return false;
	cell_t *addr;
	pContext->LocalToPhysAddr(params[4], &addr); 
	*addr = *(int*)(Data);
	return true;
}

// native bool:CTX_SetString(CTX_TYPE, String:Variable[], String:strClassName[], String:NewValue[]);
static cell_t CTX_SetString(IPluginContext *pContext, const cell_t *params)
{
	int Length; char *Target; pContext->LocalToString(params[3], &Target); char *Variable; pContext->LocalToString(params[2], &Variable);
	void *Data = GetOffset(params[1], "string", Target, Variable, Length);
	if(Data == NULL) return false;
	char *NewValue; pContext->LocalToString(params[4], &NewValue);
	memcpy(Data, NewValue, Length);
	return true;
}

// native bool:CTX_GetString(CTX_TYPE, String:Variable[], String:strClassName[], String:ReturnBuffer[])
static cell_t CTX_GetString(IPluginContext *pContext, const cell_t *params)
{
	int Length; char *Target; pContext->LocalToString(params[3], &Target); char *Variable; pContext->LocalToString(params[2], &Variable);
	void *Data = GetOffset(params[1], "string", Target, Variable, Length);
	if(Data == NULL) return false;
	pContext->StringToLocal(params[4], Length, (char*)Data);
	return true;
}

// native bool:CTX_SetStringPtr(CTX_TYPE, String:Variable[], String:strClassName[], String:NewValue[]);
static cell_t CTX_SetStringPtr(IPluginContext *pContext, const cell_t *params)
{
	int Length; char *Target; pContext->LocalToString(params[3], &Target); char *Variable; pContext->LocalToString(params[2], &Variable);
	void *Data = GetOffset(params[1], "ptr_string", Target, Variable, Length);
	if(Data == NULL) return false;
	char *NewValue; pContext->LocalToString(params[4], &NewValue);
	*(char**)(Data) = NewValue;	return true;
}

// native bool:CTX_GetStringPtr(CTX_TYPE, String:Variable[], String:strClassName[], String:ReturnBuffer[])
static cell_t CTX_GetStringPtr(IPluginContext *pContext, const cell_t *params)
{
	int Length; char *Target; pContext->LocalToString(params[3], &Target); char *Variable; pContext->LocalToString(params[2], &Variable);
	void *Data = GetOffset(params[1], "ptr_string", Target, Variable, Length);
	if(Data == NULL) return false;
	pContext->StringToLocal(params[4], sizeof((char*)Data), (char*)Data);
	return true;
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
	return RestoreSingleObject(DB1, DB2, size, ItemName);
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
	return RestoreSet(DB1, DB2, NAME_OFFSET, NAME_LENGTH, size);
}