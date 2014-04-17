#version 130

in vec4 vertex_position;
in vec4 vertex_colour;

out vec4 colour;

void main ()
{
  colour = vertex_colour;
  gl_Position = vertex_position;
}

