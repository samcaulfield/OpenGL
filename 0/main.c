#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>

#define TBUF_SZ 64
char tbuf[TBUF_SZ];

uint16_t winw = 800, winh = 800, frame = 0;

void display(void);
void idle(void);
void resize(int, int);
void timer(int);

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitContextVersion(4, 0);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,
		GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutInitWindowSize(winw, winh);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	if (glutCreateWindow("Waiting for framerate data...") < 1) {
		fprintf(stderr, "ERROR: Could not create a window.\n");
		return EXIT_FAILURE;
	}
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(resize);
	glutTimerFunc(0, timer, 0);
	GLenum result = glewInit();
	if (result != GLEW_OK) {
		fprintf(stderr, "ERROR: %s\n", glewGetErrorString(result));
		return EXIT_FAILURE;
	}
	fprintf(stdout, "INFO: OpenGL Version: %s\n", glGetString(GL_VERSION));
	glutMainLoop();
	return EXIT_SUCCESS;
}

void display(void)
{
	frame++;
	glClear(GL_COLOR_BUFFER_BIT);
	glutSwapBuffers();
	glutPostRedisplay();
}

void idle(void)
{
	glutPostRedisplay();
}

void resize(int neww, int newh)
{
	winw = neww;
	winh = newh;
	glViewport(0, 0, winw, winh);
}

void timer(int x)
{
	if (x) {
		snprintf(tbuf, TBUF_SZ, "%d FPS @ %d x %d", frame, winw, winh);
		glutSetWindowTitle(tbuf);
	}
	frame = 0;
	glutTimerFunc(1000, timer, 1);
}

