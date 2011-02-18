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
/**********************************************************************
	UI_ATOMS.C

	User interface building blocks and support functions.
**********************************************************************/
#include "ui_local.h"

uiStatic_t		uis;
qboolean		m_entersound;		// after a frame, so caching won't disrupt the sound

// these are here so the functions in q_shared.c can link
#ifndef UI_HARD_LINKED

void QDECL Com_Error( int level, const char *error, ... ) {
	va_list		argptr;
	char		text[1024];

	va_start (argptr, error);
	Q_vsnprintf (text, sizeof(text), error, argptr);
	va_end (argptr);

	trap_Error( va("%s", text) );
}

void QDECL Com_Printf( const char *msg, ... ) {
	va_list		argptr;
	char		text[1024];

	va_start (argptr, msg);
	Q_vsnprintf (text, sizeof(text), msg, argptr);
	va_end (argptr);

	trap_Print( va("%s", text) );
}

#endif

/*
=================
UI_ClampCvar
=================
*/
float UI_ClampCvar( float min, float max, float value )
{
	if ( value < min ) return min;
	if ( value > max ) return max;
	return value;
}

/*
=================
UI_StartDemoLoop
=================
*/
void UI_StartDemoLoop( void ) {
	trap_Cmd_ExecuteText( EXEC_APPEND, "d1\n" );
}

/*
=================
UI_PushMenu
=================
*/
void UI_PushMenu( menuframework_s *menu )
{
	int				i;
	menucommon_s*	item;

	// avoid stacking menus invoked by hotkeys
	for (i=0 ; i<uis.menusp ; i++)
	{
		if (uis.stack[i] == menu)
		{
			uis.menusp = i;
			break;
		}
	}

	if (i == uis.menusp)
	{
		if (uis.menusp >= MAX_MENUDEPTH)
			trap_Error("UI_PushMenu: menu stack overflow");

		uis.stack[uis.menusp++] = menu;
	}

	uis.activemenu = menu;

	// default cursor position
	menu->cursor      = 0;
	menu->cursor_prev = 0;

	m_entersound = qtrue;

	trap_Key_SetCatcher( KEYCATCH_UI );

	// force first available item to have focus
	for (i=0; i<menu->nitems; i++)
	{
		item = (menucommon_s *)menu->items[i];
		if (!(item->flags & (QMF_GRAYED|QMF_MOUSEONLY|QMF_INACTIVE)))
		{
			menu->cursor_prev = -1;
			Menu_SetCursor( menu, i );
			break;
		}
	}

	uis.firstdraw = qtrue;
}

/*
=================
UI_PopMenu
=================
*/
void UI_PopMenu (void)
{
	trap_S_StartLocalSound( menu_out_sound, CHAN_LOCAL_SOUND );

	uis.menusp--;

	if (uis.menusp < 0)
		trap_Error ("UI_PopMenu: menu stack underflow");

	if (uis.menusp) {
		uis.activemenu = uis.stack[uis.menusp-1];
		uis.firstdraw = qtrue;
// STONELANCE
		if (uis.activemenu->transition)
			uis.transitionIn = uis.realtime;
// END
	}
	else {
		UI_ForceMenuOff ();
	}
}

void UI_ForceMenuOff (void)
{
	uis.menusp     = 0;
	uis.activemenu = NULL;

	trap_Key_SetCatcher( trap_Key_GetCatcher() & ~KEYCATCH_UI );
	trap_Key_ClearStates();
	trap_Cvar_Set( "cl_paused", "0" );
}

/*
=================
UI_LerpColor
=================
*/
void UI_LerpColor(vec4_t a, vec4_t b, vec4_t c, float t)
{
	int i;

	// lerp and clamp each component
	for (i=0; i<4; i++)
	{
		c[i] = a[i] + t*(b[i]-a[i]);
		if (c[i] < 0)
			c[i] = 0;
		else if (c[i] > 1.0)
			c[i] = 1.0;
	}
}

/*
=================
UI_DrawProportionalString2
=================
*/
static int	propMap[128][3] = {
{0, 0, -1}, {0, 0, -1}, {0, 0, -1}, {0, 0, -1}, {0, 0, -1}, {0, 0, -1}, {0, 0, -1}, {0, 0, -1},
{0, 0, -1}, {0, 0, -1}, {0, 0, -1}, {0, 0, -1}, {0, 0, -1}, {0, 0, -1}, {0, 0, -1}, {0, 0, -1},

{0, 0, -1}, {0, 0, -1}, {0, 0, -1}, {0, 0, -1}, {0, 0, -1}, {0, 0, -1}, {0, 0, -1}, {0, 0, -1},
{0, 0, -1}, {0, 0, -1}, {0, 0, -1}, {0, 0, -1}, {0, 0, -1}, {0, 0, -1}, {0, 0, -1}, {0, 0, -1},

// STONELANCE
{206, 196, PROP_SPACE_WIDTH},		// SPACE
{21, 128, 20},	// !
{53, 192, 13},	// "
{72, 128, 29},	// #
{101, 128, 27},	// $
{128, 128, 27},	// %
{171, 128, 25},	// &
{145, 96, 17},	// '
{223, 128, 15},	// (
{0, 160, 15},	// )
{196, 128, 27},	// *
{15, 160, 23},	// +
{28, 192, 9},	// ,
{162, 96, 21},	// -
{37, 192, 9},	// .
{80, 160, 23},	// /

{151, 64, 25},	// 0
{176, 64, 17},	// 1
{193, 64, 26},	// 2
{219, 64, 25},	// 3
{0, 96, 21},	// 4
{21, 96, 27},	// 5
{48, 96, 25},	// 6
{73, 96, 24},	// 7
{97, 96, 25},	// 8
{122, 96, 23},	// 9
{14, 192, 14},	// :
{0, 192, 14},	// ;
{161, 160, 23},	// <
{183, 96, 23},	// =
{184, 160, 23},	// >
{66, 192, 21},	// ?

{41, 128, 31},	// @
{0, 0, 25},		// A
{25, 0, 25},	// B
{50, 0, 25},	// C
{75, 0, 22},	// D
{97, 0, 26},	// E
{123, 0, 26},	// F
{149, 0, 25},	// G
{174, 0, 25},	// H
{199, 0, 15},	// I
{214, 0, 25},	// J
{0, 32, 25},	// K
{25, 32, 19},	// L
{44, 32, 28},	// M
{72, 32, 25},	// N
{97, 32, 25},	// O

{122, 32, 25},	// P
{147, 32, 25},	// Q
{172, 32, 25},	// R
{197, 32, 27},	// S
{224, 32, 21},	// T
{0, 64, 25},	// U
{25, 64, 24},	// V
{49, 64, 28},	// W
{77, 64, 25},	// X
{102, 64, 23},	// Y
{125, 64, 26},	// Z
{38, 160, 21},	// [
{103, 160, 20},	// '\'
{59, 160, 21},	// ]
{155, 128, 16},	// ^
{87, 192, 19},	// _

{46, 192, 7},	// '
{0, 0, 25},		// A
{25, 0, 25},	// B
{50, 0, 25},	// C
{75, 0, 22},	// D
{97, 0, 26},	// E
{123, 0, 26},	// F
{149, 0, 25},	// G
{174, 0, 25},	// H
{199, 0, 15},	// I
{214, 0, 25},	// J
{0, 32, 25},	// K
{25, 32, 19},	// L
{44, 32, 28},	// M
{72, 32, 25},	// N
{97, 32, 25},	// O

{122, 32, 25},	// P
{147, 32, 25},	// Q
{172, 32, 25},	// R
{197, 32, 27},	// S
{224, 32, 21},	// T
{0, 64, 25},	// U
{25, 64, 24},	// V
{49, 64, 28},	// W
{77, 64, 25},	// X
{102, 64, 23},	// Y
{125, 64, 26},	// Z
{123, 160, 19},	// {
{207, 160, 20},	// |
{142, 160, 19},	// }
{0, 128, 21},	// ~
{0, 0, -1}		// DEL

/*
{0, 0, PROP_SPACE_WIDTH},		// SPACE
{11, 122, 7},	// !
{154, 181, 14},	// "
{55, 122, 17},	// #
{79, 122, 18},	// $
{101, 122, 23},	// %
{153, 122, 18},	// &
{9, 93, 7},		// '
{207, 122, 8},	// (
{230, 122, 9},	// )
{177, 122, 18},	// *
{30, 152, 18},	// +
{85, 181, 7},	// ,
{34, 93, 11},	// -
{110, 181, 6},	// .
{130, 152, 14},	// /

{22, 64, 17},	// 0
{41, 64, 12},	// 1
{58, 64, 17},	// 2
{78, 64, 18},	// 3
{98, 64, 19},	// 4
{120, 64, 18},	// 5
{141, 64, 18},	// 6
{204, 64, 16},	// 7
{162, 64, 17},	// 8
{182, 64, 18},	// 9
{59, 181, 7},	// :
{35,181, 7},	// ;
{203, 152, 14},	// <
{56, 93, 14},	// =
{228, 152, 14},	// >
{177, 181, 18},	// ?

{28, 122, 22},	// @
{5, 4, 18},		// A
{27, 4, 18},	// B
{48, 4, 18},	// C
{69, 4, 17},	// D
{90, 4, 13},	// E
{106, 4, 13},	// F
{121, 4, 18},	// G
{143, 4, 17},	// H
{164, 4, 8},	// I
{175, 4, 16},	// J
{195, 4, 18},	// K
{216, 4, 12},	// L
{230, 4, 23},	// M
{6, 34, 18},	// N
{27, 34, 18},	// O

{48, 34, 18},	// P
{68, 34, 18},	// Q
{90, 34, 17},	// R
{110, 34, 18},	// S
{130, 34, 14},	// T
{146, 34, 18},	// U
{166, 34, 19},	// V
{185, 34, 29},	// W
{215, 34, 18},	// X
{234, 34, 18},	// Y
{5, 64, 14},	// Z
{60, 152, 7},	// [
{106, 151, 13},	// '\'
{83, 152, 7},	// ]
{128, 122, 17},	// ^
{4, 152, 21},	// _

{134, 181, 5},	// '
{5, 4, 18},		// A
{27, 4, 18},	// B
{48, 4, 18},	// C
{69, 4, 17},	// D
{90, 4, 13},	// E
{106, 4, 13},	// F
{121, 4, 18},	// G
{143, 4, 17},	// H
{164, 4, 8},	// I
{175, 4, 16},	// J
{195, 4, 18},	// K
{216, 4, 12},	// L
{230, 4, 23},	// M
{6, 34, 18},	// N
{27, 34, 18},	// O

{48, 34, 18},	// P
{68, 34, 18},	// Q
{90, 34, 17},	// R
{110, 34, 18},	// S
{130, 34, 14},	// T
{146, 34, 18},	// U
{166, 34, 19},	// V
{185, 34, 29},	// W
{215, 34, 18},	// X
{234, 34, 18},	// Y
{5, 64, 14},	// Z
{153, 152, 13},	// {
{11, 181, 5},	// |
{180, 152, 13},	// }
{79, 93, 17},	// ~
{0, 0, -1}		// DEL
*/
// END
};

// STONELANCE
static int propMapB[27][3] = {
{2, 0, 35},		// A
{37, 0, 35},	// B
{72, 0, 35},	// C
{107, 0, 33},	// D
{140, 0, 36},	// E
{176, 0, 36},	// F
{212, 0, 35},	// G

{2, 36, 35},	// H
{37, 36, 21},	// I
{58, 36, 35},	// J
{93, 36, 34},	// K
{127, 36, 28},	// L
{155, 36, 38},	// M
{193, 36, 35},	// N

{2, 72, 35},	// O
{37, 72, 35},	// P
{72, 72, 35},	// Q
{107, 72, 35},	// R
{142, 72, 38},	// S
{180, 72, 31},	// T
{211, 72, 35},	// U

{2, 108, 32},	// V
{34, 108, 39},	// W
{73, 108, 34},	// X
{107, 108, 33},	// Y
{140, 108, 37},	// Z

{194, 108, 35},	// 3

/*
{11, 12, 33},	// A
{49, 12, 31},	// B
{85, 12, 31},	// C
{120, 12, 30},	// D
{156, 12, 21},	// E
{183, 12, 21},	// F
{207, 12, 32},	// G

{13, 55, 30},	// H
{49, 55, 13},	// I
{66, 55, 29},	// J
{101, 55, 31},	// K
{135, 55, 21},	// L
{158, 55, 40},	// M
{204, 55, 32},	// N

{12, 97, 31},	// O
{48, 97, 31},	// P
{82, 97, 30},	// Q
{118, 97, 30},	// R
{153, 97, 30},	// S
{185, 97, 25},	// T
{213, 97, 30},	// U

{11, 139, 32},	// V
{42, 139, 51},	// W
{93, 139, 32},	// X
{126, 139, 31},	// Y
{158, 139, 25},	// Z
*/
};

// #define PROPB_GAP_WIDTH		4
// #define PROPB_SPACE_WIDTH	12
#define PROPB_GAP_WIDTH		-2
#define PROPB_SPACE_WIDTH	18
// END
#define PROPB_HEIGHT		36

/*
=================
UI_DrawBannerString
=================
*/
static void UI_DrawBannerString2( int x, int y, const char* str, vec4_t color )
{
	const char* s;
	char	ch;
	float	ax;
	float	ay;
	float	aw;
	float	ah;
	float	frow;
	float	fcol;
	float	fwidth;
	float	fheight;

	// draw the colored text
	trap_R_SetColor( color );

// STONELANCE
//	ax = x * uis.scale + uis.bias;
//	ay = y * uis.scale;
	ax = x * uis.screenXScale + uis.bias;
	ay = y * uis.screenYScale;
// END

	s = str;
	while ( *s )
	{
		ch = *s & 127;
		if ( ch == ' ' ) {
// STONELANCE
//			ax += ((float)PROPB_SPACE_WIDTH + (float)PROPB_GAP_WIDTH)* uis.scale;
			ax += ((float)PROPB_SPACE_WIDTH + (float)PROPB_GAP_WIDTH)* uis.screenXScale;
// END
		}
// STONELANCE
//		else if ( ch >= 'A' && ch <= 'Z' ) {
//			ch -= 'A';
		else if ( ch >= 'A' && ch <= 'Z' || ch == '3') {
			if ( ch == '3' ) {
				ch = 26;
			}
			else {
				ch -= 'A';
			}
// END
			fcol = (float)propMapB[ch][0] / 256.0f;
			frow = (float)propMapB[ch][1] / 256.0f;
			fwidth = (float)propMapB[ch][2] / 256.0f;
			fheight = (float)PROPB_HEIGHT / 256.0f;
// STONELANCE
//			aw = (float)propMapB[ch][2] * uis.scale;
//			ah = (float)PROPB_HEIGHT * uis.scale;
			aw = (float)propMapB[ch][2] * uis.screenXScale;
			ah = (float)PROPB_HEIGHT * uis.screenYScale;
// END
			trap_R_DrawStretchPic( ax, ay, aw, ah, fcol, frow, fcol+fwidth, frow+fheight, uis.charsetPropB );
// STONELANCE
//			ax += (aw + (float)PROPB_GAP_WIDTH * uis.scale);
			ax += (aw + (float)PROPB_GAP_WIDTH * uis.screenXScale);
// END
		}
		s++;
	}

	trap_R_SetColor( NULL );
}

void UI_DrawBannerString( int x, int y, const char* str, int style, vec4_t color ) {
	const char *	s;
	int				ch;
	int				width;
	vec4_t			drawcolor;

	// find the width of the drawn text
	s = str;
	width = 0;
	while ( *s ) {
		ch = *s;
		if ( ch == ' ' ) {
			width += PROPB_SPACE_WIDTH;
		}
		else if ( ch >= 'A' && ch <= 'Z' ) {
			width += propMapB[ch - 'A'][2] + PROPB_GAP_WIDTH;
		}
		s++;
	}
	width -= PROPB_GAP_WIDTH;

	switch( style & UI_FORMATMASK ) {
		case UI_CENTER:
			x -= width / 2;
			break;

		case UI_RIGHT:
			x -= width;
			break;

		case UI_LEFT:
		default:
			break;
	}

	if ( style & UI_DROPSHADOW ) {
		drawcolor[0] = drawcolor[1] = drawcolor[2] = 0;
		drawcolor[3] = color[3];
		UI_DrawBannerString2( x+2, y+2, str, drawcolor );
	}

	UI_DrawBannerString2( x, y, str, color );
}


int UI_ProportionalStringWidth( const char* str ) {
	const char *	s;
	int				ch;
	int				charWidth;
	int				width;

	s = str;
	width = 0;
	while ( *s ) {
		ch = *s & 127;
		charWidth = propMap[ch][2];
		if ( charWidth != -1 ) {
			width += charWidth;
			width += PROP_GAP_WIDTH;
		}
		s++;
	}

	width -= PROP_GAP_WIDTH;
	return width;
}

static void UI_DrawProportionalString2( int x, int y, const char* str, vec4_t color, float sizeScale, qhandle_t charset )
{
	const char* s;
	char	ch;
	float	ax;
	float	ay;
	float	aw;
	float	ah;
	float	frow;
	float	fcol;
	float	fwidth;
	float	fheight;

	// draw the colored text
	trap_R_SetColor( color );

// STONELANCE
//	ax = x * uis.scale + uis.bias;
//	ay = y * uis.scale;
	ax = x * uis.screenXScale + uis.bias;
	ay = y * uis.screenYScale;
// END

	s = str;
	while ( *s )
	{
		ch = *s & 127;
		if ( ch == ' ' ) {
// STONELANCE
//			aw = (float)PROP_SPACE_WIDTH * uis.scale * sizeScale;
			aw = (float)PROP_SPACE_WIDTH * uis.screenXScale * sizeScale;
// END
		}
		else if ( propMap[ch][2] != -1 ) {
			fcol = (float)propMap[ch][0] / 256.0f;
			frow = (float)propMap[ch][1] / 256.0f;
			fwidth = (float)propMap[ch][2] / 256.0f;
			fheight = (float)PROP_HEIGHT / 256.0f;
// STONELANCE
//			aw = (float)propMap[ch][2] * uis.scale * sizeScale;
//			ah = (float)PROP_HEIGHT * uis.scale * sizeScale;
			aw = (float)propMap[ch][2] * uis.screenXScale * sizeScale;
			ah = (float)PROP_HEIGHT * uis.screenYScale * sizeScale;
// END
			trap_R_DrawStretchPic( ax, ay, aw, ah, fcol, frow, fcol+fwidth, frow+fheight, charset );
		}

// STONELANCE
//		ax += (aw + (float)PROP_GAP_WIDTH * uis.scale * sizeScale);
		ax += (aw + (float)PROP_GAP_WIDTH * uis.screenXScale * sizeScale);
// END
		s++;
	}

	trap_R_SetColor( NULL );
}

/*
=================
UI_ProportionalSizeScale
=================
*/
float UI_ProportionalSizeScale( int style ) {
	if(  style & UI_SMALLFONT ) {
		return PROP_SMALL_SIZE_SCALE;
	}

// STONELANCE
//	return 1.00;
	return PROP_NORMAL_SIZE_SCALE;
// END
}


/*
=================
UI_DrawProportionalString
=================
*/
void UI_DrawProportionalString( int x, int y, const char* str, int style, vec4_t color ) {
	vec4_t	drawcolor;
	int		width;
	float	sizeScale;

	sizeScale = UI_ProportionalSizeScale( style );

	switch( style & UI_FORMATMASK ) {
		case UI_CENTER:
			width = UI_ProportionalStringWidth( str ) * sizeScale;
			x -= width / 2;
			break;

		case UI_RIGHT:
			width = UI_ProportionalStringWidth( str ) * sizeScale;
			x -= width;
			break;

		case UI_LEFT:
		default:
			break;
	}

	if ( style & UI_DROPSHADOW ) {
		drawcolor[0] = drawcolor[1] = drawcolor[2] = 0;
		drawcolor[3] = color[3];
		UI_DrawProportionalString2( x+2, y+2, str, drawcolor, sizeScale, uis.charsetProp );
	}

	if ( style & UI_INVERSE ) {
		drawcolor[0] = color[0] * 0.7;
		drawcolor[1] = color[1] * 0.7;
		drawcolor[2] = color[2] * 0.7;
		drawcolor[3] = color[3];
		UI_DrawProportionalString2( x, y, str, drawcolor, sizeScale, uis.charsetProp );
		return;
	}

	if ( style & UI_PULSE ) {
		drawcolor[0] = color[0] * 0.7;
		drawcolor[1] = color[1] * 0.7;
		drawcolor[2] = color[2] * 0.7;
		drawcolor[3] = color[3];
		UI_DrawProportionalString2( x, y, str, color, sizeScale, uis.charsetProp );

// STONELANCE
/*
		drawcolor[0] = color[0];
		drawcolor[1] = color[1];
		drawcolor[2] = color[2];
*/
		drawcolor[0] = text_color_highlight[0];
		drawcolor[1] = text_color_highlight[1];
		drawcolor[2] = text_color_highlight[2];
// END
		drawcolor[3] = 0.5 + 0.5 * sin( uis.realtime / PULSE_DIVISOR );
		UI_DrawProportionalString2( x, y, str, drawcolor, sizeScale, uis.charsetPropGlow );
		return;
	}

	UI_DrawProportionalString2( x, y, str, color, sizeScale, uis.charsetProp );
}

/*
=================
UI_DrawString2
=================
*/
static void UI_DrawString2( int x, int y, const char* str, vec4_t color, int charw, int charh )
{
	const char* s;
	char	ch;
	int forceColor = qfalse; //APSFIXME;
	vec4_t	tempcolor;
	float	ax;
	float	ay;
	float	aw;
	float	ah;
	float	frow;
	float	fcol;

	if (y < -charh)
		// offscreen
		return;

	// draw the colored text
	trap_R_SetColor( color );
	
// STONELANCE - move font closer together
	x -= 1;
// END
// STONELANCE
/*
	ax = x * uis.scale + uis.bias;
	ay = y * uis.scale;
	aw = charw * uis.scale;
	ah = charh * uis.scale;
*/
	ax = x * uis.screenXScale + uis.bias;
	ay = y * uis.screenYScale;
	aw = charw * uis.screenXScale;
	ah = charh * uis.screenYScale;
// END

	s = str;
	while ( *s )
	{
		if ( Q_IsColorString( s ) )
		{
			if ( !forceColor )
			{
				memcpy( tempcolor, g_color_table[ColorIndex(s[1])], sizeof( tempcolor ) );
				tempcolor[3] = color[3];
				trap_R_SetColor( tempcolor );
			}
			s += 2;
			continue;
		}

		ch = *s & 255;
		if (ch != ' ')
		{
			frow = (ch>>4)*0.0625;
			fcol = (ch&15)*0.0625;
			trap_R_DrawStretchPic( ax, ay, aw, ah, fcol, frow, fcol + 0.0625, frow + 0.0625, uis.charset );
		}

// STONELANCE - move font closer together
//		ax += aw;
		ax += aw - 2 * uis.screenXScale;
// END
		s++;
	}

	trap_R_SetColor( NULL );
}

/*
=================
UI_DrawString
=================
*/
void UI_DrawString( int x, int y, const char* str, int style, vec4_t color )
{
	int		len;
	int		charw;
	int		charh;
	vec4_t	newcolor;
	vec4_t	lowlight;
	float	*drawcolor;
	vec4_t	dropcolor;

	if( !str ) {
		return;
	}

	if ((style & UI_BLINK) && ((uis.realtime/BLINK_DIVISOR) & 1))
		return;

	if (style & UI_SMALLFONT)
	{
		charw =	SMALLCHAR_WIDTH;
		charh =	SMALLCHAR_HEIGHT;
	}
	else if (style & UI_GIANTFONT)
	{
		charw =	GIANTCHAR_WIDTH;
		charh =	GIANTCHAR_HEIGHT;
	}
	else
	{
		charw =	BIGCHAR_WIDTH;
		charh =	BIGCHAR_HEIGHT;
	}

	if (style & UI_PULSE)
	{
		lowlight[0] = 0.8*color[0]; 
		lowlight[1] = 0.8*color[1];
		lowlight[2] = 0.8*color[2];
		lowlight[3] = 0.8*color[3];
		UI_LerpColor(color,lowlight,newcolor,0.5+0.5*sin(uis.realtime/PULSE_DIVISOR));
		drawcolor = newcolor;
	}	
	else
		drawcolor = color;

	switch (style & UI_FORMATMASK)
	{
		case UI_CENTER:
			// center justify at x
			len = strlen(str);
			x   = x - len*charw/2;
			break;

		case UI_RIGHT:
			// right justify at x
			len = strlen(str);
			x   = x - len*charw;
			break;

		default:
			// left justify at x
			break;
	}

	if ( style & UI_DROPSHADOW )
	{
		dropcolor[0] = dropcolor[1] = dropcolor[2] = 0;
		dropcolor[3] = drawcolor[3];
// STONELANCE
		if (style & UI_SMALLFONT)
			UI_DrawString2(x+2,y+2,str,dropcolor,8,charh);
		else
// END
		UI_DrawString2(x+2,y+2,str,dropcolor,charw,charh);
	}

// STONELANCE
	if (style & UI_SMALLFONT){
		UI_DrawString2(x,y,str,drawcolor,8,charh);
	}
	else
// END
	UI_DrawString2(x,y,str,drawcolor,charw,charh);
}

/*
=================
UI_DrawChar
=================
*/
void UI_DrawChar( int x, int y, int ch, int style, vec4_t color )
{
	char	buff[2];

	buff[0] = ch;
	buff[1] = '\0';

	UI_DrawString( x, y, buff, style, color );
}

qboolean UI_IsFullscreen( void ) {
	if ( uis.activemenu && ( trap_Key_GetCatcher() & KEYCATCH_UI ) ) {
		return uis.activemenu->fullscreen;
	}

	return qfalse;
}

static void NeedCDAction( qboolean result ) {
	if ( !result ) {
		trap_Cmd_ExecuteText( EXEC_APPEND, "quit\n" );
	}
}

static void NeedCDKeyAction( qboolean result ) {
	if ( !result ) {
		trap_Cmd_ExecuteText( EXEC_APPEND, "quit\n" );
	}
}


void UI_SetActiveMenu( uiMenuCommand_t menu ) {
	// this should be the ONLY way the menu system is brought up
	// enusure minumum menu data is cached
	Menu_Cache();

	switch ( menu ) {
	case UIMENU_NONE:
		UI_ForceMenuOff();
		return;
	case UIMENU_MAIN:
// STONELANCE
//		UI_MainMenu();
		UI_GFX_Loading();
// END
		return;
	case UIMENU_NEED_CD:
		UI_ConfirmMenu( "Insert the CD", NULL, NeedCDAction );
		return;
	case UIMENU_BAD_CD_KEY:
		UI_ConfirmMenu( "Bad CD Key", NULL, NeedCDKeyAction );
		return;
	case UIMENU_INGAME:
		/*
		//GRank
		UI_RankingsMenu();
		return;
		*/
		trap_Cvar_Set( "cl_paused", "1" );
		UI_InGameMenu();
		return;
	}
}

/*
=================
UI_KeyEvent
=================
*/
void UI_KeyEvent( int key, int down ) {
	sfxHandle_t		s;

	if (!uis.activemenu) {
		return;
	}

	if (!down) {
		return;
	}

// STONELANCE - FIXME: disable keys during transition
	if (uis.transitionIn || uis.transitionOut)
		return;
// END

	if (uis.activemenu->key)
		s = uis.activemenu->key( key );
	else
		s = Menu_DefaultKey( uis.activemenu, key );

	if ((s > 0) && (s != menu_null_sound))
		trap_S_StartLocalSound( s, CHAN_LOCAL_SOUND );
}

/*
=================
UI_MouseEvent
=================
*/
void UI_MouseEvent( int dx, int dy )
{
	int				i;
	menucommon_s*	m;

	if (!uis.activemenu)
		return;

	// update mouse screen position
// STONELANCE
	uis.cursorpx = uis.cursorx;
	uis.cursorpy = uis.cursory;
// END

	uis.cursorx += dx;
	if (uis.cursorx < 0)
		uis.cursorx = 0;
	else if (uis.cursorx > SCREEN_WIDTH)
		uis.cursorx = SCREEN_WIDTH;

	uis.cursory += dy;
	if (uis.cursory < 0)
		uis.cursory = 0;
	else if (uis.cursory > SCREEN_HEIGHT)
		uis.cursory = SCREEN_HEIGHT;

	// region test the active menu items
	for (i=0; i<uis.activemenu->nitems; i++)
	{
		m = (menucommon_s*)uis.activemenu->items[i];

		if (m->flags & (QMF_GRAYED|QMF_INACTIVE))
			continue;

		if ((uis.cursorx < m->left) ||
			(uis.cursorx > m->right) ||
			(uis.cursory < m->top) ||
			(uis.cursory > m->bottom))
		{
			// cursor out of item bounds
			continue;
		}

		// set focus to item at cursor
		if (uis.activemenu->cursor != i)
		{
			Menu_SetCursor( uis.activemenu, i );
			((menucommon_s*)(uis.activemenu->items[uis.activemenu->cursor_prev]))->flags &= ~QMF_HASMOUSEFOCUS;

// STONELANCE - remove sound
/*
			if ( !(((menucommon_s*)(uis.activemenu->items[uis.activemenu->cursor]))->flags & QMF_SILENT ) ) {
				trap_S_StartLocalSound( menu_move_sound, CHAN_LOCAL_SOUND );
			}
*/
// END
		}

		((menucommon_s*)(uis.activemenu->items[uis.activemenu->cursor]))->flags |= QMF_HASMOUSEFOCUS;
		return;
	}  

	if (uis.activemenu->nitems > 0) {
		// out of any region
		((menucommon_s*)(uis.activemenu->items[uis.activemenu->cursor]))->flags &= ~QMF_HASMOUSEFOCUS;
	}
}

char *UI_Argv( int arg ) {
	static char	buffer[MAX_STRING_CHARS];

	trap_Argv( arg, buffer, sizeof( buffer ) );

	return buffer;
}


char *UI_Cvar_VariableString( const char *var_name ) {
	static char	buffer[MAX_STRING_CHARS];

	trap_Cvar_VariableStringBuffer( var_name, buffer, sizeof( buffer ) );

	return buffer;
}


/*
=================
UI_Cache
=================
*/
void UI_Cache_f( void ) {
	MainMenu_Cache();
	InGame_Cache();
	ConfirmMenu_Cache();
	PlayerModel_Cache();
	PlayerSettings_Cache();
	Controls_Cache();
	Demos_Cache();
	UI_CinematicsMenu_Cache();
	Preferences_Cache();
	ServerInfo_Cache();
	SpecifyServer_Cache();
	ArenaServers_Cache();
	StartServer_Cache();
	ServerOptions_Cache();
	DriverInfo_Cache();
	GraphicsOptions_Cache();
	UI_DisplayOptionsMenu_Cache();
	UI_SoundOptionsMenu_Cache();
	UI_NetworkOptionsMenu_Cache();
	UI_SPLevelMenu_Cache();
	UI_SPSkillMenu_Cache();
	UI_SPPostgameMenu_Cache();
	TeamMain_Cache();
	UI_AddBots_Cache();
	UI_RemoveBots_Cache();
	UI_SetupMenu_Cache();
//	UI_LoadConfig_Cache();
//	UI_SaveConfigMenu_Cache();
	UI_BotSelectMenu_Cache();
	UI_CDKeyMenu_Cache();
	UI_ModsMenu_Cache();
}


/*
=================
UI_ConsoleCommand
=================
*/
qboolean UI_ConsoleCommand( int realTime ) {
	char	*cmd;

	cmd = UI_Argv( 0 );

	// ensure minimum menu data is available
// STONELANCE
//	Menu_Cache();
// END

	if ( Q_stricmp (cmd, "levelselect") == 0 ) {
		UI_SPLevelMenu_f();
		return qtrue;
	}

	if ( Q_stricmp (cmd, "postgame") == 0 ) {
		UI_SPPostgameMenu_f();
		return qtrue;
	}

	if ( Q_stricmp (cmd, "ui_cache") == 0 ) {
		UI_Cache_f();
		return qtrue;
	}

	if ( Q_stricmp (cmd, "ui_cinematics") == 0 ) {
		UI_CinematicsMenu_f();
		return qtrue;
	}

	if ( Q_stricmp (cmd, "ui_teamOrders") == 0 ) {
		UI_TeamOrdersMenu_f();
		return qtrue;
	}

	if ( Q_stricmp (cmd, "iamacheater") == 0 ) {
		UI_SPUnlock_f();
		return qtrue;
	}

	if ( Q_stricmp (cmd, "iamamonkey") == 0 ) {
		UI_SPUnlockMedals_f();
		return qtrue;
	}

	if ( Q_stricmp (cmd, "ui_cdkey") == 0 ) {
		UI_CDKeyMenu_f();
		return qtrue;
	}

	return qfalse;
}

/*
=================
UI_Shutdown
=================
*/
void UI_Shutdown( void ) {
}

/*
=================
UI_Init
=================
*/
void UI_Init( void ) {
// STONELANCE
	fileHandle_t	f;
	char			modelname[MAX_QPATH];
	char			*slash;
// END

	UI_RegisterCvars();

	UI_InitGameinfo();


	// cache redundant calulations
	trap_GetGlconfig( &uis.glconfig );

	// for 640x480 virtualized screen
// STONELANCE
//	uis.scale = uis.glconfig.vidHeight * (1.0/480.0);
	uis.screenXScale = uis.glconfig.vidWidth * (1.0/640.0);
	uis.screenYScale = uis.glconfig.vidHeight * (1.0/480.0);
// END
	if ( uis.glconfig.vidWidth * 480 > uis.glconfig.vidHeight * 640 ) {
		// wide screen
		uis.bias = 0.5 * ( uis.glconfig.vidWidth - ( uis.glconfig.vidHeight * (640.0/480.0) ) );
	}
	else {
		// no wide screen
		uis.bias = 0;
	}

// STONELANCE
	trap_Cvar_VariableStringBuffer( "head", modelname, sizeof( modelname ) );

	// load cvars over top of the crap from q3 if loaded from mod menu
	if (modelname[0] == 0)
		trap_Cmd_ExecuteText( EXEC_NOW, "exec q3config.cfg" );

	// check for head again
	trap_Cvar_VariableStringBuffer( "head", modelname, sizeof( modelname ) );
	if (modelname[0] == 0)
	{
		trap_Cvar_Set( "head", "doom" );
	}

	// check for head again
	trap_Cvar_VariableStringBuffer( "rim", modelname, sizeof( modelname ) );
	if (modelname[0] == 0)
	{
		trap_Cvar_Set( "rim", "svt_cobra" );
	}

	// check for head again
	trap_Cvar_VariableStringBuffer( "plate", modelname, sizeof( modelname ) );
	if (modelname[0] == 0)
	{
		trap_Cvar_Set( "plate", "usa_california" );
	}

	// check to see that the player is a valid model
	trap_Cvar_VariableStringBuffer( "model", modelname, sizeof( modelname ) );
	slash = strchr(modelname, '/');
	if ( slash )
		*slash = 0;

	trap_FS_FOpenFile( va("models/players/%s/body.md3", modelname), &f, FS_READ );
	if ( !f )
		trap_Cvar_Set( "model", "sidepipe/red" );
	else
		trap_FS_FCloseFile( f );
// END

	// initialize the menu system
	Menu_Cache();


	uis.activemenu = NULL;
	uis.menusp     = 0;
}

/*
================
UI_AdjustFrom640

Adjusted for resolution and screen aspect ratio
================
*/
void UI_AdjustFrom640( float *x, float *y, float *w, float *h ) {
	// expect valid pointers
// STONELANCE
/*
	*x = *x * uis.scale + uis.bias;
	*y *= uis.scale;
	*w *= uis.scale;
	*h *= uis.scale;
*/
	*x = *x * uis.screenXScale + uis.bias;
	*y *= uis.screenYScale;
	*w *= uis.screenXScale;
	*h *= uis.screenYScale;
// END
}

void UI_DrawNamedPic( float x, float y, float width, float height, const char *picname ) {
	qhandle_t	hShader;

	hShader = trap_R_RegisterShaderNoMip( picname );
	UI_AdjustFrom640( &x, &y, &width, &height );
	trap_R_DrawStretchPic( x, y, width, height, 0, 0, 1, 1, hShader );
}

void UI_DrawHandlePic( float x, float y, float w, float h, qhandle_t hShader ) {
	float	s0;
	float	s1;
	float	t0;
	float	t1;

	if( w < 0 ) {	// flip about vertical
		w  = -w;
		s0 = 1;
		s1 = 0;
	}
	else {
		s0 = 0;
		s1 = 1;
	}

	if( h < 0 ) {	// flip about horizontal
		h  = -h;
		t0 = 1;
		t1 = 0;
	}
	else {
		t0 = 0;
		t1 = 1;
	}
	
	UI_AdjustFrom640( &x, &y, &w, &h );
	trap_R_DrawStretchPic( x, y, w, h, s0, t0, s1, t1, hShader );
}

/*
================
UI_FillRect

Coordinates are 640*480 virtual values
=================
*/
void UI_FillRect( float x, float y, float width, float height, const float *color ) {
	trap_R_SetColor( color );

	UI_AdjustFrom640( &x, &y, &width, &height );
	trap_R_DrawStretchPic( x, y, width, height, 0, 0, 0, 0, uis.whiteShader );

	trap_R_SetColor( NULL );
}

/*
================
UI_DrawRect

Coordinates are 640*480 virtual values
=================
*/
void UI_DrawRect( float x, float y, float width, float height, const float *color ) {
	trap_R_SetColor( color );

	UI_AdjustFrom640( &x, &y, &width, &height );

	trap_R_DrawStretchPic( x, y, width, 1, 0, 0, 0, 0, uis.whiteShader );
	trap_R_DrawStretchPic( x, y, 1, height, 0, 0, 0, 0, uis.whiteShader );
	trap_R_DrawStretchPic( x, y + height - 1, width, 1, 0, 0, 0, 0, uis.whiteShader );
	trap_R_DrawStretchPic( x + width - 1, y, 1, height, 0, 0, 0, 0, uis.whiteShader );

	trap_R_SetColor( NULL );
}

void UI_SetColor( const float *rgba ) {
	trap_R_SetColor( rgba );
}

void UI_UpdateScreen( void ) {
	trap_UpdateScreen();
}


// STONELANCE
/*
=================
UI_DrawCursor
=================
*/
qboolean UI_DrawCursor( float x, float y, float w, float h, qhandle_t c ){
	refdef_t		refdef;
	refEntity_t		cursor;
	vec3_t			mins = {-12, -12, -12};
	vec3_t			maxs = {12, 12, 12};
	vec3_t			origin;
	vec3_t			angles;
	float			len;

	UI_AdjustFrom640( &x, &y, &w, &h );

	memset( &refdef, 0, sizeof( refdef ) );
	memset( &cursor, 0, sizeof( cursor ) );

	// Setup refdef
	refdef.rdflags = RDF_NOWORLDMODEL;
	AxisClear( refdef.viewaxis );

	refdef.x = x;
	refdef.y = y;
	refdef.width = w;
	refdef.height = h;

	refdef.fov_x = 90;
	refdef.fov_y = atan2( refdef.height, refdef.width / tan( refdef.fov_x / 360 * M_PI ) );
	refdef.fov_y *= ( 360 / M_PI );

	// calculate distance so the cursor nearly fills the box
	len = 0.7 * ( maxs[2] - mins[2] );
	origin[0] = len / tan( DEG2RAD(refdef.fov_x) * 0.5 );
	origin[1] = 0.5 * ( mins[1] + maxs[1] );
	origin[2] = -0.5 * ( mins[2] + maxs[2] );

	refdef.time = uis.realtime;

	trap_R_ClearScene();

	// add cursor
	cursor.hModel = c;
	if (!cursor.hModel) {
		return qfalse;
	}

//	cursor.customShader = s;
//	cursor.customShader = trap_R_RegisterShaderNoMip( "menu/models/wrench" );

	VectorClear(angles);
	angles[YAW] = 180;
//	angles[YAW] = 360 * ( (uis.realtime / 4) & 1023 ) / -1024.0f;
	AnglesToAxis(angles, cursor.axis);

	VectorCopy( origin, cursor.origin );

	VectorCopy( origin, cursor.lightingOrigin );
	cursor.renderfx = RF_LIGHTING_ORIGIN | RF_NOSHADOW;
	VectorCopy (cursor.origin, cursor.oldorigin);

	trap_R_AddRefEntityToScene( &cursor );

	trap_R_RenderScene( &refdef );

	return qtrue;
}
// END


/*
=================
UI_Refresh
=================
*/
void UI_Refresh( int realtime )
{
	uis.frametime = realtime - uis.realtime;
	uis.realtime  = realtime;

	if ( !( trap_Key_GetCatcher() & KEYCATCH_UI ) ) {
		return;
	}

	UI_UpdateCvars();

	if ( uis.activemenu )
	{
		if (uis.activemenu->fullscreen)
		{
			// draw the background
			if( uis.activemenu->showlogo ) {
				UI_DrawHandlePic( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, uis.menuBackShader );
			}
			else {
				UI_DrawHandlePic( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, uis.menuBackNoLogoShader );
			}
		}

		if (uis.activemenu->transparent){
			if (uis.menusp > 1){
				// draw the background pic
				if (uis.stack[uis.menusp-2]->fullscreen){
					if( uis.stack[uis.menusp-2]->showlogo )
						UI_DrawHandlePic( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, uis.menuBackShader );
					else
						UI_DrawHandlePic( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, uis.menuBackNoLogoShader );
				}

				// draw the background menu
				if (uis.stack[uis.menusp-2]->draw)
					uis.stack[uis.menusp-2]->draw();
				else
					Menu_Draw( uis.stack[uis.menusp-2] );

				// dim the background menu
				UI_FillRect( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, menu_back_color );
			}
		}

		if (uis.activemenu->draw)
			uis.activemenu->draw();
		else
			Menu_Draw( uis.activemenu );

		if( uis.firstdraw ) {
			UI_MouseEvent( 0, 0 );
			uis.firstdraw = qfalse;
		}
	}
//=================
//  draw cursor
//=================

//  UI_SetColor( NULL );
//  UI_DrawCursor( uis.cursorx-24, uis.cursory-24, 48, 48, uis.cursorModel );

UI_SetColor( NULL );
UI_DrawHandlePic( uis.cursorx-16, uis.cursory-16, 32, 32, uis.cursor);

//	if (!UI_DrawCursor( uis.cursorx-24, uis.cursory-24, 48, 48, uis.cursorModel )){
//  UI_SetColor( NULL );
//  UI_DrawHandlePic( uis.cursorx-16, uis.cursory-16, 32, 32, uis.cursor);
//	}



#ifndef NDEBUG
	if (uis.debug)
	{
		// cursor coordinates
		UI_DrawString( 0, 0, va("(%d,%d)",uis.cursorx,uis.cursory), UI_LEFT|UI_SMALLFONT, colorRed );
	}
#endif

	// delay playing the enter sound until after the
	// menu has been drawn, to avoid delay while
	// caching images
	if (m_entersound)
	{
		trap_S_StartLocalSound( menu_in_sound, CHAN_LOCAL_SOUND );
		m_entersound = qfalse;
	}
}

void UI_DrawTextBox (int x, int y, int width, int lines)
{
	UI_FillRect( x + BIGCHAR_WIDTH/2, y + BIGCHAR_HEIGHT/2, ( width + 1 ) * BIGCHAR_WIDTH, ( lines + 1 ) * BIGCHAR_HEIGHT, colorBlack );
	UI_DrawRect( x + BIGCHAR_WIDTH/2, y + BIGCHAR_HEIGHT/2, ( width + 1 ) * BIGCHAR_WIDTH, ( lines + 1 ) * BIGCHAR_HEIGHT, colorWhite );
}

qboolean UI_CursorInRect (int x, int y, int width, int height)
{
	if (uis.cursorx < x ||
		uis.cursory < y ||
		uis.cursorx > x+width ||
		uis.cursory > y+height)
		return qfalse;

	return qtrue;
}
