#include "Chunk.h"

Chunk::Chunk(const std::string& name, int xStart, int yStart, Ogre::SceneManager* mSceneManager, BiomeManager* biomeMgr, Perlin* perlin, Simulator* sim) : _name(name), _xStart(xStart), _yStart(yStart), _mSceneManager(mSceneManager), _simulator(sim) {

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
			StaticObject* so;
			Biome::BiomeType tempType;

			if(y >= 15 && curBiome != nullptr ) {
				if ( curBiome->getType() != Biome::SAND )
					so = new StaticObject(curBiome->getCubeEntity(i, j, y, &tempType), tempType, scale, pos, sim);
			}
			if ( curBiome != nullptr )
				so = new StaticObject(curBiome->getCubeEntity(i, j, y, &tempType), tempType, scale, pos, sim);
			else
				so = new StaticObject(biomeMgr->getTerrain(Biome::GRASS), Biome::GRASS, scale, pos, sim);

			createTree(pos, so->_cubeType);

			_staticObjects.push_back(so);

			_sg->addEntity(so->_geom, so->_pos, so->_orientation, so->_scale);
		}
	}
	_sg->build();
}

Chunk::~Chunk() {
	for(auto& var : _staticObjects) {
		delete var;
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
		var->addToSimulator();
	}
}

void Chunk::createTree(const Ogre::Vector3& pos, Biome::BiomeType type) {

	static const int breadth = 5;

	if ( type == Biome::GRASS && rand()%200 != 5 ) return;
	if ( type == Biome::SAND && rand()%2000 != 5 ) return;

	// Describes the shape of the leaves, think of each 5x5 grid as another layer
	bool leaves[breadth][breadth][breadth] = {	{ { 0, 0, 0, 0, 0 }, { 0, 1, 1, 1, 0 }, { 0, 1, 1, 1, 0 }, { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } },
												{ { 0, 1, 1, 1, 0 }, { 1, 1, 1, 1, 1 }, { 1, 1, 1, 1, 1 }, { 1, 1, 1, 1, 1 }, { 0, 1, 1, 1, 0 } },
												{ { 0, 1, 1, 1, 0 }, { 1, 1, 1, 1, 1 }, { 1, 1, 1, 1, 1 }, { 1, 1, 1, 1, 1 }, { 0, 1, 1, 1, 0 } },
												{ { 0, 1, 1, 1, 0 }, { 1, 1, 1, 1, 1 }, { 1, 1, 1, 1, 1 }, { 1, 1, 1, 1, 1 }, { 0, 1, 1, 1, 0 } },
												{ { 0, 0, 0, 0, 0 }, { 0, 1, 1, 1, 0 }, { 0, 1, 1, 1, 0 }, { 0, 1, 1, 1, 0 }, { 0, 0, 0, 0, 0 } } };
	
	Ogre::Vector3 scale(CHUNK_SCALE, CHUNK_SCALE, CHUNK_SCALE);

	switch ( type ) {
		case Biome::GRASS:
			// Trunk
			for (int i = 1; i <= 3; i++) {
				StaticObject* treeLimb = new StaticObject(_biome->getEntity(Biome::WOOD), Biome::WOOD, scale, pos + Ogre::Vector3(0, i*CHUNK_SCALE_FULL, 0), _simulator);
				_staticObjects.push_back(treeLimb);
				_sg->addEntity(treeLimb->_geom, treeLimb->_pos, treeLimb->_orientation, treeLimb->_scale);
			}

			// Leaves
			for (int i = 0; i < breadth; i++) {
				for (int j = 0; j < breadth; j++) {
					for (int k = 0; k < breadth; k++) {
						if (leaves[i][j][k]) {
							StaticObject* treeLimb = new StaticObject(_biome->getEntity(Biome::LEAF), Biome::LEAF, scale, pos + Ogre::Vector3((i - 2)*CHUNK_SCALE_FULL, (j + breadth - 1)*CHUNK_SCALE_FULL, (k - 2)*CHUNK_SCALE_FULL), _simulator);
							_staticObjects.push_back(treeLimb);
							_sg->addEntity(treeLimb->_geom, treeLimb->_pos, treeLimb->_orientation, treeLimb->_scale);
						}
					}
				}
			}
			break;
		case Biome::SNOW:
			break;
		case Biome::SAND:
			//Cactus
			for (int i = 1; i <= 10; i++) {
				StaticObject* stalk = new StaticObject(_biome->getEntity(Biome::CACTUS), Biome::CACTUS, scale, pos + Ogre::Vector3(0, i*CHUNK_SCALE_FULL, 0), _simulator);
				_staticObjects.push_back(stalk);
				_sg->addEntity(stalk->_geom, stalk->_pos, stalk->_orientation, stalk->_scale);
			}
			//Arms
			if ( rand()%2 ) {
				StaticObject* armConnector1 = new StaticObject(_biome->getEntity(Biome::CACTUS), Biome::CACTUS, scale, pos + Ogre::Vector3(CHUNK_SCALE_FULL, 4*CHUNK_SCALE_FULL, 0), _simulator);
				_staticObjects.push_back(armConnector1);
				_sg->addEntity(armConnector1->_geom, armConnector1->_pos, armConnector1->_orientation, armConnector1->_scale);
			
				StaticObject* armConnector2 = new StaticObject(_biome->getEntity(Biome::CACTUS), Biome::CACTUS, scale, pos + Ogre::Vector3(-CHUNK_SCALE_FULL, 4*CHUNK_SCALE_FULL, 0), _simulator);
				_staticObjects.push_back(armConnector2);
				_sg->addEntity(armConnector2->_geom, armConnector2->_pos, armConnector2->_orientation, armConnector2->_scale);
				
				for (int i = 4; i <= 8; i++) {
					StaticObject* armBlock = new StaticObject(_biome->getEntity(Biome::CACTUS), Biome::CACTUS, scale, pos + Ogre::Vector3(2*CHUNK_SCALE_FULL, i*CHUNK_SCALE_FULL, 0), _simulator);
					_staticObjects.push_back(armBlock);
					_sg->addEntity(armBlock->_geom, armBlock->_pos, armBlock->_orientation, armBlock->_scale);
				}
				for (int i = 4; i <= 9; i++) {
					StaticObject* armBlock = new StaticObject(_biome->getEntity(Biome::CACTUS), Biome::CACTUS, scale, pos + Ogre::Vector3(-2*CHUNK_SCALE_FULL, i*CHUNK_SCALE_FULL, 0), _simulator);
					_staticObjects.push_back(armBlock);
					_sg->addEntity(armBlock->_geom, armBlock->_pos, armBlock->_orientation, armBlock->_scale);
				}
			}
			else {
				StaticObject* armConnector1 = new StaticObject(_biome->getEntity(Biome::CACTUS), Biome::CACTUS, scale, pos + Ogre::Vector3(0, 4*CHUNK_SCALE_FULL, CHUNK_SCALE_FULL), _simulator);
				_staticObjects.push_back(armConnector1);
				_sg->addEntity(armConnector1->_geom, armConnector1->_pos, armConnector1->_orientation, armConnector1->_scale);
			
				StaticObject* armConnector2 = new StaticObject(_biome->getEntity(Biome::CACTUS), Biome::CACTUS, scale, pos + Ogre::Vector3(0, 4*CHUNK_SCALE_FULL, -CHUNK_SCALE_FULL), _simulator);
				_staticObjects.push_back(armConnector2);
				_sg->addEntity(armConnector2->_geom, armConnector2->_pos, armConnector2->_orientation, armConnector2->_scale);
				
				for (int i = 4; i <= 8; i++) {
					StaticObject* armBlock = new StaticObject(_biome->getEntity(Biome::CACTUS), Biome::CACTUS, scale, pos + Ogre::Vector3(0, i*CHUNK_SCALE_FULL, 2*CHUNK_SCALE_FULL), _simulator);
					_staticObjects.push_back(armBlock);
					_sg->addEntity(armBlock->_geom, armBlock->_pos, armBlock->_orientation, armBlock->_scale);
				}
				for (int i = 4; i <= 9; i++) {
					StaticObject* armBlock = new StaticObject(_biome->getEntity(Biome::CACTUS), Biome::CACTUS, scale, pos + Ogre::Vector3(0, i*CHUNK_SCALE_FULL, -2*CHUNK_SCALE_FULL), _simulator);
					_staticObjects.push_back(armBlock);
					_sg->addEntity(armBlock->_geom, armBlock->_pos, armBlock->_orientation, armBlock->_scale);
				}
			}
			break;
	}
}