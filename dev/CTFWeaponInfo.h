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

class TF2_FileWeaponInfo_t
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

class CTFWeaponInfo :
	public TF2_FileWeaponInfo_t						// +0
{
public :
	int m_nPDamage;									// +1784	- Primary Projectile Damage
	int m_nPBulletsPerShot;							// +1788	- Primary Projectile Bullets Used Per Shot (display only?)
	float m_fPRange;								// +1792	- Primary Projectile Range
	float m_fPSpread;								// +1796	- Primary Projectile Spread
	float m_fPPunchAngle;							// +1800	- Primary Projectile Punch Angle (used for some projectiles)
	float m_fPTimeFireDelay;						// +1804	- Primary Projectile Fire Delay
	float m_fPTimeIdle;								// +1808	- Primary Projectile Time Idle
	float m_fPTimeIdleEmpty;						// +1812	- Primary Projectile Time to Idle PreReload
	float m_fPTimeReloadStart;						// +1816	- Primary Projectile Reload Start Time
	float m_fPTimeReload;							// +1820	- Primary Projectile Time to Reload
	bool m_bPDrawCrosshair;							// +1824	- Primary Projectile Draw Crosshair?
	int m_nPProjectileType;								// +1828	- Primary Projectile Type
	int m_nPAmmoUsed;								// +1832	- Primary Projectile Ammo Per Shot
	float m_fPProjSpeed;							// +1836	- Primary Projectile Speed
	float m_fPSmackDelay;							// +1840	- Primary Projectile Smack Delay
	bool m_bPUseRapidFireCrits;						// +1844	- Primary Projectile Should use Rapid Fire Crits (e.g. Minigun)
	int m_nSDamage;									// +1848	- Secondary Projectile Damage
	int m_nSAmmoPerShot;							// +1852	- Secondary Projectile Ammo Used Per Shot
	float m_fSRange;								// +1856	- Secondary Projectile Range
	float m_fSSpread;								// +1860	- Secondary Projectile Spread
	float m_fSPunchAngle;							// +1864	- Secondary Projectile Punch Angle
	float m_fSTimeFireDelay;						// +1868	- Secondary Projectile Delay PreFire
	float m_fSTimeIdle;								// +1872	- Secondary Projectile Time Idle
	float m_fSTimeIdleEmpty;						// +1876	- Secondary Projectile Time Empty PreReload
	float m_fSTimeReloadStart;						// +1880	- Secondary Projectile Reload Start Time
	float m_fSTimeReload;							// +1884	- Secondary Projectile Reload Time
	bool m_bSDrawCrosshair;							// +1888	- Secondary Projectile Draw Crosshair (does nothing?!)
	int m_nSProjectileType;							// +1892	- Secondary Projectile Type
	int m_nSAmmoUsed;								// +1896	- Secondary Projectile Ammo Per Shot
	float m_fSPSpeed;								// +1900	- Secondary Projectile Projectile Speed
	float m_fSSmackDelay;							// +1904	- Secondary Projectile Smack Delay
	bool m_bSRFCrits;								// +1908	- Secondary Projectile SHould Use RapidFIre Crits
	int m_nWeaponType;								// +1912	- Weapon Type
	bool m_bGrenade;								// +1916	- Is it a grenade?
	float m_fDamageRadius;							// +1920	- Damage Radius
	float m_fPrimerTimer;							// +1924	- Primer Timer
	bool m_bLowerMainWeapon;						// +1928	- Lower Main Weapon
	bool m_bPlayGrenTimer;							// +1929	- Play Grenade Timer
	bool m_bHasTeamSkins;							// +1930	- Uses team skins
	bool m_bHasTeamSkinsWorld;						// +1931	- Has team skins on the world model?
	char m_achMuzzleFlash[128];						// +1932	- Muzzle Flash model
	float m_fMuzzleFlashTime;						// +2060	- Muzzle Flash Duration
	char m_achMuzzleFlashFX[128];					// +2064	- Muzzle Flash Particle Effects
	char m_achTracerEffect[128];					// +2192	- Tracer Effect
	bool m_bDoInstantEjectBrass;					// +2320	- Do Instant Eject of Brass
	char m_achBrassModel[128];						// +2321	- Brass Model (ejected shell casing)
	char m_achExplosionSound[128];					// +2449	- Explosion Sound
	char m_achExplosionEffect[128];					// +2577	- Explosion Effect
	char m_achExplosionPlayerFX[128];				// +2705	- Effect for blowing up players
	char m_achExplosionWaterFX[128];				// +2833	- Effect for hitting water
	bool m_bDontDrop;								// +2961	- Should not be allowed to drop? (i.e. !Droppable)
};

#endif	// +_INCLUDE_CTFWEAPONINFO_H
