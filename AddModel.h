#ifndef ADDMODEL_CLASS_H
#define ADDMODEL_CLASS_H

#include"Model.h"
#include <cstdlib> // Para a função rand()
#include <ctime>   // Para a função time()
#include<physx/PxPhysics.h>
#include<physx/PxPhysicsAPI.h>


struct Models {
     std::string file;
     glm::vec3 initialPosition;
     glm::vec3 initialRotation;
     glm::vec3 initialScale;
	 physx::PxRigidDynamic* dBody;
	 physx::PxRigidStatic* sBody;
	 bool isStatic = false;
	 bool physX = false;
    Model* model;
};


class AddModel
{

	public:

		AddModel();

		physx::PxScene* mScene = NULL;

		std::vector<Models> assembleModels(const std::string& file, const glm::vec3& initialPosition, const glm::vec3& initialRotation, const glm::vec3& initialScale, const bool isStatic);

		

		std::vector<Models> models;

		void AddModelOnRuntime();

		void deleteModels();


		void Inputs(GLFWwindow* window);
	



	private:
	
		
};



#endif // !ADDMODEL_CLASS_H
