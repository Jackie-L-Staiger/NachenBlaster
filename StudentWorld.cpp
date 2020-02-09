// StudentWorld Implementations 

#include "StudentWorld.h"
#include "GameConstants.h"
#include "GraphObject.h"
#include "Actor.h"
#include <stdlib.h> 
#include <string>
#include <list>
#include <vector>
#include <sstream> 
using namespace std;

vector<Actor*> destroyedOnes;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Constructor 
StudentWorld::StudentWorld(string assetDir)
	: GameWorld(assetDir), m_num_aliens_destroyed(0), m_num_aliens(0), m_completed_level(0) {}  

// Destructor
StudentWorld::~StudentWorld() 
{ 
	cleanUp();
}

int StudentWorld::init()
{
	// Allocate and insert a valid NachenBlaster object into the game world at position (0, 128)
	m_NachenBlaster = new NachenBlaster(this, IID_NACHENBLASTER, 0, 128, 0, 1, 0);
	// Add 30 stars to game with image ID of IID_STAR, direction of 0, depth of 3, everything else chosen randomly
	for (int i = 0; i < 30; i++)
	{
		double rand_double = (randInt(5, 50));
		rand_double = rand_double / 100;
		m_actors.push_back(new Star(this, IID_STAR, randInt(0, VIEW_WIDTH - 1), randInt(0, VIEW_HEIGHT - 1), 0, rand_double, 3));
	}
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::addAlien()
{
	// Get random number, then determine which kind of aliens need to be added with appropriate probability based on current level 
	int s = 60 + (20 + (getLevel() * 5)) + (5 + (getLevel() * 10));
	int num = randInt(0, s);
	if (num < 60)
		m_actors.push_back(new Smallgon(this, IID_SMALLGON));
	else if (num < 60 + (20 + (getLevel() * 5)))
		m_actors.push_back(new Smoregon(this, IID_SMOREGON));
	else
		m_actors.push_back(new Snagglegon(this, IID_SNAGGLEGON));
}

int StudentWorld::move() 
{
	// Add another Alien to game to replace destroyed one (if necessary)              
	addNewAlien();
	// If alive, give NachenBlaster a chance to do something:
	if (m_NachenBlaster != nullptr && m_NachenBlaster->isAlive())
		m_NachenBlaster->doSomething(); 
	// For every other actor in the game world, give actor a chance to do something 
	for (list<Actor*>::const_iterator i = m_actors.begin(); i != m_actors.end(); i++)                                  
	{
		// If actor is still alive, (must check if *i != nullptr since m_actors could be an empty list)
		if ((*i) != nullptr && ((*i)->isAlive() == 1))                                                       
		{
			// Tell each actor to do something (e.g. move)
			(*i)->doSomething();
			// If NachenBlaster dies, end current level
			if (m_NachenBlaster->isAlive() == 0) 
			{
				decLives();
				// Update alien count to 0 again 
				update_num_aliens(-m_num_aliens);
				return GWSTATUS_PLAYER_DIED;
			}
			// If the NachenBlaster completed the current level, 
			if (completedLevel() == 1)
			{
				// Increase NachenBlaster's score appropriately 
				increaseScore(250);                                    // ??????????????????????????????????????????
				// Reset completedLevel() for next level 
				update_completed_level(0);
				// Play sound 
				playSound(SOUND_FINISHED_LEVEL);
				return GWSTATUS_FINISHED_LEVEL;
			}
		}
	}
	// Remove dead actors after each tick
 	removeDeadGameObjects(); 

	// Update the Game Status Line and send stringstream to GameWorld (updateDisplayText();)
	ostringstream gamestream; 
	gamestream << "Lives: " << getLives() << "  Health: " << abs(m_NachenBlaster->hitpoints() / 50.0) * 100.0 << "%  Score: " << getScore() << "  Level: " << getLevel() << "  Cabbages: " << (m_NachenBlaster->num_cabbages() * 100) / 30 << "%  Torpedos: " << m_NachenBlaster->num_torpedos();
	string stringstream = gamestream.str();
	setGameStatText(stringstream);

	// Since the player hasn’t completed the current level and hasn’t died, continue playing the current level
	return GWSTATUS_CONTINUE_GAME;
}

// Delete all allocated actors and NachenBlaster 
void StudentWorld::cleanUp()
{
	// Delete ALL the actors (after NachenBlaster dies)
	for (list<Actor*>::iterator p = m_actors.begin(); p != m_actors.end(); p++)
	{
		if (*p != nullptr)
		{
			delete *p;
			*p = nullptr;
			m_num_aliens_destroyed = 0;
		}
	}
	// Delete NachenBlaster
	if (m_NachenBlaster != nullptr)
	{
		delete m_NachenBlaster;
		m_NachenBlaster = nullptr;
	}
}

void StudentWorld::removeDeadGameObjects()                                        
{
	list<Actor*>::const_iterator i = m_actors.begin();
	while (i != m_actors.end())
	{
		// If the pointer i "points" to points to a dead actor, remove it from the m_actors list and add it to destroyedOnes to be destroyed 
		if ((*i) != nullptr && (*i)->isAlive() == 0)
		{
			// Save pointers to items to be deleted in vector destroyedOnes
			destroyedOnes.push_back((*i));
			i = m_actors.erase(i);
			if (i != m_actors.begin())
				i--;
		}
		i++;
	}

	for (int k = 0; k != destroyedOnes.size(); k++)
	{
		// Delete Dead actors: 
		delete destroyedOnes[k];
		destroyedOnes.erase(destroyedOnes.begin() + k);
		k--;
	}
}

// Inserts actor into game 
void StudentWorld::addActor(int actor_num, Actor* a)
{
	double rand_double = (randInt(5, 50));
	rand_double = rand_double / 100;
	switch (actor_num)
	{
	case 1: 
		m_actors.push_back(new Star(this, IID_STAR, randInt(0, VIEW_WIDTH - 1), randInt(0, VIEW_HEIGHT - 1), 0, rand_double, 3)); break;
	case 2: 
		m_actors.push_back(new Explosion(this, IID_EXPLOSION, a->getX(), a->getY())); break;
	case 6:
		m_actors.push_back(new Cabbage(this, IID_CABBAGE, m_NachenBlaster->getX() + 12, m_NachenBlaster->getY())); break;
	case 7:
		m_actors.push_back(new Turnip(this, IID_TURNIP, a->getX() - 14, a->getY())); break;                          
	case 8:
		m_actors.push_back(new PlayerLaunchedTorpedo(this, IID_TORPEDO, m_NachenBlaster->getX() + 12, m_NachenBlaster->getY())); break;      
	case 9:
		m_actors.push_back(new AlienLaunchedTorpedo(this, IID_TORPEDO, m_NachenBlaster->getX() - 14, m_NachenBlaster->getY())); break;   
	case 10:
		m_actors.push_back(new ExtraLife(this, IID_LIFE_GOODIE, a->getX(), a->getY())); break;     
	case 11:
		m_actors.push_back(new Repair(this, IID_REPAIR_GOODIE, a->getX(), a->getY())); break;      
	case 12: 
		m_actors.push_back(new Torpedo_Goody(this, IID_TORPEDO, a->getX(), a->getY())); break;          
	}       
}

double StudentWorld::euclidean_dist(double x1, double y1, double x2, double y2) const 
{
	return sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
}

bool StudentWorld::collision(const Actor* a1, const Actor* a2) const 
{
	// If 2 Actors collided, return true
	if (euclidean_dist(a1->getX(), a1->getY(), a2->getX(), a2->getY()) < (0.75*(a1->getRadius() + a2->getRadius())))
		return 1;
	// Else, return false 
	else
		return 0;
}
																															 
// If there's at least one alien that's collided with a (A PROJECTILE), return a pointer to one of them (ONE OF THE ALIENS); otherwise, return a null pointer  
Actor* StudentWorld::getOneCollidingAlien(const Actor* a) const 
{
	// Go through all the actors (other than NachenBlaster) to see if the projectile collided with an alien 
	for (list<Actor*>::const_iterator p = m_actors.begin(); p != m_actors.end(); p++)
	{
		// If iterator points to an alien,
		if (*p != nullptr && (*p)->isAlien())
		{
			// If collision occurred, return pointer to actor that caddAliaused collision 
			if (collision(*p, a))
				return *p;
		}
	}
	return nullptr;
}

// If the NachenBlaster has collided with a (COULD BE PROJECTILE OR ALIEN OR GOODY), update value of num; return a pointer to the NachenBlaster (in ALL cases)             
NachenBlaster* StudentWorld::getCollidingPlayer(const Actor* a, int& num)
{
	// Pass pointer to NachenBlaster and pointer to actor into collision function to determine if collision took place 
	if (collision(m_NachenBlaster, a))
		num = 1;
	return m_NachenBlaster;
}

void StudentWorld::addNewAlien()
{
	// destroyed is the total number of aliens destroyed by the NachenBlaster (does NOT include those that have flown off the screen)
	double destroyed = num_aliens_destroyed();
	double left = (6 + (4 * getLevel())) - destroyed;
	double max = 4 + (0.5 * getLevel());
	int min;
	if (left < max)
		min = left;
	else
		min = max;
	if (num_aliens() < min)
	{
		addAlien();
		update_num_aliens(1);                                            
	}
	// If level ended (number of aliens destroyed equals the number that needs to be destroyed)
	if (destroyed == (6 + (4 * getLevel())))
	{
		update_completed_level(1);       
	}
}
