#include "StudentWorld.h"
#include "GameConstants.h"
#include"Actor.h"
#include <string>
#include <iostream>
#include<algorithm>
#include<sstream>
#include<iomanip>
#include<cmath>

using namespace std;

//Helper Functions
void getViewPos(int& x,int&y);



GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}


StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
	m_socrate = nullptr;
}

//getLifetime
int StudentWorld::getGoodieLifetime() {
	return max(rand() % (300 - 10 * getLevel()), 50);
}
void StudentWorld::addFood(double x,double y) {
	Food* newFood = new Food(this, x, y);
	m_actors.push_back(newFood);
}

void StudentWorld::addSpray(double x, double y,Direction startMovDir) {
	Spray* newSpray = new Spray(this, x, y, startMovDir);
	m_actors.push_back(newSpray);
	playSound(SOUND_PLAYER_SPRAY);
}

void StudentWorld::addFlames(double x, double y,Direction startMovDir) {
	Flame* newFlame;
	for (int t = 0; t < 16; t++) {
		newFlame = new Flame(this, x, y, startMovDir + t * 22);
		m_actors.push_back(newFlame);
	}
	playSound(SOUND_PLAYER_FIRE);
}

void StudentWorld::addRegEnemy(double x, double y) {
	RegEnemy* newRegEnemy= new RegEnemy(this, x, y);
	m_actors.push_back(newRegEnemy);
}

void StudentWorld::addAggEnemy(double x, double y) {
	AggEnemy* newAggEnemy = new AggEnemy(this, x, y);
	m_actors.push_back(newAggEnemy);
}

void StudentWorld::addEColi(double x, double y) {
	EColi* newEColi = new EColi(this, x, y);
	m_actors.push_back(newEColi);
}

void StudentWorld::addGoodies() {
	int chanceGoodie = min(510 - getLevel() * 10, 250);
	int chanceFungus = min(510 - getLevel() * 10, 200);
	int ranIntGoodie = randInt(0, chanceGoodie-1);   //randint includsive???
	int ranIntGungus = randInt(0, chanceFungus - 1);
	//getLifetime
	int lifetime = getGoodieLifetime();

	//check goodies
	if (ranIntGoodie == 0) {
		//get random goodie type
		int randInt2 = randInt(1, 10);
		//get Random positioning angle
		int ranPosAng = randInt(0, 359);

		//10% extra life
		if (randInt2 <= 1) {
			ExLifeGoodie* newGoodie = new ExLifeGoodie(this, VIEW_RADIUS, VIEW_RADIUS, lifetime);
			//set positioning Angle
			newGoodie->moveAngle(ranPosAng, VIEW_RADIUS);
			m_actors.push_back(newGoodie);
		}
		//30% Flame Goodie
		else if (randInt2 <= 4) {
			FlameGoodie* newGoodie = new FlameGoodie(this, VIEW_RADIUS, VIEW_RADIUS, lifetime);
			newGoodie->moveAngle(ranPosAng, VIEW_RADIUS);
			m_actors.push_back(newGoodie);
		}
		//60% health goodie
		else {
			HPGoodie* newGoodie = new HPGoodie(this, VIEW_RADIUS, VIEW_RADIUS, lifetime);
			newGoodie->moveAngle(ranPosAng, VIEW_RADIUS);
			m_actors.push_back(newGoodie);
		}
	}

	//check Fungus
	if (ranIntGungus == 0) {
		int ranPosAng = randInt(0, 359);
		Fungus* newFungus = new Fungus(this, VIEW_RADIUS, VIEW_RADIUS, lifetime);
		newFungus->moveAngle(ranPosAng, VIEW_RADIUS);
		m_actors.push_back(newFungus);
	}

}

/*Helper Functions*/
double getDistance(Actor* a, Actor* b) {
	return sqrt(pow(a->getX() - b->getX(), 2) + pow(a->getY() - b->getY(), 2));
}

double getDistance(double aX, double aY, double bX, double bY) {
	return sqrt(pow(aX - bX, 2) + pow(aY - bY, 2));
}

bool overlaps(Actor* a, Actor* b, double range) {
	if (a == b) return false;
	return (getDistance(a, b) <= range);
}

bool overlaps(double aX, double aY, Actor* b, double range) {
	return (getDistance(aX, aY, b->getX(), b->getY())<=range);
}

Direction getDirectionToThisPos(double sourceX, double sourceY, double destX, double destY) {
	const double PI = 4 * atan(1);
	Direction res = atan((destY - sourceY) / (destX - sourceX)) * (180 / PI);
	if (destX < sourceX)
		res += 180;
	return res;
}


//do damage return true if the agent is in contact with a damageable object
//return false otherwise
bool StudentWorld::AmmoDoDamage(double x,double y,int damage) {
	for (auto it = m_actors.begin(); it != m_actors.end(); it++) {
		if (overlaps(x, y, (*it), HARM_PICKUP_RANGE)&&(*it)->isActive()) {
			if ((*it)->isDamageable()) {
				(*it)->decreaseHP(damage);
				return true;
			}
		}
	}
	return false;
}

//check if agent overlaps with socrates
//do damage and return true if success
//return false otherwise
bool StudentWorld::EnemyDoDamage(Enemy* agent) {
	if (overlaps(agent, m_socrate, HARM_PICKUP_RANGE) && agent->isActive()) {
		m_socrate->decreaseHP(agent->getAttack());
		return true;
	}
	return false;
}
//Goodies check eaten, do effect and return true if successfull
//return false if there is no overlap
bool StudentWorld::GoodieCheckEaten(Goodie* agent) {
	if (overlaps(agent, m_socrate, HARM_PICKUP_RANGE)&&agent->isActive()) {
		agent->doEffect();
		return true;
	}
	return false;
}
//Enemies attempt to eat food
//return true is successfull
//return false if there is no overlap
bool StudentWorld::EnemyEatFood(Enemy* agent) {
	for (auto it = m_actors.begin(); it != m_actors.end(); it++) {
		if ((*it)->isActive() && (*it)->isFood()) {
			if (overlaps(agent, *it, HARM_PICKUP_RANGE)) {
				agent->incrementFoodCount();
				(*it)->setDead();
				return true;
			}
		}
	}
	return false;
}

bool StudentWorld::hasBlockage(double x,double y,double range) {
	for (auto it = m_actors.begin(); it != m_actors.end(); it++) {
		if ((*it)->isActive() && (*it)->isBlockage()) {
			if (overlaps(x, y, *it, range))
				return true;
		}
	}
	return false;
}

//Find closest food in range
//return -1 if not found
Direction StudentWorld::FindFood(Enemy* agent, double range) {
	double minDistance = -1;
	Direction res = -1;
	for (auto it = m_actors.begin(); it != m_actors.end(); it++) {
		if ((*it)->isActive() && (*it)->isFood()) {
			if (overlaps(agent, *it, range)) {
				double temp = getDistance(agent, *it);
				if (minDistance == -1) {
					minDistance = temp;
					res = getDirectionToThisPos(agent->getX(), agent->getY(), (*it)->getX(), (*it)->getY());
				}
				else {
					if (minDistance > temp) {
						minDistance = temp;
						res = getDirectionToThisPos(agent->getX(), agent->getY(), (*it)->getX(), (*it)->getY());
					}
				}
					
			}
		}
	}
	return res;
}

//Find Socrates in range
//return -1 if not found
Direction StudentWorld::FindSocrates(Enemy* agent, double range) {
	if (overlaps(agent, m_socrate, range)) {
		return getDirectionToThisPos(agent->getX(), agent->getY(), m_socrate->getX(), m_socrate->getY());
	}
	else
		return -1;
}

int StudentWorld::init()
{	
	////Add Socrates

	m_socrate = new Socrates(this);

	//Add Pits
	list<Actor*> newPitList;
	for (int i = 0; i < getLevel(); i++) {
		int ranX, ranY;
		getViewPos(ranX, ranY);
		Pit* newPit = new Pit(this, ranX, ranY);
		checkNoOverLap(newPit, newPitList, this);
		newPitList.push_back(newPit);
	}
	m_actors.insert(m_actors.begin(), newPitList.begin(), newPitList.end());

	//add Food
	list<Actor*> newFoodList;
	for (int i = 0; i < min(5 * getLevel(), 25); i++) {
		int ranX, ranY;
		getViewPos(ranX, ranY);
		Food* newFood = new Food(this, ranX, ranY);
		checkNoOverLap(newFood, newFoodList,this);
		checkNoOverLap(newFood, newPitList, this);
		newFoodList.push_back(newFood);
	}
	m_actors.insert(m_actors.begin(), newFoodList.begin(), newFoodList.end());

	//add Dirt
	for (int i = 0; i < max(180 - 20 * getLevel(), 20); i++) {
		int ranX, ranY;
		getViewPos(ranX, ranY);
		Dirt* newDirt = new Dirt(this, ranX, ranY);
		checkNoOverLap(newDirt, newFoodList,this);
		checkNoOverLap(newDirt, newPitList, this);
		m_actors.push_back(newDirt);
	}

    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
	if(m_socrate->isActive())
		m_socrate->doSomething();
   
	for (auto it = m_actors.begin(); it != m_actors.end(); it++) {
		if((*it)->isActive())
			(*it)->doSomething();
}

	//check death
	if (!m_socrate->isActive()) {
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}

	//deleting inActive actors (excluding socrates)
	for (auto it = m_actors.begin(); it != m_actors.end();) {
		if (!(*it)->isActive()) {
			delete *it;
			it = m_actors.erase(it);
		}
		else {
			it++;
		}
	}

	//add goodies
	addGoodies();

	//set Text
	setGameStatText(getText());

	//check level up
	if (checkLevelUp()) {
		playSound(SOUND_FINISHED_LEVEL);
		return GWSTATUS_FINISHED_LEVEL;
	}

	
    return  GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	for (list<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++) {
		delete *it;
	}
	if (m_socrate != nullptr) {
		delete m_socrate;
		m_socrate = nullptr;
	}
	while (!m_actors.empty()) {
		m_actors.pop_back();
	}

}

//return a pair of random(x,y) that is inside the circle
//distance from center is at most 120
void getViewPos(int& x, int& y) {
	int ranY = randInt(0, VIEW_HEIGHT);
	int ranX = randInt(0, VIEW_WIDTH);
	while (sqrt(pow(ranX - VIEW_WIDTH / 2, 2) + pow(ranY - VIEW_HEIGHT / 2, 2)) >= 120) {
		ranY = randInt(0, VIEW_HEIGHT);
		ranX = randInt(0, VIEW_WIDTH);
	}
	x = ranX;
	y = ranY;
}

bool StudentWorld::checkLevelUp() {
	for (auto it = m_actors.begin(); it != m_actors.end(); it++) {
		if ((*it)->isRequiredtoKill())
			return false;
	}
	return true;
}

//get Statistics texts
string StudentWorld::getText() {
	ostringstream oss;
	int score = getScore();
	int nSprays, nFlames;
	m_socrate->getSocratesStat(nSprays, nFlames);
	oss.fill('0');
	if (score >= 0) {
		oss.fill('0');
		oss << setw(6) << score;
	}
	else {
		oss << '-';
		oss << setw(5) << -score;
	}
	string scoreStr = oss.str();
	string res= "Score: " + scoreStr + 
				"  Level: " + to_string(getLevel()) + 
				"  Lives: " + to_string(getLives()) + 
				"  Health: " + to_string(m_socrate->getHP()) + 
				"  Sprays: " + to_string(nSprays) + 
				"  Flames: " + to_string(nFlames);
	return res;
}