#include "PlayerObject.h"

PlayerObject::PlayerObject(Ogre::String nme, GameObject::objectType tp, Ogre::SceneManager* scnMgr, GameManager* ssm, Ogre::SceneNode* node, Ogre::Entity* ent, OgreMotionState* ms, Simulator* sim, Ogre::Real mss, Ogre::Real rest, Ogre::Real frict, Ogre::Vector3 scal, bool kin) :
GameObject(nme, tp, scnMgr, ssm, node, ent, ms, sim, mss, rest, frict, scal, kin) {
	auto var = ent->getBoundingBox();
	auto size = var.getSize();

	shape = new btCapsuleShape(65, 255);
}

void PlayerObject::update() {
	if(context->hit) {
		int x = round(context->normal.x());
		int y = round(context->normal.y());
		int z = round(context->normal.z());

		if(x == 0 && y == 1 && z == 0)
			canJump = true;

		static int deathSpeed = 7500;

		// kill the player if their velocity is too high
		btVector3 velocity = getBody()->getLinearVelocity();
	    btScalar speed = velocity.length();
	    if(speed >= deathSpeed) {
	        isDead = true;
	        // std::cout << "DEAD" << std::endl;
	    }
	}
}

void PlayerObject::updateTransform() {
	Ogre::Vector3 pos = rootNode->getPosition();
	tr.setOrigin(btVector3(pos.x, pos.y, pos.z));

	Ogre::Quaternion qt = rootNode->getOrientation();
	tr.setRotation(btQuaternion(qt.x, qt.y, qt.z, qt.w));

	motionState->updateTransform(tr);
}