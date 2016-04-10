#include "Cube.h"

Cube::Cube(Ogre::String nme, GameObject::objectType tp, Ogre::SceneManager* scnMgr, GameManager* ssm, Ogre::SceneNode* node, Ogre::Entity* ent, OgreMotionState* ms, Simulator* sim, Ogre::Real mss, Ogre::Real rest, Ogre::Real frict, Ogre::Vector3 scal, bool kin) : 
GameObject(nme, tp, scnMgr, ssm, node, ent, ms, sim, mss, rest, frict, scal, kin) {
	auto var = ent->getBoundingBox();
	auto size = var.getSize();

	// TODO Change Cube to PlayerObject
	//shape = new btBoxShape(btVector3((var.getSize().x*vscale.x)/2, (var.getSize().y*vscale.y)/2, (var.getSize().z*vscale.z)/2));
	shape = new btSphereShape(15);
}

void Cube::update() {
	if(context->hit) {
		canJump = true;
	}
}

void Cube::updateTransform() {
	Ogre::Vector3 pos = rootNode->getPosition();
	tr.setOrigin(btVector3(pos.x, pos.y, pos.z));

	Ogre::Quaternion qt = rootNode->getOrientation();
	tr.setRotation(btQuaternion(qt.x, qt.y, qt.z, qt.w));

	motionState->updateTransform(tr);
}