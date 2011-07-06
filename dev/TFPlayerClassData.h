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

#ifndef _INCLUDE_TFPLAYERCLASSDATA_H
#define _INCLUDE_TFPLAYERCLASSDATA_H

#include <utldict.h>

// Derived from the SDK and RE

// Number of actual classes (listed below)
#define TF_CLASS_COUNT	11

// Classes - as they really fall
#ifdef _SWIXELTFCLASSES

enum PlayerClassNames
{
	UNDEFINED = 0,				//  0- Undefined
	Tf_Classname_Scout,			//	1 - Scout
	Tf_Classname_Sniper,		//  2 - Sniper
	Tf_Classname_Soldier,		//  3 - Soldier
	Tf_Classname_Demoman,		//  4 - Demoman
	Tf_Classname_Medic,			//  5 - Medic
	Tf_Classname_HWGuy,			//  6 - Heavy
	Tf_Classname_Pyro,			//  7 - Pyro
	Tf_Classname_Spy,			//  8 - Spy
	Tf_Classname_Engineer,		//  9 - Engineer
	Tf_Classname_Civilian,		// 10 - Civilian / "Invalid"
};

#endif
	  
class TFPlayerClassData_t
{
public:
	char m_achClassname[128];						// +0	- Class name (e.g. tf_scout)
    char m_achModel[128];							// +128 - Player model
    char m_achModelHMW[128];						// +256 - World model
    char m_achmodelHands[128];						// +384 - Model for hands
    char m_achLocalisedName[128];					// +512 - Localised Name
    float m_fMaxSpeed;								// +640 - Speed Maximum
    int m_nMaxHealth;								// +644 - Health Maximum
    int m_nMaxArmor;								// +648 - Armour Maximum (Not used)
    int m_nPrimaryWeapon;							// +652 - Primary Weapon (Slot 0)
	int m_nSecondaryWeapon;							// +656 - Secondary Weapon (Slot 1)
	int m_nMeleeWeapon;								// +660 - Melee Weapon (Slot 3)
	int m_nGrenade;									// +664 - Grenade Slot
	int m_nBuilding;								// +668 - Build Slot
    int m_nPDA1;									// +672 - PDA #1
	int m_nPDA2;									// +676	- PDA #2
    char DO_NOT_USE[4];								// +680	- PADDING
    int m_nPrimaryAmmo;								// +684 - Primary Weapon Ammo
	int m_nSecondaryAmmo;							// +688 - Secondary Weapon Ammo
	int m_nMetal;									// +692 - Metal
	int m_nGrenade1;								// +696 - Ammo Grenade #1
	int m_nGrenade2;								// +700	- Ammo Grenade #2
    int m_nBuildable1;								// +704 - Building #1
	int m_nBuildable2;								// +708 - Building #2
	int m_nBuildable3;								// +712 - Building #3
	int m_nBuildable4;								// +716 - Building #4
	int m_nBuildable5;								// +720	- Building #5
	int m_nBuildable6;								// +724	- Building #6
    bool m_bDontDoAirWalk;							// +728	- Don't do air walk (air strafe?)
    bool m_bDontDoNewJump;							// +729	- Do not do new jump (double jump?)
    bool m_bInitialised;							// +730	- Loaded into database?
    float m_fCameraForward;							// +732	- Offset of Camera forward
    float m_fCameraRight;							// +736	- Offset of Camera right
    float m_fCameraUp;								// +740	- Offset of Camera up
    char m_achSoundDeath[128];						// +744	- Sound of a generic death
    char m_achSoundCritDeath[128];					// +872	- Sound of a crit death
    char m_achSoundMeleeDeath[128];					// +1000	- Sound of a melee death
    char m_achSoundExplosionDeath[128];				// +1128	- Sound of an exploding Death
};

#endif
