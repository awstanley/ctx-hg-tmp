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
#define PLUGIN_VERSION		"0.0.2"
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
	// We now use buffers for everything!
	
	// Primary Damage (int) buffer
	new PDamage;
	
	// Damage Radius
	new Float:DRadius;
	
	// Primary Ammo return and temp store
	new String:PAType[80]; new String:PAType2[80];
	
	// I pity the bool...
	new bool:bSwitchTo;
	
	ReplyToCommand(client,"Testing int on tf_weapon_minigun");
	CTX_GetInt(CTX_WEAPON, "pdamage", "tf_weapon_minigun", PDamage);
	ReplyToCommand(client,"1 - Existing Primary Damage: %i", PDamage);
	
	if(CTX_SetInt(CTX_WEAPON, "pdamage", "tf_weapon_minigun", 25))
	{
		CTX_GetInt(CTX_WEAPON, "pdamage", "tf_weapon_minigun", PDamage);
		ReplyToCommand(client,"1 - Updated Primary Damage should be '25': %i", PDamage);
	}
	else
	{
		ReplyToCommand(client,"1 - Failed to updated Primary Damage");
	}
	
	ReplyToCommand(client,"Testing floats on tf_weapon_minigun");
	CTX_GetFloat(CTX_WEAPON, "damageradius", "tf_weapon_minigun", DRadius);	
	ReplyToCommand(client,"2 - Existing Damage Radius: %f", DRadius);
	if(CTX_SetFloat(CTX_WEAPON, "damageradius", "tf_weapon_minigun", 50.0))
	{
		CTX_GetFloat(CTX_WEAPON, "damageradius", "tf_weapon_minigun", DRadius);
		ReplyToCommand(client,"2 - Updated Damage Radius should be '50.0': %f", DRadius);
	}
	else
	{
		ReplyToCommand(client,"2 - Failed to updated Damage Radius");
	}

	ReplyToCommand(client,"Testing Strings (not pointer strings) on tf_weapon_minigun");
	CTX_GetString(CTX_WEAPON, "pammo", "tf_weapon_minigun", PAType);
	ReplyToCommand(client,"3 - Existing: %s", PAType);
	if(CTX_SetString(CTX_WEAPON, "pammo", "tf_weapon_minigun", "THIS IS A BAD IDEA"))
	{
		CTX_GetString(CTX_WEAPON, "pammo", "tf_weapon_minigun", PAType2);
		ReplyToCommand(client,"3 - Updated to 'THIS IS A BAD IDEA': %s", PAType2);
		
		if(CTX_SetString(CTX_WEAPON, "pammo", "tf_weapon_minigun", PAType))
		{
			CTX_GetString(CTX_WEAPON, "pammo", "tf_weapon_minigun", PAType2);
			ReplyToCommand(client,"3 - Reset Primary Ammo Type: %s", PAType2);
		}
		else
		{
			ReplyToCommand(client,"3 - Failed to reset Primary Ammo Type", PAType2);
		}
	}
	else
	{
		ReplyToCommand(client,"3 - Failed to set Primary Ammo Type", PAType2);
	}

   
	ReplyToCommand(client,"Testing boolean on tf_weapon_minigun");
	CTX_GetBool(CTX_WEAPON, "autoswitchto", "tf_weapon_minigun", bSwitchTo);
	ReplyToCommand(client,"4 - Existing AutoSwitchTo: %i", bSwitchTo);
	if(CTX_SetBool(CTX_WEAPON, "autoswitchto", "tf_weapon_minigun", false))
	{
		CTX_GetBool(CTX_WEAPON, "autoswitchto", "tf_weapon_minigun", bSwitchTo);
		ReplyToCommand(client,"4 - Toggled AutoSwitchTo to 0: %i", bSwitchTo);
		if(CTX_SetBool(CTX_WEAPON, "autoswitchto", "tf_weapon_minigun", true))
		{
			CTX_GetBool(CTX_WEAPON, "autoswitchto", "tf_weapon_minigun", bSwitchTo);
			ReplyToCommand(client,"4 - Toggled AutoSwitchTo to 1: %i", bSwitchTo);
		}
		else
		{
				ReplyToCommand(client,"4 - Failed to update AutoSwitchTo");
		}
	}
	else
	{
			ReplyToCommand(client,"4 - Failed to update AutoSwitchTo");
	}
}


public Action:CTXFixMinigun(client, args)
{
	CTX_Reset(CTX_WEAPON, "tf_weapon_minigun");
	ReplyToCommand(client, "Consider the minigun fixed.");   
}