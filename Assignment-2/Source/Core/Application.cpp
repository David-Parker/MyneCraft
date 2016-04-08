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

		t1 = new Timer();

		_simulator = new Simulator();

		setupWindowRendererSystem();

		setupOIS();

		netManager = new NetManager();
		setupCEGUI();

		setupCameras();

		setupGM();

		ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

		setupLighting();

		sg = mSceneManager->createStaticGeometry("CubeArea");

		createObjects();

	}
	catch (Exception e) {
		std::cout << "Exception Caught: " << e.what() << std::endl;
	}
}

/* 
* Update Methods 
*/
bool Application::frameRenderingQueued(const FrameEvent &evt)
{

	static float dTime = t1->getMilliseconds();
	CEGUI::System::getSingleton().injectTimePulse(evt.timeSinceLastFrame);

	if (!mRunning)
	{
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

	cameraMan->frameRenderingQueued(evt);

	return true;
}

// Called once per predefined frame
bool Application::update(const FrameEvent &evt) {

	OIS::KeyCode lastKey = _oisManager->lastKeyPressed();
	Ogre::Camera* camMan = mSceneManager->getCamera("Camera Man");
	playerCam->setPosition(camMan->getPosition());
	playerCam->lookAt(Ogre::Vector3(0,0,0));

	if (lastKey == OIS::KC_M) {
		gameManager->mute();
	}
	else if (lastKey == OIS::KC_1 || lastKey == OIS::KC_2 || lastKey == OIS::KC_3 || lastKey == OIS::KC_4) {
		int index = lastKey - 2;
		if (index >= 0 && index < cameras.size()) {
			mRenderWindow->removeAllViewports();
			mRenderWindow->addViewport(cameras[index]);
		}
	}
	else if(lastKey == OIS::KC_ESCAPE) {
		// close window when ESC is pressed
		mRunning = false;
	}

	Ogre::Vector3 pos = camMan->getPosition();
	static int range = CHUNK_SIZE * 1;

	try {
		//if (!currentChunk->pointInChunk(pos.x, pos.z)) {
		//	int oldXStart = currentChunk->_xStart;
		//	int oldXEnd = currentChunk->_xEnd;
		//	int oldYStart = currentChunk->_yStart;
		//	int oldYEnd = currentChunk->_yEnd;
		//	Ogre::Vector3 scale = currentChunk->_scale;

		//	if (pos.x > oldXEnd*scale.x * 2) {
		//		chunks.push_back(currentChunk = new Chunk(oldXEnd, oldYStart, mSceneManager, perlin, _simulator));
		//	}
		//	else if (pos.x < oldXStart*scale.x * 2) {
		//		chunks.push_back(currentChunk = new Chunk(oldXStart - CHUNK_SIZE, oldYStart, mSceneManager, perlin, _simulator));
		//	}
		//	else if (pos.z > oldYEnd*scale.y * 2) {
		//		chunks.push_back(currentChunk = new Chunk(oldXStart, oldYEnd, mSceneManager, perlin, _simulator));
		//	}
		//	else if (pos.z < oldYStart*scale.y * 2) {
		//		chunks.push_back(currentChunk = new Chunk(oldXStart, oldYStart - CHUNK_SIZE, mSceneManager, perlin, _simulator));
		//	}
		//}

		// Determine closest x,y aligned to the chunk size grid
		int x = (int)pos.x - ((int)pos.x % CHUNK_SIZE);
		int z = (int)pos.z - ((int)pos.z % CHUNK_SIZE);

		for (int i = (x - range); i < (x + range); i += CHUNK_SIZE) {
			for (int j = (z - range); j < (z + range); j += CHUNK_SIZE) {
				bool create = true;
				for (int k = 0; k < chunks.size(); k++) {
					// Chunk already exists
					if ((chunks[k]->_xStart == i && chunks[k]->_yStart == j)) {
						create = false;
					}
				}
				if (create)
					chunks.push_back(new Chunk(i, j, mSceneManager, perlin, _simulator));
			}
		}
	}
	catch (Exception e) {

	}
	
	// _simulator->stepSimulation(evt.timeSinceLastFrame, 1, 1.0 / fps);

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
}

Ogre::Entity* Application::createRootEntity(std::string mesh, int x, int y, int z) {

	Ogre::Entity* ogreEntity = mSceneManager->createEntity(mesh);
	ogreEntity->setCastShadows(true);
	Ogre::SceneNode* ogreNode = mSceneManager->getRootSceneNode()->createChildSceneNode();
	ogreNode->attachObject(ogreEntity);
	ogreNode->setPosition(x, y, z);
	ogreNode->setScale(50,50,50);

	return ogreEntity;
}

void Application::createChildEntity(std::string name, std::string mesh, Ogre::SceneNode* sceneNode, int x, int y, int z) {

	Ogre::Entity* ogreEntity = mSceneManager->createEntity(name, mesh);
	ogreEntity->setCastShadows(true);
	Ogre::SceneNode* ogreNode = sceneNode->createChildSceneNode(name);
	ogreNode->attachObject(ogreEntity);
	ogreNode->setPosition(x, y, z);
}

Cube* Application::createCube(Ogre::String nme, GameObject::objectType tp, Ogre::String meshName, int x, int y, int z, Ogre::Vector3 scale, Ogre::Degree pitch, Ogre::Degree yaw, Ogre::Degree roll, Ogre::SceneManager* scnMgr, GameManager* ssm, Ogre::Real mss, Ogre::Real rest, Ogre::Real frict, bool kinematic, Simulator* mySim) {
	float fi = (float)x/(float)100.0f;
	float fj = (float)z/(float)100.0f;
	y = 0;
	//y = (int)((perlin->getPerlin(fi, 0, fj))*100);
	createRootEntity(nme, meshName, x*scale.x*2, y*scale.y*2, z*scale.z*2);
	Ogre::SceneNode* sn = mSceneManager->getSceneNode(nme);
	Ogre::Entity* ent = SceneHelper::getEntity(mSceneManager, nme, 0);
	const btTransform pos;
	OgreMotionState* ms = new OgreMotionState(pos, sn);
	sn->setScale(scale.x, scale.y, scale.z);

	sn->pitch(pitch);
	sn->yaw(yaw);
	sn->roll(roll);

	Cube* obj = new Cube(nme, tp, mSceneManager, ssm, sn, ent, ms, mySim, mss, rest, frict, scale, kinematic);
	obj->addToSimulator();

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

}

void Application::setupCameras(void) {

	Ogre::Camera* camMan = mSceneManager->createCamera("Camera Man");
	playerCam = mSceneManager->createCamera("Player Cam");

	playerCam->setAutoAspectRatio(true);
	playerCam->setPosition(0,5000,0);
	playerCam->lookAt(0,0,0);
	playerCam->setNearClipDistance(1.0f);
	playerCam->setFarClipDistance(10.0f);

	camMan->setAutoAspectRatio(true);
	camMan->setPosition(0,300,0);
	camMan->lookAt(0,120,1800);
	camMan->setFarClipDistance(20000.0f);

	// Add viewport and cameras
	mRenderWindow->addViewport(camMan);

	cameras = std::vector<Ogre::Camera*>();
	cameras.push_back(camMan);
	cameras.push_back(playerCam);

	cameraMan = new OgreBites::SdkCameraMan(camMan);
	_oisManager->setupCameraMan(cameraMan);
	std::cout << camMan->getFarClipDistance() << std::endl;
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

	// mSceneManager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

}

void Application::createObjects(void) {
	int xmax = 128;
	int ymax = xmax;
	int rndmax = 8;
	float density = 1.5f; // 1 is very steep, 10 is pretty flat.

	perlin = new Perlin(xmax, ymax, rndmax, density);

	static Ogre::Entity* grassCube = nullptr;

	if(grassCube == nullptr) {
		grassCube = mSceneManager->createEntity("Cube-Grass.mesh");
	}

	chunks.push_back(currentChunk = new Chunk(0, 0, mSceneManager, perlin, _simulator));
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