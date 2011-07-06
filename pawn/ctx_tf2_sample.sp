#pragma semicolon 1

/*--------------------------------------------------------------------------------
/ Includes
/-------------------------------------------------------------------------------*/
#include <sourcemod>
#define REQUIRE_EXTENSIONS
#include <ctx>

/*--------------------------------------------------------------------------------
/ Constants
/-------------------------------------------------------------------------------*/
#define PLUGIN_NAME		"[CTX] Simple TF2 Example"
#define PLUGIN_AUTHOR		"A.W. 'Swixel' Stanley"
#define PLUGIN_VERSION		"0.0.1"
#define PLUGIN_CONTACT		"http://swixel.net/"

/*--------------------------------------------------------------------------------
/ Plugin
/-------------------------------------------------------------------------------*/
public Plugin:myinfo =
{
	name			= PLUGIN_NAME,
	author			= PLUGIN_AUTHOR,
	description		= PLUGIN_NAME,
	version			= PLUGIN_VERSION,
	url				= PLUGIN_CONTACT
};

public OnPluginStart()
{
	CreateConVar("sm_ctxsprojectile_version", PLUGIN_VERSION, PLUGIN_NAME, FCVAR_SPONLY|FCVAR_REPLICATED|FCVAR_NOTIFY);
	RegAdminCmd("sm_ctxsample_poke", CTXPokeMinigun, ADMFLAG_GENERIC, "sm_ctxsample_poke");
	RegAdminCmd("sm_ctxsample_fix", CTXFixMinigun, ADMFLAG_GENERIC, "sm_ctxsample_fix");
}													  


/*--------------------------------------------------------------------------------
/ Commands
/-------------------------------------------------------------------------------*/

public Action:CTXPokeMinigun(client, args)
{
	ReplyToCommand(client,"Testing CTX_INT on tf_weapon_minigun");
	ReplyToCommand(client,"1 - Existing Primary Damage: %i", CTX_Get(CTX_WEAPON, CTX_INT, "pdamage", "tf_weapon_minigun"));
	CTX_Set(CTX_WEAPON, CTX_INT, "pdamage", "tf_weapon_minigun", "25");
	ReplyToCommand(client,"1 - Updated Primary Damage should be '25': %i", CTX_Get(CTX_WEAPON, CTX_INT, "pdamage", "tf_weapon_minigun"));
	
	ReplyToCommand(client,"Testing CTX_FLOAT on tf_weapon_minigun");
	ReplyToCommand(client,"2 - Existing Damage Radius: %i", CTX_Get(CTX_WEAPON, CTX_FLOAT, "damageradius", "tf_weapon_minigun"));
	CTX_Set(CTX_WEAPON, CTX_FLOAT, "damageradius", "tf_weapon_minigun", "50.0");
	ReplyToCommand(client,"2 - Updated Damage Radius should be '50.0': %f", CTX_Get(CTX_WEAPON, CTX_FLOAT, "damageradius", "tf_weapon_minigun"));

	ReplyToCommand(client,"Testing CTX_STRING on tf_weapon_minigun");
	new String:PAType[80]; new String:PAType2[80];
	CTX_Get(CTX_WEAPON, CTX_STRING, "pammo", "tf_weapon_minigun", PAType);
	ReplyToCommand(client,"3 - Existing: %s", PAType);
	CTX_Set(CTX_WEAPON, CTX_STRING, "pammo", "tf_weapon_minigun", "THIS IS A BAD IDEA");
	CTX_Get(CTX_WEAPON, CTX_STRING, "pammo", "tf_weapon_minigun", PAType2);
	ReplyToCommand(client,"3 - Updated to 'THIS IS A BAD IDEA': %s", PAType2);
	CTX_Set(CTX_WEAPON, CTX_STRING, "pammo", "tf_weapon_minigun", PAType);
	CTX_Get(CTX_WEAPON, CTX_STRING, "pammo", "tf_weapon_minigun", PAType2);
	ReplyToCommand(client,"3 - Resetting: %s", PAType2);
   
	ReplyToCommand(client,"Testing CTX_BOOL on tf_weapon_minigun");
	ReplyToCommand(client,"4 - Existing AutoSwitchTo: %i", CTX_Get(CTX_WEAPON, CTX_BOOL, "autoswitchto", "tf_weapon_minigun"));
	CTX_Set(CTX_WEAPON, CTX_BOOL, "autoswitchto", "tf_weapon_minigun", "0");
	ReplyToCommand(client,"4 - Updated AutoSwitchTo 0: %i", CTX_Get(CTX_WEAPON, CTX_BOOL, "autoswitchto", "tf_weapon_minigun"));
	CTX_Set(CTX_WEAPON, CTX_BOOL, "autoswitchto", "tf_weapon_minigun", "1");
	ReplyToCommand(client,"4 - Updated AutoSwitchTo 1: %i", CTX_Get(CTX_WEAPON, CTX_BOOL, "autoswitchto", "tf_weapon_minigun"));
}


public Action:CTXFixMinigun(client, args)
{
	CTX_Reset(CTX_WEAPON, "tf_weapon_minigun");
	ReplyToCommand(client, "Consider the minigun fixed.");   
}