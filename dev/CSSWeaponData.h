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

#ifndef _INCLUDE_CTFWEAPONINFO_H
#define _INCLUDE_CTFWEAPONINFO_H

#include <utldict.h>

class FileWeaponInfo_t
{
public :
	void *m_pVTable;								// +0	- Virtual Table
	bool m_bInitialised;							// +4	- Has it been installed?
	bool m_bLoadedHudElements;						// +5	- Have hud elements been loaded?
	char m_achClassname[80];						// +6	- Class name (e.g. tf_weapon_minigun)
	char m_achPrintName[80];						// +86	- Print Name (prelocalised)
	char m_achViewModel[80];						// +166	- View Model
	char m_achPlayerModel[80];						// +246	- Player Model
	char m_achAnimationPrefix[16];					// +326	- Animation Prefix
	int m_nBucket;									// +344	- Slot (or 'bucket')
	int m_nBucketPosition;							// +348	- Position within the slot (vertical)
	int m_nPClipSize;								// +352	- Primary Clip Size
	int m_nSClipSize;								// +356	- Secondary Clip Size
	int m_nPDefaultClip;							// +360	- Primary Default Clip Size
	int m_nSDefaultClip;							// +364	- Secondary Default Clip Size
	int m_nWeight;									// +368	- Weapon Weight
	int m_nRumble;									// +372	- Weapon 'Rumble'
	bool m_bAutoSwitchTo;							// +376	- AutoSwitchTo (true/false)
	bool m_bAutoSwitchFrom;							// +377	- AutoSwitchFrom (true/false)
	int m_nItemFlags;								// +380	- Item Flags
	char m_achPAmmo[32];							// +384	- Primary Ammo Name
	char m_achSAmmo[32];							// +416	- Secondary Ammo Name
	char m_achSoundData[16][80];					// +448	- Sound Data
	int m_nPAmmoType;								// +1728	- Primary Ammo Type
	int m_nSAmmoType;								// +1732	- Secondary Ammo Type
	bool m_bMeleeWeapon;							// +1736	- Is it a Melee Weapon?
	bool m_bBuiltRightHanded;						// +1737	- Is it built right handed?
	bool m_bAllowFlipping;							// +1738	- Allow flipping (to left)?
	unsigned char m_ClientSideData[41];				// +1739	- Client Side Data (unknown)
	bool m_bShowHintUsage;							// +1780	- Show Hints?
};

class CCSWeaponInfo :								// +0
	public FileWeaponInfo_t							// +0
{
public:
    float m_fMaxPlayerSpeed;						// +1784
    int m_nWeaponType;								// +1788
    int m_nTeam;									// +1792
    float m_fUnknown;								// +1796
    float m_fWeaponArmorRatio;						// +1800
    int m_nCrosshairMinDistance;					// +1804
    int m_nCrosshariDeltaDistance;					// +1808
    bool m_bCanEquipWithShield;						// +1812
    char m_aWrongTeamMsg[32];						// +1813
    char m_aPlayerAnimationExtension[16];			// +1845
    char m_aShieldViewModel[64];					// +1861
	char m_aAddonModel[80];							// +1925
    char m_aDroppedModel[80];						// +2005
    char m_aSilencerModel[80];						// +2085
	char PADDING[3];								// +2165
    int m_nMuzzleFlashStyle;						// +2168
    float m_fMuzzleFlashScale;						// +2172
    int m_nPenetration;								// +2176
    int m_nDamage;									// +2180
    float m_fRange;									// +2184
    float m_fRangeModifier;							// +2188
    int m_nBullets;									// +2192
    float m_fCycleTime;								// +2196
    int m_nAccuracyQuadratic;						// +2200
    float m_fAccuracyDivisor;						// +2204
    float m_fAccuracyOffset;						// +2208
    float m_fMaxInaccuracy;							// +2212
    float m_fTimeToIdle;							// +2216
    float m_fIdleInterval;							// +2220
    int m_nBlackMarketPrice;						// +2224
    int m_nBlackMarketPrice2;						// +2228
    int m_nBlackMarketPreviousPrice;				// +2232
};  


#endif	// +_INCLUDE_CTFWEAPONINFO_H
