#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define DEG_TO_RAD(x) (x * 0.0174532925)

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

	/* Define an axis-aligned unit cube centered at the origin. */
	GLfloat vertices[] =
	{
		/* Front. */
		-0.5f, 0.5f, -0.5f, 1.0f,
		0.5f, 0.5f, -0.5f, 1.0f,
		-0.5f, -0.5f, -0.5f, 1.0f,
		0.5f, 0.5f, -0.5f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f,
		-0.5f, -0.5f, -0.5f, 1.0f,

		/* Right. */
		0.5f, 0.5f, -0.5f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f,
		0.5f, -0.5f, 0.5f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f,

		/* Bottom. */
		-0.5f, -0.5f, -0.5f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f,
		-0.5f, -0.5f, 0.5f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f,
		0.5f, -0.5f, 0.5f, 1.0f,
		-0.5f, -0.5f, 0.5f, 1.0f,

		/* Top. */
		-0.5f, 0.5f, -0.5f, 1.0f,
		0.5f, 0.5f, -0.5f, 1.0f,
		-0.5f, 0.5f, 0.5f, 1.0f,
		0.5f, 0.5f, -0.5f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f,
		-0.5f, 0.5f, 0.5f, 1.0f,

		/* Left. */
		-0.5f, 0.5f, -0.5f, 1.0f,
		-0.5f, 0.5f, 0.5f, 1.0f,
		-0.5f, -0.5f, -0.5f, 1.0f,
		-0.5f, 0.5f, 0.5f, 1.0f,
		-0.5f, -0.5f, 0.5f, 1.0f,
		-0.5f, -0.5f, -0.5f, 1.0f,

		/* Back. */
		-0.5f, 0.5f, 0.5f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f,
		-0.5f, -0.5f, 0.5f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f,
		0.5f, -0.5f, 0.5f, 1.0f,
		-0.5f, -0.5f, 0.5f, 1.0f,
	};

	GLfloat colours[] =
	{
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,

		0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,

		1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f,

		0.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f, 1.0f,

		1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f
	};

	GLuint cbo, vao, vbo;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &cbo);
	glBindBuffer(GL_ARRAY_BUFFER, cbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof colours, colours, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	GLfloat axes[] =
	{
		0.0f, 0.0f, 0.0f, 1.0f,
		1000.0f, 0.0f, 0.0f, 1.0f,

		0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1000.0f, 0.0f, 1.0f,

		0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1000.0f, 1.0f,
	};

	GLfloat axis_colours[] =
	{
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,

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

	GLfloat crosshair_vertices[] =
	{
		-0.1f, 0.1f, -1.1f, 1.0f,
		0.1f, -0.1f, -1.1f, 1.0f,
		0.1, 0.1, -1.1f, 1.0f,
		-0.1, -0.1, -1.1f, 1.0f
	};

	GLfloat crosshair_colours[] =
	{
		0.0f, 1.0f, 0.0f, 0.5f,
		0.0f, 1.0f, 0.0f, 0.5f,
		0.0f, 1.0f, 0.0f, 0.5f,
		0.0f, 1.0f, 0.0f, 0.5f
	};

	GLuint crosshair_vao, crosshair_vbo, crosshair_cbo;

	glGenVertexArrays(1, &crosshair_vao);
	glBindVertexArray(crosshair_vao);

	glGenBuffers(1, &crosshair_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, crosshair_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof crosshair_vertices, crosshair_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &crosshair_cbo);
	glBindBuffer(GL_ARRAY_BUFFER, crosshair_cbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof crosshair_colours, crosshair_colours, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

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

	Camera.x = 2;
	Camera.y = 2;
	Camera.z = 5;
	Camera.x_rotation = Camera.y_rotation = Camera.z_rotation = 0;
	Camera.movement_speed = 0.1f;
	Camera.rotation_speed = 2.0f;

	float identity_matrix[] =
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	float camera_translation_matrix[] =
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	float camera_x_rotation_matrix[] =
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	
	float camera_y_rotation_matrix[] =
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	float camera_z_rotation_matrix[] =
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	/* Set up the voxel map. */
	#define depth 64
	#define height 8
	#define width 64

	uint8_t voxels[width][height][depth] =
	{
		0
	};

	voxels[30][4][30] = 1;

	uint8_t x, y, z;

	int32_t v_index = 0, c_index = 0;
	float vertex_buffer[96];
	float colour_buffer[96];

	for (x = 1; x < width - 1; x++) {
		for (y = 1; y < height - 1; y++) {
			for (z = 1; z < depth - 1; z++) {
				if (voxels[x][y][z] != voxels[x + 1][y][z]) {
					vertex_buffer[v_index++] = x + 0.5f;
					vertex_buffer[v_index++] = y + 0.5f;
					vertex_buffer[v_index++] = z + 0.5f;
					vertex_buffer[v_index++] = 1.0f;

					vertex_buffer[v_index++] = x + 0.5f;
					vertex_buffer[v_index++] = y + 0.5f;
					vertex_buffer[v_index++] = z - 0.5f;
					vertex_buffer[v_index++] = 1.0f;

					vertex_buffer[v_index++] = x + 0.5f;
					vertex_buffer[v_index++] = y - 0.5f;
					vertex_buffer[v_index++] = z + 0.5f;
					vertex_buffer[v_index++] = 1.0f;

					vertex_buffer[v_index++] = x + 0.5f;
					vertex_buffer[v_index++] = y - 0.5f;
					vertex_buffer[v_index++] = z + 0.5f;
					vertex_buffer[v_index++] = 1.0f;

					vertex_buffer[v_index++] = x + 0.5f;
					vertex_buffer[v_index++] = y - 0.5f;
					vertex_buffer[v_index++] = z - 0.5f;
					vertex_buffer[v_index++] = 1.0f;

					vertex_buffer[v_index++] = x + 0.5f;
					vertex_buffer[v_index++] = y + 0.5f;
					vertex_buffer[v_index++] = z - 0.5f;
					vertex_buffer[v_index++] = 1.0f;

					int32_t i;
					for (i = 0; i < 24; i++)
						colour_buffer[c_index++] = 1.0f;
				}
			}
		}
	}

	GLuint voxel_vao, voxel_vbo, voxel_cbo;

	glGenVertexArrays(1, &voxel_vao);
	glBindVertexArray(voxel_vao);

	glGenBuffers(1, &voxel_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, voxel_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof vertex_buffer, vertex_buffer, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &voxel_cbo);
	glBindBuffer(GL_ARRAY_BUFFER, voxel_cbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof colour_buffer, colour_buffer, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	/* The main loop. */
	while (!glfwWindowShouldClose(window))
	{
		/* Camera translation. */
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			Camera.x += Camera.movement_speed * sin(DEG_TO_RAD(Camera.y_rotation));
			Camera.y -= Camera.movement_speed * sin(DEG_TO_RAD(Camera.x_rotation));
			Camera.z -= Camera.movement_speed * cos(DEG_TO_RAD(Camera.y_rotation));
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			Camera.x -= Camera.movement_speed * cos(DEG_TO_RAD(Camera.y_rotation));
			//Camera.z
			Camera.z -= Camera.movement_speed * sin(DEG_TO_RAD(Camera.y_rotation));
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			Camera.x -= Camera.movement_speed * sin(DEG_TO_RAD(Camera.y_rotation));
			Camera.y += Camera.movement_speed * sin(DEG_TO_RAD(Camera.x_rotation));
			Camera.z += Camera.movement_speed * cos(DEG_TO_RAD(Camera.y_rotation));
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			Camera.x += Camera.movement_speed * cos(DEG_TO_RAD(Camera.y_rotation));
			//Camera.z
			Camera.z += Camera.movement_speed * sin(DEG_TO_RAD(Camera.y_rotation));
		}

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			Camera.y += Camera.movement_speed;
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			Camera.y -= Camera.movement_speed;

		/* Camera rotation. */
		if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
			Camera.y_rotation -= Camera.rotation_speed;
		if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
			Camera.x_rotation += Camera.rotation_speed;
		if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
			Camera.x_rotation -= Camera.rotation_speed;
		if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
			Camera.y_rotation += Camera.rotation_speed;

		camera_translation_matrix[3] = -Camera.x;
		camera_translation_matrix[7] = -Camera.y;
		camera_translation_matrix[11] = -Camera.z;

		camera_x_rotation_matrix[5] = cos(DEG_TO_RAD(Camera.x_rotation));
		camera_x_rotation_matrix[6] = -sin(DEG_TO_RAD(Camera.x_rotation));
		camera_x_rotation_matrix[9] = sin(DEG_TO_RAD(Camera.x_rotation));
		camera_x_rotation_matrix[10] = cos(DEG_TO_RAD(Camera.x_rotation));

		camera_y_rotation_matrix[0] = cos(DEG_TO_RAD(Camera.y_rotation));
		camera_y_rotation_matrix[2] = sin(DEG_TO_RAD(Camera.y_rotation));
		camera_y_rotation_matrix[8] = -sin(DEG_TO_RAD(Camera.y_rotation));
		camera_y_rotation_matrix[10] = cos(DEG_TO_RAD(Camera.y_rotation));

		glUniformMatrix4fv(glGetUniformLocation(program_id, "camera_translation_matrix"), 1, GL_TRUE, camera_translation_matrix);

		glUniformMatrix4fv(glGetUniformLocation(program_id, "camera_x_rotation_matrix"), 1, GL_TRUE, camera_x_rotation_matrix);
		glUniformMatrix4fv(glGetUniformLocation(program_id, "camera_y_rotation_matrix"), 1, GL_TRUE, camera_y_rotation_matrix);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(axes_vao);
		glDrawArrays(GL_LINES, 0, 6);

		glBindVertexArray(voxel_vao);
		glDrawArrays(GL_TRIANGLES, 0, 12);

		glUniformMatrix4fv(glGetUniformLocation(program_id, "camera_translation_matrix"), 1, GL_TRUE, identity_matrix);
		glUniformMatrix4fv(glGetUniformLocation(program_id, "camera_x_rotation_matrix"), 1, GL_TRUE, identity_matrix);
		glUniformMatrix4fv(glGetUniformLocation(program_id, "camera_y_rotation_matrix"), 1, GL_TRUE, identity_matrix);

		glBindVertexArray(crosshair_vao);
		glDrawArrays(GL_LINES, 0, 4);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	printf("Exiting.\n");

	glDeleteShader(fragment_shader_id);
	glDeleteShader(vertex_shader_id);
	glfwTerminate();

	return EXIT_SUCCESS;
}

