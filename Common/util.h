#ifndef UTIL_H
#define UTIL_H

GLuint program_object;

/* The dimensions of the window. */
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600


/* Initialises glut, glew and the shaders. */
void init (int *argc, char **argv, char *window_title);

#endif
