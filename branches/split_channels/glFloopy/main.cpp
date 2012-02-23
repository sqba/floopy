#include <string.h>
#include <stdlib.h>

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "engine/engine.h"
CEngine m_Engine;

/* ASCII code for the various keys used */
#define ESCAPE			27     /* esc */
#define ROTyp			105    /* i   */
#define ROTym			109    /* m   */
#define ROTxp			107    /* k   */
#define ROTxm			106    /* j   */
#define SCAp			43     /* +   */
#define SCAm			45     /* -   */
/* The mouse buttons */
#define LMB				0	/* Left */
#define MMB				1   /* Middle */
#define RMB				2   /* Right */
/* Number of samples for frame rate */
#define FR_SAMPLES		10
/* Other Constants */
#define PI				3.14159265358979323846
#define FOV				30   /* Field of view for perspective mode */
#define PCR				2  /* Helps pan adapt to varying model sizes */


char arg1[100], arg2[100], arg3[100], window_name[120];
//std::vector<CSolidBody> bodies;
int window; /* The number of our GLUT window */
float scale = 0;
float ROTx = 0, ROTy = 0;
float PANx = 0, PANy = 0;
int MOUSEx = 0, MOUSEy = 0, BUTTON = 0;
/* Stuff for the frame rate calculation */
int FrameCount=0;
float FrameRate=0;
/* Settings for the light  */
float oScale = 1.0;
bool update = true, idle_draw = false;
float Z_Depth=-500, Big_Extent=300;
int window_height = 480;




void list_hits(GLint hits, GLuint *names)
{
	int i;

	/*
	For each hit in the buffer are allocated 4 bytes:
	1. Number of hits selected (always one,
	beacuse when we draw each object
	we use glLoadName, so we replace the
	prevous name in the stack)
	2. Min Z
	3. Max Z
	4. Name of the hit (glLoadName)
	*/

//	printf("%d hits:\n", hits);
/*
	for (i = 0; i < hits; i++)
	{
		GLubyte id = (GLubyte)names[i * 4 + 3];
		std::vector<CSolidBody>::iterator it;
		for(it=bodies.begin(); it<bodies.end(); it++)
		{
			CSolidBody &body = *it;
			body.select( id == body.id() );
		}
		//printf(	"Number: %d\n"
		//"Min Z: %d\n"
		//"Max Z: %d\n"
		//"Name on stack: %d\n",
		//(GLubyte)names[i * 4],
		//(GLubyte)names[i * 4 + 1],
		//(GLubyte)names[i * 4 + 2],
		//(GLubyte)names[i * 4 + 3]);
	}

	printf("\n");
*/
}

void DrawGLScene();

void gl_select(int x, int y)
{
	GLuint buff[64] = {0};
	GLint hits, view[4];
	int id;

	// This choose the buffer where store the values for the selection data
	glSelectBuffer(64, buff);

	// This retrieves info about the viewport
	glGetIntegerv(GL_VIEWPORT, view);

	// Switching in selecton mode
	glRenderMode(GL_SELECT);

	// Clearing the names' stack
	// This stack contains all the info about the objects
	glInitNames();

	// Now fill the stack with one element (or glLoadName will generate an error)
	glPushName(0);

	// Now modify the viewing volume, restricting selection area around the cursor
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	// restrict the draw to an area around the cursor
	gluPickMatrix(x, y, 1.0, 1.0, view);
	gluPerspective(60, (float)view[2]/(float)view[3], 0.0001, 1000.0);

	// Draw the objects onto the screen
	glMatrixMode(GL_MODELVIEW);

	// draw only the names in the stack, and fill the array
//	gl_draw();
	DrawGLScene();

	// Do you remeber? We do pushMatrix in PROJECTION mode
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	//get number of objects drawed in that area and return to render mode
	hits = glRenderMode(GL_RENDER);

	// Print a list of the objects
	list_hits(hits, buff);

	// uncomment this to show the whole buffer
	//gl_selall(hits, buff);

	glMatrixMode(GL_MODELVIEW);
}





/* The function called whenever a mouse button event occurs */
void mouseButtonPress(int button, int state, int x, int y)
{
	BUTTON = button;
	MOUSEx = x;
	MOUSEy = y;
	update = true;
}

/* The function called whenever a mouse motion event occurs */
void mouseMotionPress(int x, int y)
{
	switch(BUTTON)
	{
	case MMB:
		PANx = PANx + ((MOUSEx - x)*(tan(0.26179939)*(Z_Depth+scale)))*.005;
		PANy = PANy - ((MOUSEy - y)*(tan(0.26179939)*(Z_Depth+scale)))*.005;
		MOUSEx = x;
		MOUSEy = y;
		break;
	case LMB:
		ROTy = ROTy - ((MOUSEx - x)*0.5);
		ROTx = ROTx - ((MOUSEy - y)*0.5);
		MOUSEx = x;
		MOUSEy = y;
		break;
	case RMB:
		scale = scale + ((MOUSEy - y)*(tan(0.26179939)*(Z_Depth+scale)))*.01;
		oScale = oScale + ((MOUSEy - y)*(tan(0.26179939)*(Z_Depth+scale)))*.01;
		MOUSEx = x;
		MOUSEy = y;
		break;
	}
	update = true;
}

/* The function called whenever a key is pressed. */
void keyPressed(unsigned char key, int x, int y)
{
	if (key == ESCAPE)
	{
		glutDestroyWindow(window);
		exit(0);
	}
	update = true;
}

/* This function is for the special keys.  */
/* The dynamic viewing keys need to be time based */
void specialkeyPressed (int key, int x, int y)
{
	/* keep track of time between calls, if it exceeds a certian value, then */
	/* assume the user has released the key and wants to start fresh */
	static bool first = true;
	static clock_t last = 0;
	clock_t now;
	float delta;

	/* Properly initialize the MOUSE vars on first time through this function */
	if (first)
	{
		first = false;
		MOUSEx = x;
		MOUSEy = y;
	}

	/* If the clock exceeds a reasonable value, assume user has released F key */
	now  = clock();
	delta= (now - last) / (float) CLOCKS_PER_SEC;
	last = now;
	if (delta > 0.1)
	{
		MOUSEx = x;
		MOUSEy = y;

	}

	switch(key)
	{
	case 1:
		/* Pan is assigned the F1 key */
		PANx = PANx + ((MOUSEx - x)*(tan(0.26179939)*(Z_Depth+scale)))*.005;
		PANy = PANy - ((MOUSEy - y)*(tan(0.26179939)*(Z_Depth+scale)))*.005;
		MOUSEx = x;
		MOUSEy = y;
		break;
	case 2:
		/* Zoom or Scale is the F2 key */
		scale = scale + ((MOUSEy - y)*(tan(0.26179939)*(Z_Depth+scale)))*.01;
		oScale = oScale + ((MOUSEy - y)*(tan(0.26179939)*(Z_Depth+scale)))*.01;
		MOUSEx = x;
		MOUSEy = y;
		break;
	case 3:
		/* Rotate assigned the F3 key */
		ROTy = ROTy - ((MOUSEx - x)*0.5);
		ROTx = ROTx - ((MOUSEy - y)*0.5);
		MOUSEx = x;
		MOUSEy = y;
		break;
	case 4:
		glPolygonMode(GL_FRONT, GL_FILL);
		break;
	case 5:
		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_FILL);
		break;
	case 6:
		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_POINT);
		break;
	case 7:
		glDisable(GL_CULL_FACE);
		break;
	case 8:
		glEnable(GL_CULL_FACE);
		break;
//	case 9:
//		gl_select(x, window_height-y); //Important: gl (0,0) ist bottom left but window coords (0,0) are top left so we have to change this!
//		update = false;
//		return;
	}
	update = true;
}


/* Sets up Projection matrix according to command switch -o or -p */
/* called from initgl and the window resize function */
void SetView(int Width, int Height)
{
	window_height = Height;
	gluPerspective(FOV, (GLfloat)Width/(GLfloat)Height, 0.1f, (Z_Depth + Big_Extent));
}

/* Frame rate counter.  Based off of the Oreilly OGL demo !  */
/* updates the global variables FrameCount & FrameRate each time it is called. */
/* called from the main drawing function */
void GetFPS()
{
	static clock_t last=0;
	clock_t now;
	float delta;

	if (++FrameCount >= FR_SAMPLES)
	{
		now  = clock();
		delta= (now - last) / (float) CLOCKS_PER_SEC;
		last = now;

		FrameRate = FR_SAMPLES / delta;
		FrameCount = 0;
	}
}

/* The function called when our window is resized  */
void ReSizeGLScene(int Width, int Height)
{
	if (Height==0)	/* Prevent A Divide By Zero If The Window Is Too Small*/
		Height = 1;

	glViewport(0, 0, Width, Height);    /* Reset The Current Viewport And Perspective Transformation*/
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	SetView(Width, Height);

	glMatrixMode(GL_MODELVIEW);
	update = true;
}

void DrawAxes()
{
	int len = 20;

	// Draw coordinates
	glBegin(GL_LINES);

	// X axis
	glColor3f(1.f, 0.f, 0.f);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(len, 0.f, 0.f);
	// Y axis
	glColor3f(0.f, 1.f, 0.f);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(0.f, len, 0.f);
	// Z axis
	glColor3f(0.f, 0.f, 1.f);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(0.f, 0.f, len);

	glEnd();
}

void DrawEngine()
{
//	glLoadName( pbody->id() );

    m_Engine.DrawFrame();
}

void DrawControls()
{
	int screenWidth, screenHeight, windowWidth, windowHeight;
	screenWidth = glutGet(GLUT_SCREEN_WIDTH);
	screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
	windowWidth = glutGet(GLUT_WINDOW_WIDTH);
	windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

	glMatrixMode (GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity ();
	gluOrtho2D(0, windowWidth, 0, windowHeight);
	//glTranslatef (0.375, 0.375, 0.);
	glMatrixMode (GL_MODELVIEW);

	//glDisable(GL_DEPTH_TEST) or glDepthFunc (GL_ALWAYS)

//	glMatrixMode (GL_PROJECTION);
//	glOrtho (0, screenWidth, screenHeight, 0, 0, 1);
//	glMatrixMode (GL_MODELVIEW);
	glDisable(GL_DEPTH_TEST);
	glLoadIdentity ();

	float fWidth = 100.f;
	glColor3f(1.0f, 0.0f, 0.0f);
	glRectf(0.f, 1.f, fWidth, windowHeight-1);

	glColor3f(0.0f, 1.0f, 0.0f);
	glRectf(windowWidth-fWidth, 1.f, windowWidth, windowHeight-1);

	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

/* The main drawing function. */
void DrawGLScene()
{
	if ((!update) && (!idle_draw))
		return;
	update = false;

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);	/* Clear The Screen And The Depth Buffer*/

//	glEnable (GL_BLEND);
//	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	//glBlendFunc(GL_ONE_MINUS_DST_ALPHA,GL_DST_ALPHA);

	glLoadIdentity();

    glTranslatef(PANx, PANy, (Z_Depth + scale));
    glRotatef(ROTx, 1.0f, 0.0f, 0.0f);
    glRotatef(ROTy, 0.0f, 1.0f, 0.0f);

	DrawEngine();

	DrawAxes();

	DrawControls();

	glutSwapBuffers(); /* swap the buffers to display, since double buffering is used.*/

	GetFPS();  /* Get frame rate stats */

	if (FrameCount == 0)
	{
		sprintf(window_name, "%.2f FPS", FrameRate);
		glutSetWindowTitle(window_name);
	}
}

int main(int argc, char *argv[])
{
	// Window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(0, 0);
	window = glutCreateWindow(arg1);

	// Callbacks
	glutDisplayFunc(&DrawGLScene);
	glutIdleFunc(&DrawGLScene);
	glutReshapeFunc(&ReSizeGLScene);
	glutKeyboardFunc(&keyPressed);
	glutSpecialFunc(&specialkeyPressed);
	glutMouseFunc(&mouseButtonPress);
	glutMotionFunc(&mouseMotionPress);

	glClearColor(0.4f, 0.4f, 0.4f, 0.0f);
	glClearDepth(1.0);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	SetView(640, 480);

	glMatrixMode(GL_MODELVIEW);
/*
	// Lights
	glEnable(GL_LIGHTING);
	float Light_Ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	float Light_Diffuse[] = { 1.2f, 1.2f, 1.2f, 1.0f };
	float Light_Position[]= { 2.0f, 2.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT1, GL_POSITION, Light_Position);
	glLightfv(GL_LIGHT1, GL_AMBIENT,  Light_Ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE,  Light_Diffuse);
	glEnable (GL_LIGHT1);
*/
	glEnable(GL_DEPTH_TEST); /* enable depth buffering */
	glDepthFunc(GL_LESS);    /* pedantic, GL_LESS is the default */
	glClearDepth(1.0);       /* pedantic, 1.0 is the default */

	/* frame buffer clears should be to black */
	glClearColor(0.0, 0.0, 0.0, 0.0);

	/* set up projection transform */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 10.0);
	/* establish initial viewport */
	/* pedantic, full window size is default viewport */
	glViewport(0, 0, 300, 300);

/*	2D
	glOrtho (0, 640, 480, 0, 0, 1);
	glDisable(GL_DEPTH_TEST);
*/

	CTrack *pTrack = m_Engine.AddTrack();
	CRegion *pRegion = pTrack->AddRegion();

	glutMainLoop();

	return 1;
}
