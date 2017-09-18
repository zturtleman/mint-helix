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
#include "cg_local.h"
#include "../qcommon/q_unicode.h"

static vec4_t lastTextColor = { 0, 0, 0, 1 };

/*
Load fonts for CGame and UI usage.

Try to load TrueType/OpenType fonts specified by cvars. Falls back to
original Quake 3 fonts.

cgame loads an additional font for the console.
q3_ui loads additional fonts (font1_prop, font1_prop_glo, font2_prop).
Team Arena menu/hud also load separate fonts (specified in .menu files).

Q3 gfx/2d/bigchars.tga bitmap font looks like Helvetica or Arial.
Q3 number font, font1_prop, and font2_prop look similar to Impact Wide.
Team Arena's pre-rendered menu/hud TrueType fonts (fonts/smallfont:12,
fonts/font:16, and fonts/bigfont:20) are Impact.
*/
void CG_HudTextInit( void ) {
	if ( !CG_InitTrueTypeFont( cg_hudFont.string, TINYCHAR_HEIGHT, cg_hudFontBorder.value*0.5f, &cgs.media.tinyFont ) ) {
		CG_InitBitmapFont( &cgs.media.tinyFont, TINYCHAR_HEIGHT, TINYCHAR_WIDTH );
	}

	if ( !CG_InitTrueTypeFont( cg_hudFont.string, SMALLCHAR_HEIGHT, cg_hudFontBorder.value, &cgs.media.smallFont ) ) {
		CG_InitBitmapFont( &cgs.media.smallFont, SMALLCHAR_HEIGHT, SMALLCHAR_WIDTH );
	}

	if ( !CG_InitTrueTypeFont( cg_hudFont.string, BIGCHAR_HEIGHT, cg_hudFontBorder.value, &cgs.media.textFont ) ) {
		CG_InitBitmapFont( &cgs.media.textFont, BIGCHAR_HEIGHT, BIGCHAR_WIDTH );
	}

	if ( !CG_InitTrueTypeFont( cg_hudFont.string, GIANTCHAR_HEIGHT, cg_hudFontBorder.value*2.0f, &cgs.media.bigFont ) ) {
		CG_InitBitmapFont( &cgs.media.bigFont, GIANTCHAR_HEIGHT, GIANTCHAR_WIDTH );
	}

	if ( !CG_InitTrueTypeFont( cg_numberFont.string, CHAR_HEIGHT, cg_numberFontBorder.value, &cgs.media.numberFont ) ) {
		CG_InitBitmapNumberFont( &cgs.media.numberFont, CHAR_HEIGHT, CHAR_WIDTH );
	}
}

// 256x256 image with characters that are 16x16
void CG_InitBitmapFont( fontInfo_t *font, int charHeight, int charWidth ) {
	int			i, col, row;
	const char 	*shaderName;
	qhandle_t	hShader;
	float		aspect;

	shaderName = "gfx/2d/bigchars";
	hShader = trap_R_RegisterShaderNoMip( shaderName );

	Com_sprintf( font->name, sizeof ( font->name ), "bitmapfont_%dx%d", charHeight, charWidth );
	font->glyphScale = 48.0f / 16;
	font->pointSize = charHeight;
	font->flags = FONTFLAG_CURSORS;

	aspect = (float)charWidth / (float)charHeight;

	for ( i = 0; i < GLYPHS_PER_FONT; i++ ) {
		i &= 255;
		row = i>>4;
		col = i&15;

		font->glyphs[i].height = 16;
		font->glyphs[i].top = 16;
		font->glyphs[i].left = 0;
		font->glyphs[i].pitch = 16*aspect;
		font->glyphs[i].xSkip = 16*aspect;
		font->glyphs[i].imageWidth = 16*aspect;
		font->glyphs[i].imageHeight = 16;
		font->glyphs[i].s = col*0.0625; // note: 0.0625 = 16 / 256.0f
		font->glyphs[i].t = row*0.0625;
		font->glyphs[i].s2 = col*0.0625 + 0.0625;
		font->glyphs[i].t2 = row*0.0625 + 0.0625;
		font->glyphs[i].glyph = hShader;
		Q_strncpyz( font->glyphs[i].shaderName, shaderName, sizeof ( font->glyphs[i].shaderName ) );
	}
}

// Status bar number font, separeate 32x32 images for 0-9 and minus
void CG_InitBitmapNumberFont( fontInfo_t *font, int charHeight, int charWidth ) {
	int			i, index;
	const char	*shaderName;
	float		aspect;
	static char	*sb_nums[11] = {
		"gfx/2d/numbers/zero_32b",
		"gfx/2d/numbers/one_32b",
		"gfx/2d/numbers/two_32b",
		"gfx/2d/numbers/three_32b",
		"gfx/2d/numbers/four_32b",
		"gfx/2d/numbers/five_32b",
		"gfx/2d/numbers/six_32b",
		"gfx/2d/numbers/seven_32b",
		"gfx/2d/numbers/eight_32b",
		"gfx/2d/numbers/nine_32b",
		"gfx/2d/numbers/minus_32b",
	};

	Com_Memset( font, 0, sizeof( fontInfo_t ) );

	Com_sprintf( font->name, sizeof ( font->name ), "numberfont_%dx%d", charHeight, charWidth );
	font->glyphScale = 48.0f / 48;
	font->pointSize = charHeight;
	font->flags = 0;

	aspect = (float)charWidth / (float)charHeight;

	for ( index = 0; index < 11; index++ ) {
		shaderName = sb_nums[index];

		if ( index == 10 ) {
			i = '-';
		} else {
			i = '0' + index;
		}

		font->glyphs[i].height = 48;
		font->glyphs[i].top = 48;
		font->glyphs[i].left = 0;
		font->glyphs[i].pitch = 48*aspect; // 32
		font->glyphs[i].xSkip = 48*aspect; // 32
		font->glyphs[i].imageWidth = 48*aspect; // 32
		font->glyphs[i].imageHeight = 48;
		font->glyphs[i].s = 0;
		font->glyphs[i].t = 0;
		font->glyphs[i].s2 = 1;
		font->glyphs[i].t2 = 1;
		font->glyphs[i].glyph = trap_R_RegisterShader( shaderName );
		Q_strncpyz( font->glyphs[i].shaderName, shaderName, sizeof ( font->glyphs[i].shaderName ) );
	}
}

// borderWidth is in screen-pixels, not scaled with resolution
qboolean CG_InitTrueTypeFont( const char *name, int pointSize, float borderWidth, fontInfo_t *font ) {
	int imageHeight;

	if ( cg_forceBitmapFonts.integer || !name || !name[0] ) {
		return qfalse;
	}

	trap_R_RegisterFont( name, pointSize, borderWidth, qfalse, font );

	if ( !font->name[0] ) {
		return qfalse;
	}

	// fallback if missing Q3 bigchars-like cursors only present in Spearmint rendered fonts
	if ( !( font->flags & FONTFLAG_CURSORS ) ) {
		// Team Arena pre-rendered fonts don't have cursor characters (they're just transparent space)

		// character 13 is used as a selection marker in q3_ui
		Com_Memcpy( &font->glyphs[GLYPH_ARROW], &font->glyphs[(int)'>'], sizeof ( glyphInfo_t ) );
	}

	// Most TrueType fonts don't contain the glyphs used for text input
	// cursors, so just hard code them. There is no easy way to hard code
	// the q3_ui arrow. Though, I don't think programs typically use glyphs
	// from fonts for text input cursors anyway.

	// ZTM: TODO?: Replace (unreliable/pointless) glyphInfo_t::pitch with
	// flags so it's possible to check if glyph is missing, so these can
	// be fallbacks instead of hard coded.

	// missing overstrike block
	Com_Memcpy( &font->glyphs[GLYPH_OVERSTRIKE], &font->glyphs[(int)'|'], sizeof ( glyphInfo_t ) );
	font->glyphs[GLYPH_OVERSTRIKE].glyph = cgs.media.whiteShader;
	font->glyphs[GLYPH_OVERSTRIKE].imageWidth = font->glyphs[GLYPH_OVERSTRIKE].imageHeight * 0.5f;
	font->glyphs[GLYPH_OVERSTRIKE].s = 0;
	font->glyphs[GLYPH_OVERSTRIKE].t = 0;
	font->glyphs[GLYPH_OVERSTRIKE].s2 = 1;
	font->glyphs[GLYPH_OVERSTRIKE].t2 = 1;

	// missing insert underline
	Com_Memcpy( &font->glyphs[GLYPH_INSERT], &font->glyphs[(int)'|'], sizeof ( glyphInfo_t ) );
	font->glyphs[GLYPH_INSERT].glyph = cgs.media.whiteShader;
	font->glyphs[GLYPH_INSERT].imageWidth = font->glyphs[GLYPH_INSERT].imageHeight * 0.5f;
	font->glyphs[GLYPH_INSERT].s = 0;
	font->glyphs[GLYPH_INSERT].t = 0;
	font->glyphs[GLYPH_INSERT].s2 = 1;
	font->glyphs[GLYPH_INSERT].t2 = 1;
	imageHeight = Com_Clamp( 1, font->glyphs[GLYPH_INSERT].imageHeight, 1 / ( font->pointSize / 48.0f * font->glyphScale ) );
	font->glyphs[GLYPH_INSERT].top -= font->glyphs[GLYPH_INSERT].imageHeight - imageHeight;
	font->glyphs[GLYPH_INSERT].imageHeight = imageHeight;

	return qtrue;
}

const glyphInfo_t *Text_GetGlyph( const fontInfo_t *font, unsigned long index ) {
	if ( index == 0 || index >= GLYPHS_PER_FONT ) {
		return &font->glyphs[(int)'.'];
	}

	return &font->glyphs[index];
}

float Text_Width( const char *text, const fontInfo_t *font, float scale, int limit ) {
	int count,len;
	float out;
	const glyphInfo_t *glyph;
	float useScale;
	const char *s;

	if ( !text ) {
		return 0;
	}

	useScale = scale * font->glyphScale;
	out = 0;

	len = Q_UTF8_PrintStrlen( text );
	if ( limit > 0 && len > limit ) {
		len = limit;
	}

	s = text;
	count = 0;
	while ( s && *s && count < len ) {
		if ( Q_IsColorString( s ) ) {
			s += 2;
			continue;
		}

		glyph = Text_GetGlyph( font, Q_UTF8_CodePoint( &s ) );
		out += glyph->xSkip;
		count++;
	}

	return out * useScale;
}

float Text_Height( const char *text, const fontInfo_t *font, float scale, int limit ) {
	int len, count;
	float max;
	const glyphInfo_t *glyph;
	float useScale;
	const char *s;

	if ( !text ) {
		return 0;
	}

	useScale = scale * font->glyphScale;
	max = 0;

	len = Q_UTF8_PrintStrlen( text );
	if ( limit > 0 && len > limit ) {
		len = limit;
	}

	s = text;
	count = 0;
	while ( s && *s && count < len ) {
		if ( Q_IsColorString( s ) ) {
			s += 2;
			continue;
		}

		glyph = Text_GetGlyph( font, Q_UTF8_CodePoint( &s ) );
		if ( max < glyph->height ) {
			max = glyph->height;
		}
		count++;
	}

	return max * useScale;
}

void Text_PaintGlyph( float x, float y, float w, float h, const glyphInfo_t *glyph, float *gradientColor ) {
#if 0
	vec4_t colors[2] = {
		{ 1.0, 1.0, 1.0f, 1.0f }, // white (pixel aligned)
		{ 0.0, 1.0, 0.0f, 1.0f }, // green (sub-pixel position / size)
	};

	// check which glyphs have sub-pixel blending
	gradientColor = NULL;
	if ( x != floor( x ) || y != floor( y ) ||
		 w != floor( w ) || h != floor( h ) ) {
		trap_R_SetColor( colors[1] );
	} else {
		trap_R_SetColor( colors[0] );
	}
#endif

	if ( gradientColor ) {
		trap_R_DrawStretchPicGradient( x, y, w, h, glyph->s, glyph->t, glyph->s2, glyph->t2, glyph->glyph, gradientColor );
	} else {
		trap_R_DrawStretchPic( x, y, w, h, glyph->s, glyph->t, glyph->s2, glyph->t2, glyph->glyph );
	}
}

void Text_Paint( float x, float y, const fontInfo_t *font, float scale, const vec4_t color, const char *text, float adjust, int limit, float shadowOffset, float gradient, qboolean forceColor, qboolean textInMotion ) {
	int len, count;
	vec4_t newColor;
	vec4_t gradientColor;
	const glyphInfo_t *glyph;
	const char *s;
	float yadj, xadj;
	float useScaleX, useScaleY;
	float xscale, yscale;
	float shadowOffsetX, shadowOffsetY;

	if ( !text ) {
		return;
	}

	xscale = 1.0f;
	yscale = 1.0f;
	CG_AdjustFrom640( &x, &y, &xscale, &yscale );

	shadowOffsetX = shadowOffset * xscale;
	shadowOffsetY = shadowOffset * yscale;

	adjust *= xscale;

	useScaleX = scale * font->glyphScale * xscale;
	useScaleY = scale * font->glyphScale * yscale;

	if ( !textInMotion ) {
		// prevent native resolution text from being blurred due to sub-pixel blending
		x = floor( x );
		y = floor( y );
		shadowOffsetX = floor( shadowOffsetX );
		shadowOffsetY = floor( shadowOffsetY );
	}

	trap_R_SetColor( color );
	Vector4Copy( color, newColor );
	Vector4Copy( color, lastTextColor );

	gradientColor[0] = Com_Clamp( 0, 1, newColor[0] - gradient );
	gradientColor[1] = Com_Clamp( 0, 1, newColor[1] - gradient );
	gradientColor[2] = Com_Clamp( 0, 1, newColor[2] - gradient );
	gradientColor[3] = color[3];

	len = Q_UTF8_PrintStrlen( text );
	if ( limit > 0 && len > limit ) {
		len = limit;
	}

	s = text;
	count = 0;
	while ( s && *s && count < len ) {
		if ( Q_IsColorString( s ) ) {
			if ( !forceColor ) {
				VectorCopy( g_color_table[ColorIndex(*(s+1))], newColor );
				newColor[3] = color[3];
				trap_R_SetColor( newColor );
				Vector4Copy( newColor, lastTextColor );

				gradientColor[0] = Com_Clamp( 0, 1, newColor[0] - gradient );
				gradientColor[1] = Com_Clamp( 0, 1, newColor[1] - gradient );
				gradientColor[2] = Com_Clamp( 0, 1, newColor[2] - gradient );
				gradientColor[3] = color[3];
			}
			s += 2;
			continue;
		}

		glyph = Text_GetGlyph( font, Q_UTF8_CodePoint( &s ) );

		yadj = useScaleY * glyph->top;
		xadj = useScaleX * glyph->left;
		if ( shadowOffsetX || shadowOffsetY ) {
			colorBlack[3] = newColor[3];
			trap_R_SetColor( colorBlack );
			Text_PaintGlyph( x + xadj + shadowOffsetX, y - yadj + shadowOffsetY, glyph->imageWidth * useScaleX, glyph->imageHeight * useScaleY, glyph, NULL );
			trap_R_SetColor( newColor );
			colorBlack[3] = 1.0f;
		}
		Text_PaintGlyph( x + xadj, y - yadj, glyph->imageWidth * useScaleX, glyph->imageHeight * useScaleY, glyph, ( gradient != 0 ) ? gradientColor : NULL );

		x += ( glyph->xSkip * useScaleX ) + adjust;
		count++;
	}

	trap_R_SetColor( NULL );
}

void Text_PaintWithCursor( float x, float y, const fontInfo_t *font, float scale, const vec4_t color, const char *text, int cursorPos, char cursor, float adjust, int limit, float shadowOffset, float gradient, qboolean forceColor, qboolean textInMotion ) {
	int len, count;
	vec4_t newColor;
	vec4_t gradientColor;
	const glyphInfo_t *glyph, *glyph2;
	float yadj, xadj;
	float useScaleX, useScaleY;
	float xscale, yscale;
	float shadowOffsetX, shadowOffsetY;
	const char *s;

	if ( !text ) {
		return;
	}

	xscale = 1.0f;
	yscale = 1.0f;
	CG_AdjustFrom640( &x, &y, &xscale, &yscale );

	shadowOffsetX = shadowOffset * xscale;
	shadowOffsetY = shadowOffset * yscale;

	adjust *= xscale;

	useScaleX = scale * font->glyphScale * xscale;
	useScaleY = scale * font->glyphScale * yscale;

	if ( !textInMotion ) {
		// prevent native resolution text from being blurred due to sub-pixel blending
		x = floor( x );
		y = floor( y );
		shadowOffsetX = floor( shadowOffsetX );
		shadowOffsetY = floor( shadowOffsetY );
	}

	trap_R_SetColor( color );
	Vector4Copy( color, newColor );
	Vector4Copy( color, lastTextColor );

	gradientColor[0] = Com_Clamp( 0, 1, newColor[0] - gradient );
	gradientColor[1] = Com_Clamp( 0, 1, newColor[1] - gradient );
	gradientColor[2] = Com_Clamp( 0, 1, newColor[2] - gradient );
	gradientColor[3] = color[3];

	// note: doesn't use Q_UTF8_PrintStrlen because this function draws color codes
	len = Q_UTF8_Strlen( text );
	if ( limit > 0 && len > limit ) {
		len = limit;
	}

	s = text;
	count = 0;
	glyph2 = Text_GetGlyph( font, cursor );

	while ( s && *s && count < len ) {
		if ( Q_IsColorString( s ) ) {
			if ( !forceColor ) {
				VectorCopy( g_color_table[ColorIndex(*(s+1))], newColor );
				newColor[3] = color[3];
				trap_R_SetColor( newColor );
				Vector4Copy( newColor, lastTextColor );

				gradientColor[0] = Com_Clamp( 0, 1, newColor[0] - gradient );
				gradientColor[1] = Com_Clamp( 0, 1, newColor[1] - gradient );
				gradientColor[2] = Com_Clamp( 0, 1, newColor[2] - gradient );
				gradientColor[3] = color[3];
			}
			// display color codes in edit fields instead of skipping them
		}

		glyph = Text_GetGlyph( font, Q_UTF8_CodePoint( &s ) );

		if ( count == cursorPos && ( ( cg.realTime / BLINK_DIVISOR ) & 1 ) == 0 ) {
			yadj = useScaleY * glyph2->top;

			// use horizontal width of text character (glyph)
			Text_PaintGlyph( x, y - yadj,
							(glyph->left + glyph->xSkip) * useScaleX,
							glyph2->imageHeight * useScaleY,
							glyph2,
							NULL );
		}

		yadj = useScaleY * glyph->top;
		xadj = useScaleX * glyph->left;

		if ( shadowOffsetX || shadowOffsetY ) {
			colorBlack[3] = newColor[3];
			trap_R_SetColor( colorBlack );
			Text_PaintGlyph( x + xadj + shadowOffsetX, y - yadj + shadowOffsetY, glyph->imageWidth * useScaleX, glyph->imageHeight * useScaleY, glyph, NULL );
			trap_R_SetColor( newColor );
			colorBlack[3] = 1.0f;
		}

		// make overstrike cursor invert color
		if ( count == cursorPos && !( ( cg.realTime / BLINK_DIVISOR ) & 1 ) && cursor == GLYPH_OVERSTRIKE ) {
			// invert color
			vec4_t invertedColor;

			invertedColor[0] = 1.0f - newColor[0];
			invertedColor[1] = 1.0f - newColor[1];
			invertedColor[2] = 1.0f - newColor[2];
			invertedColor[3] = color[3];

			trap_R_SetColor( invertedColor );

			Text_PaintGlyph( x + xadj, y - yadj, glyph->imageWidth * useScaleX, glyph->imageHeight * useScaleY, glyph, NULL );
		} else {
			Text_PaintGlyph( x + xadj, y - yadj, glyph->imageWidth * useScaleX, glyph->imageHeight * useScaleY, glyph, ( gradient != 0 ) ? gradientColor : NULL );
		}

		if ( count == cursorPos && !( ( cg.realTime / BLINK_DIVISOR ) & 1 ) && cursor == GLYPH_OVERSTRIKE ) {
			// restore color
			trap_R_SetColor( newColor );
		}

		x += ( glyph->xSkip * useScaleX ) + adjust;
		count++;
	}

	// need to paint cursor at end of text
	if ( cursorPos == len && !( ( cg.realTime / BLINK_DIVISOR ) & 1 ) ) {
		yadj = useScaleY * glyph2->top;
		Text_PaintGlyph( x, y - yadj, glyph2->imageWidth * useScaleX, glyph2->imageHeight * useScaleY, glyph2, NULL );
	}

	trap_R_SetColor( NULL );
}

void Text_Paint_Limit( float *maxX, float x, float y, const fontInfo_t *font, float scale, const vec4_t color, const char* text, float adjust, int limit, qboolean textInMotion ) {
	int len, count;
	vec4_t newColor;
	const glyphInfo_t *glyph;
	const char *s;
	float max;
	float yadj, xadj;
	float useScaleX, useScaleY;
	float xscale, yscale;

	if ( !text || !maxX ) {
		return;
	}

	xscale = 1.0f;
	yscale = 1.0f;
	CG_AdjustFrom640( &x, &y, &xscale, &yscale );

	max = *maxX * xscale;
	adjust *= xscale;

	useScaleX = scale * font->glyphScale * xscale;
	useScaleY = scale * font->glyphScale * yscale;

	if ( !textInMotion ) {
		// prevent native resolution text from being blurred due to sub-pixel blending
		x = floor( x );
		y = floor( y );
	}

	trap_R_SetColor( color );
	Vector4Copy( color, lastTextColor );

	len = Q_UTF8_PrintStrlen( text );
	if ( limit > 0 && len > limit ) {
		len = limit;
	}

	s = text;
	count = 0;
	while ( s && *s && count < len ) {
		if ( Q_IsColorString( s ) ) {
			VectorCopy( g_color_table[ColorIndex(*(s+1))], newColor );
			newColor[3] = color[3];
			trap_R_SetColor( newColor );
			Vector4Copy( newColor, lastTextColor );
			s += 2;
			continue;
		}

		glyph = Text_GetGlyph( font, Q_UTF8_CodePoint( &s ) );

		if ( x + ( glyph->xSkip * useScaleX ) > max ) {
			*maxX = 0;
			break;
		}

		yadj = useScaleY * glyph->top;
		xadj = useScaleX * glyph->left;

		Text_PaintGlyph( x + xadj, y - yadj, glyph->imageWidth * useScaleX, glyph->imageHeight * useScaleY, glyph, NULL );
		x += ( glyph->xSkip * useScaleX ) + adjust;
		*maxX = x / xscale;
		count++;
	}
	trap_R_SetColor( NULL );
}

#define MAX_WRAP_BYTES 1024
#define MAX_WRAP_LINES 1024

void Text_Paint_AutoWrapped( float x, float y, const fontInfo_t *font, float scale, const vec4_t color, const char *str, float adjust, int limit, float shadowOffset, float gradient, qboolean forceColor, qboolean textInMotion, float xmax, float ystep, int style ) {
	int width;
	char *s1, *s2, *s3;
	char c_bcp;
	char buf[MAX_WRAP_BYTES];
	char wrapped[MAX_WRAP_BYTES + MAX_WRAP_LINES];
	qboolean autoNewline[MAX_WRAP_LINES];
	int numLines;
	vec4_t newColor;
	const char *p, *start;
	float drawX;

	if ( !str || str[0] == '\0' )
		return;

	//
	// Wrap the text
	//

	Q_strncpyz( buf, str, sizeof(buf) );
	s1 = s2 = s3 = buf;

	wrapped[0] = 0;
	numLines = 0;

	while ( 1 ) {
		do {
			s3 += Q_UTF8_Width( s3 );
		} while ( *s3 != '\n' && *s3 != ' ' && *s3 != '\0' );
		c_bcp = *s3;
		*s3 = '\0';
		width = Text_Width( s1, font, scale, 0 );
		*s3 = c_bcp;
		if ( width > xmax ) {
			if ( s1 == s2 )
			{
				// fuck, don't have a clean cut, we'll overflow
				s2 = s3;
			}
			*s2 = '\0';

			Q_strcat( wrapped, sizeof ( wrapped ), s1 );
			Q_strcat( wrapped, sizeof ( wrapped ), "\n" );
			if ( numLines < MAX_WRAP_LINES ) {
				autoNewline[numLines] = qtrue;
				numLines++;
			}

			if ( c_bcp == '\0' )
			{
				// that was the last word
				// we could start a new loop, but that wouldn't be much use
				// even if the word is too long, we would overflow it (see above)
				// so just print it now if needed
				s2 += Q_UTF8_Width( s2 );
				if ( *s2 != '\0' && *s2 != '\n' ) // if we are printing an overflowing line we have s2 == s3
				{
					Q_strcat( wrapped, sizeof ( wrapped ), s2 );
					Q_strcat( wrapped, sizeof ( wrapped ), "\n" );
					if ( numLines < MAX_WRAP_LINES ) {
						autoNewline[numLines] = qtrue;
						numLines++;
					}
				}
				break;
			}
			s2 += Q_UTF8_Width( s2 );
			s1 = s2;
			s3 = s2;
		}
		else if ( c_bcp == '\n' )
		{
			*s3 = '\0';

			Q_strcat( wrapped, sizeof ( wrapped ), s1 );
			Q_strcat( wrapped, sizeof ( wrapped ), "\n" );
			if ( numLines < MAX_WRAP_LINES ) {
				autoNewline[numLines] = qfalse;
				numLines++;
			}

			s3 += Q_UTF8_Width( s3 );
			s1 = s3;
			s2 = s3;

			if ( *s3 == '\0' ) // we reached the end
			{
				break;
			}
		}
		else
		{
			s2 = s3;
			if ( c_bcp == '\0' ) // we reached the end
			{
				Q_strcat( wrapped, sizeof ( wrapped ), s1 );
				Q_strcat( wrapped, sizeof ( wrapped ), "\n" );
				if ( numLines < MAX_WRAP_LINES ) {
					autoNewline[numLines] = qfalse;
					numLines++;
				}
				break;
			}
		}
	}

	//
	// Draw the text
	//

	switch (style & UI_VA_FORMATMASK)
	{
		case UI_VA_CENTER:
			// center justify at y
			y = y - numLines * ystep / 2.0f;
			break;

		case UI_VA_BOTTOM:
			// bottom justify at y
			y = y - numLines * ystep;
			break;

		case UI_VA_TOP:
		default:
			// top justify at y
			break;
	}

	numLines = 0;
	Vector4Copy( color, newColor );

	start = wrapped;
	p = strchr(wrapped, '\n');
	while (p && *p) {
		strncpy(buf, start, p-start+1);
		buf[p-start] = '\0';

		switch (style & UI_FORMATMASK)
		{
			case UI_CENTER:
				// center justify at x
				drawX = x - Text_Width( buf, font, scale, 0 ) / 2;
				break;

			case UI_RIGHT:
				// right justify at x
				drawX = x - Text_Width( buf, font, scale, 0 );
				break;

			case UI_LEFT:
			default:
				// left justify at x
				drawX = x;
				break;
		}

		Text_Paint( drawX, y, font, scale, newColor, buf, adjust, 0, shadowOffset, gradient, forceColor, textInMotion );
		y += ystep;

		if ( numLines >= MAX_WRAP_LINES || autoNewline[numLines] ) {
			Vector4Copy( lastTextColor, newColor );
		} else {
			// reset color after non-wrapped lines
			Vector4Copy( color, newColor );
		}

		numLines++;

		start += p - start + 1;
		p = strchr(p+1, '\n');
	}
}

