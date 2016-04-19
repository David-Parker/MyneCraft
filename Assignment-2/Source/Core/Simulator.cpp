#include "Simulator.h"
#include "GameObject.h"
#include "StaticObject.h"
#include <exception>

Simulator::Simulator() : objList(), collisionShapes(), objListStatic() { 
  collisionConfiguration = new btDefaultCollisionConfiguration(); 
  dispatcher = new btCollisionDispatcher(collisionConfiguration); 
  overlappingPairCache = new btDbvtBroadphase();
  //overlappingPairCache = new bt32BitAxisSweep3(btVector3(-15000,-15000,-15000), btVector3(15000, 15000, 15000));
  solver = new btSequentialImpulseConstraintSolver(); 
  dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration); 
  dynamicsWorld->setGravity(btVector3(0.0, -25000.0f, 0.0));
  //Add collision shapes to reuse among rigid bodies
}

void Simulator::addObject (GameObject* o) { 
  objList.push_back(o); 
  dynamicsWorld->addRigidBody(o->getBody());       
}

void Simulator::addObject (StaticObject* o) { 
  objListStatic.push_back(o); 
  invertedObjectHash[o->getBody()] = o;
  dynamicsWorld->addCollisionObject(o->getBody());       
}

//Update the physics world state and any objects that have collision
void Simulator::stepSimulation(const Ogre::Real elapsedTime, int maxSubSteps, const Ogre::Real fixedTimestep) {
	dynamicsWorld->stepSimulation(elapsedTime, maxSubSteps, fixedTimestep);

	for(auto& var : objList) {
		dynamicsWorld->contactTest(var->getBody(), *(var->cCallBack));
		var->update();
		var->cCallBack->ctxt.hit = false;
	}
}

void Simulator::removeStaticObjects() {
	for (auto& var : objListStatic) {
		dynamicsWorld->removeCollisionObject(var->getBody());
		var->cleanUpBody();
	}
	objListStatic.clear();
	invertedObjectHash.clear();
}

bool Simulator::rayHit(const btVector3& start, const btVector3& end, StaticObject*& obj) {
	btCollisionWorld::ClosestRayResultCallback RayCallback(start, end);

	dynamicsWorld->rayTest(start, end, RayCallback);

	if (RayCallback.hasHit()) {
		obj = invertedObjectHash[RayCallback.m_collisionObject];
		return true;
	}

	return false;
}