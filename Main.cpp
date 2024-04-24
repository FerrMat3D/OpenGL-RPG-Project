
//------------------------------

#include"AddModel.h"
#include "physX.h"



unsigned int width = 800;
unsigned int height = 600;
float aspectRatio = (float)width / (float)height;
bool fullscreen = false;

void framebuffer_size_callback(GLFWwindow* window, int w, int h) {
	// Ajustar a viewport mantendo o aspect ratio
	float newAspectRatio = (float)w / (float)h;
	if (newAspectRatio > aspectRatio) {
		// Ajustar a altura para manter o aspect ratio
		int newHeight = (int)(w / aspectRatio);
		glViewport(0, (h - newHeight) / 2, w, newHeight);
	
	}
	else {
		// Ajustar a largura para manter o aspect ratio
		int newWidth = (int)(h * aspectRatio);
		glViewport((w - newWidth) / 2, 0, newWidth, h);

	}
	width = w;
	height = h;

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS && fullscreen) {
		glfwSetWindowMonitor(window, nullptr, 100, 100, width, height, GLFW_DONT_CARE);
		// Calcular o centro da janela
		double centerX = width / 2.0;
		double centerY = height / 2.0;

		// Definir a posição do cursor para o centro da janela
		glfwSetCursorPos(window, centerX, centerY);
		fullscreen = false;
	}
}

void window_maximized_callback(GLFWwindow* window, int maximized) {
	if (maximized && !fullscreen) {
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
		fullscreen = true;
	}
}

void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	std::cerr << "OpenGL debug message: " << message << std::endl;
}


int main()
{


	

	PhysX physicsInstance;

	physicsInstance.init();


	


	//Create Simulation

	//Run Simulation




	// Initialize GLFW
	glfwInit();

	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	// Create a GLFWwindow object of 800 by 800 pixels, naming it "YoutubeOpenGL"
	GLFWwindow* window = glfwCreateWindow(width, height, "Teste PhysX - Matheus Ferreira", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glfwSetKeyCallback(window, key_callback);


	// Registrar o callback para maximizar a janela
	glfwSetWindowMaximizeCallback(window, window_maximized_callback);




	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	//Load GLAD so it configures OpenGL
	gladLoadGL();
	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	//glViewport(0, 0, width, height);


	glEnable(GL_DEPTH_TEST);


	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

	// Generates Shader object using shaders default.vert and default.frag
	Shader shaderProgram("default.vert", "default.frag");

	// Take care of all the light related things
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);


	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);





	AddModel construtorDeModelos;





	// Main while loop
	while (!glfwWindowShouldClose(window))
	{


		physicsInstance.physxUpdate();



		// Specify the color of the background
		glClearColor(0.0f, 0.3f, 0.8f, 1.0f);
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		camera.Inputs(window);
		construtorDeModelos.Inputs(window);
		construtorDeModelos.mScene = physicsInstance.mScene;
		camera.updateMatrix(60.0f,0.1f,50300.0f);

		int width, height;
		glfwGetWindowSize(window, &width, &height);


		camera.updateOnResizeWindow(width, height);



		for (int i = 0; i < construtorDeModelos.models.size(); i++) {
			Model& currentModel = *construtorDeModelos.models[i].model;
			std::string currentModelFile = construtorDeModelos.models[i].file;

			
		
			if (!construtorDeModelos.models[i].physX && construtorDeModelos.models[i].dBody == NULL && !construtorDeModelos.models[i].isStatic) {
				construtorDeModelos.models[i].dBody = physicsInstance.createObject(construtorDeModelos.models[i].initialPosition, construtorDeModelos.models[i].initialRotation, construtorDeModelos.models[i].initialScale, construtorDeModelos.models[i].dBody);
				construtorDeModelos.models[i].physX = true;
			}

			 if (construtorDeModelos.models[i].dBody != NULL && !construtorDeModelos.models[i].isStatic) {
				physx::PxTransform globalTm = construtorDeModelos.models[i].dBody->getGlobalPose();
				physx::PxVec3 position = globalTm.p;
				physx::PxQuat rotation = globalTm.q;
				physx::PxVec3 axis;
				rotation.normalize();
				float posX = position.x;
				float posY = position.y;
				float posZ = position.z;
				currentModel.position = glm::vec3(-posX  , -posY , -posZ);
				glm::vec3 euler = glm::eulerAngles(glm::quat(rotation.w, rotation.x, rotation.y, rotation.z));
				currentModel.rotation.x = glm::degrees(euler.x);
				currentModel.rotation.y = glm::degrees(euler.y);
				currentModel.rotation.z = glm::degrees(euler.z);
			}

			 if (!construtorDeModelos.models[i].physX && construtorDeModelos.models[i].sBody == NULL && construtorDeModelos.models[i].isStatic) {
				
				
				
					
					construtorDeModelos.models[i].sBody = physicsInstance.createCustomMesh(currentModel.meshes,construtorDeModelos.models[i].sBody);
				
					if(construtorDeModelos.models[i].sBody != NULL){
					 construtorDeModelos.models[i].physX = true;
					}
				

					

			 }

	

			// Desenhe o modelo atual
			currentModel.Draw(shaderProgram, camera);
		}


		// Tell OpenGL which Shader Program we want to use
		shaderProgram.Activate();
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);

		camera.Matrix(shaderProgram, "camMatrix");

		// Assigns a value to the uniform; NOTE: Must always be done after activating the Shader Program

	//	construtorDeModelos.currentCameraLocation(camera.Position);
		

		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}



	shaderProgram.Delete();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}