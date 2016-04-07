#include "StaticObject.h"

StaticObject::StaticObject(Ogre::Entity* mesh, Ogre::Vector3 scale, Ogre::Vector3 pos, Simulator* simulator) :
		_geom(mesh), _scale(scale), _pos(pos), _simulator(simulator), _mass(0.0f), _friction(0.0f), _restitution(1.0f)
{

	_inertia.setZero();
	// Placeholder shape allocation
	_shape = new btBoxShape(btVector3(0,0,0));
	_kinematic = true;
	_motionState = new btDefaultMotionState();
}

void StaticObject::update() {

}

void StaticObject::addToSimulator() {
	// using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	 updateTransform();

	// rigidbody is dynamic if and only if mass is non zero, otherwise static
	if (_mass != 0.0f) 
		_shape->calculateLocalInertia(_mass, _inertia);

	btRigidBody::btRigidBodyConstructionInfo rbInfo(_mass, _motionState, _shape, _inertia);
	rbInfo.m_restitution = _restitution;
	rbInfo.m_friction = _friction;
	_body = new btRigidBody(rbInfo);
	_body->setUserPointer(this);

	if (_kinematic) {
		std::cout << "Kinematic\n"; 
		_body->setCollisionFlags(_body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		_body->setActivationState(DISABLE_DEACTIVATION);
	}

	_context = new CollisionContext();
	_cCallBack = new BulletContactCallback(*_body, *_context);
	_simulator->addObject(this);
}

void StaticObject::updateTransform() {
	_tr.setOrigin(btVector3(_pos.x, _pos.y, _pos.z));
	_tr.setRotation(btQuaternion(_orientation.x, _orientation.y, _orientation.z, _orientation.w));
}