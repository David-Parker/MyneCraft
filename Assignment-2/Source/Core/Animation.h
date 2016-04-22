#pragma once

#include <vector>

#include "Action.h"

class Animation {
public:
	Animation();
	~Animation();
	void addAction(Action* action);
	void setPickaxe();
	void createPickaxeAnimation(Ogre::SceneNode* sceneNode);
	void playAnimation();
	bool _inAction;
	Action* _currentAction = nullptr;

protected:
	std::vector<Action*> _actions;
};