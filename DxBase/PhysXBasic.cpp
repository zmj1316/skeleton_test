//#include "PhysXBasic.h"
//
//MyErrorCallback gDefaultErrorCallback;
//MyAllocator gDefaultAllocatorCallback;
//
////-----------------------------------------------------------------------------
//// PhysX initiate
////-----------------------------------------------------------------------------
//void PhysXWorld::InitWorld()
//{
//	_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback,gDefaultErrorCallback);
//
//	if (!_Foundation)
//		fatalError("PxCreateFoundation failed!");
//
//	//The ProfileZoneManager is needed if you want to 
//	//use the performance profiling facilities in PVD.  
//	//Enabling profiling certainly has an impact on performance and memory and thus should be used
//	//only in non-production builds.
//
//	bool recordMemoryAllocations = true;
//	_profileZoneManager = &PxProfileZoneManager::createProfileZoneManager(_Foundation);
//
//	if (!_profileZoneManager)
//		fatalError("PxProfileZoneManager::createProfileZoneManager failed!");
//
//	_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *_Foundation,
//		PxTolerancesScale(), recordMemoryAllocations, _profileZoneManager);
//	if (!_Physics)
//		fatalError("PxCreatePhysics failed!");
//
//	// setup default material...
//	dMaterial = _Physics->createMaterial(0.5f, 0.5f, 0.5f);
//	if (!dMaterial)
//		fatalError("createMaterial failed!");
//
//	PxSceneDesc sceneDesc(_Physics->getTolerancesScale());
//	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
//	_Dispatcher = PxDefaultCpuDispatcherCreate(2);
//	sceneDesc.cpuDispatcher = _Dispatcher;
//	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
//	gScene = _Physics->createScene(sceneDesc);
//
//	if (!gScene)
//		fatalError("createScene failed!");
//}
////-----------------------------------------------------------------------------
//// create PhysX objects.(gound, and box) 
////-----------------------------------------------------------------------------
//void PhysXWorld::InitObjects()
//{
//	//create ground
//	PxRigidStatic* groundPlane = PxCreatePlane(*_Physics, PxPlane(0, 1, 0, 0), *dMaterial);
//	gScene->addActor(*groundPlane);
//
//	//create cubes
//	PxShape* shape = _Physics->createShape(PxBoxGeometry(0.5, 0.5, 0.5), *dMaterial);
//	for (PxU32 i = 0; i < 10; i++)
//	{
//		for (PxU32 j = 0; j < 10; j++)
//		{
//			for (PxU32 k = 0; k < 10; k++)
//			{
//				PxVec3 tr = PxVec3(PxReal(i), PxReal(j+0.5), PxReal(k));
//			     PxTransform localTm(tr);
//			     PxRigidDynamic* body = _Physics->createRigidDynamic(localTm);
//			     body->attachShape(*shape);
//			     PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
//			     gScene->addActor(*body);
//			}
//
//		}
//	}
//	shape->release();
//}
////-----------------------------------------------------------------------------
//// Add a object
////-----------------------------------------------------------------------------
//void PhysXWorld::createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity)
//{
//	PxRigidDynamic* dynamic = PxCreateDynamic(*_Physics, t, geometry, *dMaterial, 10.0f);
//	dynamic->setAngularDamping(0.5f);
//	dynamic->setLinearVelocity(velocity);
//	gScene->addActor(*dynamic);
//}
//
//PxRigidDynamic* PhysXWorld::CreateRigid(PxShape* shape, const PxTransform& transform,
//	float mass = 0, float restitution = 0, float friction = 0.5f, float linear_damp = 0,
//	float angular_damp = 0, bool kinematic = false, unsigned short group = 1, unsigned short mask = 0xFFFF)
//{
//	PxRigidDynamic* body = _Physics->createRigidDynamic(transform);
//	PxFilterData filterData;
//	body->attachShape(*shape);
//	body->setAngularDamping(angular_damp);
//	body->setLinearDamping(linear_damp);
//	PxSetGroup(*body, group);
//	if (kinematic) mass = 0;
//	PxRigidBodyExt::updateMassAndInertia(*body, mass);
//	gScene->addActor(*body);
//	shape->release();
//	return body;
//}
//
//PxRigidDynamic* PhysXWorld::CreateBox(float width, float height, float depth, const PxTransform& transform, float mass, float restitution, float friction, float linear_damp, float angular_damp, bool kinematic, unsigned short group, unsigned short mask)
//{
//	PxShape* shape = _Physics->createShape(PxBoxGeometry(width/2, height/2, depth/2), *dMaterial);
//	return CreateRigid(shape, transform, mass,restitution,friction,linear_damp,angular_damp,kinematic,group,mask);
//}
//
//PxRigidDynamic* PhysXWorld::CreateSphere(float radius, const PxTransform& transform, float mass, float restitution, float friction, float linear_damp, float angular_damp, bool kinematic, unsigned short group, unsigned short mask)
//{
//	PxShape* shape = _Physics->createShape(PxSphereGeometry(radius), *dMaterial);
//	return CreateRigid(shape, transform, mass, restitution, friction, linear_damp, angular_damp, kinematic, group, mask);
//}
//
//PxRigidDynamic* PhysXWorld::CreateCylinder(float radius, float length, const PxTransform& transform, float mass, float restitution, float friction, float linear_damp, float angular_damp, bool kinematic, unsigned short group, unsigned short mask)
//{
//	PxShape* shape = _Physics->createShape(PxCapsuleGeometry(radius, length), *dMaterial);
//	return CreateRigid(shape, transform, mass, restitution, friction, linear_damp, angular_damp, kinematic, group, mask);
//}
//
//PxRigidDynamic* PhysXWorld::CreateCapsule(float radius, float height, const PxTransform& transform, float mass, float restitution, float friction, float linear_damp, float angular_damp, bool kinematic, unsigned short group, unsigned short mask)
//{
//	PxShape* shape = _Physics->createShape(PxCapsuleGeometry(radius, height), *dMaterial);
//	return CreateRigid(shape, transform, mass, restitution, friction, linear_damp, angular_damp, kinematic, group, mask);
//}
//
//void PhysXWorld::MoveRigidBody(PxRigidDynamic* body, const PxTransform& world)
//{
//	body->setKinematicTarget(world);
//}
//
//void PhysXWorld::AddPointToPointConstraint(PxRigidDynamic* body, float pivot[])
//{
//	
//}
//
//void PhysXWorld::AddPointToPointConstraint(PxRigidDynamic* bodyA, PxRigidDynamic* bodyB, float pivotA[], float pivotB[])
//{
//	
//}
//
//void PhysXWorld::Simulate()
//{
//	PX_UNUSED(true);
//	gScene->simulate(1.0f / 60.0f);
//	gScene->fetchResults(true);
//}
//void PhysXWorld::DestroyWorld()
//{
//	PX_UNUSED(true);
//	gScene->release();
//	_Physics->release();
//	_profileZoneManager->release();
//	_Foundation->release();
//}
