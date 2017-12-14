#include "MenuScene.h"
#include "FatNinja.h"
#include "helpScene.h"
#include "SimpleAudioEngine.h"

using namespace CocosDenshion;
USING_NS_CC;

Scene* MenuSence::createScene() {
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = MenuSence::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool MenuSence::init()
{

	if (!Layer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();

	// 背景图片
	auto bg = Sprite::create("bg.png");
	bg->setPosition(visibleSize / 2);
	bg->setScale(visibleSize.width / bg->getContentSize().width, visibleSize.height / bg->getContentSize().height);
	this->addChild(bg, 0);

	// 游戏标题
	auto title = Sprite::create("title.png");
	title->setPosition(Vec2(visibleSize.width - 330, visibleSize.height - 280));
	this->addChild(title, 1);

	// play按钮
	auto playItem = MenuItemImage::create("play1.png", "play2.png", CC_CALLBACK_1(MenuSence::jump1, this));
	playItem->setPosition(Vec2(visibleSize.width - 150, 150));
	// help按钮
	auto helpItem = MenuItemImage::create("help1.png", "help2.png", CC_CALLBACK_1(MenuSence::jump2, this));
	helpItem->setPosition(Vec2(visibleSize.width - 150, 55));
	auto menu = Menu::create(playItem, helpItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	SimpleAudioEngine::getInstance()->preloadBackgroundMusic("music/menu.mp3");
	SimpleAudioEngine::getInstance()->playBackgroundMusic("music/menu.mp3", true);

	return true;
}

// MenuScene界面跳转到FatNinja界面
void MenuSence::jump1(Ref* psender) {
	auto scene = FatNinja::createScene();
	Director::getInstance()->replaceScene(scene);
}

// MenuScene界面跳转到helpScene界面
void MenuSence::jump2(Ref* psender) {
	auto scene = helpScene::createScene();
	Director::getInstance()->replaceScene(scene);
}
