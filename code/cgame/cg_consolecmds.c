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
// cg_consolecmds.c -- text commands typed in at the local console, or
// executed by a key binding

#include "cg_local.h"
#include "../ui/ui_public.h"
#ifdef MISSIONPACK
#include "../ui/ui_shared.h"
#endif
#ifdef MISSIONPACK_HUD
extern menuDef_t *menuScoreboard;
#endif



void CG_TargetCommand_f( int localPlayerNum ) {
	int		targetNum;
	char	test[4];

	targetNum = CG_CrosshairPlayer( localPlayerNum );
	if ( targetNum == -1 ) {
		return;
	}

	trap_Argv( 1, test, 4 );
	trap_SendClientCommand( va( "%s %i %i", Com_LocalPlayerCvarName( localPlayerNum, "gc" ), targetNum, atoi( test ) ) );
}



/*
=================
CG_SizeUp_f

Keybinding command
=================
*/
static void CG_SizeUp_f (void) {
	// manually clamp here so cvar range warning isn't show
	trap_Cvar_SetValue("cg_viewsize", Com_Clamp( 30, 100, (int)(cg_viewsize.integer+10) ) );
}


/*
=================
CG_SizeDown_f

Keybinding command
=================
*/
static void CG_SizeDown_f (void) {
	// manually clamp here so cvar range warning isn't show
	trap_Cvar_SetValue("cg_viewsize", Com_Clamp( 30, 100, (int)(cg_viewsize.integer-10) ) );
}

/*
================
CG_MessageMode_f
================
*/
void CG_MessageMode_f( void ) {
	Q_strncpyz( cg.messageCommand, "say", sizeof (cg.messageCommand) );
	Q_strncpyz( cg.messagePrompt, "Say:", sizeof (cg.messagePrompt) );
	MField_Clear( &cg.messageField );
	cg.messageField.widthInChars = 30;
	Key_SetCatcher( Key_GetCatcher( ) ^ KEYCATCH_MESSAGE );
}

/*
================
CG_MessageMode2_f
================
*/
void CG_MessageMode2_f( void ) {
	Q_strncpyz( cg.messageCommand, "say_team", sizeof (cg.messageCommand) );
	Q_strncpyz( cg.messagePrompt, "Team Say:", sizeof (cg.messagePrompt) );
	MField_Clear( &cg.messageField );
	cg.messageField.widthInChars = 25;
	Key_SetCatcher( Key_GetCatcher( ) ^ KEYCATCH_MESSAGE );
}

/*
================
CG_MessageMode3_f
================
*/
void CG_MessageMode3_f( void ) {
	int playerNum = CG_CrosshairPlayer( 0 );
	if ( playerNum < 0 || playerNum >= MAX_CLIENTS ) {
		return;
	}
	Com_sprintf( cg.messageCommand, sizeof (cg.messageCommand), "tell %d", playerNum );
	Com_sprintf( cg.messagePrompt, sizeof (cg.messagePrompt), "Tell %s:", cgs.playerinfo[ playerNum ].name );
	MField_Clear( &cg.messageField );
	cg.messageField.widthInChars = 30;
	Key_SetCatcher( Key_GetCatcher( ) ^ KEYCATCH_MESSAGE );
}

/*
================
CG_MessageMode4_f
================
*/
void CG_MessageMode4_f( void ) {
	int playerNum = CG_LastAttacker( 0 );
	if ( playerNum < 0 || playerNum >= MAX_CLIENTS ) {
		return;
	}
	Com_sprintf( cg.messageCommand, sizeof (cg.messageCommand), "tell %d", playerNum );
	Com_sprintf( cg.messagePrompt, sizeof (cg.messagePrompt), "Tell %s:", cgs.playerinfo[ playerNum ].name );
	MField_Clear( &cg.messageField );
	cg.messageField.widthInChars = 30;
	Key_SetCatcher( Key_GetCatcher( ) ^ KEYCATCH_MESSAGE );
}


/*
=============
CG_Viewpos_f

Debugging command to print the current position
=============
*/
static void CG_Viewpos_f( int localPlayerNum ) {
	CG_Printf ("(%i %i %i) : %i\n", (int)cg.localPlayers[localPlayerNum].lastViewPos[0],
		(int)cg.localPlayers[localPlayerNum].lastViewPos[1],
		(int)cg.localPlayers[localPlayerNum].lastViewPos[2],
		(int)cg.localPlayers[localPlayerNum].lastViewAngles[YAW]);
}

/*
=============
CG_ScoresDown
=============
*/
static void CG_ScoresDown_f(int localPlayerNum) {
	localPlayer_t *player = &cg.localPlayers[localPlayerNum];

#ifdef MISSIONPACK_HUD
	CG_BuildSpectatorString();
#endif
	if ( cg.scoresRequestTime + 2000 < cg.time ) {
		// the scores are more than two seconds out of data,
		// so request new ones
		cg.scoresRequestTime = cg.time;
		trap_SendClientCommand( "score" );

		// leave the current scores up if they were already
		// displayed, but if this is the first hit, clear them out
		if ( !CG_AnyScoreboardShowing() ) {
			cg.numScores = 0;
		}

		player->showScores = qtrue;
	} else {
		// show the cached contents even if they just pressed if it
		// is within two seconds
		player->showScores = qtrue;
	}
}

/*
=============
CG_ScoresUp_f
=============
*/
static void CG_ScoresUp_f( int localPlayerNum ) {
	localPlayer_t *player = &cg.localPlayers[localPlayerNum];

	if ( player->showScores ) {
		player->showScores = qfalse;
		player->scoreFadeTime = cg.time;
	}
}

/*
=============
CG_SetModel_f
=============
*/
void CG_SetModel_f( int localPlayerNum ) {
	const char	*arg;
	char	name[256];
	char	cvarName[32];

	Q_strncpyz( cvarName, Com_LocalPlayerCvarName( localPlayerNum, "model"), sizeof (cvarName) );

	arg = CG_Argv( 1 );
	if ( arg[0] ) {
		trap_Cvar_Set( cvarName, arg );
		trap_Cvar_Set( Com_LocalPlayerCvarName( localPlayerNum, "headmodel"), arg );
	} else {
		trap_Cvar_VariableStringBuffer( cvarName, name, sizeof(name) );
		Com_Printf("%s is set to %s\n", cvarName, name);
	}
}

#ifdef MISSIONPACK_HUD
extern menuDef_t *menuScoreboard;
extern displayContextDef_t cgDC;
void Menu_Reset( void );			// FIXME: add to right include file
#ifdef MISSIONPACK
void UI_Load( void );
#endif

static void CG_LoadHud_f( void) {
  char buff[1024];
	const char *hudSet;
  memset(buff, 0, sizeof(buff));

#ifdef MISSIONPACK
	// must reload both ui and hud at once, they share the string memory pool
	UI_Load();

	Init_Display(&cgDC);
#else
	Init_Display(&cgDC);

	String_Init();
#endif

	Menu_Reset();
	
	trap_Cvar_VariableStringBuffer("cg_hudFiles", buff, sizeof(buff));
	hudSet = buff;
	if (hudSet[0] == '\0') {
		hudSet = "ui/hud.txt";
	}

	CG_LoadMenus(hudSet);
  menuScoreboard = NULL;
}

// In team gametypes skip spectators and scroll through entire team.
// First time through list only use red player then switch to only blue players, etc.
static int CG_ScrollScores( int scoreIndex, int dir ) {
	int i, team;

	if ( cg.numScores == 0 )
		return 0;

	if ( cgs.gametype < GT_TEAM ) {
		scoreIndex += dir;

		if ( scoreIndex < 0 )
			scoreIndex = cg.numScores - 1;
		else if ( scoreIndex >= cg.numScores )
			scoreIndex = 0;

		return scoreIndex;
	}

	team = cg.scores[scoreIndex].team;

	if ( team == TEAM_SPECTATOR ) {
		team = TEAM_RED;
	}

	for ( i = scoreIndex + dir; i < cg.numScores && i >= 0; i += dir ) {
		if ( cg.scores[i].team == team ) {
			return i;
		}
	}

	// didn't find any more players on this team in this direction,
	// wrap around and switch teams
	if ( cgs.gametype >= GT_TEAM ) {
		if ( team == TEAM_RED ) {
			team = TEAM_BLUE;
		} else {
			team = TEAM_RED;
		}
	}

	if ( dir > 0 )
		i = 0;
	else
		i = cg.numScores - 1;

	for ( /**/; i < cg.numScores && i >= 0; i += dir ) {
		if ( cg.scores[i].team == team ) {
			return i;
		}
	}

	// no change
	return scoreIndex;
}

static void CG_ScrollScoresDown_f( int localPlayerNum ) {
	if ( cg.snap && cg.snap->pss[localPlayerNum].pm_type == PM_INTERMISSION ) {
		cg.intermissionSelectedScore = CG_ScrollScores( cg.intermissionSelectedScore, 1 );
	}

	if ( cg.localPlayers[localPlayerNum].scoreBoardShowing ) {
		cg.localPlayers[localPlayerNum].selectedScore = CG_ScrollScores( cg.localPlayers[localPlayerNum].selectedScore, 1 );
	}
}


static void CG_ScrollScoresUp_f( int localPlayerNum ) {
	if ( cg.snap && cg.snap->pss[localPlayerNum].pm_type == PM_INTERMISSION ) {
		cg.intermissionSelectedScore = CG_ScrollScores( cg.intermissionSelectedScore, -1 );
	}

	if ( cg.localPlayers[localPlayerNum].scoreBoardShowing ) {
		cg.localPlayers[localPlayerNum].selectedScore = CG_ScrollScores( cg.localPlayers[localPlayerNum].selectedScore, -1 );
	}
}
#endif

static void CG_CameraOrbit( int speed, int delay ) {
	int i;

	trap_Cvar_SetValue( "cg_cameraOrbit", speed );
	if ( delay > 0 ) {
		trap_Cvar_SetValue( "cg_cameraOrbitDelay", delay );
	}

	for ( i = 0; i < CG_MaxSplitView(); i++ ) {
		trap_Cvar_SetValue(Com_LocalPlayerCvarName( i, "cg_thirdPerson" ), speed == 0 ? 0 : 1 );
		trap_Cvar_SetValue(Com_LocalPlayerCvarName( i, "cg_thirdPersonAngle" ), 0 );
		trap_Cvar_SetValue(Com_LocalPlayerCvarName( i, "cg_thirdPersonRange" ), 100 );
	}
}

#ifdef MISSIONPACK
static void CG_spWin_f( void) {
	CG_CameraOrbit( 2, 35 );
	CG_AddBufferedSound(cgs.media.winnerSound);
	//trap_S_StartLocalSound(cgs.media.winnerSound, CHAN_ANNOUNCER);
	CG_GlobalCenterPrint("YOU WIN!", SCREEN_HEIGHT/2, 2.0);
}

static void CG_spLose_f( void) {
	CG_CameraOrbit( 2, 35 );
	CG_AddBufferedSound(cgs.media.loserSound);
	//trap_S_StartLocalSound(cgs.media.loserSound, CHAN_ANNOUNCER);
	CG_GlobalCenterPrint("YOU LOSE...", SCREEN_HEIGHT/2, 2.0);
}

#endif

static void CG_TellTarget_f( int localPlayerNum ) {
	int		playerNum;
	char	command[128];
	char	message[128];

	playerNum = CG_CrosshairPlayer( localPlayerNum );
	if ( playerNum == -1 ) {
		return;
	}

	trap_Args( message, 128 );
	Com_sprintf( command, 128, "%s %i %s", Com_LocalPlayerCvarName( localPlayerNum, "tell" ), playerNum, message );
	trap_SendClientCommand( command );
}

static void CG_TellAttacker_f( int localPlayerNum ) {
	int		playerNum;
	char	command[128];
	char	message[128];

	playerNum = CG_LastAttacker( localPlayerNum );
	if ( playerNum == -1 ) {
		return;
	}

	trap_Args( message, 128 );
	Com_sprintf( command, 128, "%s %i %s", Com_LocalPlayerCvarName( localPlayerNum, "tell" ), playerNum, message );
	trap_SendClientCommand( command );
}

#ifdef MISSIONPACK
static void CG_VoiceTellTarget_f( int localPlayerNum ) {
	int		playerNum;
	char	command[128];
	char	message[128];

	playerNum = CG_CrosshairPlayer( localPlayerNum );
	if ( playerNum == -1 ) {
		return;
	}

	trap_Args( message, 128 );
	Com_sprintf( command, 128, "%s %i %s", Com_LocalPlayerCvarName( localPlayerNum, "vtell" ), playerNum, message );
	trap_SendClientCommand( command );
}

static void CG_VoiceTellAttacker_f( int localPlayerNum ) {
	int		playerNum;
	char	command[128];
	char	message[128];

	playerNum = CG_LastAttacker( localPlayerNum );
	if ( playerNum == -1 ) {
		return;
	}

	trap_Args( message, 128 );
	Com_sprintf( command, 128, "%s %i %s", Com_LocalPlayerCvarName( localPlayerNum, "vtell" ), playerNum, message );
	trap_SendClientCommand( command );
}

static void CG_NextTeamMember_f( int localPlayerNum ) {
  CG_SelectNextPlayer( localPlayerNum );
}

static void CG_PrevTeamMember_f( int localPlayerNum ) {
  CG_SelectPrevPlayer( localPlayerNum );
}

// ASS U ME's enumeration order as far as task specific orders, OFFENSE is zero, CAMP is last
//
static void CG_NextOrder_f( int localPlayerNum ) {
	localPlayer_t	*player;
	playerInfo_t	*pi;
	int				playerNum;
	int				team;

	player = &cg.localPlayers[ localPlayerNum ];

	if ( player->playerNum == -1 ) {
		return;
	}

	playerNum = cg.snap->pss[ localPlayerNum ].playerNum;
	team = cg.snap->pss[ localPlayerNum ].persistant[PERS_TEAM];

	pi = cgs.playerinfo + playerNum;

	if (pi) {
		if (!pi->teamLeader && sortedTeamPlayers[team][cg_currentSelectedPlayer[localPlayerNum].integer] != playerNum) {
			return;
		}
	}
	if (player->currentOrder < TEAMTASK_CAMP) {
		player->currentOrder++;

		if (player->currentOrder == TEAMTASK_RETRIEVE) {
			if (!CG_OtherTeamHasFlag()) {
				player->currentOrder++;
			}
		}

		if (player->currentOrder == TEAMTASK_ESCORT) {
			if (!CG_YourTeamHasFlag()) {
				player->currentOrder++;
			}
		}

	} else {
		player->currentOrder = TEAMTASK_OFFENSE;
	}
	player->orderPending = qtrue;
	player->orderTime = cg.time + 3000;
}


static void CG_ConfirmOrder_f( int localPlayerNum ) {
	localPlayer_t *player;

	player = &cg.localPlayers[ localPlayerNum ];

	if ( player->playerNum == -1 ) {
		return;
	}

	trap_Cmd_ExecuteText(EXEC_NOW, va("cmd %s %d %s\n", Com_LocalPlayerCvarName(localPlayerNum, "vtell"), player->acceptLeader, VOICECHAT_YES));
	trap_Cmd_ExecuteText(EXEC_NOW, "+button5; wait; -button5");
	if (cg.time < player->acceptOrderTime) {
		trap_SendClientCommand(va("teamtask %d\n", player->acceptTask));
		player->acceptOrderTime = 0;
	}
}

static void CG_DenyOrder_f( int localPlayerNum ) {
	localPlayer_t *player;

	player = &cg.localPlayers[ localPlayerNum ];

	if ( player->playerNum == -1 ) {
		return;
	}

	trap_Cmd_ExecuteText(EXEC_NOW, va("cmd %s %d %s\n", Com_LocalPlayerCvarName(localPlayerNum, "vtell"), player->acceptLeader, VOICECHAT_NO));
	trap_Cmd_ExecuteText(EXEC_NOW, va("%s; wait; %s", Com_LocalPlayerCvarName(localPlayerNum, "+button6"), Com_LocalPlayerCvarName(localPlayerNum, "-button6")));
	if (cg.time < player->acceptOrderTime) {
		player->acceptOrderTime = 0;
	}
}

static void CG_TaskOffense_f( int localPlayerNum ) {
	if (cgs.gametype == GT_CTF || cgs.gametype == GT_1FCTF) {
		trap_Cmd_ExecuteText(EXEC_NOW, va("cmd %s %s\n", Com_LocalPlayerCvarName(localPlayerNum, "vsay_team"), VOICECHAT_ONGETFLAG));
	} else {
		trap_Cmd_ExecuteText(EXEC_NOW, va("cmd %s %s\n", Com_LocalPlayerCvarName(localPlayerNum, "vsay_team"), VOICECHAT_ONOFFENSE));
	}
	trap_SendClientCommand(va("%s %d\n", Com_LocalPlayerCvarName(localPlayerNum, "teamtask"), TEAMTASK_OFFENSE));
}

static void CG_TaskDefense_f( int localPlayerNum ) {
	trap_Cmd_ExecuteText(EXEC_NOW, va("cmd %s %s\n", Com_LocalPlayerCvarName(localPlayerNum, "vsay_team"), VOICECHAT_ONDEFENSE));
	trap_SendClientCommand(va("teamtask %d\n", TEAMTASK_DEFENSE));
}

static void CG_TaskPatrol_f( int localPlayerNum ) {
	trap_Cmd_ExecuteText(EXEC_NOW, va("cmd %s %s\n", Com_LocalPlayerCvarName(localPlayerNum, "vsay_team"), VOICECHAT_ONPATROL));
	trap_SendClientCommand(va("%s %d\n", Com_LocalPlayerCvarName(localPlayerNum, "teamtask"), TEAMTASK_PATROL));
}

static void CG_TaskCamp_f( int localPlayerNum ) {
	trap_Cmd_ExecuteText(EXEC_NOW, va("cmd %s %s\n", Com_LocalPlayerCvarName(localPlayerNum, "vsay_team"), VOICECHAT_ONCAMPING));
	trap_SendClientCommand(va("%s %d\n", Com_LocalPlayerCvarName(localPlayerNum, "teamtask"), TEAMTASK_CAMP));
}

static void CG_TaskFollow_f( int localPlayerNum ) {
	trap_Cmd_ExecuteText(EXEC_NOW, va("cmd %s %s\n", Com_LocalPlayerCvarName(localPlayerNum, "vsay_team"), VOICECHAT_ONFOLLOW));
	trap_SendClientCommand(va("%s %d\n", Com_LocalPlayerCvarName(localPlayerNum, "teamtask"), TEAMTASK_FOLLOW));
}

static void CG_TaskRetrieve_f( int localPlayerNum ) {
	trap_Cmd_ExecuteText(EXEC_NOW, va("cmd %s %s\n", Com_LocalPlayerCvarName(localPlayerNum, "vsay_team"), VOICECHAT_ONRETURNFLAG));
	trap_SendClientCommand(va("%s %d\n", Com_LocalPlayerCvarName(localPlayerNum, "teamtask"), TEAMTASK_RETRIEVE));
}

static void CG_TaskEscort_f( int localPlayerNum ) {
	trap_Cmd_ExecuteText(EXEC_NOW, va("cmd %s %s\n", Com_LocalPlayerCvarName(localPlayerNum, "vsay_team"), VOICECHAT_ONFOLLOWCARRIER));
	trap_SendClientCommand(va("%s %d\n", Com_LocalPlayerCvarName(localPlayerNum, "teamtask"), TEAMTASK_ESCORT));
}

static void CG_TaskOwnFlag_f( int localPlayerNum ) {
	trap_Cmd_ExecuteText(EXEC_NOW, va("cmd %s %s\n", Com_LocalPlayerCvarName(localPlayerNum, "vsay_team"), VOICECHAT_IHAVEFLAG));
}

static void CG_TauntKillInsult_f( int localPlayerNum ) {
	trap_Cmd_ExecuteText(EXEC_NOW, va("cmd %s %s\n", Com_LocalPlayerCvarName(localPlayerNum, "vsay"), VOICECHAT_KILLINSULT));
}

static void CG_TauntPraise_f( int localPlayerNum ) {
	trap_Cmd_ExecuteText(EXEC_NOW, va("cmd %s %s\n", Com_LocalPlayerCvarName(localPlayerNum, "vsay"), VOICECHAT_PRAISE));
}

static void CG_TauntTaunt_f( int localPlayerNum ) {
	trap_Cmd_ExecuteText(EXEC_NOW, va("cmd %s\n", Com_LocalPlayerCvarName(localPlayerNum, "vtaunt")));
}

static void CG_TauntDeathInsult_f( int localPlayerNum ) {
	trap_Cmd_ExecuteText(EXEC_NOW, va("cmd %s %s\n", Com_LocalPlayerCvarName(localPlayerNum, "vsay"), VOICECHAT_DEATHINSULT));
}

static void CG_TauntGauntlet_f( int localPlayerNum ) {
	trap_Cmd_ExecuteText(EXEC_NOW, va("cmd %s %s\n", Com_LocalPlayerCvarName(localPlayerNum, "vsay"), VOICECHAT_KILLGAUNTLET));
}

static void CG_TaskSuicide_f( int localPlayerNum ) {
	int		playerNum;
	char	command[128];

	playerNum = CG_CrosshairPlayer(0);
	if ( playerNum == -1 ) {
		return;
	}

	Com_sprintf( command, 128, "%s %i suicide", Com_LocalPlayerCvarName( localPlayerNum, "tell" ), playerNum );
	trap_SendClientCommand( command );
}



/*
==================
CG_TeamMenu_f
==================
*/
/*
static void CG_TeamMenu_f( void ) {
  if (Key_GetCatcher() & KEYCATCH_CGAME) {
    CG_EventHandling(CGAME_EVENT_NONE);
    Key_SetCatcher(0);
  } else {
    CG_EventHandling(CGAME_EVENT_TEAMMENU);
    //Key_SetCatcher(KEYCATCH_CGAME);
  }
}
*/

/*
==================
CG_EditHud_f
==================
*/
/*
static void CG_EditHud_f( void ) {
  //cls.keyCatchers ^= KEYCATCH_CGAME;
  //VM_Call (cgvm, CG_EVENT_HANDLING, (cls.keyCatchers & KEYCATCH_CGAME) ? CGAME_EVENT_EDITHUD : CGAME_EVENT_NONE);
}
*/

#endif

/*
==================
CG_StartOrbit_f
==================
*/

static void CG_StartOrbit_f( void ) {
	char var[MAX_TOKEN_CHARS];

	trap_Cvar_VariableStringBuffer( "developer", var, sizeof( var ) );
	if ( !atoi(var) ) {
		return;
	}
	if (cg_cameraOrbit.value != 0) {
		CG_CameraOrbit( 0, -1 );
	} else {
		CG_CameraOrbit( 5, -1 );
	}
}

/*
static void CG_Camera_f( void ) {
	char name[1024];
	trap_Argv( 1, name, sizeof(name));
	if (trap_loadCamera(name)) {
		cg.cameraMode = qtrue;
		trap_startCamera(cg.time);
	} else {
		CG_Printf ("Unable to load camera %s\n",name);
	}
}
*/


void CG_GenerateTracemap(void)
{
	bgGenTracemap_t gen;

	if ( !cg.mapcoordsValid ) {
		CG_Printf( "Need valid mapcoords in the worldspawn to be able to generate a tracemap.\n" );
		return;
	}

	gen.trace = CG_Trace;
	gen.pointcontents = CG_PointContents;

	BG_GenerateTracemap(cgs.mapname, cg.mapcoordsMins, cg.mapcoordsMaxs, &gen);
}

/*
==================
CG_RemapShader_f
==================
*/
static void CG_RemapShader_f( void ) {
	char shader1[MAX_QPATH];
	char shader2[MAX_QPATH];
	char timeoffset[MAX_QPATH];

	if ( cg.connected && trap_Cvar_VariableIntegerValue( "sv_pure" ) ) {
		CG_Printf( "%s command cannot be used on pure servers.\n", CG_Argv( 0 ) );
		return;
	}

	if (trap_Argc() < 2 || trap_Argc() > 4) {
		CG_Printf( "Usage: %s <original shader> [new shader] [time offset]\n", CG_Argv( 0 ) );
		return;
	}

	Q_strncpyz( shader1, CG_Argv( 1 ), sizeof(shader1) );
	Q_strncpyz( shader2, CG_Argv( 2 ), sizeof( shader2 ) );

	if ( !strlen( shader2 ) ) {
		// reset shader remap
		Q_strncpyz( shader2, shader1, sizeof(shader2) );
	}

	Q_strncpyz( timeoffset, CG_Argv( 3 ), sizeof( timeoffset ) );

	trap_R_RemapShader( shader1, shader2, timeoffset );
}

/*
=================
CG_Play_f
=================
*/
void CG_Play_f( void ) {
	int 		i;
	int			c;
	sfxHandle_t	h;

	c = trap_Argc();

	if( c < 2 ) {
		Com_Printf ("Usage: play <sound filename> [sound filename] [sound filename] ...\n");
		return;
	}

	for( i = 1; i < c; i++ ) {
		h = trap_S_RegisterSound( CG_Argv( i ), qfalse );

		if( h ) {
			trap_S_StartLocalSound( h, CHAN_LOCAL_SOUND );
		}
	}
}

/*
=================
S_Music_f
=================
*/
void CG_Music_f( void ) {
	int		c;
	char	intro[MAX_QPATH];
	char	loop[MAX_QPATH];
	float	volume;
	float	loopVolume;

	c = trap_Argc();

	if ( c < 2 || c > 5 ) {
		Com_Printf ("Usage: music <musicfile> [loopfile] [volume] [loopvolume]\n");
		return;
	}

	trap_Argv( 1, intro, sizeof( intro ) );
	trap_Argv( 2, loop, sizeof( loop ) );

	volume = loopVolume = 1.0f;

	if ( c > 3 ) {
		volume = loopVolume = atof( CG_Argv( 3 ) );
	}
	if ( c > 4 ) {
		loopVolume = atof( CG_Argv( 4 ) );
	}

	trap_S_StartBackgroundTrack( intro, loop, volume, loopVolume );
}

/*
=================
CG_StopMusic_f
=================
*/
void CG_StopMusic_f( void ) {
	trap_S_StopBackgroundTrack();
}

/*
=================
CG_StopCinematic_f
=================
*/
void CG_StopCinematic_f( void ) {
	if ( cg.cinematicHandle < 0 )
		return;

	trap_CIN_StopCinematic(cg.cinematicHandle);
	cg.cinematicHandle = -1;
	//trap_S_StopAllSounds();
}

/*
=================
CG_Cinematic_f
=================
*/
void CG_Cinematic_f( void ) {
	char	arg[MAX_QPATH];
	char	s[6];
	float	x, y, width, height;
	int		bits = CIN_system;

	Com_DPrintf("CG_Cinematic_f\n");
	CG_StopCinematic_f();

	trap_Argv( 1, arg, sizeof( arg ) );
	trap_Argv( 2, s, sizeof( s ) );

	if (s[0] == '1' || Q_stricmp(arg,"demoend.roq")==0 || Q_stricmp(arg,"end.roq")==0) {
		bits |= CIN_hold;
	}
	if (s[0] == '2') {
		bits |= CIN_loop;
	}

	//trap_S_StopAllSounds();

	x = 0;
	y = 0;
	width = SCREEN_WIDTH;
	height = SCREEN_HEIGHT;
	CG_SetScreenPlacement( PLACE_CENTER, PLACE_CENTER );
	CG_AdjustFrom640( &x, &y, &width, &height );

	cg.cinematicHandle = trap_CIN_PlayCinematic( arg, x, y, width, height, bits );
}

/*
===================
CG_ToggleMenu_f
===================
*/
void CG_ToggleMenu_f( void ) {
	CG_DistributeKeyEvent( K_ESCAPE, qtrue, trap_Milliseconds(), cg.connState, -1, 0 );
	CG_DistributeKeyEvent( K_ESCAPE, qfalse, trap_Milliseconds(), cg.connState, -1, 0 );
}

static consoleCommand_t	cg_commands[] = {
	{ "testgun", CG_TestGun_f, CMD_INGAME },
	{ "testmodel", CG_TestModel_f, CMD_INGAME },
	{ "nextframe", CG_TestModelNextFrame_f, CMD_INGAME },
	{ "prevframe", CG_TestModelPrevFrame_f, CMD_INGAME },
	{ "nextskin", CG_TestModelNextSkin_f, CMD_INGAME },
	{ "prevskin", CG_TestModelPrevSkin_f, CMD_INGAME },
	{ "sizeup", CG_SizeUp_f, 0 },
	{ "sizedown", CG_SizeDown_f, 0 },
#ifdef MISSIONPACK
	{ "spWin", CG_spWin_f, CMD_INGAME },
	{ "spLose", CG_spLose_f, CMD_INGAME },
#ifdef MISSIONPACK_HUD
	{ "loadhud", CG_LoadHud_f, CMD_INGAME },
#endif
#endif
	{ "startOrbit", CG_StartOrbit_f, CMD_INGAME },
	//{ "camera", CG_Camera_f, CMD_INGAME },
	{ "loaddeferred", CG_LoadDeferredPlayers, CMD_INGAME },
	{ "generateTracemap", CG_GenerateTracemap, CMD_INGAME },
	{ "remapShader", CG_RemapShader_f, 0 },
	{ "play", CG_Play_f, 0 },
	{ "music", CG_Music_f, 0 },
	{ "stopmusic", CG_StopMusic_f, 0 },
	{ "cinematic", CG_Cinematic_f, 0 },
	{ "stopcinematic", CG_StopCinematic_f, 0 },
	{ "messageMode", CG_MessageMode_f },
	{ "messageMode2", CG_MessageMode2_f },
	{ "messageMode3", CG_MessageMode3_f },
	{ "messageMode4", CG_MessageMode4_f },
	{ "clear", Con_ClearConsole_f },
	{ "toggleconsole", Con_ToggleConsole_f },
	{ "togglemenu", CG_ToggleMenu_f }
};

static int cg_numCommands = ARRAY_LEN( cg_commands );

typedef struct {
	char	*cmd;
	void	(*function)(int);
	int		flags;
} playerConsoleCommand_t;

static playerConsoleCommand_t	playerCommands[] = {
	{ "+attack", IN_Button0Down, 0 },
	{ "-attack", IN_Button0Up, 0 },
	{ "+back",IN_BackDown, 0 },
	{ "-back",IN_BackUp, 0 },
	{ "+button0", IN_Button0Down, 0 },
	{ "-button0", IN_Button0Up, 0 },
	{ "+button10", IN_Button10Down, 0 },
	{ "-button10", IN_Button10Up, 0 },
	{ "+button11", IN_Button11Down, 0 },
	{ "-button11", IN_Button11Up, 0 },
	{ "+button12", IN_Button12Down, 0 },
	{ "-button12", IN_Button12Up, 0 },
	{ "+button13", IN_Button13Down, 0 },
	{ "-button13", IN_Button13Up, 0 },
	{ "+button14", IN_Button14Down, 0 },
	{ "-button14", IN_Button14Up, 0 },
	{ "+button1", IN_Button1Down, 0 },
	{ "-button1", IN_Button1Up, 0 },
	{ "+button2", IN_Button2Down, 0 },
	{ "-button2", IN_Button2Up, 0 },
	{ "+button3", IN_Button3Down, 0 },
	{ "-button3", IN_Button3Up, 0 },
	{ "+button4", IN_Button4Down, 0 },
	{ "-button4", IN_Button4Up, 0 },
	{ "+button5", IN_Button5Down, 0 },
	{ "-button5", IN_Button5Up, 0 },
	{ "+button6", IN_Button6Down, 0 },
	{ "-button6", IN_Button6Up, 0 },
	{ "+button7", IN_Button7Down, 0 },
	{ "-button7", IN_Button7Up, 0 },
	{ "+button8", IN_Button8Down, 0 },
	{ "-button8", IN_Button8Up, 0 },
	{ "+button9", IN_Button9Down, 0 },
	{ "-button9", IN_Button9Up, 0 },
	{ "+forward",IN_ForwardDown, 0 },
	{ "-forward",IN_ForwardUp, 0 },
	{ "+left",IN_LeftDown, 0 },
	{ "-left",IN_LeftUp, 0 },
	{ "+lookdown", IN_LookdownDown, 0 },
	{ "-lookdown", IN_LookdownUp, 0 },
	{ "+lookup", IN_LookupDown, 0 },
	{ "-lookup", IN_LookupUp, 0 },
	{ "+mlook", IN_MLookDown, 0 },
	{ "-mlook", IN_MLookUp, 0 },
	{ "+movedown",IN_DownDown, 0 },
	{ "-movedown",IN_DownUp, 0 },
	{ "+moveleft", IN_MoveleftDown, 0 },
	{ "-moveleft", IN_MoveleftUp, 0 },
	{ "+moveright", IN_MoverightDown, 0 },
	{ "-moveright", IN_MoverightUp, 0 },
	{ "+moveup",IN_UpDown, 0 },
	{ "-moveup",IN_UpUp, 0 },
	{ "+right",IN_RightDown, 0 },
	{ "-right",IN_RightUp, 0 },
	{ "+scores", CG_ScoresDown_f, CMD_INGAME },
	{ "-scores", CG_ScoresUp_f, CMD_INGAME },
	{ "+speed", IN_SpeedDown, 0 },
	{ "-speed", IN_SpeedUp, 0 },
	{ "+strafe", IN_StrafeDown, 0 },
	{ "-strafe", IN_StrafeUp, 0 },
	{ "+zoom", CG_ZoomDown_f, CMD_INGAME },
	{ "-zoom", CG_ZoomUp_f, CMD_INGAME },
	{ "centerview", IN_CenterView, 0 },
	{ "tcmd", CG_TargetCommand_f, CMD_INGAME },
	{ "tell_target", CG_TellTarget_f, CMD_INGAME },
	{ "tell_attacker", CG_TellAttacker_f, CMD_INGAME },
#ifdef MISSIONPACK
	{ "vtell_target", CG_VoiceTellTarget_f, CMD_INGAME },
	{ "vtell_attacker", CG_VoiceTellAttacker_f, CMD_INGAME },
	{ "nextTeamMember", CG_NextTeamMember_f, CMD_INGAME },
	{ "prevTeamMember", CG_PrevTeamMember_f, CMD_INGAME },
	{ "nextOrder", CG_NextOrder_f, CMD_INGAME },
	{ "confirmOrder", CG_ConfirmOrder_f, CMD_INGAME },
	{ "denyOrder", CG_DenyOrder_f, CMD_INGAME },
	{ "taskOffense", CG_TaskOffense_f, CMD_INGAME },
	{ "taskDefense", CG_TaskDefense_f, CMD_INGAME },
	{ "taskPatrol", CG_TaskPatrol_f, CMD_INGAME },
	{ "taskCamp", CG_TaskCamp_f, CMD_INGAME },
	{ "taskFollow", CG_TaskFollow_f, CMD_INGAME },
	{ "taskRetrieve", CG_TaskRetrieve_f, CMD_INGAME },
	{ "taskEscort", CG_TaskEscort_f, CMD_INGAME },
	{ "taskSuicide", CG_TaskSuicide_f, CMD_INGAME },
	{ "taskOwnFlag", CG_TaskOwnFlag_f, CMD_INGAME },
	{ "tauntKillInsult", CG_TauntKillInsult_f, CMD_INGAME },
	{ "tauntPraise", CG_TauntPraise_f, CMD_INGAME },
	{ "tauntTaunt", CG_TauntTaunt_f, CMD_INGAME },
	{ "tauntDeathInsult", CG_TauntDeathInsult_f, CMD_INGAME },
	{ "tauntGauntlet", CG_TauntGauntlet_f, CMD_INGAME },
#endif
#ifdef MISSIONPACK_HUD
	{ "scoresDown", CG_ScrollScoresDown_f, CMD_INGAME },
	{ "scoresUp", CG_ScrollScoresUp_f, CMD_INGAME },
#endif
	{ "model", CG_SetModel_f, 0 },
	{ "viewpos", CG_Viewpos_f, CMD_INGAME },
	{ "weapnext", CG_NextWeapon_f, CMD_INGAME },
	{ "weapprev", CG_PrevWeapon_f, CMD_INGAME },
	{ "weapon", CG_Weapon_f, CMD_INGAME }
};

static int numPlayerCommands = ARRAY_LEN( playerCommands );

/*
=================
CG_CheckCmdFlags
=================
*/
static qboolean CG_CheckCmdFlags( const char *cmd, int flags ) {
	if ( ( flags & CMD_INGAME ) && !cg.snap ) {
		CG_Printf("Must be in game to use command \"%s\"\n", cmd);
		return qtrue;
	}

	if ( ( flags & CMD_MENU ) && cg.connected ) {
		CG_Printf("Cannot use command \"%s\" while in game\n", cmd);
		return qtrue;
	}

	return qfalse;
}

/*
=================
CG_CheckCommands
=================
*/
static qboolean CG_CheckCommands( consoleCommand_t *commands, int numCommands, const char *cmd ) {
	int i;

	for ( i = 0 ; i < numCommands ; i++ ) {
		if ( !Q_stricmp( cmd, commands[i].cmd )) {
			if ( CG_CheckCmdFlags( cmd, commands[i].flags ) ) {
				return qtrue;
			}
			commands[i].function();
			return qtrue;
		}
	}
	return qfalse;
}

/*
=================
CG_ConsoleCommand

The string has been tokenized and can be retrieved with
Cmd_Argc() / Cmd_Argv()
=================
*/
qboolean CG_ConsoleCommand( connstate_t state, int realTime ) {
	char		buffer[BIG_INFO_STRING];
	const char	*cmd;
	int		i;
	int		localPlayerNum;
	const char	*baseCmd;

	cg.connState = state;

	// update UI frame time
	UI_ConsoleCommand( realTime );

	cmd = CG_Argv(0);

	localPlayerNum = Com_LocalPlayerForCvarName( cmd );
	baseCmd = Com_LocalPlayerBaseCvarName( cmd );

	for ( i = 0 ; i < numPlayerCommands ; i++ ) {
		if ( !Q_stricmp( baseCmd, playerCommands[i].cmd )) {
			if ( CG_CheckCmdFlags( cmd, playerCommands[i].flags ) ) {
				return qtrue;
			}
			playerCommands[i].function( localPlayerNum );
			return qtrue;
		}
	}

	if ( localPlayerNum == 0 ) {
		if ( CG_CheckCommands( cg_commands, cg_numCommands, cmd ) ) {
			return qtrue;
		}

		if ( CG_CheckCommands( ui_commands, ui_numCommands, cmd ) ) {
			return qtrue;
		}
	}

	if ( cg.connected && trap_GetDemoState() != DS_PLAYBACK ) {
		// the game server will interpret these commands
		trap_LiteralArgs( buffer, sizeof ( buffer ) );
		trap_SendClientCommand( buffer );
		return qtrue;
	}

	return qfalse;
}


/*
=================
CG_InitConsoleCommands

Let the client system know about all of our commands
so it can perform tab completion
=================
*/
void CG_InitConsoleCommands( void ) {
	int		i, j;

	for ( i = 0 ; i < cg_numCommands ; i++ ) {
		trap_AddCommand( cg_commands[i].cmd );
	}

	for ( i = 0 ; i < ui_numCommands ; i++ ) {
		trap_AddCommand( ui_commands[i].cmd );
	}

	for ( i = 0 ; i < numPlayerCommands ; i++ ) {
		for ( j = 0; j < CG_MaxSplitView(); j++ ) {
			trap_AddCommand( Com_LocalPlayerCvarName( j, playerCommands[i].cmd ) );
		}
	}

	if ( !cg.connected ) {
		return;
	}

	//
	// the game server will interpret these commands, which will be automatically
	// forwarded to the server after they are not recognized locally
	//
	for (i = 0; i < CG_MaxSplitView(); i++) {
		trap_AddCommand(Com_LocalPlayerCvarName(i, "say"));
		trap_AddCommand(Com_LocalPlayerCvarName(i, "say_team"));
		trap_AddCommand(Com_LocalPlayerCvarName(i, "tell"));
#ifdef MISSIONPACK
		trap_AddCommand(Com_LocalPlayerCvarName(i, "vsay"));
		trap_AddCommand(Com_LocalPlayerCvarName(i, "vsay_team"));
		trap_AddCommand(Com_LocalPlayerCvarName(i, "vtell"));
		trap_AddCommand(Com_LocalPlayerCvarName(i, "vosay"));
		trap_AddCommand(Com_LocalPlayerCvarName(i, "vosay_team"));
		trap_AddCommand(Com_LocalPlayerCvarName(i, "votell"));
		trap_AddCommand(Com_LocalPlayerCvarName(i, "vtaunt"));
#endif
		trap_AddCommand(Com_LocalPlayerCvarName(i, "give"));
		trap_AddCommand(Com_LocalPlayerCvarName(i, "god"));
		trap_AddCommand(Com_LocalPlayerCvarName(i, "notarget"));
		trap_AddCommand(Com_LocalPlayerCvarName(i, "noclip"));
		trap_AddCommand(Com_LocalPlayerCvarName(i, "where"));
		trap_AddCommand(Com_LocalPlayerCvarName(i, "kill"));
		trap_AddCommand(Com_LocalPlayerCvarName(i, "teamtask"));
		trap_AddCommand(Com_LocalPlayerCvarName(i, "levelshot"));
		trap_AddCommand(Com_LocalPlayerCvarName(i, "follow"));
		trap_AddCommand(Com_LocalPlayerCvarName(i, "follownext"));
		trap_AddCommand(Com_LocalPlayerCvarName(i, "followprev"));
		trap_AddCommand(Com_LocalPlayerCvarName(i, "team"));
		trap_AddCommand(Com_LocalPlayerCvarName(i, "callvote"));
		trap_AddCommand(Com_LocalPlayerCvarName(i, "vote"));
		trap_AddCommand(Com_LocalPlayerCvarName(i, "callteamvote"));
		trap_AddCommand(Com_LocalPlayerCvarName(i, "teamvote"));
		trap_AddCommand(Com_LocalPlayerCvarName(i, "setviewpos"));
		trap_AddCommand(Com_LocalPlayerCvarName(i, "stats"));
	}
}
