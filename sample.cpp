#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <ctype.h>
#include <time.h>


#ifndef F_PI
#define F_PI		((float)(M_PI))
#define F_2_PI		((float)(2.f*F_PI))
#define F_PI_2		((float)(F_PI/2.f))
#endif


#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#endif


#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include "glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "glut.h"

#include "loadobjmtlfiles.h"



//	This is a sample OpenGL / GLUT program
//
//	The objective is to draw a 3d object and change the color of the axes
//		with a glut menu
//
//	The left mouse button does rotation
//	The middle mouse button does scaling
//	The user interface allows:
//		1. The axes to be turned on and off
//		2. The color of the axes to be changed
//		3. Debugging to be turned on and off
//		4. Depth cueing to be turned on and off
//		5. The projection to be changed
//		6. The transformations to be reset
//		7. The program to quit
//
//	Author:			Joe Graphics

// title of these windows:

const char *WINDOWTITLE = "OpenGL / GLUT Sample -- Joe Graphics";
const char *GLUITITLE   = "User Interface Window";

// what the glui package defines as true and false:

const int GLUITRUE  = true;
const int GLUIFALSE = false;

// the escape key:

const int ESCAPE = 0x1b;

// initial window size:

const int INIT_WINDOW_SIZE = 1000;

// size of the Wall to be drawn:

float WALLSIZE = 10.f;

// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = 1.f;
const float SCLFACT = 0.005f;

// minimum allowable scale factor:

const float MINSCALE = 0.05f;

// scroll wheel button values:

const int SCROLL_WHEEL_UP   = 3;
const int SCROLL_WHEEL_DOWN = 4;

// equivalent mouse movement when we click the scroll wheel:

const float SCROLL_WHEEL_CLICK_FACTOR = 5.f;

// active mouse buttons (or them together):

const int LEFT   = 4;
const int MIDDLE = 2;
const int RIGHT  = 1;

// which projection:

enum Projections
{
	ORTHO,
	PERSP
};

// which button:

enum ButtonVals
{
	RESET,
	QUIT
};

// window background color (rgba):

const GLfloat BACKCOLOR[ ] = { 0., 0., 0., 1. };

// line width for the axes:

const GLfloat AXES_WIDTH   = 3.;

// the color numbers:
// this order must match the radio button order, which must match the order of the color names,
// 	which must match the order of the color RGB values

enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA
};

char * ColorNames[ ] =
{
	(char *)"Red",
	(char*)"Yellow",
	(char*)"Green",
	(char*)"Cyan",
	(char*)"Blue",
	(char*)"Magenta"
};

// the color definitions:
// this order must match the menu order

const GLfloat Colors[ ][3] = 
{
	{ 1., 0., 0. },		// red
	{ 1., 1., 0. },		// yellow
	{ 0., 1., 0. },		// green
	{ 0., 1., 1. },		// cyan
	{ 0., 0., 1. },		// blue
	{ 1., 0., 1. },		// magenta
};


// --------------------------- Constants / Globals ---------------------------
#define MS_PER_CYCLE 10000    // 10 second cycle
#define SPEED 1.0f	

bool Frozen = false;

GLuint GridDL;
GLuint WallBackDL;
GLuint WallRightDL;
GLuint Wall1DL = 0;
GLuint Wall2DL = 0;
GLuint Obj1DL = 0;   // OBJ loaded object (or fallback)
GLuint Obj2DL = 0;
GLuint Obj3DL = 0;
GLuint ObjModelDL = 0; // Display list for loaded OBJ
GLuint BunnyObj; // Display list for bunny OBJ
GLuint DinoObj; // Display list for dino OBJ
GLuint SalmonObj; // Display list for salmon OBJ

float TimeFraction = 0.0f;

// Light state
bool LightIsSpot = false;
GLfloat LightColor[4] = {1.f, 1.f, 1.f, 1.f};

// Light path
#define LIGHTRADIUS 6.0f
#define LIGHTHEIGHT 2.5f

// Add global for light angle
float gLightAngle = 0.0f;

// Add global for circle toggle
int CircleOn = 0;

// what options should we compile-in?
// in general, you don't need to worry about these
// i compile these in to show class examples of things going wrong
//#define DEMO_Z_FIGHTING
//#define DEMO_DEPTH_BUFFER


// non-constant global variables:

int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
GLuint	BoxList;				// object display list
int		DebugOn;				// != 0 means to print debugging info
int		DepthCueOn;				// != 0 means to use intensity depth cueing
int		DepthBufferOn;			// != 0 means to use the z-buffer
int		DepthFightingOn;		// != 0 means to force the creation of z-fighting
int		MainWindow;				// window id for main graphics window
int		NowColor;				// index into Colors[ ]
int		NowProjection;		// ORTHO or PERSP
float	Scale;					// scaling factor
int		ShadowsOn;				// != 0 means to turn shadows on
float	Time;					// used for animation, this has a value between 0. and 1.
int		Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;				// rotation angles in degrees

GLuint LightBallList;      		// spot light list


// function prototypes:

void	Animate( );
void	Display( );
void	DoAxesMenu( int );
void	DoColorMenu( int );
void	DoDepthBufferMenu( int );
void	DoDepthFightingMenu( int );
void	DoDepthMenu( int );
void	DoDebugMenu( int );
void	DoMainMenu( int );
void	DoProjectMenu( int );
void	DoRasterString( float, float, float, char * );
void	DoStrokeString( float, float, float, float, char * );
float	ElapsedSeconds( );
void	InitGraphics( );
void	InitLists( );
void	InitMenus( );
void	Keyboard( unsigned char, int, int );
void	MouseButton( int, int, int, int );
void	MouseMotion( int, int );
void	Reset( );
void	Resize( int, int );
void	Visibility( int );

void			Axes( float );
void			HsvRgb( float[3], float [3] );
void			Cross(float[3], float[3], float[3]);
float			Dot(float [3], float [3]);
float			Unit(float [3], float [3]);
float			Unit(float [3]);
void            DrawCircle(float, float, float, float, int);


// utility to create an array from 3 separate values:

float *
Array3( float a, float b, float c )
{
	static float array[4];

	array[0] = a;
	array[1] = b;
	array[2] = c;
	array[3] = 1.;
	return array;
}

// utility to create an array from a multiplier and an array:

float *
MulArray3( float factor, float array0[ ] )
{
	static float array[4];

	array[0] = factor * array0[0];
	array[1] = factor * array0[1];
	array[2] = factor *array0[2];
	array[3] = 1.;
	return array;
}


float *
MulArray3(float factor, float a, float b, float c )
{
	static float array[4];

	float* abc = Array3(a, b, c);
	array[0] = factor * abc[0];
	array[1] = factor * abc[1];
	array[2] = factor * abc[2];
	array[3] = 1.;
	return array;
}


float
Ranf( float low, float high )
{
        float r = (float) rand();               // 0 - RAND_MAX
        float t = r  /  (float) RAND_MAX;       // 0. - 1.

        return   low  +  t * ( high - low );
}

// call this if you want to force your program to use
// a different random number sequence every time you run it:
void
TimeOfDaySeed( )
{
	struct tm y2k;
	y2k.tm_hour = 0;    y2k.tm_min = 0; y2k.tm_sec = 0;
	y2k.tm_year = 2000; y2k.tm_mon = 0; y2k.tm_mday = 1;

	time_t  now;
	time( &now );
	double seconds = difftime( now, mktime(&y2k) );
	unsigned int seed = (unsigned int)( 1000.*seconds );    // milliseconds
	srand( seed );
}

// these are here for when you need them -- just uncomment the ones you need:

//#include "setmaterial.cpp"
//#include "setlight.cpp"
//#include "osusphere.cpp"
//#include "osucube.cpp"
//#include "osucylindercone.cpp"
//#include "osutorus.cpp"
//#include "bmptotexture.cpp"
//#include "loadobjmtlfiles.cpp"
//#include "keytime.cpp"
//#include "glslprogram.cpp"
//#include "vertexbufferobject.cpp"


// main program:

int
main( int argc, char *argv[ ] )
{
	// turn on the glut package:
	// (do this before checking argc and argv since glutInit might
	// pull some command line arguments out)

	glutInit( &argc, argv );

	// setup all the graphics stuff:

	InitGraphics( );

	// create the display lists that **will not change**:

	InitLists( );

	glutIdleFunc(Animate);


	// init all the global variables used by Display( ):
	// this will also post a redisplay

	Reset( );

	// setup all the user interface stuff:

	InitMenus( );

	// draw the scene once and wait for some interaction:
	// (this will never return)

	glutSetWindow( MainWindow );
	glutMainLoop( );

	// glutMainLoop( ) never actually returns
	// the following line is here to make the compiler happy:

	return 0;
}


// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutPostRedisplay( ) do it

void Animate() {
	int ms = glutGet( GLUT_ELAPSED_TIME );
	ms %= MS_PER_CYCLE;
	Time = (float)ms / (float)MS_PER_CYCLE;		// [0.,1.)
    // Animate spotlight angle
    gLightAngle += 0.5f;
    if (gLightAngle > 360.f) gLightAngle -= 360.f;
    glutPostRedisplay(); // Force continuous redraw for smooth animation
}


void SetMaterial(float r, float g, float b, float shininess, bool dull) {
    GLfloat ambient[4] = { 0.1f * r, 0.1f * g, 0.1f * b, 1.0f };
    GLfloat diffuse[4] = { 0.7f * r, 0.7f * g, 0.7f * b, 1.0f };
    GLfloat specular[4] = { (dull ? 0.02f : 0.6f), (dull ? 0.02f : 0.6f), (dull ? 0.02f : 0.6f), 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}

// draw the complete scene:

void Display()
{
    if (DebugOn != 0)
        fprintf(stderr, "Starting Display.\n");

    glutSetWindow(MainWindow);
    glDrawBuffer(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // ---------------- CAMERA ----------------
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.f, 1.f, 0.1f, 100.f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	//set the view
    gluLookAt(20.f, 8.f, 15.f, 0.f, 2.f, 0.f, 0.f, 1.f, 0.f);

    // Apply user rotation/scaling
    glRotatef((GLfloat)Yrot, 0.f, 1.f, 0.f);
    glRotatef((GLfloat)Xrot, 1.f, 0.f, 0.f);
    if (Scale < MINSCALE) Scale = MINSCALE;
    glScalef(Scale, Scale, Scale);

    // ---------------- LIGHT ----------------
    float radius = 8.0f;
    float lightX = radius * cos(gLightAngle * M_PI / 180.0f);
    float lightZ = radius * sin(gLightAngle * M_PI / 180.0f);
    float lightY = 8.0f;
    GLfloat lightPos[] = { lightX, lightY, lightZ, 1.0f };
    if (LightIsSpot) {
        // Spotlight setup
        GLfloat spotDir[] = { -lightX * 0.1f, -0.3f, -lightZ * 0.1f };
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
        glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotDir);
        glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 80.0f);    // wider cone, softer edge
        glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 10.0f);   // softer falloff
        glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.8f);
        glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.05f);
        glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.01f);
        // Brighter spotlight
        float spotBrightness = 6.0f; // Only for spot light
        GLfloat spotDiffuse[]  = { LightColor[0] * spotBrightness, LightColor[1] * spotBrightness, LightColor[2] * spotBrightness, 1.f };
        GLfloat spotSpecular[] = { LightColor[0] * spotBrightness, LightColor[1] * spotBrightness, LightColor[2] * spotBrightness, 1.f };
        glLightfv(GL_LIGHT0, GL_DIFFUSE, spotDiffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, spotSpecular);
    } else {
        // Point light setup
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
        glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180.0f); // disables spot effect
        // Smoother attenuation for point light
        glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.8f);
        glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.05f);
        glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.01f);
        float brightness = 2.0f; // Only for point light
        GLfloat diffuse[]  = { LightColor[0] * brightness, LightColor[1] * brightness, LightColor[2] * brightness, 1.f };
        GLfloat specular[] = { LightColor[0] * brightness, LightColor[1] * brightness, LightColor[2] * brightness, 1.f };
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    }

    // ---------------- SCENE DRAW ----------------
    // Floor and walls
    glCallList(GridDL);
    glCallList(WallBackDL);
    glCallList(WallRightDL);

    // Light sphere (unlit)
    glDisable(GL_LIGHTING);
    glPushMatrix();
        glTranslatef(lightX, lightY, lightZ);
        glCallList(LightBallList);
    glPopMatrix();
    // Draw tracking circle if enabled
    if (CircleOn) {
        DrawCircle(lightX, lightY, lightZ, 0.6f, 64);
    }
    glEnable(GL_LIGHTING);

    // Axes
    if (AxesOn) {
        glDisable(GL_LIGHTING);
        glCallList(AxesList);
        glEnable(GL_LIGHTING);
    }

    // Draw OBJ model with lighting

	// --- BUNNY ---
	// Bunny center: (0.0, 0.437, 0.0), span: (1.287, 0.875, 1.178)
	// Largest span: 1.287 (X)
    glPushMatrix();
        glTranslatef(2.5f, 5.0f, 2.f); 
        glScalef(20.f, 20.f, 20.f);  
        SetMaterial(1.f, 0.8f, 0.6f, 10.f, 1); 
        if (BunnyObj) glCallList(BunnyObj);
    glPopMatrix();

    // --- DINO ---
    // Dino center: (-1.442, 0.186, 0.016), span: (17.716, 7.755, 5.857)
    // Largest span: 17.716 (X)
    float dinoTarget = 5.0f;
    float dinoScale = dinoTarget / 17.716f;
    glPushMatrix();
        glTranslatef(-2.5f, 5.5f, -2.f); 
        glScalef(dinoScale, dinoScale, dinoScale);
        glTranslatef(1.442f, -0.186f, -0.016f); 
        SetMaterial(0.6f, 1.f, 0.6f, 10.f, 1);
        if (DinoObj) glCallList(DinoObj);
    glPopMatrix();

    // --- SALMON ---
    // Salmon center: (0.0, 0.0, -0.858), span: (0.946, 1.895, 4.902)
    // Largest span: 4.902 (Z)
    float salmonTarget = 5.0f;
    float salmonScale = salmonTarget / 4.902f;
    glPushMatrix();
        glTranslatef(2.5f, 6.0f, -2.f); 
        glScalef(salmonScale, salmonScale, salmonScale);
        glTranslatef(0.0f, 0.0f, 0.858f); 
        SetMaterial(0.6f, 0.6f, 1.f, 10.f, 1);
        if (SalmonObj) glCallList(SalmonObj);
    glPopMatrix();

    glutSwapBuffers();
}


void
DoAxesMenu( int id )
{
	AxesOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoColorMenu( int id )
{
	NowColor = id - RED;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDebugMenu( int id )
{
	DebugOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthBufferMenu( int id )
{
	DepthBufferOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthFightingMenu( int id )
{
	DepthFightingOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthMenu( int id )
{
	DepthCueOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// main menu callback:

void
DoMainMenu( int id )
{
	switch( id )
	{
		case RESET:
			Reset( );
			break;

		case QUIT:
			// gracefully close out the graphics:
			// gracefully close the graphics window:
			// gracefully exit the program:
			glutSetWindow( MainWindow );
			glFinish( );
			glutDestroyWindow( MainWindow );
			exit( 0 );
			break;

		default:
			fprintf( stderr, "Don't know what to do with Main Menu ID %d\n", id );
	}

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoProjectMenu( int id )
{
	NowProjection = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// use glut to display a string of characters using a raster font:

void
DoRasterString( float x, float y, float z, char *s )
{
	glRasterPos3f( (GLfloat)x, (GLfloat)y, (GLfloat)z );

	char c;			// one character to print
	for( ; ( c = *s ) != '\0'; s++ )
	{
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, c );
	}
}


// use glut to display a string of characters using a stroke font:

void
DoStrokeString( float x, float y, float z, float ht, char *s )
{
	glPushMatrix( );
		glTranslatef( (GLfloat)x, (GLfloat)y, (GLfloat)z );
		float sf = ht / ( 119.05f + 33.33f );
		glScalef( (GLfloat)sf, (GLfloat)sf, (GLfloat)sf );
		char c;			// one character to print
		for( ; ( c = *s ) != '\0'; s++ )
		{
			glutStrokeCharacter( GLUT_STROKE_ROMAN, c );
		}
	glPopMatrix( );
}


// return the number of seconds since the start of the program:

float
ElapsedSeconds( )
{
	// get # of milliseconds since the start of the program:

	int ms = glutGet( GLUT_ELAPSED_TIME );

	// convert it to seconds:

	return (float)ms / 1000.f;
}



// initialize the glut and OpenGL libraries:
//	also setup callback functions

void
InitGraphics( )
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitGraphics.\n");

	// request the display modes:
	// ask for red-green-blue-alpha color, double-buffering, and z-buffering:

	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

	// set the initial window configuration:

	glutInitWindowPosition( 0, 0 );
	glutInitWindowSize( INIT_WINDOW_SIZE, INIT_WINDOW_SIZE );

	// open the window and set its title:

	MainWindow = glutCreateWindow( WINDOWTITLE );
	glutSetWindowTitle( WINDOWTITLE );

	// set the framebuffer clear values:

	glClearColor( BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3] );

	// setup the callback functions:
	// DisplayFunc -- redraw the window
	// ReshapeFunc -- handle the user resizing the window
	// KeyboardFunc -- handle a keyboard input
	// MouseFunc -- handle the mouse button going down or up
	// MotionFunc -- handle the mouse moving with a button down
	// PassiveMotionFunc -- handle the mouse moving with a button up
	// VisibilityFunc -- handle a change in window visibility
	// EntryFunc	-- handle the cursor entering or leaving the window
	// SpecialFunc -- handle special keys on the keyboard
	// SpaceballMotionFunc -- handle spaceball translation
	// SpaceballRotateFunc -- handle spaceball rotation
	// SpaceballButtonFunc -- handle spaceball button hits
	// ButtonBoxFunc -- handle button box hits
	// DialsFunc -- handle dial rotations
	// TabletMotionFunc -- handle digitizing tablet motion
	// TabletButtonFunc -- handle digitizing tablet button hits
	// MenuStateFunc -- declare when a pop-up menu is in use
	// TimerFunc -- trigger something to happen a certain time from now
	// IdleFunc -- what to do when nothing else is going on

	glutSetWindow( MainWindow );
	glutDisplayFunc( Display );
	glutReshapeFunc( Resize );
	glutKeyboardFunc( Keyboard );
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	glutPassiveMotionFunc(MouseMotion);
	//glutPassiveMotionFunc( NULL );
	glutVisibilityFunc( Visibility );
	glutEntryFunc( NULL );
	glutSpecialFunc( NULL );
	glutSpaceballMotionFunc( NULL );
	glutSpaceballRotateFunc( NULL );
	glutSpaceballButtonFunc( NULL );
	glutButtonBoxFunc( NULL );
	glutDialsFunc( NULL );
	glutTabletMotionFunc( NULL );
	glutTabletButtonFunc( NULL );
	glutMenuStateFunc( NULL );
	glutTimerFunc( -1, NULL, 0 );

	// setup glut to call Animate( ) every time it has
	// 	nothing it needs to respond to (which is most of the time)
	// we don't need to do this for this program, and really should set the argument to NULL
	// but, this sets us up nicely for doing animation

	glutIdleFunc( Animate );

	// init the glew package (a window must be open to do this):

#ifdef WIN32
	GLenum err = glewInit( );
	if( err != GLEW_OK )
	{
		fprintf( stderr, "glewInit Error\n" );
	}
	else
		fprintf( stderr, "GLEW initialized OK\n" );
	fprintf( stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

	// all other setups go here, such as GLSLProgram and KeyTime setups:

}


// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

void InitLists()
{
    if (DebugOn != 0)
        fprintf(stderr, "Starting InitLists.\n");

   #define XSIDE 20.0f
	#define ZSIDE 20.0f
	#define NX 40
	#define NZ 40
	#define DX (XSIDE / (float)NX)
	#define DZ (ZSIDE / (float)NZ)
	#define X0 (-XSIDE / 2.0f)
	#define Z0 (-ZSIDE / 2.0f)
	#define YGRID 0.0f

	// Floor 
	GridDL = glGenLists(1);
	glNewList(GridDL, GL_COMPILE);
		SetMaterial(0.4f, 0.35f, 0.35f, 20.f, 1); // floor 
		glNormal3f(0.f, 1.f, 0.f);
		for (int i = 0; i < NZ; i++) {
			glBegin(GL_QUAD_STRIP);
			for (int j = 0; j <= NX; j++) {
				glVertex3f(X0 + DX * j, YGRID, Z0 + DZ * i);
				glVertex3f(X0 + DX * j, YGRID, Z0 + DZ * (i + 1));
			}
			glEnd();
		}
	glEndList();

	// Back wall (Z = Z0)
	WallBackDL = glGenLists(1);
	glNewList(WallBackDL, GL_COMPILE);
		SetMaterial(0.3f, 0.45f, 0.3f, 5.f, 1); // wall 
		glNormal3f(0.f, 0.f, 1.f);
		for (int i = 0; i < NZ; i++) {
			glBegin(GL_QUAD_STRIP);
			for (int j = 0; j <= NX; j++) {
				glVertex3f(X0 + DX * j, YGRID + DZ * i, Z0);
				glVertex3f(X0 + DX * j, YGRID + DZ * (i + 1), Z0);
			}
			glEnd();
		}
	glEndList();

	// Left wall (X = X0)
	WallRightDL = glGenLists(1);
	glNewList(WallRightDL, GL_COMPILE);
		SetMaterial(0.35f, 0.4f, 0.6f, 5.f, 1); // wall
		glNormal3f(1.f, 0.f, 0.f);
		for (int i = 0; i < NZ; i++) {
			glBegin(GL_QUAD_STRIP);
			for (int j = 0; j <= NX; j++) {
				glVertex3f(X0, YGRID + DZ * i, Z0 + DX * j);
				glVertex3f(X0, YGRID + DZ * (i + 1), Z0 + DX * j);
			}
			glEnd();
		}
	glEndList();

    // ---------- AXES ----------
    AxesList = glGenLists(1);
    glNewList(AxesList, GL_COMPILE);
        glLineWidth(AXES_WIDTH);
        Axes(2.5);
        glLineWidth(1.);
    glEndList();

    // ---------- LIGHT BALL ----------
    LightBallList = glGenLists(1);
    glNewList(LightBallList, GL_COMPILE);
        glColor3f(1.0f, 1.0f, 1.0f);
        glutSolidSphere(0.15f, 20, 20);
    glEndList();

    // Load OBJ model
    BunnyObj = LoadObjMtlFiles((char*)"bunny.obj");
	DinoObj = LoadObjMtlFiles((char*)"dino.obj");
	SalmonObj = LoadObjMtlFiles((char*)"salmon.obj");

    if (DebugOn != 0)
        fprintf(stderr, "Finished InitLists.\n");
}




// initialize the glui window:

void
InitMenus( )
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitMenus.\n");

	glutSetWindow( MainWindow );

	int numColors = sizeof( Colors ) / ( 3*sizeof(float) );
	int colormenu = glutCreateMenu( DoColorMenu );
	for( int i = 0; i < numColors; i++ )
	{
		glutAddMenuEntry( ColorNames[i], i );
	}

	int axesmenu = glutCreateMenu( DoAxesMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthcuemenu = glutCreateMenu( DoDepthMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthbuffermenu = glutCreateMenu( DoDepthBufferMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthfightingmenu = glutCreateMenu( DoDepthFightingMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int debugmenu = glutCreateMenu( DoDebugMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int projmenu = glutCreateMenu( DoProjectMenu );
	glutAddMenuEntry( "Orthographic",  ORTHO );
	glutAddMenuEntry( "Perspective",   PERSP );

	int mainmenu = glutCreateMenu( DoMainMenu );
	glutAddSubMenu(   "Axes",          axesmenu);
	glutAddSubMenu(   "Axis Colors",   colormenu);

#ifdef DEMO_DEPTH_BUFFER
	glutAddSubMenu(   "Depth Buffer",  depthbuffermenu);
#endif

#ifdef DEMO_Z_FIGHTING
	glutAddSubMenu(   "Depth Fighting",depthfightingmenu);
#endif

	glutAddSubMenu(   "Depth Cue",     depthcuemenu);
	glutAddSubMenu(   "Projection",    projmenu );
	glutAddMenuEntry( "Reset",         RESET );
	glutAddSubMenu(   "Debug",         debugmenu);
	glutAddMenuEntry( "Quit",          QUIT );

// attach the pop-up menu to the right mouse button:

	glutAttachMenu( GLUT_RIGHT_BUTTON );
}


// ---------------- Keyboard controls ----------------
void Keyboard(unsigned char ch, int x, int y)
{
    switch (ch) {
        case 'f': case 'F':
            Frozen = !Frozen;
            if (Frozen) glutIdleFunc(NULL);
            else glutIdleFunc(Animate);
            break;
        case 'l': // toggle spot/point light
            LightIsSpot = !LightIsSpot;
            break;
        case 'w': // white
           LightColor[0] = LightColor[1] = LightColor[2] = 1.f; break;
        case 'r': // red
            LightColor[0] = 1.f; LightColor[1] = LightColor[2] = 0.f; break;
        case 'o': // orange
            LightColor[0] = 1.f; LightColor[1] = 0.5f; LightColor[2] = 0.f; break;
        case 'y': // yellow
            LightColor[0] = 1.f; LightColor[1] = 1.f; LightColor[2] = 0.f; break;
        case 'g': // green
            LightColor[0] = 0.f; LightColor[1] = 1.f; LightColor[2] = 0.f; break;
        case 'c': // cyan
            LightColor[0] = 0.f; LightColor[1] = 1.f; LightColor[2] = 1.f; break;
        case 'm': // magenta
            LightColor[0] = 1.f; LightColor[1] = 0.f; LightColor[2] = 1.f; break;
        case 't': // toggle tracking circle
            CircleOn = !CircleOn;
            break;
        case 27:
            exit(0);
            break;
    }
    glutPostRedisplay();
}


// Utility to draw a circle in XZ plane at (cx, cy, cz)
void DrawCircle(float cx, float cy, float cz, float radius, int segments) {
    glColor3f(1.f, 1.f, 0.f); // yellow
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segments; ++i) {
        float theta = 2.0f * M_PI * float(i) / float(segments);
        float x = radius * cosf(theta);
        float z = radius * sinf(theta);
        glVertex3f(cx + x, cy, cz + z);
    }
    glEnd();
}

// called when the mouse button transitions down or up:

void
MouseButton( int button, int state, int x, int y )
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	if( DebugOn != 0 )
		fprintf( stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y );

	
	// get the proper button bit mask:

	switch( button )
	{
		case GLUT_LEFT_BUTTON:
			b = LEFT;		break;

		case GLUT_MIDDLE_BUTTON:
			b = MIDDLE;		break;

		case GLUT_RIGHT_BUTTON:
			b = RIGHT;		break;

		case SCROLL_WHEEL_UP:
			printf("Scroll wheel up\n");
			Scale += SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
			// keep object from turning inside-out or disappearing:
			if (Scale < MINSCALE)
				Scale = MINSCALE;
			break;

		case SCROLL_WHEEL_DOWN:
			printf("Scroll wheel down\n");
			Scale -= SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
			// keep object from turning inside-out or disappearing:
			if (Scale < MINSCALE)
				Scale = MINSCALE;
			break;

		default:
			b = 0;
			fprintf( stderr, "Unknown mouse button: %d\n", button );
	}

	// button down sets the bit, up clears the bit:

	if( state == GLUT_DOWN )
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}

	glutSetWindow(MainWindow);
	glutPostRedisplay();

}


// called when the mouse moves while a button is down:

void
MouseMotion( int x, int y )
{
	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if( ( ActiveButton & LEFT ) != 0 )
	{
		Xrot += ( ANGFACT*dy );
		Yrot += ( ANGFACT*dx );
	}

	if( ( ActiveButton & MIDDLE ) != 0 )
	{
		Scale += SCLFACT * (float) ( dx - dy );

		// keep object from turning inside-out or disappearing:

		if( Scale < MINSCALE )
			Scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void
Reset( )
{
	ActiveButton = 0;
	AxesOn = 1;
	DebugOn = 0;
	DepthBufferOn = 1;
	DepthFightingOn = 0;
	DepthCueOn = 0;
	Scale  = 1.0;
	ShadowsOn = 0;
	NowColor = YELLOW;
	NowProjection = PERSP;
	Xrot = Yrot = 0.;
	Frozen = false;
}


// called when user resizes the window:

void
Resize( int width, int height )
{
	// don't really need to do anything since window size is
	// checked each time in Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// handle a change to the window's visibility:

void
Visibility ( int state )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Visibility: %d\n", state );

	if( state == GLUT_VISIBLE )
	{
		glutSetWindow( MainWindow );
		glutPostRedisplay( );
	}
	else
	{
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}



///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////


// the stroke characters 'X' 'Y' 'Z' :

static float xx[ ] = { 0.f, 1.f, 0.f, 1.f };

static float xy[ ] = { -.5f, .5f, .5f, -.5f };

static int xorder[ ] = { 1, 2, -3, 4 };

static float yx[ ] = { 0.f, 0.f, -.5f, .5f };

static float yy[ ] = { 0.f, .6f, 1.f, 1.f };

static int yorder[ ] = { 1, 2, 3, -2, 4 };

static float zx[ ] = { 1.f, 0.f, 1.f, 0.f, .25f, .75f };

static float zy[ ] = { .5f, .5f, -.5f, -.5f, 0.f, 0.f };

static int zorder[ ] = { 1, 2, 3, 4, -5, 6 };

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void Axes(float length) {
    
    glPushMatrix();
    glTranslatef(0.0f, 6.5f, 0.0f); // axes offset

    // X axis (yellow)
    glColor3fv(Colors[YELLOW]);
    glBegin(GL_LINE_STRIP);
        glVertex3f(0., 0., 0.);
        glVertex3f(length, 0., 0.);
    glEnd();
    // Y axis (yellow)
    glColor3fv(Colors[YELLOW]);
    glBegin(GL_LINE_STRIP);
        glVertex3f(0., 0., 0.);
        glVertex3f(0., length, 0.);
    glEnd();
    // Z axis (yellow)
    glColor3fv(Colors[YELLOW]);
    glBegin(GL_LINE_STRIP);
        glVertex3f(0., 0., 0.);
        glVertex3f(0., 0., length);
    glEnd();

    // Draw X, Y, Z labels in white
    glColor3f(1.f, 1.f, 1.f);
    float fact = LENFRAC * length;
    float base = BASEFRAC * length;
    // X label
    glBegin(GL_LINE_STRIP);
        for(int i = 0; i < 4; i++) {
            int j = xorder[i];
            if(j < 0) { glEnd(); glBegin(GL_LINE_STRIP); j = -j; }
            j--;
            glVertex3f(base + fact*xx[j], fact*xy[j], 0.0);
        }
    glEnd();
    // Y label
    glBegin(GL_LINE_STRIP);
        for(int i = 0; i < 5; i++) {
            int j = yorder[i];
            if(j < 0) { glEnd(); glBegin(GL_LINE_STRIP); j = -j; }
            j--;
            glVertex3f(fact*yx[j], base + fact*yy[j], 0.0);
        }
    glEnd();
    // Z label
    glBegin(GL_LINE_STRIP);
        for(int i = 0; i < 6; i++) {
            int j = zorder[i];
            if(j < 0) { glEnd(); glBegin(GL_LINE_STRIP); j = -j; }
            j--;
            glVertex3f(0.0, fact*zy[j], base + fact*zx[j]);
        }
    glEnd();

    glPopMatrix();
}


// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

void
HsvRgb( float hsv[3], float rgb[3] )
{
	// guarantee valid input:

	float h = hsv[0] / 60.f;
	while( h >= 6. )	h -= 6.;
	while( h <  0. ) 	h += 6.;

	float s = hsv[1];
	if( s < 0. )
		s = 0.;
	if( s > 1. )
		s = 1.;

	float v = hsv[2];
	if( v < 0. )
		v = 0.;
	if( v > 1. )
		v = 1.;

	// if sat==0, then is a gray:

	if( s == 0.0 )
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}

	// get an rgb from the hue itself:
	
	float i = (float)floor( h );
	float f = h - i;
	float p = v * ( 1.f - s );
	float q = v * ( 1.f - s*f );
	float t = v * ( 1.f - ( s * (1.f-f) ) );

	float r=0., g=0., b=0.;			// red, green, blue
	switch( (int) i )
	{
		case 0:
			r = v;	g = t;	b = p;
			break;
	
		case 1:
			r = q;	g = v;	b = p;
			break;
	
		case 2:
			r = p;	g = v;	b = t;
			break;
	
		case 3:
			r = p;	g = q;	b = v;
			break;
	
		case 4:
			r = t;	g = p;	b = v;
			break;
	
		case 5:
			r = v;	g = p;	b = q;
			break;
	}


	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}

void
Cross(float v1[3], float v2[3], float vout[3])
{
	float tmp[3];
	tmp[0] = v1[1] * v2[2] - v2[1] * v1[2];
	tmp[1] = v2[0] * v1[2] - v1[0] * v2[2];
	tmp[2] = v1[0] * v2[1] - v2[0] * v1[1];
	vout[0] = tmp[0];
	vout[1] = tmp[1];
	vout[2] = tmp[2];
}

float
Dot(float v1[3], float v2[3])
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}


float
Unit(float vin[3], float vout[3])
{
	float dist = vin[0] * vin[0] + vin[1] * vin[1] + vin[2] * vin[2];
	if (dist > 0.0)
	{
		dist = sqrtf(dist);
		vout[0] = vin[0] / dist;
		vout[1] = vin[1] / dist;
		vout[2] = vin[2] / dist;
	}
	else
	{
		vout[0] = vin[0];
		vout[1] = vin[1];
		vout[2] = vin[2];
	}
	return dist;
}


float
Unit( float v[3] )
{
	float dist = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	if (dist > 0.0)
	{
		dist = sqrtf(dist);
		v[0] /= dist;
		v[1] /= dist;
		v[2] /= dist;
	}
	return dist;
}
