#ifndef PHYSX_CLASS_H
#define PHYSX_CLASS_H


#include<glm/glm.hpp>
#include<physx/PxPhysics.h>
#include<physx/PxPhysicsAPI.h>

class PhysX {

	public:

		physx::PxDefaultAllocator				mDefaultAllocatorCallback;
		physx::PxDefaultErrorCallback			mDefaultErrorCallback;
		physx::PxDefaultCpuDispatcher* mDispatcher = NULL;
		physx::PxTolerancesScale				mTolerancesScale;
		physx::PxFoundation* mFoundation = NULL;
		physx::PxPhysics* mPhysics = NULL;
		physx::PxScene* mScene = NULL;
		physx::PxMaterial* mMaterial = NULL;
		physx::PxPvd* mPvd = NULL;



		PhysX();


		void init();
		physx::PxRigidDynamic* createObject(glm::vec3 initialPosition, glm::vec3 initialRotate, glm::vec3 initialScale, physx::PxRigidDynamic* body);
		void physxUpdate();
};




#endif PHYSX_CLASS_H