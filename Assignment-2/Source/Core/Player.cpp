#include "Player.h"

Player::Player(Ogre::Camera* camera, GameObject* body, Ogre::SceneManager* sm) : _body(body), _playerCam(camera), _sceneManager(sm) {
	_body->getNode()->setVisible(false);
	_body->getBody()->setAngularFactor(btVector3(0, 0, 0));

	for(int i = 0; i < averageSize; i++) {
		camAvg[i] = Ogre::Vector3::ZERO;
	}
	Ogre::Entity* item;
	Ogre::SceneNode* node;
	Ogre::SceneNode* rotNode;

	item = _sceneManager->createEntity("Pickaxe", "Mynecraft-Pickaxe.mesh");
	item->setCastShadows(true);
	node = _sceneManager->getRootSceneNode()->createChildSceneNode("Pickaxe");
	node->attachObject(item);
	node->setDirection(Ogre::Vector3(0, 1 ,0));
	node->setScale(3, 3, 3);
	node->setVisible(false);

	inventory.push_back(node);

	item = _sceneManager->createEntity("Sword", "Mynecraft-Sword.mesh");
	item->setCastShadows(true);
	node = _sceneManager->getRootSceneNode()->createChildSceneNode("Sword");
	rotNode = node->createChildSceneNode("SwordNode");
	rotNode->attachObject(item);
	rotNode->setPosition(Ogre::Vector3(-2, 6, 10));
	rotNode->roll(Ogre::Degree(90));
	rotNode->pitch(Ogre::Degree(-90));
	node->setScale(1, 1, 1);
	node->setVisible(false);

	inventory.push_back(node);


	item = _sceneManager->createEntity("Light", "torch.mesh");
	item->setCastShadows(true);
	node = _sceneManager->getRootSceneNode()->createChildSceneNode("Light");
	rotNode = node->createChildSceneNode("LightNode");
	rotNode->attachObject(item);
	rotNode->setPosition(Ogre::Vector3(0, 0, 3));
	rotNode->setDirection(Ogre::Vector3(0, -1 , 0));
	node->setScale(4, 4, 4);

	Ogre::Light* light = _sceneManager->createLight("torch");
	light->setDiffuseColour(1, .6, .05);
	light->setType(Ogre::Light::LT_POINT);
	float range = 5000.0f;
	light->setAttenuation(range, 1.0f, .5f/range, 5.0f/(range*range));
	node->attachObject(light);
	node->setVisible(false);

	inventory.push_back(node);

	item = _sceneManager->createEntity("GrassCube", "Cube-Grass.mesh");
	item->setCastShadows(true);
	node = _sceneManager->getRootSceneNode()->createChildSceneNode("GrassCube");
	rotNode = node->createChildSceneNode("GrassCubeNode");
	rotNode->attachObject(item);
	rotNode->setPosition(Ogre::Vector3(0, 0, 2));
	rotNode->roll(Ogre::Degree(90));
	rotNode->pitch(Ogre::Degree(-90));
	node->setScale(9, 9, 9);
	node->setVisible(false);

	inventory.push_back(node);

	item = _sceneManager->createEntity("RockCube", "Cube-Rock.mesh");
	item->setCastShadows(true);
	node = _sceneManager->getRootSceneNode()->createChildSceneNode("RockCube");
	rotNode = node->createChildSceneNode("RockCubeNode");
	rotNode->attachObject(item);
	rotNode->setPosition(Ogre::Vector3(0, 0, 2));
	rotNode->roll(Ogre::Degree(90));
	rotNode->pitch(Ogre::Degree(-90));
	node->setScale(9, 9, 9);
	node->setVisible(false);

	inventory.push_back(node);

	item = _sceneManager->createEntity("SnowCube", "Cube-Snow.mesh");
	item->setCastShadows(true);
	node = _sceneManager->getRootSceneNode()->createChildSceneNode("SnowCube");
	rotNode = node->createChildSceneNode("SnowCubeNode");
	rotNode->attachObject(item);
	rotNode->setPosition(Ogre::Vector3(0, 0, 2));
	rotNode->roll(Ogre::Degree(90));
	rotNode->pitch(Ogre::Degree(-90));
	node->setScale(9, 9, 9);
	node->setVisible(false);

	inventory.push_back(node);

	item = _sceneManager->createEntity("SandCube", "Cube-Sand.mesh");
	item->setCastShadows(true);
	node = _sceneManager->getRootSceneNode()->createChildSceneNode("SandCube");
	rotNode = node->createChildSceneNode("SandCubeNode");
	rotNode->attachObject(item);
	rotNode->setPosition(Ogre::Vector3(0, 0, 2));
	rotNode->roll(Ogre::Degree(90));
	rotNode->pitch(Ogre::Degree(-90));
	node->setScale(9, 9, 9);
	node->setVisible(false);

	inventory.push_back(node);

	item = _sceneManager->createEntity("DirtCube", "Cube-Dirt.mesh");
	item->setCastShadows(true);
	node = _sceneManager->getRootSceneNode()->createChildSceneNode("DirtCube");
	rotNode = node->createChildSceneNode("DirtCubeNode");
	rotNode->attachObject(item);
	rotNode->setPosition(Ogre::Vector3(0, 0, 2));
	rotNode->roll(Ogre::Degree(90));
	rotNode->pitch(Ogre::Degree(-90));
	node->setScale(9, 9, 9);
	node->setVisible(false);

	inventory.push_back(node);

	equippedItem = -1;
}

void Player::setWeapon ( int i ) {
	if ( equippedItem >= 0 && equippedItem < inventory.size() ) {
		inventory[equippedItem]->setVisible(false);
	}
	if ( i < 0 || i >= inventory.size() )
		equippedItem = -1;
	else {
		equippedItem = i;
		inventory[equippedItem]->setVisible(true);
	}
}

int Player::getWeapon() {
	return equippedItem;
}

Ogre::SceneNode* Player::getWeaponNode() {
	if ( equippedItem != -1 )
		return inventory[equippedItem];
	else
		return nullptr;
}

void Player::update(OISManager* ois) {
	static int speed = 1000;
	static int camAvgIndex = 0;

	Ogre::Vector3 movePos = _playerCam->getDirection();
	movePos = Ogre::Vector3(movePos.x, 0, movePos.z);
	movePos.normalise();
	Ogre::Vector3 strafePos = movePos.crossProduct(Ogre::Vector3(0, 1, 0));
	float currentY = _body->getBody()->getLinearVelocity().y();

	OIS::KeyCode lastkey;
	bool moved = false;

	if (ois->isKeyDown(OIS::KC_LSHIFT)) {
		speed = 2500;
	}
	else {
		speed = 1000;
	}

	if (ois->isKeyDown(OIS::KC_W)) {
		_body->setVelocity(movePos.x*speed, currentY, movePos.z*speed);
		moved = true;
	}
	if (ois->isKeyDown(OIS::KC_S)) {
		_body->setVelocity(movePos.x*-speed, currentY, movePos.z*-speed);
		moved = true;
	}
	if (ois->isKeyDown(OIS::KC_D)) {
		_body->setVelocity(strafePos.x*speed, currentY, strafePos.z*speed);
		moved = true;
	}
	if (ois->isKeyDown(OIS::KC_A)) {
		_body->setVelocity(strafePos.x*-speed, currentY, strafePos.z*-speed);
		moved = true;
	}
	if (ois->isKeyDown(OIS::KC_SPACE) && _body->canJump) {
		_body->setVelocity(_body->getBody()->getLinearVelocity().x(), 2500, _body->getBody()->getLinearVelocity().z());
		moved = true;
		_body->canJump = false;
	}

	if (!moved) {
		_body->setVelocity(0, currentY, 0);
	}

	camAvg[camAvgIndex % averageSize] = _body->getNode()->getPosition() + Ogre::Vector3(0, 170, 0);
	camAvgIndex++;

	Ogre::Vector3 total(0,0,0);
	int n = 0;

	for(int i = 0; i < averageSize; i++) {
		if(camAvg[i] != Ogre::Vector3::ZERO) {
			total += camAvg[i];
			n++;
		}
	}

	if(n > 0)
		total /= n;

	_playerCam->setPosition(total);

	Ogre::SceneNode* node = getWeaponNode();
	if ( node != nullptr ) {
		Ogre::Vector3 unit = _playerCam->getDirection().normalisedCopy();
		Ogre::Vector3 u = unit.crossProduct(Ogre::Vector3::UNIT_Y).normalisedCopy();
		Ogre::Vector3 v = unit.crossProduct(u).normalisedCopy();

		Ogre::Quaternion qt(unit, u, v);

		node->setOrientation(qt);

		Ogre::Vector3 offset = unit.crossProduct(qt.zAxis()).normalisedCopy();
		node->setPosition(total + unit*50 - offset*20);
		node->roll(Ogre::Degree(-25));
	}

}

bool Player::clickAction(StaticObject* hitObj, const btVector3& hitnormal, std::unordered_map<std::pair<int, int>, Chunk*>& chunks, std::unordered_map<std::pair<int, int>, Chunk*>& modifiedChunks) {
	if (equippedItem == PICKAXE) {
		pickaxeAction(hitObj, chunks, modifiedChunks);
		return true;
	}
	if (equippedItem == GRASS_CUBE) {
		cubePlaceAction(hitObj, hitnormal, chunks, modifiedChunks, Biome::GRASS);
		return true;
	}
	if (equippedItem == ROCK_CUBE) {
		cubePlaceAction(hitObj, hitnormal, chunks, modifiedChunks, Biome::ROCK);
		return true;
	}
	if (equippedItem == SNOW_CUBE) {
		cubePlaceAction(hitObj, hitnormal, chunks, modifiedChunks, Biome::SNOW);
		return true;
	}
	if (equippedItem == SAND_CUBE) {
		cubePlaceAction(hitObj, hitnormal, chunks, modifiedChunks, Biome::SAND);
		return true;
	}
	if (equippedItem == DIRT_CUBE) {
		cubePlaceAction(hitObj, hitnormal, chunks, modifiedChunks, Biome::DIRT);
		return true;
	}
	return false;
}

void Player::pickaxeAction(StaticObject* hitObj, std::unordered_map<std::pair<int, int>, Chunk*>& chunks, std::unordered_map<std::pair<int, int>, Chunk*>& modifiedChunks) {
	Chunk* chunk = hitObj->_chunk;

	if (chunk != nullptr) {
		int x = chunk->_xStart;
		int z = chunk->_yStart;

		if (hitObj != nullptr) {
			// Check neighboring chunks because this objects neighbors may be in seperate chunks
			std::vector<Chunk*> chunklist;
			getNeighborChunks(chunklist, x, z, chunks, chunk);

			chunk->removeBlock(chunklist, hitObj);
			chunk->modified = true;

			std::pair<int, int> name(x, z);
			modifiedChunks[name] = chunk;
		}
	}
}

void Player::cubePlaceAction(StaticObject* hitObj, const btVector3& hitnormal, std::unordered_map<std::pair<int, int>, Chunk*>& chunks, std::unordered_map<std::pair<int, int>, Chunk*>& modifiedChunks, Biome::BiomeType type) {
	Chunk* chunk = hitObj->_chunk;

	if (chunk != nullptr) {
		int x = chunk->_xStart;
		int z = chunk->_yStart;

		if (hitObj != nullptr) {
			// Check neighboring chunks because this objects neighbors may be in seperate chunks
			std::vector<Chunk*> chunklist;
			getNeighborChunks(chunklist, x, z, chunks, chunk);

			chunk->addBlock(chunklist, hitObj, hitnormal, type);
			chunk->modified = true;

			std::pair<int, int> name(x, z);
			modifiedChunks[name] = chunk;
		}
	}
}

void Player::getNeighborChunks(std::vector<Chunk*>& chunklist, int x, int z, std::unordered_map<std::pair<int, int>, Chunk*>& chunks, Chunk* chunk) {
	std::pair<int, int> left(x - CHUNK_SIZE, z);
	Chunk* leftChunk = chunks[left];

	std::pair<int, int> right(x + CHUNK_SIZE, z);
	Chunk* rightChunk = chunks[right];

	std::pair<int, int> bottom(x, z - CHUNK_SIZE);
	Chunk* bottomChunk = chunks[bottom];

	std::pair<int, int> top(x, z + CHUNK_SIZE);
	Chunk* topChunk = chunks[top];

	chunklist.push_back(leftChunk);
	chunklist.push_back(rightChunk);
	chunklist.push_back(bottomChunk);
	chunklist.push_back(topChunk);
	chunklist.push_back(chunk);
}

void Player::constrainSpeed() {
	static int maxSpeed = 15000;

	// Limit player speed
	btVector3 velocity = _body->getBody()->getLinearVelocity();
    btScalar lspeed = velocity.length();
    if(lspeed > maxSpeed) {
        velocity *= maxSpeed/lspeed;
        _body->setVelocity(velocity.x(), velocity.y(), velocity.z());
    }

	// std::cout << "Speed: " << _body->getBody()->getLinearVelocity().length() << std::endl;
}