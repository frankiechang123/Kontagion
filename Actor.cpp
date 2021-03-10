#include "Actor.h"
#include "StudentWorld.h"
#include<iostream>
#include<cmath>
using namespace std;

//helper function
double Distance(double x1, double y1, double x2, double y2) {
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}


// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

void Actor::decreaseHP(int value) {
	if (getHP() - value > 0) {
		setHP(getHP() - value);
		getWorld()->playSound(getHurtSound());
	}
	else {
		setHP(getHP() - value);
		getWorld()->playSound(getDieSound());
		deathEffect();
	}
}

void Socrates::doSomething() {
	if (!isActive()) return;
	
	int ch;
	if (getWorld()->getKey(ch)) {
		if (ch != KEY_PRESS_SPACE)
			m_haveSprayed = false;//turn the flag to be false if Socrates did not spray this tick
		
		switch (ch) {
		case KEY_PRESS_LEFT:

			moveAngle(180 + m_posAngle, VIEW_RADIUS);
			m_posAngle += 5;
			m_posAngle %= 360;
			moveAngle(m_posAngle, VIEW_RADIUS);
			setDirection(m_posAngle + 180);
			break;
		case KEY_PRESS_RIGHT:
			moveAngle(180 + m_posAngle, VIEW_RADIUS);
			m_posAngle -= 5;
			m_posAngle %= 360;
			moveAngle(m_posAngle, VIEW_RADIUS);
			setDirection(m_posAngle + 180);
			break;
		case KEY_PRESS_ESCAPE:
			break;
		case KEY_PRESS_ENTER:

			if (!(m_nFlames >= 1)) //break if no enough ammo
				break;
			addFlames();
			break;
		case KEY_PRESS_SPACE:
			if (!(m_nSprays >= 1)) {
				m_haveSprayed = false;
				break; //break if no enough ammo
			}
			addSpray();
			m_haveSprayed = true;
			break;

		default:
			break;
		}
	}
	else {
		//If the player did NOT press a key, then check if nSprays is maximum and whether it sprayed last tick
		if (!m_haveSprayed&&m_nSprays < 20) {
			m_nSprays++;
		}
		m_haveSprayed = false;
	}
}

//shoot a Spray
//play sound
//decrease ammo
void Socrates::addSpray() {

	getWorld()->addSpray(getX(), getY(), getDirection());
	m_nSprays--;
	
}

//shoot flames
//decrease ammo
void Socrates::addFlames() {

	getWorld()->addFlames(getX(),getY(),getDirection());
	m_nFlames--;
	
}


void Ammo::doSomething() {
	if (!isActive()) return;

	if (doDamage()) {
		setDead();
		return;
	}
	//Otherwise, the ammo will move forward in its current direction by SPRITE_RADIUS*2 pixels
	moveAngle(getDirection(), SPRITE_RADIUS * 2);
	m_range -= 2 * SPRITE_RADIUS;
	//If the ammo has moved a total pixels
	if (m_range <= 0)
		setDead();
	return;
	
}

//Ammo do Damage, return true if success
bool Ammo::doDamage() {
	return getWorld()->AmmoDoDamage(getX(), getY(), getDamage());
}

void Goodie::doSomething() {
	if (!isActive()) return;
	m_lifetime--;//decreaseLife eveytick
	if (getWorld()->GoodieCheckEaten(this)) {
		setDead();
		return;
	}
	if (m_lifetime <= 0) {
		setDead();
	}
}

void HPGoodie::doEffect() {
	//250 points
	getWorld()->increaseScore(SCORE_INCREASE_HPGOODIE);
	//setDead
	setDead();
	//play sound
	getWorld()->playSound(SOUND_GOT_GOODIE);
	//heal socrates
	getWorld()->restoreSocratesHP();
}

void ExLifeGoodie::doEffect() {
	//500 points
	getWorld()->increaseScore(SCORE_INCREASE_EXLIFEGOODIE);
	//setdead
	setDead();
	//play sound
	getWorld()->playSound(SOUND_GOT_GOODIE);
	//increaseLife
	getWorld()->incLives();
}
void FlameGoodie::doEffect() {
	
	//300 points
	getWorld()->increaseScore(SCORE_INCREASE_FLAMEGOODIE);
	//setdead
	setDead();
	//play sound
	getWorld()->playSound(SOUND_GOT_GOODIE);
	//increase socrates flame
	getWorld()->increaseSocrateFlameAmmo(NFLAME_INCREASE_FLAMEGOODIE);
}
void Fungus::doEffect() {
	
	//-50Point
	getWorld()->increaseScore(SCORE_INCREASE_FUNGUS);
	//setDead
	setDead();
	//deacrease socrates HP by 20
	getWorld()->decreaseSocratesHP(HP_DECREASE_FUNGUS);
}

//Pit addEnemy every tick
void Pit::addEnemy() {
	int ranInt = randInt(1, 50);
	if (ranInt == 1) {
		//erase empty inventory from map
		for (auto it = m_inventory.begin(); it != m_inventory.end();) {
			if (it->second == 0)
				it = m_inventory.erase(it);
			else
				it++;
		}
		if (m_inventory.size() == 0)
		{
			return;
		}
		int ranInt2 = randInt(0, m_inventory.size()-1);
		auto it = m_inventory.begin();
		for (int t = 0; t < ranInt2; t++) {
			it++;
		}
		string type = it->first;

		if (type == "Regular") {
			getWorld()->addRegEnemy(getX(),getY());
			m_inventory["Regular"]--;
		}
		else if (type == "Aggresive") {
			getWorld()->addAggEnemy(getX(), getY());
			m_inventory["Aggresive"]--;
		}
		else {
			getWorld()->addEColi(getX(),getY());
			m_inventory["Ecoli"]--;
		}
		getWorld()->playSound(SOUND_BACTERIUM_BORN);
	
	}
}

//return if the inventory is Empty
bool Pit::isInventoryEmpty() {
	for (auto it = m_inventory.begin(); it != m_inventory.end(); it++) {
		if (it->second != 0)
			return false;
	}
	return true;
}

void Pit::doSomething() {
	if (isInventoryEmpty())
		setDead();
	addEnemy();
}


void Enemy::deathEffect()
{
	getWorld()->increaseScore(getScoreKilledByAmmo());
	int ranInt = randInt(0, 1);
	if (ranInt == 0) {
		getWorld()->addFood(getX(), getY());
	}
}

bool Enemy::attackPlayer() {
	if (getWorld()->EnemyDoDamage(this))
		return true;
	return false;
}

bool Enemy::eatFood() {
	
	if (getWorld()->EnemyEatFood(this))
		return true;
	return false;
}

bool Enemy::getDividePos(double& x, double& y) {
	x = getX(); y = getY();
	if (x < VIEW_WIDTH / 2) {
		x += SPRITE_RADIUS;
	}
	else {                        //?????WHAT IF x,y == VIEW_RADIUS
		if(x>VIEW_WIDTH/2)
			x -= SPRITE_RADIUS;
	}

	if (y < VIEW_HEIGHT / 2) {
		y += SPRITE_RADIUS;
	}
	else {
		if (y > VIEW_HEIGHT / 2) {
			y -= SPRITE_RADIUS;
		}
	}
	return true;
}

bool Enemy::attemptMove(double n) {
	//attempt to forward n pixels
	double newX;
	double newY;
	getPositionInThisDirection(getDirection(), n, newX, newY);

	if (getWorld()->hasBlockage(newX, newY, MOVEMENT_OVERLAP_RANGE))
		return false;
	if (Distance(VIEW_WIDTH / 2, VIEW_HEIGHT / 2, newX, newY) >= VIEW_RADIUS)
		return false;
	return true;
}

void Enemy::reDirection() {
	int newDir = randInt(0, 359);
	setDirection(newDir);
	m_movPlanDis = 10;
}

//if movement plan distance is equal to zero, return false
//otherwise: decrement the movement plan by one and attempt to move
//if succussful: move the Enemy and return true
//if unsuccussful: get random direction and refresh the plan and return true
bool Enemy::doMovementPlan() {
	if (m_movPlanDis <= 0)
		return false;
	m_movPlanDis--;
	if (attemptMove(3)) { //attempt to move 3 pixels
		moveAngle(getDirection(), 3);
	}
	else {
		reDirection();
	}
	return true;
}


void Enemy::followOrRefreshPlan() {
	if (doMovementPlan()) {
		return;
	}
	else {
		Direction newDir = getWorld()->FindFood(this, 128);
		if (newDir == -1) {//if food not found
			reDirection(); //refresh plan
			return;
		}
		else {
			setDirection(newDir); //set direction to food
			m_movPlanDis = 100; //go straight looking for food
			if (!attemptMove(3)) { //if blocked when moving 3 pixels
				reDirection(); //refresh plan
				return;
			}
			return;
		}
	}
}

bool RegEnemy::divide() {
	if (getFoodEatenCount() != 3) 
		return false;

	double newX, newY;
	getDividePos(newX, newY);
	getWorld()->addRegEnemy(newX,newY);
	clearFoodEaten();
	return true;
}


void RegEnemy::doSomething() {
	if (!isActive()) return;

	//attempt to attack the player everytick
	if (attackPlayer() || divide() || eatFood()) {
		followOrRefreshPlan();
	}
	else {
		followOrRefreshPlan();
	}

}

void AggEnemy::doSomething() {
	//check if alive
	if (!isActive()) return;

	//find socrate
	Direction DirToSocrate = getWorld()->FindSocrates(this,72);//search range =72
	if (DirToSocrate != -1 && DirToSocrate != -2) {
		m_targetingPlayer = true;
		setDirection(DirToSocrate);
		if (attemptMove(3)) //attempt to move 3 pixels
			moveAngle(DirToSocrate, 3);
	}
	else {
		m_targetingPlayer = false;
	}

	if (attackPlayer() || divide() || eatFood()) {
		if (m_targetingPlayer)
			return;
		else {
			followOrRefreshPlan();
			return;
		}
	}
	else {
		if (m_targetingPlayer)
			return;
		else {
			followOrRefreshPlan();
			return;
		}
	}

}

bool AggEnemy::divide() {
	if (getFoodEatenCount() != 3)
		return false;

		double newX, newY;
		getDividePos(newX, newY);
		getWorld()->addAggEnemy(newX,newY);
		clearFoodEaten();
		return true;
}

void EColi::doSomething() {
	if (!isActive()) return;

	if (attackPlayer() || divide() || eatFood()) {
		followPlayer();
		return;
	}
	followPlayer();
}

bool EColi::divide() {
	if (getFoodEatenCount() != 3)
		return false;

	double newX, newY;
	getDividePos(newX, newY);
	getWorld()->addEColi(newX, newY);
	clearFoodEaten();
	return true;
}

void EColi::followPlayer() {
	Direction newDir = getWorld()->FindSocrates(this, 256);
	if (newDir != -1 && newDir != -2) {
		setDirection(newDir);
		for (int t = 0; t < 10; t++) {
			if (attemptMove(2)) { //atttemp to move 2 pixels
				moveAngle(getDirection(), 2);
				return;
			}
			else { //adjust angle
				setDirection(getDirection() + 10);
			}
		}
		setDirection(newDir);//return to the former direction if 10 times of attempts don't work

	}
}