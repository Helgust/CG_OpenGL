//internal includes
#include "common.h"
#include "ShaderProgram.h"

//External dependencies
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "dependencies/include/stb/stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "dependencies/include/stb/stb_image.h"

#include "dependencies/include/TEE/camera.h"

#include <random> 

static const GLsizei WIDTH = 1920, HEIGHT = 1080; //размеры окна

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = false;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

int initGL()
{
	int res = 0;
	//грузим функции opengl через glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize OpenGL context" << std::endl;
		return -1;
	}

	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	std::cout << "© Egor Tyutyunikov" << std::endl;

	return 0;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	std::cout << key << std::endl;
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key >= 0 && key < 1024)
    {
        if(action == GLFW_PRESS)
            keys[key] = true;
        else if(action == GLFW_RELEASE)
            keys[key] = false;	
    }
}

void Do_Movement()
{
    // Camera controls
    if(keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if(keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if(keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if(keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);
	if(keys[GLFW_KEY_LEFT_SHIFT])
        camera.ProcessKeyboard(UP, deltaTime);
	if(keys[GLFW_KEY_LEFT_CONTROL])
        camera.ProcessKeyboard(DOWN, deltaTime);
		
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

	
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left
    
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}	

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}







int main(int argc, char **argv)
{
	if (!glfwInit())
		return -1;

	//запрашиваем контекст opengl версии 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL CG Task 2", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	if (initGL() != 0)
		return -1;

	//Reset any OpenGL errors which could be present for some reason
	GLenum gl_error = glGetError();
	while (gl_error != GL_NO_ERROR)
		gl_error = glGetError();

	//создание шейдерной программы из двух файлов с исходниками шейдеров
	//используется класс-обертка ShaderProgram
	std::unordered_map<GLenum, std::string> shaders1;
	shaders1[GL_VERTEX_SHADER] = "vertex.glsl";
	shaders1[GL_FRAGMENT_SHADER] = "fragment.glsl";

	ShaderProgram program1(shaders1);

	GL_CHECK_ERRORS;

	glfwSwapInterval(1); // force 60 frames per second
	glEnable(GL_DEPTH_TEST);  

	//Создаем и загружаем геометрию поверхности
	//


	GLfloat vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};




	GLuint g_vertexBufferObject;
	GLuint g_vertexArrayObject;
	//GLuint g_elementBufferObject;

	glGenVertexArrays(1,&g_vertexArrayObject);
	glGenBuffers(1,&g_vertexBufferObject);
	//glGenBuffers(1,&g_elementBufferObject);

	// FIRST TRIANGLE
	glBindVertexArray(g_vertexArrayObject);

	glBindBuffer(GL_ARRAY_BUFFER,g_vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);

	//glBindBuffer(GL_ARRAY_BUFFER,g_elementBufferObject);
	//glBufferData(GL_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	
	glBindVertexArray(0);

	
	//Load texture
	unsigned int texture;
	glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); 
     // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps

	int width, height,n;
	unsigned char *image = stbi_load("../bath.jpg", &width, &height, &n, 0);
	if (image)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    	glGenerateMipmap(GL_TEXTURE_2D);
	} 
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(image);




	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//цикл обработки сообщений и отрисовки сцены каждый кадр https://stackoverflow.com/questions/17768008/how-to-build-install-glfw-3-and-use-it-in-a-linux-project
	while (!glfwWindowShouldClose(window))
	{
		//Set frame time
		GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

		//Check and call
		glfwPollEvents();
		Do_Movement();
		
		//очищаем экран каждый кадр
		//glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		GL_CHECK_ERRORS;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		GL_CHECK_ERRORS;

		// Bind Texture
		glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
		//program1.SetUniform("texture",1);

		program1.StartUseShader();
		GL_CHECK_ERRORS;

		// очистка и заполнение экрана цветом

		glViewport(0, 0, WIDTH, HEIGHT);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); 


		// Create camera transformation
        glm::mat4 view;
        view = camera.GetViewMatrix();
        glm::mat4 projection;	
        projection = glm::perspective(camera.Zoom, (float)WIDTH/(float)HEIGHT, 0.1f, 1000.0f);

		// create transformations
        glm::mat4 model         = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

		//model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));

        // retrieve the matrix uniform locations
		program1.SetUniform("model",model);
		program1.SetUniform("view",view);
		program1.SetUniform("projection",projection);
        // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
		
		// draw call
		
		glBindVertexArray(g_vertexArrayObject); 
		GL_CHECK_ERRORS;       
		glDrawArrays(GL_TRIANGLES,0,36);
		//std::cout << "CHECK" << std::endl;
		GL_CHECK_ERRORS;
		glBindVertexArray(0);
		GL_CHECK_ERRORS;
		
		program1.StopUseShader();
		GL_CHECK_ERRORS;

		glfwSwapBuffers(window);
	}

	//очищаем vbo и vao перед закрытием программы
	//
	glDeleteVertexArrays(1, &g_vertexArrayObject);
	glDeleteBuffers(1, &g_vertexBufferObject);
	//glDeleteBuffers(1, &g_elementBufferObject);

	glfwTerminate();
	return 0;
}
