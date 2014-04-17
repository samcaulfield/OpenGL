#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>

#include "../Common/util.h"

void display ()
{
  /* Clear the display with the glClearColor that was set previously. */
  glClear (GL_COLOR_BUFFER_BIT);

  glDrawArrays (GL_TRIANGLES, 0, 3);

  /* Promote the contents of the back buffer to the contents of the front
   * buffer. */
  glutSwapBuffers ();
}

int
main (int argc, char **argv)
{
  /* Initialise glut, glew and the shaders. */
  init (&argc, argv, "Blank Window");

  /* Set glut's display callback to the display routine in this file. */
  glutDisplayFunc (display);

  /* Vertex coordinates in normalised device coordinates. */
  GLfloat vertices[] = {-1.0f, -1.0f, 0.0f, 1.0f,
                         1.0f, -1.0f, 0.0f, 1.0f,
                         0.0f,  1.0f, 0.0f, 1.0f};

  /* Get a handle to the vertex_position input of the vertex shader so we can
   * feed it information. */
  GLint vertex_location = glGetAttribLocation (program_object, "vertex_position");

  /* Vertex data must be stored in a VBO and associated with a VAO. */
  GLuint vbo;
  /* Only one is needed. */
  glGenBuffers (1, &vbo);
  /* Bind the VBO for initialisation. */
  glBindBuffer (GL_ARRAY_BUFFER, vbo);

  glBufferData (GL_ARRAY_BUFFER, 12 * sizeof (GLfloat), vertices,
                GL_STATIC_DRAW);

  /* A VAO stores the data of a geometric object on the GPU. */
  GLuint vao;
  /* Only one is needed. */
  glGenVertexArrays (1, &vao);
  /* Bind the VAO for initialisation. */
  glBindVertexArray (vao);

  glBindBuffer (GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer (vertex_location, 4, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray (vertex_location);

  /* Put glut into its event processing loop. This will configure glut to call
   * any callbacks that have been registered. */
  glutMainLoop ();

  return EXIT_SUCCESS;
}
