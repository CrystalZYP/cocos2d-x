#pragma execution_character_set("utf-8")

#include "FatNinja.h"
#include "cocos2d.h"
#include "MenuScene.h"
#include "SimpleAudioEngine.h"

using namespace CocosDenshion;
using namespace std;
USING_NS_CC;

int FatNinja::count = 0;
int FatNinja::deadBoss = 0;
void FatNinja::setPhysicsWorld(PhysicsWorld* world) { m_world = world; }

Scene* FatNinja::createScene()
{
	srand((unsigned)time(NULL));
	auto scene = Scene::createWithPhysics();

	// 关闭自动步进
	scene->getPhysicsWorld()->setAutoStep(true);
	// Debug 模式
	//scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	scene->getPhysicsWorld()->setGravity(Vec2(0, -300.0f));
	auto layer = FatNinja::create();
	layer->setPhysicsWorld(scene->getPhysicsWorld());
	scene->addChild(layer);
	return scene;
}

void FatNinja::update(float dt) {
	//手动设置步进
	this->getScene()->getPhysicsWorld()->step(1 / 100.0f);
	//player速度
	auto x = 0;
	auto y = 0;
	if (IsPlayer1Left || IsPlayer1Right) {
		if (IsPlayer1Left) 
			x = -700;
		else 
			x = 10;
		y = player1->getPhysicsBody()->getVelocity().y;
		player1->getPhysicsBody()->setVelocity(Vec2(x, y));
	}
	player1->getPhysicsBody()->setDynamic(true);

	// 判断player是否在跳跃
	if (player1->getPositionY() < 120) {
		IsPlayer1Jump = false;
	}
	// 判断是否发动fart攻击消灭敌人
	if (IsPlayer1Fart) {
		// 消灭boss1
		list<Sprite*>::iterator it1 = boss1.begin();
		for (; it1 != boss1.end();) {
			if ((*it1) != NULL) {
				if ((*it1)->getPosition().getDistance(player1->getPosition()) < 200) {
					(*it1)->stopAllActions();
					auto animation = (Animate::create(AnimationCache::getInstance()->getAnimation("boss1DeadAnimation")));
					// 回调函数
					Sprite *temp = (*it1);
					auto seq = Sequence::create(animation, CallFunc::create([temp] {
						temp->removeFromParentAndCleanup(true);
					}), nullptr);
					temp->runAction(seq);
					it1 = boss1.erase(it1);
					boss1_survival--;
					deadBoss++;
					break;
				}
				else 
					it1++;
			}
		}

		// 消灭boss2
		list<Sprite*>::iterator it2 = boss2.begin();
		for (; it2 != boss2.end();) {
			if ((*it2) != NULL) {
				if ((*it2)->getPosition().getDistance(player1->getPosition()) < 200) {
					(*it2)->stopAllActions();
					auto animation = (Animate::create(AnimationCache::getInstance()->getAnimation("boss2DeadAnimation")));
					// 回调函数
					Sprite *temp = (*it2);
					auto seq = Sequence::create(animation, CallFunc::create([temp] {
						temp->removeFromParentAndCleanup(true);
					}), nullptr);
					temp->runAction(seq);
					it2 = boss2.erase(it2);
					boss2_survival--;
					deadBoss++;
					break;
				}
				else 
					it2++;
			}
		}
	}
}

// 更新boss状态、数量
void FatNinja::updateBoss(float dt) {
	// boss1：最多5个，每次射击可扣player3滴血，射击范围为150
	// 更新boss1数量
	if (boss1_survival == 0) {
		// 每次产生的数量加1，增加难度
		boss1_production++;
		if (boss1_production >= 5) boss1_production %= 4;
		boss1_survival = boss1_production;
		for (int i = 0; i < boss1_production; i++) {
			Sprite* boss = Sprite::create("boss1walk.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 4, 100, 135)));
			boss->setPhysicsBody(PhysicsBody::createBox(Size(102, 128), PhysicsMaterial(1.0f, 1.0f, 10.0f)));
			// 左右两边都会产生boss
			if (i % 2 == 0) {
				boss->setPosition(visibleSize.width / 2 - 800, 80);
			}
			else {
				boss->setPosition(visibleSize.width / 2 + 800, 80);
			}
			boss->getPhysicsBody()->setCategoryBitmask(4);
			boss->getPhysicsBody()->setCollisionBitmask(4);
			boss->getPhysicsBody()->setContactTestBitmask(4);
			boss->getPhysicsBody()->setRotationEnable(false);
			boss1.push_back(boss);
			this->addChild(boss, 3);
		}
	}

	// 更新boss1状态
	list<Sprite*>::iterator it = boss1.begin();
	for (; it != boss1.end(); it++) {
		if ((*it) != NULL) {
			// 运动状态
			if (player1->getPosition().getDistance((*it)->getPosition()) > 150) {
				// 判断方向
				if (player1->getPosition().x < (*it)->getPosition().x) {
					(*it)->setFlippedX(true);
				}
				else {
					(*it)->setFlippedX(false);
				}
				// 播放walk动画
				Action* animation = RepeatForever::create(Animate::create(AnimationCache::getInstance()->getAnimation("boss1WalkAnimation")));
				animation->setTag(12);
				int num = 1;
				(*it)->runAction(animation);
				num = 2;
				// 子弹到达相应坐标后消失
				auto xx = 0;
				if ((*it)->isFlipX())
					xx = -500;
				else xx = 500;
				(*it)->getPhysicsBody()->setVelocity(Vec2(xx, 0));
			}
			// 到达攻击范围
			if (player1->getPosition().getDistance((*it)->getPosition()) < 150) {
				auto animation = RepeatForever::create(Animate::create(AnimationCache::getInstance()->getAnimation("boss1ShotAnimation")));
				animation->setTag(15);
				(*it)->runAction(animation);
				auto bullet = Sprite::create("bullet.png");
				bullet->setAnchorPoint(Vec2(0.5, 0.5));
				if ((*it)->isFlippedX()) {
					bullet->setPosition((*it)->getPosition().x - 50, 100);
				}
				else {
					bullet->setPosition((*it)->getPosition().x + 50, 100);
				}
				this->addChild(bullet);
				// 播放射击音效
				SimpleAudioEngine::getInstance()->playEffect("music/shot.mp3", false);
				// 子弹到达相应坐标后消失
				auto temp_position = player1->getPosition();
				auto seq = Sequence::create(MoveTo::create(0.8f, Vec2(temp_position.x, bullet->getPositionY())),
					RemoveSelf::create(), nullptr);
				bullet->runAction(seq);
				
				// 当player未跳跃的时候才能攻击到
				if (temp_position.y < 210) {
					// 当player仍在攻击范围内时才能攻击到
					if ((player1->getPositionX() <= temp_position.x && player1->getPositionX() >= (*it)->getPositionX()) ||
						((player1->getPositionX() >= temp_position.x && player1->getPositionX() <= (*it)->getPositionX()))) {
						// 血条减少
						hp -= 3;
						if (hp <= 0) {
							hp = 0;
							this->GameOver();
						}
						CCProgressTo* pro = CCProgressTo::create(1.0, hp);
						pT->runAction(pro);
					}
				}
			}
		}
	}

	// boss2：最多3个，每次射击可扣player5滴血，射击范围为200
	// 更新boss2数量 
	if (boss2_survival == 0) {
		// 每次产生的数量加1，增加难度
		boss2_production++;
		if (boss2_production >= 4) boss2_production %= 4;
		boss2_survival = boss2_production;
		for (int i = 0; i < boss2_production; i++) {
			Sprite* boss = Sprite::create("boss2walk.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 4, 105, 140)));
			boss->setPhysicsBody(PhysicsBody::createBox(Size(105, 140), PhysicsMaterial(1.0f, 1.0f, 10.0f)));
			// 左右两边都会产生boss
			if (i % 2 == 0) {
				boss->setPosition(visibleSize.width / 2 - 700, 80);
			}
			else {
				boss->setPosition(visibleSize.width / 2 + 700, 80);
			}
			boss->getPhysicsBody()->setCategoryBitmask(8);
			boss->getPhysicsBody()->setCollisionBitmask(8);
			boss->getPhysicsBody()->setContactTestBitmask(8);
			boss->getPhysicsBody()->setRotationEnable(false);
			boss2.push_back(boss);
			this->addChild(boss, 3);
		}
	}

	// 更新boss2状态
	list<Sprite*>::iterator it1 = boss2.begin();
	for (; it1 != boss2.end(); it1++) {
		if ((*it1) != NULL) {
			// 运动状态
			if (player1->getPosition().getDistance((*it1)->getPosition()) > 200) {
				// 判断方向
				if (player1->getPosition().x < (*it1)->getPosition().x) {
					(*it1)->setFlippedX(true);
				}
				else {
					(*it1)->setFlippedX(false);
				}
				// 播放walk动画
				Action* animation = RepeatForever::create(Animate::create(AnimationCache::getInstance()->getAnimation("boss2WalkAnimation")));
				animation->setTag(12);
				(*it1)->runAction(animation);
				// 初速度
				auto xx = 0;
				if ((*it1)->isFlipX())
					xx = -500;
				else xx = 800;
				(*it1)->getPhysicsBody()->setVelocity(Vec2(xx, 0));
			}
			// 到达攻击范围
			if (player1->getPosition().getDistance((*it1)->getPosition()) < 200) {
				auto animation = RepeatForever::create(Animate::create(AnimationCache::getInstance()->getAnimation("boss2ShotAnimation")));
				animation->setTag(15);
				(*it1)->runAction(animation);
				auto bullet = Sprite::create("bullet.png");
				bullet->setAnchorPoint(Vec2(0.5, 0.5));
				if ((*it1)->isFlippedX()) {
					bullet->setPosition((*it1)->getPosition().x - 40, 120);
				}
				else {
					bullet->setPosition((*it1)->getPosition().x + 40, 120);
				}
				this->addChild(bullet);
				// 播放射击音效
				SimpleAudioEngine::getInstance()->playEffect("music/shot.mp3", false);				
				// 子弹到达相应坐标后消失
				auto temp_position = player1->getPosition();
				auto seq = Sequence::create(MoveTo::create(0.8f, Vec2(temp_position.x, bullet->getPositionY())),
					RemoveSelf::create(), nullptr);
				bullet->runAction(seq);

				// 当player未跳跃的时候才能攻击到
				if (temp_position.y < 210) {
					// 当player仍在攻击范围内时才能攻击到
					if ((player1->getPositionX() <= temp_position.x && player1->getPositionX() >= (*it1)->getPositionX()) ||
						((player1->getPositionX() >= temp_position.x && player1->getPositionX() <= (*it1)->getPositionX()))) {
						// 血条减少
						hp -= 5;
						if (hp <= 0) {
							hp = 0;
							this->GameOver();
						}
						CCProgressTo* pro = CCProgressTo::create(1.0, hp);
						pT->runAction(pro);
					}
				}
			}
		}
	}
}

// player1站立
void FatNinja::idleAnimation(float dt) {
	this->getScene()->getPhysicsWorld()->step(1.0f);

	if (!(IsPlayer1Fart || IsPlayer1Left || IsPlayer1Right)) {
		auto animation = (Animate::create(AnimationCache::getInstance()->getAnimation("player1IdleAnimation")));
		animation->setTag(11);
		player1->runAction(animation);
	}
}

// 为player1出场添加粒子系统
void FatNinja::addPatical() {
	ParticleExplosion* flower = ParticleExplosion::create();
	flower->setPosition(player1->getPosition());
	flower->setDuration(0.5);
	this->addChild(flower);
}

// on "init" you need to initialize your instance
bool FatNinja::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
	visibleSize = Director::getInstance()->getVisibleSize();
	origin = Director::getInstance()->getVisibleOrigin();

	//add background picture
	auto bgSprite = Sprite::create("background.png");
	bgSprite->setPosition(visibleSize / 2);
	bgSprite->setScale(visibleSize.width / bgSprite->getContentSize().width, visibleSize.height / bgSprite->getContentSize().height);
	this->addChild(bgSprite, 0);

	// hp条player能量
	Sprite* sp0 = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
	Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));
	pT = ProgressTimer::create(sp);
	pT->setScaleX(90);
	pT->setAnchorPoint(Vec2(0, 0));
	pT->setType(ProgressTimerType::BAR);
	pT->setBarChangeRate(Point(1, 0));	//(0,1)表示竖向,(1,0)表示横向
	pT->setMidpoint(Point(0, 1));
	pT->setPercentage(100);
	pT->setPosition(Vec2(origin.x + 14 * pT->getContentSize().width, origin.y + visibleSize.height - 2 * pT->getContentSize().height));
	this->addChild(pT, 1);
	sp0->setAnchorPoint(Vec2(0, 0));
	sp0->setPosition(Vec2(origin.x + pT->getContentSize().width, origin.y + visibleSize.height - sp0->getContentSize().height));
	this->addChild(sp0, 0);

	//攻击boss数量
	number = Label::createWithTTF("0", "fonts/arial.TTF", 35);
	number->setColor(Color3B(0, 0, 0));
	number->setPosition(Vec2(origin.x + 350, origin.y + visibleSize.height - 25));
	addChild(number, 1);

	loadMusic();
	addSprite();
	addListener();
	addPlayer();
	addPatical();
	setBoundary();


	//boss初始数量
	boss1_survival = 0, boss1_production = 0;
	boss2_survival = 0, boss2_production = 0;

	loadBoss1Animation("boss1");
	loadBoss2Animation("boss2");

	// 添加调度器
	schedule(schedule_selector(FatNinja::update), 0.01f, kRepeatForever, 0.1f);
	schedule(schedule_selector(FatNinja::idleAnimation), 2.0f, kRepeatForever, 1.0f);
	schedule(schedule_selector(FatNinja::foodfall), 2.0f, kRepeatForever, 0);
	schedule(schedule_selector(FatNinja::updateDeadBossNum), 0.1f, kRepeatForever, 0);
	schedule(schedule_selector(FatNinja::updateBoss), 0.1f, kRepeatForever, 0);

	//SimpleAudioEngine::getInstance()->preloadBackgroundMusic("exclusion.wav");

	//SimpleAudioEngine::getInstance()->playBackgroundMusic("exclusion.wav", true);

    return true;
}

// 定义世界的边界
void FatNinja::setBoundary() {
	Vec2* points = new Vec2[4];
	points[0] = Vec2(-visibleSize.width / 2 + 10, visibleSize.height/2);
	points[1] = Vec2(visibleSize.width / 2 - 10, visibleSize.height/2);
	points[2] = Vec2(visibleSize.width / 2 - 10, -visibleSize.height / 2 + 55);
	points[3] = Vec2(-visibleSize.width / 2 + 10, -visibleSize.height / 2 + 55); 
	auto body = PhysicsBody::createEdgePolygon(points, 4, PhysicsMaterial(1.0f, 0.0f, 1.0f));
	edgeNode = Node::create();
	edgeNode->setPosition(visibleSize / 2);
	edgeNode->setPhysicsBody(body);
	this->addChild(edgeNode);
}
// 掉落食物
void FatNinja::foodfall(float dt) {
	Sprite* food;
	if (count % 5 == 0) 
		food = Sprite::create("berry.png");
	else if (count % 5 == 1) 
		food = Sprite::create("chicken.png");
	else if (count % 5 == 2) 
		food = Sprite::create("cream.png");
	else if (count % 5 == 3) 
		food = Sprite::create("pear.png");
	else if (count % 5 == 4) 
		food = Sprite::create("bread.png");
	count++;
	auto foodbody = PhysicsBody::createBox(food->getContentSize(), PhysicsMaterial(1000.0f, 0.0f, 1000.0f));  // 材质密度、恢复系数、摩擦系数
	foodbody->setCategoryBitmask(3);
	foodbody->setCollisionBitmask(3);
	foodbody->setContactTestBitmask(3);
	foodbody->setRotationEnable(false);
	food->setPhysicsBody(foodbody);
	food->setPosition(rand() % (int)visibleSize.width, visibleSize.height);
	food->setName("food");
	this->addChild(food);
}

// 添加背景和各种精灵
void FatNinja::addSprite() {
	// add ground
	ground = Sprite::create("ground.png");
	ground->setScale(visibleSize.width / ground->getContentSize().width, 3.1f);
	ground->setPosition(visibleSize.width / 2, 30);
	auto groundbody = PhysicsBody::createBox(ground->getContentSize(), PhysicsMaterial(100.0f, 0.0f, 10.0f));	// 材质密度、恢复系数、摩擦系数
	groundbody->setCategoryBitmask(0xFFFFFFFF);
	groundbody->setCollisionBitmask(0xFFFFFFFF);
	groundbody->setContactTestBitmask(0xFFFFFFFF);
	groundbody->setDynamic(false);  // 静态刚体不受重力影响
	ground->setPhysicsBody(groundbody);
	this->addChild(ground, 1);
}

// 创建角色
void FatNinja::addPlayer() {
	IsPlayer1Left = false;
	IsPlayer1Right = false;
	IsPlayer1Fart = false;
	IsPlayer1Jump = false;
	LastPlayer1Press = 'D';

	// create player1
	auto texture1 = Director::getInstance()->getTextureCache()->addImage("idle.png");
	frame1 = SpriteFrame::createWithTexture(texture1, CC_RECT_PIXELS_TO_POINTS(Rect(0, 4, 102, 128)));
	player1 = Sprite::createWithSpriteFrame(frame1);
	player1->setPhysicsBody(PhysicsBody::createBox(Size(90, 100), PhysicsMaterial(1.0f, 1.0f, 10.0f)));
	player1->setPosition(Vec2(visibleSize.width / 2, 80));
	player1->getPhysicsBody()->setCategoryBitmask(2);
	player1->getPhysicsBody()->setCollisionBitmask(2);
	player1->getPhysicsBody()->setContactTestBitmask(2);
	player1->getPhysicsBody()->setRotationEnable(false);
	player1->setName("player1");
	this->addChild(player1, 2);
	loadPlayerAnimation("player1");
}

// 添加监听器
void FatNinja::addListener() {
	//Keyboard监听器
	auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(FatNinja::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(FatNinja::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);
	//自定义监听器
	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(FatNinja::onConcactBegin, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
}

// 加载音效
void FatNinja::loadMusic() {
	SimpleAudioEngine::getInstance()->preloadEffect("music/gameover.mp3");
	SimpleAudioEngine::getInstance()->preloadEffect("music/fart.mp3");
	SimpleAudioEngine::getInstance()->preloadEffect("music/exclusion.wav");
	SimpleAudioEngine::getInstance()->preloadEffect("music/shot.mp3");

	SimpleAudioEngine::getInstance()->preloadBackgroundMusic("music/bgm.mp3");

	SimpleAudioEngine::getInstance()->playBackgroundMusic("music/bgm.mp3", true);
}

// 按下按键
// 上下左右
void FatNinja::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
	Action* animation;
	list<Sprite*>::iterator it = boss1.begin();
	list<Sprite*>::iterator it1 = boss2.begin();
	ParticleGalaxy* exclusion = ParticleGalaxy::create();
	ParticleSmoke* fart = ParticleSmoke::create();

	switch (code) {
	case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		if (code == EventKeyboard::KeyCode::KEY_LEFT_ARROW) {
			IsPlayer1Left = true;
			IsPlayer1Right = false;
			if (LastPlayer1Press == 'D')
				player1->setFlipX(true);
			LastPlayer1Press = 'A';
		}
		else {
			IsPlayer1Right = true;
			IsPlayer1Left = false;
			if (LastPlayer1Press == 'A')
				player1->setFlipX(false);
			LastPlayer1Press = 'D';
		}
		player1->stopAllActionsByTag(11);
		animation = RepeatForever::create(Animate::create(AnimationCache::getInstance()->getAnimation("player1RunAnimation")));
		animation->setTag(11);
		player1->runAction(animation);
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
		if (IsPlayer1Jump) break;
		player1->stopAllActionsByTag(11);
		IsPlayer1Jump = true;
		player1->getPhysicsBody()->setVelocity(Vec2(0, 350));
		break;
	// 技能：放屁攻击，范围攻击（200以内的敌人）
	case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:
		if (IsPlayer1Fart || IsPlayer1Jump) break;
		IsPlayer1Fart = true;
		// 播放放屁音效
		SimpleAudioEngine::getInstance()->playEffect("music/fart.mp3", false);
		//　加个特效
		fart->setPosition(player1->getPosition());
		fart->setDuration(0.05f);
		this->addChild(fart);

		player1->stopAllActions();
		animation = Animate::create(AnimationCache::getInstance()->getAnimation("player1FartAnimation"));
		animation->setTag(11);
		player1->runAction(animation); 
		break;
	// 技能：排开敌人，范围攻击（350以内的敌人）
	case cocos2d::EventKeyboard::KeyCode::KEY_ENTER:
		//　加个特效
		exclusion->setPosition(player1->getPosition());
		exclusion->setDuration(0.2);
		this->addChild(exclusion);
		// 播放音效
		SimpleAudioEngine::getInstance()->playEffect("music/exclusion.wav", false);
		// 排开boss1（排到500外）
		for (; it != boss1.end(); it++) {
			if ((*it) != NULL) {
				if (player1->getPosition().getDistance((*it)->getPosition()) < 350) {
					if ((*it)->isFlippedX()) {
						(*it)->setPosition(player1->getPosition().x + 500, (*it)->getPositionY());
					}
					else {
						(*it)->setPosition(player1->getPosition().x - 500, (*it)->getPositionY());
					}
				}
			}
		}

		//排开boss2（排到400外）
		for (; it1 != boss2.end(); it1++) {
			if ((*it1) != NULL) {
				if (player1->getPosition().getDistance((*it1)->getPosition()) < 350) {
					if ((*it1)->isFlippedX()) {
						(*it1)->setPosition(player1->getPosition().x + 400, (*it1)->getPositionY());
					}
					else {
						(*it1)->setPosition(player1->getPosition().x - 400, (*it1)->getPositionY());
					}
				}
			}
		}

		break;
	default:
		break;
	}
}

// 释放按键
void FatNinja::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
	case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		if (code == EventKeyboard::KeyCode::KEY_LEFT_ARROW)
			IsPlayer1Left = false;
		else 
			IsPlayer1Right = false;
		player1->stopAllActionsByTag(11);
		player1->getPhysicsBody()->setVelocity(Vec2(0, 0));
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:
		IsPlayer1Fart = false;
		player1->getPhysicsBody()->setVelocity(Vec2(0, 0));
		break;
	default:
		IsPlayer1Fart = false;
		break;
	}
}

// 碰撞情况
bool FatNinja::onConcactBegin(PhysicsContact & contact) {
	auto shapeA = contact.getShapeA();
	auto shapeB = contact.getShapeB();
	//player和food
	if (shapeA->getCollisionBitmask() == 3 || shapeB->getCollisionBitmask() == 3) {
		if (shapeA->getCollisionBitmask()&shapeB->getCategoryBitmask() == 2 && shapeA->getCategoryBitmask()&shapeB->getCollisionBitmask() == 2) {
			hp += 20;
			if (hp >= 100)
				hp = 100;
			CCProgressTo* pro = CCProgressTo::create(1.0, hp);
			pT->runAction(pro);
		}
		auto food = getChildByName("food");
		food->removeFromParentAndCleanup(true);
	}
	return true;
}

// 更新player攻击的boss数量
void FatNinja::updateDeadBossNum(float dt) {
	String* str = String::createWithFormat("%d", deadBoss);
	this->number->setString(str->getCString());
}

// 加载玩家动画帧
void FatNinja::loadPlayerAnimation(string filepath) {
	Vector<SpriteFrame*> PlayerAttackFart;
	Vector<SpriteFrame*> PlayerIdle;
	Vector<SpriteFrame*> PlayerRun;

	// 放屁攻击动画（帧数：9，高：140，宽：114）
	auto texture = Director::getInstance()->getTextureCache()->addImage("attackFart.png");
	PlayerAttackFart.reserve(9);
	for (int i = 0; i < 9; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(102 * i, 0, 102, 136)));
		PlayerAttackFart.pushBack(frame);
	}
	Animation* PlayerAttackFartAnimation = Animation::createWithSpriteFrames(PlayerAttackFart, 0.1f);
	AnimationCache::getInstance()->addAnimation(PlayerAttackFartAnimation, filepath + "FartAnimation");

	// 站立动画（帧数：150，高：136，宽：102）!!!
	texture = Director::getInstance()->getTextureCache()->addImage("idle.png");
	PlayerIdle.reserve(6);
	for (int i = 0; i < 6; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(102 * i, 0, 102, 128)));
		PlayerIdle.pushBack(frame);
	}
	Animation* PlayerIdleAnimation = Animation::createWithSpriteFrames(PlayerIdle, 0.8);
	AnimationCache::getInstance()->addAnimation(PlayerIdleAnimation, filepath + "IdleAnimation");

	// 跑动画（帧数：14，高：128，宽：95）!!!
	texture = Director::getInstance()->getTextureCache()->addImage("run.png");
	PlayerRun.reserve(14);
	for (int i = 0; i < 14; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(95 * i, 0, 95, 128)));
		
		PlayerRun.pushBack(frame);
	}
	Animation* PlayerRunAnimation = Animation::createWithSpriteFrames(PlayerRun, 0.1f);
	AnimationCache::getInstance()->addAnimation(PlayerRunAnimation, filepath + "RunAnimation");
}

// 加载boss1动画帧
void FatNinja::loadBoss1Animation(string filepath) {
	Vector<SpriteFrame*> boss1Dead;
	Vector<SpriteFrame*> boss1Shot;
	Vector<SpriteFrame*> boss1Walk;

	//boss1走路动画（帧数：8，高：135，宽：100）
	auto texture = Director::getInstance()->getTextureCache()->addImage("boss1walk.png");
	boss1Walk.reserve(8);
	for (int i = 0; i < 8; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(100 * i, 0, 100, 135)));
		boss1Walk.pushBack(frame);
	}
	Animation* boss1WalkAnimation = Animation::createWithSpriteFrames(boss1Walk, 0.9f);
	AnimationCache::getInstance()->addAnimation(boss1WalkAnimation, filepath + "WalkAnimation");

	//boss1攻击动画（帧数：2，高：135，宽：102）
	texture = Director::getInstance()->getTextureCache()->addImage("boss1shot.png");
	boss1Shot.reserve(2);
	for (int i = 0; i < 2; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(102 * i, 0, 102, 135)));
		boss1Shot.pushBack(frame);
	}
	Animation* boss1ShotAnimation = Animation::createWithSpriteFrames(boss1Shot, 0.2f);
	AnimationCache::getInstance()->addAnimation(boss1ShotAnimation, filepath + "ShotAnimation");

	//boss1死动画（帧数：5，高：135，宽：102）
	texture = Director::getInstance()->getTextureCache()->addImage("boss1dead.png");
	boss1Dead.reserve(5);
	for (int i = 0; i < 5; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(102 * i, 0, 102, 135)));
		boss1Dead.pushBack(frame);
	}
	Animation* boss1DeadAnimation = Animation::createWithSpriteFrames(boss1Dead, 0.1f);
	AnimationCache::getInstance()->addAnimation(boss1DeadAnimation, filepath + "DeadAnimation");	
}

// 加载boss2动画帧
void FatNinja::loadBoss2Animation(string filepath) {
	Vector<SpriteFrame*> boss2Dead;
	Vector<SpriteFrame*> boss2Shot;
	Vector<SpriteFrame*> boss2Walk;
	//boss2走路动画（帧数：7，高：140，宽：108）
	auto texture = Director::getInstance()->getTextureCache()->addImage("boss2walk.png");
	boss2Walk.reserve(7);
	for (int i = 0; i < 7; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(105 * i, 0, 105, 140)));
		boss2Walk.pushBack(frame);
	}
	Animation* boss2WalkAnimation = Animation::createWithSpriteFrames(boss2Walk, 0.3f);
	AnimationCache::getInstance()->addAnimation(boss2WalkAnimation, filepath + "WalkAnimation");

	//boss2攻击动画（帧数：2，高：140，宽：102）
	texture = Director::getInstance()->getTextureCache()->addImage("boss2shot.png");
	boss2Shot.reserve(2);
	for (int i = 0; i < 2; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(102 * i, 0, 102, 140)));
		boss2Shot.pushBack(frame);
	}
	Animation* boss2ShotAnimation = Animation::createWithSpriteFrames(boss2Shot, 0.3f);
	AnimationCache::getInstance()->addAnimation(boss2ShotAnimation, filepath + "ShotAnimation");

	//boss2死动画（帧数：5，高：140，宽：105）
	texture = Director::getInstance()->getTextureCache()->addImage("boss2dead.png");
	boss2Dead.reserve(5);
	for (int i = 0; i < 5; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(105 * i, 0, 102, 140)));
		boss2Dead.pushBack(frame);
	}
	Animation* boss2DeadAnimation = Animation::createWithSpriteFrames(boss2Dead, 0.1f);
	AnimationCache::getInstance()->addAnimation(boss2DeadAnimation, filepath + "DeadAnimation");
}

// 游戏结束
void FatNinja::GameOver() {
	//删除调度器
	unschedule(schedule_selector(FatNinja::update));
	unschedule(schedule_selector(FatNinja::updateDeadBossNum));
	unschedule(schedule_selector(FatNinja::idleAnimation));
	unschedule(schedule_selector(FatNinja::foodfall));
	unschedule(schedule_selector(FatNinja::updateBoss));
	m_world->removeBody(edgeNode->getPhysicsBody());

	SimpleAudioEngine::getInstance()->stopBackgroundMusic("music/bgm.wav");
	SimpleAudioEngine::getInstance()->playEffect("music/gameover.mp3", false);

	// 停止boss的动作
	list<Sprite*>::iterator it = boss1.begin();
	for (; it != boss1.end(); it++) {
		if ((*it) != NULL) {
			boss1_frame = (*it)->getSpriteFrame();
			(*it)->stopAllActions();
			(*it)->setSpriteFrame(boss1_frame);
		}
	}
	list<Sprite*>::iterator it1 = boss2.begin();
	for (; it1 != boss2.end(); it1++) {
		if ((*it1) != NULL) {
			boss2_frame = (*it1)->getSpriteFrame();
			(*it1)->stopAllActions();
			(*it1)->setSpriteFrame(boss2_frame);
		}
	}

	//GameOver
	auto gameover = Sprite::create("gameover.png");
	gameover->setPosition(visibleSize.width / 2, visibleSize.height / 2 + 70);
	this->addChild(gameover);

	//replay
	auto label = Label::createWithTTF("Replay", "fonts/arial.TTF", 40);
	label->setColor(Color3B(255, 255, 255));
	auto replayBtn = MenuItemLabel::create(label, CC_CALLBACK_1(FatNinja::clickCallback, this));
	Menu* replay = Menu::create(replayBtn, NULL);
	replay->setPosition(visibleSize.width / 2 - 95, visibleSize.height / 2 - 80);
	this->addChild(replay);

	//exit
	label = Label::createWithTTF("Exit", "fonts/arial.TTF", 40);
	label->setColor(Color3B(255, 255, 255));
	auto exitBtn = MenuItemLabel::create(label, CC_CALLBACK_1(FatNinja::exitCallback, this));
	Menu* exit = Menu::create(exitBtn, NULL);
	exit->setPosition(visibleSize.width / 2 + 130, visibleSize.height / 2 - 80);
	this->addChild(exit);

}

// 继续或重玩按钮响应函数
void FatNinja::clickCallback(Ref * pSender) {
	Director::getInstance()->replaceScene(FatNinja::createScene());
}

// 退出游戏
void FatNinja::exitCallback(Ref * pSender) {
	auto scene = MenuSence::createScene();
	Director::getInstance()->replaceScene(scene);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}