#include"physX.h"



PhysX::PhysX(){}

void PhysX::physxUpdate() {

	PhysX::mScene->simulate(1.0f / 60.0f);
	PhysX::mScene->fetchResults(true);


}

void PhysX::init(){

	mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mDefaultAllocatorCallback, mDefaultErrorCallback);
	if (!mFoundation) throw("PxCreateFoundation FALHOU !!!");
	mPvd = PxCreatePvd(*mFoundation);
	physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	mPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
	mTolerancesScale.length = 100;
	mTolerancesScale.speed = 981;
	mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, mTolerancesScale, true, mPvd);
	physx::PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -1.81f, 0.0f);
	mDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = mDispatcher;
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	mScene = mPhysics->createScene(sceneDesc);
	physx::PxPvdSceneClient* pvdClient = mScene->getScenePvdClient();

	if (pvdClient) {
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}



}


physx::PxRigidDynamic* PhysX::createObject(glm::vec3 initialPosition, glm::vec3 initialRotate, glm::vec3 initialScale, physx::PxRigidDynamic* body){



	mMaterial = PhysX::mPhysics->createMaterial(0.5f, 0.5f, 0.5f);
	physx::PxShape* shape = mPhysics->createShape(physx::PxBoxGeometry(initialScale.x, initialScale.y, initialScale.z), *mMaterial);

	physx::PxTransform localTm(physx::PxVec3(initialPosition.x, initialPosition.y, initialPosition.z)); // Ajustado para uma posição mais alta
	body = mPhysics->createRigidDynamic(localTm);
	body->attachShape(*shape);
	physx::PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
	mScene->addActor(*body);
	shape->release();
	

	return body;
}


