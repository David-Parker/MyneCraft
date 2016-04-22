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
	static int offset = -10;
	for (int i = 0; i <= numtimes; ++i)
		action->addAction(Action::YAW, Ogre::Degree(offset*i));
	for (int i = numtimes; i >= 0; --i)
		action->addAction(Action::YAW, Ogre::Degree(offset*i));

	addAction(action);
}

void Animation::setPickaxe() {
	if (_inAction && _currentAction != _actions.at(0))
		return;
	_currentAction = _actions.at(0);
	_inAction = true;
}

