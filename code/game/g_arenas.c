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
//
// g_arenas.c
//

#include "g_local.h"


gentity_t	*podium1;
gentity_t	*podium2;
gentity_t	*podium3;


/*
==================
UpdateTournamentInfo
==================
*/
void UpdateTournamentInfo( void ) {
	int			i;
	gentity_t	*ent;
	int			playerNum;
	int			n, accuracy, perfect,	msglen;
#ifdef MISSIONPACK
  int score1, score2;
	qboolean won;
#endif
	char		buf[32];
	char		msg[MAX_STRING_CHARS];

	// find the real player
	ent = NULL;
	for (i = 0; i < level.maxplayers; i++ ) {
		ent = &g_entities[i];
		if ( !ent->inuse ) {
			continue;
		}
		if ( !( ent->r.svFlags & SVF_BOT ) ) {
			break;
		}
	}
	// this should never happen!
	if ( !ent || i == level.maxplayers ) {
		return;
	}
	playerNum = i;

	CalculateRanks();

	if ( level.players[playerNum].sess.sessionTeam == TEAM_SPECTATOR ) {
#ifdef MISSIONPACK
		Com_sprintf( msg, sizeof(msg), "postgame %i %i 0 0 0 0 0 0 0 0 0 0 0", level.numNonSpectatorPlayers, playerNum );
#else
		Com_sprintf( msg, sizeof(msg), "postgame %i %i 0 0 0 0 0 0", level.numNonSpectatorPlayers, playerNum );
#endif
	}
	else {
		if( ent->player->accuracy_shots ) {
			accuracy = ent->player->accuracy_hits * 100 / ent->player->accuracy_shots;
		}
		else {
			accuracy = 0;
		}
#ifdef MISSIONPACK
		won = qfalse;
		if (g_gametype.integer >= GT_TEAM) {
			score1 = level.teamScores[TEAM_RED];
			score2 = level.teamScores[TEAM_BLUE];
			if (level.players[playerNum].sess.sessionTeam	== TEAM_RED) {
				won = (level.teamScores[TEAM_RED] > level.teamScores[TEAM_BLUE]);
			} else {
				won = (level.teamScores[TEAM_BLUE] > level.teamScores[TEAM_RED]);
			}
		} else {
			if (&level.players[playerNum] == &level.players[ level.sortedPlayers[0] ]) {
				won = qtrue;
				score1 = level.players[ level.sortedPlayers[0] ].ps.persistant[PERS_SCORE];
				score2 = level.players[ level.sortedPlayers[1] ].ps.persistant[PERS_SCORE];
			} else {
				score2 = level.players[ level.sortedPlayers[0] ].ps.persistant[PERS_SCORE];
				score1 = level.players[ level.sortedPlayers[1] ].ps.persistant[PERS_SCORE];
			}
		}
		if (won && ent->player->ps.persistant[PERS_KILLED] == 0) {
			perfect = 1;
		} else {
			perfect = 0;
		}
		Com_sprintf( msg, sizeof(msg), "postgame %i %i %i %i %i %i %i %i %i %i %i %i %i %i", level.numNonSpectatorPlayers, playerNum, accuracy,
			ent->player->ps.persistant[PERS_IMPRESSIVE_COUNT], ent->player->ps.persistant[PERS_EXCELLENT_COUNT], ent->player->ps.persistant[PERS_DEFEND_COUNT],
			ent->player->ps.persistant[PERS_ASSIST_COUNT], ent->player->ps.persistant[PERS_GAUNTLET_FRAG_COUNT], ent->player->ps.persistant[PERS_SCORE],
			perfect, score1, score2, level.time, ent->player->ps.persistant[PERS_CAPTURES] );

#else
		perfect = ( level.players[playerNum].ps.persistant[PERS_RANK] == 0 && ent->player->ps.persistant[PERS_KILLED] == 0 ) ? 1 : 0;
		Com_sprintf( msg, sizeof(msg), "postgame %i %i %i %i %i %i %i %i", level.numNonSpectatorPlayers, playerNum, accuracy,
			ent->player->ps.persistant[PERS_IMPRESSIVE_COUNT], ent->player->ps.persistant[PERS_EXCELLENT_COUNT],
			ent->player->ps.persistant[PERS_GAUNTLET_FRAG_COUNT], ent->player->ps.persistant[PERS_SCORE],
			perfect );
#endif
	}

	msglen = strlen( msg );
	for( i = 0; i < level.numNonSpectatorPlayers; i++ ) {
		n = level.sortedPlayers[i];
		Com_sprintf( buf, sizeof(buf), " %i %i %i", n, level.players[n].ps.persistant[PERS_RANK], level.players[n].ps.persistant[PERS_SCORE] );
		msglen += strlen( buf );
		if( msglen >= sizeof(msg) ) {
			break;
		}
		strcat( msg, buf );
	}
	trap_Cmd_ExecuteText( EXEC_APPEND, msg );
}


static gentity_t *SpawnModelOnVictoryPad( gentity_t *pad, vec3_t offset, gentity_t *ent, int place ) {
	gentity_t	*body;
	vec3_t		vec;
	vec3_t		f, r, u;

	body = G_Spawn();
	if ( !body ) {
		G_Printf( S_COLOR_RED "ERROR: out of gentities\n" );
		return NULL;
	}

	body->classname = ent->player->pers.netname;
	body->player = ent->player;
	body->s = ent->s;
	body->s.eType = ET_PLAYER;		// could be ET_INVISIBLE
	body->s.eFlags = 0;				// clear EF_TALK, etc
	body->s.powerups = 0;			// clear powerups
	body->s.loopSound = 0;			// clear lava burning
	body->s.number = body - g_entities;
	body->timestamp = level.time;
	body->physicsObject = qtrue;
	body->physicsBounce = 0;		// don't bounce
	body->s.event = 0;
	body->s.pos.trType = TR_STATIONARY;
	body->s.groundEntityNum = ENTITYNUM_WORLD;
	body->s.legsAnim = LEGS_IDLE;
	body->s.torsoAnim = TORSO_STAND;
	if( body->s.weapon == WP_NONE ) {
		body->s.weapon = WP_MACHINEGUN;
	}
	if( body->s.weapon == WP_GAUNTLET) {
		body->s.torsoAnim = TORSO_STAND2;
	}
	body->s.event = 0;
	body->r.svFlags = ent->r.svFlags;
	VectorCopy (ent->s.mins, body->s.mins);
	VectorCopy (ent->s.maxs, body->s.maxs);
	VectorCopy (ent->r.absmin, body->r.absmin);
	VectorCopy (ent->r.absmax, body->r.absmax);
	body->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;
	body->s.contents = CONTENTS_BODY;
	body->r.ownerNum = ent->r.ownerNum;
	body->takedamage = qfalse;

	VectorSubtract( level.intermission_origin, pad->r.currentOrigin, vec );
	vectoangles( vec, body->s.apos.trBase );
	body->s.apos.trBase[PITCH] = 0;
	body->s.apos.trBase[ROLL] = 0;

	AngleVectors( body->s.apos.trBase, f, r, u );
	VectorMA( pad->r.currentOrigin, offset[0], f, vec );
	VectorMA( vec, offset[1], r, vec );
	VectorMA( vec, offset[2], u, vec );

	G_SetOrigin( body, vec );

	trap_LinkEntity (body);

	body->count = place;

	return body;
}


static void CelebrateStop( gentity_t *player ) {
	int		anim;

	if( player->s.weapon == WP_GAUNTLET) {
		anim = TORSO_STAND2;
	}
	else {
		anim = TORSO_STAND;
	}
	player->s.torsoAnim = ( ( player->s.torsoAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | anim;
}


#define	TIMER_GESTURE	(34*66+50)
static void CelebrateStart( gentity_t *player ) {
	player->s.torsoAnim = ( ( player->s.torsoAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | TORSO_GESTURE;
	player->nextthink = level.time + TIMER_GESTURE;
	player->think = CelebrateStop;

	/*
	player->player->ps.events[player->player->ps.eventSequence & (MAX_PS_EVENTS-1)] = EV_TAUNT;
	player->player->ps.eventParms[player->player->ps.eventSequence & (MAX_PS_EVENTS-1)] = 0;
	player->player->ps.eventSequence++;
	*/
	G_AddEvent(player, EV_TAUNT, 0);
}


static vec3_t	offsetFirst  = {0, 0, 74};
static vec3_t	offsetSecond = {-10, 60, 54};
static vec3_t	offsetThird  = {-19, -60, 45};

static void PodiumPlacementThink( gentity_t *podium ) {
	vec3_t		vec;
	vec3_t		origin;
	vec3_t		f, r, u;

	podium->nextthink = level.time + 100;

	AngleVectors( level.intermission_angle, vec, NULL, NULL );
	VectorMA( level.intermission_origin, trap_Cvar_VariableIntegerValue( "g_podiumDist" ), vec, origin );
	origin[2] -= trap_Cvar_VariableIntegerValue( "g_podiumDrop" );
	G_SetOrigin( podium, origin );

	if( podium1 ) {
		VectorSubtract( level.intermission_origin, podium->r.currentOrigin, vec );
		vectoangles( vec, podium1->s.apos.trBase );
		podium1->s.apos.trBase[PITCH] = 0;
		podium1->s.apos.trBase[ROLL] = 0;

		AngleVectors( podium1->s.apos.trBase, f, r, u );
		VectorMA( podium->r.currentOrigin, offsetFirst[0], f, vec );
		VectorMA( vec, offsetFirst[1], r, vec );
		VectorMA( vec, offsetFirst[2], u, vec );

		G_SetOrigin( podium1, vec );
	}

	if( podium2 ) {
		VectorSubtract( level.intermission_origin, podium->r.currentOrigin, vec );
		vectoangles( vec, podium2->s.apos.trBase );
		podium2->s.apos.trBase[PITCH] = 0;
		podium2->s.apos.trBase[ROLL] = 0;

		AngleVectors( podium2->s.apos.trBase, f, r, u );
		VectorMA( podium->r.currentOrigin, offsetSecond[0], f, vec );
		VectorMA( vec, offsetSecond[1], r, vec );
		VectorMA( vec, offsetSecond[2], u, vec );

		G_SetOrigin( podium2, vec );
	}

	if( podium3 ) {
		VectorSubtract( level.intermission_origin, podium->r.currentOrigin, vec );
		vectoangles( vec, podium3->s.apos.trBase );
		podium3->s.apos.trBase[PITCH] = 0;
		podium3->s.apos.trBase[ROLL] = 0;

		AngleVectors( podium3->s.apos.trBase, f, r, u );
		VectorMA( podium->r.currentOrigin, offsetThird[0], f, vec );
		VectorMA( vec, offsetThird[1], r, vec );
		VectorMA( vec, offsetThird[2], u, vec );

		G_SetOrigin( podium3, vec );
	}
}


static gentity_t *SpawnPodium( void ) {
	gentity_t	*podium;
	vec3_t		vec;
	vec3_t		origin;

	podium = G_Spawn();
	if ( !podium ) {
		return NULL;
	}

	podium->classname = "podium";
	podium->s.eType = ET_GENERAL;
	podium->s.number = podium - g_entities;
	podium->clipmask = CONTENTS_SOLID;
	podium->s.contents = CONTENTS_SOLID;
	podium->s.modelindex = G_ModelIndex( SP_PODIUM_MODEL );

	AngleVectors( level.intermission_angle, vec, NULL, NULL );
	VectorMA( level.intermission_origin, trap_Cvar_VariableIntegerValue( "g_podiumDist" ), vec, origin );
	origin[2] -= trap_Cvar_VariableIntegerValue( "g_podiumDrop" );
	G_SetOrigin( podium, origin );

	VectorSubtract( level.intermission_origin, podium->r.currentOrigin, vec );
	podium->s.apos.trBase[YAW] = vectoyaw( vec );
	trap_LinkEntity (podium);

	podium->think = PodiumPlacementThink;
	podium->nextthink = level.time + 100;
	return podium;
}


/*
==================
SpawnModelsOnVictoryPads
==================
*/
void SpawnModelsOnVictoryPads( void ) {
	gentity_t	*player;
	gentity_t	*podium;

	podium1 = NULL;
	podium2 = NULL;
	podium3 = NULL;

	podium = SpawnPodium();

	player = SpawnModelOnVictoryPad( podium, offsetFirst, &g_entities[level.sortedPlayers[0]],
				level.players[ level.sortedPlayers[0] ].ps.persistant[PERS_RANK] &~ RANK_TIED_FLAG );
	if ( player ) {
		player->nextthink = level.time + 2000;
		player->think = CelebrateStart;
		podium1 = player;
	}

	player = SpawnModelOnVictoryPad( podium, offsetSecond, &g_entities[level.sortedPlayers[1]],
				level.players[ level.sortedPlayers[1] ].ps.persistant[PERS_RANK] &~ RANK_TIED_FLAG );
	if ( player ) {
		podium2 = player;
	}

	if ( level.numNonSpectatorPlayers > 2 ) {
		player = SpawnModelOnVictoryPad( podium, offsetThird, &g_entities[level.sortedPlayers[2]],
				level.players[ level.sortedPlayers[2] ].ps.persistant[PERS_RANK] &~ RANK_TIED_FLAG );
		if ( player ) {
			podium3 = player;
		}
	}
}


/*
===============
Svcmd_AbortPodium_f
===============
*/
void Svcmd_AbortPodium_f( void ) {
	if( g_gametype.integer != GT_SINGLE_PLAYER ) {
		return;
	}

	if( podium1 ) {
		podium1->nextthink = level.time;
		podium1->think = CelebrateStop;
	}
}
