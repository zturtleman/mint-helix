/*
===========================================================================
Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company.

This file is part of Spearmint Source Code.

Spearmint Source Code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 3 of the License,
or (at your option) any later version.

Spearmint Source Code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Spearmint Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, Spearmint Source Code is also subject to certain additional terms.
You should have received a copy of these additional terms immediately following
the terms and conditions of the GNU General Public License.  If not, please
request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional
terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc.,
Suite 120, Rockville, Maryland 20850 USA.
===========================================================================
*/
//
// bg_misc.c -- both games misc functions, all completely stateless

#include "../qcommon/q_shared.h"
#include "bg_public.h"

/*QUAKED item_***** ( 0 0 0 ) (-16 -16 -16) (16 16 16) suspended
DO NOT USE THIS CLASS, IT JUST HOLDS GENERAL INFORMATION.
The suspended flag will allow items to hang in the air, otherwise they are dropped to the next surface.

If an item is the target of another entity, it will not spawn in until fired.

An item fires all of its targets when it is picked up.  If the toucher can't carry it, the targets won't be fired.

"notfree" if set to 1, don't spawn in free for all games
"notteam" if set to 1, don't spawn in team games
"notsingle" if set to 1, don't spawn in single player games
"wait"	override the default wait before respawning.  -1 = never respawn automatically, which can be used with targeted spawning.
"random" random number of plus or minus seconds varied from the respawn time
"count" override quantity or duration on most items.
*/

gitem_t	bg_itemlist[] = 
{
	{
		NULL,
		NULL,
		{ NULL,
		NULL,
		NULL, NULL} ,
/* icon */		NULL,
/* pickup */	NULL,
		0,
		0,
		0,
/* sounds */ ""
	},	// leave index 0 alone

	//
	// ARMOR
	//

/*QUAKED item_armor_shard (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"item_armor_shard", 
		"sound/misc/ar1_pkup.wav",
		{ "models/powerups/armor/shard.md3", 
		"models/powerups/armor/shard_sphere.md3",
		NULL, NULL} ,
/* icon */		"icons/iconr_shard",
/* pickup */	"Armor Shard",
		5,
		IT_ARMOR,
		0,
/* sounds */ ""
	},

/*QUAKED item_armor_combat (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"item_armor_combat", 
		"sound/misc/ar2_pkup.wav",
        { "models/powerups/armor/armor_yel.md3",
		NULL, NULL, NULL},
/* icon */		"icons/iconr_yellow",
/* pickup */	"Armor",
		50,
		IT_ARMOR,
		0,
/* sounds */ ""
	},

/*QUAKED item_armor_body (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"item_armor_body", 
		"sound/misc/ar2_pkup.wav",
        { "models/powerups/armor/armor_red.md3",
		NULL, NULL, NULL},
/* icon */		"icons/iconr_red",
/* pickup */	"Heavy Armor",
		100,
		IT_ARMOR,
		0,
/* sounds */ ""
	},

	//
	// health
	//
/*QUAKED item_health_small (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"item_health_small",
		"sound/items/s_health.wav",
        { "models/powerups/health/small_cross.md3", 
		"models/powerups/health/small_sphere.md3", 
		NULL, NULL },
/* icon */		"icons/iconh_green",
/* pickup */	"5 Health",
		5,
		IT_HEALTH,
		0,
/* sounds */ ""
	},

/*QUAKED item_health (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"item_health",
		"sound/items/n_health.wav",
        { "models/powerups/health/medium_cross.md3", 
		"models/powerups/health/medium_sphere.md3", 
		NULL, NULL },
/* icon */		"icons/iconh_yellow",
/* pickup */	"25 Health",
		25,
		IT_HEALTH,
		0,
/* sounds */ ""
	},

/*QUAKED item_health_large (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"item_health_large",
		"sound/items/l_health.wav",
        { "models/powerups/health/large_cross.md3", 
		"models/powerups/health/large_sphere.md3", 
		NULL, NULL },
/* icon */		"icons/iconh_red",
/* pickup */	"50 Health",
		50,
		IT_HEALTH,
		0,
/* sounds */ ""
	},

/*QUAKED item_health_mega (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"item_health_mega",
		"sound/items/m_health.wav",
        { "models/powerups/health/mega_cross.md3", 
		"models/powerups/health/mega_sphere.md3", 
		NULL, NULL },
/* icon */		"icons/iconh_mega",
/* pickup */	"Mega Health",
		100,
		IT_HEALTH,
		0,
/* sounds */ ""
	},


	//
	// WEAPONS 
	//

/*QUAKED weapon_gauntlet (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_gauntlet", 
		"sound/misc/w_pkup.wav",
        { "models/weapons2/gauntlet/gauntlet.md3",
		NULL, NULL, NULL},
/* icon */		"icons/iconw_gauntlet",
/* pickup */	"Gauntlet",
		0,
		IT_WEAPON,
		WP_GAUNTLET,
/* sounds */ ""
	},

/*QUAKED weapon_shotgun (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_shotgun", 
		"sound/misc/w_pkup.wav",
        { "models/weapons2/shotgun/shotgun.md3", 
		NULL, NULL, NULL},
/* icon */		"icons/iconw_shotgun",
/* pickup */	"Shotgun",
		10,
		IT_WEAPON,
		WP_SHOTGUN,
/* sounds */ ""
	},

/*QUAKED weapon_machinegun (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_machinegun", 
		"sound/misc/w_pkup.wav",
        { "models/weapons2/machinegun/machinegun.md3", 
		NULL, NULL, NULL},
/* icon */		"icons/iconw_machinegun",
/* pickup */	"Machinegun",
		40,
		IT_WEAPON,
		WP_MACHINEGUN,
/* sounds */ ""
	},

/*QUAKED weapon_grenadelauncher (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_grenadelauncher",
		"sound/misc/w_pkup.wav",
        { "models/weapons2/grenadel/grenadel.md3", 
		NULL, NULL, NULL},
/* icon */		"icons/iconw_grenade",
/* pickup */	"Grenade Launcher",
		10,
		IT_WEAPON,
		WP_GRENADE_LAUNCHER,
/* sounds */ "sound/weapons/grenade/hgrenb1a.wav sound/weapons/grenade/hgrenb2a.wav"
	},

/*QUAKED weapon_rocketlauncher (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_rocketlauncher",
		"sound/misc/w_pkup.wav",
        { "models/weapons2/rocketl/rocketl.md3", 
		NULL, NULL, NULL},
/* icon */		"icons/iconw_rocket",
/* pickup */	"Rocket Launcher",
		10,
		IT_WEAPON,
		WP_ROCKET_LAUNCHER,
/* sounds */ ""
	},

/*QUAKED weapon_lightning (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_lightning", 
		"sound/misc/w_pkup.wav",
        { "models/weapons2/lightning/lightning.md3", 
		NULL, NULL, NULL},
/* icon */		"icons/iconw_lightning",
/* pickup */	"Lightning Gun",
		100,
		IT_WEAPON,
		WP_LIGHTNING,
/* sounds */ ""
	},

/*QUAKED weapon_railgun (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_railgun", 
		"sound/misc/w_pkup.wav",
        { "models/weapons2/railgun/railgun.md3", 
		NULL, NULL, NULL},
/* icon */		"icons/iconw_railgun",
/* pickup */	"Railgun",
		10,
		IT_WEAPON,
		WP_RAILGUN,
/* sounds */ ""
	},

/*QUAKED weapon_plasmagun (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_plasmagun", 
		"sound/misc/w_pkup.wav",
        { "models/weapons2/plasma/plasma.md3", 
		NULL, NULL, NULL},
/* icon */		"icons/iconw_plasma",
/* pickup */	"Plasma Gun",
		50,
		IT_WEAPON,
		WP_PLASMAGUN,
/* sounds */ ""
	},

/*QUAKED weapon_bfg (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_bfg",
		"sound/misc/w_pkup.wav",
        { "models/weapons2/bfg/bfg.md3", 
		NULL, NULL, NULL},
/* icon */		"icons/iconw_bfg",
/* pickup */	"BFG10K",
		20,
		IT_WEAPON,
		WP_BFG,
/* sounds */ ""
	},

/*QUAKED weapon_grapplinghook (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_grapplinghook",
		"sound/misc/w_pkup.wav",
        { "models/weapons2/grapple/grapple.md3", 
		NULL, NULL, NULL},
/* icon */		"icons/iconw_grapple",
/* pickup */	"Grappling Hook",
		0,
		IT_WEAPON,
		WP_GRAPPLING_HOOK,
/* sounds */ ""
	},

	//
	// AMMO ITEMS
	//

/*QUAKED ammo_shells (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"ammo_shells",
		"sound/misc/am_pkup.wav",
        { "models/powerups/ammo/shotgunam.md3", 
		NULL, NULL, NULL},
/* icon */		"icons/icona_shotgun",
/* pickup */	"Shells",
		10,
		IT_AMMO,
		WP_SHOTGUN,
/* sounds */ ""
	},

/*QUAKED ammo_bullets (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"ammo_bullets",
		"sound/misc/am_pkup.wav",
        { "models/powerups/ammo/machinegunam.md3", 
		NULL, NULL, NULL},
/* icon */		"icons/icona_machinegun",
/* pickup */	"Bullets",
		50,
		IT_AMMO,
		WP_MACHINEGUN,
/* sounds */ ""
	},

/*QUAKED ammo_grenades (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"ammo_grenades",
		"sound/misc/am_pkup.wav",
        { "models/powerups/ammo/grenadeam.md3", 
		NULL, NULL, NULL},
/* icon */		"icons/icona_grenade",
/* pickup */	"Grenades",
		5,
		IT_AMMO,
		WP_GRENADE_LAUNCHER,
/* sounds */ ""
	},

/*QUAKED ammo_cells (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"ammo_cells",
		"sound/misc/am_pkup.wav",
        { "models/powerups/ammo/plasmaam.md3", 
		NULL, NULL, NULL},
/* icon */		"icons/icona_plasma",
/* pickup */	"Cells",
		30,
		IT_AMMO,
		WP_PLASMAGUN,
/* sounds */ ""
	},

/*QUAKED ammo_lightning (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"ammo_lightning",
		"sound/misc/am_pkup.wav",
        { "models/powerups/ammo/lightningam.md3", 
		NULL, NULL, NULL},
/* icon */		"icons/icona_lightning",
/* pickup */	"Lightning",
		60,
		IT_AMMO,
		WP_LIGHTNING,
/* sounds */ ""
	},

/*QUAKED ammo_rockets (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"ammo_rockets",
		"sound/misc/am_pkup.wav",
        { "models/powerups/ammo/rocketam.md3", 
		NULL, NULL, NULL},
/* icon */		"icons/icona_rocket",
/* pickup */	"Rockets",
		5,
		IT_AMMO,
		WP_ROCKET_LAUNCHER,
/* sounds */ ""
	},

/*QUAKED ammo_slugs (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"ammo_slugs",
		"sound/misc/am_pkup.wav",
        { "models/powerups/ammo/railgunam.md3", 
		NULL, NULL, NULL},
/* icon */		"icons/icona_railgun",
/* pickup */	"Slugs",
		10,
		IT_AMMO,
		WP_RAILGUN,
/* sounds */ ""
	},

/*QUAKED ammo_bfg (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"ammo_bfg",
		"sound/misc/am_pkup.wav",
        { "models/powerups/ammo/bfgam.md3", 
		NULL, NULL, NULL},
/* icon */		"icons/icona_bfg",
/* pickup */	"Bfg Ammo",
		15,
		IT_AMMO,
		WP_BFG,
/* sounds */ ""
	},

	//
	// HOLDABLE ITEMS
	//
/*QUAKED holdable_teleporter (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"holdable_teleporter", 
		"sound/items/holdable.wav",
        { "models/powerups/holdable/teleporter.md3", 
		NULL, NULL, NULL},
/* icon */		"icons/teleporter",
/* pickup */	"Personal Teleporter",
		60,
		IT_HOLDABLE,
		HI_TELEPORTER,
/* sounds */ ""
	},
/*QUAKED holdable_medkit (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"holdable_medkit", 
		"sound/items/holdable.wav",
        { 
		"models/powerups/holdable/medkit.md3", 
		"models/powerups/holdable/medkit_sphere.md3",
		NULL, NULL},
/* icon */		"icons/medkit",
/* pickup */	"Medkit",
		60,
		IT_HOLDABLE,
		HI_MEDKIT,
/* sounds */ "sound/items/use_medkit.wav"
	},

	//
	// POWERUP ITEMS
	//
/*QUAKED item_quad (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"item_quad", 
		"sound/items/quaddamage.wav",
        { "models/powerups/instant/quad.md3", 
        "models/powerups/instant/quad_ring.md3",
		NULL, NULL },
/* icon */		"icons/quad",
/* pickup */	"Quad Damage",
		30,
		IT_POWERUP,
		PW_QUAD,
/* sounds */ "sound/items/damage2.wav sound/items/damage3.wav"
	},

/*QUAKED item_enviro (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"item_enviro",
		"sound/items/protect.wav",
        { "models/powerups/instant/enviro.md3", 
		"models/powerups/instant/enviro_ring.md3", 
		NULL, NULL },
/* icon */		"icons/envirosuit",
/* pickup */	"Battle Suit",
		30,
		IT_POWERUP,
		PW_BATTLESUIT,
/* sounds */ "sound/items/airout.wav sound/items/protect3.wav"
	},

/*QUAKED item_haste (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"item_haste",
		"sound/items/haste.wav",
        { "models/powerups/instant/haste.md3", 
		"models/powerups/instant/haste_ring.md3", 
		NULL, NULL },
/* icon */		"icons/haste",
/* pickup */	"Speed",
		30,
		IT_POWERUP,
		PW_HASTE,
/* sounds */ ""
	},

/*QUAKED item_invis (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"item_invis",
		"sound/items/invisibility.wav",
        { "models/powerups/instant/invis.md3", 
		"models/powerups/instant/invis_ring.md3", 
		NULL, NULL },
/* icon */		"icons/invis",
/* pickup */	"Invisibility",
		30,
		IT_POWERUP,
		PW_INVIS,
/* sounds */ ""
	},

/*QUAKED item_regen (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"item_regen",
		"sound/items/regeneration.wav",
        { "models/powerups/instant/regen.md3", 
		"models/powerups/instant/regen_ring.md3", 
		NULL, NULL },
/* icon */		"icons/regen",
/* pickup */	"Regeneration",
		30,
		IT_POWERUP,
		PW_REGEN,
/* sounds */ "sound/items/regen.wav"
	},

/*QUAKED item_flight (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"item_flight",
		"sound/items/flight.wav",
        { "models/powerups/instant/flight.md3", 
		"models/powerups/instant/flight_ring.md3", 
		NULL, NULL },
/* icon */		"icons/flight",
/* pickup */	"Flight",
		60,
		IT_POWERUP,
		PW_FLIGHT,
/* sounds */ "sound/items/flight.wav"
	},

/*QUAKED team_CTF_redflag (1 0 0) (-16 -16 -16) (16 16 16)
Only in CTF games
*/
	{
		"team_CTF_redflag",
		NULL,
        { "models/flags/r_flag.md3",
		NULL, NULL, NULL },
/* icon */		"icons/iconf_red1",
/* pickup */	"Red Flag",
		0,
		IT_TEAM,
		PW_REDFLAG,
/* sounds */ ""
	},

/*QUAKED team_CTF_blueflag (0 0 1) (-16 -16 -16) (16 16 16)
Only in CTF games
*/
	{
		"team_CTF_blueflag",
		NULL,
        { "models/flags/b_flag.md3",
		NULL, NULL, NULL },
/* icon */		"icons/iconf_blu1",
/* pickup */	"Blue Flag",
		0,
		IT_TEAM,
		PW_BLUEFLAG,
/* sounds */ ""
	},

#ifdef MISSIONPACK
/*QUAKED holdable_kamikaze (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"holdable_kamikaze", 
		"sound/items/holdable.wav",
        { "models/powerups/kamikazi.md3", 
		NULL, NULL, NULL},
/* icon */		"icons/kamikaze",
/* pickup */	"Kamikaze",
		60,
		IT_HOLDABLE,
		HI_KAMIKAZE,
/* sounds */ "sound/items/kamikazerespawn.wav"
	},

/*QUAKED holdable_portal (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"holdable_portal", 
		"sound/items/holdable.wav",
        { "models/powerups/holdable/porter.md3",
		NULL, NULL, NULL},
/* icon */		"icons/portal",
/* pickup */	"Portal",
		60,
		IT_HOLDABLE,
		HI_PORTAL,
/* sounds */ ""
	},

/*QUAKED holdable_invulnerability (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"holdable_invulnerability", 
		"sound/items/holdable.wav",
        { "models/powerups/holdable/invulnerability.md3", 
		NULL, NULL, NULL},
/* icon */		"icons/invulnerability",
/* pickup */	"Invulnerability",
		60,
		IT_HOLDABLE,
		HI_INVULNERABILITY,
/* sounds */ ""
	},

/*QUAKED ammo_nails (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"ammo_nails",
		"sound/misc/am_pkup.wav",
        { "models/powerups/ammo/nailgunam.md3", 
		NULL, NULL, NULL},
/* icon */		"icons/icona_nailgun",
/* pickup */	"Nails",
		20,
		IT_AMMO,
		WP_NAILGUN,
/* sounds */ ""
	},

/*QUAKED ammo_mines (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"ammo_mines",
		"sound/misc/am_pkup.wav",
        { "models/powerups/ammo/proxmineam.md3", 
		NULL, NULL, NULL},
/* icon */		"icons/icona_proxlauncher",
/* pickup */	"Proximity Mines",
		10,
		IT_AMMO,
		WP_PROX_LAUNCHER,
/* sounds */ ""
	},

/*QUAKED ammo_belt (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"ammo_belt",
		"sound/misc/am_pkup.wav",
        { "models/powerups/ammo/chaingunam.md3", 
		NULL, NULL, NULL},
/* icon */		"icons/icona_chaingun",
/* pickup */	"Chaingun Belt",
		100,
		IT_AMMO,
		WP_CHAINGUN,
/* sounds */ ""
	},

	//
	// PERSISTANT POWERUP ITEMS
	//
/*QUAKED item_scout (.3 .3 1) (-16 -16 -16) (16 16 16) suspended redTeam blueTeam
*/
	{
		"item_scout",
		"sound/items/scout.wav",
        { "models/powerups/scout.md3", 
		NULL, NULL, NULL },
/* icon */		"icons/scout",
/* pickup */	"Scout",
		30,
		IT_PERSISTANT_POWERUP,
		PW_SCOUT,
/* sounds */ ""
	},

/*QUAKED item_guard (.3 .3 1) (-16 -16 -16) (16 16 16) suspended redTeam blueTeam
*/
	{
		"item_guard",
		"sound/items/guard.wav",
        { "models/powerups/guard.md3", 
		NULL, NULL, NULL },
/* icon */		"icons/guard",
/* pickup */	"Guard",
		30,
		IT_PERSISTANT_POWERUP,
		PW_GUARD,
/* sounds */ ""
	},

/*QUAKED item_doubler (.3 .3 1) (-16 -16 -16) (16 16 16) suspended redTeam blueTeam
*/
	{
		"item_doubler",
		"sound/items/doubler.wav",
        { "models/powerups/doubler.md3", 
		NULL, NULL, NULL },
/* icon */		"icons/doubler",
/* pickup */	"Doubler",
		30,
		IT_PERSISTANT_POWERUP,
		PW_DOUBLER,
/* sounds */ ""
	},

/*QUAKED item_doubler (.3 .3 1) (-16 -16 -16) (16 16 16) suspended redTeam blueTeam
*/
	{
		"item_ammoregen",
		"sound/items/ammoregen.wav",
        { "models/powerups/ammo.md3",
		NULL, NULL, NULL },
/* icon */		"icons/ammo_regen",
/* pickup */	"Ammo Regen",
		30,
		IT_PERSISTANT_POWERUP,
		PW_AMMOREGEN,
/* sounds */ ""
	},

	/*QUAKED team_CTF_neutralflag (0 0 1) (-16 -16 -16) (16 16 16)
Only in One Flag CTF games
*/
	{
		"team_CTF_neutralflag",
		NULL,
        { "models/flags/n_flag.md3",
		NULL, NULL, NULL },
/* icon */		"icons/iconf_neutral1",
/* pickup */	"Neutral Flag",
		0,
		IT_TEAM,
		PW_NEUTRALFLAG,
/* sounds */ ""
	},

	{
		"item_redcube",
		"sound/misc/am_pkup.wav",
        { "models/powerups/orb/r_orb.md3",
		NULL, NULL, NULL },
/* icon */		"icons/iconh_rorb",
/* pickup */	"Red Cube",
		0,
		IT_TEAM,
		0,
/* sounds */ ""
	},

	{
		"item_bluecube",
		"sound/misc/am_pkup.wav",
        { "models/powerups/orb/b_orb.md3",
		NULL, NULL, NULL },
/* icon */		"icons/iconh_borb",
/* pickup */	"Blue Cube",
		0,
		IT_TEAM,
		0,
/* sounds */ ""
	},
/*QUAKED weapon_nailgun (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_nailgun", 
		"sound/misc/w_pkup.wav",
        { "models/weapons/nailgun/nailgun.md3", 
		NULL, NULL, NULL},
/* icon */		"icons/iconw_nailgun",
/* pickup */	"Nailgun",
		10,
		IT_WEAPON,
		WP_NAILGUN,
/* sounds */ ""
	},

/*QUAKED weapon_prox_launcher (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_prox_launcher", 
		"sound/misc/w_pkup.wav",
        { "models/weapons/proxmine/proxmine.md3", 
		NULL, NULL, NULL},
/* icon */		"icons/iconw_proxlauncher",
/* pickup */	"Prox Launcher",
		5,
		IT_WEAPON,
		WP_PROX_LAUNCHER,
/* sounds */ "sound/weapons/proxmine/wstbtick.wav "
			"sound/weapons/proxmine/wstbactv.wav "
			"sound/weapons/proxmine/wstbimpl.wav "
			"sound/weapons/proxmine/wstbimpm.wav "
			"sound/weapons/proxmine/wstbimpd.wav "
			"sound/weapons/proxmine/wstbactv.wav"
	},

/*QUAKED weapon_chaingun (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
*/
	{
		"weapon_chaingun", 
		"sound/misc/w_pkup.wav",
        { "models/weapons/vulcan/vulcan.md3", 
		NULL, NULL, NULL},
/* icon */		"icons/iconw_chaingun",
/* pickup */	"Chaingun",
		80,
		IT_WEAPON,
		WP_CHAINGUN,
/* sounds */ "sound/weapons/vulcan/wvulwind.wav"
	},
#endif

	// end of list marker
	{NULL}
};

int		bg_numItems = ARRAY_LEN( bg_itemlist ) - 1;

// entityState_t fields
#define	NETF(x) (size_t)&((entityState_t*)0)->x, 1
#define	NETA(x) (size_t)&((entityState_t*)0)->x, ARRAY_LEN( ((entityState_t*)0)->x )

vmNetField_t	bg_entityStateFields[] = 
{
{ NETF(pos.trTime), 32 },
{ NETF(pos.trBase[0]), 0 },
{ NETF(pos.trBase[1]), 0 },
{ NETF(pos.trDelta[0]), 0 },
{ NETF(pos.trDelta[1]), 0 },
{ NETF(pos.trBase[2]), 0 },
{ NETF(apos.trBase[1]), 0 },
{ NETF(pos.trDelta[2]), 0 },
{ NETF(apos.trBase[0]), 0 },
{ NETF(event), 10 },
{ NETF(angles2[1]), 0 },
{ NETF(eType), 8 },
{ NETF(torsoAnim), 8 },
{ NETF(eventParm), 8 },
{ NETF(legsAnim), 8 },
{ NETF(groundEntityNum), GENTITYNUM_BITS },
{ NETF(pos.trType), 8 },
{ NETF(eFlags), 32 },
{ NETF(otherEntityNum), GENTITYNUM_BITS },
{ NETF(weapon), MAX( 8, WEAPONNUM_BITS ) }, // because 'weapon' is used for things besides weaponnum it must be minimum of 8 bits
{ NETF(playerNum), 8 },
{ NETF(angles[1]), 0 },
{ NETF(pos.trDuration), 32 },
{ NETF(apos.trType), 8 },
{ NETF(origin[0]), 0 },
{ NETF(origin[1]), 0 },
{ NETF(origin[2]), 0 },
{ NETF(contents), 32 },
{ NETF(collisionType), 16 },
{ NETF(mins[0]), 0 },
{ NETF(mins[1]), 0 },
{ NETF(mins[2]), 0 },
{ NETF(maxs[0]), 0 },
{ NETF(maxs[1]), 0 },
{ NETF(maxs[2]), 0 },
{ NETF(powerups), MAX_POWERUPS },
{ NETF(modelindex), MODELINDEX_BITS },
{ NETF(otherEntityNum2), GENTITYNUM_BITS },
{ NETF(loopSound), 8 },
{ NETF(tokens), 8 },
{ NETF(team), 8 },
{ NETF(origin2[2]), 0 },
{ NETF(origin2[0]), 0 },
{ NETF(origin2[1]), 0 },
{ NETF(modelindex2), MODELINDEX_BITS },
{ NETF(angles[0]), 0 },
{ NETF(time), 32 },
{ NETF(apos.trTime), 32 },
{ NETF(apos.trDuration), 32 },
{ NETF(apos.trBase[2]), 0 },
{ NETF(apos.trDelta[0]), 0 },
{ NETF(apos.trDelta[1]), 0 },
{ NETF(apos.trDelta[2]), 0 },
{ NETF(time2), 32 },
{ NETF(angles[2]), 0 },
{ NETF(angles2[0]), 0 },
{ NETF(angles2[2]), 0 },
{ NETF(constantLight), 32 },
{ NETF(dl_intensity), 32 },
{ NETF(density), 10},
{ NETF(frame), 16 }
};

int bg_numEntityStateFields = ARRAY_LEN(bg_entityStateFields);

// playerState_t fields
#define	PSF(x) (size_t)&((playerState_t*)0)->x, 1
#define	PSA(x) (size_t)&((playerState_t*)0)->x, ARRAY_LEN( ((playerState_t*)0)->x )

vmNetField_t	bg_playerStateFields[] = 
{
{ PSF(commandTime), 32 },				
{ PSF(origin[0]), 0 },
{ PSF(origin[1]), 0 },
{ PSF(bobCycle), 8 },
{ PSF(velocity[0]), 0 },
{ PSF(velocity[1]), 0 },
{ PSF(viewangles[1]), 0 },
{ PSF(viewangles[0]), 0 },
{ PSF(weaponTime), -16 },
{ PSF(origin[2]), 0 },
{ PSF(velocity[2]), 0 },
{ PSF(legsTimer), 8 },
{ PSF(pm_time), -16 },
{ PSF(eventSequence), 16 },
{ PSF(torsoAnim), 8 },
{ PSF(movementDir), 4 },
{ PSF(events[0]), 8 },
{ PSF(legsAnim), 8 },
{ PSF(events[1]), 8 },
{ PSF(pm_flags), 16 },
{ PSF(groundEntityNum), GENTITYNUM_BITS },
{ PSF(weaponstate), 4 },
{ PSF(eFlags), 32 },
{ PSA(stats), -16 },
{ PSA(persistant), -16 },
{ PSA(ammo), -16 },
{ PSA(powerups), 32 },
{ PSF(contents), 32 },
{ PSF(collisionType), 16 },
{ PSF(linked), 1 },
{ PSF(externalEvent), 10 },
{ PSF(gravity), 16 },
{ PSF(speed), 16 },
{ PSF(delta_angles[1]), 16 },
{ PSF(externalEventParm), 8 },
{ PSF(viewheight), -8 },
{ PSF(damageEvent), 8 },
{ PSF(damageYaw), 8 },
{ PSF(damagePitch), 8 },
{ PSF(damageCount), 8 },
{ PSF(tokens), 8 },
{ PSF(pm_type), 8 },					
{ PSF(delta_angles[0]), 16 },
{ PSF(delta_angles[2]), 16 },
{ PSF(torsoTimer), 12 },
{ PSF(eventParms[0]), 8 },
{ PSF(eventParms[1]), 8 },
{ PSF(playerNum), 8 },
{ PSF(weapon), WEAPONNUM_BITS },
{ PSF(viewangles[2]), 0 },
{ PSF(grapplePoint[0]), 0 },
{ PSF(grapplePoint[1]), 0 },
{ PSF(grapplePoint[2]), 0 },
{ PSF(jumppad_ent), GENTITYNUM_BITS },
{ PSF(loopSound), 16 },
{ PSF(mins[0]), 0 },
{ PSF(mins[1]), 0 },
{ PSF(mins[2]), 0 },
{ PSF(maxs[0]), 0 },
{ PSF(maxs[1]), 0 },
{ PSF(maxs[2]), 0 }
};

int bg_numPlayerStateFields = ARRAY_LEN(bg_playerStateFields);

// may not contain spaces, dpmaster will reject the server
const char *bg_netGametypeNames[GT_MAX_GAME_TYPE] = {
	"FFA",
	"Tournament",
	"SP",
	"TeamDM",
	"CTF",
#ifdef MISSIONPACK
	"1FCTF",
	"Overload",
	"Harvester"
#endif
};

const char *bg_displayGametypeNames[GT_MAX_GAME_TYPE] = {
	"Free For All",
	"Tournament",
	"Single Player",
	"Team Deathmatch",
	"Capture the Flag",
#ifdef MISSIONPACK
	"One Flag CTF",
	"Overload",
	"Harvester"
#endif
};

/*
==============
BG_CheckSpawnEntity
==============
*/
qboolean BG_CheckSpawnEntity( const bgEntitySpawnInfo_t *info ) {
	int			i, gametype;
	char		*s, *value, *gametypeName;
	static char *gametypeNames[GT_MAX_GAME_TYPE] = {"ffa", "tournament", "single", "team", "ctf"
#ifdef MISSIONPACK
		, "oneflag", "obelisk", "harvester"
#endif
		};

	gametype = info->gametype;

	// check for "notsingle" flag
	if ( gametype == GT_SINGLE_PLAYER ) {
		info->spawnInt( "notsingle", "0", &i );
		if ( i ) {
			return qfalse;
		}
	}

	// check for "notteam" flag (GT_FFA, GT_TOURNAMENT, GT_SINGLE_PLAYER)
	if ( gametype >= GT_TEAM ) {
		info->spawnInt( "notteam", "0", &i );
		if ( i ) {
			return qfalse;
		}
	} else {
		info->spawnInt( "notfree", "0", &i );
		if ( i ) {
			return qfalse;
		}
	}

#ifdef MISSIONPACK
	info->spawnInt( "notta", "0", &i );
	if ( i ) {
			return qfalse;
	}
#else
	info->spawnInt( "notq3a", "0", &i );
	if ( i ) {
			return qfalse;
	}
#endif

	if( info->spawnString( "!gametype", NULL, &value ) ) {
		if( gametype >= 0 && gametype < GT_MAX_GAME_TYPE ) {
			gametypeName = gametypeNames[gametype];

			s = strstr( value, gametypeName );
			if( s ) {
				return qfalse;
			}
		}
	}

	if( info->spawnString( "gametype", NULL, &value ) ) {
		if( gametype >= 0 && gametype < GT_MAX_GAME_TYPE ) {
			gametypeName = gametypeNames[gametype];

			s = strstr( value, gametypeName );
			if( !s ) {
				return qfalse;
			}
		}
	}

	return qtrue;
}

/*
==============
BG_FindItemForPowerup
==============
*/
gitem_t	*BG_FindItemForPowerup( powerup_t pw ) {
	int		i;

	for ( i = 0 ; i < bg_numItems ; i++ ) {
		if ( (bg_itemlist[i].giType == IT_POWERUP || 
					bg_itemlist[i].giType == IT_TEAM ||
					bg_itemlist[i].giType == IT_PERSISTANT_POWERUP) && 
			bg_itemlist[i].giTag == pw ) {
			return &bg_itemlist[i];
		}
	}

	return NULL;
}


/*
==============
BG_FindItemForHoldable
==============
*/
gitem_t	*BG_FindItemForHoldable( holdable_t pw ) {
	int		i;

	for ( i = 0 ; i < bg_numItems ; i++ ) {
		if ( bg_itemlist[i].giType == IT_HOLDABLE && bg_itemlist[i].giTag == pw ) {
			return &bg_itemlist[i];
		}
	}

	Com_Error( ERR_DROP, "HoldableItem not found" );

	return NULL;
}


/*
===============
BG_FindItemForWeapon

===============
*/
gitem_t	*BG_FindItemForWeapon( weapon_t weapon ) {
	gitem_t	*it;
	
	for ( it = bg_itemlist + 1 ; it->classname ; it++) {
		if ( it->giType == IT_WEAPON && it->giTag == weapon ) {
			return it;
		}
	}

	Com_Error( ERR_DROP, "Couldn't find item for weapon %i", weapon);
	return NULL;
}

/*
===============
BG_FindItemForAmmo

===============
*/
gitem_t	*BG_FindItemForAmmo( weapon_t weapon ) {
	gitem_t	*it;
	
	for ( it = bg_itemlist + 1 ; it->classname ; it++) {
		if ( it->giType == IT_AMMO && it->giTag == weapon ) {
			return it;
		}
	}

	return NULL;
}

/*
===============
BG_FindItem

===============
*/
gitem_t	*BG_FindItem( const char *pickupName ) {
	gitem_t	*it;
	
	for ( it = bg_itemlist + 1 ; it->classname ; it++ ) {
		if ( !Q_stricmp( it->pickup_name, pickupName ) )
			return it;
	}

	return NULL;
}

/*
============
BG_PlayerTouchesItem

Items can be picked up without actually touching their physical bounds to make
grabbing them easier
============
*/
qboolean	BG_PlayerTouchesItem( playerState_t *ps, entityState_t *item, int atTime ) {
	vec3_t		origin;

	BG_EvaluateTrajectory( &item->pos, atTime, origin );

	// we are ignoring ducked differences here
	if ( ps->origin[0] - origin[0] > 44
		|| ps->origin[0] - origin[0] < -50
		|| ps->origin[1] - origin[1] > 36
		|| ps->origin[1] - origin[1] < -36
		|| ps->origin[2] - origin[2] > 36
		|| ps->origin[2] - origin[2] < -36 ) {
		return qfalse;
	}

	return qtrue;
}



/*
================
BG_CanItemBeGrabbed

Returns false if the item should not be picked up.
This needs to be the same for client side prediction and server use.
================
*/
qboolean BG_CanItemBeGrabbed( int gametype, const entityState_t *ent, const playerState_t *ps ) {
	gitem_t	*item;
#ifdef MISSIONPACK
	int		upperBound;
#endif

	if ( ent->modelindex < 1 || ent->modelindex >= BG_NumItems() ) {
		Com_Error( ERR_DROP, "BG_CanItemBeGrabbed: index out of range" );
	}

	item = BG_ItemForItemNum( ent->modelindex );

	switch( item->giType ) {
	case IT_WEAPON:
		return qtrue;	// weapons are always picked up

	case IT_AMMO:
		if ( ps->ammo[ item->giTag ] >= 200 ) {
			return qfalse;		// can't hold any more
		}
		return qtrue;

	case IT_ARMOR:
#ifdef MISSIONPACK
		if( BG_ItemForItemNum( ps->stats[STAT_PERSISTANT_POWERUP] )->giTag == PW_SCOUT ) {
			return qfalse;
		}

		// we also clamp armor to the maxhealth for handicapping
		if( BG_ItemForItemNum( ps->stats[STAT_PERSISTANT_POWERUP] )->giTag == PW_GUARD ) {
			upperBound = ps->stats[STAT_MAX_HEALTH];
		}
		else {
			upperBound = ps->stats[STAT_MAX_HEALTH] * 2;
		}

		if ( ps->stats[STAT_ARMOR] >= upperBound ) {
			return qfalse;
		}
#else
		if ( ps->stats[STAT_ARMOR] >= ps->stats[STAT_MAX_HEALTH] * 2 ) {
			return qfalse;
		}
#endif
		return qtrue;

	case IT_HEALTH:
		// small and mega healths will go over the max, otherwise
		// don't pick up if already at max
#ifdef MISSIONPACK
		if( BG_ItemForItemNum( ps->stats[STAT_PERSISTANT_POWERUP] )->giTag == PW_GUARD ) {
		}
		else
#endif
		if ( item->quantity == 5 || item->quantity == 100 ) {
			if ( ps->stats[STAT_HEALTH] >= ps->stats[STAT_MAX_HEALTH] * 2 ) {
				return qfalse;
			}
			return qtrue;
		}

		if ( ps->stats[STAT_HEALTH] >= ps->stats[STAT_MAX_HEALTH] ) {
			return qfalse;
		}
		return qtrue;

	case IT_POWERUP:
#ifdef MISSIONPACK
		// scout overrides haste
		if (item->giTag == PW_HASTE && BG_ItemForItemNum( ps->stats[STAT_PERSISTANT_POWERUP] )->giTag == PW_SCOUT ) {
			return qfalse;
		}
#endif
		return qtrue;

#ifdef MISSIONPACK
	case IT_PERSISTANT_POWERUP:
		// can only hold one item at a time
		if ( ps->stats[STAT_PERSISTANT_POWERUP] ) {
			return qfalse;
		}

		// check team only
		if( ent->team != 255 && ( ps->persistant[PERS_TEAM] != ent->team ) ) {
			return qfalse;
		}

		return qtrue;
#endif

	case IT_TEAM: // team items, such as flags
#ifdef MISSIONPACK		
		if( gametype == GT_1FCTF ) {
			// neutral flag can always be picked up
			if( item->giTag == PW_NEUTRALFLAG ) {
				return qtrue;
			}
			if (ps->persistant[PERS_TEAM] == TEAM_RED) {
				if (item->giTag == PW_BLUEFLAG  && ps->powerups[PW_NEUTRALFLAG] ) {
					return qtrue;
				}
			} else if (ps->persistant[PERS_TEAM] == TEAM_BLUE) {
				if (item->giTag == PW_REDFLAG  && ps->powerups[PW_NEUTRALFLAG] ) {
					return qtrue;
				}
			}
		}
#endif
		if( gametype == GT_CTF ) {
			// ent->modelindex2 is non-zero on items if they are dropped
			// we need to know this because we can pick up our dropped flag (and return it)
			// but we can't pick up our flag at base
			if (ps->persistant[PERS_TEAM] == TEAM_RED) {
				if (item->giTag == PW_BLUEFLAG ||
					(item->giTag == PW_REDFLAG && ent->modelindex2) ||
					(item->giTag == PW_REDFLAG && ps->powerups[PW_BLUEFLAG]) )
					return qtrue;
			} else if (ps->persistant[PERS_TEAM] == TEAM_BLUE) {
				if (item->giTag == PW_REDFLAG ||
					(item->giTag == PW_BLUEFLAG && ent->modelindex2) ||
					(item->giTag == PW_BLUEFLAG && ps->powerups[PW_REDFLAG]) )
					return qtrue;
			}
		}

#ifdef MISSIONPACK
		if( gametype == GT_HARVESTER ) {
			return qtrue;
		}
#endif
		return qfalse;

	case IT_HOLDABLE:
		// can only hold one item at a time
		if ( ps->stats[STAT_HOLDABLE_ITEM] ) {
			return qfalse;
		}
		return qtrue;

        case IT_BAD:
            Com_Error( ERR_DROP, "BG_CanItemBeGrabbed: IT_BAD" );
        default:
#ifndef Q3_VM
#ifndef NDEBUG
          Com_Printf("BG_CanItemBeGrabbed: unknown enum %d\n", item->giType );
#endif
#endif
         break;
	}

	return qfalse;
}

//======================================================================

/*
================
BG_EvaluateTrajectory

================
*/
void BG_EvaluateTrajectory( const trajectory_t *tr, int atTime, vec3_t result ) {
	float		deltaTime;
	float		phase;

	switch( tr->trType ) {
	case TR_STATIONARY:
	case TR_INTERPOLATE:
		VectorCopy( tr->trBase, result );
		break;
	case TR_LINEAR:
		deltaTime = ( atTime - tr->trTime ) * 0.001;	// milliseconds to seconds
		VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
		break;
	case TR_SINE:
		deltaTime = ( atTime - tr->trTime ) / (float) tr->trDuration;
		phase = sin( deltaTime * M_PI * 2 );
		VectorMA( tr->trBase, phase, tr->trDelta, result );
		break;
	case TR_LINEAR_STOP:
		if ( atTime > tr->trTime + tr->trDuration ) {
			atTime = tr->trTime + tr->trDuration;
		}
		deltaTime = ( atTime - tr->trTime ) * 0.001;	// milliseconds to seconds
		if ( deltaTime < 0 ) {
			deltaTime = 0;
		}
		VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
		break;
	case TR_GRAVITY:
		deltaTime = ( atTime - tr->trTime ) * 0.001;	// milliseconds to seconds
		VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
		result[2] -= 0.5 * DEFAULT_GRAVITY * deltaTime * deltaTime;		// FIXME: local gravity...
		break;
	default:
		Com_Error( ERR_DROP, "BG_EvaluateTrajectory: unknown trType: %i", tr->trType );
		break;
	}
}

/*
================
BG_EvaluateTrajectoryDelta

For determining velocity at a given time
================
*/
void BG_EvaluateTrajectoryDelta( const trajectory_t *tr, int atTime, vec3_t result ) {
	float	deltaTime;
	float	phase;

	switch( tr->trType ) {
	case TR_STATIONARY:
	case TR_INTERPOLATE:
		VectorClear( result );
		break;
	case TR_LINEAR:
		VectorCopy( tr->trDelta, result );
		break;
	case TR_SINE:
		deltaTime = ( atTime - tr->trTime ) / (float) tr->trDuration;
		phase = cos( deltaTime * M_PI * 2 );	// derivative of sin = cos
		phase *= 0.5;
		VectorScale( tr->trDelta, phase, result );
		break;
	case TR_LINEAR_STOP:
		if ( atTime > tr->trTime + tr->trDuration ) {
			VectorClear( result );
			return;
		}
		VectorCopy( tr->trDelta, result );
		break;
	case TR_GRAVITY:
		deltaTime = ( atTime - tr->trTime ) * 0.001;	// milliseconds to seconds
		VectorCopy( tr->trDelta, result );
		result[2] -= DEFAULT_GRAVITY * deltaTime;		// FIXME: local gravity...
		break;
	default:
		Com_Error( ERR_DROP, "BG_EvaluateTrajectoryDelta: unknown trType: %i", tr->trType );
		break;
	}
}

char *eventnames[] = {
	"EV_NONE",

	"EV_FOOTSTEP",
	"EV_FOOTSTEP_METAL",
	"EV_FOOTSPLASH",
	"EV_FOOTWADE",
	"EV_SWIM",

	"EV_STEP_4",
	"EV_STEP_8",
	"EV_STEP_12",
	"EV_STEP_16",

	"EV_FALL_SHORT",
	"EV_FALL_MEDIUM",
	"EV_FALL_FAR",

	"EV_JUMP_PAD",			// boing sound at origin", jump sound on player

	"EV_JUMP",
	"EV_WATER_TOUCH",	// foot touches
	"EV_WATER_LEAVE",	// foot leaves
	"EV_WATER_UNDER",	// head touches
	"EV_WATER_CLEAR",	// head leaves

	"EV_ITEM_PICKUP",			// normal item pickups are predictable
	"EV_GLOBAL_ITEM_PICKUP",	// powerup / team sounds are broadcast to everyone

	"EV_NOAMMO",
	"EV_CHANGE_WEAPON",
	"EV_FIRE_WEAPON",

	"EV_USE_ITEM0",
	"EV_USE_ITEM1",
	"EV_USE_ITEM2",
	"EV_USE_ITEM3",
	"EV_USE_ITEM4",
	"EV_USE_ITEM5",
	"EV_USE_ITEM6",
	"EV_USE_ITEM7",
	"EV_USE_ITEM8",
	"EV_USE_ITEM9",
	"EV_USE_ITEM10",
	"EV_USE_ITEM11",
	"EV_USE_ITEM12",
	"EV_USE_ITEM13",
	"EV_USE_ITEM14",
	"EV_USE_ITEM15",

	"EV_ITEM_RESPAWN",
	"EV_ITEM_POP",
	"EV_PLAYER_TELEPORT_IN",
	"EV_PLAYER_TELEPORT_OUT",

	"EV_GRENADE_BOUNCE",		// eventParm will be the soundindex

	"EV_GENERAL_SOUND",
	"EV_GLOBAL_SOUND",		// no attenuation
	"EV_GLOBAL_TEAM_SOUND",

	"EV_BULLET_HIT_FLESH",
	"EV_BULLET_HIT_WALL",

	"EV_MISSILE_HIT",
	"EV_MISSILE_MISS",
	"EV_MISSILE_MISS_METAL",
	"EV_RAILTRAIL",
	"EV_SHOTGUN",

	"EV_PAIN",
	"EV_DEATH1",
	"EV_DEATH2",
	"EV_DEATH3",
	"EV_OBITUARY",

	"EV_POWERUP_QUAD",
	"EV_POWERUP_BATTLESUIT",
	"EV_POWERUP_REGEN",

	"EV_SCOREPLUM",			// score plum

//#ifdef MISSIONPACK
	"EV_PROXIMITY_MINE_STICK",
	"EV_PROXIMITY_MINE_TRIGGER",
	"EV_KAMIKAZE",			// kamikaze explodes
	"EV_OBELISKEXPLODE",		// obelisk explodes
	"EV_OBELISKPAIN",		// obelisk pain
	"EV_INVUL_IMPACT",		// invulnerability sphere impact
	"EV_JUICED",				// invulnerability juiced effect
	"EV_LIGHTNINGBOLT",		// lightning bolt bounced of invulnerability sphere
//#endif

	"EV_DEBUG_LINE",
	"EV_STOPLOOPINGSOUND",
	"EV_TAUNT",
	"EV_TAUNT_YES",
	"EV_TAUNT_NO",
	"EV_TAUNT_FOLLOWME",
	"EV_TAUNT_GETFLAG",
	"EV_TAUNT_GUARDBASE",
	"EV_TAUNT_PATROL"

};

/*
===============
BG_AddPredictableEventToPlayerstate

Handles the sequence numbers
===============
*/

void	trap_Cvar_VariableStringBuffer( const char *var_name, char *buffer, int bufsize );

void BG_AddPredictableEventToPlayerstate( int newEvent, int eventParm, playerState_t *ps ) {

#ifdef _DEBUG
	{
		char buf[256];
		trap_Cvar_VariableStringBuffer("showevents", buf, sizeof(buf));
		if ( atof(buf) != 0 ) {
#ifdef GAME
			Com_Printf(" game event svt %5d -> %5d: num = %20s parm %d\n", ps->pmove_framecount/*ps->commandTime*/, ps->eventSequence, eventnames[newEvent], eventParm);
#else
			Com_Printf("Cgame event svt %5d -> %5d: num = %20s parm %d\n", ps->pmove_framecount/*ps->commandTime*/, ps->eventSequence, eventnames[newEvent], eventParm);
#endif
		}
	}
#endif
	ps->events[ps->eventSequence & (MAX_PS_EVENTS-1)] = newEvent;
	ps->eventParms[ps->eventSequence & (MAX_PS_EVENTS-1)] = eventParm;
	ps->eventSequence++;
}

/*
========================
BG_TouchJumpPad
========================
*/
void BG_TouchJumpPad( playerState_t *ps, entityState_t *jumppad ) {
	vec3_t	angles;
	float p;
	int effectNum;

	// spectators don't use jump pads
	if ( ps->pm_type != PM_NORMAL ) {
		return;
	}

	// flying characters don't hit bounce pads
	if ( ps->powerups[PW_FLIGHT] ) {
		return;
	}

	// if we didn't hit this same jumppad the previous frame
	// then don't play the event sound again if we are in a fat trigger
	if ( ps->jumppad_ent != jumppad->number ) {

		vectoangles( jumppad->origin2, angles);
		p = fabs( AngleNormalize180( angles[PITCH] ) );
		if( p < 45 ) {
			effectNum = 0;
		} else {
			effectNum = 1;
		}
		BG_AddPredictableEventToPlayerstate( EV_JUMP_PAD, effectNum, ps );
	}
	// remember hitting this jumppad this frame
	ps->jumppad_ent = jumppad->number;
	ps->jumppad_frame = ps->pmove_framecount;
	// give the player the velocity from the jumppad
	VectorCopy( jumppad->origin2, ps->velocity );
}

/*
========================
BG_PlayerStateToEntityState

This is done after each set of usercmd_t on the server,
and after local prediction on the client
========================
*/
void BG_PlayerStateToEntityState( playerState_t *ps, entityState_t *s, qboolean snap ) {
	int		i;

	if ( !ps->linked ) {
		s->eType = ET_INVISIBLE;
	} else {
		s->eType = ET_PLAYER;
	}

	s->number = ps->playerNum;

	s->pos.trType = TR_INTERPOLATE;
	VectorCopy( ps->origin, s->pos.trBase );
	if ( snap ) {
		SnapVector( s->pos.trBase );
	}
	// set the trDelta for flag direction
	VectorCopy( ps->velocity, s->pos.trDelta );

	s->apos.trType = TR_INTERPOLATE;
	VectorCopy( ps->viewangles, s->apos.trBase );
	if ( snap ) {
		SnapVector( s->apos.trBase );
	}

	s->angles2[YAW] = ps->movementDir;
	s->legsAnim = ps->legsAnim;
	s->torsoAnim = ps->torsoAnim;
	s->playerNum = ps->playerNum;		// ET_PLAYER looks here instead of at number
										// so corpses can also reference the proper config
	s->eFlags = ps->eFlags;
	if ( ps->stats[STAT_HEALTH] <= 0 ) {
		s->eFlags |= EF_DEAD;
	} else {
		s->eFlags &= ~EF_DEAD;
	}

	if ( ps->externalEvent ) {
		s->event = ps->externalEvent;
		s->eventParm = ps->externalEventParm;
	} else if ( ps->entityEventSequence < ps->eventSequence ) {
		int		seq;

		if ( ps->entityEventSequence < ps->eventSequence - MAX_PS_EVENTS) {
			ps->entityEventSequence = ps->eventSequence - MAX_PS_EVENTS;
		}
		seq = ps->entityEventSequence & (MAX_PS_EVENTS-1);
		s->event = ps->events[ seq ] | ( ( ps->entityEventSequence & 3 ) << 8 );
		s->eventParm = ps->eventParms[ seq ];
		ps->entityEventSequence++;
	}

	s->weapon = ps->weapon;
	s->groundEntityNum = ps->groundEntityNum;

	s->powerups = 0;
	for ( i = 0 ; i < MAX_POWERUPS ; i++ ) {
		if ( ps->powerups[ i ] ) {
			s->powerups |= 1 << i;
		}
	}

	s->contents = ps->contents;
	s->loopSound = ps->loopSound;
	s->tokens = ps->tokens;
	s->team = ps->persistant[PERS_TEAM];

	s->collisionType = ps->collisionType;

	VectorCopy( ps->mins, s->mins );
	VectorCopy( ps->maxs, s->maxs );
	if ( snap ) {
		SnapVector( s->mins );
		SnapVector( s->maxs );
	}
}

/*
========================
BG_PlayerStateToEntityStateExtraPolate

This is done after each set of usercmd_t on the server,
and after local prediction on the client
========================
*/
void BG_PlayerStateToEntityStateExtraPolate( playerState_t *ps, entityState_t *s, int time, qboolean snap ) {
	int		i;

	if ( !ps->linked ) {
		s->eType = ET_INVISIBLE;
	} else {
		s->eType = ET_PLAYER;
	}

	s->number = ps->playerNum;

	s->pos.trType = TR_LINEAR_STOP;
	VectorCopy( ps->origin, s->pos.trBase );
	if ( snap ) {
		SnapVector( s->pos.trBase );
	}
	// set the trDelta for flag direction and linear prediction
	VectorCopy( ps->velocity, s->pos.trDelta );
	// set the time for linear prediction
	s->pos.trTime = time;
	// set maximum extra polation time
	s->pos.trDuration = 50; // 1000 / sv_fps (default = 20)

	s->apos.trType = TR_INTERPOLATE;
	VectorCopy( ps->viewangles, s->apos.trBase );
	if ( snap ) {
		SnapVector( s->apos.trBase );
	}

	s->angles2[YAW] = ps->movementDir;
	s->legsAnim = ps->legsAnim;
	s->torsoAnim = ps->torsoAnim;
	s->playerNum = ps->playerNum;		// ET_PLAYER looks here instead of at number
										// so corpses can also reference the proper config
	s->eFlags = ps->eFlags;
	if ( ps->stats[STAT_HEALTH] <= 0 ) {
		s->eFlags |= EF_DEAD;
	} else {
		s->eFlags &= ~EF_DEAD;
	}

	if ( ps->externalEvent ) {
		s->event = ps->externalEvent;
		s->eventParm = ps->externalEventParm;
	} else if ( ps->entityEventSequence < ps->eventSequence ) {
		int		seq;

		if ( ps->entityEventSequence < ps->eventSequence - MAX_PS_EVENTS) {
			ps->entityEventSequence = ps->eventSequence - MAX_PS_EVENTS;
		}
		seq = ps->entityEventSequence & (MAX_PS_EVENTS-1);
		s->event = ps->events[ seq ] | ( ( ps->entityEventSequence & 3 ) << 8 );
		s->eventParm = ps->eventParms[ seq ];
		ps->entityEventSequence++;
	}

	s->weapon = ps->weapon;
	s->groundEntityNum = ps->groundEntityNum;

	s->powerups = 0;
	for ( i = 0 ; i < MAX_POWERUPS ; i++ ) {
		if ( ps->powerups[ i ] ) {
			s->powerups |= 1 << i;
		}
	}

	s->contents = ps->contents;
	s->loopSound = ps->loopSound;
	s->tokens = ps->tokens;
	s->team = ps->persistant[PERS_TEAM];

	s->collisionType = ps->collisionType;

	VectorCopy( ps->mins, s->mins );
	VectorCopy( ps->maxs, s->maxs );
	if ( snap ) {
		SnapVector( s->mins );
		SnapVector( s->maxs );
	}
}

/*
========================
BG_ComposeBits
========================
*/
void BG_ComposeBits( int *msg, int *bitsUsed, int value, int bits ) {
	*msg |= ( value & ( ( 1 << bits ) - 1 ) ) << *bitsUsed;
	*bitsUsed += bits;

	if ( *bitsUsed > 32 ) {
		Com_Error( ERR_DROP, "BG_ComposeBits exceeded 32 bits" );
	}
}

/*
========================
BG_DecomposeBits
========================
*/
void BG_DecomposeBits( int msg, int *bitsUsed, int *value, int bits ) {
	if ( value ) {
		*value = ( msg >> *bitsUsed ) & ( ( 1 << bits ) - 1 );
	}
	*bitsUsed += bits;

	if ( *bitsUsed > 32 ) {
		Com_Error( ERR_DROP, "BG_DecomposeBits exceeded 32 bits" );
	}
}

/*
========================
BG_ComposeUserCmdValue
========================
*/
int BG_ComposeUserCmdValue( int weapon ) {
	int value = 0;
	int bitsUsed = 0;

	BG_ComposeBits( &value, &bitsUsed, weapon, WEAPONNUM_BITS );

	return value;
}

/*
========================
BG_DecomposeUserCmdValue
========================
*/
void BG_DecomposeUserCmdValue( int value, int *weapon ) {
	int		bitsUsed = 0;

	BG_DecomposeBits( value, &bitsUsed, weapon, WEAPONNUM_BITS );
}

int cmdcmp( const void *a, const void *b ) {
  return Q_stricmp( (const char *)a, ((dummyCmd_t *)b)->name );
}

/*
=================
PC_SourceWarning
=================
*/
void PC_SourceWarning(int handle, char *format, ...) {
	int line;
	char filename[128];
	va_list argptr;
	static char string[4096];

	va_start (argptr, format);
	Q_vsnprintf (string, sizeof(string), format, argptr);
	va_end (argptr);

	filename[0] = '\0';
	line = 0;
	trap_PC_SourceFileAndLine(handle, filename, &line);

	Com_Printf(S_COLOR_YELLOW "WARNING: %s, line %d: %s\n", filename, line, string);
}

/*
=================
PC_SourceError
=================
*/
void PC_SourceError(int handle, char *format, ...) {
	int line;
	char filename[128];
	va_list argptr;
	static char string[4096];

	va_start (argptr, format);
	Q_vsnprintf (string, sizeof(string), format, argptr);
	va_end (argptr);

	filename[0] = '\0';
	line = 0;
	trap_PC_SourceFileAndLine(handle, filename, &line);

	Com_Printf(S_COLOR_RED "ERROR: %s, line %d: %s\n", filename, line, string);
}

/*
=================
PC_CheckTokenString
=================
*/
int PC_CheckTokenString(int handle, char *string) {
	pc_token_t tok;

	if (!trap_PC_ReadToken(handle, &tok)) return qfalse;
	//if the token is available
	if (!strcmp(tok.string, string)) return qtrue;
	//
	trap_PC_UnreadToken(handle);
	return qfalse;
}

/*
=================
PC_ExpectTokenString
=================
*/
int PC_ExpectTokenString(int handle, char *string) {
	pc_token_t token;

	if (!trap_PC_ReadToken(handle, &token))
	{
		PC_SourceError(handle, "couldn't find expected %s", string);
		return qfalse;
	} //end if

	if (strcmp(token.string, string))
	{
		PC_SourceError(handle, "expected %s, found %s", string, token.string);
		return qfalse;
	} //end if
	return qtrue;
}

/*
=================
PC_ExpectTokenType
=================
*/
int PC_ExpectTokenType(int handle, int type, int subtype, pc_token_t *token) {
	char str[MAX_TOKENLENGTH];

	if (!trap_PC_ReadToken(handle, token))
	{
		PC_SourceError(handle, "couldn't read expected token");
		return qfalse;
	}

	if (token->type != type)
	{
		strcpy(str, "");
		if (type == TT_STRING) strcpy(str, "string");
		if (type == TT_LITERAL) strcpy(str, "literal");
		if (type == TT_NUMBER) strcpy(str, "number");
		if (type == TT_NAME) strcpy(str, "name");
		if (type == TT_PUNCTUATION) strcpy(str, "punctuation");
		PC_SourceError(handle, "expected a %s, found %s", str, token->string);
		return qfalse;
	}
	if (token->type == TT_NUMBER)
	{
		if ((token->subtype & subtype) != subtype)
		{
			if (subtype & TT_DECIMAL) strcpy(str, "decimal");
			if (subtype & TT_HEX) strcpy(str, "hex");
			if (subtype & TT_OCTAL) strcpy(str, "octal");
			if (subtype & TT_BINARY) strcpy(str, "binary");
			if (subtype & TT_LONG) strcat(str, " long");
			if (subtype & TT_UNSIGNED) strcat(str, " unsigned");
			if (subtype & TT_FLOAT) strcat(str, " float");
			if (subtype & TT_INTEGER) strcat(str, " integer");
			PC_SourceError(handle, "expected %s, found %s", str, token->string);
			return qfalse;
		}
	}
	else if (token->type == TT_PUNCTUATION)
	{
		if (token->subtype != subtype)
		{
			PC_SourceError(handle, "found %s", token->string);
			return qfalse;
		}
	}
	return qtrue;
}

/*
=================
PC_ExpectAnyToken
=================
*/
int PC_ExpectAnyToken(int handle, pc_token_t *token) {
	if (!trap_PC_ReadToken(handle, token)) {
		PC_SourceError(handle, "couldn't read expected token");
		return qfalse;
	} else {
		return qtrue;
	}
}

//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
fielddef_t *FindField(fielddef_t *defs, char *name)
{
	int i;

	for (i = 0; defs[i].name; i++)
	{
		if (!strcmp(defs[i].name, name)) return &defs[i];
	} //end for
	return NULL;
} //end of the function FindField
//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
qboolean ReadNumber(int source, fielddef_t *fd, void *p)
{
	pc_token_t token;
	int negative = qfalse;
	long int intval, intmin = 0, intmax = 0;
	double floatval;

	if (!PC_ExpectAnyToken(source, &token)) return 0;

	//check for minus sign
	if (token.type == TT_PUNCTUATION)
	{
		if (fd->type & FT_UNSIGNED)
		{
			PC_SourceError(source, "expected unsigned value, found %s", token.string);
			return 0;
		} //end if
		//if not a minus sign
		if (strcmp(token.string, "-"))
		{
			PC_SourceError(source, "unexpected punctuation %s", token.string);
			return 0;
		} //end if
		negative = qtrue;
		//read the number
		if (!PC_ExpectAnyToken(source, &token)) return 0;
	} //end if
	//check if it is a number
	if (token.type != TT_NUMBER)
	{
		PC_SourceError(source, "expected number, found %s", token.string);
		return 0;
	} //end if
	//check for a float value
	if (token.subtype & TT_FLOAT)
	{
		if ((fd->type & FT_TYPE) != FT_FLOAT)
		{
			PC_SourceError(source, "unexpected float");
			return 0;
		} //end if
		floatval = token.floatvalue;
		if (negative) floatval = -floatval;
		if (fd->type & FT_BOUNDED)
		{
			if (floatval < fd->floatmin || floatval > fd->floatmax)
			{
				PC_SourceError(source, "float out of range [%f, %f]", fd->floatmin, fd->floatmax);
				return 0;
			} //end if
		} //end if
		*(float *) p = (float) floatval;
		return 1;
	} //end if
	//
	intval = token.intvalue;
	if (negative) intval = -intval;
	//check bounds
	if ((fd->type & FT_TYPE) == FT_CHAR)
	{
		if (fd->type & FT_UNSIGNED) {intmin = 0; intmax = 255;}
		else {intmin = -128; intmax = 127;}
	} //end if
	if ((fd->type & FT_TYPE) == FT_INT)
	{
		if (fd->type & FT_UNSIGNED) {intmin = 0; intmax = 65535;}
		else {intmin = -32768; intmax = 32767;}
	} //end else if
	if ((fd->type & FT_TYPE) == FT_CHAR || (fd->type & FT_TYPE) == FT_INT)
	{
		if (fd->type & FT_BOUNDED)
		{
			intmin = MAX(intmin, fd->floatmin);
			intmax = MIN(intmax, fd->floatmax);
		} //end if
		if (intval < intmin || intval > intmax)
		{
			PC_SourceError(source, "value %ld out of range [%ld, %ld]", intval, intmin, intmax);
			return 0;
		} //end if
	} //end if
	else if ((fd->type & FT_TYPE) == FT_FLOAT)
	{
		if (fd->type & FT_BOUNDED)
		{
			if (intval < fd->floatmin || intval > fd->floatmax)
			{
				PC_SourceError(source, "value %ld out of range [%f, %f]", intval, fd->floatmin, fd->floatmax);
				return 0;
			} //end if
		} //end if
	} //end else if
	//store the value
	if ((fd->type & FT_TYPE) == FT_CHAR)
	{
		if (fd->type & FT_UNSIGNED) *(unsigned char *) p = (unsigned char) intval;
		else *(char *) p = (char) intval;
	} //end if
	else if ((fd->type & FT_TYPE) == FT_INT)
	{
		if (fd->type & FT_UNSIGNED) *(unsigned int *) p = (unsigned int) intval;
		else *(int *) p = (int) intval;
	} //end else
	else if ((fd->type & FT_TYPE) == FT_FLOAT)
	{
		*(float *) p = (float) intval;
	} //end else
	return 1;
} //end of the function ReadNumber
//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
qboolean ReadChar(int source, fielddef_t *fd, void *p)
{
	pc_token_t token;

	if (!PC_ExpectAnyToken(source, &token)) return 0;

	//take literals into account
	if (token.type == TT_LITERAL)
	{
		*(char *) p = token.string[0];
	} //end if
	else
	{
		trap_PC_UnreadToken(source);
		if (!ReadNumber(source, fd, p)) return 0;
	} //end if
	return 1;
} //end of the function ReadChar
//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
int ReadString(int source, fielddef_t *fd, void *p)
{
	pc_token_t token;

	if (!PC_ExpectTokenType(source, TT_STRING, 0, &token)) return 0;
	//copy the string
	strncpy((char *) p, token.string, MAX_STRINGFIELD);
	//make sure the string is closed with a zero
	((char *)p)[MAX_STRINGFIELD-1] = '\0';
	//
	return 1;
} //end of the function ReadString

/*
=================
PC_ReadStructure
=================
*/
qboolean PC_ReadStructure(int source, structdef_t *def, void *structure)
{
	pc_token_t token;
	fielddef_t *fd;
	void *p;
	int num;

	if (!PC_ExpectTokenString(source, "{")) return 0;
	while(1)
	{
		if (!PC_ExpectAnyToken(source, &token)) return qfalse;
		//if end of structure
		if (!strcmp(token.string, "}")) break;
		//find the field with the name
		fd = FindField(def->fields, token.string);
		if (!fd)
		{
			PC_SourceError(source, "unknown structure field %s", token.string);
			return qfalse;
		} //end if
		if (fd->type & FT_ARRAY)
		{
			num = fd->maxarray;
			if (!PC_ExpectTokenString(source, "{")) return qfalse;
		} //end if
		else
		{
			num = 1;
		} //end else
		p = (void *)((byte*)structure + fd->offset);
		while (num-- > 0)
		{
			if (fd->type & FT_ARRAY)
			{
				if (PC_CheckTokenString(source, "}")) break;
			} //end if
			switch(fd->type & FT_TYPE)
			{
				case FT_CHAR:
				{
					if (!ReadChar(source, fd, p)) return qfalse;
					p = (char *) p + sizeof(char);
					break;
				} //end case
				case FT_INT:
				{
					if (!ReadNumber(source, fd, p)) return qfalse;
					p = (char *) p + sizeof(int);
					break;
				} //end case
				case FT_FLOAT:
				{
					if (!ReadNumber(source, fd, p)) return qfalse;
					p = (char *) p + sizeof(float);
					break;
				} //end case
				case FT_STRING:
				{
					if (!ReadString(source, fd, p)) return qfalse;
					p = (char *) p + MAX_STRINGFIELD;
					break;
				} //end case
				case FT_STRUCT:
				{
					if (!fd->substruct)
					{
						PC_SourceError(source, "BUG: no sub structure defined");
						return qfalse;
					} //end if
					PC_ReadStructure(source, fd->substruct, (char *) p);
					p = (char *) p + fd->substruct->size;
					break;
				} //end case
			} //end switch
			if (fd->type & FT_ARRAY)
			{
				if (!PC_ExpectAnyToken(source, &token)) return qfalse;
				if (!strcmp(token.string, "}")) break;
				if (strcmp(token.string, ","))
				{
					PC_SourceError(source, "expected a comma, found %s", token.string);
					return qfalse;
				} //end if
			} //end if
		} //end while
	} //end while
	return qtrue;
} //end of the function ReadStructure
