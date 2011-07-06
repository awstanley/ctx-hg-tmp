/*
 * ================================================================================
 * SourceMod Weapon Patcher Extension
 * Copyright (C) 2010 A.W. 'Swixel' Stanley.  All rights reserved.
 *
 * Special thanks to Paul 'Wazz' Clothier for exposing the ability to patch
 * precached CTX weapon data.
 * ================================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _INCLUDE_CDODPlayerClassInfo_H
#define _INCLUDE_CDODPlayerClassInfo_H

#include <utldict.h>

// Valve change this a lot ... ffs
class DOD_FilePlayerClassInfo_t
{
public :
	void *vtable;									// +0		- vtable
	bool m_bInitialised;							// +4		- Initialised
	char m_achClassname[80];						// +5		- Classname
	char DO_NOT_USE[48];							// +85		- I hate you Valve.  Learn to use what you reserve...
	char m_achPrintname[128];						// +133		- Print name
	char m_achPlayerModel[128];						// +261		- Player Model
	char m_achSelectCommand[32];					// +389		- Select Command
};

class  :
	public DOD_FilePlayerClassInfo_t				// +0
{
public :
	char DO_NOT_USE[3];								// +421		- I hate you Valve.  Learn to use what you reserve...
	int m_nTeam;									// +424		- Team (2 & 3 are playable)
	int	m_nPrimaryWeapon;							// +428		- Primary Weapon
	int	m_nSecondaryWeapon;							// +432		- Secondary Weapon
	int	m_nMeleeWeapon;								// +436		- Melee Weapon
	int	m_nGrenadeCount1;							// +440		- Grenade #1 Count
	int	m_nGrenadeType1;							// +444		- Grenade #1 Type
	int	m_nGrenadeCount2;							// +448		- Grenade #2 Count
	int	m_nGrenadeType2;							// +452		- Grenade #2 Type
	int m_nNumBandages;								// +456		- Number of Bandages
	int m_nHelmetGroup;								// +460		- Helmet Group (?)
	int m_nHairGroup;								// +464		- Hair Group (?!)
	int m_nDropHelmet;								// +468		- Can drop helmet
	int m_nClassLimit;								// +472		- Limit (number allowed)
	bool m_bMergeMG;								// +536		- Merge MG Class (?)
	char m_achHealthImage[64];						// +537		- Health Image
	char m_achHealthImageBG[64];					// +601		- Health Image Background
};


#endif	// +_INCLUDE_CDODPlayerClassInfo_H
