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
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
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


physx::PxRigidDynamic* PhysX::createObject(glm::vec3 initialPosition, glm::vec3 initialRotate, glm::vec3 initialScale, physx::PxRigidDynamic* dBody){

	

	mMaterial = PhysX::mPhysics->createMaterial(0.5f, 0.5f, 0.5f);
	physx::PxShape* shape = mPhysics->createShape(physx::PxBoxGeometry(initialScale.x, initialScale.y, initialScale.z), *mMaterial);

	physx::PxTransform localTm(physx::PxVec3(initialPosition.x, initialPosition.y, initialPosition.z)); // Ajustado para uma posição mais alta
	dBody = mPhysics->createRigidDynamic(localTm);
	dBody->attachShape(*shape);
	physx::PxRigidBodyExt::updateMassAndInertia(*dBody, 10.0f);
	mScene->addActor(*dBody);
	shape->release();
	

	return dBody;
}


physx::PxRigidStatic* PhysX::createCustomMesh(std::vector<Mesh>& meshes, physx::PxRigidStatic* sBody) {

	mMaterial = PhysX::mPhysics->createMaterial(0.5f, 0.5f, 0.5f);
	// Criar uma malha de triângulos
	physx::PxTriangleMeshDesc meshDesc;

	if(meshes.data() != NULL){

	// Alocar e preencher o array de vértices convertidos
	physx::PxVec3* verticesArray = new physx::PxVec3[meshes.data()->vertices.size()];
	for (size_t i = 0; i < meshes.data()->vertices.size(); ++i) {
		verticesArray[i] = physx::PxVec3(meshes.data()->vertices[i].position.x, meshes.data()->vertices[i].position.y, meshes.data()->vertices[i].position.z);
	}
	
	meshDesc.points.data = verticesArray;
	meshDesc.points.count = meshes.data()->vertices.size();
	meshDesc.points.stride = sizeof(float) * 3;

	meshDesc.triangles.data = meshes.data()->indices.data();
	meshDesc.triangles.count = meshes.data()->indices.size();
	meshDesc.triangles.stride = sizeof(GLuint) *3 ;

	//meshDesc.flags |= physx::PxMeshFlag::e16_BIT_INDICES;

	
	physx::PxTolerancesScale scale;
	physx::PxCookingParams params(scale);

	params.midphaseDesc = physx::PxMeshMidPhase::eBVH34;

	bool skipMeshCleanup = false;

	setupCommonCookingParams(params, skipMeshCleanup, true);

	float numPrimsPerLeaf = static_cast<float>(meshes.data()->indices.size())  / meshes.data()->vertices.size();

	// Cooking mesh with less triangles per leaf produces larger meshes with better runtime performance
	// and worse cooking performance. Cooking time is better when more triangles per leaf are used.
	params.midphaseDesc.mBVH34Desc.numPrimsPerLeaf = static_cast<uint32_t>(numPrimsPerLeaf);

#if defined(PX_CHECKED) || defined(PX_DEBUG)
	// If DISABLE_CLEAN_MESH is set, the mesh is not cleaned during the cooking. 
	// We should check the validity of provided triangles in debug/checked builds though.
	if (skipMeshCleanup)
	{
		PX_ASSERT(PxValidateTriangleMesh(params, meshDesc));
	}
#endif // DEBUG

	physx::PxTriangleMesh* triMesh = NULL;
	physx::PxU32 meshSize = 0;

	// The cooked mesh may either be saved to a stream for later loading, or inserted directly into PxPhysics.
	

	
		physx::PxDefaultMemoryOutputStream outBuffer;
		PxCookTriangleMesh(params, meshDesc, outBuffer);

		physx::PxDefaultMemoryInputData stream(outBuffer.getData(), outBuffer.getSize());
		triMesh = mPhysics->createTriangleMesh(stream);

		meshSize = outBuffer.getSize();
	

	// Criar uma forma física a partir da malha de triângulos
	physx::PxShape* shape = mPhysics->createShape(physx::PxTriangleMeshGeometry(triMesh), *mMaterial);

	// Criar o corpo rígido estático
	physx::PxTransform localTm(physx::PxVec3(0.0f, 0.0f, 0.0f));
	sBody = mPhysics->createRigidStatic(localTm);
	sBody->attachShape(*shape);

	// Adicionar o corpo à cena física
	mScene->addActor(*sBody);

	// Liberar memória não utilizada
	shape->release();
	triMesh->release();
	delete[] verticesArray;
	}
	return sBody;
}

// Setup common cooking params
void PhysX::setupCommonCookingParams(physx::PxCookingParams& params, bool skipMeshCleanup, bool skipEdgeData)
{
	// we suppress the triangle mesh remap table computation to gain some speed, as we will not need it 
// in this snippet
	params.suppressTriangleMeshRemapTable = true;

	// If DISABLE_CLEAN_MESH is set, the mesh is not cleaned during the cooking. The input mesh must be valid. 
	// The following conditions are true for a valid triangle mesh :
	//  1. There are no duplicate vertices(within specified vertexWeldTolerance.See PxCookingParams::meshWeldTolerance)
	//  2. There are no large triangles(within specified PxTolerancesScale.)
	// It is recommended to run a separate validation check in debug/checked builds, see below.

	if (!skipMeshCleanup)
		params.meshPreprocessParams &= ~static_cast<physx::PxMeshPreprocessingFlags>(physx::PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH);
	else
		params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;

	// If eDISABLE_ACTIVE_EDGES_PRECOMPUTE is set, the cooking does not compute the active (convex) edges, and instead 
	// marks all edges as active. This makes cooking faster but can slow down contact generation. This flag may change 
	// the collision behavior, as all edges of the triangle mesh will now be considered active.
	if (!skipEdgeData)
		params.meshPreprocessParams &= ~static_cast<physx::PxMeshPreprocessingFlags>(physx::PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE);
	else
		params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
}