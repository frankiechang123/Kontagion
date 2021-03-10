#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <cmath>
#include <iostream>
#include<vector>


using namespace std;
/*******CONSTANTS********/
const int FLAME_DAMAGE = 5;
const int SPRAY_DAMAGE = 2;

const int START_N_FLAME = 5;

const int FLAME_RANGE = 32;
const int SPRAY_RANGE = 112;

const int SOCRATES_HP = 100;

const int SCORE_INCREASE_HPGOODIE = 250;
const int SCORE_INCREASE_FLAMEGOODIE = 300;
const int SCORE_INCREASE_EXLIFEGOODIE = 500;
const int SCORE_INCREASE_FUNGUS = -50;

const int HP_DECREASE_FUNGUS = 20;
const int NFLAME_INCREASE_FLAMEGOODIE = 5;

const int REGULAR_START_HP = 4;
const int AGGRESSIVE_START_HP = 10;
const int ECOLI_START_HP = 5;

const int REGULAR_ATTACK = 1;
const int AGGRESSIVE_ATTACK = 2;
const int ECOLI_ATTACK = 4;

const int SCORE_INCREASE_ENEMY_KILL = 100;

const double MOVEMENT_OVERLAP_RANGE = SPRITE_WIDTH / 2;
const double HARM_PICKUP_RANGE = SPRITE_WIDTH;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class StudentWorld;
class Actor :public GraphObject {
public:
	Actor(StudentWorld* world, int imageID, double startX, double startY, int hp = 1, Direction dir = 0, int depth = 0, double size = 1.0)
		:GraphObject(imageID, startX, startY, dir, depth, size){
		m_world = world;
		m_HP = hp;
	}

	virtual ~Actor(){}
	virtual bool isActive() {
		return m_HP > 0;
	}
	virtual void doSomething() = 0;

	void setDead() {
		m_HP = 0;
	}

	virtual void decreaseHP(int value);

	
	virtual void deathEffect() {
		return;
	}

	void setHP(int value) {
		m_HP = value;
	}


	int getHP() {
		return m_HP;
	}

	StudentWorld* getWorld() {
		return m_world;
	}

	//used only for Socrates, output num of sprays to nsprays and the same for nFlames
	virtual bool getSocratesStat(int& nSprays, int& nFlames) {
		return false;
	}
	//used only for Socrates, set the value of nFlames
	virtual bool setSocratesNFlame(int n) {
		return false;
	}

	virtual bool isDamageable() {
		return false;
	}

	virtual bool isRequiredtoKill() {
		return false;
	}

	virtual bool isFood() {
		return false;
	}

	virtual bool isBlockage() {
		return false;
	}

	virtual int getHurtSound() {
		return SOUND_NONE;
	}

	virtual int getDieSound() {
		return SOUND_NONE;
	}


private:
	StudentWorld* m_world;
	int m_HP;
};

class Socrates:public Actor {
public:
	Socrates(StudentWorld* world,int imageID = IID_PLAYER, double startX = 0, double startY = VIEW_HEIGHT / 2) 
		:Actor(world,imageID,startX,startY,SOCRATES_HP) {//startHP=100
		m_nSprays = 20;
		m_nFlames = START_N_FLAME;
		m_posAngle = 180;
		m_haveSprayed = false;
	}

	virtual bool getSocratesStat(int& nSprays, int& nFlames) {
		nSprays = m_nSprays;
		nFlames = m_nFlames;
		return true;
	}
	void addSpray();
	void addFlames();
	virtual void doSomething();
	virtual bool setSocratesNFlame(int n) {
		m_nFlames = n;
		return true;
	}
	virtual int getHurtSound() {
		return SOUND_PLAYER_HURT;
	}

	virtual int getDieSound() {
		return SOUND_PLAYER_DIE;
	}


private:
	int m_nSprays;
	int m_nFlames;
	int m_posAngle;
	bool m_haveSprayed;
};

class Dirt :public Actor {
public:
	Dirt(StudentWorld* world, double startX, double startY, int imageID = IID_DIRT)
		:Actor(world, imageID, startX, startY,1,0,1) { //starting HP=1,direction=0, depth=1
	}

	virtual bool isDamageable() {
		return true;
	}
	virtual bool isBlockage() {
		return true;
	}
	virtual void doSomething() {
		return;
	}

};

class Food :public Actor {
public:
	Food(StudentWorld* world, double startX, double startY)
		:Actor(world, IID_FOOD, startX, startY, 1,90, 1, 1.0) { //starting HP=1, direction=90,depth=1
	}

	virtual bool isFood() {
		return true;
	}
	virtual void doSomething() {
		return;
	}

	
};

class Enemy;
class Ammo :public Actor {
public:
	Ammo(StudentWorld* world,double startX,double startY,int imageID,int range,Direction startMovDir,int attackValue)
		:Actor(world,imageID,startX,startY,1,startMovDir,1){ //hp=1
		m_range = range;
	}

	virtual ~Ammo() {}

	virtual int getDamage() = 0;

	bool doDamage();

	virtual void doSomething();


private:
	int m_range;
};

class Flame :public Ammo {
public: 
	Flame(StudentWorld* world, double startX, double startY, Direction startMovDir)
		:Ammo(world,startX,startY,IID_FLAME,FLAME_RANGE,startMovDir,FLAME_DAMAGE) {//range =32, damage as specified

	}
	virtual ~Flame() {}

	virtual int getDamage() {
		return FLAME_DAMAGE;
	}
};
class Spray :public Ammo {
public:
	Spray(StudentWorld* world, double startX, double startY, Direction startMovDir)
		:Ammo(world, startX, startY, IID_SPRAY, SPRAY_RANGE, startMovDir,SPRAY_DAMAGE) {//range =112, damage as specified

	}
	virtual ~Spray(){}
	virtual int getDamage() {
		return SPRAY_DAMAGE;
	}
};

class Enemy :public Actor {
public: 
	Enemy(StudentWorld* world, double startX, double startY, int imageID, int startHP,int attack)
		:Actor(world,imageID, startX, startY,startHP, 90) { //direction =90
		m_movPlanDis = 0;
		m_foodEatenCount = 0;
		m_attack = attack;
	}
	virtual bool isDamageable() {
		return true;
	}

	virtual bool isRequiredtoKill() {
		return true;
	}

	virtual void doSomething()=0;
	

	int getFoodEatenCount() {
		return m_foodEatenCount;
	}

	void clearFoodEaten() {
		m_foodEatenCount = 0;
	}

	int getAttack() {
		return m_attack;
	}

	void incrementFoodCount() {
		m_foodEatenCount++;
	}


	virtual int getScoreKilledByAmmo() {
		return SCORE_INCREASE_ENEMY_KILL;
	}

	//redefine the function to create food and increase score
	virtual void deathEffect();

	//attempt to attack the player everytick
	//do damage and return true if it overlaps with the player
	//do nothing and return false if it does not overlap with the player
	bool attackPlayer();

	//attempt to eat a food every tick
	//eat food and return true if it overlaps with a food
	//do nothing and return false if it does not overlap with a food
	bool eatFood();

	//return the new enemy pos divided by current enemy
	bool getDividePos(double& x,double& y);

	//attempt to move on current direction
	//return true if ok
	//return false if it will be blocked or out of scope
	bool attemptMove(double n);

	//pick a random direction and update movement plan
	void reDirection();

	//conduct the plan and return true if movement plan is greater than zero
	//otherwise return false
	bool doMovementPlan();

	virtual bool divide() = 0;

	void followOrRefreshPlan();

	

	


private:
	int m_movPlanDis;
	int m_foodEatenCount;
	int m_attack;
};

class RegEnemy :public Enemy {
public:
	RegEnemy(StudentWorld* world, double startX, double startY)
		:Enemy(world, startX, startY, IID_SALMONELLA, REGULAR_START_HP,REGULAR_ATTACK) {
	}

	virtual void doSomething();

	virtual int getHurtSound() {
		return SOUND_SALMONELLA_HURT;
	}

	virtual int getDieSound() {
		return SOUND_SALMONELLA_DIE;
	}
	
	virtual bool divide();
};

class AggEnemy :public Enemy {
public:
	AggEnemy(StudentWorld* world, double startX, double startY)
		:Enemy(world, startX, startY, IID_SALMONELLA, AGGRESSIVE_START_HP,AGGRESSIVE_ATTACK) {
		m_targetingPlayer = false;
	}
	bool isTargetingPlayer() {
		return m_targetingPlayer;
	}

	virtual void doSomething();

	virtual int getHurtSound() {
		return SOUND_SALMONELLA_HURT;
	}

	virtual int getDieSound() {
		return SOUND_SALMONELLA_DIE;
	}
	virtual bool divide();
private:
	bool m_targetingPlayer;
};

class EColi : public Enemy {
public:
	EColi(StudentWorld* world, double startX, double startY)
		:Enemy(world, startX, startY, IID_ECOLI, ECOLI_START_HP, ECOLI_ATTACK) {
	}

	virtual void doSomething();

	virtual int getHurtSound() {
		return SOUND_ECOLI_HURT;
	}

	virtual int getDieSound() {
		return SOUND_ECOLI_DIE;
	}
	virtual bool divide();

	//follow socrate specified by step 5
	void followPlayer();
};

class Goodie :public Actor {
public:
	Goodie(StudentWorld* world,double startX,double startY,int imageID,int lifetime)
		:Actor(world,imageID,startX,startY,1,0,1){//hp=1,dir=0,depth=1
		m_lifetime = lifetime;
	}
	virtual void doSomething();
	virtual void doEffect() = 0;

	virtual bool isDamageable() {
		return true;
	}
	
	virtual int getGoodieSound() {
		return SOUND_GOT_GOODIE;
	}
private:
	int m_lifetime;
};

class HPGoodie :public Goodie {
public:
	HPGoodie(StudentWorld* world, double startX, double startY,int lifetime)
		:Goodie(world, startX, startY, IID_RESTORE_HEALTH_GOODIE,lifetime) {

	}

	virtual void doEffect();
};

class ExLifeGoodie :public Goodie {
public:
	ExLifeGoodie(StudentWorld* world, double startX, double startY,int lifetime)
		:Goodie(world, startX, startY, IID_EXTRA_LIFE_GOODIE,lifetime) {

	}

	virtual void doEffect();
};

class FlameGoodie :public Goodie {
public:
	FlameGoodie(StudentWorld* world, double startX, double startY,int lifetime)
		:Goodie(world, startX, startY, IID_FLAME_THROWER_GOODIE,lifetime) {

	}

	virtual void doEffect();
};

class Fungus:public Goodie {
public:
	Fungus(StudentWorld* world, double startX, double startY,int lifetime)
		:Goodie(world, startX, startY, IID_FUNGUS,lifetime) {
	}

	virtual int getGoodieSound() {
		return SOUND_NONE;
	}
	virtual void doEffect();
	
};

class Pit :public Actor {
public:
	Pit(StudentWorld* world, double startX, double startY)
		:Actor(world,IID_PIT, startX, startY, 1, 0, 1) {//hp=1,dirction=0, depth=1
		m_inventory["Regular"] = 5;
		m_inventory["Aggresive"] = 3;
		m_inventory["Ecoli"] = 2;
	}
	virtual bool isRequiredtoKill() {
		return true;
	}
	virtual void doSomething();
	void addEnemy();
	bool isInventoryEmpty();
private:
	map<std::string, int> m_inventory;
};

#endif // ACTOR_H_
