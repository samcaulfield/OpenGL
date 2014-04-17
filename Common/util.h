#ifndef UTIL_H
#define UTIL_H

/* Initialises glut, glew and the shaders. */
void init (int *argc, char **argv, char *window_title);

/* Attempts to load, compile and link a vertex shader and a fragment shader into
 * a program for OpenGL to use. If these operations are unsuccessful, the
 * program will terminate with an error message. The vertex shader must be
 * called vertex_shader.glsl and the fragment shader must be called
 * fragment_shader.glsl. */
void init_shaders ();

#endif
