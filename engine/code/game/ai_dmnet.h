/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2002-2009 Q3Rally Team (Per Thormann - perle@q3rally.com)

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
//

/*****************************************************************************
 * name:		ai_dmnet.h
 *
 * desc:		Quake3 bot AI
 *
 * $Archive: /source/code/botai/ai_chat.c $
 *
 *****************************************************************************/

#define MAX_NODESWITCHES	50

void AIEnter_Intermission(bot_state_t *bs);
void AIEnter_Observer(bot_state_t *bs);
void AIEnter_Respawn(bot_state_t *bs);
void AIEnter_Stand(bot_state_t *bs);
void AIEnter_Seek_ActivateEntity(bot_state_t *bs);
void AIEnter_Seek_NBG(bot_state_t *bs);
void AIEnter_Seek_LTG(bot_state_t *bs);
void AIEnter_Seek_Camp(bot_state_t *bs);
void AIEnter_Battle_Fight(bot_state_t *bs);
void AIEnter_Battle_Chase(bot_state_t *bs);
void AIEnter_Battle_Retreat(bot_state_t *bs);
void AIEnter_Battle_NBG(bot_state_t *bs);
int AINode_Intermission(bot_state_t *bs);
int AINode_Observer(bot_state_t *bs);
int AINode_Respawn(bot_state_t *bs);
int AINode_Stand(bot_state_t *bs);
int AINode_Seek_ActivateEntity(bot_state_t *bs);
int AINode_Seek_NBG(bot_state_t *bs);
int AINode_Seek_LTG(bot_state_t *bs);
int AINode_Battle_Fight(bot_state_t *bs);
int AINode_Battle_Chase(bot_state_t *bs);
int AINode_Battle_Retreat(bot_state_t *bs);
int AINode_Battle_NBG(bot_state_t *bs);

// STONELANCE
void AIEnter_MoveToNextCheckpoint(bot_state_t *bs);
int AINode_MoveToNextCheckpoint(bot_state_t *bs);
// END

void BotResetNodeSwitches(void);
void BotDumpNodeSwitches(bot_state_t *bs);

