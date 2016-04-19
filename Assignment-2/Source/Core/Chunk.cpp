#include "Chunk.h"
#include "MultiPlatformHelper.h"

Chunk::Chunk(int xStart, int yStart, Ogre::SceneManager* mSceneManager, BiomeManager* biomeMgr, Perlin* perlin, Simulator* sim) : _biomeMgr(biomeMgr), _xStart(xStart), _yStart(yStart), _mSceneManager(mSceneManager), _simulator(sim) {
	if (air == nullptr) air = new StaticObject(nullptr, Biome::AIR, Ogre::Vector3(CHUNK_SCALE, CHUNK_SCALE, CHUNK_SCALE), Ogre::Vector3::ZERO, sim, this);

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
					so = new StaticObject(type, tempType, scale, pos, sim, this);
				}
			}
			if (curBiome != nullptr) {
				Ogre::Entity* type = curBiome->getCubeEntity(i, j, y, tempType);
				so = new StaticObject(type, tempType, scale, pos, sim, this);
			}
				
			else {
				so = new StaticObject(biomeMgr->getTerrain(Biome::GRASS), Biome::GRASS, scale, pos, sim, this);
			}

			createTree(pos, so->_cubeType);

			key airIndex = getKey(pos + Ogre::Vector3(0, CHUNK_SCALE_FULL, 0));

			_staticObjects[index] = so;
			_staticObjects[airIndex] = air;

			_sg->addEntity(so->_geom, so->_pos, so->_orientation, so->_scale);
		}
	}

	_sg->setRegionDimensions(Ogre::Vector3(3000, 300, 3000));
	_sg->build();
}

Chunk::~Chunk() {
	for(auto& var : _staticObjects) {
		if (var.second == nullptr || var.second == air) continue;
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
		if(var.second != nullptr && var.second != air)
			var.second->addToSimulator();
	}
}

void Chunk::removeBlock(const std::vector<Chunk*>& chunks, StaticObject* obj) {
	if (obj == nullptr) assert(!"Block was null");
	if (obj == air) return;
	
	key index = getKey(obj->_pos);
	std::stringstream str;
	str << "Rem X: " << obj->_pos.x << " Rem Z: " << obj->_pos.z << std::endl;

	if (_staticObjects.count(index) == 0) return;

	_sg->reset();
	_staticObjects[index] = air;

	for (auto& so : _staticObjects) {
		if (so.second == air || so.second == nullptr) continue;
		_sg->addEntity(so.second->_geom, so.second->_pos, so.second->_orientation, so.second->_scale);
	}

	// Only create neighbors for non-tree blocks
	if (!_biomeMgr->isTreeType(obj->_cubeType)) {
		// top
		Biome::BiomeType newType = getGeneratedType(obj->_cubeType);

		Ogre::Vector3 topPos = obj->_pos + Ogre::Vector3(0, CHUNK_SCALE_FULL, 0);
		key topIndex = getKey(topPos);
		StaticObject* topObj = getObjFromChunks(chunks, topIndex);
		if (topObj == nullptr) {
			Ogre::Vector3 scale(CHUNK_SCALE, CHUNK_SCALE, CHUNK_SCALE);
			StaticObject* so = new StaticObject(_biomeMgr->getTerrain(newType), newType, scale, topPos, _simulator, this);
			if(so == nullptr) assert(!"StaticObject is null");
			_staticObjects[topIndex] = so;
			_sg->addEntity(so->_geom, so->_pos, so->_orientation, so->_scale);
		}
		else {
			if (topObj == air) {
				// Do nothing
			}
			else {
				// Do nothing
			}
		}
		// bottom
		Ogre::Vector3 bottomPos = obj->_pos + Ogre::Vector3(0, -CHUNK_SCALE_FULL, 0);
		key bottomIndex = getKey(bottomPos);
		StaticObject* bottomObj = getObjFromChunks(chunks, bottomIndex);
		if (bottomObj == nullptr) {
			Ogre::Vector3 scale(CHUNK_SCALE, CHUNK_SCALE, CHUNK_SCALE);
			StaticObject* so = new StaticObject(_biomeMgr->getTerrain(newType), newType, scale, bottomPos, _simulator, this);
			_staticObjects[bottomIndex] = so;
			_sg->addEntity(so->_geom, so->_pos, so->_orientation, so->_scale);
		}

		// left
		Ogre::Vector3 leftPos = obj->_pos + Ogre::Vector3(-CHUNK_SCALE_FULL, 0, 0);
		key leftIndex = getKey(leftPos);
		StaticObject* leftObj = getObjFromChunks(chunks, leftIndex);
		if (leftObj == nullptr) {
			Ogre::Vector3 scale(CHUNK_SCALE, CHUNK_SCALE, CHUNK_SCALE);
			StaticObject* so = new StaticObject(_biomeMgr->getTerrain(newType), newType, scale, leftPos, _simulator, this);
			_staticObjects[leftIndex] = so;
			_sg->addEntity(so->_geom, so->_pos, so->_orientation, so->_scale);
		}

		// right
		Ogre::Vector3 rightPos = obj->_pos + Ogre::Vector3(CHUNK_SCALE_FULL, 0, 0);
		key rightIndex = getKey(rightPos);
		StaticObject* rightObj = getObjFromChunks(chunks, rightIndex);
		if (rightObj == nullptr) {
			Ogre::Vector3 scale(CHUNK_SCALE, CHUNK_SCALE, CHUNK_SCALE);
			StaticObject* so = new StaticObject(_biomeMgr->getTerrain(newType), newType, scale, rightPos, _simulator, this);
			_staticObjects[rightIndex] = so;
			_sg->addEntity(so->_geom, so->_pos, so->_orientation, so->_scale);
		}

		// back
		Ogre::Vector3 backPos = obj->_pos + Ogre::Vector3(0, 0, -CHUNK_SCALE_FULL);
		key backIndex = getKey(backPos);
		StaticObject* backObj = getObjFromChunks(chunks, backIndex);
		if (backObj == nullptr) {
			Ogre::Vector3 scale(CHUNK_SCALE, CHUNK_SCALE, CHUNK_SCALE);
			StaticObject* so = new StaticObject(_biomeMgr->getTerrain(newType), newType, scale, backPos, _simulator, this);
			_staticObjects[backIndex] = so;
			_sg->addEntity(so->_geom, so->_pos, so->_orientation, so->_scale);
		}

		// front
		Ogre::Vector3 frontPos = obj->_pos + Ogre::Vector3(0, 0, CHUNK_SCALE_FULL);
		key frontIndex = getKey(frontPos);
		StaticObject* frontObj = getObjFromChunks(chunks, frontIndex);
		if (frontObj == nullptr) {
			Ogre::Vector3 scale(CHUNK_SCALE, CHUNK_SCALE, CHUNK_SCALE);
			StaticObject* so = new StaticObject(_biomeMgr->getTerrain(newType), newType, scale, frontPos, _simulator, this);
			_staticObjects[frontIndex] = so;
			_sg->addEntity(so->_geom, so->_pos, so->_orientation, so->_scale);
		}
	}

	_sg->build();
}

StaticObject* Chunk::getObjFromChunks(const std::vector<Chunk*>& chunks, key index) {
	for (auto& var : chunks) {
		if (var->_staticObjects[index]) return var->_staticObjects[index];
	}

	return nullptr;
}

StaticObject* Chunk::getBlock(int x, int y, int z) {

	key index = getKey(x,y,z);

	StaticObject* theObj = _staticObjects[index];

	if(theObj) return theObj;
	else return nullptr;
}

// This is just temporary and needs to be improved
Biome::BiomeType Chunk::getGeneratedType(Biome::BiomeType objType) {
	switch (objType) {
		case Biome::GRASS :
			return Biome::ROCK;
			break;
		case Biome::SAND :
			return Biome::SAND;
			break;
		case Biome::SNOW :
			return Biome::GRASS;
			break;
		case Biome::ROCK :
			return Biome::ROCK;
			break;
		default :
			return objType;
	}
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
				StaticObject* treeLimb = new StaticObject(_biome->getEntity(Biome::WOOD), Biome::WOOD, scale, pos + Ogre::Vector3(0, i*CHUNK_SCALE_FULL, 0), _simulator, this);
				key index = getKey(treeLimb->_pos);
				_staticObjects[index] = treeLimb;
				_sg->addEntity(treeLimb->_geom, treeLimb->_pos, treeLimb->_orientation, treeLimb->_scale);
			}

			// Leaves
			for (int i = 0; i < breadth; i++) {
				for (int j = 0; j < breadth; j++) {
					for (int k = 0; k < breadth; k++) {
						if (leaves[i][j][k]) {
							StaticObject* treeLimb = new StaticObject(_biome->getEntity(Biome::LEAF), Biome::LEAF, scale, pos + Ogre::Vector3((i - 2)*CHUNK_SCALE_FULL, (j + breadth - 1)*CHUNK_SCALE_FULL, (k - 2)*CHUNK_SCALE_FULL), _simulator, this);
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
							StaticObject* firBlock = new StaticObject(_biome->getEntity(Biome::ICE), Biome::ICE, scale, pos + Ogre::Vector3((i - 3)*CHUNK_SCALE_FULL, (2*j+3)*CHUNK_SCALE_FULL, (k - 3)*CHUNK_SCALE_FULL), _simulator, this);
							key index = getKey(firBlock->_pos);
							_staticObjects[index] = firBlock;
							_sg->addEntity(firBlock->_geom, firBlock->_pos, firBlock->_orientation, firBlock->_scale);
						}
					}
				}
			}
			//Trunk
			for (int i = 0 ; i <= firHeight*2 ; i++ ) {
				StaticObject* firBlock = new StaticObject(_biome->getEntity(Biome::WOOD), Biome::WOOD, scale, pos + Ogre::Vector3(0, i*CHUNK_SCALE_FULL, 0), _simulator, this);
				key index = getKey(firBlock->_pos);
				_staticObjects[index] = firBlock;
				_sg->addEntity(firBlock->_geom, firBlock->_pos, firBlock->_orientation, firBlock->_scale);
			}
			break;
		case Biome::SAND:
			//Cactus
			for (int i = 1; i <= 10; i++) {
				StaticObject* stalk = new StaticObject(_biome->getEntity(Biome::CACTUS), Biome::CACTUS, scale, pos + Ogre::Vector3(0, i*CHUNK_SCALE_FULL, 0), _simulator, this);
				key index = getKey(stalk->_pos);
				_staticObjects[index] = stalk;
				_sg->addEntity(stalk->_geom, stalk->_pos, stalk->_orientation, stalk->_scale);
			}
			//Arms
			if ( rand()%2 ) {
				StaticObject* armConnector1 = new StaticObject(_biome->getEntity(Biome::CACTUS), Biome::CACTUS, scale, pos + Ogre::Vector3(CHUNK_SCALE_FULL, 4*CHUNK_SCALE_FULL, 0), _simulator, this);
				key index = getKey(armConnector1->_pos);
				_staticObjects[index] = armConnector1;
				_sg->addEntity(armConnector1->_geom, armConnector1->_pos, armConnector1->_orientation, armConnector1->_scale);
			
				StaticObject* armConnector2 = new StaticObject(_biome->getEntity(Biome::CACTUS), Biome::CACTUS, scale, pos + Ogre::Vector3(-CHUNK_SCALE_FULL, 4*CHUNK_SCALE_FULL, 0), _simulator, this);
				key index2 = getKey(armConnector2->_pos);
				_staticObjects[index2] = armConnector2;
				_sg->addEntity(armConnector2->_geom, armConnector2->_pos, armConnector2->_orientation, armConnector2->_scale);
				
				for (int i = 4; i <= 8; i++) {
					StaticObject* armBlock = new StaticObject(_biome->getEntity(Biome::CACTUS), Biome::CACTUS, scale, pos + Ogre::Vector3(2*CHUNK_SCALE_FULL, i*CHUNK_SCALE_FULL, 0), _simulator, this);
					key index = getKey(armBlock->_pos);
					_staticObjects[index] = armBlock;
					_sg->addEntity(armBlock->_geom, armBlock->_pos, armBlock->_orientation, armBlock->_scale);
				}
				for (int i = 4; i <= 9; i++) {
					StaticObject* armBlock = new StaticObject(_biome->getEntity(Biome::CACTUS), Biome::CACTUS, scale, pos + Ogre::Vector3(-2*CHUNK_SCALE_FULL, i*CHUNK_SCALE_FULL, 0), _simulator, this);
					key index = getKey(armBlock->_pos);
					_staticObjects[index] = armBlock;
					_sg->addEntity(armBlock->_geom, armBlock->_pos, armBlock->_orientation, armBlock->_scale);
				}
			}
			else {
				StaticObject* armConnector1 = new StaticObject(_biome->getEntity(Biome::CACTUS), Biome::CACTUS, scale, pos + Ogre::Vector3(0, 4*CHUNK_SCALE_FULL, CHUNK_SCALE_FULL), _simulator, this);
				key index = getKey(armConnector1->_pos);
				_staticObjects[index] = armConnector1;
				_sg->addEntity(armConnector1->_geom, armConnector1->_pos, armConnector1->_orientation, armConnector1->_scale);
			
				StaticObject* armConnector2 = new StaticObject(_biome->getEntity(Biome::CACTUS), Biome::CACTUS, scale, pos + Ogre::Vector3(0, 4*CHUNK_SCALE_FULL, -CHUNK_SCALE_FULL), _simulator, this);
				key index2 = getKey(armConnector2->_pos);
				_staticObjects[index2] = armConnector2;
				_sg->addEntity(armConnector2->_geom, armConnector2->_pos, armConnector2->_orientation, armConnector2->_scale);
				
				for (int i = 4; i <= 8; i++) {
					StaticObject* armBlock = new StaticObject(_biome->getEntity(Biome::CACTUS), Biome::CACTUS, scale, pos + Ogre::Vector3(0, i*CHUNK_SCALE_FULL, 2*CHUNK_SCALE_FULL), _simulator, this);
					key index = getKey(armBlock->_pos);
					_staticObjects[index] = armBlock;
					_sg->addEntity(armBlock->_geom, armBlock->_pos, armBlock->_orientation, armBlock->_scale);
				}
				for (int i = 4; i <= 9; i++) {
					StaticObject* armBlock = new StaticObject(_biome->getEntity(Biome::CACTUS), Biome::CACTUS, scale, pos + Ogre::Vector3(0, i*CHUNK_SCALE_FULL, -2*CHUNK_SCALE_FULL), _simulator, this);
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

Chunk::key Chunk::getKey(const Ogre::Vector3& pos) {
	return getKey((int)pos.x, (int)pos.y, (int)pos.z);
}