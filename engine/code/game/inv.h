/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2002-2015 Q3Rally Team (Per Thormann - q3rally@gmail.com)

This file is part of q3rally source code.

q3rally source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

q3rally source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with q3rally; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

#define INVENTORY_NONE				0
//armor
#define INVENTORY_ARMOR				1
//weapons
#define INVENTORY_GAUNTLET			4
#define INVENTORY_SHOTGUN			5
#define INVENTORY_MACHINEGUN		6
#define INVENTORY_GRENADELAUNCHER	7
#define INVENTORY_ROCKETLAUNCHER	8
#define INVENTORY_LIGHTNING			9
#define INVENTORY_RAILGUN			10
#define INVENTORY_PLASMAGUN			11
#define INVENTORY_BFG10K			13
#define INVENTORY_GRAPPLINGHOOK		14
#define INVENTORY_NAILGUN			15
#define INVENTORY_PROXLAUNCHER		16
#define INVENTORY_CHAINGUN			17
//Q3Rally Code Start
#define INVENTORY_FLAMETHROWER 18
//Q3Rally Code END
//ammo
#define INVENTORY_SHELLS			19
#define INVENTORY_BULLETS			20
#define INVENTORY_GRENADES			21
#define INVENTORY_CELLS				22
#define INVENTORY_LIGHTNINGAMMO		23
#define INVENTORY_ROCKETS			24
#define INVENTORY_SLUGS				25
#define INVENTORY_BFGAMMO			26
#define INVENTORY_NAILS				27
#define INVENTORY_MINES				28
#define INVENTORY_BELT				29
//powerups
#define INVENTORY_HEALTH			30
#define INVENTORY_TELEPORTER		31
#define INVENTORY_MEDKIT			32
#define INVENTORY_KAMIKAZE			33
#define INVENTORY_PORTAL			34
#define INVENTORY_INVULNERABILITY	35
#define INVENTORY_QUAD				36
#define INVENTORY_ENVIRONMENTSUIT	37
#define INVENTORY_HASTE				38
#define INVENTORY_INVISIBILITY		39
#define INVENTORY_REGEN				40
#define INVENTORY_FLIGHT			41
#define INVENTORY_SCOUT				42
#define INVENTORY_GUARD				43
#define INVENTORY_DOUBLER			44
#define INVENTORY_AMMOREGEN			45

#define INVENTORY_REDFLAG			46
#define INVENTORY_BLUEFLAG			47
#define INVENTORY_NEUTRALFLAG		48
#define INVENTORY_REDCUBE			49
#define INVENTORY_BLUECUBE			50
//enemy stuff
#define ENEMY_HORIZONTAL_DIST		200
#define ENEMY_HEIGHT				201
#define NUM_VISIBLE_ENEMIES			202
#define NUM_VISIBLE_TEAMMATES		203

// if running the mission pack
#ifdef MISSIONPACK

//#error "running mission pack"

#endif

//item numbers (make sure they are in sync with bg_itemlist in bg_misc.c)
#define MODELINDEX_ARMORSHARD		1
#define MODELINDEX_ARMORCOMBAT		2
#define MODELINDEX_ARMORBODY		3
#define MODELINDEX_HEALTHSMALL		4
#define MODELINDEX_HEALTH			5
#define MODELINDEX_HEALTHLARGE		6
#define MODELINDEX_HEALTHMEGA		7

#define MODELINDEX_GAUNTLET			8
#define MODELINDEX_SHOTGUN			9
#define MODELINDEX_MACHINEGUN		10
#define MODELINDEX_GRENADELAUNCHER	11
#define MODELINDEX_ROCKETLAUNCHER	12
#define MODELINDEX_LIGHTNING		13
#define MODELINDEX_RAILGUN			14
#define MODELINDEX_PLASMAGUN		15
#define MODELINDEX_BFG10K			16
#define MODELINDEX_GRAPPLINGHOOK	17
#define MODELINDEX_FLAMETHROWER   18
#define MODELINDEX_RWPSMOKE    19
#define MODELINDEX_RWPOIL     20
#define MODELINDEX_RWPMINE    21
#define MODELINDEX_RWPFLAME    22
#define MODELINDEX_RWPBIO      23


#define MODELINDEX_SHELLS			24
#define MODELINDEX_BULLETS			25
#define MODELINDEX_GRENADES			26
#define MODELINDEX_CELLS			27
#define MODELINDEX_LIGHTNINGAMMO	28
#define MODELINDEX_ROCKETS			29
#define MODELINDEX_SLUGS			30
#define MODELINDEX_BFGAMMO			31

#define MODELINDEX_TELEPORTER		32
#define MODELINDEX_MEDKIT			33
#define MODELINDEX_QUAD				34
#define MODELINDEX_ENVIRONMENTSUIT	35
#define MODELINDEX_HASTE			36
#define MODELINDEX_INVISIBILITY		37
#define MODELINDEX_REGEN			38
#define MODELINDEX_FLIGHT			39

#define MODELINDEX_REDFLAG			40
#define MODELINDEX_BLUEFLAG			41

// mission pack only defines

#define MODELINDEX_KAMIKAZE			42
#define MODELINDEX_PORTAL			43
#define MODELINDEX_INVULNERABILITY	44

#define MODELINDEX_NAILS			45
#define MODELINDEX_MINES			46
#define MODELINDEX_BELT				47

#define MODELINDEX_SCOUT			48
#define MODELINDEX_GUARD			49
#define MODELINDEX_DOUBLER			50
#define MODELINDEX_AMMOREGEN		51

#define MODELINDEX_NEUTRALFLAG		52
#define MODELINDEX_REDCUBE			53
#define MODELINDEX_BLUECUBE			54

#define MODELINDEX_NAILGUN			55
#define MODELINDEX_PROXLAUNCHER		56
#define MODELINDEX_CHAINGUN			57


//
#define WEAPONINDEX_GAUNTLET			1
#define WEAPONINDEX_MACHINEGUN			2
#define WEAPONINDEX_SHOTGUN				3
#define WEAPONINDEX_GRENADE_LAUNCHER	4
#define WEAPONINDEX_ROCKET_LAUNCHER		5
#define WEAPONINDEX_LIGHTNING			6
#define WEAPONINDEX_RAILGUN				7
#define WEAPONINDEX_PLASMAGUN			8
#define WEAPONINDEX_BFG					9
#define WEAPONINDEX_GRAPPLING_HOOK		10
#define WEAPONINDEX_FLAMETHROWER   11
#define WEAPONINDEX_NAILGUN				12
#define WEAPONINDEX_PROXLAUNCHER		13
#define WEAPONINDEX_CHAINGUN			14
