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

#ifndef _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_
#define _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_

#define CTX_PLAYER 0
#define CTX_PLAYER_TEAM2 1
#define CTX_WEAPON 2
#define CTX_OBJECT 3

#define CTX_STRING		0
#define CTX_BOOL		1
#define CTX_INT			2
#define CTX_FLOAT		3
#define CTX_PTR_STRING	4

#include "smsdk_ext.h"
#include "utldict.h"
using namespace std;

using namespace SourceHook;

// DB Type
typedef CUtlDict<void*,unsigned short> ValveDB;

bool SetupWeaponDatabasePointer();
bool ReplicateWpnDatabase();

bool SetupPlayerDatabasePointer();
bool SetupObjectDatabase();

class CTXPatcher : public SDKExtension, public IConCommandBaseAccessor
{
public: //SDK
	virtual bool SDK_OnLoad(char *error, size_t maxlength, bool late);
	virtual void SDK_OnUnload();
public:
#if defined SMEXT_CONF_METAMOD
	virtual bool SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlength, bool late);
	void SDK_OnAllLoaded();
#endif
public: //IConCommandBaseAccessor
	bool RegisterConCommandBase(ConCommandBase *pCommand);
};

extern IGameConfig *g_pGameConf;
extern sm_sendprop_info_t *playerSharedOffset;

// Utils
bool RestoreSingleObject(ValveDB *DB1, ValveDB *DB2, size_t size, const char *Classname);
bool RestoreSet(ValveDB *DB1, ValveDB *DB2, int NAME_OFFSET, int NAME_LENGTH, size_t size);

void BackUpOneTeam(void *addr);
void BackUpTwoTeams(void *addr);

// Manually written Natives
static cell_t CTX_ResetOne(IPluginContext *pContext, const cell_t *params);
static cell_t CTX_ResetAll(IPluginContext *pContext, const cell_t *params);
static cell_t CTX_Set(IPluginContext *pContext, const cell_t *params);
static cell_t CTX_Get(IPluginContext *pContext, const cell_t *params);

#endif // _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_