#include "AddModel.h"


AddModel::AddModel() {



}


std::vector<Models> AddModel::assembleModels(const std::string& file, const glm::vec3& initialPosition, const glm::vec3& initialRotation, const glm::vec3& initialScale, const bool isStatic) {

		



		 // Inicialize o modelo recém-adicionado

		 bool jaexiste = false;
		 int endereco;

		 for (int i = 0; i < models.size(); i++) {

			 if (models[i].file == file) {


				 jaexiste = true;
				 endereco = i;

			


			 }

		 }


		 if(!jaexiste){
		models.emplace_back(Models{ file, initialPosition, initialRotation, initialScale, NULL, NULL, isStatic, NULL });
		 auto lastElement = std::prev(models.end());
		  lastElement->model = new Model(file, initialPosition, initialRotation, initialScale);

		 }
		 else {

			 models.emplace_back(models[endereco]);
			 auto lastElement = std::prev(models.end());
			 lastElement->dBody = NULL;
			 lastElement->sBody = NULL;
			 lastElement->physX = false;
			 lastElement->isStatic = isStatic;
			 lastElement->initialScale = initialScale;
			 lastElement->initialRotation = initialRotation;
			 lastElement->initialPosition = initialPosition;

		 }
	



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
	models.shrink_to_fit();


//	const char* modelFile = "models/scene/firstMap.gltf";
	//glm::vec3 modelPos = glm::vec3(0.0f, 0.0f, 0.0f);
//	glm::vec3 modelRot = glm::vec3(0.0f, 0.0f, 0.0f);
	//glm::vec3 modelSca = glm::vec3(1.0, 1.0f, 1.0f);
	//assembleModels(modelFile, modelPos, modelRot, modelSca, true);

}

void AddModel::AddModelOnRuntime(int what) {


	if (what == 1) {
	const char* modelFile = "models/scene/block.gltf";
	glm::vec3 modelPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 modelRot = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 modelSca = glm::vec3(1.0f, 1.0f, 1.0f);
	assembleModels(modelFile, modelPos, modelRot, modelSca, false);
}

	if (what == 2) {

		const char* modelFile = "models/scene/firstMap.gltf";
		glm::vec3 modelPos = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 modelRot = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 modelSca = glm::vec3(1.0, 1.0f, 1.0f);
		assembleModels(modelFile, modelPos, modelRot, modelSca, true);

	}

}






void AddModel::Inputs(GLFWwindow* window) {
	// Handles key inputs
	static bool key8Pressed = false; // Variável estática para controlar se a tecla foi pressionada anteriormente
	static bool key7Pressed = false; // Variável estática para controlar se a tecla foi pressionada anteriormente

	if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS ) {
		// Adiciona o modelo em tempo de execução apenas se a tecla 8 for pressionada e não tiver sido pressionada anteriormente
		AddModelOnRuntime(1);
		key8Pressed = true; // Define a variável de controle como verdadeira para indicar que a tecla foi pressionada
	}else if (glfwGetKey(window, GLFW_KEY_8) == GLFW_RELEASE) {
		// Se a tecla 8 for liberada, redefine a variável de controle para falso para que o modelo possa ser adicionado novamente quando a tecla for pressionada novamente
		key8Pressed = false;
	}


	if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS) {

		deleteModels();

	}
	else if (glfwGetKey(window, GLFW_KEY_9) == GLFW_RELEASE) {


	}

	if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS && !key7Pressed) {
		key7Pressed = true; // Define a variável de controle como verdadeira para indicar que a tecla foi pressionada

		AddModelOnRuntime(2);
	
	}
	else if (glfwGetKey(window, GLFW_KEY_7) == GLFW_RELEASE) {
		key7Pressed = false;

	}


}