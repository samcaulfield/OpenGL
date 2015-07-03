#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <assert.h>
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "sc_mat4f.h"
#include "sc_vecf.h"
#include "sc_vec4f.h"

//#define DEBUG

#define RAD(x) (x * 0.0174532925)

static const GLchar *fragment_shader_source =
{
"#version 130\n"\

"in vec4 out_colour;\n"\

"void main(void)\n"\
"{\n"\
"	gl_FragColor = out_colour;\n"\
"}\n"\
};

static const GLchar *vertex_shader_source =
{
"#version 130\n"\

"in vec4 in_colour;\n"\
"in vec4 position;\n"\
"out vec4 out_colour;\n"\

"uniform mat4 camera_translation_matrix;\n"\
"uniform mat4 camera_x_rotation_matrix;\n"\
"uniform mat4 camera_y_rotation_matrix;\n"\
"uniform mat4 perspective_matrix;\n"\

"void main(void)\n"\
"{\n"\
"	mat4 view_matrix = camera_x_rotation_matrix * camera_y_rotation_matrix * camera_translation_matrix;\n"\
"	gl_Position = (perspective_matrix * view_matrix) * position;\n"\
"	out_colour = in_colour;\n"\
"}\n"\
};

int32_t main(int32_t num_args, uint8_t args)
{
	/* Use GLFW to create an OpenGL context. */
	if (!glfwInit())
	{
		fprintf(stderr, "GLFW couldn't be initialised. Exiting.\n");
		return EXIT_FAILURE;
	}

	GLFWwindow *window = glfwCreateWindow(600, 600, "OpenGL Demo", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		fprintf(stderr, "There was an error creating a window. Exiting.\n");
		return EXIT_FAILURE;
	}
	glfwMakeContextCurrent(window);

	/* Once the OpenGL context is created GLEW can be initialised. */
	GLenum result = glewInit();
	if (result != GLEW_OK)
	{
		fprintf(stderr, "Error: %s.\n", glewGetErrorString(result));
		return EXIT_FAILURE;
	}

	printf("Using GLEW %s.\n", glewGetString(GLEW_VERSION));

	/* Set up the shaders. */
	GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader_id, 1, &fragment_shader_source, NULL);
	glCompileShader(fragment_shader_id);

	GLint compilation_status;
	glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &compilation_status);

	if (compilation_status == GL_FALSE)
	{
		fprintf(stderr, "The fragment shader did not compile successfully.\n");

		GLint error_log_max_length;
		glGetShaderiv(fragment_shader_id, GL_INFO_LOG_LENGTH, &error_log_max_length);
		GLchar error_log[error_log_max_length];
		glGetShaderInfoLog(fragment_shader_id, error_log_max_length, &error_log_max_length, error_log);
		printf("%s\n", error_log);

		glDeleteShader(fragment_shader_id);
		glfwTerminate();
		return EXIT_FAILURE;	
	}

	GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader_id, 1, &vertex_shader_source, NULL);
	glCompileShader(vertex_shader_id);

	compilation_status;
	glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &compilation_status);

	if (compilation_status == GL_FALSE)
	{
		fprintf(stderr, "The vertex shader did not compile successfully.\n");

		GLint error_log_max_length;
		glGetShaderiv(vertex_shader_id, GL_INFO_LOG_LENGTH, &error_log_max_length);
		GLchar error_log[error_log_max_length];
		glGetShaderInfoLog(vertex_shader_id, error_log_max_length, &error_log_max_length, error_log);
		printf("%s\n", error_log);

		glDeleteShader(fragment_shader_id);
		glDeleteShader(vertex_shader_id);
		glfwTerminate();
		return EXIT_FAILURE;	
	}

	GLuint program_id = glCreateProgram();
	glBindAttribLocation(program_id, 0, "position");
	glBindAttribLocation(program_id, 1, "in_colour");

	glAttachShader(program_id, fragment_shader_id);
	glAttachShader(program_id, vertex_shader_id);

	glLinkProgram(program_id);

	GLint link_status;
	glGetProgramiv(program_id, GL_LINK_STATUS, &link_status);
	if (link_status == GL_FALSE)
	{
		fprintf(stderr, "The shader program was not linked successfully.\n");

		GLint error_log_max_length;
		glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &error_log_max_length);
		GLchar error_log[error_log_max_length];
		glGetProgramInfoLog(program_id, error_log_max_length, &error_log_max_length, error_log);
		printf("%s\n", error_log);

		glDeleteProgram(program_id);
		glDeleteShader(fragment_shader_id);
		glDeleteShader(vertex_shader_id);
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glUseProgram(program_id);

	/* Define a three-dimensional grid of uniformly-spaced points (voxels). */
	#define NUM_VOXELS_X 100
	#define NUM_VOXELS_Y 100
	#define NUM_VOXELS_Z 100
	#define COMPONENTS_PER_VERTEX 4

	uint8_t voxels[NUM_VOXELS_X][NUM_VOXELS_Y][NUM_VOXELS_Z] =
	{
		0
	};

	sc_vecf *vertex_buffer, *colour_buffer;
	{
		float *initial_vertex_buffer = malloc(64 * sizeof *initial_vertex_buffer);
		vertex_buffer = sc_vecf_new(initial_vertex_buffer, 0, 64, 64);

		float *initial_colour_buffer = malloc(64 * sizeof *initial_colour_buffer);
		colour_buffer = sc_vecf_new(initial_colour_buffer, 0, 64, 64);

		if (!initial_vertex_buffer || !vertex_buffer
			|| !initial_colour_buffer || !colour_buffer) {
			printf("Memory allocation error.\n");
			return EXIT_FAILURE;
		}
	}

	{
		int32_t x, y, z;
		for (x = 0; x < NUM_VOXELS_X; x++)
			for (y = 0; y < NUM_VOXELS_Y; y++)
				for (z = 0; z < NUM_VOXELS_Z; z++)
					voxels[x][y][z] = 1;
	}

	int32_t x, y, z;

	#define add_x_quad(offset) \
		do { \
			sc_vecf_append(vertex_buffer, x + (offset)); \
			sc_vecf_append(vertex_buffer, y - 0.5f); \
			sc_vecf_append(vertex_buffer, z + 0.5f); \
			sc_vecf_append(vertex_buffer, 1.0f); \
			sc_vecf_append(vertex_buffer, x + (offset)); \
			sc_vecf_append(vertex_buffer, y + 0.5f); \
			sc_vecf_append(vertex_buffer, z + 0.5f); \
			sc_vecf_append(vertex_buffer, 1.0f); \
			sc_vecf_append(vertex_buffer, x + (offset)); \
			sc_vecf_append(vertex_buffer, y + 0.5f); \
			sc_vecf_append(vertex_buffer, z - 0.5f); \
			sc_vecf_append(vertex_buffer, 1.0f); \
			sc_vecf_append(vertex_buffer, x + (offset)); \
			sc_vecf_append(vertex_buffer, y + 0.5f); \
			sc_vecf_append(vertex_buffer, z - 0.5f); \
			sc_vecf_append(vertex_buffer, 1.0f); \
			sc_vecf_append(vertex_buffer, x + (offset)); \
			sc_vecf_append(vertex_buffer, y - 0.5f); \
			sc_vecf_append(vertex_buffer, z - 0.5f); \
			sc_vecf_append(vertex_buffer, 1.0f); \
			sc_vecf_append(vertex_buffer, x + (offset)); \
			sc_vecf_append(vertex_buffer, y - 0.5f); \
			sc_vecf_append(vertex_buffer, z + 0.5f); \
			sc_vecf_append(vertex_buffer, 1.0f); \
			int32_t i; \
			for (i = 0; i < 6; i++) { \
				sc_vecf_append(colour_buffer, 0.0f); \
				sc_vecf_append(colour_buffer, 1.0f); \
				sc_vecf_append(colour_buffer, 0.0f); \
				sc_vecf_append(colour_buffer, 1.0f); \
			} \
		} while (0)

	#define add_y_quad(offset) \
		do { \
			sc_vecf_append(vertex_buffer, x - 0.5f); \
			sc_vecf_append(vertex_buffer, y + (offset)); \
			sc_vecf_append(vertex_buffer, z + 0.5f); \
			sc_vecf_append(vertex_buffer, 1.0f); \
			sc_vecf_append(vertex_buffer, x + 0.5f ); \
			sc_vecf_append(vertex_buffer, y + (offset)); \
			sc_vecf_append(vertex_buffer, z + 0.5f); \
			sc_vecf_append(vertex_buffer, 1.0f); \
			sc_vecf_append(vertex_buffer, x + 0.5f); \
			sc_vecf_append(vertex_buffer, y + (offset)); \
			sc_vecf_append(vertex_buffer, z - 0.5f); \
			sc_vecf_append(vertex_buffer, 1.0f); \
			sc_vecf_append(vertex_buffer, x + 0.5f); \
			sc_vecf_append(vertex_buffer, y + (offset)); \
			sc_vecf_append(vertex_buffer, z - 0.5f); \
			sc_vecf_append(vertex_buffer, 1.0f); \
			sc_vecf_append(vertex_buffer, x - 0.5f); \
			sc_vecf_append(vertex_buffer, y + (offset)); \
			sc_vecf_append(vertex_buffer, z - 0.5f); \
			sc_vecf_append(vertex_buffer, 1.0f); \
			sc_vecf_append(vertex_buffer, x - 0.5f); \
			sc_vecf_append(vertex_buffer, y + (offset)); \
			sc_vecf_append(vertex_buffer, z + 0.5f); \
			sc_vecf_append(vertex_buffer, 1.0f); \
			int32_t i; \
			for (i = 0; i < 6; i++) { \
				sc_vecf_append(colour_buffer, 0.0f); \
				sc_vecf_append(colour_buffer, 1.0f); \
				sc_vecf_append(colour_buffer, 0.0f); \
				sc_vecf_append(colour_buffer, 1.0f); \
			} \
		} while (0)

	#define add_z_quad(offset) \
		do { \
			sc_vecf_append(vertex_buffer, x - 0.5f); \
			sc_vecf_append(vertex_buffer, y + 0.5f); \
			sc_vecf_append(vertex_buffer, z + (offset)); \
			sc_vecf_append(vertex_buffer, 1.0f); \
			sc_vecf_append(vertex_buffer, x + 0.5f ); \
			sc_vecf_append(vertex_buffer, y + 0.5f); \
			sc_vecf_append(vertex_buffer, z + (offset)); \
			sc_vecf_append(vertex_buffer, 1.0f); \
			sc_vecf_append(vertex_buffer, x + 0.5f); \
			sc_vecf_append(vertex_buffer, y - 0.5f); \
			sc_vecf_append(vertex_buffer, z + (offset)); \
			sc_vecf_append(vertex_buffer, 1.0f); \
			sc_vecf_append(vertex_buffer, x + 0.5f); \
			sc_vecf_append(vertex_buffer, y - 0.5f); \
			sc_vecf_append(vertex_buffer, z + (offset)); \
			sc_vecf_append(vertex_buffer, 1.0f); \
			sc_vecf_append(vertex_buffer, x - 0.5f); \
			sc_vecf_append(vertex_buffer, y - 0.5f); \
			sc_vecf_append(vertex_buffer, z + (offset)); \
			sc_vecf_append(vertex_buffer, 1.0f); \
			sc_vecf_append(vertex_buffer, x - 0.5f); \
			sc_vecf_append(vertex_buffer, y + 0.5f); \
			sc_vecf_append(vertex_buffer, z + (offset)); \
			sc_vecf_append(vertex_buffer, 1.0f); \
			int32_t i; \
			for (i = 0; i < 6; i++) { \
				sc_vecf_append(colour_buffer, 0.0f); \
				sc_vecf_append(colour_buffer, 1.0f); \
				sc_vecf_append(colour_buffer, 0.0f); \
				sc_vecf_append(colour_buffer, 1.0f); \
			} \
		} while (0)

	for (x = 0; x < NUM_VOXELS_X; x++) {
		for (y = 0; y < NUM_VOXELS_Y; y++) {
			for (z = 0; z < NUM_VOXELS_Z; z++) {
				if (voxels[x][y][z]) {
					if (!x) /* Do left. */
						add_x_quad(-0.5f);

					if (!y) /* Do bottom. */
						add_y_quad(-0.5f);

					if (!z) /* Do back. */
						add_z_quad(-0.5f);

					if (x == NUM_VOXELS_X - 1 || (voxels[x][y][z] != voxels[x + 1][y][z])) /* Do right. */
						add_x_quad(0.5f);

					if (y == NUM_VOXELS_Y - 1 || voxels[x][y][z] != voxels[x][y + 1][z]) /* Do top. */
						add_y_quad(0.5f);

					if (z == NUM_VOXELS_Z - 1 || voxels[x][y][z] != voxels[x][y][z + 1]) /* Do front. */
						add_z_quad(0.5f);
				}
			}
		}
	}

	printf("Vertex buffer index: %" PRIu64 ".\n", vertex_buffer->index);
	printf("Colour buffer index: %" PRIu64 ".\n", colour_buffer->index);
	printf("Vertex buffer size: %" PRIu64 ".\n", vertex_buffer->size);
	printf("Colour buffer size: %" PRIu64 ".\n", colour_buffer->size);

	GLuint voxel_vao, voxel_vbo, voxel_cbo;

	glGenVertexArrays(1, &voxel_vao);
	glBindVertexArray(voxel_vao);

	glGenBuffers(1, &voxel_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, voxel_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_buffer->index, vertex_buffer->data, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &voxel_cbo);
	glBindBuffer(GL_ARRAY_BUFFER, voxel_cbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * colour_buffer->index, colour_buffer->data, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	uint32_t num_voxel_vertices = vertex_buffer->index / 4;

	sc_vecf_free(vertex_buffer);
	sc_vecf_free(colour_buffer);

#ifdef DEBUG
	float voxel_vertices[NUM_VOXELS_X * NUM_VOXELS_Y * NUM_VOXELS_Z * COMPONENTS_PER_VERTEX];
	float voxel_colours[NUM_VOXELS_X * NUM_VOXELS_Y * NUM_VOXELS_Z * COMPONENTS_PER_VERTEX];

	{
		float x, y, z;
		int32_t colours_i = 0, vertices_i = 0;

		for (x = 0; x < NUM_VOXELS_X; x++) {
			for (y = 0; y < NUM_VOXELS_Y; y++) {
				for (z = 0; z < NUM_VOXELS_Z; z++) {
					voxel_vertices[vertices_i++] = x;
					voxel_vertices[vertices_i++] = y;
					voxel_vertices[vertices_i++] = z;
					voxel_vertices[vertices_i++] = 1.0f;

					voxel_colours[colours_i++] = 1.0f;
					voxel_colours[colours_i++] = 1.0f;
					voxel_colours[colours_i++] = 1.0f;
					voxel_colours[colours_i++] = 1.0f;
				}
			}
		}
	}

	GLuint voxels_vao, voxels_vbo, voxels_cbo;

	glGenVertexArrays(1, &voxels_vao);
	glBindVertexArray(voxels_vao);

	glGenBuffers(1, &voxels_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, voxels_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof voxel_vertices, voxel_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &voxels_cbo);
	glBindBuffer(GL_ARRAY_BUFFER, voxels_cbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof voxel_colours, voxel_colours, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	GLfloat axes[] =
	{
		0.0f, 0.0f, 0.0f, 1.0f,
		NUM_VOXELS_X - 1, 0.0f, 0.0f, 1.0f,

		0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, NUM_VOXELS_Y - 1, 0.0f, 1.0f,

		0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, NUM_VOXELS_Z - 1, 1.0f,
	};

	GLfloat axis_colours[] =
	{
		1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,

		0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,

		1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f
	};

	GLuint axes_vao, axes_vbo, axes_cbo;

	glGenVertexArrays(1, &axes_vao);
	glBindVertexArray(axes_vao);

	glGenBuffers(1, &axes_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, axes_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof axes, axes, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &axes_cbo);
	glBindBuffer(GL_ARRAY_BUFFER, axes_cbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof axis_colours, axis_colours, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
#endif

	/* Set up viewing information. */
	float near = 1.0f, far = 1000.0f, left = -1.0f, right = 1.0f, bottom = -1.0f, top = 1.0f;
	float a = (right + left) / (right - left);
	float b = (top + bottom) / (top - bottom);
	float c = -((far + near) / (far - near));
	float d = -((2.0f * far * near) / (far - near));

	float perspective_matrix[] =
	{
		(2.0f * near) / (right - left), 0.0f, a, 0.0f,
		0.0f, (2.0f * near) / (top - bottom), b, 0.0f,
		0.0f, 0.0f, c, d,
		0.0f, 0.0f, -1.0f, 0.0f
	};

	glUniformMatrix4fv(glGetUniformLocation(program_id, "perspective_matrix"), 1, GL_TRUE, perspective_matrix);

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	struct
	{
		float x, y, z;
		float x_rotation, y_rotation, z_rotation;
		float movement_speed, rotation_speed;
	} Camera;

	Camera.x = NUM_VOXELS_X / 2.0f;
	Camera.y = NUM_VOXELS_Y / 2.0f;
	Camera.z = NUM_VOXELS_Z / 2.0f;
	Camera.x_rotation = Camera.y_rotation = Camera.z_rotation = 0;
	Camera.movement_speed = 10.0f;
	Camera.rotation_speed = 70.0f;

	float camera_translation_matrix[] =
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	double current_frame_time = 0, delta, previous_frame_time;

	uint64_t frame = 0;

	/* The main loop. */
	while (!glfwWindowShouldClose(window))
	{
		frame++;

		previous_frame_time = current_frame_time;
		current_frame_time = glfwGetTime();
		delta = current_frame_time - previous_frame_time;

		if (frame >= 100 && glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
			printf("*---* Camera Details: *---*\n");
			printf("*-* Camera.x: %f\n", Camera.x);
			printf("*-* Camera.y: %f\n", Camera.y);
			printf("*-* Camera.z: %f\n", Camera.z);
			printf("*-* Camera.x_rotation: %f\n", Camera.x_rotation);
			printf("*-* Camera.y_rotation: %f\n", Camera.y_rotation);
			printf("*-* Camera.z_rotation: %f\n", Camera.z_rotation);
			printf("*-------------------------*\n\n");
			frame = 0;
		}

		/* Camera rotation. */
		if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
			Camera.y_rotation += delta * Camera.rotation_speed;
		if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
			Camera.x_rotation -= delta * Camera.rotation_speed;
		if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
			Camera.x_rotation += delta * Camera.rotation_speed;
		if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
			Camera.y_rotation -= delta * Camera.rotation_speed;

		if (Camera.x_rotation >= 360.0f)
			Camera.x_rotation -= 360.0f;
		if (Camera.x_rotation <= -360.0f)
			Camera.x_rotation += 360.0f;

		if (Camera.y_rotation >= 360.0f)
			Camera.y_rotation -= 360.0f;
		if (Camera.y_rotation <= -360.0f)
			Camera.y_rotation += 360.0f;

		/* Compute move vector from camera orientation */
		float x_rotation_data[] =
		{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, cos(RAD(Camera.x_rotation)), -sin(RAD(Camera.x_rotation)), 0.0f,
			0.0f, sin(RAD(Camera.x_rotation)), cos(RAD(Camera.x_rotation)), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};

		float y_rotation_data[] =
		{
			cos(RAD(Camera.y_rotation)), 0.0f, sin(RAD(Camera.y_rotation)), 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			-sin(RAD(Camera.y_rotation)), 0.0f, cos(RAD(Camera.y_rotation)), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};

		float z_rotation_data[] =
		{
			cos(RAD(Camera.z_rotation)), -sin(RAD(Camera.z_rotation)), 0.0f, 0.0f,
			sin(RAD(Camera.z_rotation)), cos(RAD(Camera.z_rotation)), 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};

		/* A move vector for the forward direction. */
		sc_vec4f forward_move_vector =
		{
			.x = 0.0f, .y = 0.0f, .z = -Camera.movement_speed, .w = 1.0f
		};

		sc_vec4f left_move_vector =
		{
			.x = -Camera.movement_speed, .y = 0.0f, .z = 0.0f, .w = 1.0f
		};

		sc_vec4f up_move_vector =
		{
			.x = 0.0f, .y = Camera.movement_speed, .z = 0.0f, .w = 1.0f
		};

		sc_vec4f x_result_vector, y_result_vector, z_result_vector, result_vector;

		sc_mat4f *x_rotation_mat = sc_mat4f_new(x_rotation_data);
		assert(x_rotation_mat);

		sc_mat4f *y_rotation_mat = sc_mat4f_new(y_rotation_data);
		assert(y_rotation_mat);

		sc_mat4f *z_rotation_mat = sc_mat4f_new(z_rotation_data);
		assert(z_rotation_mat);

		/* Camera translation. */
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			sc_mat4f_mulv(x_rotation_mat, &forward_move_vector, &x_result_vector);
			sc_mat4f_mulv(y_rotation_mat, &x_result_vector, &result_vector);

			Camera.x += delta * result_vector.x;
			Camera.y += delta * result_vector.y;
			Camera.z += delta * result_vector.z;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			sc_mat4f_mulv(x_rotation_mat, &left_move_vector, &x_result_vector);
			sc_mat4f_mulv(y_rotation_mat, &x_result_vector, &result_vector);

			Camera.x += delta * result_vector.x;
			Camera.y += delta * result_vector.y;
			Camera.z += delta * result_vector.z;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			sc_mat4f_mulv(x_rotation_mat, &forward_move_vector, &x_result_vector);
			sc_mat4f_mulv(y_rotation_mat, &x_result_vector, &result_vector);

			Camera.x -= delta * result_vector.x;
			Camera.y -= delta * result_vector.y;
			Camera.z -= delta * result_vector.z;

		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			sc_mat4f_mulv(x_rotation_mat, &left_move_vector, &x_result_vector);
			sc_mat4f_mulv(y_rotation_mat, &x_result_vector, &result_vector);

			Camera.x -= delta * result_vector.x;
			Camera.y -= delta * result_vector.y;
			Camera.z -= delta * result_vector.z;
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			sc_mat4f_mulv(x_rotation_mat, &up_move_vector, &x_result_vector);
			sc_mat4f_mulv(y_rotation_mat, &x_result_vector, &result_vector);

			Camera.x += delta * result_vector.x;
			Camera.y += delta * result_vector.y;
			Camera.z += delta * result_vector.z;
		}
//		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
//			Camera.y -= delta * Camera.movement_speed;


		free(x_rotation_mat);
		free(y_rotation_mat);
		free(z_rotation_mat);

		camera_translation_matrix[3] = -Camera.x;
		camera_translation_matrix[7] = -Camera.y;
		camera_translation_matrix[11] = -Camera.z;

		float x_rotation_inverse[] =
		{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, cos(RAD(-Camera.x_rotation)), -sin(RAD(-Camera.x_rotation)), 0.0f,
			0.0f, sin(RAD(-Camera.x_rotation)), cos(RAD(-Camera.x_rotation)), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};

		float y_rotation_inverse[] =
		{
			cos(RAD(-Camera.y_rotation)), 0.0f, sin(RAD(-Camera.y_rotation)), 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			-sin(RAD(-Camera.y_rotation)), 0.0f, cos(RAD(-Camera.y_rotation)), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};

		glUniformMatrix4fv(
			glGetUniformLocation(program_id, "camera_translation_matrix"),
			1, GL_TRUE, camera_translation_matrix);

		glUniformMatrix4fv(
			glGetUniformLocation(program_id, "camera_x_rotation_matrix"),
			1, GL_TRUE, x_rotation_inverse);

		glUniformMatrix4fv(
			glGetUniformLocation(program_id, "camera_y_rotation_matrix"),
			1, GL_TRUE, y_rotation_inverse);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#ifdef DEBUG
		glBindVertexArray(axes_vao);
		glDrawArrays(GL_LINES, 0, 6);

		glBindVertexArray(voxels_vao);
		glDrawArrays(GL_POINTS, 0, NUM_VOXELS_X * NUM_VOXELS_Y * NUM_VOXELS_Z);
#endif

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBindVertexArray(voxel_vao);
		glDrawArrays(GL_TRIANGLES, 0, num_voxel_vertices);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	printf("Exiting.\n");

	glDeleteShader(fragment_shader_id);
	glDeleteShader(vertex_shader_id);
	glfwTerminate();

	return EXIT_SUCCESS;
}

