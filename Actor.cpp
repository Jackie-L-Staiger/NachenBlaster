// Actor Implementations

#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>
#include <iostream>
using namespace std;


// Implementations of Member Functions for Actors 

// Virtual fuction-- used for goodies to test if flown off screen
bool Actor::testAlive() 
{
	if (!isAlive())
		return 0;
	// If Actor has flown off screen (in southwest direction), set as dead so can be removed
	if (getX() < 0 || getY() < 0)
	{
		setToDead();
		return 0;
	}
	return 1;
}

void Star::doSomething()
{
	// Move star to the left by one space (if possible)
	if (getX() > -1)
		moveTo(getX() - 1, getY());
	else
	{
		setToDead();
		// Add another star to game to replace dead star 
		getStudentWorld()->addActor(1, this);                               
	}
}

int Explosion::update_count(int num)
{
	return m_count += num;
}

void Explosion::doSomething()
{
	update_count(1); 
	// Increase by factor of 1.5
	setSize(1.5*getSize());
	// After exactly 4 ticks, must set status to dead and reset count 
	if (tick_count() == 4)
	{
		setToDead();
		update_count(0);
	}
}

// For NachenBlaster:
void NachenBlaster::sufferDamage(double amount)                               
{
	update_hitpoints(amount);
	getStudentWorld()->playSound(SOUND_BLAST);
	// If NachenBlaster dies, setToDead
	if (hitpoints() <= 0)                                                
	{
		setToDead();
	}
}

double NachenBlaster::update_hitpoints(double amount)
{
	// If amount can be added (hitpoints may not exceed 50), increase hitpoints accordingly 
	if (hitpoints() + amount <= 50 && hitpoints() - amount >= 0)
		return DamageableObject::update_hitpoints(amount);
	// If amount cannot be added, keep current score 
	else
		return hitpoints();
}

void NachenBlaster::doSomething()
{
	if (!isAlive())
		return;
	int key;
	// If user input available,                                
	if (getStudentWorld()->getKey(key))
	{
		// user hit a key during tick
		switch (key)
		{
			// If user pressed the UP key, increase y location by 1
		case KEY_PRESS_UP:
			if (getY() < VIEW_HEIGHT - 1)
				moveTo(getX(), getY() + 6);
			break;
			// If user pressed the DOWN key, decrease y location by 1
		case KEY_PRESS_DOWN:
			if (getY() > 0)
				moveTo(getX(), getY() - 6);
			break;
			// If user pressed the LEFT key, decrease x location by 1
		case KEY_PRESS_LEFT:
			if (getX() > 0)
				moveTo(getX() - 6, getY());
			break;
			// If user pressed the RIGHT key, increase x location by 1
		case KEY_PRESS_RIGHT:
			if (getX() < VIEW_WIDTH - 1)
				moveTo(getX() + 6, getY());
			break;
			// If user pressed the SPACE BAR to fire and the NachenBlaster has at least 5 cabbage energy points,
		case KEY_PRESS_SPACE:
			if (num_cabbages() >= 5)
			{
				// Add cabbage:
				getStudentWorld()->addActor(6, this);
				// Play sound:
				getStudentWorld()->playSound(SOUND_PLAYER_SHOOT);
				// Decrement number of cabbages by 5
				update_cabbages(5);
			}
			break;
			// If user pressed the TAB KEY to fire and the NachenBlaster has at least 1 torpedo,
		case KEY_PRESS_TAB:
			if (num_torpedos() > 0)
			{
				// Add torpedo:
				getStudentWorld()->addActor(8, this);
				// Play sound:
				getStudentWorld()->playSound(SOUND_TORPEDO);
				// Decrement number of torpedos by 1
				update_torpedos(-1);
			}
		}
	}
	// Replenish NachenBlaster cabbages:
	if (num_cabbages() < 30)
		update_cabbages(1);
	// Collision: 
	/*Actor* actor = getStudentWorld()->getOneCollidingPlayer(this);
	if (actor != nullptr)
		sufferDamage(-2);  */                                    
}

int NachenBlaster::update_cabbages(int num)                  
{
	switch (num)
	{
	case 1:
		return m_cabbages = m_cabbages + 1; break;
	case 5:
		return m_cabbages = m_cabbages - 5; break;
	default:
		return m_cabbages; break;
	}
}

// Aliens: 

void Alien::sufferDamageHelper(NachenBlaster* NB_ptr)
{
	// Update player's score
	if (alienNum() == 1 || alienNum() == 2)
		NB_ptr->getStudentWorld()->increaseScore(250);
	else
		NB_ptr->getStudentWorld()->increaseScore(1000);
	// Set Smallgon to dead
	setToDead();
	// Increment number of aliens destroyed
	getStudentWorld()->update_num_aliens_destroyed();
	// Play sound:
	getStudentWorld()->playSound(SOUND_DEATH);
	// Create explosion object
	getStudentWorld()->addActor(2, this);
	// If Smoregon or Snagglegon, check to see if drops goody
	if (alienNum() == 2 || alienNum() == 3) // virtual!
		possiblyDropGoody();
}

void Alien::sufferDamage(double amount, Alien* alien)  // re-defined 
{
	// Test if Alien collided with NachenBlaster
	int num = 0;
	NachenBlaster* NB_ptr = getStudentWorld()->getCollidingPlayer(this, num);
	// If Alien collided with NachenBlaster,
	if (num == 1)
	{
		// Damages NachenBlaster by causing it to lose 5 or 15 hitpoints  
		NB_ptr->update_hitpoints(amount);
		sufferDamageHelper(NB_ptr);
	}
	// If Alien gets hit by NachenBlaster projectile (but Smallgon not necessarily destroyed),
	else if (alienCollidedCount() > 0) 
	{
		// Decrement hitpoints by 2 from cabbages
		update_hitpoints(-2);
		// If Alien gets hit by projectile and is destroyed,
		if (hitpoints() <= 0)
		{
			sufferDamageHelper(NB_ptr);
		}
		// If Alien gets hit, but is not destroyed,
		else
			getStudentWorld()->playSound(SOUND_BLAST);
	}
	// Reset m_alienCollided()
	increaseAlienCollided(0);        
}

// Move Aliens
void Alien::move(int alien_num)
{
	// Flight direction and length: 
	// For all Aliens: 
	if (flightLength() == 0 || (getX() == VIEW_WIDTH || getY() == VIEW_HEIGHT))
	{
		if (getY() >= VIEW_HEIGHT - 1)
			update_flight_direction(6);
		else if (getY() <= 0)
			update_flight_direction(8);
		// For only Smallgons and Smoregons:
		else if ((flightLength() == 0) && alienNum() != 3)
		{
			int num = randInt(6, 8);
			update_flight_direction(num);
		}
		// Randomly pick flight length 
		update_flight_length(randInt(1, 32));
		//cout << flightLength() << endl;
	}
	// Possibly Fire turnips/torpedos:
	int num = 0;
	NachenBlaster* NB_ptr = getStudentWorld()->getCollidingPlayer(this, num);
	// If NB is to the left of Alien and Alien has y coordinate within [-4,4] pixels of NB,              
	if ((NB_ptr->getX() < this->getX()) && (((this->getY() - 4) == NB_ptr->getY()) || ((this->getY() + 4) == NB_ptr->getY()) || ((this->getY() - 3) == NB_ptr->getY()) || (this->getY() + 3) == NB_ptr->getY() ||
		(this->getY() - 2) == NB_ptr->getY() || (this->getY() + 2) == NB_ptr->getY() || (this->getY() - 4) == NB_ptr->getY() || (this->getY() + 4) == NB_ptr->getY()))
	{
		if (alienNum() != 3)
		{
			// Chance of firing turnip:
			int num1 = randInt(1, ((20 / getStudentWorld()->getLevel()) + 5));                                  // changed successfuly 
			if (num1 == 1)
			{
				// Fire turnip:
				getStudentWorld()->addActor(7, this);
				getStudentWorld()->playSound(SOUND_ALIEN_SHOOT);
				return;
			}
			// Chance that Smoregon will change path:
			if (alien_num == 2)
			{
				int num2 = randInt(1, ((20 / getStudentWorld()->getLevel()) + 5));                             // changed successfully 
				if (num2 != 1)
				{
					update_flight_direction(7);
					update_flight_length(VIEW_WIDTH);
					update_speed(5);
				}
			}
		}
		else
		{
			// Chance of firing torpedo:
			int num3 = randInt(1, ((15 / getStudentWorld()->getLevel()) + 10));     
			if (num3 == 1)
			{
				// Fire torpedo:
				getStudentWorld()->addActor(9, this);
				getStudentWorld()->playSound(SOUND_TORPEDO);
				return;
			}
		}
	}
	// Alien takes one step in flight plan
	travelDirection(flightDirection(), this);
}


bool Alien::testAlive()
{
	if (!isAlive())
		return 0;
	// If Alien has flown off screen, set as dead so can be removed
	if (getX() < 0)
	{
		setToDead();
		// Decrement/Update number of aliens on the screen 
		getStudentWorld()->update_num_aliens(-1);
		return 0;
	}
	return 1;
}

// Takes fight direction as first parameter and pointer to Alien as second 
void Alien::travelDirection(int num, Alien* a)
{
	switch (num)
	{
	case 6:
		moveTo(getX() - speed(), getY() - speed());  break;
	case 7:
		moveTo(getX() - speed(), getY());  break;
	case 8:
		moveTo(getX() - speed(), getY() + speed());  break;
	default:
		moveTo(getX() - speed(), getY() + speed());  break;
	}
	update_flight_direction(num);
	// Reduce flight plan length by 1
	if (flightLength() > 0)
		update_flight_length(-1);
}

double Smallgon::calculate_start_hitpoints()                                               
{
	return update_hitpoints(5 * (1 + (getStudentWorld()->getLevel() - 1)* 0.1));
}

// (alien_num = 1)
void Smallgon::doSomething()
{
	if (testAlive() == 0)
		return;
	// Test if Smallgon collided with either NachenBlaster or projectile (and suffered damage)
	sufferDamage(-5, this);
	// Move Alien with move function 
	move(alienNum());
	// Test if Smallgon collided with either NachenBlaster or projectile (and suffered damage)
	sufferDamage(-5, this);
}

double Smoregon::calculate_start_hitpoints()                                             
{
	return update_hitpoints(5 * (1 + (getStudentWorld()->getLevel() - 1)* 0.1));
}

// (alien_num = 2)
void Smoregon::doSomething()
{
	if (testAlive() == 0)
		return;
	// Test if Smoregon collided with either NachenBlaster or projectile (and suffered damage)
	sufferDamage(-5, this);
	// Move Alien with move function 
	move(alienNum());
	// Test if Smoregon collided with either NachenBlaster or projectile (and suffered damage)
	sufferDamage(-5, this);
}

// For Smoregons, possibly drop goody:
void Smoregon::possiblyDropGoody()
{
	int rand_num = randInt(1, 3);
	if (rand_num == 1)
	{
		int rand_num2 = randInt(1, 2);
		if (rand_num2 == 1)
			getStudentWorld()->addActor(11, this); // drop Repair goody
		else
			getStudentWorld()->addActor(12, this); // drop Torpedo goody 
	}
}

// (alien_num = 3)
void Snagglegon::doSomething()
{
	if (testAlive() == 0)
		return;
	// Test if Snagglegon collided with either NachenBlaster or projectile (and suffered damage)
	sufferDamage(-15, this);
	// Move Alien with move function 
	move(alienNum());
	// Test if Snagglegon collided with either NachenBlaster or projectile (and suffered damage)
	sufferDamage(-15, this);

}

void Snagglegon::possiblyDropGoody()                                                           
{
	int rand_num = randInt(1, 6);
	if (rand_num == 1)
		getStudentWorld()->addActor(10, this); // drop ExtraLife goody

}

double Snagglegon::calculate_start_hitpoints()
{
	return update_hitpoints(10 * (1 + (getStudentWorld()->getLevel() - 1)* 0.1));
}

// Goodies

void Goody::goodyHelper(Goody* goody, NachenBlaster* NB_ptr)
{
	getStudentWorld()->increaseScore(100);
	setToDead();
	getStudentWorld()->playSound(SOUND_GOODIE);
	// For Repair goody:
	if (goodyNum() == 1)
		getStudentWorld()->incLives();
	else if (goodyNum() == 2)
		NB_ptr->update_hitpoints(10);
	else
		NB_ptr->update_torpedos(5);
}

// Used for Repair and Torpedo goodies 
void Goody::goodyTick(Goody* goody)
{
	// Check to see is alive and not flown off screen
	if (testAlive() == 0)
		return;
	int num = 0;
	// Check to see if collided with NachenBlaster 
	NachenBlaster* NB_ptr = getStudentWorld()->getCollidingPlayer(this, num);
	if (num == 1)
	{
		goodyHelper(this, NB_ptr);
		return;
	}
	num = 0;
	// Animate the goody: 
	moveTo(getX() - 0.75, getY() - 0.75);
	// Check to see if collided with NachenBlaster again
	getStudentWorld()->getCollidingPlayer(this, num);
	if (num == 1)
	{
		goodyHelper(this, NB_ptr);
		return;
	}
}

// (goody_num = 1)
void ExtraLife::doSomething()
{
	goodyTick(this);            
}

// (goody_num = 2)
void Repair::doSomething()
{
	goodyTick(this);
}

// (goody_num = 3)
void Torpedo_Goody::doSomething()
{
	goodyTick(this);
}

// Projectiles:

bool Projectile::testAlive()
{
	if (!isAlive())
		return 0;
	// Delete projectiles that fly off either side of the screen 
	if (getX() < 0 || getX() >= VIEW_WIDTH)
	{
		setToDead();
		return 0;
	}
	return 1;
}

void Projectile::projectile_NB_helper(Projectile* projectile, Actor* alien)
{
	if (alien != nullptr)
	{
		// Downcast from Actor to Alien 
		Alien* a1 = static_cast<Alien*>(alien);
		if (projectile_num() == 2)
			a1->sufferDamage(-2, a1);
		else // projectile_num = 3
			a1->sufferDamage(-8, a1);
		a1->increaseAlienCollided(1);
		setToDead();
	}
}

// Returns 1 if collided or 0 if did not 
int Projectile::projectile_alien_helper(Projectile* projectile)
{
	int num = 0;
	// (will always return valid pointer to NachenBlaster, so no need to check nullptr)
	NachenBlaster* NB_ptr = getStudentWorld()->getCollidingPlayer(this, num);
	// If Turnip collided with NachenBlaster,
	if (num == 1)
	{
		// Turnip damages NachenBlaster by causing it to lose 2 hitpoints 
		if (projectile_num() == 1)
			NB_ptr->sufferDamage(-2);
		// Alien torpedo damages NachenBlaster by causing it to lose 8 hitpoints 
		else
			NB_ptr->sufferDamage(-8);
		setToDead();
		return 1;
	}
	return 0;
}

void Projectile::firedByNachenBlaster(Projectile*)
{
	if (testAlive() == 0)
		return;
	// Check to see if cabbage collided with alien ship (if not nullptr, collision occured)
	Actor* alien = getStudentWorld()->getOneCollidingAlien(this);
	if (alien != nullptr)
	{
		projectile_NB_helper(this, alien);
		return;
	}
	// Animate the cabbage/torpedo: 
	moveTo(getX() + 8, getY());
	if (projectile_num() == 2)
		setDirection(getDirection() + 20);
	// Check to see if cabbage collided with alien ship (if not nullptr, collision occured)
	Actor* alien2 = getStudentWorld()->getOneCollidingAlien(this);
	if (alien2 != nullptr)
	{
		projectile_NB_helper(this, alien2);
		return;
	}
}

void Projectile::firedByAlien(Projectile*)
{
	if (testAlive() == 0)
		return;
	if (projectile_alien_helper(this) == 1)
		return;
	// Animate the Turnip/Torpedo: 
	moveTo(getX() - 6, getY());
	// Rotate turnip:
	if (projectile_num() == 1)
		setDirection(getDirection() + 20);
	if (projectile_alien_helper(this) == 1)
		return;
}

void Turnip::doSomething()
{
	firedByAlien(this);
}

void Cabbage::doSomething()
{
	firedByNachenBlaster(this);
}

void PlayerLaunchedTorpedo::doSomething()
{
	firedByNachenBlaster(this); // 8 points of damage to Alien 
}

void AlienLaunchedTorpedo::doSomething()
{
	firedByAlien(this); // 8 points of damage to NB
}



