#pragma once
#ifndef __HELPSCENE_H__
#define __HELPSCENE_H__

#include "cocos2d.h"


class helpScene : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	void jump(Ref* psender);

	// implement the "static create()" method manually
	CREATE_FUNC(helpScene);
};

#endif 