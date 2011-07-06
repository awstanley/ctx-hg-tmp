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

// Do not trust Valve.
class DODS_FileWeaponInfo_t
{
public :
	void *vtable;									// +0		- yay \O/
	bool m_bInitialised;							// +4		- Have we initialised this?
	char DO_NOT_USE[1];								// +5		- I hate you Valve.
	char m_achClassname[80];						// +6		- Classname
	char m_achPrintname[80];						// +86		- Print name
	char m_achViewModel[80];						// +166		- View Model
	char m_achPlayerModel[80];						// +246		- Player Model
	char m_achAnimPrefix[80];						// +326		- Animation Prefix
	int m_nBucket;									// +344		- Bucket
	int m_nBucketPosition;							// +348		- Bucket Position
	int m_nClipSize1;								// +352		- Clip Size #1
	int m_nClipSize2;								// +356		- Clip Size #2
	int m_nDefaultClip1;							// +360		- Default Clip #1
	int m_nDefaultClip2;							// +364		- Default Clip #2
	int m_nWeight;									// +368		- Weight
	int m_nRumble;									// +372		- Rumble
	bool m_bAutoSwitchTo;							// +376		- Auto Switch To Weapon
	bool m_bAutoSwitchFrom;							// +377		- Auto Switch From Weapon
	int m_nItemFlags;								// +380		- Item Flags
	char m_achPrimaryAmmoType[32];					// +384		- Primary Ammo Type
	char m_achSecondaryAmmoType[32];				// +416		- Secondary Ammo Type
	char m_achSoundData[16];						// +448		- Sound Data
	int m_nDefaultPrimaryAmmo;						// +1732	- Default Primary Ammo
	int m_nDefaultSecondaryAmmo;					// +1732	- Default Secondary Ammo
	bool m_bMeleeWeapon;							// +1736	- Is it a Melee Weapon?
	bool m_bBuildRightHanded;						// +1737	- Is Right Handed
	bool m_bAllowFlipping;							// +1738	- Allow Flipping
	char DO_NOT_USE[41];							// +1739	- I hate you Valve.
	bool m_bShowUsageHint;							// +1780	- Show Usage Hint
};

class CDODWeaponInfo :
	public DODS_FileWeaponInfo_t					// +0
{
public :
	int m_nPrimaryDamage;							// +1784	- Primary Projectile Damage
	float m_fPenetration;							// +1788	- .
	bool m_bBulletsPerShot;							// +1792	- .
	float m_fMuzzleFlashType;						// +1796	- .
	float m_fMuzzleFlashScale;						// +1800	- .
	bool m_bCanDrop;								// +1804	- .
	float m_fRecoil;								// +1808	- .
	char DO_NOT_USE[8];								// +1812	- Why me?
	float m_fAccuracy;								// +1820	- .
	float m_fUnknown1;								// +1824	- Secondary related, unknown.  Probably related to accuracy.
	float m_fAccuracyMovePenalty;					// +1828	- .
	float m_fFireDelay;								// +1832	- .
	float m_fUnkown2;								// +1836	- Likely to be delay related, something to do with 'SECONDARY'
	int m_nCrosshairMinDistance;					// +1840	- .
	int m_nCrosshairDeltaDistance;					// +1844	- .
	int m_nWeaponType;								// +1848	- .
													// 1 - Melee
													// 2 - Grenade
													// 8 Pistol
													// 16 Rifle
													// 32 sniper
													// 64 SMG
													// 128 SMGAmmo
													// 256 Bazooka
													// 512 Bandage
													// 1024 Sidearm
													// 2048 RifleGrenade
													// 4096 Bomb
	float m_fBotAudibleRange;						// +1852	- .
	char m_achReloadModel[80];						// +1856	- .
	char m_achDeployedModel[80];					// +1936	- .
	char m_achProneModel[80];						// +2016	- .
	char m_achIronSightModel[80];					// +2096	- .
	float m_fIdleTimeAfterFire;						// +2176	- .
	float m_fIdleInterval;							// +2180	- .
	int m_nDefaultAmmoClips;						// +2184	- .
	int m_nAmmoPickupClips;							// +2188	- .
	int m_nHudClipHeight;							// +2192	- .
	int m_nHudClipBaseHeight;						// +2196	- .
	int m_nHudClipBulletHeight;						// +2200	- .
	int m_nTracer;									// +2204	- .
	float m_fViewModelFOV;							// +2208	- .
	int m_nAltFireAbility;							// +2212	- .
	float m_fNormalOffset_1;						// +2216	- .
	float m_fNormalOffset_2;						// +2220	- .
	float m_fNormalOffset_3;						// +2224	- .
	float m_fProneOffset_1;							// +2228	- .
	float m_fProneOffset_2;							// +2232	- .
	float m_fProneOffset_3;							// +2236	- .
	float m_fIronSightOffset_1;						// +2240	- .
	float m_fIronSightOffset_2;						// +2244	- .
	float m_fIronSightOffset_3;						// +2248	- .
	int m_nDefaultTeam;								// +2252	- .

};

#endif	// +_INCLUDE_CDODPlayerClassInfo_H
