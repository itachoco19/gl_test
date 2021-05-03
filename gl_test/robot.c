/* List: p3-robot.c
* Robot arm with two links and two joints.
* Examination of world-local coordinates, modeling transfomation and
* operation of matrix stack.
*/
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>

static double shoulder_r = 0, elbow_r = 0;
static double shoulder_l = 0, elbow_l = 0;
static double shoulder_rz = 0, elbow_rz = 0, elbow_ry = 0;
static double shoulder_lz = 0, elbow_lz = 0, elbow_ly = 0;
static double list_r = 0, list_l = 0;
static double headangle = 0;
static double bodyangle = 0;
static int mode = 0;
const double speed = 0.1;
unsigned char mouseFlag = GL_FALSE;
int xStart, yStart;
double xAngle = 0.0, yAngle = 0.0;

float mtrl_ambient[] = { 0.1, 0, 0, 0 };
float mtrl_diffuse[] = { 0, 0, 0, 0 };
float mtrl_specular[] = { 0, 0, 0, 0 };
float mtrl_shininess[] = { 128.0 };

void mySetLight()
{
	float light0_position[] = { 1.0, 1.0, 1.0, 1.0 };	// point light source
	float light1_position[] = { -1.0, -1.0, 1.0, 1.0 };	// point light source
	float light1_ambient[] = { 0.0, 0.3, 0.0, 1.0 };
	float light1_diffuse[] = { 0.0, 0.1, 0.0, 1.0 };
	float light1_specular[] = { 0.0, 0.0, 1.0, 1.0 };

	/* Set up LIGHT0 which uses the default parameters except position */
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	/* Set up LIGHT1 */
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);

	glEnable(GL_LIGHT0);		// enable the 0th light
	glEnable(GL_LIGHT1);		// enable the 1st light
}

void myInit(char *progname)
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutCreateWindow(progname);
	glClearColor(0.0, 0.0, 0.0, 0.0);
}

void myArm_left(double x, double y, double z)
{
	glPushMatrix();
	{
		/* 2nd arm*/
		glTranslated(x, y, z);
		glRotated((double)shoulder_l, -1.0, 0.0, 0.0);
		glRotated(shoulder_lz, 0.0, 0.0, 1.0);
		glRotated(-90.0, 0.0, 0.0, 1.0);
		glTranslated(1.0, 0.0, 0.0);
		glPushMatrix();
		{
			glScaled(2.0, 0.4, 1.0);
			glutSolidCube(1.0);
		}
		glPopMatrix();
		/* 2nd link */
		glTranslated(1.0, 0.0, 0.0);				//move to the end of 1st link
		glRotated((double)elbow_lz, 0.0, 0.0, 1.0);
		glRotated((double)elbow_ly, 1.0, 0.0, 0.0);
		glRotated((double)elbow_l, 0.0, -1.0, 0.0);
		glTranslated(1.0, 0.0, 0.0);
		glPushMatrix();
		{
			glScaled(2.0, 0.4, 1.0);
			glutWireCube(1.0);
		}
		glPopMatrix();

		glTranslated(1.0, 0.0, 0.0);
		glRotated(list_l, 0.0, 0.0, 1.0);
		glTranslated(0.5, 0.0, 0.0);
		glPushMatrix();
		{
			glScaled(1.0, 0.4, 1.0);
			glutWireCube(1.0);
		}
		glPopMatrix();
	}
	glPopMatrix();
}

void myArm_right(double x, double y, double z)
{
	glPushMatrix();
	{
		/* 2nd arm*/
		glTranslated(x, y, z);
		glRotated((double)shoulder_r, -1.0, 0.0, 0.0);
		glRotated((double)shoulder_rz, 0.0, 0.0, 1.0);
		glRotated(-90.0, 0.0, 0.0, 1.0);
		glTranslated(1.0, 0.0, 0.0);
		glPushMatrix();
		{
			glScaled(2.0, 0.4, 1.0);
			glutWireCube(1.0);
		}
		glPopMatrix();
		/* 2nd link */
		glTranslated(1.0, 0.0, 0.0);				//move to the end of 1st link
		glRotated((double)elbow_ry, -1.0, 0.0, 0.0);
		glRotated((double)elbow_rz, 0.0, 0.0, -1.0);
		glRotated((double)elbow_r, 0.0, -1.0, 0.0);
		glTranslated(1.0, 0.0, 0.0);
		glPushMatrix();
		{
			glScaled(2.0, 0.4, 1.0);
			glutWireCube(1.0);
		}
		glPopMatrix();

		glTranslated(1.0, 0.0, 0.0);
		glRotated(list_r, 0.0, 0.0, 1.0);
		glTranslated(0.5, 0.0, 0.0);
		glPushMatrix();
		{
			glScaled(1.0, 0.4, 1.0);
			glutWireCube(1.0);
		}
		glPopMatrix();
	}
	glPopMatrix();
}

void myHead(double x, double y, double z)
{
	glPushMatrix();
	{
		glTranslated(x, y, z);
		glRotated(headangle, 0.0, 1.0, 0.0);
		glPushMatrix();
		{
			glScaled(1.0, 1.0, 0.8);
			glutWireCube(1.0);
		}
		glPopMatrix();

		glPushMatrix();
		{

			glutWireCube(1.0);
		}
		glPopMatrix();
	}
	glPopMatrix();
}

void myBody(double x, double y, double z)
{

	glPushMatrix();
	{
		glTranslated(x, y, z);
		glRotated(0.0, 0.0, 0.0, 0.0);
		glPushMatrix();
		{
			glScaled(4.6, 3.0, 1.0);
			glutWireCube(1.0);
		}
		glPopMatrix();
	}
	glPopMatrix();

}

void myRobot(double size, double x, double y, double z)
{
	glPushMatrix();
	{
		glTranslated(x, y, z);
		glScaled(size, size, size);
		glRotated(10 + bodyangle, 1.0, 0.0, 0.0);
		myArm_right(-2.5, 0.0, 0.0);
		myArm_left(2.5, 0.0, 0.0);
		myBody(0.0, -1.0, 0.0);
		myHead(0.0, 1.0, 0.0);
	}
	glPopMatrix();
}

void myIdle(void)
{
	switch (mode)
	{
	case 0:
		shoulder_r = fmod(shoulder_r + speed, 360.0);
		shoulder_l = fmod(shoulder_l + speed, 360.0);
		if (shoulder_r >= 90) mode = 1;
		break;
	case 1:
		elbow_ly = fmod(elbow_ly + (speed / 2.0), 360.0);
		elbow_ry = fmod(elbow_ry + (speed / 2.0), 360.0);
		elbow_l = fmod(elbow_l + (speed / 2.0), 360.0);
		elbow_r = fmod(elbow_r + (speed / 2.0), 360.0);
		list_l = fmod(list_l + (speed / 2.0), 360);
		list_r = fmod(list_r - (speed / 2.0), 360);
		if (elbow_ly >= 90) mode = 2;
		break;
	case 2:
		elbow_l = fmod(elbow_l - (speed / 2.0), 360.0);
		shoulder_lz = fmod(shoulder_lz - (speed / 2.0), 360.0);
		elbow_r = fmod(elbow_r - (speed / 2.0), 360.0);
		shoulder_rz = fmod(shoulder_rz + (speed / 2.0), 360.0);
		if (shoulder_rz >= 10) mode = 3;
		break;
	case 3:
		bodyangle = fmod(bodyangle + (speed / 10.0), 360);
		if (bodyangle >= 30.0) mode = 4;
		break;
	case 4:
		bodyangle = fmod(bodyangle - (speed / 10.0), 360.0);
		if (bodyangle <= 0.0) glutIdleFunc(NULL);
	}
	glutPostRedisplay();

}

void myDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	glPushMatrix();
	{
		glRotated(yAngle, 0.0, 1.0, 0.0);
		glRotated(xAngle, 1.0, 0.0, 0.0);
		myRobot(1.0, 0.0, 0.0, 0.0);
	}
	glPopMatrix();
	glutSwapBuffers();
}

void myReshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (double)width / (double)height, 0.1, 20.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(0.0, 0.0, -10.0);				// move to enable viewing
}

void myKeyboard(unsigned char key, int x, int y)
{

	switch (key) {
	case 'q':
		shoulder_r = fmod(shoulder_r + 5, 360.0);
		glutPostRedisplay();
		break;
	case 'Q':
		shoulder_r = fmod(shoulder_r - 5, 360);
		glutPostRedisplay();
		break;
	case 'a':
		elbow_r = fmod(elbow_r + 5, 360);
		glutPostRedisplay();
		break;
	case 'A':
		elbow_r = fmod(elbow_r - 5, 360);
		glutPostRedisplay();
		break;
	case 'w':
		shoulder_l = fmod(shoulder_l + 5, 360);
		glutPostRedisplay();
		break;
	case 'W':
		shoulder_l = fmod(shoulder_l - 5, 360);
		glutPostRedisplay();
		break;
	case 's':
		elbow_l = fmod(elbow_l + 5, 360);
		glutPostRedisplay();
		break;
	case 'S':
		elbow_l = fmod(elbow_l - 5, 360);
		glutPostRedisplay();
		break;
	case 'z':
		headangle = fmod(headangle + 5, 360);
		glutPostRedisplay();
		break;
	case 'Z':
		headangle = fmod(headangle - 5, 360);
		glutPostRedisplay();
		break;
	case 'e':
		shoulder_lz = fmod(shoulder_lz + 5, 360);
		glutPostRedisplay();
		break;
	case 'E':
		shoulder_lz = fmod(shoulder_lz - 5, 360);
		glutPostRedisplay();
		break;
	case 'd':
		elbow_lz = fmod(elbow_lz + 5, 360);
		glutPostRedisplay();
		break;
	case 'D':
		elbow_lz = fmod(elbow_lz - 5, 360);
		glutPostRedisplay();
		break;
	case 'r':
		shoulder_rz = fmod(shoulder_rz + 5, 360);
		glutPostRedisplay();
		break;
	case 'R':
		shoulder_rz = fmod(shoulder_rz - 5, 360);
		glutPostRedisplay();
		break;
	case 'f':
		elbow_rz = fmod(elbow_rz + 5, 360);
		glutPostRedisplay();
		break;
	case 'F':
		elbow_rz = fmod(elbow_rz - 5, 360);
		glutPostRedisplay();
		break;
	case 'x':
		list_r = fmod(list_r + 5, 360);
		glutPostRedisplay();
		break;
	case 'X':
		list_r = fmod(list_r - 5, 360);
		glutPostRedisplay();
		break;
	case 'c':
		list_l = fmod(list_l + 5, 360);
		glutPostRedisplay();
		break;
	case 27:
		exit(0);
		break;
	default:
		break;
	}
}

void myMouseMotion(int x, int y)
{
	int		xdis, ydis;
	double	a = 0.5;

	if (mouseFlag == GL_FALSE) return;
	xdis = x - xStart;
	ydis = y - yStart;
	/* To match mouse's movement and its rotation axis */
	xAngle += (double)ydis *a;
	yAngle += (double)xdis *a;

	xStart = x;
	yStart = y;
	glutPostRedisplay();
}

void myMouseFunc(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON  && state == GLUT_DOWN){
		xStart = x;
		yStart = y;
		mouseFlag = GL_TRUE;
	}
	else {
		mouseFlag = GL_FALSE;
	}
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	myInit(argv[0]);
	mySetLight();
	glutKeyboardFunc(myKeyboard);
	glutMouseFunc(myMouseFunc);
	glutMotionFunc(myMouseMotion);
	glutReshapeFunc(myReshape);
	glutIdleFunc(myIdle);
	glutDisplayFunc(myDisplay);
	glutMainLoop();
	return 0;
}
