struct ObjectInfos_t
{
	char *m_achSectionName;					// +0	- .
	char *m_achClassname;						// +4	- .
	char *m_achStatusName;						// +8	- .
	float m_flBuildTime;						// +12	- .
	int m_nMaxObjects;						// +16	- .
	int m_nCost;								// +20	- .
	float m_flCostMultiplier;					// +24	- .
	int m_nUpgradeCost;						// +28	- .
	int m_nMaxUpgradeLevel;					// +32	- .
	char *m_achBuilderWeaponName;				// +36	- .
 	char *m_achBuilderPlacementString;			// +40	- .
 	int m_nSelectionSlot;						// +44	- .
 	int m_nSelectionPosition;					// +48	- .
 	bool m_bSolidToPlayerMovement;			// +52	- .
 	bool m_bUseItemInfo;						// +53	- .
 	char *m_achViewModel;						// +56	- .
 	char *m_achPlayerModel;					// +60	- .
 	int m_nDisplayPriority;					// +64	- .
 	bool m_bVisibleInWeaponSelection;			// +68	- .
 	char *m_achExplodeSound;					// +72	- .
 	char *m_achExplodeEffect;					// +76	- .
 	bool m_bAutoSwitchTo;						// +80	- .
 	char *m_achUpgradeSound;					// +84	- .
	float m_flUpgradeDuration;				// +88	- .
 	int m_nBuildCount;						// +92	- .
	int m_nAltModeCount;						// +96	- .
	char *m_achDefault_StatusName;				// +100	- .
	char *m_achDefault_ModeName;				// +104	- .
	char *m_achDefault_IconMenu;				// +108	- .
	char *m_achAltMode1_StatusName;			// +112	- .
	char *m_achAltMode1_ModeName;				// +116	- .
	char *m_achAltMode1_IconMenu;				// +120	- .
	char *m_achAltMode2_StatusName;			// +124	- .
	char *m_achAltMode2_ModeName;				// +128	- .
	char *m_achAltMode2_IconMenu;				// +132	- .
	char *m_achIconActive;						// +136	- .
	char *m_achIconInactive;					// +140	- .
	char *m_achIconMenu2;						// +144	- .
	char *m_achHudStatusIcon;					// +148	- .
	int m_nMetalToDropInGibs;					// +152	- .
};