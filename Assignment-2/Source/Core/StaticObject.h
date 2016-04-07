//How this callback works in the code:
#pragma once

#include "BulletContactCallback.h"
#include "Simulator.h"
#include "CollisionContext.h"
#include "OgreMotionState.h"
#include "OISManager.h"
#include "GameManager.h"

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>
#include <OgreWindowEventUtilities.h>
#include <OgreRenderTargetListener.h>
#include <OgreParticleSystem.h>

/* Static object is as the name implies, cannot be moved or translated but exists without a scenenode for fast allocation */
class StaticObject {
protected:
	Simulator* _simulator;
	btCollisionShape* _shape;
	btRigidBody* _body;
	btTransform _tr;
	btVector3 _inertia;
	CollisionContext* _context;

	BulletContactCallback* _cCallBack;
	btScalar _mass;
	btScalar _restitution;
	btScalar _friction;
	btDefaultMotionState* _motionState;
	bool _kinematic;
	bool _needsUpdates;

public:
	Ogre::Entity* _geom;
	Ogre::Vector3 _scale;
	Ogre::Vector3 _pos;
	Ogre::Quaternion _orientation{};
	StaticObject(Ogre::Entity* mesh, Ogre::Vector3 scale, Ogre::Vector3 pos, Simulator* simulator);
	btRigidBody* getBody() { return _body; }
	void addToSimulator();
	virtual void updateTransform();
	void setVelocity(float x, float y, float z);
	void setOrientation(Ogre::Quaternion qt);

	virtual void update();
};
