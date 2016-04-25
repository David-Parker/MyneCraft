#include "Application.h"
#include "CoreConfig.h"
#include "MultiPlatformHelper.h"
#include "SceneHelper.h"

#include <OISMouse.h>
#include <OISKeyboard.h>
#include <OISInputManager.h>

#include <string>
#include "btBulletDynamicsCommon.h"
#include "OISManager.h"
#include "OgreMotionState.h"
#include "Simulator.h"
#include "GameObject.h"
#include <cstring>

using namespace Ogre;

Application::Application()
{
}

Application::~Application()
{
}

void Application::init()
{
	// This is really just a debugging try-catch block for catching and printing exceptions
	try {

		loadSeed();

		t1 = new Timer();

		_simulator = new Simulator();

		setupWindowRendererSystem();

		setupOIS();

		netManager = new NetManager();
		setupCEGUI();

		setupGM();

		setupCameras();

		ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

		setupLighting();

		sg = mSceneManager->createStaticGeometry("CubeArea");

		createObjects();

	}
	catch (Exception e) {
		std::cout << "Exception Caught: " << e.what() << std::endl;
	}
}

void Application::setupWorld() {
	std::fstream saveFile;

	saveFile.open("save.txt", std::ios::in);

	std::string line;
	getline(saveFile, line);

	if (line != magicHeader) {
		// New save
		saveFile.close();
		saveFile.open("save.txt", std::ios::out);
		saveFile << magicHeader << std::endl;
		saveFile << seed << std::endl;
		saveFile.close();
	}
	else {
		getline(saveFile, line);
		seed = std::stoi(line);

		while (getline(saveFile, line)) {
			if (line == "EOF") break;

			// Generate this new chunk
			if (line == "CHUNK") {
				getline(saveFile, line);
				int x = std::stoi(line);
				getline(saveFile, line);
				int z = std::stoi(line);
				std::pair<int, int> index(x, z);

				Chunk* chunk = new Chunk(x, z, mSceneManager, biomeManager, perlin, _simulator, false);
				modifiedChunks[index] = chunk;
				std::vector<BlockInfo> blocks;

				while (getline(saveFile, line)) {
					if (line == "ENDCHUNK") break;

					int bx = std::stoi(line);
					getline(saveFile, line);
					int by = std::stoi(line);
					getline(saveFile, line);
					int bz = std::stoi(line);
					getline(saveFile, line);
					Biome::BiomeType type = (Biome::BiomeType) std::stoi(line);

					BlockInfo b(bx, by, bz, type);
					blocks.push_back(b);
				}

				chunk->rebuildFromSave(blocks);
			}
		}
	}

	saveFile.close();
}

void Application::saveWorld() {
	std::fstream saveFile;
	saveFile.open("save.txt", std::ios::out);

	// Write out header
	saveFile << magicHeader << std::endl;
	saveFile << seed << std::endl;

	for (auto& var : modifiedChunks) {
		if (var.second == nullptr) continue;
		saveFile << "CHUNK" << std::endl;
		saveFile << var.second->_xStart << std::endl;
		saveFile << var.second->_yStart << std::endl;
		for (auto& block : var.second->_staticObjects) {
			if (block.second == nullptr) continue;
			BlockInfo bi = var.second->getBlockInfo(block.first, block.second->_cubeType);
			saveFile << bi.x << std::endl;
			saveFile << bi.y << std::endl;
			saveFile << bi.z << std::endl;
			saveFile << bi.type << std::endl;
		}
		saveFile << "ENDCHUNK" << std::endl;
	}

	saveFile << "EOF";
	saveFile.close();
}

/* 
* Update Methods 
*/
bool Application::frameRenderingQueued(const FrameEvent &evt) {

	static float dTime = t1->getMilliseconds();
#ifdef _DEBUG
	CEGUI::System::getSingleton().injectTimePulse(evt.timeSinceLastFrame);
#endif

	if (!mRunning)
	{
		saveWorld();
		return false;
	}
	try {
		_oisManager->capture();
	}
	catch (Exception e) {

	}

	/* Begun split this into three areas, Setup & GUI, server, client */
	/* This line has been left here for the sake of closing the window. Maybe we'll move it later. */
	if (mRenderWindow->isClosed())
	{
		return false;
	}

	// Code per frame in fixed FPS
	float temp = t1->getMilliseconds();
	if ((temp - dTime) >= (1.0 / fps)*1000.0) {
		// This update method gets called once per frame and is called whether or not we are a server or a client
		update(evt);
		dTime = temp;
	}

	_simulator->stepSimulation(evt.timeSinceLastFrame, 7, 1.0 / fps);

	player->constrainSpeed();

	return true;
}

// Called once per predefined frame
bool Application::update(const FrameEvent &evt) {

	moveDayTime(evt.timeSinceLastFrame);

	// std::cout << "FPS: " << mRenderWindow->getLastFPS() << std::endl;

	OIS::KeyCode lastKey = _oisManager->getKeyPressed();
	Ogre::Camera* camMan = mSceneManager->getCamera("Camera Man");

	if (lastKey == OIS::KC_M) {
		gameManager->mute();
	}
	else if ( lastKey >= OIS::KC_1 && lastKey <= OIS::KC_9 ) {
		int index = lastKey - 2;
		player->setWeapon(index);
	}
	else if(lastKey == OIS::KC_ESCAPE) {
		// close window when ESC is pressed
		mRunning = false;
	}

	if (int delta = _oisManager->getMouseWheel()) {
		int index = player->getWeapon() + delta;
		index = index % (Player::NUM_WEP+1);
		if (index < 0)
			index += Player::NUM_WEP+1;

		player->setWeapon(index);
		_oisManager->resetWheel();
	}

	Ogre::Vector3 pos = player->_body->getNode()->getPosition();
	static int range = CHUNK_SIZE * 1;
	static Chunk* currentChunk = nullptr;

	try {

		float fx = (pos.x / CHUNK_SCALE_FULL);
		float fz = (pos.z / CHUNK_SCALE_FULL);
		int numChunks = (fieldOfView*1.1 / (CHUNK_SCALE_FULL*CHUNK_SIZE));

		int currX = ((int)fx - ((int)fx % CHUNK_SIZE));
		int currZ = ((int)fz - ((int)fz % CHUNK_SIZE));

		std::unordered_map<std::pair<int,int>, Chunk*> chunks;

		// Check for new chunks in FOV proximity to create
		for(int i = -numChunks; i <= numChunks; i++) {
			for(int j = -numChunks; j <= numChunks; j++) {

				int x = currX;
				int z = currZ;

				x += i*CHUNK_SIZE;
		 		z += j*CHUNK_SIZE;

		 		std::pair<int, int> name(x ,z);

				// Object persists
				if(prevChunks[name]) {
					chunks[name] = prevChunks[name];
				}
				else if(!chunks[name]) {
					if (modifiedChunks[name]) {
						chunks[name] = modifiedChunks[name];
					}
					else {
						chunks[name] = new Chunk(x, z, mSceneManager, biomeManager, perlin, _simulator, true);
					}
				}
			}
		}

		for(auto& var : prevChunks) {
			if(!chunks[var.first] && !modifiedChunks[var.first]) {
				delete var.second;
				var.second = nullptr;
			}
		}

		prevChunks.swap(chunks);

		if (pos.x < 0) {
			currX -= CHUNK_SIZE;
		}
		if (pos.z < 0) {
			currZ -= CHUNK_SIZE;
		}

		// Add only the current chunk's static objects to the bullet simulation
		Chunk* chunk = getChunk(chunks, currX, currZ);
		if (chunk != currentChunk) {
			recomputeColliders(chunks, currX, currZ);
			currentChunk = chunk;
		}	

		Ogre::Vector3 norm = playerCam->getDirection().normalisedCopy();
		Ogre::Vector3 camPos = playerCam->getPosition();
		btVector3 start(camPos.x, camPos.y, camPos.z);
		btVector3 end = start + btVector3(norm.x, norm.y, norm.z) * 1000;
		btVector3 hitNormal;
		StaticObject* hitObj = nullptr;

		if (_simulator->rayHit(start, end, hitObj, hitNormal)) {
			if (hitObj != nullptr) {
				highlight->getNode()->setVisible(true);
				highlight->setPosition(hitObj->_pos);

				if (_oisManager->mouseClicked) {
					// Return value tells us if we need to recompute the colliders or not
					if (player->clickAction(hitObj, hitNormal, chunks, modifiedChunks)) {
						recomputeColliders(chunks, currX, currZ);
					}

					_oisManager->mouseClicked = false;
				}
			}
		}
		else {
			// No ray hit
			highlight->getNode()->setVisible(false);
		}
	}

	catch (Exception e) {
		std::cout << e.what() << std::endl;
		std::cout << currentChunk->getName() << std::endl;
		exit(0);
	}

	player->update(_oisManager);

	return true;
}

bool Application::handleGUI(const FrameEvent &evt) {

	if(!begin) {
		_oisManager->capture();

		OIS::KeyCode lastKey = _oisManager->lastKeyPressed();

		if(lastKey == OIS::KC_ESCAPE) {
			// close window when ESC is pressed
			mRunning = false;
		}
		return true;
	}
	else {
		return false;
	}
}

bool Application::updateServer(const FrameEvent &evt) {
	
	static float previousTime = t1->getMilliseconds();

	if ( netManager->pollForActivity(1) ) {
		previousTime = t1->getMilliseconds();
		// Only accept one connection at a time.
		netManager->denyConnections();
	}
	else {
		float dt = t1->getMilliseconds() - previousTime;
		if(dt >= 2000 && netManager->getClients() > 0){
			return false;
		}
	}
	return true;
}

/* All logic is now in update client. Update server will be implemented soon. */
bool Application::updateClient(const FrameEvent &evt) {

	static float previousTime = t1->getMilliseconds();
	
	if ( netManager->pollForActivity(1)) {

	}

	std::string t = "";

	if(t.length() - 1 > NetManager::MESSAGE_LENGTH) {
		std::cout << "Message was too large." << std::endl;
		return error();
	}

    return true;
}
/* 
* End Update Methods 
*/


/* 
* Create Object Methods 
*/
void Application::createRootEntity(std::string name, std::string mesh, int x, int y, int z) {

	Ogre::Entity* ogreEntity = mSceneManager->createEntity(name, mesh);
	ogreEntity->setCastShadows(true);
	Ogre::SceneNode* ogreNode = mSceneManager->getRootSceneNode()->createChildSceneNode(name);
	ogreNode->attachObject(ogreEntity);
	ogreNode->setPosition(x, y, z);
	ogreNode->setScale(50, 50, 50);
}

Ogre::Entity* Application::createRootEntity(std::string mesh, int x, int y, int z) {

	Ogre::Entity* ogreEntity = mSceneManager->createEntity(mesh);
	ogreEntity->setCastShadows(true);
	Ogre::SceneNode* ogreNode = mSceneManager->getRootSceneNode()->createChildSceneNode();
	ogreNode->attachObject(ogreEntity);
	ogreNode->setPosition(x, y, z);

	return ogreEntity;
}

void Application::createChildEntity(std::string name, std::string mesh, Ogre::SceneNode* sceneNode, int x, int y, int z) {

	Ogre::Entity* ogreEntity = mSceneManager->createEntity(name, mesh);
	ogreEntity->setCastShadows(true);
	Ogre::SceneNode* ogreNode = sceneNode->createChildSceneNode(name);
	ogreNode->attachObject(ogreEntity);
	ogreNode->setPosition(x, y, z);
}

PlayerObject* Application::createPlayerObject(Ogre::String nme, GameObject::objectType tp, Ogre::String meshName, int x, int y, int z, Ogre::Vector3 scale, Ogre::Degree pitch, Ogre::Degree yaw, Ogre::Degree roll, Ogre::SceneManager* scnMgr, GameManager* ssm, Ogre::Real mss, Ogre::Real rest, Ogre::Real frict, bool kinematic, Simulator* mySim) {
	createRootEntity(nme, meshName, x, y, z);
	Ogre::SceneNode* sn = mSceneManager->getSceneNode(nme);
	Ogre::Entity* ent = SceneHelper::getEntity(mSceneManager, nme, 0);
	const btTransform pos;
	OgreMotionState* ms = new OgreMotionState(pos, sn);
	sn->setScale(scale.x, scale.y, scale.z);

	sn->pitch(pitch);
	sn->yaw(yaw);
	sn->roll(roll);

	PlayerObject* obj = new PlayerObject(nme, tp, mSceneManager, ssm, sn, ent, ms, mySim, mss, rest, frict, scale, kinematic);
	obj->addToSimulator();

	return obj;
}

Cube* Application::createCube(Ogre::String nme, GameObject::objectType tp, Ogre::String meshName, int x, int y, int z, Ogre::Vector3 scale, Ogre::Degree pitch, Ogre::Degree yaw, Ogre::Degree roll, Ogre::SceneManager* scnMgr, GameManager* ssm, Ogre::Real mss, Ogre::Real rest, Ogre::Real frict, bool kinematic, Simulator* mySim) {
	createRootEntity(nme, meshName, x*scale.x*2, y*scale.y*2, z*scale.z*2);
	Ogre::SceneNode* sn = mSceneManager->getSceneNode(nme);
	Ogre::Entity* ent = SceneHelper::getEntity(mSceneManager, nme, 0);
	ent->setMaterialName("highlight");
	ent->setCastShadows(false);
	const btTransform pos;
	OgreMotionState* ms = new OgreMotionState(pos, sn);
	sn->setScale(scale.x, scale.y, scale.z);

	Cube* obj = new Cube(nme, tp, mSceneManager, ssm, sn, ent, ms, mySim, mss, rest, frict, scale, kinematic);

	return obj;
}

/*
* End Create Object Methods 
*/


/* 
* Initialization Methods
*/
void Application::setupWindowRendererSystem(void) {

	mResourcesCfg = "resources.cfg";
	mPluginsCfg = "plugins.cfg";

	NameValuePairList params;
	// Initialization
	mRoot = new Root(mPluginsCfg);

	setupResources();

	// load plugins
#ifdef _WIN32
	#ifdef _DEBUG
		mRoot->loadPlugin("RenderSystem_GL_d");
		mRoot->loadPlugin("Plugin_ParticleFX_d");
	#else
		mRoot->loadPlugin("RenderSystem_GL");
		mRoot->loadPlugin("Plugin_ParticleFX");
	#endif

#endif
#ifdef __linux__
	mRoot->loadPlugin("/lusr/opt/ogre-1.9/lib/OGRE/RenderSystem_GL");
#endif

	// Select render system
	const RenderSystemList &renderers = mRoot->getAvailableRenderers();
	RenderSystem * renderSystem = nullptr;
	LogManager::getSingletonPtr()->logMessage("Getting available renderers");
	for (auto renderer = renderers.begin(); renderer != renderers.end(); renderer++)
	{
		String name = (*renderer)->getName();
		LogManager::getSingletonPtr()->logMessage(name);
		renderSystem = *renderer;
	}
	if (renderSystem)
	{
		LogManager::getSingletonPtr()->logMessage("Using renderer " + renderSystem->getName());
		mRoot->setRenderSystem(renderSystem);
	}
	else
	{
		LogManager::getSingletonPtr()->logMessage(LML_CRITICAL, "Initializing render system failed. No renderers available.");
	}

	// Initialize with render system, no new window (yet)
	mRoot->initialise(false);

	// Create scene manager, render window, and camera
	mSceneManager = mRoot->createSceneManager(ST_GENERIC);
	mRenderWindow = mRoot->createRenderWindow(PROJECT_NAME, width = 1200, height = 900, false, &params);
}

void Application::setupResources(void) {

	// Load resource paths from config file
    Ogre::ConfigFile cf;
    cf.load(mResourcesCfg);

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;

            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
        }
    }
}


void Application::setupOIS(void) {

    _oisManager = OISManager::getSingletonPtr();
    _oisManager->initialise( mRenderWindow );
    _oisManager->addKeyListener( (OIS::KeyListener*)_oisManager, "keyboardListener" );
    _oisManager->addMouseListener( (OIS::MouseListener*)_oisManager, "mouseListener" );
}

void Application::setupCEGUI(void) {
#ifdef _DEBUG
	mRenderer = &CEGUI::OgreRenderer::bootstrapSystem(*mRenderWindow);
	CEGUI::ImageManager::setImagesetDefaultResourceGroup("Imagesets");
	CEGUI::Font::setDefaultResourceGroup("Fonts");
	CEGUI::Scheme::setDefaultResourceGroup("Schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
	CEGUI::WindowManager::setDefaultResourceGroup("Layouts");

	CEGUI::SchemeManager::getSingleton().createFromFile("AlfiskoSkin.scheme");
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("AlfiskoSkin/MouseArrow");

	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window *sheet = wmgr.createWindow("DefaultWindow", "_MasterRoot");
	CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(sheet);

	CEGUI::Window* quitButton = wmgr.createWindow("AlfiskoSkin/Button", "QuitButton");
	quitButton->setArea(CEGUI::URect(CEGUI::UVector2(CEGUI::UDim(0.0f, 0), CEGUI::UDim(0.0f, 0)),
		CEGUI::UVector2(CEGUI::UDim(0.1f, 0), CEGUI::UDim(0.05f, 0))));
	quitButton->setText("Quit");

	sheet->addChild(quitButton);

	quitButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Application::Quit, this));
#endif

}

void Application::setupCameras(void) {

	playerCam = mSceneManager->createCamera("Camera Man");

	playerCam->setAutoAspectRatio(true);
	playerCam->setPosition(0,300,0);
	playerCam->lookAt(0,120,1800);
	playerCam->setFarClipDistance(fieldOfView);
	playerCam->setNearClipDistance(8);

	// Add viewport and cameras
	mRenderWindow->addViewport(playerCam);

	cameras = std::vector<Ogre::Camera*>();
	cameras.push_back(playerCam);

	_oisManager->setupCameraMan(playerCam);
	//std::cout << camMan->getFarClipDistance() << std::endl;
}

/* Setup GameManager */
void Application::setupGM(void) {

	mRoot->addFrameListener(this);
	WindowEventUtilities::addWindowEventListener(mRenderWindow, this);
	mRenderWindow->addListener(this);

	gameManager = new GameManager();
	gameManager->startMusic();
}

void Application::setupLighting(void) {

	mSceneManager->setAmbientLight(Ogre::ColourValue(0.2, 0.2, 0.2));

	sun = mSceneManager->createLight("Sun");

	sun->setType(Ogre::Light::LightTypes::LT_DIRECTIONAL);
	sun->setCastShadows(false);
	sun->setDiffuseColour(Ogre::ColourValue(.2, .2, .2));
	sun->setSpecularColour(Ogre::ColourValue(.4, .4, .4));

	sun->setDirection(Ogre::Vector3(0, -1, .5));

}

void Application::createObjects(void) {
	mSceneManager->setSkyDome(true, "day-night", 5, 8);

	int xmax = 128;
	int ymax = xmax;
	int rndmax = 8;
	float density = 1.8f; // 1 is very steep, 10 is pretty flat.

	perlin = new Perlin(xmax, ymax, rndmax, density);

	biomeManager = new BiomeManager(mSceneManager);

	GameObject* playerObj = createPlayerObject("Player", GameObject::CUBE_OBJECT, "sphere.mesh", 0, 2500, 0, Ogre::Vector3(0.1, 0.1, 0.1), Ogre::Degree(0), Ogre::Degree(0), Ogre::Degree(0), mSceneManager, gameManager, 1.0f, 0.0f, 0.0f, false, _simulator);
	player = new Player(playerCam, playerObj, mSceneManager);
	highlight = createCube("highlight", GameObject::CUBE_OBJECT, "cube.mesh", 0, 0, 0, Ogre::Vector3(1.01, 1.01, 1.01), Ogre::Degree(0), Ogre::Degree(0), Ogre::Degree(0), mSceneManager, gameManager, 0.0f, 0.0f, 0.0f, true, _simulator);

	setupWorld();
}
/* 
* End Initialization Methods
*/


/* 
*CEGUI Button Callbacks 
*/


bool Application::Quit(const CEGUI::EventArgs& e) {

	mRunning = false;
    return true;
}

bool Application::setupNetwork(bool isServer) {

	netManager = new NetManager();

	if(!netManager->initNetManager()) {
		std::cout << "Failed to init the server!" << std::endl;
		return false;
	}
	else {
		// Opens a connection on port 51215
		netManager->addNetworkInfo(PROTOCOL_UDP, isServer ? NULL : "localghost :P", 51215);
	}
	if (isServer) {
		if(!netManager->startServer()) {
			std::cout << "Failed to start the server!" << std::endl;
			return false;
		}
		return true;
	}
	else {
		if(!netManager->startClient()) {
			std::cout << "Failed to start the client!" << std::endl;
			return false;
		}
		return true;
	}

}

bool Application::error() {

	mRunning = false;
	return false;
}

std::unordered_map<std::string, char*> Application::dataParser(char* buf) {

	if(buf == NULL) {
		std::cout << "Buffer is empty!" << std::endl;
		return std::unordered_map<std::string, char*>();
	}

	std::unordered_map<std::string, char*> kvpairs;
    char *end_str;

    char *token = MultiPlatformHelper::strtok(buf, "\n", &end_str);

    while (token != NULL)
    {
        char *end_token;
		char *token2 = MultiPlatformHelper::strtok(token, " ", &end_token);

        std::vector<char*> info;
        while (token2 != NULL)
        {
    		info.push_back(token2);
			token2 = MultiPlatformHelper::strtok(NULL, " ", &end_token);
        }

        char* key = info[0];
        char* value = info[1];
        kvpairs[std::string(key)] = value;

		token = MultiPlatformHelper::strtok(NULL, "\n", &end_str);
    }

    return kvpairs;
}

void Application::hideGui() {

}

void Application::showGui() {

}


void Application::resetNetManager() {
	if(netManager) {
		delete netManager;
		netManager = new NetManager();
	} 
}

Chunk* Application::getChunk(std::unordered_map<std::pair<int, int>, Chunk*>& chunks, int x, int z) {
	std::pair<int, int> name(x,z);

	if (chunks[name]) return chunks[name];
	else return nullptr;
}

void Application::recomputeColliders(std::unordered_map<std::pair<int, int>, Chunk*>& chunks, int currX, int currZ) {
// Remove the old static objects currently in the simulator
	_simulator->removeStaticObjects();

	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			int x = currX;
			int z = currZ;

			x += i*CHUNK_SIZE;
			z += j*CHUNK_SIZE;

			std::pair<int, int> name(x ,z);

			if (chunks[name]) {
				chunks[name]->addChunksToSimulator();
			}
		}
	}
}

void Application::moveDayTime(float time) {
	static float daySeconds = 300;
	static float lastTime = 0;
	static float minLight = .1;

	float currTime = lastTime + time;
	lastTime = currTime;
	float intensity = Ogre::Math::Cos(Ogre::Math::PI / daySeconds * currTime)*.3 + .5;
	float sunAngleX = Ogre::Math::Cos( (currTime * (Ogre::Math::PI) / daySeconds) + 3*Ogre::Math::PI/2);
	float sunAngleY = Ogre::Math::Sin( (currTime * (Ogre::Math::PI) / daySeconds) + 3*Ogre::Math::PI/2);

	mSceneManager->setAmbientLight(Ogre::ColourValue(intensity, intensity, intensity));

	sun->setDiffuseColour(Ogre::ColourValue(intensity, intensity, intensity));
	sun->setDirection(Ogre::Vector3(sunAngleX, sunAngleY, 0));
}

void Application::loadSeed() {
	std::fstream saveFile;
	saveFile.open("save.txt", std::ios::in);

	std::string line;
	getline(saveFile, line);

	if (line == magicHeader) {
		getline(saveFile, line);
		seed = std::stoi(line);
	}
	else {
		seed = time(0);
	}

	saveFile.close();

	srand(seed);
}
