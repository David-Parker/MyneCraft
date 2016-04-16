#include "Chunk.h"
#include "MultiPlatformHelper.h"

Chunk::Chunk(int xStart, int yStart, Ogre::SceneManager* mSceneManager, BiomeManager* biomeMgr, Perlin* perlin, Simulator* sim) : _biomeMgr(biomeMgr), _xStart(xStart), _yStart(yStart), _mSceneManager(mSceneManager), _simulator(sim) {

	_name = getChunkName(xStart, yStart);

	Ogre::Vector3 scale(CHUNK_SCALE, CHUNK_SCALE, CHUNK_SCALE);
	_scale = scale;

	_xEnd = xStart + CHUNK_SIZE;
	_yEnd = yStart + CHUNK_SIZE;

	float steepness = 90.0f;

	_sg = mSceneManager->createStaticGeometry(_name);

	Biome* curBiome = biomeMgr->inBiome(_xStart, _yStart);

	for (int i = xStart; i < _xEnd; ++i) {

		for (int j = yStart; j < _yEnd; ++j) {

			float fi = (float)i / (float)100.0f;
			float fj = (float)j / (float)100.0f;

			int y = (int)((perlin->getPerlin(fi, fj)) * steepness);

			Ogre::Vector3 pos(i*scale.x * 2, y*scale.y * 2, j*scale.z * 2);
			key index = getKey(pos);
			StaticObject* so;
			Biome::BiomeType tempType;

			if(y >= 15 && curBiome != nullptr ) {
				if (curBiome->getType() != Biome::SAND) {
					Ogre::Entity* type = curBiome->getCubeEntity(i, j, y, tempType);
					so = new StaticObject(type, tempType, scale, pos, sim);
				}
			}
			if (curBiome != nullptr) {
				Ogre::Entity* type = curBiome->getCubeEntity(i, j, y, tempType);
				so = new StaticObject(type, tempType, scale, pos, sim);
			}
				
			else {
				so = new StaticObject(biomeMgr->getTerrain(Biome::GRASS), Biome::GRASS, scale, pos, sim);
			}

			createTree(pos, so->_cubeType);

			_staticObjects[index] = so;

			_sg->addEntity(so->_geom, so->_pos, so->_orientation, so->_scale);
		}
	}

	_sg->setRegionDimensions(Ogre::Vector3(3000, 300, 3000));
	_sg->build();
}

void Chunk::generateNeighborPointers() {
	//for (int i = _xStart; i < _xEnd; ++i) {
	//	for (int j = _yStart; j < _yEnd; ++j) {
	//	key currIndex(i*CHUNK_SCALE_FULL, j*CHUNK_SCALE_FULL);
	//	StaticObject* curr = _staticObjects[currIndex];
	//	if(curr == nullptr) assert(!"Static object must exist");

	//		if(i == 0) {
	//		// Back pointer
	//			key index = getKey((i+1)*CHUNK_SCALE_FULL, 0, j*CHUNK_SCALE_FULL);
	//			StaticObject* obj = _staticObjects[index];
	//			// if(obj == nullptr) assert(!"Static object must exist");

	//			curr->back = obj;
	//		}
	//		if(j == 0) {
	//		// Right pointer
	//			key index = getKey((i)*CHUNK_SCALE_FULL, 0, (j+1)*CHUNK_SCALE_FULL);
	//			StaticObject* obj = _staticObjects[index];
	//			// if(obj == nullptr) assert(!"Static object must exist");

	//			curr->right = obj;
	//		}
	//		if(i == _xEnd - 1) {
	//		// Front pointer
	//			key index = getKey((i-1)*CHUNK_SCALE_FULL, 0, j*CHUNK_SCALE_FULL);
	//			StaticObject* obj = _staticObjects[index];
	//			// if(obj == nullptr) assert(!"Static object must exist");

	//			curr->front = obj;
	//		}
	//		if(j == _yEnd - 1) {
	//		// Left pointer
	//			key index = getKey((i)*CHUNK_SCALE_FULL, 0, (j-1)*CHUNK_SCALE_FULL);
	//			StaticObject* obj = _staticObjects[index];
	//			// if(obj == nullptr) assert(!"Static object must exist");

	//			curr->left = obj;
	//		}
	//		// Inner cube
	//		if( (i > 0 && i < _xEnd - 1) && (j > 0 && j < _yEnd - 1) ) {
	//			// All neighbors
	//			// back pointer
	//			key backindex = getKey((i+1)*CHUNK_SCALE_FULL, 0, j*CHUNK_SCALE_FULL);
	//			StaticObject* backobj = _staticObjects[backindex];
	//			// if(backobj == nullptr) assert(!"Static object must exist");

	//			curr->back = backobj;

	//			// Right pointer
	//			key rightindex = getKey((i)*CHUNK_SCALE_FULL, 0, (j+1)*CHUNK_SCALE_FULL);
	//			StaticObject* rightobj = _staticObjects[rightindex];
	//			// if(rightobj == nullptr) assert(!"Static object must exist");

	//			curr->right = rightobj;

	//			// front pointer
	//			key frontindex = getKey((i-1)*CHUNK_SCALE_FULL, 0, j*CHUNK_SCALE_FULL);
	//			StaticObject* frontobj = _staticObjects[frontindex];
	//			// if(frontobj == nullptr) assert(!"Static object must exist");

	//			curr->front = frontobj;

	//			// Left pointer
	//			key leftindex = getKey((i)*CHUNK_SCALE_FULL, 0, (j-1)*CHUNK_SCALE_FULL);
	//			StaticObject* leftobj = _staticObjects[leftindex];
	//			// if(leftobj == nullptr) assert(!"Static object must exist");

	//			curr->left = leftobj;

	//		}
	//	}
	//}
}

Chunk::~Chunk() {
	for(auto& var : _staticObjects) {
		delete var.second;
		_staticObjects[var.first] = nullptr;
	}
	_staticObjects.clear();
	_mSceneManager->destroyStaticGeometry(_sg);
}

bool Chunk::pointInChunk(float x, float y) {
	int roundX = (int) x;
	int roundY = (int) y;

	return (roundX >= _xStart*_scale.x*2 && roundX <= _xEnd*_scale.x*2) && (roundY >= _yStart*_scale.y*2 && roundY <= _yEnd*_scale.y*2);
}

void Chunk::addChunksToSimulator() {
	for (auto& var : _staticObjects) {
		if(var.second)
			var.second->addToSimulator();
	}
}

void Chunk::removeBlock(StaticObject* obj) {
	if (obj == nullptr) assert(!"Block was null");
	key index = getKey(obj->_pos);
	std::stringstream str;
	str << "Rem X: " << obj->_pos.x << " Rem Z: " << obj->_pos.z << std::endl;

	if (_staticObjects.count(index) == 0) return;

	_sg->reset();
	_staticObjects.erase(index);

	for (auto& so : _staticObjects) {
		if (so.second == nullptr) continue;
		_sg->addEntity(so.second->_geom, so.second->_pos, so.second->_orientation, so.second->_scale);
	}

	//if(obj->bottom == nullptr) {
	//	Ogre::Vector3 scale(CHUNK_SCALE, CHUNK_SCALE, CHUNK_SCALE);
	//	Ogre::Vector3 pos(obj->_pos.x, obj->_pos.y - CHUNK_SCALE_FULL, obj->_pos.z);
	//	StaticObject* so = new StaticObject(_biomeMgr->getTerrain(Biome::GRASS), Biome::GRASS, scale, pos, _simulator);
	//	_staticObjects[index] = so;
	//	_sg->addEntity(so->_geom, so->_pos, so->_orientation, so->_scale);
	//}

	_sg->build();
}

StaticObject* Chunk::getBlock(int x, int y, int z) {

	key index = getKey(x,y,z);

	StaticObject* theObj = _staticObjects[index];

	if(theObj) return theObj;
	else return nullptr;
}

void Chunk::createTree(const Ogre::Vector3& pos, Biome::BiomeType type) {

	static const int breadth = 5;
	static const int firHeight = 5;
	static const int firBreadth = 7;
	

	if ( type == Biome::GRASS && rand()%400 != 5 ) return;
	if ( type == Biome::SNOW && rand()%3000 != 5 ) return;
	if ( type == Biome::SAND && rand()%2000 != 5 ) return;

	// Describes the shape of the leaves, think of each 5x5 grid as another layer
	bool leaves[breadth][breadth][breadth] = {	{ { 0, 0, 0, 0, 0 }, { 0, 1, 1, 1, 0 }, { 0, 1, 1, 1, 0 }, { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } },
												{ { 0, 1, 1, 1, 0 }, { 1, 0, 0, 0, 1 }, { 1, 0, 0, 0, 1 }, { 1, 0, 0, 0, 1 }, { 0, 1, 1, 1, 0 } },
												{ { 0, 1, 0, 1, 0 }, { 1, 0, 0, 0, 1 }, { 1, 0, 0, 0, 1 }, { 1, 0, 0, 0, 1 }, { 0, 1, 1, 1, 0 } },
												{ { 0, 1, 1, 1, 0 }, { 1, 0, 0, 0, 1 }, { 1, 0, 0, 0, 1 }, { 1, 0, 0, 0, 1 }, { 0, 1, 1, 1, 0 } },
												{ { 0, 0, 0, 0, 0 }, { 0, 1, 1, 1, 0 }, { 0, 1, 1, 1, 0 }, { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } };

	bool firTree[firBreadth][firHeight][firBreadth] = {		{ { 0, 0, 1, 1, 1, 0, 0 }, { 0, 0, 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0 } },
															{ { 0, 1, 1, 1, 1, 1, 0 }, { 0, 0, 1, 1, 1, 0, 0 }, { 0, 0, 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0 } },
															{ { 1, 1, 1, 0, 1, 1, 1 }, { 0, 1, 1, 0, 1, 1, 0 }, { 0, 0, 1, 1, 1, 0, 0 }, { 0, 0, 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0 } },
															{ { 1, 1, 0, 0, 0, 1, 1 }, { 1, 1, 0, 0, 0, 1, 1 }, { 0, 1, 1, 0, 1, 1, 0 }, { 0, 0, 1, 0, 1, 0, 0 }, { 0, 0, 0, 1, 0, 0, 0 } },
															{ { 1, 1, 1, 0, 1, 1, 1 }, { 0, 1, 1, 0, 1, 1, 0 }, { 0, 0, 1, 1, 1, 0, 0 }, { 0, 0, 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0 } },
															{ { 0, 1, 1, 1, 1, 1, 0 }, { 0, 0, 1, 1, 1, 0, 0 }, { 0, 0, 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0 } },
															{ { 0, 0, 1, 1, 1, 0, 0 }, { 0, 0, 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0 } } };
	
	Ogre::Vector3 scale(CHUNK_SCALE, CHUNK_SCALE, CHUNK_SCALE);

	switch ( type ) {
		case Biome::GRASS:
			// Trunk
			for (int i = 1; i <= 3; i++) {
				StaticObject* treeLimb = new StaticObject(_biome->getEntity(Biome::WOOD), Biome::WOOD, scale, pos + Ogre::Vector3(0, i*CHUNK_SCALE_FULL, 0), _simulator);
				key index = getKey(treeLimb->_pos);
				_staticObjects[index] = treeLimb;
				_sg->addEntity(treeLimb->_geom, treeLimb->_pos, treeLimb->_orientation, treeLimb->_scale);
			}

			// Leaves
			for (int i = 0; i < breadth; i++) {
				for (int j = 0; j < breadth; j++) {
					for (int k = 0; k < breadth; k++) {
						if (leaves[i][j][k]) {
							StaticObject* treeLimb = new StaticObject(_biome->getEntity(Biome::LEAF), Biome::LEAF, scale, pos + Ogre::Vector3((i - 2)*CHUNK_SCALE_FULL, (j + breadth - 1)*CHUNK_SCALE_FULL, (k - 2)*CHUNK_SCALE_FULL), _simulator);
							key index = getKey(treeLimb->_pos);
							_staticObjects[index] = treeLimb;
							_sg->addEntity(treeLimb->_geom, treeLimb->_pos, treeLimb->_orientation, treeLimb->_scale);
						}
					}
				}
			}
			break;
		case Biome::SNOW:
			//Fir Tree
			for (int i = 0; i < firBreadth; i++) {
				for (int j = 0; j < firHeight; j++) {
					for (int k = 0; k < firBreadth; k++) {
						if (firTree[i][j][k]) {
							StaticObject* firBlock = new StaticObject(_biome->getEntity(Biome::ICE), Biome::ICE, scale, pos + Ogre::Vector3((i - 3)*CHUNK_SCALE_FULL, (2*j+3)*CHUNK_SCALE_FULL, (k - 3)*CHUNK_SCALE_FULL), _simulator);
							key index = getKey(firBlock->_pos);
							_staticObjects[index] = firBlock;
							_sg->addEntity(firBlock->_geom, firBlock->_pos, firBlock->_orientation, firBlock->_scale);
						}
					}
				}
			}
			//Trunk
			for (int i = 0 ; i <= firHeight*2 ; i++ ) {
				StaticObject* firBlock = new StaticObject(_biome->getEntity(Biome::WOOD), Biome::WOOD, scale, pos + Ogre::Vector3(0, i*CHUNK_SCALE_FULL, 0), _simulator);
				key index = getKey(firBlock->_pos);
				_staticObjects[index] = firBlock;
				_sg->addEntity(firBlock->_geom, firBlock->_pos, firBlock->_orientation, firBlock->_scale);
			}
			break;
		case Biome::SAND:
			//Cactus
			for (int i = 1; i <= 10; i++) {
				StaticObject* stalk = new StaticObject(_biome->getEntity(Biome::CACTUS), Biome::CACTUS, scale, pos + Ogre::Vector3(0, i*CHUNK_SCALE_FULL, 0), _simulator);
				key index = getKey(stalk->_pos);
				_staticObjects[index] = stalk;
				_sg->addEntity(stalk->_geom, stalk->_pos, stalk->_orientation, stalk->_scale);
			}
			//Arms
			if ( rand()%2 ) {
				StaticObject* armConnector1 = new StaticObject(_biome->getEntity(Biome::CACTUS), Biome::CACTUS, scale, pos + Ogre::Vector3(CHUNK_SCALE_FULL, 4*CHUNK_SCALE_FULL, 0), _simulator);
				key index = getKey(armConnector1->_pos);
				_staticObjects[index] = armConnector1;
				_sg->addEntity(armConnector1->_geom, armConnector1->_pos, armConnector1->_orientation, armConnector1->_scale);
			
				StaticObject* armConnector2 = new StaticObject(_biome->getEntity(Biome::CACTUS), Biome::CACTUS, scale, pos + Ogre::Vector3(-CHUNK_SCALE_FULL, 4*CHUNK_SCALE_FULL, 0), _simulator);
				key index2 = getKey(armConnector2->_pos);
				_staticObjects[index2] = armConnector2;
				_sg->addEntity(armConnector2->_geom, armConnector2->_pos, armConnector2->_orientation, armConnector2->_scale);
				
				for (int i = 4; i <= 8; i++) {
					StaticObject* armBlock = new StaticObject(_biome->getEntity(Biome::CACTUS), Biome::CACTUS, scale, pos + Ogre::Vector3(2*CHUNK_SCALE_FULL, i*CHUNK_SCALE_FULL, 0), _simulator);
					key index = getKey(armBlock->_pos);
					_staticObjects[index] = armBlock;
					_sg->addEntity(armBlock->_geom, armBlock->_pos, armBlock->_orientation, armBlock->_scale);
				}
				for (int i = 4; i <= 9; i++) {
					StaticObject* armBlock = new StaticObject(_biome->getEntity(Biome::CACTUS), Biome::CACTUS, scale, pos + Ogre::Vector3(-2*CHUNK_SCALE_FULL, i*CHUNK_SCALE_FULL, 0), _simulator);
					key index = getKey(armBlock->_pos);
					_staticObjects[index] = armBlock;
					_sg->addEntity(armBlock->_geom, armBlock->_pos, armBlock->_orientation, armBlock->_scale);
				}
			}
			else {
				StaticObject* armConnector1 = new StaticObject(_biome->getEntity(Biome::CACTUS), Biome::CACTUS, scale, pos + Ogre::Vector3(0, 4*CHUNK_SCALE_FULL, CHUNK_SCALE_FULL), _simulator);
				key index = getKey(armConnector1->_pos);
				_staticObjects[index] = armConnector1;
				_sg->addEntity(armConnector1->_geom, armConnector1->_pos, armConnector1->_orientation, armConnector1->_scale);
			
				StaticObject* armConnector2 = new StaticObject(_biome->getEntity(Biome::CACTUS), Biome::CACTUS, scale, pos + Ogre::Vector3(0, 4*CHUNK_SCALE_FULL, -CHUNK_SCALE_FULL), _simulator);
				key index2 = getKey(armConnector2->_pos);
				_staticObjects[index2] = armConnector2;
				_sg->addEntity(armConnector2->_geom, armConnector2->_pos, armConnector2->_orientation, armConnector2->_scale);
				
				for (int i = 4; i <= 8; i++) {
					StaticObject* armBlock = new StaticObject(_biome->getEntity(Biome::CACTUS), Biome::CACTUS, scale, pos + Ogre::Vector3(0, i*CHUNK_SCALE_FULL, 2*CHUNK_SCALE_FULL), _simulator);
					key index = getKey(armBlock->_pos);
					_staticObjects[index] = armBlock;
					_sg->addEntity(armBlock->_geom, armBlock->_pos, armBlock->_orientation, armBlock->_scale);
				}
				for (int i = 4; i <= 9; i++) {
					StaticObject* armBlock = new StaticObject(_biome->getEntity(Biome::CACTUS), Biome::CACTUS, scale, pos + Ogre::Vector3(0, i*CHUNK_SCALE_FULL, -2*CHUNK_SCALE_FULL), _simulator);
					key index = getKey(armBlock->_pos);
					_staticObjects[index] = armBlock;
					_sg->addEntity(armBlock->_geom, armBlock->_pos, armBlock->_orientation, armBlock->_scale);
				}
			}
			break;
	}
}

Chunk::key Chunk::getKey(int x, int y, int z) {
	char buf[64];
	sprintf(buf, "%d_%d_%d", x, y, z);
	return std::string(buf);
}

Chunk::key Chunk::getKey(Ogre::Vector3& pos) {
	return getKey((int)pos.x, (int)pos.y, (int)pos.z);
}