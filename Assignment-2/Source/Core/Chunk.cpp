#include "Chunk.h"
#include "MultiPlatformHelper.h"

Chunk::Chunk(int xStart, int yStart, Ogre::SceneManager* mSceneManager, BiomeManager* biomeMgr, Perlin* perlin, Simulator* sim, bool generate) : _biomeMgr(biomeMgr), _xStart(xStart), _yStart(yStart), _mSceneManager(mSceneManager), _simulator(sim) {
	if (air == nullptr) air = new StaticObject(nullptr, CubeManager::AIR, Ogre::Vector3(CHUNK_SCALE, CHUNK_SCALE, CHUNK_SCALE), Ogre::Vector3::ZERO, sim, this);

	_name = getChunkName(xStart, yStart);

	Ogre::Vector3 scale(CHUNK_SCALE, CHUNK_SCALE, CHUNK_SCALE);
	_scale = scale;

	_xEnd = xStart + CHUNK_SIZE;
	_yEnd = yStart + CHUNK_SIZE;

	float steepness = 90.0f;

	_sg = mSceneManager->createStaticGeometry(_name);

	Biome* curBiome = biomeMgr->inBiome(_xStart, _yStart);

	// Does this chunk generate new terrain?
	if (generate) {
		for (int i = xStart; i < _xEnd; ++i) {

			for (int j = yStart; j < _yEnd; ++j) {

				float fi = (float)i / (float)100.0f;
				float fj = (float)j / (float)100.0f;

				int y = (int)((perlin->getPerlin(fi, fj)) * steepness);

				Ogre::Vector3 pos(i*scale.x * 2, y*scale.y * 2, j*scale.z * 2);
				key index = getKey(pos);
				StaticObject* so;
				CubeManager::CubeType tempType;

				if (y >= 15 && curBiome != nullptr) {
					if (curBiome->getType() != CubeManager::SAND) {
						Ogre::Entity* type = curBiome->getCubeEntity(i, j, y, tempType);
						so = new StaticObject(type, tempType, scale, pos, sim, this);
					}
				}
				if (curBiome != nullptr) {
					Ogre::Entity* type = curBiome->getCubeEntity(i, j, y, tempType);
					so = new StaticObject(type, tempType, scale, pos, sim, this);
				}

				else {
					so = new StaticObject(CubeManager::getSingleton()->getEntity(CubeManager::GRASS), CubeManager::GRASS, scale, pos, sim, this);
				}

				_staticObjects[index] = so;

				// Create tree returns true if a tree was created in this position.
				if (!createTree(pos, so->_cubeType)) {
					key airIndex = getKey(pos + Ogre::Vector3(0, CHUNK_SCALE_FULL, 0));
					_staticObjects[airIndex] = air;
				}

				createCloud(pos);

				_sg->addEntity(so->_geom, so->_pos, so->_orientation, so->_scale);
			}
		}

		_sg->setRegionDimensions(Ogre::Vector3(3000, 300, 3000));
		_sg->build();
	}
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

	if (_staticObjects.count(index) == 0) return;

	_sg->reset();
	_staticObjects[index] = air;

	key tkey = "torch_" + index;
	if (lights[tkey])
		lights[tkey]->setVisible(false);

	for (auto& so : _staticObjects) {
		if (so.second == air || so.second == nullptr) continue;
		_sg->addEntity(so.second->_geom, so.second->_pos, so.second->_orientation, so.second->_scale);
	}

	// Only create neighbors for non-tree blocks
	if (!_biomeMgr->isTreeType(obj->_cubeType)) {
		// top
		CubeManager::CubeType newType = getGeneratedType(obj->_cubeType, (int) obj->_pos.y);

		Ogre::Vector3 topPos = obj->_pos + Ogre::Vector3(0, CHUNK_SCALE_FULL, 0);
		key topIndex = getKey(topPos);
		StaticObject* topObj = getObjFromChunks(chunks, topIndex);
		if (topObj == nullptr) {
			Ogre::Vector3 scale(CHUNK_SCALE, CHUNK_SCALE, CHUNK_SCALE);
			StaticObject* so = new StaticObject(CubeManager::getSingleton()->getEntity(newType), newType, scale, topPos, _simulator, this);
			if(so == nullptr) assert(!"StaticObject is null");
			_staticObjects[topIndex] = so;
			_sg->addEntity(so->_geom, so->_pos, so->_orientation, so->_scale);
		}

		// bottom
		Ogre::Vector3 bottomPos = obj->_pos + Ogre::Vector3(0, -CHUNK_SCALE_FULL, 0);
		key bottomIndex = getKey(bottomPos);
		StaticObject* bottomObj = getObjFromChunks(chunks, bottomIndex);
		if (bottomObj == nullptr) {
			Ogre::Vector3 scale(CHUNK_SCALE, CHUNK_SCALE, CHUNK_SCALE);
			StaticObject* so = new StaticObject(CubeManager::getSingleton()->getEntity(newType), newType, scale, bottomPos, _simulator, this);
			_staticObjects[bottomIndex] = so;
			_sg->addEntity(so->_geom, so->_pos, so->_orientation, so->_scale);
		}

		// left
		Ogre::Vector3 leftPos = obj->_pos + Ogre::Vector3(-CHUNK_SCALE_FULL, 0, 0);
		key leftIndex = getKey(leftPos);
		StaticObject* leftObj = getObjFromChunks(chunks, leftIndex);
		if (leftObj == nullptr) {
			Ogre::Vector3 scale(CHUNK_SCALE, CHUNK_SCALE, CHUNK_SCALE);
			StaticObject* so = new StaticObject(CubeManager::getSingleton()->getEntity(newType), newType, scale, leftPos, _simulator, this);
			_staticObjects[leftIndex] = so;
			_sg->addEntity(so->_geom, so->_pos, so->_orientation, so->_scale);
		}

		// right
		Ogre::Vector3 rightPos = obj->_pos + Ogre::Vector3(CHUNK_SCALE_FULL, 0, 0);
		key rightIndex = getKey(rightPos);
		StaticObject* rightObj = getObjFromChunks(chunks, rightIndex);
		if (rightObj == nullptr) {
			Ogre::Vector3 scale(CHUNK_SCALE, CHUNK_SCALE, CHUNK_SCALE);
			StaticObject* so = new StaticObject(CubeManager::getSingleton()->getEntity(newType), newType, scale, rightPos, _simulator, this);
			_staticObjects[rightIndex] = so;
			_sg->addEntity(so->_geom, so->_pos, so->_orientation, so->_scale);
		}

		// back
		Ogre::Vector3 backPos = obj->_pos + Ogre::Vector3(0, 0, -CHUNK_SCALE_FULL);
		key backIndex = getKey(backPos);
		StaticObject* backObj = getObjFromChunks(chunks, backIndex);
		if (backObj == nullptr) {
			Ogre::Vector3 scale(CHUNK_SCALE, CHUNK_SCALE, CHUNK_SCALE);
			StaticObject* so = new StaticObject(CubeManager::getSingleton()->getEntity(newType), newType, scale, backPos, _simulator, this);
			_staticObjects[backIndex] = so;
			_sg->addEntity(so->_geom, so->_pos, so->_orientation, so->_scale);
		}

		// front
		Ogre::Vector3 frontPos = obj->_pos + Ogre::Vector3(0, 0, CHUNK_SCALE_FULL);
		key frontIndex = getKey(frontPos);
		StaticObject* frontObj = getObjFromChunks(chunks, frontIndex);
		if (frontObj == nullptr) {
			Ogre::Vector3 scale(CHUNK_SCALE, CHUNK_SCALE, CHUNK_SCALE);
			StaticObject* so = new StaticObject(CubeManager::getSingleton()->getEntity(newType), newType, scale, frontPos, _simulator, this);
			_staticObjects[frontIndex] = so;
			_sg->addEntity(so->_geom, so->_pos, so->_orientation, so->_scale);
		}
	}

	_sg->build();
}

void Chunk::addBlock(const std::vector<Chunk*>& chunks, StaticObject* obj, const btVector3& hitnormal, CubeManager::CubeType newType) {
	if (obj == nullptr) assert(!"Block was null");
	if (obj == air) return;

	// Sometimes the vector comes in with very small values close to zero, just truncate values to 0 or 1
	int x = round(hitnormal.x());
	int y = round(hitnormal.y());
	int z = round(hitnormal.z());
	
	Ogre::Vector3 scale;
	if (newType == CubeManager::TORCH)
		scale = Ogre::Vector3(CHUNK_SCALE/5, 4*CHUNK_SCALE/5, CHUNK_SCALE/5);
	else
		scale = Ogre::Vector3(CHUNK_SCALE, CHUNK_SCALE, CHUNK_SCALE);

	btVector3 norm = btVector3(x, y, z);

	key index = getKey(obj->_pos);

	if (_staticObjects.count(index) == 0) return;

	_sg->reset();

	for (auto& so : _staticObjects) {
		if (so.second == air || so.second == nullptr) continue;
		_sg->addEntity(so.second->_geom, so.second->_pos, so.second->_orientation, so.second->_scale);
	}
	
	Ogre::Vector3 pos;

	// Top
	if (norm == btVector3(0, 1, 0)) {
		pos = obj->_pos + Ogre::Vector3(0, CHUNK_SCALE_FULL, 0);
		key topIndex = getKey(pos);
		StaticObject* topObj = getObjFromChunks(chunks, topIndex);

		if (topObj == air) {
			StaticObject* so = new StaticObject(CubeManager::getSingleton()->getEntity(newType), newType, scale, pos, _simulator, this);
			if (so == nullptr) assert(!"StaticObject is null");
			_staticObjects[topIndex] = so;
			_sg->addEntity(so->_geom, so->_pos, so->_orientation, so->_scale);
			createAirNeighbors(chunks, so);
		}
	}

	// Bottom
	if (norm == btVector3(0, -1, 0)) {
		pos = obj->_pos + Ogre::Vector3(0, -CHUNK_SCALE_FULL, 0);
		key index = getKey(pos);
		StaticObject* obj = getObjFromChunks(chunks, index);

		if (obj == air) {
			StaticObject* so = new StaticObject(CubeManager::getSingleton()->getEntity(newType), newType, scale, pos, _simulator, this);
			if (so == nullptr) assert(!"StaticObject is null");
			_staticObjects[index] = so;
			_sg->addEntity(so->_geom, so->_pos, so->_orientation, so->_scale);
			createAirNeighbors(chunks, so);
		}
	}

	// Left
	if (norm == btVector3(-1, 0, 0)) {
		pos = obj->_pos + Ogre::Vector3(-CHUNK_SCALE_FULL, 0, 0);
		key index = getKey(pos);
		StaticObject* obj = getObjFromChunks(chunks, index);

		if (obj == air) {
			StaticObject* so = new StaticObject(CubeManager::getSingleton()->getEntity(newType), newType, scale, pos, _simulator, this);
			if (so == nullptr) assert(!"StaticObject is null");
			_staticObjects[index] = so;
			_sg->addEntity(so->_geom, so->_pos, so->_orientation, so->_scale);
			createAirNeighbors(chunks, so);
		}
	}

	// Right
	if (norm == btVector3(1, 0, 0)) {
		pos = obj->_pos + Ogre::Vector3(CHUNK_SCALE_FULL, 0, 0);
		key index = getKey(pos);
		StaticObject* obj = getObjFromChunks(chunks, index);

		if (obj == air) {
			StaticObject* so = new StaticObject(CubeManager::getSingleton()->getEntity(newType), newType, scale, pos, _simulator, this);
			if (so == nullptr) assert(!"StaticObject is null");
			_staticObjects[index] = so;
			_sg->addEntity(so->_geom, so->_pos, so->_orientation, so->_scale);
			createAirNeighbors(chunks, so);
		}
	}

	// Front
	if (norm == btVector3(0, 0, 1)) {
		pos = obj->_pos + Ogre::Vector3(0, 0, CHUNK_SCALE_FULL);
		key index = getKey(pos);
		StaticObject* obj = getObjFromChunks(chunks, index);

		if (obj == air) {
			StaticObject* so = new StaticObject(CubeManager::getSingleton()->getEntity(newType), newType, scale, pos, _simulator, this);
			if (so == nullptr) assert(!"StaticObject is null");
			_staticObjects[index] = so;
			_sg->addEntity(so->_geom, so->_pos, so->_orientation, so->_scale);
			createAirNeighbors(chunks, so);
		}
	}

	// Back
	if (norm == btVector3(0, 0, -1)) {
		pos = obj->_pos + Ogre::Vector3(0, 0, -CHUNK_SCALE_FULL);
		key index = getKey(pos);
		StaticObject* obj = getObjFromChunks(chunks, index);

		if (obj == air) {
			StaticObject* so = new StaticObject(CubeManager::getSingleton()->getEntity(newType), newType, scale, pos, _simulator, this);
			if (so == nullptr) assert(!"StaticObject is null");
			_staticObjects[index] = so;
			_sg->addEntity(so->_geom, so->_pos, so->_orientation, so->_scale);
			createAirNeighbors(chunks, so);
		}
	}

	if (newType == CubeManager::TORCH) {
		key tkey = "torch_"+getKey(pos);
		if (!lights[tkey]) {
			float range = 5000.0f;
			Ogre::Light* light = _mSceneManager->createLight(tkey);
			light->setDiffuseColour(0.9, 0.4, 0.2);
			light->setType(Ogre::Light::LT_POINT);
			light->setAttenuation(range, 1.0f, .5f/range, 5.0f/(range*range));
			light->setPosition(pos);
			light->setVisible(true);
			lights[tkey] = light;
		}
	}
	_sg->build();
}

void Chunk::createAirNeighbors(const std::vector<Chunk*>& chunks, StaticObject* obj) {
	// top
	Ogre::Vector3 topPos = obj->_pos + Ogre::Vector3(0, CHUNK_SCALE_FULL, 0);
	key topIndex = getKey(topPos);
	StaticObject* topObj = getObjFromChunks(chunks, topIndex);
	if (topObj == nullptr) {
		_staticObjects[topIndex] = air;
	}

	// bottom
	Ogre::Vector3 bottomPos = obj->_pos + Ogre::Vector3(0, -CHUNK_SCALE_FULL, 0);
	key bottomIndex = getKey(bottomPos);
	StaticObject* bottomObj = getObjFromChunks(chunks, bottomIndex);
	if (bottomObj == nullptr) {
		_staticObjects[bottomIndex] = air;
	}

	// left
	Ogre::Vector3 leftPos = obj->_pos + Ogre::Vector3(-CHUNK_SCALE_FULL, 0, 0);
	key leftIndex = getKey(leftPos);
	StaticObject* leftObj = getObjFromChunks(chunks, leftIndex);
	if (leftObj == nullptr) {
		_staticObjects[leftIndex] = air;
	}

	// right
	Ogre::Vector3 rightPos = obj->_pos + Ogre::Vector3(CHUNK_SCALE_FULL, 0, 0);
	key rightIndex = getKey(rightPos);
	StaticObject* rightObj = getObjFromChunks(chunks, rightIndex);
	if (rightObj == nullptr) {
		_staticObjects[rightIndex] = air;
	}

	// back
	Ogre::Vector3 backPos = obj->_pos + Ogre::Vector3(0, 0, -CHUNK_SCALE_FULL);
	key backIndex = getKey(backPos);
	StaticObject* backObj = getObjFromChunks(chunks, backIndex);
	if (backObj == nullptr) {
		_staticObjects[backIndex] = air;
	}

	// front
	Ogre::Vector3 frontPos = obj->_pos + Ogre::Vector3(0, 0, CHUNK_SCALE_FULL);
	key frontIndex = getKey(frontPos);
	StaticObject* frontObj = getObjFromChunks(chunks, frontIndex);
	if (frontObj == nullptr) {
		_staticObjects[frontIndex] = air;
	}
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
// Playing around with probabilities
CubeManager::CubeType Chunk::getGeneratedType(CubeManager::CubeType objType, int height) {
	static int offset = 18 * CHUNK_SCALE_FULL;
	switch (objType) {
		case CubeManager::GRASS :
		case CubeManager::DIRT :
			if (rand()%500 < (height + offset))
				return CubeManager::DIRT;
			// would use dirt but don't know how to use blender - Jeremy :)
			else 
				return CubeManager::ROCK;
			break;
		case CubeManager::SAND :
			if (rand()%500 < (height + offset))
				return CubeManager::SAND;
			else
				return CubeManager::ROCK;
			break;
		case CubeManager::SNOW :
			return CubeManager::DIRT;
			break;
		case CubeManager::ROCK :
			return CubeManager::ROCK;
			break;
		default :
			return objType;
	}
}

bool Chunk::createTree(const Ogre::Vector3& pos, CubeManager::CubeType type) {

	static const int breadth = 5;
	static const int firHeight = 5;
	static const int firBreadth = 7;
	

	if ( type == CubeManager::GRASS && rand()%500 != 5 ) return false;
	if ( type == CubeManager::SNOW && rand()%3000 != 5 ) return false;
	if ( type == CubeManager::SAND && rand()%2000 != 5 ) return false;

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
		case CubeManager::GRASS:
			// Trunk
			for (int i = 1; i <= 3; i++) {
				StaticObject* treeLimb = new StaticObject(CubeManager::getSingleton()->getEntity(CubeManager::WOOD), CubeManager::WOOD, scale, pos + Ogre::Vector3(0, i*CHUNK_SCALE_FULL, 0), _simulator, this);
				key index = getKey(treeLimb->_pos);
				_staticObjects[index] = treeLimb;
				_sg->addEntity(treeLimb->_geom, treeLimb->_pos, treeLimb->_orientation, treeLimb->_scale);
			}

			// Leaves
			for (int i = 0; i < breadth; i++) {
				for (int j = 0; j < breadth; j++) {
					for (int k = 0; k < breadth; k++) {
						if (leaves[i][j][k]) {
							StaticObject* treeLimb = new StaticObject(CubeManager::getSingleton()->getEntity(CubeManager::LEAF), CubeManager::LEAF, scale, pos + Ogre::Vector3((i - 2)*CHUNK_SCALE_FULL, (j + breadth - 1)*CHUNK_SCALE_FULL, (k - 2)*CHUNK_SCALE_FULL), _simulator, this);
							key index = getKey(treeLimb->_pos);
							_staticObjects[index] = treeLimb;
							_sg->addEntity(treeLimb->_geom, treeLimb->_pos, treeLimb->_orientation, treeLimb->_scale);
						}
					}
				}
			}
			return true;
		case CubeManager::SNOW:
			//Fir Tree
			for (int i = 0; i < firBreadth; i++) {
				for (int j = 0; j < firHeight; j++) {
					for (int k = 0; k < firBreadth; k++) {
						if (firTree[i][j][k]) {
							StaticObject* firBlock = new StaticObject(CubeManager::getSingleton()->getEntity(CubeManager::ICE), CubeManager::ICE, scale, pos + Ogre::Vector3((i - 3)*CHUNK_SCALE_FULL, (2*j+3)*CHUNK_SCALE_FULL, (k - 3)*CHUNK_SCALE_FULL), _simulator, this);
							key index = getKey(firBlock->_pos);
							_staticObjects[index] = firBlock;
							_sg->addEntity(firBlock->_geom, firBlock->_pos, firBlock->_orientation, firBlock->_scale);
						}
					}
				}
			}
			//Trunk
			for (int i = 0 ; i <= firHeight*2 ; i++ ) {
				StaticObject* firBlock = new StaticObject(CubeManager::getSingleton()->getEntity(CubeManager::WOOD), CubeManager::WOOD, scale, pos + Ogre::Vector3(0, i*CHUNK_SCALE_FULL, 0), _simulator, this);
				key index = getKey(firBlock->_pos);
				_staticObjects[index] = firBlock;
				_sg->addEntity(firBlock->_geom, firBlock->_pos, firBlock->_orientation, firBlock->_scale);
			}
			return true;
		case CubeManager::SAND:
			//Cactus
			for (int i = 1; i <= 10; i++) {
				StaticObject* stalk = new StaticObject(CubeManager::getSingleton()->getEntity(CubeManager::CACTUS), CubeManager::CACTUS, scale, pos + Ogre::Vector3(0, i*CHUNK_SCALE_FULL, 0), _simulator, this);
				key index = getKey(stalk->_pos);
				_staticObjects[index] = stalk;
				_sg->addEntity(stalk->_geom, stalk->_pos, stalk->_orientation, stalk->_scale);
			}
			//Arms
			if ( rand()%2 ) {
				StaticObject* armConnector1 = new StaticObject(CubeManager::getSingleton()->getEntity(CubeManager::CACTUS), CubeManager::CACTUS, scale, pos + Ogre::Vector3(CHUNK_SCALE_FULL, 4*CHUNK_SCALE_FULL, 0), _simulator, this);
				key index = getKey(armConnector1->_pos);
				_staticObjects[index] = armConnector1;
				_sg->addEntity(armConnector1->_geom, armConnector1->_pos, armConnector1->_orientation, armConnector1->_scale);
			
				StaticObject* armConnector2 = new StaticObject(CubeManager::getSingleton()->getEntity(CubeManager::CACTUS), CubeManager::CACTUS, scale, pos + Ogre::Vector3(-CHUNK_SCALE_FULL, 4*CHUNK_SCALE_FULL, 0), _simulator, this);
				key index2 = getKey(armConnector2->_pos);
				_staticObjects[index2] = armConnector2;
				_sg->addEntity(armConnector2->_geom, armConnector2->_pos, armConnector2->_orientation, armConnector2->_scale);
				
				for (int i = 4; i <= 8; i++) {
					StaticObject* armBlock = new StaticObject(CubeManager::getSingleton()->getEntity(CubeManager::CACTUS), CubeManager::CACTUS, scale, pos + Ogre::Vector3(2*CHUNK_SCALE_FULL, i*CHUNK_SCALE_FULL, 0), _simulator, this);
					key index = getKey(armBlock->_pos);
					_staticObjects[index] = armBlock;
					_sg->addEntity(armBlock->_geom, armBlock->_pos, armBlock->_orientation, armBlock->_scale);
				}
				for (int i = 4; i <= 9; i++) {
					StaticObject* armBlock = new StaticObject(CubeManager::getSingleton()->getEntity(CubeManager::CACTUS), CubeManager::CACTUS, scale, pos + Ogre::Vector3(-2*CHUNK_SCALE_FULL, i*CHUNK_SCALE_FULL, 0), _simulator, this);
					key index = getKey(armBlock->_pos);
					_staticObjects[index] = armBlock;
					_sg->addEntity(armBlock->_geom, armBlock->_pos, armBlock->_orientation, armBlock->_scale);
				}
			}
			else {
				StaticObject* armConnector1 = new StaticObject(CubeManager::getSingleton()->getEntity(CubeManager::CACTUS), CubeManager::CACTUS, scale, pos + Ogre::Vector3(0, 4*CHUNK_SCALE_FULL, CHUNK_SCALE_FULL), _simulator, this);
				key index = getKey(armConnector1->_pos);
				_staticObjects[index] = armConnector1;
				_sg->addEntity(armConnector1->_geom, armConnector1->_pos, armConnector1->_orientation, armConnector1->_scale);
			
				StaticObject* armConnector2 = new StaticObject(CubeManager::getSingleton()->getEntity(CubeManager::CACTUS), CubeManager::CACTUS, scale, pos + Ogre::Vector3(0, 4*CHUNK_SCALE_FULL, -CHUNK_SCALE_FULL), _simulator, this);
				key index2 = getKey(armConnector2->_pos);
				_staticObjects[index2] = armConnector2;
				_sg->addEntity(armConnector2->_geom, armConnector2->_pos, armConnector2->_orientation, armConnector2->_scale);
				
				for (int i = 4; i <= 8; i++) {
					StaticObject* armBlock = new StaticObject(CubeManager::getSingleton()->getEntity(CubeManager::CACTUS), CubeManager::CACTUS, scale, pos + Ogre::Vector3(0, i*CHUNK_SCALE_FULL, 2*CHUNK_SCALE_FULL), _simulator, this);
					key index = getKey(armBlock->_pos);
					_staticObjects[index] = armBlock;
					_sg->addEntity(armBlock->_geom, armBlock->_pos, armBlock->_orientation, armBlock->_scale);
				}
				for (int i = 4; i <= 9; i++) {
					StaticObject* armBlock = new StaticObject(CubeManager::getSingleton()->getEntity(CubeManager::CACTUS), CubeManager::CACTUS, scale, pos + Ogre::Vector3(0, i*CHUNK_SCALE_FULL, -2*CHUNK_SCALE_FULL), _simulator, this);
					key index = getKey(armBlock->_pos);
					_staticObjects[index] = armBlock;
					_sg->addEntity(armBlock->_geom, armBlock->_pos, armBlock->_orientation, armBlock->_scale);
				}
			}
			return true;
		default: return false;
	}
}

bool Chunk::createCloud(const Ogre::Vector3& pos) {
	if(rand() % 400 != 10) return false;

	int imax = rand() % 25 + 5;
	int jmax = rand() % 25 + 5;

	Ogre::Vector3 scale(imax*CHUNK_SCALE, CHUNK_SCALE, jmax*CHUNK_SCALE);

	Ogre::Vector3 cloudPos(pos.x, 60*CHUNK_SCALE_FULL, pos.z);
	StaticObject* cloud = new StaticObject(CubeManager::getSingleton()->getEntity(CubeManager::CLOUD), CubeManager::CLOUD, scale, cloudPos, _simulator, this);
	key index = getKey(cloud->_pos);
	_staticObjects[index] = cloud;
	_sg->addEntity(cloud->_geom, cloud->_pos, cloud->_orientation, cloud->_scale);

	return true;
}

Chunk::key Chunk::getKey(int x, int y, int z) {
	char buf[64];
	sprintf(buf, "%d_%d_%d", x, y, z);
	return std::string(buf);
}

Chunk::key Chunk::getKey(const Ogre::Vector3& pos) {
	return getKey((int)pos.x, (int)pos.y, (int)pos.z);
}

BlockInfo Chunk::getBlockInfo(key thekey, CubeManager::CubeType type) {
	int x, y, z;
	std::string delimiter = "_";
	size_t last = 0; 
	size_t next = 0; 

	next = thekey.find(delimiter, last);
	x = std::stoi(thekey.substr(last, next - last));
	last = next + 1;

	next = thekey.find(delimiter, last);
	y = std::stoi(thekey.substr(last, next - last));
	last = next + 1;

	next = thekey.find(delimiter, last);
	z = std::stoi(thekey.substr(last));

	return BlockInfo(x, y, z, type);
}

void Chunk::rebuildFromSave(const std::vector<BlockInfo>& blocks) {
	for (auto& var : blocks) {
		StaticObject* so;
		Ogre::Vector3 pos(var.x, var.y, var.z);
		Ogre::Vector3 scale(CHUNK_SCALE, CHUNK_SCALE, CHUNK_SCALE);
		key index = getKey(pos);

		if (var.type == CubeManager::AIR) {
			_staticObjects[index] = air;
		}
		else if (var.type == CubeManager::TORCH) {
			scale = Ogre::Vector3(CHUNK_SCALE / 5, 4 * CHUNK_SCALE / 5, CHUNK_SCALE / 5);

			key tkey = "torch_" + getKey(pos);
			if (!lights[tkey]) {
				float range = 5000.0f;
				Ogre::Light* light = _mSceneManager->createLight(tkey);
				light->setDiffuseColour(0.9, 0.4, 0.2);
				light->setType(Ogre::Light::LT_POINT);
				light->setAttenuation(range, 1.0f, .5f / range, 5.0f / (range*range));
				light->setPosition(pos);
				light->setVisible(true);
				lights[tkey] = light;
			}

			so = new StaticObject(CubeManager::getSingleton()->getEntity(var.type), var.type, scale, pos, _simulator, this);
			_staticObjects[index] = so;
			_sg->addEntity(so->_geom, so->_pos, so->_orientation, so->_scale);
		}
		else {
			so = new StaticObject(CubeManager::getSingleton()->getEntity(var.type), var.type, scale, pos, _simulator, this);
			_staticObjects[index] = so;
			_sg->addEntity(so->_geom, so->_pos, so->_orientation, so->_scale);
		}	
	}
	_sg->build();
}