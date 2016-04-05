#include "Simulator.h"
#include "GameObject.h"
#include <exception>

Simulator::Simulator() : objList(), collisionShapes() { 
  collisionConfiguration = new btDefaultCollisionConfiguration(); 
  dispatcher = new btCollisionDispatcher(collisionConfiguration); 
  overlappingPairCache = new btDbvtBroadphase(); 
  solver = new btSequentialImpulseConstraintSolver(); 
  dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration); 
  dynamicsWorld->setGravity(btVector3(0.0, -3000.0f, 0.0));
  //Add collision shapes to reuse among rigid bodies
}

void Simulator::addObject (GameObject* o) { 
  objList.push_back(o); 
  dynamicsWorld->addRigidBody(o->getBody());       
}

//Update the physics world state and any objects that have collision
void Simulator::stepSimulation(const Ogre::Real elapsedTime, int maxSubSteps, const Ogre::Real fixedTimestep) {
	dynamicsWorld->stepSimulation(elapsedTime, maxSubSteps, fixedTimestep);

	// Compare each object to every other
	for (auto& outer : objList) {
		// Clear the all previous hits
		outer->cCallBack->ctxt.hit = false;
		for (auto& inner : objList) {
			if (outer == inner) continue;
			// Compare if a contact is happening between these two gameobjects
			dynamicsWorld->contactPairTest(outer->getBody(), inner->getBody(), *(outer->cCallBack));
			outer->update();
			outer->cCallBack->ctxt.hit = false;
		}
	}
}