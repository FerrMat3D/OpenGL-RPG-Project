#include "AddModel.h"


AddModel::AddModel() {

	const char* modelFile = "models/scene/firstMap.gltf";
	glm::vec3 modelPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 modelRot = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 modelSca = glm::vec3(1.0, 1.0f, 1.0f);
	assembleModels(modelFile, modelPos, modelRot, modelSca);

}


std::vector<Models> AddModel::assembleModels(const std::string& file, const glm::vec3& initialPosition, const glm::vec3& initialRotation, const glm::vec3& initialScale) {

		models.push_back(Models{ file, initialPosition, initialRotation, initialScale });
		// Inicialize o modelo recém-adicionado
		models.back().model = new Model(file, initialPosition, initialRotation, initialScale);
	

	return models;
}

void AddModel::deleteModels() {


	models.clear();
	const char* modelFile = "models/scene/firstMap.gltf";
	glm::vec3 modelPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 modelRot = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 modelSca = glm::vec3(1.0, 1.0f, 1.0f);
	assembleModels(modelFile, modelPos, modelRot, modelSca);

}

void AddModel::AddModelOnRuntime() {

	const char* modelFile = "models/scene/block.gltf";

	// Seed para a função rand() baseada no tempo atual
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	// Geração de números aleatórios entre -25.0f e 25.0f para cada dimensão
	float randomX = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * 50.0f - 25.0f;
	float randomY = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * 50.0f - 25.0f;
	float randomZ = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * 50.0f - 25.0f;

	glm::vec3 modelPos = glm::vec3(randomX, randomY, randomZ);
	glm::vec3 modelRot = glm::vec3(randomX, randomY, randomZ);
	glm::vec3 modelSca = glm::vec3(1, 1, 1);

	assembleModels(modelFile, modelPos, modelRot, modelSca);
}




void AddModel::Inputs(GLFWwindow* window) {
	// Handles key inputs
	static bool keyPressed = false; // Variável estática para controlar se a tecla foi pressionada anteriormente

	if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS &&!keyPressed) {
		// Adiciona o modelo em tempo de execução apenas se a tecla 8 for pressionada e não tiver sido pressionada anteriormente
		AddModelOnRuntime();
		keyPressed = true; // Define a variável de controle como verdadeira para indicar que a tecla foi pressionada
	}else if (glfwGetKey(window, GLFW_KEY_8) == GLFW_RELEASE) {
		// Se a tecla 8 for liberada, redefine a variável de controle para falso para que o modelo possa ser adicionado novamente quando a tecla for pressionada novamente
		keyPressed = false;
	}


	if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS) {

		deleteModels();

	}
	else if (glfwGetKey(window, GLFW_KEY_9) == GLFW_RELEASE) {


	}


}