#include "MenuScene.h"
#include "FatNinja.h"
#include "helpScene.h"
#include "SimpleAudioEngine.h"

using namespace CocosDenshion;

USING_NS_CC;

Scene* helpScene::createScene() {
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = helpScene::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool helpScene::init()
{

	if (!Layer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();

	// 背景图片
	auto bg1 = Sprite::create("bg1.png");
	bg1->setPosition(visibleSize / 2);
	bg1->setScale(visibleSize.width / bg1->getContentSize().width, visibleSize.height / bg1->getContentSize().height);
	this->addChild(bg1, 0);

	//  back按钮
	auto label = Label::createWithTTF("Back", "fonts/arial.TTF", 40);
	label->setColor(Color3B(0, 0, 0));
	auto backBtn = MenuItemLabel::create(label, CC_CALLBACK_1(helpScene::jump, this));
	Menu* back = Menu::create(backBtn, NULL);
	back->setPosition(visibleSize.width - 300, 220);
	this->addChild(back);

	SimpleAudioEngine::getInstance()->preloadBackgroundMusic("music/menu.mp3");
	SimpleAudioEngine::getInstance()->playBackgroundMusic("music/menu.mp3", true);

	return true;
}

// helpScene界面跳转到MenuScene界面
void helpScene::jump(Ref* psender) {
	auto scene = MenuSence::createScene();
	Director::getInstance()->replaceScene(scene);
}
