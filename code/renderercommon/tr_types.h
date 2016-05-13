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
#ifndef __TR_TYPES_H
#define __TR_TYPES_H


#define MAX_CORONAS		32		// not really a reason to limit this other than trying to keep a reasonable count
#define	MAX_DLIGHTS		32		// can't be increased, because bit flags are used on surfaces

#define	REFENTITYNUM_BITS	12		// can't be increased without changing drawsurf bit packing
#define	REFENTITYNUM_MASK	((1<<REFENTITYNUM_BITS) - 1)
// the last N-bit number (2^REFENTITYNUM_BITS - 1) is reserved for the special world refentity,
//  and this is reflected by the value of MAX_REFENTITIES (which therefore is not a power-of-2)
#define	MAX_REFENTITIES		((1<<REFENTITYNUM_BITS) - 1)
#define	REFENTITYNUM_WORLD	((1<<REFENTITYNUM_BITS) - 1)

// renderfx flags
#define	RF_NO_DIRECTED_LIGHT	0x0001	// don't use directed light from world light grid or dlights
#define	RF_ONLY_MIRROR		0x0002		// only draw in mirrors and portals
#define	RF_NO_MIRROR		0x0004		// do not draw in mirrors or portals
#define	RF_DEPTHHACK		0x0008		// for view weapon Z crunching

#define RF_CROSSHAIR		0x0010		// This item is a cross hair and will draw over everything similar to
						// DEPTHHACK in stereo rendering mode, with the difference that the
						// projection matrix won't be hacked to reduce the stereo separation as
						// is done for the gun.

#define	RF_CONST_AMBIENT	0x0020		// use refEntity->ambientLight instead of world light grid + refEntity->ambientLight

#define	RF_NOSHADOW		0x0040		// don't add stencil shadows

#define RF_LIGHTING_ORIGIN	0x0080		// use refEntity->lightingOrigin instead of refEntity->origin
						// for lighting.  This allows entities to sink into the floor
						// with their origin going solid, and allows all parts of a
						// player to get the same lighting

#define	RF_SHADOW_PLANE		0x0100		// use refEntity->shadowPlane
#define	RF_WRAP_FRAMES		0x0200		// mod the model frames by the maxframes to allow continuous
										// animation without needing to know the frame count

#define RF_FORCE_ENT_ALPHA	0x0400		// override shader alpha value and take the one from the entity
#define	RF_RGB_TINT			0x0800		// override shader color values and take the ones from the entity

#define	RF_AUTOAXIS			0x1000		// Similar to autosprite, rotate model forward/side axiis to face screen (including in mirrors and portals)
#define	RF_AUTOAXIS2		0x2000		// Similar to autosprite2, rotate RT_POLY_LOCAL side axis to face screen (including in mirrors and portals)

#define	RF_LIGHTING_GRID	0x4000		// make refEntity in a scene with RDF_NOWORLDMODEL use lighting grid

// refdef flags
#define RDF_NOWORLDMODEL	0x0001		// used for player configuration screen
#define RDF_UNDERWATER		0x0002		// underwater
#define RDF_HYPERSPACE		0x0004		// teleportation effect
#define RDF_SUNLIGHT		0x0008		// render sunlight and shadows
#define RDF_NOSKY			0x0010		// do not render sky. used on world when there is a separate skybox portal.
#define RDF_ONLYSKY			0x0020		// only render sky. used on skybox portal when want to disable ground.

// any change in the LIGHTMAP_* defines here MUST be reflected in
// R_FindShader() in tr_bsp.c
#define LIGHTMAP_2D         -4	// shader is for 2D rendering
#define LIGHTMAP_BY_VERTEX  -3	// pre-lit triangle models
#define LIGHTMAP_WHITEIMAGE -2
#define LIGHTMAP_NONE       -1

typedef struct {
	vec3_t		xyz;
	float		st[2];
	byte		modulate[4];
} polyVert_t;

// =========================================
// Gordon, these MUST NOT exceed the values for SHADER_MAX_VERTEXES/SHADER_MAX_INDEXES
#define MAX_PB_VERTS    1025 // SHADER_MAX_VERTEXES
#define MAX_PB_INDICIES ( MAX_PB_VERTS * 6 )

typedef struct polyBuffer_s {
	vec4_t xyz[MAX_PB_VERTS];
	vec2_t st[MAX_PB_VERTS];
	byte color[MAX_PB_VERTS][4];
	int numVerts;

	int indicies[MAX_PB_INDICIES];
	int numIndicies;

	qhandle_t shader;
} polyBuffer_t;
// =========================================

typedef enum {
	RT_MODEL,
	RT_POLY_GLOBAL,			// verts are in world coords
	RT_POLY_LOCAL,			// verts are in local space; relative to ref entity origin and axis. useful for RF_AUTOAXIS
	RT_SPRITE,
	RT_PORTALSURFACE,		// doesn't draw anything, just info for portals

	RT_MAX_REF_ENTITY_TYPE,

	// ZTM: FIXME: these are only used in cgame...
	RT_BEAM,
	RT_RAIL_CORE,
	RT_RAIL_RINGS,
	RT_LIGHTNING

} refEntityType_t;

typedef struct {
	refEntityType_t	reType;
	int			renderfx;

	qhandle_t	hModel;				// opaque type outside refresh

	// most recent data
	vec3_t		lightingOrigin;		// so multi-part models can be lit identically (RF_LIGHTING_ORIGIN)
	float		shadowPlane;		// projection shadows go here, stencils go slightly lower

	vec3_t		axis[3];			// rotation vectors
	qboolean	nonNormalizedAxes;	// axis are not normalized, i.e. they have scale
	float		origin[3];			// also used as MODEL_BEAM's "from"
	int			frame;				// also used as MODEL_BEAM's diameter
	qhandle_t	frameModel;			// use skeleton from another model

	// previous data for frame interpolation
	float		oldorigin[3];		// also used as MODEL_BEAM's "to"
	int			oldframe;
	qhandle_t	oldframeModel;		// old skeleton model
	float		backlerp;			// 0.0 = current, 1.0 = old

	// texturing
	int			skinNum;			// inline skin index
	qhandle_t	customSkin;			// NULL for default skin
	qhandle_t	customShader;		// use one image for the entire thing

	// misc
	byte		shaderRGBA[4];		// colors used by rgbgen entity shaders
	float		shaderTexCoord[2];	// texture coordinates used by tcMod entity modifiers
	int			shaderTime;			// subtracted from refdef time to control effect start times
	float		ambientLight[3];	// add to light grid ambient light or use instead of light grid (RF_CONST_AMBIENT)

	// extra sprite information
	float		radius;
	float		rotation;

	//
	// added in Spearmint 0.2
	//
	vec3_t		fireRiseDir;		// for alphaGen normalzfade and waveforms with negative frequency

	// for MDS and MDX/MDM models
	vec3_t		torsoAxis[3];		// rotation vectors for torso section of skeletal animation
	int			torsoFrame;			// skeletal torso can have frame independant of legs frame
	qhandle_t	torsoFrameModel;
	int			oldTorsoFrame;
	qhandle_t	oldTorsoFrameModel;
	float		torsoBacklerp;

} refEntity_t;


#define	MAX_RENDER_STRINGS			8
#define	MAX_RENDER_STRING_LENGTH	32

typedef enum {
	FT_NONE,
	FT_EXP,
	FT_LINEAR,

	FT_MAX_FOG_TYPE
} fogType_t;

typedef struct {
	int			x, y, width, height;
	float		fov_x, fov_y;
	vec3_t		vieworg;
	vec3_t		viewaxis[3];		// transformation matrix

	// time in milliseconds for shader effects and other time dependent rendering issues
	int			time;

	int			rdflags;			// RDF_NOWORLDMODEL, etc

	// 1 bits will prevent the associated area from rendering at all
	byte		areamask[MAX_MAP_AREA_BYTES];

	// text messages for deform text shaders
	char		text[MAX_RENDER_STRINGS][MAX_RENDER_STRING_LENGTH];

	// fog
	fogType_t	fogType;
	vec3_t		fogColor;
	float		fogDepthForOpaque;
	float		fogDensity;

	// OpenGL2 renderer extras
	float			blurFactor;

	// specific sun shadow casting information, if RDF_SUNLIGHT
	float           sunDir[3];
	float           sunCol[3];
	float           sunAmbCol[3];

	// for alphaGen skyAlpha and oneMinusSkyAlpha
	float			skyAlpha;

	// maximum distance for the far clip plane
	float			farClip;
} refdef_t;


typedef enum {
	STEREO_CENTER,
	STEREO_LEFT,
	STEREO_RIGHT
} stereoFrame_t;


/*
** glconfig_t
**
** Contains variables specific to the OpenGL configuration
** being run right now.  These are constant once the OpenGL
** subsystem is initialized.
*/
typedef enum {
	TC_NONE,
	TC_S3TC,  // this is for the GL_S3_s3tc extension.
	TC_S3TC_ARB  // this is for the GL_EXT_texture_compression_s3tc extension.
} textureCompression_t;

typedef struct {
	char					renderer_string[MAX_STRING_CHARS];
	char					vendor_string[MAX_STRING_CHARS];
	char					version_string[MAX_STRING_CHARS];
	char					extensions_string[BIG_INFO_STRING * 4];

	int						maxTextureSize;			// queried from GL
	int						numTextureUnits;		// multitexture ability

	int						colorBits, depthBits, stencilBits;

	qboolean				deviceSupportsGamma;
	textureCompression_t	textureCompression;
	qboolean				textureEnvAddAvailable;
	qboolean				textureFilterAnisotropic;
	int						maxAnisotropy;

	// Game resolution, aspect, and refresh rate.
	int						vidWidth;
	int						vidHeight;
	float					windowAspect;

	// Display (desktop) resolution, aspect, and refresh rate.
	int						displayWidth;
	int						displayHeight;
	float					displayAspect;
	int						displayFrequency;

	// synonymous with "does rendering consume the entire screen?"
	qboolean				isFullscreen;
	qboolean				stereoEnabled;
} glconfig_t;


// font support
#define GLYPH_START 0
#define GLYPH_END 255
#define GLYPH_CHARSTART 32
#define GLYPH_CHAREND 127
#define GLYPHS_PER_FONT GLYPH_END - GLYPH_START + 1
typedef struct {
	int			height;			// number of scan lines
	int			top;			// top of glyph in buffer
	int			left;			// left of glyph in buffer
	int			pitch;			// width for copying
	int			xSkip;			// x adjustment
	int			imageWidth;		// width of actual image
	int			imageHeight;	// height of actual image
	float		s;				// x offset in image where glyph starts
	float		t;				// y offset in image where glyph starts
	float		s2;
	float		t2;
	qhandle_t	glyph;			// handle to the shader with the glyph
	char		shaderName[32];
} glyphInfo_t;

#define FONTFLAG_CURSORS    0x0001 // font has extra glyphs (notably cursors) like Q3's bigchars font
#define FONTFLAG_BORDER     0x0002 // font has a border

typedef struct {
	glyphInfo_t	glyphs [GLYPHS_PER_FONT];
	float		glyphScale;
	char		name[MAX_QPATH];
	int			pointSize;
	int			flags;
} fontInfo_t;

#endif	// __TR_TYPES_H
