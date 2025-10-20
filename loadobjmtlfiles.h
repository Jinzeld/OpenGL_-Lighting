#ifndef LOADOBJFILE_H
#define LOADOBJFILE_H

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <vector>


#define OBJDELIMS		" \t"
#define NULLPTR 0
#define DELIMS " \t"

struct Vertex
{
	float x, y, z;
};

struct Normal
{
	float nx, ny, nz;
};

struct TextureCoord
{
	float s, t, p;
};

struct face
{
	int v, n, t;
};


GLuint	LoadObjFile( char * );
GLuint  LoadObjMtlFiles(char *name);

void	Cross( float [3], float [3], float [3] );
char *	ReadRestOfLine( FILE * );
void	ReadObjVTN( char *, int *, int *, int * );
float	Unit( float [3] );
float	Unit( float [3], float [3] );

int	Readline( FILE *, char * );
float * Array3( float, float, float );
float * Array3( float * );

int	FindMtlName( char * );
void	SetOpenglMtlProperties( );
char *	GetSuffix( char * );


unsigned char * BmpToTexture( char *, int *, int * );


#endif		// #ifndef
