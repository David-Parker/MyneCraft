#include "Animation.h"

Animation::Animation(){
	_inAction = false;
}

Animation::~Animation(){}

void Animation::addAction(Action* action) {
	_actions.push_back(action);
}

void Animation::playAnimation() {
	if (!_inAction || _currentAction == nullptr)
		return;

	_inAction = _currentAction->playAction();
}

void Animation::createPickaxeAnimation(Ogre::SceneNode* sceneNode) {
	Action* action = new Action(sceneNode);
	static int numtimes = 4;
	static int dOffset = -10;
	static int tOffset = 2;
	for (int i = 0; i <= numtimes; ++i)
		action->addAction(Action::YAW, Ogre::Degree(dOffset * i), Ogre::Vector3(tOffset, 0, 0) * i);
	for (int i = numtimes; i >= 0; --i)
		action->addAction(Action::YAW, Ogre::Degree(dOffset * i), Ogre::Vector3(tOffset, 0, 0) * i);

	addAction(action);
}

void Animation::createSwordAnimation(Ogre::SceneNode* sceneNode) {
	createPickaxeAnimation(sceneNode);
}

void Animation::createTorchAnimation(Ogre::SceneNode* sceneNode) {
	createPickaxeAnimation(sceneNode);
}

void Animation::createBlockAnimation(Ogre::SceneNode* sceneNode) {
	Action* action = new Action(sceneNode);
	static int numtimes = 4;
	static int dOffset = 10;
	static int tOffset = 0;
	for (int i = 0; i <= numtimes; ++i)
		action->addAction(Action::YAW, Ogre::Degree(dOffset * i), Ogre::Vector3(0, tOffset, tOffset) * i);
	for (int i = numtimes; i >= 0; --i)
		action->addAction(Action::YAW, Ogre::Degree(dOffset * i), Ogre::Vector3(0, tOffset, tOffset) * i);

	addAction(action);
}

void Animation::setActionLock(int actionPos) {
	if (_actions.size() <= actionPos)
		return;
	if (_inAction && _currentAction != _actions.at(actionPos))
		return;
	_currentAction = _actions.at(actionPos);
	_inAction = true;
}
