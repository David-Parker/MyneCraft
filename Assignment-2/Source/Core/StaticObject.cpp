#include "StaticObject.h"
#include "MultiPlatformHelper.h"

StaticObject::StaticObject(Ogre::Entity* mesh, Ogre::Vector3 scale, Ogre::Vector3 pos, Simulator* simulator) :
		_geom(mesh), _scale(scale), _pos(pos), _simulator(simulator), _mass(1.0f), _friction(0.0f), _restitution(0.0f)
{
	auto var = mesh->getBoundingBox();
	auto size = var.getSize();

	_inertia.setZero();
	_shape = new btBoxShape(btVector3((var.getSize().x*scale.x) / 2, (var.getSize().y*scale.y) / 2, (var.getSize().z*scale.z) / 2));
	_motionState = new btDefaultMotionState();

	std::string type = mesh->getMesh()->getName();

	// TODO refactor Chunk to pass in the type instead of name checking the mesh name
	if (type == "Cube-Grass.mesh") _cubeType = Biome::GRASS;
	else if (type == "Cube-Snow.mesh") _cubeType = Biome::SNOW;
	else if (type == "Cube-Sand.mesh") _cubeType = Biome::SAND;
	else if (type == "Cube-Wood.mesh") _cubeType = Biome::WOOD;
	else if (type == "Cube-Leaves.mesh") _cubeType = Biome::LEAF;

}

StaticObject::~StaticObject() {
	cleanUpBody();
	delete _shape;
	delete _motionState;
}

void StaticObject::update() {}

void StaticObject::addToSimulator() {
	// using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	 updateTransform();
	 _collisionObject = new btCollisionObject();
	 _collisionObject->setCollisionShape(_shape);
	 _collisionObject->setWorldTransform(_tr);
	 _collisionObject->setCollisionFlags(_collisionObject->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
	 _collisionObject->setFriction(_friction);
	 _collisionObject->setRestitution(_restitution);

	_simulator->addObject(this);
}

void StaticObject::updateTransform() {
	_tr.setOrigin(btVector3(_pos.x, _pos.y, _pos.z));
	_tr.setRotation(btQuaternion(_orientation.x, _orientation.y, _orientation.z, _orientation.w));
}

void StaticObject::cleanUpBody() {
	if(_collisionObject != nullptr) {
		delete _collisionObject;
		_collisionObject = nullptr;	
	}
}