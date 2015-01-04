#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

const float step = 0.04;

/* camera angle (Y) and pos. */
float canglex, cangley, cx, cy, cz;

float camtranso[] = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};

float camtransb[] = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};

static const double PI = 3.14159265358979323846;

GLuint fshaderid, vshaderid, programid, vao, vbo, ibo;

float angle;

float pers[] = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};

float trans[] = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, -2, 1
};

float rotY[] = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};

float viewX[] = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};

float viewY[] = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};

const GLchar *fshader = {
"#version 400\n"\

"in vec4 ex_Color;\n"\
"out vec4 out_Color;\n"\

"void main(void)\n"\
"{\n"\
"	out_Color = ex_Color;\n"\
"}\n"
};

/* model view projection = projection * view * model */

/* trans and rotY combine to form the model matrix */
const GLchar *vshader = {
"#version 400\n"\

"layout(location=0) in vec4 in_Position;\n"\
"layout(location=1) in vec4 in_Color;\n"\
"out vec4 ex_Color;\n"\

"uniform mat4 camtranso;\n"\
"uniform mat4 camtransb;\n"\
"uniform mat4 trans;\n"\
"uniform mat4 rotY;\n"\
"uniform mat4 pers;\n"\
"uniform mat4 viewX;\n"\
"uniform mat4 viewY;\n"\

"void main(void)\n"\
"{\n"\
"	gl_Position = (pers * (camtranso * (viewX * viewY) * camtransb) * (trans * rotY))  * in_Position;\n"\
"	ex_Color = in_Color;\n"\
"}\n"
};

#define TBUF_SZ 64
char tbuf[TBUF_SZ];

uint16_t winw = 800, winh = 800, frame = 0;

void createshaders(void);
void createvbo(void);
void display(void);
void idle(void);
void keyboard(unsigned char, int, int);
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
	GLenum result = glewInit();
	if (result != GLEW_OK) {
		fprintf(stderr, "ERROR: %s\n", glewGetErrorString(result));
		return EXIT_FAILURE;
	}
	createshaders();
	createvbo();
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(resize);
	glutTimerFunc(0, timer, 0);
	fprintf(stdout, "INFO: OpenGL Version: %s\n", glGetString(GL_VERSION));
	glutMainLoop();
	return EXIT_SUCCESS;
}

void createshaders(void)
{
	vshaderid = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vshaderid, 1, &vshader, NULL);
	glCompileShader(vshaderid);
	fshaderid = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fshaderid, 1, &fshader, NULL);
	glCompileShader(fshaderid);
	programid = glCreateProgram();
	glAttachShader(programid, vshaderid);
	glAttachShader(programid, fshaderid);
	glLinkProgram(programid);
	glUseProgram(programid);
}

void createvbo(void)
{
	const float vertices[64] = {
		-.5f, -.5f, -0.5f, 1,  0, 0, 1, 1,
		-.5f, .5f, -0.5f, 1,   1, 0, 0, 1,
		.5f, .5f, -0.5f, 1,    0, 1, 0, 1,
		.5f, -.5f, -0.5f, 1,   1, 1, 0, 1,
		-.5f, -.5f, 0.5f, 1, 1, 1, 1, 1,
		-.5f, .5f, 0.5f, 1,  1, 0, 0, 1,
		.5f, .5f, 0.5f, 1,   1, 0, 1, 1,
		.5f, -.5f, 0.5f, 1,  0, 0, 1, 1  
	};
	const GLuint indices[36] = {
		0,2,1, 0,3,2,
		4,3,0, 4,7,3,
		4,1,5, 4,0,1,
		3,6,2, 3,7,6,
		1,6,5, 1,2,6,
		7,5,6, 7,4,5
	};
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices,
		GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT),
		0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT),
		(GLvoid *) (4 * sizeof(GL_FLOAT)));
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indices, indices,
		GL_STATIC_DRAW);
}

void display(void)
{
	frame++;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	angle += 0.001;
	if (angle > 180)
		angle = 0;
	/*rotY[0] = (float) cos(angle);
	rotY[8] = (float) sin(angle);
	rotY[2] = (float) -sin(angle);
	rotY[10] = (float) cos(angle);*/

	camtranso[12] = -cx;
	camtranso[13] = -cy;
	camtranso[14] = -cz;
	glUniformMatrix4fv(glGetUniformLocation(programid, "camtranso"), 1,
		GL_FALSE, camtranso);

	camtransb[12] = cx;
	camtransb[13] = cy;
	camtransb[14] = cz;
	glUniformMatrix4fv(glGetUniformLocation(programid, "camtransb"), 1,
		GL_FALSE, camtransb);

	glUniformMatrix4fv(glGetUniformLocation(programid, "trans"), 1, GL_FALSE,
		trans);
	glUniformMatrix4fv(glGetUniformLocation(programid, "rotY"), 1, GL_FALSE,
		rotY);
	if (cangley > 180)
		cangley = 0;
	viewY[0] = (float) cos(cangley);
	viewY[8] = (float) sin(cangley);
	viewY[2] = (float) -sin(cangley);
	viewY[10] = (float) cos(cangley);
	glUniformMatrix4fv(glGetUniformLocation(programid, "viewY"), 1, GL_FALSE,
		viewY);

	viewX[5] = (float) cos(canglex);
	viewX[6] = (float) -sin(canglex);
	viewX[9] = (float) sin(canglex);
	viewX[10] = (float) cos(canglex);
	glUniformMatrix4fv(glGetUniformLocation(programid, "viewX"), 1, GL_FALSE,
		viewX);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glutSwapBuffers();
	glutPostRedisplay();
}

void idle(void)
{
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'j':
		cx -= step;
		viewY[12] = cx;
		break;
	case 'k':
		cz -= step;
		viewY[14] = cz;
		break;
	case 'l':
		cz += step;
		viewY[14] = cz;
		break;
	case ';':
		cx += step;
		viewY[12] = cx;
		break;
	case 'a':
		cangley -= step;
		break;
	case 's':
		canglex += step;
		break;
	case 'd':
		canglex -= step;
		break;
	case 'f':
		cangley += step;
		break;
	}
	glutPostRedisplay();
}

void resize(int neww, int newh)
{
	winw = neww;
	winh = newh;
	glViewport(0, 0, winw, winh);

	float fovy = 60;
	float aspr = (float) winw / winh;
	float near = 1.0f, far = 100.0f;

	float yscale = (float) (1 / tan((fovy / 2) * (float) (PI / 180)));

	pers[0] = yscale / aspr;
	pers[5] = yscale;
	pers[10] = -((far + near) / (far - near));
	pers[11] = -1;
	pers[14] = -((2 * near * far) / (far - near));

	glUniformMatrix4fv(glGetUniformLocation(programid, "pers"), 1, GL_FALSE,
		pers);

	glUniformMatrix4fv(glGetUniformLocation(programid, "viewX"), 1, GL_FALSE,
		viewX);
	glUniformMatrix4fv(glGetUniformLocation(programid, "viewY"), 1, GL_FALSE,
		viewY);
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

