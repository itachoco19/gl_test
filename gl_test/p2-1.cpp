#include <stdlib.h>
#include <gl/glut.h>

void myKeyboard(unsigned char key, int x, int y)
{
	if (key == 27)
	{
		exit(0);
	}
}

void myDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3d(1.0, 0.0, 0.0);

	glBegin(GL_POLYGON);
	glVertex2d(-0.5, -0.5);
	glVertex2d(0.5, -0.5);
	glVertex2d(0.0, 0.0);
	glVertex2d(0.5, 0.5);
	glVertex2d(-0.5, 0.5);
	glEnd();

	glFlush();
}

void myInit(char* progname)
{
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(500, 400);
	glutInitWindowPosition(0, 0);
	glutCreateWindow(progname);
	glClearColor(1.0, 1.0, 0.0, 0.0);
}

int main(int argc, char** argv)
{
	glutInit_ATEXIT_HACK(&argc, argv);
	myInit(argv[0]);
	glutKeyboardFunc(myKeyboard);
	glutDisplayFunc(myDisplay);
	glutMainLoop();
	return 0;
}