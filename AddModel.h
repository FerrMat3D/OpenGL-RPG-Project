#ifndef ADDMODEL_CLASS_H
#define ADDMODEL_CLASS_H

#include"Model.h"
#include <cstdlib> // Para a fun��o rand()
#include <ctime>   // Para a fun��o time()


struct Models {
     std::string file;
     glm::vec3 initialPosition;
     glm::vec3 initialRotation;
     glm::vec3 initialScale;
    Model* model;
};


class AddModel
{

	public:

		AddModel();

		std::vector<Models> assembleModels(const std::string& file, const glm::vec3& initialPosition, const glm::vec3& initialRotation, const glm::vec3& initialScale);

		

		std::vector<Models> models;

		void AddModelOnRuntime();

		void deleteModels();


		void Inputs(GLFWwindow* window);
	



	private:
	
		
};



#endif // !ADDMODEL_CLASS_H
