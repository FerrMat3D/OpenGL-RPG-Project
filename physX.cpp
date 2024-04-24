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
	mTolerancesScale.speed = 10;
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

physx::PxRigidDynamic* PhysX::createObject(glm::vec3 initialPosition, glm::vec3 initialRotate, glm::vec3 initialScale, physx::PxRigidDynamic* dBody) {



	mMaterial = PhysX::mPhysics->createMaterial(0.2f, 0.2f, 0.2f);
	physx::PxShape* shape = mPhysics->createShape(
		physx::PxBoxGeometry(
			static_cast<physx::PxReal>(initialScale.x/1.99f),
			static_cast<physx::PxReal>(initialScale.y/1.99f),
			static_cast<physx::PxReal>(initialScale.z/1.99f)
		),
		*mMaterial
	);

	physx::PxTransform localTm(physx::PxVec3(initialPosition.x, initialPosition.y +10.0f, initialPosition.z)); // Ajustado para uma posição mais alta
	dBody = mPhysics->createRigidDynamic(localTm);
	dBody->attachShape(*shape);
	physx::PxRigidBodyExt::updateMassAndInertia(*dBody, 10.0f);
	mScene->addActor(*dBody);
	shape->release();


	return dBody;
}


physx::PxRigidStatic* PhysX::createCustomMesh(std::vector<Mesh>& meshes, physx::PxRigidStatic* sBody) {


	if (meshes.data() != NULL) {

	mMaterial = PhysX::mPhysics->createMaterial(0.5f, 0.5f, 0.5f);
	// Criar uma malha de triângulos
	physx::PxTriangleMeshDesc meshDesc;

		
	float scaleFactor = 10.0f;

	// Alocar e preencher o array de vértices convertidos com a escala aplicada
	physx::PxVec3* verticesArray = new physx::PxVec3[meshes.data()->vertices.size()];
	for (size_t i = 0; i < meshes.data()->vertices.size(); ++i) {
		verticesArray[i] = physx::PxVec3(-meshes.data()->vertices[i].position.x * scaleFactor,
			-meshes.data()->vertices[i].position.y * scaleFactor,
			-meshes.data()->vertices[i].position.z * scaleFactor);
	}
	
			meshDesc.points.data = verticesArray;
			meshDesc.points.count = meshes.data()->vertices.size();
			meshDesc.points.stride = sizeof(physx::PxVec3);



			std::vector<physx::PxU32> invertedIndices;
				invertedIndices.reserve(meshes.data()->indices.size());
				for (size_t i = 0; i < meshes.data()->indices.size(); i += 3) {
					invertedIndices.push_back(meshes.data()->indices[i]);     // Índice do terceiro vértice
					invertedIndices.push_back(meshes.data()->indices[i + 1]); // Índice do segundo vértice
					invertedIndices.push_back(meshes.data()->indices[i + 2]); // Índice do primeiro vértice
				}
			

			meshDesc.triangles.data = invertedIndices.data();
			meshDesc.triangles.count = invertedIndices.size();
			meshDesc.triangles.stride = sizeof(physx::PxU32) * 3 ;

			//meshDesc.flags |= physx::PxMeshFlag::e16_BIT_INDICES;

	
			physx::PxTolerancesScale scale;
			physx::PxCookingParams params(scale);
	

			params.midphaseDesc = physx::PxMeshMidPhase::eBVH34;
			params.midphaseDesc.mBVH34Desc.numPrimsPerLeaf = static_cast<uint32_t>(3);


			setupCommonCookingParams(params, false, false);
		
		

	// Cooking mesh with less triangles per leaf produces larger meshes with better runtime performance
	// and worse cooking performance. Cooking time is better when more triangles per leaf are used.
			

		


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

			// Criando uma transformação para girar o corpo em torno do eixo Z
			//physx::PxQuat rotationZ(physx::PxPi, physx::PxVec3(0.0f, 0.0f, 1.0f)); // Girando 180 graus em torno do eixo Z


			//localTm.q = rotationZ;

			sBody = mPhysics->createRigidStatic(localTm);
			sBody->attachShape(*shape);

			// Adicionar o corpo à cena física
			mScene->addActor(*sBody);

			// Liberar memória não utilizada
			shape->release();
			triMesh->release();
			delete[] verticesArray;
		}
		else {

			return NULL;
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