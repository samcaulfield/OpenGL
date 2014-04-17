#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>

#include "../Common/util.h"

void display ()
{
  /* Clear the display with the glClearColor that was set previously. */
  glClear (GL_COLOR_BUFFER_BIT);

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

  /* Put glut into its event processing loop. This will configure glut to call
   * any callbacks that have been registered. */
  glutMainLoop ();

  return EXIT_SUCCESS;
}
