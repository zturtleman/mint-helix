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

#include "g_local.h"


typedef struct teamgame_s {
	float			last_flag_capture;
	int				last_capture_team;
	flagStatus_t	redStatus;	// CTF
	flagStatus_t	blueStatus;	// CTF
	flagStatus_t	flagStatus;	// One Flag CTF
	int				redTakenTime;
	int				blueTakenTime;
	int				redObeliskAttackedTime;
	int				blueObeliskAttackedTime;
} teamgame_t;

teamgame_t teamgame;

gentity_t	*neutralObelisk;

void Team_SetFlagStatus( int team, flagStatus_t status );

void Team_InitGame( void ) {
	memset(&teamgame, 0, sizeof teamgame);

	switch( g_gametype.integer ) {
	case GT_CTF:
		teamgame.redStatus = -1; // Invalid to force update
		Team_SetFlagStatus( TEAM_RED, FLAG_ATBASE );
		 teamgame.blueStatus = -1; // Invalid to force update
		Team_SetFlagStatus( TEAM_BLUE, FLAG_ATBASE );
		break;
#ifdef MISSIONPACK
	case GT_1FCTF:
		teamgame.flagStatus = -1; // Invalid to force update
		Team_SetFlagStatus( TEAM_FREE, FLAG_ATBASE );
		break;
#endif
	default:
		break;
	}
}

int OtherTeam(int team) {
	if (team==TEAM_RED)
		return TEAM_BLUE;
	else if (team==TEAM_BLUE)
		return TEAM_RED;
	return team;
}

const char *TeamName(int team)  {
	if (team==TEAM_RED)
		return "RED";
	else if (team==TEAM_BLUE)
		return "BLUE";
	else if (team==TEAM_SPECTATOR)
		return "SPECTATOR";
	return "FREE";
}

const char *TeamColorString(int team) {
	if (team==TEAM_RED)
		return S_COLOR_RED;
	else if (team==TEAM_BLUE)
		return S_COLOR_BLUE;
	else if (team==TEAM_SPECTATOR)
		return S_COLOR_YELLOW;
	return S_COLOR_WHITE;
}

// NULL for everyone
static __attribute__ ((format (printf, 2, 3))) void QDECL PrintMsg( gentity_t *ent, const char *fmt, ... ) {
	char		msg[1024];
	va_list		argptr;
	char		*p;
	
	va_start (argptr,fmt);
	if (Q_vsnprintf (msg, sizeof(msg), fmt, argptr) >= sizeof(msg)) {
		G_Error ( "PrintMsg overrun" );
	}
	va_end (argptr);

	// double quotes are bad
	while ((p = strchr(msg, '"')) != NULL)
		*p = '\'';

	trap_SendServerCommand ( ( (ent == NULL) ? -1 : ent-g_entities ), va("print \"%s\"", msg ));
}

/*
==============
AddTeamScore

 for gametype GT_TEAM this is called in AddScore in g_combat.c (checks fraglimit to win)
 for gametype > GT_TEAM this is called when gametype-specific scoring happens (checks capturelimit to win)
        (e.g. capture flag, kill obelisk, return skulls)
==============
*/
void AddTeamScore(vec3_t origin, int team, int score) {
	int			eventParm;
	int			otherTeam;
	gentity_t	*te;

	if ( score == 0 ) {
		return;
	}

	eventParm = -1;
	otherTeam = OtherTeam( team );

	if ( level.teamScores[ team ] + score == level.teamScores[ otherTeam ] ) {
		//teams are tied sound
		eventParm = GTS_TEAMS_ARE_TIED;
	} else if ( level.teamScores[ team ] >= level.teamScores[ otherTeam ] &&
				level.teamScores[ team ] + score < level.teamScores[ otherTeam ] ) {
		// other team took the lead sound (negative score)
		eventParm = ( otherTeam == TEAM_RED ) ? GTS_REDTEAM_TOOK_LEAD : GTS_BLUETEAM_TOOK_LEAD;
	} else if ( level.teamScores[ team ] <= level.teamScores[ otherTeam ] &&
				level.teamScores[ team ] + score > level.teamScores[ otherTeam ] ) {
		// this team took the lead sound
		eventParm = ( team == TEAM_RED ) ? GTS_REDTEAM_TOOK_LEAD : GTS_BLUETEAM_TOOK_LEAD;
	} else if ( score > 0 && g_gametype.integer != GT_TEAM ) {
		// team scored sound
		eventParm = ( team == TEAM_RED ) ? GTS_REDTEAM_SCORED : GTS_BLUETEAM_SCORED;
	}

	if ( eventParm != -1 ) {
		te = G_TempEntity(origin, EV_GLOBAL_TEAM_SOUND );
		te->r.svFlags |= SVF_BROADCAST;
		te->s.eventParm = eventParm;
	}

	level.teamScores[ team ] += score;
}

/*
==============
OnSameTeam
==============
*/
qboolean OnSameTeam( gentity_t *ent1, gentity_t *ent2 ) {
	if ( !ent1 || !ent1->player || !ent2 || !ent2->player ) {
		return qfalse;
	}

	if ( g_gametype.integer < GT_TEAM ) {
		return qfalse;
	}

	if ( ent1->player->sess.sessionTeam == ent2->player->sess.sessionTeam ) {
		return qtrue;
	}

	return qfalse;
}


static char ctfFlagStatusRemap[] = { '0', '1', '*', '*', '2' };
static char oneFlagStatusRemap[] = { '0', '1', '2', '3', '4' };

void Team_SetFlagStatus( int team, flagStatus_t status ) {
	qboolean modified = qfalse;

	switch( team ) {
	case TEAM_RED:	// CTF
		if( teamgame.redStatus != status ) {
			teamgame.redStatus = status;
			modified = qtrue;
		}
		break;

	case TEAM_BLUE:	// CTF
		if( teamgame.blueStatus != status ) {
			teamgame.blueStatus = status;
			modified = qtrue;
		}
		break;

	case TEAM_FREE:	// One Flag CTF
		if( teamgame.flagStatus != status ) {
			teamgame.flagStatus = status;
			modified = qtrue;
		}
		break;
	}

	if( modified ) {
		char st[4];

		if( g_gametype.integer == GT_CTF ) {
			st[0] = ctfFlagStatusRemap[teamgame.redStatus];
			st[1] = ctfFlagStatusRemap[teamgame.blueStatus];
			st[2] = 0;
		}
		else {		// GT_1FCTF
			st[0] = oneFlagStatusRemap[teamgame.flagStatus];
			st[1] = 0;
		}

		trap_SetConfigstring( CS_FLAGSTATUS, st );
	}
}

void Team_CheckDroppedItem( gentity_t *dropped ) {
	if( dropped->item->giTag == PW_REDFLAG ) {
		Team_SetFlagStatus( TEAM_RED, FLAG_DROPPED );
	}
	else if( dropped->item->giTag == PW_BLUEFLAG ) {
		Team_SetFlagStatus( TEAM_BLUE, FLAG_DROPPED );
	}
	else if( dropped->item->giTag == PW_NEUTRALFLAG ) {
		Team_SetFlagStatus( TEAM_FREE, FLAG_DROPPED );
	}
}

/*
================
Team_ForceGesture
================
*/
void Team_ForceGesture(int team) {
	int i;
	gentity_t *ent;

	for (i = 0; i < MAX_CLIENTS; i++) {
		ent = &g_entities[i];
		if (!ent->inuse)
			continue;
		if (!ent->player)
			continue;
		if (ent->player->sess.sessionTeam != team)
			continue;
		//
		ent->flags |= FL_FORCE_GESTURE;
	}
}

/*
================
Team_FragBonuses

Calculate the bonuses for flag defense, flag carrier defense, etc.
Note that bonuses are not cumulative.  You get one, they are in importance
order.
================
*/
void Team_FragBonuses(gentity_t *targ, gentity_t *inflictor, gentity_t *attacker)
{
	int i;
	gentity_t *ent;
	int flag_pw, enemy_flag_pw;
	int otherteam;
	gentity_t *flag, *carrier = NULL;
	char *c;
	vec3_t v1, v2;
	int team;

	// no bonus for fragging yourself or team mates
	if (!targ->player || !attacker->player || targ == attacker || OnSameTeam(targ, attacker))
		return;

	team = targ->player->sess.sessionTeam;
	otherteam = OtherTeam(targ->player->sess.sessionTeam);
	if (otherteam < 0)
		return; // whoever died isn't on a team

	// same team, if the flag at base, check to he has the enemy flag
	if (team == TEAM_RED) {
		flag_pw = PW_REDFLAG;
		enemy_flag_pw = PW_BLUEFLAG;
	} else {
		flag_pw = PW_BLUEFLAG;
		enemy_flag_pw = PW_REDFLAG;
	}

#ifdef MISSIONPACK
	if (g_gametype.integer == GT_1FCTF) {
		flag_pw = PW_NEUTRALFLAG;
		enemy_flag_pw = PW_NEUTRALFLAG;
	} 
#endif

	// did the attacker frag the flag carrier?
	if (targ->player->ps.powerups[enemy_flag_pw]) {
		attacker->player->pers.teamState.lastfraggedcarrier = level.time;
		AddScore(attacker, targ->r.currentOrigin, CTF_FRAG_CARRIER_BONUS);
		PrintMsg(NULL, "%s" S_COLOR_WHITE " fragged %s's flag carrier!\n",
			attacker->player->pers.netname, TeamName(team));

		// the target had the flag, clear the hurt carrier
		// field on the other team
		for (i = 0; i < g_maxplayers.integer; i++) {
			ent = g_entities + i;
			if (ent->inuse && ent->player->sess.sessionTeam == otherteam)
				ent->player->pers.teamState.lasthurtcarrier = 0;
		}
		return;
	}

	// did the attacker frag a skull carrier?
#ifdef MISSIONPACK
	if (g_gametype.integer == GT_HARVESTER && targ->player->ps.tokens) {
		int tokens = targ->player->ps.tokens;

		attacker->player->pers.teamState.lastfraggedcarrier = level.time;
		AddScore(attacker, targ->r.currentOrigin, CTF_FRAG_CARRIER_BONUS * tokens * tokens);
		PrintMsg(NULL, "%s" S_COLOR_WHITE " fragged %s's skull carrier!\n",
			attacker->player->pers.netname, TeamName(team));

		// the target had the flag, clear the hurt carrier
		// field on the other team
		for (i = 0; i < g_maxplayers.integer; i++) {
			ent = g_entities + i;
			if (ent->inuse && ent->player->sess.sessionTeam == otherteam)
				ent->player->pers.teamState.lasthurtcarrier = 0;
		}
		return;
	}
#endif

	if (targ->player->pers.teamState.lasthurtcarrier &&
		level.time - targ->player->pers.teamState.lasthurtcarrier < CTF_CARRIER_DANGER_PROTECT_TIMEOUT &&
		!attacker->player->ps.powerups[flag_pw]) {
		// attacker is on the same team as the flag carrier and
		// fragged a guy who hurt our flag carrier
		AddScore(attacker, targ->r.currentOrigin, CTF_CARRIER_DANGER_PROTECT_BONUS);

		targ->player->pers.teamState.lasthurtcarrier = 0;

		attacker->player->ps.persistant[PERS_DEFEND_COUNT]++;
		// add the sprite over the player's head
		attacker->player->ps.eFlags &= ~(EF_AWARD_IMPRESSIVE | EF_AWARD_EXCELLENT | EF_AWARD_GAUNTLET | EF_AWARD_ASSIST | EF_AWARD_DEFEND | EF_AWARD_CAP );
		attacker->player->ps.eFlags |= EF_AWARD_DEFEND;
		attacker->player->rewardTime = level.time + REWARD_SPRITE_TIME;

		return;
	}

	// flag and flag carrier area defense bonuses

	// we have to find the flag and carrier entities

#ifdef MISSIONPACK	
	if( g_gametype.integer == GT_OBELISK ) {
		// find the team obelisk
		switch (attacker->player->sess.sessionTeam) {
		case TEAM_RED:
			c = "team_redobelisk";
			break;
		case TEAM_BLUE:
			c = "team_blueobelisk";
			break;		
		default:
			return;
		}
		
	} else if (g_gametype.integer == GT_HARVESTER ) {
		// find the center obelisk
		c = "team_neutralobelisk";
	} else {
#endif
	// find the flag
	switch (attacker->player->sess.sessionTeam) {
	case TEAM_RED:
		c = "team_CTF_redflag";
		break;
	case TEAM_BLUE:
		c = "team_CTF_blueflag";
		break;		
	default:
		return;
	}
	// find attacker's team's flag carrier
	for (i = 0; i < g_maxplayers.integer; i++) {
		carrier = g_entities + i;
		if (carrier->inuse && carrier->player->ps.powerups[flag_pw])
			break;
		carrier = NULL;
	}
#ifdef MISSIONPACK
	}
#endif
	flag = NULL;
	while ((flag = G_Find (flag, FOFS(classname), c)) != NULL) {
		if (!(flag->flags & FL_DROPPED_ITEM))
			break;
	}

	if (!flag)
		return; // can't find attacker's flag

	// ok we have the attackers flag and a pointer to the carrier

	// check to see if we are defending the base's flag
	VectorSubtract(targ->r.currentOrigin, flag->r.currentOrigin, v1);
	VectorSubtract(attacker->r.currentOrigin, flag->r.currentOrigin, v2);

	if ( ( ( VectorLength(v1) < CTF_TARGET_PROTECT_RADIUS &&
		trap_InPVS(flag->r.currentOrigin, targ->r.currentOrigin ) ) ||
		( VectorLength(v2) < CTF_TARGET_PROTECT_RADIUS &&
		trap_InPVS(flag->r.currentOrigin, attacker->r.currentOrigin ) ) ) &&
		attacker->player->sess.sessionTeam != targ->player->sess.sessionTeam) {

		// we defended the base flag
		AddScore(attacker, targ->r.currentOrigin, CTF_FLAG_DEFENSE_BONUS);

		attacker->player->ps.persistant[PERS_DEFEND_COUNT]++;
		// add the sprite over the player's head
		attacker->player->ps.eFlags &= ~(EF_AWARD_IMPRESSIVE | EF_AWARD_EXCELLENT | EF_AWARD_GAUNTLET | EF_AWARD_ASSIST | EF_AWARD_DEFEND | EF_AWARD_CAP );
		attacker->player->ps.eFlags |= EF_AWARD_DEFEND;
		attacker->player->rewardTime = level.time + REWARD_SPRITE_TIME;

		return;
	}

	if (carrier && carrier != attacker) {
		VectorSubtract(targ->r.currentOrigin, carrier->r.currentOrigin, v1);
		VectorSubtract(attacker->r.currentOrigin, carrier->r.currentOrigin, v2);

		if ( ( ( VectorLength(v1) < CTF_ATTACKER_PROTECT_RADIUS &&
			trap_InPVS(carrier->r.currentOrigin, targ->r.currentOrigin ) ) ||
			( VectorLength(v2) < CTF_ATTACKER_PROTECT_RADIUS &&
				trap_InPVS(carrier->r.currentOrigin, attacker->r.currentOrigin ) ) ) &&
			attacker->player->sess.sessionTeam != targ->player->sess.sessionTeam) {
			AddScore(attacker, targ->r.currentOrigin, CTF_CARRIER_PROTECT_BONUS);

			attacker->player->ps.persistant[PERS_DEFEND_COUNT]++;
			// add the sprite over the player's head
			attacker->player->ps.eFlags &= ~(EF_AWARD_IMPRESSIVE | EF_AWARD_EXCELLENT | EF_AWARD_GAUNTLET | EF_AWARD_ASSIST | EF_AWARD_DEFEND | EF_AWARD_CAP );
			attacker->player->ps.eFlags |= EF_AWARD_DEFEND;
			attacker->player->rewardTime = level.time + REWARD_SPRITE_TIME;

			return;
		}
	}
}

/*
================
Team_CheckHurtCarrier

Check to see if attacker hurt the flag carrier.  Needed when handing out bonuses for assistance to flag
carrier defense.
================
*/
void Team_CheckHurtCarrier(gentity_t *targ, gentity_t *attacker)
{
	int flag_pw;

	if (!targ->player || !attacker->player)
		return;

	if (targ->player->sess.sessionTeam == TEAM_RED)
		flag_pw = PW_BLUEFLAG;
	else
		flag_pw = PW_REDFLAG;

#ifdef MISSIONPACK
	if (g_gametype.integer == GT_1FCTF) {
		flag_pw = PW_NEUTRALFLAG;
	}
#endif

	// flags
	if (targ->player->ps.powerups[flag_pw] &&
		targ->player->sess.sessionTeam != attacker->player->sess.sessionTeam)
		attacker->player->pers.teamState.lasthurtcarrier = level.time;

	// skulls
	if (targ->player->ps.tokens &&
		targ->player->sess.sessionTeam != attacker->player->sess.sessionTeam)
		attacker->player->pers.teamState.lasthurtcarrier = level.time;
}


gentity_t *Team_ResetFlag( int team ) {
	char *c;
	gentity_t *ent, *rent = NULL;

	switch (team) {
	case TEAM_RED:
		c = "team_CTF_redflag";
		break;
	case TEAM_BLUE:
		c = "team_CTF_blueflag";
		break;
	case TEAM_FREE:
		c = "team_CTF_neutralflag";
		break;
	default:
		return NULL;
	}

	ent = NULL;
	while ((ent = G_Find (ent, FOFS(classname), c)) != NULL) {
		if (ent->flags & FL_DROPPED_ITEM)
			G_FreeEntity(ent);
		else {
			rent = ent;
			RespawnItem(ent);
		}
	}

	Team_SetFlagStatus( team, FLAG_ATBASE );

	return rent;
}

void Team_ResetFlags( void ) {
	if( g_gametype.integer == GT_CTF ) {
		Team_ResetFlag( TEAM_RED );
		Team_ResetFlag( TEAM_BLUE );
	}
#ifdef MISSIONPACK
	else if( g_gametype.integer == GT_1FCTF ) {
		Team_ResetFlag( TEAM_FREE );
	}
#endif
}

void Team_ReturnFlagSound( gentity_t *ent, int team ) {
	gentity_t	*te;

	if (ent == NULL) {
		G_Printf ("Warning:  NULL passed to Team_ReturnFlagSound\n");
		return;
	}

	te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_TEAM_SOUND );
	if( team == TEAM_BLUE ) {
		te->s.eventParm = GTS_RED_RETURN;
	}
	else {
		te->s.eventParm = GTS_BLUE_RETURN;
	}
	te->r.svFlags |= SVF_BROADCAST;
}

void Team_TakeFlagSound( gentity_t *ent, int team ) {
	gentity_t	*te;

	if (ent == NULL) {
		G_Printf ("Warning:  NULL passed to Team_TakeFlagSound\n");
		return;
	}

	// only play sound when the flag was at the base
	// or not picked up the last 10 seconds
	switch(team) {
		case TEAM_RED:
			if( teamgame.blueStatus != FLAG_ATBASE ) {
				if (teamgame.blueTakenTime > level.time - 10000)
					return;
			}
			teamgame.blueTakenTime = level.time;
			break;

		case TEAM_BLUE:	// CTF
			if( teamgame.redStatus != FLAG_ATBASE ) {
				if (teamgame.redTakenTime > level.time - 10000)
					return;
			}
			teamgame.redTakenTime = level.time;
			break;
	}

	te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_TEAM_SOUND );
	if( team == TEAM_BLUE ) {
		te->s.eventParm = GTS_RED_TAKEN;
	}
	else {
		te->s.eventParm = GTS_BLUE_TAKEN;
	}
	te->r.svFlags |= SVF_BROADCAST;
}

void Team_CaptureFlagSound( gentity_t *ent, int team ) {
	gentity_t	*te;

	if (ent == NULL) {
		G_Printf ("Warning:  NULL passed to Team_CaptureFlagSound\n");
		return;
	}

	te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_TEAM_SOUND );
	if( team == TEAM_BLUE ) {
		te->s.eventParm = GTS_BLUE_CAPTURE;
	}
	else {
		te->s.eventParm = GTS_RED_CAPTURE;
	}
	te->r.svFlags |= SVF_BROADCAST;
}

void Team_ReturnFlag( int team ) {
	Team_ReturnFlagSound(Team_ResetFlag(team), team);
	if( team == TEAM_FREE ) {
		PrintMsg(NULL, "The flag has returned!\n" );
	}
	else {
		PrintMsg(NULL, "The %s flag has returned!\n", TeamName(team));
	}
}

void Team_FreeEntity( gentity_t *ent ) {
	if( ent->item->giTag == PW_REDFLAG ) {
		Team_ReturnFlag( TEAM_RED );
	}
	else if( ent->item->giTag == PW_BLUEFLAG ) {
		Team_ReturnFlag( TEAM_BLUE );
	}
	else if( ent->item->giTag == PW_NEUTRALFLAG ) {
		Team_ReturnFlag( TEAM_FREE );
	}
}

/*
==============
Team_DroppedFlagThink

Automatically set in Launch_Item if the item is one of the flags

Flags are unique in that if they are dropped, the base flag must be respawned when they time out
==============
*/
void Team_DroppedFlagThink(gentity_t *ent) {
	int		team = TEAM_FREE;

	if( ent->item->giTag == PW_REDFLAG ) {
		team = TEAM_RED;
	}
	else if( ent->item->giTag == PW_BLUEFLAG ) {
		team = TEAM_BLUE;
	}
	else if( ent->item->giTag == PW_NEUTRALFLAG ) {
		team = TEAM_FREE;
	}

	Team_ReturnFlagSound( Team_ResetFlag( team ), team );
	// Reset Flag will delete this entity
}


/*
==============
Team_DroppedFlagThink
==============
*/
int Team_TouchOurFlag( gentity_t *ent, gentity_t *other, int team ) {
	int			i;
	gentity_t	*player;
	gplayer_t	*cl = other->player;
	int			enemy_flag;

#ifdef MISSIONPACK
	if( g_gametype.integer == GT_1FCTF ) {
		enemy_flag = PW_NEUTRALFLAG;
	}
	else {
#endif
	if (cl->sess.sessionTeam == TEAM_RED) {
		enemy_flag = PW_BLUEFLAG;
	} else {
		enemy_flag = PW_REDFLAG;
	}

	if ( ent->flags & FL_DROPPED_ITEM ) {
		// hey, it's not home.  return it by teleporting it back
		PrintMsg( NULL, "%s" S_COLOR_WHITE " returned the %s flag!\n", 
			cl->pers.netname, TeamName(team));
		AddScore(other, ent->r.currentOrigin, CTF_RECOVERY_BONUS);

		other->player->pers.teamState.lastreturnedflag = level.time;
		//ResetFlag will remove this entity!  We must return zero
		Team_ReturnFlagSound(Team_ResetFlag(team), team);
		return 0;
	}
#ifdef MISSIONPACK
	}
#endif

	// the flag is at home base.  if the player has the enemy
	// flag, he's just won!
	if (!cl->ps.powerups[enemy_flag])
		return 0; // We don't have the flag
#ifdef MISSIONPACK
	if( g_gametype.integer == GT_1FCTF ) {
		trap_SendServerCommand( -1, va("cp \"%s" S_COLOR_WHITE "\ncaptured the flag!\n\"", cl->pers.netname ));
	}
	else {
#endif
	trap_SendServerCommand( -1, va("cp \"%s" S_COLOR_WHITE "\ncaptured the %s flag!\n\"", cl->pers.netname, TeamName(OtherTeam(team))));
#ifdef MISSIONPACK
	}
#endif

	cl->ps.powerups[enemy_flag] = 0;

	teamgame.last_flag_capture = level.time;
	teamgame.last_capture_team = team;

	// Increase the team's score
	AddTeamScore(ent->s.pos.trBase, other->player->sess.sessionTeam, 1);
	Team_ForceGesture(other->player->sess.sessionTeam);

	// add the sprite over the player's head
	other->player->ps.eFlags &= ~(EF_AWARD_IMPRESSIVE | EF_AWARD_EXCELLENT | EF_AWARD_GAUNTLET | EF_AWARD_ASSIST | EF_AWARD_DEFEND | EF_AWARD_CAP );
	other->player->ps.eFlags |= EF_AWARD_CAP;
	other->player->rewardTime = level.time + REWARD_SPRITE_TIME;
	other->player->ps.persistant[PERS_CAPTURES]++;

	// other gets another 10 frag bonus
	AddScore(other, ent->r.currentOrigin, CTF_CAPTURE_BONUS);

	Team_CaptureFlagSound( ent, team );

	// Ok, let's do the player loop, hand out the bonuses
	for (i = 0; i < g_maxplayers.integer; i++) {
		player = &g_entities[i];

		// also make sure we don't award assist bonuses to the flag carrier himself.
		if (!player->inuse || player == other)
			continue;

		if (player->player->sess.sessionTeam !=
			cl->sess.sessionTeam) {
			player->player->pers.teamState.lasthurtcarrier = -5;
		} else if (player->player->sess.sessionTeam ==
			cl->sess.sessionTeam) {
#ifdef MISSIONPACK
			AddScore(player, ent->r.currentOrigin, CTF_TEAM_BONUS);
#endif
			// award extra points for capture assists
			if (player->player->pers.teamState.lastreturnedflag + 
				CTF_RETURN_FLAG_ASSIST_TIMEOUT > level.time) {
				AddScore (player, ent->r.currentOrigin, CTF_RETURN_FLAG_ASSIST_BONUS);

				player->player->ps.persistant[PERS_ASSIST_COUNT]++;
				// add the sprite over the player's head
				player->player->ps.eFlags &= ~(EF_AWARD_IMPRESSIVE | EF_AWARD_EXCELLENT | EF_AWARD_GAUNTLET | EF_AWARD_ASSIST | EF_AWARD_DEFEND | EF_AWARD_CAP );
				player->player->ps.eFlags |= EF_AWARD_ASSIST;
				player->player->rewardTime = level.time + REWARD_SPRITE_TIME;

			} 
			if (player->player->pers.teamState.lastfraggedcarrier + 
				CTF_FRAG_CARRIER_ASSIST_TIMEOUT > level.time) {
				AddScore(player, ent->r.currentOrigin, CTF_FRAG_CARRIER_ASSIST_BONUS);

				player->player->ps.persistant[PERS_ASSIST_COUNT]++;
				// add the sprite over the player's head
				player->player->ps.eFlags &= ~(EF_AWARD_IMPRESSIVE | EF_AWARD_EXCELLENT | EF_AWARD_GAUNTLET | EF_AWARD_ASSIST | EF_AWARD_DEFEND | EF_AWARD_CAP );
				player->player->ps.eFlags |= EF_AWARD_ASSIST;
				player->player->rewardTime = level.time + REWARD_SPRITE_TIME;
			}
		}
	}
	Team_ResetFlags();

	CalculateRanks();

	return 0; // Do not respawn this automatically
}

int Team_TouchEnemyFlag( gentity_t *ent, gentity_t *other, int team ) {
	gplayer_t *cl = other->player;

#ifdef MISSIONPACK
	if( g_gametype.integer == GT_1FCTF ) {
		PrintMsg (NULL, "%s" S_COLOR_WHITE " got the flag!\n", other->player->pers.netname );

		cl->ps.powerups[PW_NEUTRALFLAG] = INT_MAX; // flags never expire

		if( team == TEAM_RED ) {
			Team_SetFlagStatus( TEAM_FREE, FLAG_TAKEN_RED );
		}
		else {
			Team_SetFlagStatus( TEAM_FREE, FLAG_TAKEN_BLUE );
		}
	}
	else{
#endif
		PrintMsg (NULL, "%s" S_COLOR_WHITE " got the %s flag!\n",
			other->player->pers.netname, TeamName(team));

		if (team == TEAM_RED)
			cl->ps.powerups[PW_REDFLAG] = INT_MAX; // flags never expire
		else
			cl->ps.powerups[PW_BLUEFLAG] = INT_MAX; // flags never expire

		Team_SetFlagStatus( team, FLAG_TAKEN );
#ifdef MISSIONPACK
	}

	AddScore(other, ent->r.currentOrigin, CTF_FLAG_BONUS);
#endif
	Team_TakeFlagSound( ent, team );

	return -1; // Do not respawn this automatically, but do delete it if it was FL_DROPPED
}

int Pickup_Team( gentity_t *ent, gentity_t *other ) {
	int team;
	gplayer_t *cl = other->player;

#ifdef MISSIONPACK
	if( g_gametype.integer == GT_OBELISK ) {
		// there are no team items that can be picked up in obelisk
		G_FreeEntity( ent );
		return 0;
	}

	if( g_gametype.integer == GT_HARVESTER ) {
		// the only team items that can be picked up in harvester are the cubes
		if( ent->s.team != cl->sess.sessionTeam ) {
			cl->ps.tokens += 1;
		}
		G_FreeEntity( ent );
		return 0;
	}
#endif
	// figure out what team this flag is
	if( strcmp(ent->classname, "team_CTF_redflag") == 0 ) {
		team = TEAM_RED;
	}
	else if( strcmp(ent->classname, "team_CTF_blueflag") == 0 ) {
		team = TEAM_BLUE;
	}
#ifdef MISSIONPACK
	else if( strcmp(ent->classname, "team_CTF_neutralflag") == 0  ) {
		team = TEAM_FREE;
	}
#endif
	else {
		PrintMsg ( other, "Don't know what team the flag is on.\n");
		return 0;
	}
#ifdef MISSIONPACK
	if( g_gametype.integer == GT_1FCTF ) {
		if( team == TEAM_FREE ) {
			return Team_TouchEnemyFlag( ent, other, cl->sess.sessionTeam );
		}
		if( team != cl->sess.sessionTeam) {
			return Team_TouchOurFlag( ent, other, cl->sess.sessionTeam );
		}
		return 0;
	}
#endif
	// GT_CTF
	if( team == cl->sess.sessionTeam) {
		return Team_TouchOurFlag( ent, other, team );
	}
	return Team_TouchEnemyFlag( ent, other, team );
}

/*
===========
Team_GetLocation

Report a location for the player. Uses placed nearby target_location entities
============
*/
gentity_t *Team_GetLocation(gentity_t *ent)
{
	gentity_t		*eloc, *best;
	float			bestlen, len;
	vec3_t			origin;

	if ( !ent ) {
		return NULL;
	}

	best = NULL;
	bestlen = 3*8192.0*8192.0;

	VectorCopy( ent->r.currentOrigin, origin );

	for (eloc = level.locationHead; eloc; eloc = eloc->nextTrain) {
		len = ( origin[0] - eloc->r.currentOrigin[0] ) * ( origin[0] - eloc->r.currentOrigin[0] )
			+ ( origin[1] - eloc->r.currentOrigin[1] ) * ( origin[1] - eloc->r.currentOrigin[1] )
			+ ( origin[2] - eloc->r.currentOrigin[2] ) * ( origin[2] - eloc->r.currentOrigin[2] );

		if ( len > bestlen ) {
			continue;
		}

		if ( !trap_InPVS( origin, eloc->r.currentOrigin ) ) {
			continue;
		}

		bestlen = len;
		best = eloc;
	}

	return best;
}


/*
===========
Team_GetLocation

Report a location for the player. Uses placed nearby target_location entities
============
*/
qboolean Team_GetLocationMsg(gentity_t *ent, char *loc, int loclen)
{
	gentity_t *best;

	best = Team_GetLocation( ent );
	
	if (!best)
		return qfalse;

	if (best->count) {
		if (best->count < 0)
			best->count = 0;
		if (best->count > 7)
			best->count = 7;
		Com_sprintf(loc, loclen, "%c%c%s" S_COLOR_WHITE, Q_COLOR_ESCAPE, best->count + '0', best->message );
	} else
		Com_sprintf(loc, loclen, "%s", best->message);

	return qtrue;
}


/*---------------------------------------------------------------------------*/

/*
================
SelectRandomTeamSpawnPoint

go to a random point that doesn't telefrag
================
*/
#define	MAX_TEAM_SPAWN_POINTS	32
gentity_t *SelectRandomTeamSpawnPoint( int teamstate, team_t team ) {
	gentity_t	*spot;
	int			count;
	int			selection;
	gentity_t	*spots[MAX_TEAM_SPAWN_POINTS];
	char		*classname;

	if (teamstate == TEAM_BEGIN) {
		if (team == TEAM_RED)
			classname = "team_CTF_redplayer";
		else if (team == TEAM_BLUE)
			classname = "team_CTF_blueplayer";
		else
			return NULL;
	} else {
		if (team == TEAM_RED)
			classname = "team_CTF_redspawn";
		else if (team == TEAM_BLUE)
			classname = "team_CTF_bluespawn";
		else
			return NULL;
	}
	count = 0;

	spot = NULL;

	while ((spot = G_Find (spot, FOFS(classname), classname)) != NULL) {
		if ( SpotWouldTelefrag( spot ) ) {
			continue;
		}
		spots[ count ] = spot;
		if (++count == MAX_TEAM_SPAWN_POINTS)
			break;
	}

	if ( !count ) {	// no spots that won't telefrag
		return G_Find( NULL, FOFS(classname), classname);
	}

	selection = rand() % count;
	return spots[ selection ];
}


/*
===========
SelectCTFSpawnPoint

============
*/
gentity_t *SelectCTFSpawnPoint ( team_t team, int teamstate, vec3_t origin, vec3_t angles, qboolean isbot ) {
	gentity_t	*spot;

	spot = SelectRandomTeamSpawnPoint ( teamstate, team );

	if (!spot) {
		return SelectSpawnPoint( vec3_origin, origin, angles, isbot );
	}

	VectorCopy (spot->s.origin, origin);
	origin[2] += 9;
	VectorCopy (spot->s.angles, angles);

	return spot;
}

/*---------------------------------------------------------------------------*/

static int QDECL SortPlayers( const void *a, const void *b ) {
	return *(int *)a - *(int *)b;
}


/*
==================
TeamplayLocationsMessage

Format:
	playerNum location health armor weapon powerups

==================
*/
void TeamplayInfoMessage( gentity_t *ent ) {
	char		entry[1024];
	char		string[8192];
	int			stringlength;
	int			i, j;
	gentity_t	*player;
	int			cnt;
	int			h, a;
	int			players[TEAM_MAXOVERLAY];
	int			team;

	if ( ! ent->player->pers.teamInfo )
		return;

	// send team info to spectator for team of followed player
	if (ent->player->sess.sessionTeam == TEAM_SPECTATOR) {
		if ( ent->player->sess.spectatorState != SPECTATOR_FOLLOW
			|| ent->player->sess.spectatorPlayer < 0 ) {
			return;
		}
		team = g_entities[ ent->player->sess.spectatorPlayer ].player->sess.sessionTeam;
	} else {
		team = ent->player->sess.sessionTeam;
	}

	if (team != TEAM_RED && team != TEAM_BLUE) {
		return;
	}

	// figure out what player should be on the display
	// we are limited to 8, but we want to use the top eight players
	// but in player order (so they don't keep changing position on the overlay)
	for (i = 0, cnt = 0; i < g_maxplayers.integer && cnt < TEAM_MAXOVERLAY; i++) {
		player = g_entities + level.sortedPlayers[i];
		if (player->inuse && player->player->sess.sessionTeam == team ) {
			players[cnt++] = level.sortedPlayers[i];
		}
	}

	// We have the top eight players, sort them by playerNum
	qsort( players, cnt, sizeof( players[0] ), SortPlayers );

	// send the latest information on all players
	string[0] = 0;
	stringlength = 0;

	for (i = 0, cnt = 0; i < g_maxplayers.integer && cnt < TEAM_MAXOVERLAY; i++) {
		player = g_entities + i;
		if (player->inuse && player->player->sess.sessionTeam == team ) {

			h = player->player->ps.stats[STAT_HEALTH];
			a = player->player->ps.stats[STAT_ARMOR];
			if (h < 0) h = 0;
			if (a < 0) a = 0;

			Com_sprintf (entry, sizeof(entry),
				" %i %i %i %i %i %i", 
//				level.sortedPlayers[i], player->player->pers.teamState.location, h, a, 
				i, player->player->pers.teamState.location, h, a, 
				player->player->ps.weapon, player->s.powerups);
			j = strlen(entry);
			if (stringlength + j >= sizeof(string))
				break;
			strcpy (string + stringlength, entry);
			stringlength += j;
			cnt++;
		}
	}

	trap_SendServerCommand( ent-g_entities, va("tinfo %i %i %s", team, cnt, string) );
}

void CheckTeamStatus(void) {
	int i;
	gentity_t *loc, *ent;

	if (level.time - level.lastTeamLocationTime > TEAM_LOCATION_UPDATE_TIME) {

		level.lastTeamLocationTime = level.time;

		for (i = 0; i < g_maxplayers.integer; i++) {
			ent = g_entities + i;

			if ( ent->player->pers.connected != CON_CONNECTED ) {
				continue;
			}

			if (ent->inuse && (ent->player->sess.sessionTeam == TEAM_RED ||	ent->player->sess.sessionTeam == TEAM_BLUE)) {
				loc = Team_GetLocation( ent );
				if (loc)
					ent->player->pers.teamState.location = loc->health;
				else
					ent->player->pers.teamState.location = 0;
			}
		}

		for (i = 0; i < g_maxplayers.integer; i++) {
			ent = g_entities + i;

			if ( ent->player->pers.connected != CON_CONNECTED ) {
				continue;
			}

			if (ent->inuse) {
				TeamplayInfoMessage( ent );
			}
		}
	}
}

/*-----------------------------------------------------------------*/

/*QUAKED team_CTF_redplayer (1 0 0) (-16 -16 -16) (16 16 32)
Only in CTF games.  Red players spawn here at game start.
*/
void SP_team_CTF_redplayer( gentity_t *ent ) {
}


/*QUAKED team_CTF_blueplayer (0 0 1) (-16 -16 -16) (16 16 32)
Only in CTF games.  Blue players spawn here at game start.
*/
void SP_team_CTF_blueplayer( gentity_t *ent ) {
}


/*QUAKED team_CTF_redspawn (1 0 0) (-16 -16 -24) (16 16 32)
potential spawning position for red team in CTF games.
Targets will be fired when someone spawns in on them.
*/
void SP_team_CTF_redspawn(gentity_t *ent) {
}

/*QUAKED team_CTF_bluespawn (0 0 1) (-16 -16 -24) (16 16 32)
potential spawning position for blue team in CTF games.
Targets will be fired when someone spawns in on them.
*/
void SP_team_CTF_bluespawn(gentity_t *ent) {
}


#ifdef MISSIONPACK
/*
================
Obelisks
================
*/

static void ObeliskRegen( gentity_t *self ) {
	self->nextthink = level.time + g_obeliskRegenPeriod.integer * 1000;
	if( self->health >= g_obeliskHealth.integer ) {
		return;
	}

	G_AddEvent( self, EV_POWERUP_REGEN, 0 );
	self->health += g_obeliskRegenAmount.integer;
	if ( self->health > g_obeliskHealth.integer ) {
		self->health = g_obeliskHealth.integer;
	}

	self->activator->s.modelindex2 = self->health * 0xff / g_obeliskHealth.integer;
	self->activator->s.frame = 0;
}


static void ObeliskRespawn( gentity_t *self ) {
	self->takedamage = qtrue;
	self->health = g_obeliskHealth.integer;

	self->think = ObeliskRegen;
	self->nextthink = level.time + g_obeliskRegenPeriod.integer * 1000;

	self->activator->s.frame = 0;
}


static void ObeliskDie( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod ) {
	int			otherTeam;

	otherTeam = OtherTeam( self->spawnflags );
	AddTeamScore(self->s.pos.trBase, otherTeam, 1);
	Team_ForceGesture(otherTeam);

	CalculateRanks();

	self->takedamage = qfalse;
	self->think = ObeliskRespawn;
	self->nextthink = level.time + g_obeliskRespawnDelay.integer * 1000;

	self->activator->s.modelindex2 = 0xff;
	self->activator->s.frame = 2;

	G_AddEvent( self->activator, EV_OBELISKEXPLODE, 0 );

	AddScore(attacker, self->r.currentOrigin, CTF_CAPTURE_BONUS);

	// add the sprite over the player's head
	attacker->player->ps.eFlags &= ~(EF_AWARD_IMPRESSIVE | EF_AWARD_EXCELLENT | EF_AWARD_GAUNTLET | EF_AWARD_ASSIST | EF_AWARD_DEFEND | EF_AWARD_CAP );
	attacker->player->ps.eFlags |= EF_AWARD_CAP;
	attacker->player->rewardTime = level.time + REWARD_SPRITE_TIME;
	attacker->player->ps.persistant[PERS_CAPTURES]++;

	Team_CaptureFlagSound(self, self->spawnflags);

	teamgame.redObeliskAttackedTime = 0;
	teamgame.blueObeliskAttackedTime = 0;

	trap_SendServerCommand( -1, va("cp \"%s" S_COLOR_WHITE "\ndestroyed the %s obelisk.\n\"",
			attacker->player->pers.netname, TeamName(self->spawnflags)));
}


static void ObeliskTouch( gentity_t *self, gentity_t *other, trace_t *trace ) {
	int			tokens;
	team_t		otherTeam;

	if ( !other->player ) {
		return;
	}

	otherTeam = OtherTeam( other->player->sess.sessionTeam );
	if ( otherTeam != self->spawnflags ) {
		return;
	}

	tokens = other->player->ps.tokens;
	if( tokens <= 0 ) {
		return;
	}

	trap_SendServerCommand( -1, va( "cp \"%s" S_COLOR_WHITE "\nbrought in %i %s %s.\n\"",
					other->player->pers.netname, tokens, TeamName( otherTeam ), ( tokens == 1 ) ? "skull" : "skulls" ) );

	AddTeamScore(self->s.pos.trBase, other->player->sess.sessionTeam, tokens);
	Team_ForceGesture(other->player->sess.sessionTeam);

	AddScore(other, self->r.currentOrigin, CTF_CAPTURE_BONUS*tokens);

	// add the sprite over the player's head
	other->player->ps.eFlags &= ~(EF_AWARD_IMPRESSIVE | EF_AWARD_EXCELLENT | EF_AWARD_GAUNTLET | EF_AWARD_ASSIST | EF_AWARD_DEFEND | EF_AWARD_CAP );
	other->player->ps.eFlags |= EF_AWARD_CAP;
	other->player->rewardTime = level.time + REWARD_SPRITE_TIME;
	other->player->ps.persistant[PERS_CAPTURES] += tokens;
	
	other->player->ps.tokens = 0;
	CalculateRanks();

	Team_CaptureFlagSound( self, self->spawnflags );
}

static void ObeliskPain( gentity_t *self, gentity_t *attacker, int damage ) {
	int actualDamage = damage / 10;
	if (actualDamage <= 0) {
		actualDamage = 1;
	}
	self->activator->s.modelindex2 = self->health * 0xff / g_obeliskHealth.integer;
	if (!self->activator->s.frame) {
		G_AddEvent(self, EV_OBELISKPAIN, 0);
	}
	self->activator->s.frame = 1;
	AddScore(attacker, self->r.currentOrigin, actualDamage);
}

// spawn invisible damagable obelisk entity / harvester base trigger.
gentity_t *SpawnObelisk( vec3_t origin, vec3_t mins, vec3_t maxs, int team ) {
	gentity_t	*ent;

	ent = G_Spawn();

	VectorCopy( origin, ent->s.origin );
	VectorCopy( origin, ent->s.pos.trBase );
	VectorCopy( origin, ent->r.currentOrigin );

	VectorCopy( mins, ent->s.mins );
	VectorCopy( maxs, ent->s.maxs );

	ent->s.eType = ET_GENERAL;
	ent->flags = FL_NO_KNOCKBACK;

	if( g_gametype.integer == GT_OBELISK ) {
		ent->s.contents = CONTENTS_SOLID;
		ent->takedamage = qtrue;
		ent->health = g_obeliskHealth.integer;
		ent->die = ObeliskDie;
		ent->pain = ObeliskPain;
		ent->think = ObeliskRegen;
		ent->nextthink = level.time + g_obeliskRegenPeriod.integer * 1000;
	}
	if( g_gametype.integer == GT_HARVESTER ) {
		ent->s.contents = CONTENTS_TRIGGER;
		ent->touch = ObeliskTouch;
	}

	G_SetOrigin( ent, ent->s.origin );

	ent->spawnflags = team;

	trap_LinkEntity( ent );

	return ent;
}

// setup entity for team base model / obelisk model.
void ObeliskInit( gentity_t *ent ) {
	trace_t		tr;
	vec3_t		dest;

	ent->s.eType = ET_TEAM;

	VectorSet( ent->s.mins, -15, -15, 0 );
	VectorSet( ent->s.maxs, 15, 15, 87 );

	if ( ent->spawnflags & 1 ) {
		// suspended
		G_SetOrigin( ent, ent->s.origin );
	} else {
		// mappers like to put them exactly on the floor, but being coplanar
		// will sometimes show up as starting in solid, so lif it up one pixel
		ent->s.origin[2] += 1;

		// drop to floor
		VectorSet( dest, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2] - 4096 );
		trap_Trace( &tr, ent->s.origin, ent->s.mins, ent->s.maxs, dest, ent->s.number, MASK_SOLID );
		if ( tr.startsolid ) {
			ent->s.origin[2] -= 1;
			G_Printf( "SpawnObelisk: %s startsolid at %s\n", ent->classname, vtos(ent->s.origin) );

			ent->s.groundEntityNum = ENTITYNUM_NONE;
			G_SetOrigin( ent, ent->s.origin );
		}
		else {
			// allow to ride movers
			ent->s.groundEntityNum = tr.entityNum;
			G_SetOrigin( ent, tr.endpos );
		}
	}
}

/*QUAKED team_redobelisk (1 0 0) (-16 -16 0) (16 16 8)
*/
void SP_team_redobelisk( gentity_t *ent ) {
	gentity_t *obelisk;

	if ( g_gametype.integer <= GT_TEAM ) {
		G_FreeEntity(ent);
		return;
	}
	ObeliskInit( ent );
	if ( g_gametype.integer == GT_OBELISK ) {
		obelisk = SpawnObelisk( ent->s.origin, ent->s.mins, ent->s.maxs, TEAM_RED );
		obelisk->activator = ent;
		// initial obelisk health value
		ent->s.modelindex2 = 0xff;
		ent->s.frame = 0;
	}
	if ( g_gametype.integer == GT_HARVESTER ) {
		obelisk = SpawnObelisk( ent->s.origin, ent->s.mins, ent->s.maxs, TEAM_RED );
		obelisk->activator = ent;
	}
	ent->s.modelindex = TEAM_RED;
	trap_LinkEntity(ent);
}

/*QUAKED team_blueobelisk (0 0 1) (-16 -16 0) (16 16 88)
*/
void SP_team_blueobelisk( gentity_t *ent ) {
	gentity_t *obelisk;

	if ( g_gametype.integer <= GT_TEAM ) {
		G_FreeEntity(ent);
		return;
	}
	ObeliskInit( ent );
	if ( g_gametype.integer == GT_OBELISK ) {
		obelisk = SpawnObelisk( ent->s.origin, ent->s.mins, ent->s.maxs, TEAM_BLUE );
		obelisk->activator = ent;
		// initial obelisk health value
		ent->s.modelindex2 = 0xff;
		ent->s.frame = 0;
	}
	if ( g_gametype.integer == GT_HARVESTER ) {
		obelisk = SpawnObelisk( ent->s.origin, ent->s.mins, ent->s.maxs, TEAM_BLUE );
		obelisk->activator = ent;
	}
	ent->s.modelindex = TEAM_BLUE;
	trap_LinkEntity(ent);
}

/*QUAKED team_neutralobelisk (0 0 1) (-16 -16 0) (16 16 88)
*/
void SP_team_neutralobelisk( gentity_t *ent ) {
	if ( g_gametype.integer != GT_1FCTF && g_gametype.integer != GT_HARVESTER ) {
		G_FreeEntity(ent);
		return;
	}
	ObeliskInit( ent );
	if ( g_gametype.integer == GT_HARVESTER) {
		neutralObelisk = SpawnObelisk( ent->s.origin, ent->s.mins, ent->s.maxs, TEAM_FREE );
		neutralObelisk->activator = ent;
	}
	ent->s.modelindex = TEAM_FREE;
	trap_LinkEntity(ent);
}


/*
================
CheckObeliskAttack
================
*/
qboolean CheckObeliskAttack( gentity_t *obelisk, gentity_t *attacker ) {
	gentity_t	*te;

	// if this really is an obelisk
	if( obelisk->die != ObeliskDie ) {
		return qfalse;
	}

	// if the attacker is a player
	if( !attacker->player ) {
		return qfalse;
	}

	// if the obelisk is on the same team as the attacker then don't hurt it
	if( obelisk->spawnflags == attacker->player->sess.sessionTeam ) {
		return qtrue;
	}

	// obelisk may be hurt

	// if not played any sounds recently
	if ((obelisk->spawnflags == TEAM_RED &&
		teamgame.redObeliskAttackedTime < level.time - OVERLOAD_ATTACK_BASE_SOUND_TIME) ||
		(obelisk->spawnflags == TEAM_BLUE &&
		teamgame.blueObeliskAttackedTime < level.time - OVERLOAD_ATTACK_BASE_SOUND_TIME) ) {

		// tell which obelisk is under attack
		te = G_TempEntity( obelisk->s.pos.trBase, EV_GLOBAL_TEAM_SOUND );
		if( obelisk->spawnflags == TEAM_RED ) {
			te->s.eventParm = GTS_REDOBELISK_ATTACKED;
			teamgame.redObeliskAttackedTime = level.time;
		}
		else {
			te->s.eventParm = GTS_BLUEOBELISK_ATTACKED;
			teamgame.blueObeliskAttackedTime = level.time;
		}
		te->r.svFlags |= SVF_BROADCAST;
	}

	return qfalse;
}
#endif
