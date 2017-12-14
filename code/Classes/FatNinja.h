#ifndef __FATNINJA_H__
#define __FATNINJA_H__

#include "cocos2d.h"

using namespace std;
USING_NS_CC;

class FatNinja : public Layer {
public:
	static PhysicsWorld* world;
	static cocos2d::Scene* createScene();
	void setPhysicsWorld(PhysicsWorld * world);
	void update(float dt);
	void idleAnimation(float dt);
	void addPatical();
	virtual bool init();
	void updateBoss(float dt);
	void setBoundary();
	void foodfall(float dt);
	void addSprite();
	void addPlayer();
	void addListener();
	void loadMusic();
	void onKeyPressed(EventKeyboard::KeyCode code, Event * event);
	void onKeyReleased(EventKeyboard::KeyCode code, Event * event);
	bool onConcactBegin(PhysicsContact & contact);
	void updateDeadBossNum(float dt);
	void loadPlayerAnimation(string filepath);
	void loadBoss1Animation(string filepath);
	void loadBoss2Animation(string filepath);
	void GameOver();
	void clickCallback(Ref * pSender);
	void exitCallback(Ref * pSender);
	// implement the "static create()" method manually
	CREATE_FUNC(FatNinja);
private:
	PhysicsWorld* m_world;
	Size visibleSize;
	Vec2 origin;
	Node* edgeNode;

	list<PhysicsBody*> food;
	list<Sprite*> boss1;
	list<Sprite*> boss2;
	Sprite* player1;
	Sprite* ground;

	SpriteFrame* frame1;
	SpriteFrame* boss1_frame;
	SpriteFrame* boss2_frame;

	cocos2d::ProgressTimer* pT;	   //player血条
	int hp = 100;

	cocos2d::Label* number;	//攻击boss的数量
	static int deadBoss;

	char LastPlayer1Press;	//player1运动方向
	bool IsPlayer1Left;		
	bool IsPlayer1Right;
	bool IsPlayer1Fart;
	bool IsPlayer1Jump;
	bool boss1move;
	bool boss2move;
	int boss1_survival;
	int boss2_survival;
	int boss1_production;
	int boss2_production;

	static int count;
};

#endif __FATNINJA_H__