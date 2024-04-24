#include "AddModel.h"


AddModel::AddModel() {

	const char* modelFile = "models/scene/firstMap.gltf";
	glm::vec3 modelPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 modelRot = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 modelSca = glm::vec3(1.0, 1.0f, 1.0f);
	assembleModels(modelFile, modelPos, modelRot, modelSca, true);

}


std::vector<Models> AddModel::assembleModels(const std::string& file, const glm::vec3& initialPosition, const glm::vec3& initialRotation, const glm::vec3& initialScale, const bool isStatic) {

		models.push_back(Models{ file, initialPosition, initialRotation, initialScale, NULL, NULL, isStatic });


		// Inicialize o modelo recém-adicionado
		models.back().model = new Model(file, initialPosition, initialRotation, initialScale);
	

	return models;
}

void AddModel::deleteModels() {


	// Obter o número de atores
	const physx::PxU32 numActors = mScene->getNbActors(physx::PxActorTypeFlag::eRIGID_STATIC | physx::PxActorTypeFlag::eRIGID_DYNAMIC);

	// Alocar um buffer para armazenar os atores
	physx::PxActor** actorsBuffer = new physx::PxActor * [numActors];

	// Obter os atores reais
	mScene->getActors(physx::PxActorTypeFlag::eRIGID_STATIC | physx::PxActorTypeFlag::eRIGID_DYNAMIC, actorsBuffer, numActors);

	// Remover os atores da cena
	mScene->removeActors(actorsBuffer, numActors);

	// Liberar a memória alocada para o buffer
	delete[] actorsBuffer;


	models.clear();


//	const char* modelFile = "models/scene/firstMap.gltf";
	//glm::vec3 modelPos = glm::vec3(0.0f, 0.0f, 0.0f);
//	glm::vec3 modelRot = glm::vec3(0.0f, 0.0f, 0.0f);
	//glm::vec3 modelSca = glm::vec3(1.0, 1.0f, 1.0f);
	//assembleModels(modelFile, modelPos, modelRot, modelSca, true);

}

void AddModel::AddModelOnRuntime() {

	const char* modelFile = "models/scene/block.gltf";



	glm::vec3 modelPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 modelRot = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 modelSca = glm::vec3(1.0f, 1.0f, 1.0f);

	assembleModels(modelFile, modelPos, modelRot, modelSca, false);
}




void AddModel::Inputs(GLFWwindow* window) {
	// Handles key inputs
	static bool keyPressed = false; // Variável estática para controlar se a tecla foi pressionada anteriormente

	if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS ) {
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