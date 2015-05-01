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

/*****************************************************************************
 * name:		ai_ea.h
 *
 * desc:		elementary actions
 *
 * $Archive: /source/code/game/ai_ea.h $
 *
 *****************************************************************************/

#ifndef __AI_EA_H__
#define __AI_EA_H__

//action flags
#define ACTION_ATTACK			0x00000001
#define ACTION_USE			0x00000002
#define ACTION_RESPAWN			0x00000008
#define ACTION_JUMP			0x00000010
#define ACTION_MOVEUP			0x00000020
#define ACTION_CROUCH			0x00000080
#define ACTION_MOVEDOWN			0x00000100
#define ACTION_MOVEFORWARD		0x00000200
#define ACTION_MOVEBACK			0x00000800
#define ACTION_MOVELEFT			0x00001000
#define ACTION_MOVERIGHT		0x00002000
#define ACTION_DELAYEDJUMP		0x00008000
#define ACTION_TALK			0x00010000
#define ACTION_GESTURE			0x00020000
#define ACTION_WALK			0x00080000
#define ACTION_AFFIRMATIVE		0x00100000
#define ACTION_NEGATIVE			0x00200000
#define ACTION_GETFLAG			0x00800000
#define ACTION_GUARDBASE		0x01000000
#define ACTION_PATROL			0x02000000
#define ACTION_FOLLOWME			0x08000000
#define ACTION_JUMPEDLASTFRAME		0x10000000

//the bot input, will be converted to a usercmd_t
typedef struct bot_input_s
{
	float thinktime;		//time since last output (in seconds)
	vec3_t dir;				//movement direction
	float speed;			//speed in the range [0, 400]
	vec3_t viewangles;		//the view angles
	int actionflags;		//one of the ACTION_? flags
	int weapon;				//weapon to use
} bot_input_t;

//ClientCommand elementary actions
void EA_Say(int playerNum, char *str);
void EA_SayTeam(int playerNum, char *str);
void EA_Command(int playerNum, char *command );

void EA_Action(int playerNum, int action);
void EA_Crouch(int playerNum);
void EA_MoveUp(int playerNum);
void EA_MoveDown(int playerNum);
void EA_MoveForward(int playerNum);
void EA_MoveBack(int playerNum);
void EA_MoveLeft(int playerNum);
void EA_MoveRight(int playerNum);
void EA_Attack(int playerNum);
void EA_Respawn(int playerNum);
void EA_Talk(int playerNum);
void EA_Gesture(int playerNum);
void EA_Use(int playerNum);

//regular elementary actions
void EA_SelectWeapon(int playerNum, int weapon);
void EA_Jump(int playerNum);
void EA_DelayedJump(int playerNum);
void EA_Move(int playerNum, vec3_t dir, float speed);
void EA_View(int playerNum, vec3_t viewangles);

//send regular input to the server
void EA_EndRegular(int playerNum, float thinktime);
void EA_GetInput(int playerNum, float thinktime, bot_input_t *input);
void EA_ResetInput(int playerNum);
//setup and shutdown routines
int EA_Setup(void);
void EA_Shutdown(void);

#endif // __AI_EA_H__
