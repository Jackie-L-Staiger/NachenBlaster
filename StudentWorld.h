// StudentWorld Class

#ifndef STUDENTWORLD_H_INCLUDED
#define STUDENTWORLD_H_INCLUDED

#include "GameWorld.h"
#include <list>
#include "Actor.h"
#include <string>
class NachenBlaster;        

// StudentWorld Class (derived from GameWorld) (NOT abstract)
class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);
	virtual ~StudentWorld();

	// Mutators:
	virtual int init();
	virtual int move();
	virtual void cleanUp();
	void removeDeadGameObjects();
	void addActor(int actor_num, Actor* a); // adds actor to the private m_actors list	                                                     
	void update_completed_level(bool num) { m_completed_level = num; }
	void addAlien(); 
	void update_num_aliens_destroyed() { m_num_aliens_destroyed++; m_num_aliens--; }                 
	void update_num_aliens(int n) { m_num_aliens += n; }
	void addNewAlien();

	// Accessors: 
	NachenBlaster* getNachenBlaster() { return m_NachenBlaster; }
	bool collision(const Actor* a1, const Actor* a2) const; // determines whether 2 objects collide              
	double euclidean_dist(double x1, double y1, double x2, double y2) const;
	Actor* getOneCollidingAlien(const Actor* a) const;
	NachenBlaster* getCollidingPlayer(const Actor* a, int& num); 
	GameWorld* getGameWorld() const { return m_GameWorld_ptr; }
	bool completedLevel() { return m_completed_level; } // if level completed, updated accordingly 
	int num_aliens_destroyed() const { return m_num_aliens_destroyed; }
	int num_aliens() const { return m_num_aliens; }

private:
	std::list<Actor*> m_actors; // Collection of all the actors (excluding Nachenblaster) 
	NachenBlaster* m_NachenBlaster; // Pointer to the NachenBlaster 
	GameWorld* m_GameWorld_ptr;
	int m_num_aliens_destroyed; // keeps track of how many Alien ships have been destroyed so far  
	int m_num_aliens;
	bool m_completed_level;  
};

#endif //STUDENTWORLD_H_INCLUDED
