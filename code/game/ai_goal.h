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
 * name:		ai_goal.h
 *
 * desc:		goal AI
 *
 * $Archive: /source/code/game/ai_goal.h $
 *
 *****************************************************************************/

#define MAX_AVOIDGOALS			256
#define MAX_GOALSTACK			8

#define GFL_NONE				0
#define GFL_ITEM				1
#define GFL_ROAM				2
#define GFL_DROPPED				4
#define GFL_AIR					8

//a bot goal
typedef struct bot_goal_s
{
	vec3_t origin;				//origin of the goal
	int areanum;				//area number of the goal
	vec3_t mins, maxs;			//mins and maxs of the goal
	int entitynum;				//number of the goal entity
	int number;					//goal number
	int flags;					//goal flags
	int iteminfo;				//item information
} bot_goal_t;

//goal state
typedef struct bot_goalstate_s
{
	struct weightconfig_s *itemweightconfig;	//weight config
	int itemweightindex[MAX_ITEMS];				//index from item to weight
	//
	int playernum;								//player using this goal state
	int lastreachabilityarea;					//last area with reachabilities the bot was in
	//
	bot_goal_t goalstack[MAX_GOALSTACK];		//goal stack
	int goalstacktop;							//the top of the goal stack
	//
	int avoidgoals[MAX_AVOIDGOALS];				//goals to avoid
	float avoidgoaltimes[MAX_AVOIDGOALS];		//times to avoid the goals
} bot_goalstate_t;

//reset the whole goal state, but keep the item weights
void BotResetGoalState(int goalstate);
//reset avoid goals
void BotResetAvoidGoals(int goalstate);
//remove the goal with the given number from the avoid goals
void BotRemoveFromAvoidGoals(int goalstate, int number);
//push a goal onto the goal stack
void BotPushGoal(int goalstate, bot_goal_t *goal);
//pop a goal from the goal stack
void BotPopGoal(int goalstate);
//empty the bot's goal stack
void BotEmptyGoalStack(int goalstate);
//dump the avoid goals
void BotDumpAvoidGoals(int goalstate);
//dump the goal stack
void BotDumpGoalStack(int goalstate);
//get the name name of the goal with the given number
void BotGoalName(int number, char *name, int size);
//get the top goal from the stack
int BotGetTopGoal(int goalstate, bot_goal_t *goal);
//get the second goal on the stack
int BotGetSecondGoal(int goalstate, bot_goal_t *goal);
//choose the best long term goal item for the bot
int BotChooseLTGItem(int goalstate, vec3_t origin, int *inventory, int travelflags);
//choose the best nearby goal item for the bot
//the item may not be further away from the current bot position than maxtime
//also the travel time from the nearby goal towards the long term goal may not
//be larger than the travel time towards the long term goal from the current bot position
int BotChooseNBGItem(int goalstate, vec3_t origin, int *inventory, int travelflags,
							bot_goal_t *ltg, float maxtime);
//returns true if the bot touches the goal
int BotTouchingGoal(vec3_t origin, bot_goal_t *goal);
//returns true if the goal should be visible but isn't
int BotItemGoalInVisButNotVisible(int viewer, vec3_t eye, vec3_t viewangles, bot_goal_t *goal);
//search for a goal for the given classname, the index can be used
//as a start point for the search when multiple goals are available with that same classname
int BotGetLevelItemGoal(int index, char *classname, bot_goal_t *goal);
//get the next camp spot in the map
int BotGetNextCampSpotGoal(int num, bot_goal_t *goal);
//get the map location with the given name
int BotGetMapLocationGoal(char *name, bot_goal_t *goal);
//returns the avoid goal time
float BotAvoidGoalTime(int goalstate, int number);
//set the avoid goal time
void BotSetAvoidGoalTime(int goalstate, int number, float avoidtime);
//initializes the items in the level
void BotInitLevelItems(void);
//regularly update dynamic entity items (dropped weapons, flags etc.)
void BotUpdateEntityItems(void);
//interbreed the goal fuzzy logic
void BotInterbreedGoalFuzzyLogic(int parent1, int parent2, int child);
//save the goal fuzzy logic to disk
void BotSaveGoalFuzzyLogic(int goalstate, char *filename);
//mutate the goal fuzzy logic
void BotMutateGoalFuzzyLogic(int goalstate, float range);
//loads item weights for the bot
int BotLoadItemWeights(int goalstate, char *filename);
//frees the item weights of the bot
void BotFreeItemWeights(int goalstate);
//returns the handle of a newly allocated goal state
int BotAllocGoalState(int player);
//free the given goal state
void BotFreeGoalState(int handle);
//setup the goal AI
int BotSetupGoalAI(void);
//shut down the goal AI
void BotShutdownGoalAI(void);
