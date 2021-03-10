#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <map>
#include<list>
#include"Actor.h"


bool overlaps(Actor* a, Actor* b, double range);
void getViewPos(int& x, int& y);
//template function
//check and modify so that the imput element doesn't overlap with any element in the list
template<class ActorItem>
void  checkNoOverLap(ActorItem* &curr, list<Actor*> list,StudentWorld* world) {
	//while loop --> check if curr overlaps with others
	auto it = list.begin();
	while (it != list.end()) {
		if (overlaps(curr,*it,SPRITE_WIDTH)) {
			int ranX, ranY;
			delete curr;
			getViewPos(ranX, ranY);
			curr = new ActorItem(world, ranX, ranY);
			it = list.begin();
		}
		else {
			it++;
		}
	}
}

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();

	void restoreSocratesHP() {
		m_socrate->setHP(SOCRATES_HP);
	}

	void decreaseSocratesHP(int value) {
		m_socrate->decreaseHP(value);
	}

	void increaseSocrateFlameAmmo(int value) {
		int nSprays, nFlames;
		m_socrate->getSocratesStat(nSprays, nFlames);
		m_socrate->setSocratesNFlame(nFlames + value);
	}

	void addFood(double x,double y);
	void addSpray(double x, double y,Direction startMovDir);
	void addFlames(double x, double y,Direction startMovDir);
	void addRegEnemy(double x, double y);
	void addAggEnemy(double x, double y);
	void addEColi(double x, double y);

	
	bool AmmoDoDamage(double x, double y,int damage);
	bool GoodieCheckEaten(Goodie* agent);
	bool EnemyDoDamage(Enemy* agent);
	bool EnemyEatFood(Enemy* agent);

	bool hasBlockage(double x, double y, double range);
	Direction FindFood(Enemy* agent, double range);
	Direction FindSocrates(Enemy* agent, double range);

	~StudentWorld() {
		cleanUp();
	}
	////return the pointer to the container
	//std::list<Actor*>* getActors() {
	//	return &m_actors;
	//}

	//Actor* getSocrate() {
	//	return m_socrate;
	//}

	//Every tick, attecmpt to add good goodies and fungus to the world
	void addGoodies();

	int getGoodieLifetime();

	bool checkLevelUp();

	std::string getText();

private:
	std::list<Actor*> m_actors;
	Actor* m_socrate;
};



#endif // STUDENTWORLD_H_
