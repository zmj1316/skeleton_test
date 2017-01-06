//#ifndef PHYSXBASIC_H_
//#define PHYSXBASIC_H_
////------------------------------------------------------------------------------------
//// simple physx foundation class
////------------------------------------------------------------------------------------
//#include <vector>
//#include <iostream>
//#include <string>
//#include <PxPhysicsAPI.h>
//
//using namespace physx;
//
////output error message
//static void fatalError(std::string message)
//{
//	std::cout << message << std::endl;
//}
//
//#include <malloc.h>//WIN crt_aligned_malloc
//
//class MyAllocator : public PxAllocatorCallback
//{
//public:
//	// brief Allocates size bytes of memory, which must be 16-byte aligned.
//	virtual void* allocate(size_t size, const char* typeName, const char* filename, int line)
//	{
//		void* ptr = _aligned_malloc(size, 16);
//		if (NULL != ptr)
//			return ptr;
//	}
//	// param ptr Memory to free.
//	virtual void deallocate(void* ptr)
//	{
//		_aligned_free(ptr);
//	}
//
//};
//
//class MyErrorCallback : public PxErrorCallback
//{
//public:
//	virtual void reportError(PxErrorCode::Enum code, const char* message, const char* file,
//		int line)
//	{
//		// error processing implementation
//		std::cout << "some error happened: " << __FUNCTION__ << "in" << __FILE__ << std::endl;
//	}
//
//};
//
//class PhysXWorld
//{
//public:
//	PhysXWorld() {}
//	~PhysXWorld() { DestroyWorld(); }
//
//
//	//init dest physx component
//	void InitWorld();
//	void DestroyWorld();
//
//	//init object
//	void InitObjects();
//	void Simulate();
//	//shoot a ball
//	void createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity = PxVec3(0));
//	PxRigidDynamic* CreateRigid(PxShape* shape, const PxTransform& transform,
//		float mass, float restitution, float friction, float linear_damp,
//		float angular_damp, bool kinematic, unsigned short group, unsigned short mask );
//	PxRigidDynamic* CreateBox(float width,float height,float depth, const PxTransform &transform,
//		float mass, float restitution, float friction, float linear_damp,
//		float angular_damp , bool kinematic , unsigned short group , unsigned short mask );
//
//	PxRigidDynamic* CreateSphere(float radius, const PxTransform &transform,
//		float mass, float restitution, float friction, float linear_damp,
//		float angular_damp, bool kinematic, unsigned short group, unsigned short mask);
//
//	PxRigidDynamic* CreateCylinder(float radius, float length, const PxTransform &transform,
//		float mass, float restitution, float friction, float linear_damp,
//		float angular_damp, bool kinematic, unsigned short group, unsigned short mask);
//
//	PxRigidDynamic* CreateCapsule(float radius, float height, const PxTransform &transform,
//		float mass, float restitution, float friction, float linear_damp,
//		float angular_damp, bool kinematic, unsigned short group, unsigned short mask);
//
//	void MoveRigidBody(PxRigidDynamic* body, const PxTransform& world);
//	void AddPointToPointConstraint(PxRigidDynamic* body, float pivot[]);
//	void AddPointToPointConstraint(PxRigidDynamic* bodyA, PxRigidDynamic* bodyB, float pivotA[], float pivotB[]);
//
//
//	//components
//	PxFoundation						 *_Foundation;
//	PxProfileZoneManager				 *_profileZoneManager;
//	PxPhysics						     *_Physics;
//	PxDefaultCpuDispatcher				 *_Dispatcher;
//	PxScene							     *gScene;
//	PxMaterial						     *dMaterial;
//
//	std::vector<PxRigidDynamic*> rigidBodies;
//};											 
//
//#endif