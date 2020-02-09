// Actor Class

#ifndef ACTOR_H_INCLUDED
#define ACTOR_H_INCLUDED

#include "GraphObject.h"
#include <iostream>
class StudentWorld;
using namespace std; // remove

// Actor Class (abstract base class)-- all the actors are derived from 
class Actor : public GraphObject 
{
public:
	Actor(StudentWorld* studentWorld_ptr, int imageID, double startX, double startY, Direction dir = 0, double size = 1.0, unsigned int depth = 0)
		: GraphObject(imageID, startX, startY, dir, size, depth), m_isAlive(1), m_StudentWorld_ptr(studentWorld_ptr), m_alienCollided(0) {} // Don't mess with!
	virtual ~Actor() {};
	// Accessors: 
	bool isAlive() const { return m_isAlive; }
	virtual bool isAlien() const { return 0; } // redefined/modified by alien class
	virtual bool isGoody() const { return 0; }
	virtual bool testAlive(); // used by aliens and projectiles -- redefined/modified by alien class 
	// Mutators: 
	virtual void doSomething() = 0;
	void setToDead() { m_isAlive = 0; } // mark actor as dead 
	StudentWorld* getStudentWorld() const { return m_StudentWorld_ptr; }
	void increaseAlienCollided(int num) { m_alienCollided = num; }
	int alienCollidedCount() const { return m_alienCollided; }
private:
	bool m_isAlive; // keeps track of whether actor is dead or alive 
	StudentWorld* m_StudentWorld_ptr; 
	int m_alienCollided;
};

// Star Class:
class Star : public Actor
{
public:
	Star(StudentWorld* studentWorld_ptr, int imageID, double startX, double startY, Direction dir, double size, unsigned int depth)
		: Actor(studentWorld_ptr, imageID, startX, startY, dir, size, depth) {}
	~Star() {};
	virtual void doSomething();
private:
};

// Explosion Class:
class Explosion : public Actor
{
public:
	Explosion(StudentWorld* studentWorld_ptr, int imageID, double startX, double startY, Direction dir = 0, double size = 1.0, unsigned int depth = 0)  // correct
		: Actor(studentWorld_ptr, imageID, startX, startY, dir, size, depth), m_count(0) {}
	~Explosion() {};
	virtual void doSomething();
	int tick_count() const { return m_count; }
	int update_count(int num);
private:
	int m_count; /// keeps track of number of tick marks until deletion
};

// DamageableObject Class: 
class DamageableObject : public Actor
{
public:
	DamageableObject(StudentWorld* studentWorld_ptr, int imageID, double startX, double startY, Direction dir, double size, unsigned int depth, double hitpoints)
		: Actor(studentWorld_ptr, imageID, startX, startY, dir, size, depth), m_hitpoints(hitpoints) {}
	double hitpoints() const { return m_hitpoints; }
	virtual double update_hitpoints(double amount) { return m_hitpoints += amount;  } // increase actor's hitpoints by amount
	virtual void sufferDamage(double amount, DamageableObject* alien_num) { } // This actor suffers an amount of damage caused by being hit by either a ship or a projectile (see constants above)
private:
	double m_hitpoints; // aka health 
};

// NachenBlaster Class
class NachenBlaster : public DamageableObject 
{
public:
	NachenBlaster(StudentWorld* studentWorld_ptr, int imageID, double startX, double startY, Direction dir = 0, double size = 1.0, unsigned int depth = 0)
		: DamageableObject(studentWorld_ptr, imageID, startX, startY, dir, size, depth, 50), m_cabbages(30), m_torpedos(10) {}     // RESET TO 0 !!!!!!!!!!!!!!!!  
	~NachenBlaster() { /*if (m_health < 0) delete this;*/ };
	// Accessors:
	int num_cabbages() const { return m_cabbages; }
	int num_torpedos() const { return m_torpedos; }
	// Mutators:
	virtual void doSomething();
	virtual double update_hitpoints(double amount); // different from DamageableObject's funciton since cannot exceed 50 hitpoints                                     
	int update_cabbages(int num);
	int update_torpedos(int num) { return m_torpedos += num; }
	virtual void sufferDamage(double amount);
	// Data Members:
private:
	int m_cabbages;
	int m_torpedos;
};

// Alien Class (abstract base class):
class Alien : public DamageableObject
{
public:
	Alien(StudentWorld* studentWorld_ptr, int imageID, double speed, int alien_num, int flightDirection = 0, Direction dir = 0, double size = 1.5, unsigned int depth = 1,
		double startX = VIEW_WIDTH - 1, double startY = randInt(0, VIEW_HEIGHT - 1))
		: DamageableObject(studentWorld_ptr, imageID, startX, startY, dir, size, depth, 0), // calculate_start_hitpoints() ??
		m_speed(speed), m_flightLength(0), m_flightDirection(flightDirection), m_alien_num(alien_num) {} // not sure waht damageamount should be ???????!!!!!!!!!!!!!!!!!!   
	~Alien() {};

	// Accessors:
	virtual bool isAlien() const { return 1; } // should be good for all aliens now
	virtual double calculate_start_hitpoints() = 0;
	int speed() const { return m_speed; }
	virtual void possiblyDropGoody() = 0;
	int flightLength() const { return m_flightLength; }
	int flightDirection() const { return m_flightDirection; }
	int alienNum() const { return m_alien_num; }

	// Mutators:
	virtual void sufferDamage(double amount, Alien* alien); // re-defines original from DamageableObject alien_num is for which alien it is (1, 2, or 3)
	void sufferDamageHelper(NachenBlaster* NB_ptr);
	void move(int num); // 1 for Smallgons, 2 for Smoregons, 3 for Snagglegons
	void update_speed(double speed) { m_speed = speed; }              
	virtual bool testAlive();
	void update_flight_length(int num) { m_flightLength += num; }
	void update_flight_direction(int num) { m_flightDirection = num; }
	void travelDirection(int num, Alien* a);
	void update_alien_num(int alien_num) { m_alien_num = alien_num; }

private:
	double m_speed;
	int m_flightLength;
	int m_flightDirection;
	int m_alien_num; // identification number for alien (1, 2, or 3)
};

// Smallgon Class (derived from alien):
class Smallgon : public Alien
{
public:
	Smallgon(StudentWorld* studentWorld_ptr, int imageID, double speed = 2, int alien_num = 1)
		: Alien(studentWorld_ptr, imageID, speed, alien_num) {}
	virtual void doSomething();
	virtual void possiblyDropGoody() {}
	virtual double calculate_start_hitpoints(); 
};
	
// Smoregon Class (derived from Alien):
class Smoregon : public Alien
{
public:
	Smoregon(StudentWorld* studentWorld_ptr, int imageID, double speed = 2, int alien_num = 2)
		: Alien(studentWorld_ptr, imageID, speed, alien_num, 6) {} // initial flightDirection of 6 
	virtual void doSomething();
	virtual void possiblyDropGoody();
	virtual double calculate_start_hitpoints();
};

// Snagglegon (derived from Alien):
class Snagglegon : public Alien
{
public:
	Snagglegon(StudentWorld* studentWorld_ptr, int imageID, double speed = 1.75, int alien_num = 3)
		: Alien(studentWorld_ptr, imageID, speed, alien_num) {}
	virtual void doSomething();
	virtual void possiblyDropGoody();
	virtual double calculate_start_hitpoints();
private:
};

// Goody Class (abstract base class):
class Goody : public Actor
{
public:
	Goody(StudentWorld* studentWorld_ptr, int imageID, double startX, double startY, int goody_num, Direction dir = 0, double size = 0.5, unsigned int depth = 1)
		: Actor(studentWorld_ptr, imageID, startX, startY, dir, size, depth), m_goody_num(goody_num) {}
	virtual void doSomething() = 0;
	virtual void goodyTick(Goody* goody);
	void goodyHelper(Goody* goody, NachenBlaster* NB_ptr);
	int goodyNum() const { return m_goody_num; }
	void update_goody_num(int num_goody) { m_goody_num = num_goody; }
	virtual bool isGoody() const { return 1; }
private:
	int m_goody_num; // Goody ID number: 1 for ExtraLife, 2 for Repair, 3 for Torpedo 
};

// ExtraLife Class (derived from Goody):
class ExtraLife : public Goody
{
public:
	ExtraLife(StudentWorld* studentWorld_ptr, int imageID, double startX, double startY)
		: Goody(studentWorld_ptr, imageID, startX, startY, 1) {}
	virtual void doSomething();
};

// Repair Class (derived from Goody):
class Repair : public Goody
{
public:
	Repair(StudentWorld* studentWorld_ptr, int imageID, double startX, double startY)
		: Goody(studentWorld_ptr, imageID, startX, startY, 2) {}
	virtual void doSomething();
};

// Torpedo Class (derived from Goody)
class Torpedo_Goody : public Goody
{
public:
	Torpedo_Goody(StudentWorld* studentWorld_ptr, int imageID, double startX, double startY)
		: Goody(studentWorld_ptr, imageID, startX, startY, 3) {}
	virtual void doSomething();
};

// Projectile Class (abstract base class):
class Projectile : public Actor
{
public:
	Projectile(StudentWorld* studentWorld_ptr, int imageID, double startX, double startY, int projectile_num, Direction dir, double size = 0.5, unsigned int depth = 1)
		: Actor(studentWorld_ptr, imageID, startX, startY, dir, size, depth), m_projectile_num(projectile_num) {}
	virtual void doSomething() = 0;
	virtual bool testAlive();
	int projectile_num() { return m_projectile_num; }
	void projectile_NB_helper(Projectile* projectile, Actor* alien);
	int projectile_alien_helper(Projectile* projectile);
	void firedByNachenBlaster(Projectile* projectile);
	void firedByAlien(Projectile*);
private:
	int m_projectile_num;
};

// Turnip Class (derived from Projectile)
class Turnip : public Projectile
{
public:
	Turnip(StudentWorld* studentWorld_ptr, int imageID, double startX, double startY)
		: Projectile(studentWorld_ptr, imageID, startX, startY, 1, 0) {} // projectile_num = 1, direction = 0
	virtual void doSomething();
};

// Cabbage Class (derived from Projectile)
class Cabbage : public Projectile
{
public:
	Cabbage(StudentWorld* studentWorld_ptr, int imageID, double startX, double startY)
		: Projectile(studentWorld_ptr, imageID, startX, startY, 2, 0) {} // (direction 0)
	virtual void doSomething();
};

// Torpedo_Projectile Class (derived from Projectile)
class Torpedo_Projectile : public Projectile
{
public:
	Torpedo_Projectile(StudentWorld* studentWorld_ptr, int imageID, double startX, double startY, int projectile_num, Direction dir = 0) // must specify 180 degrees for alien 
		: Projectile(studentWorld_ptr, imageID, startX, startY, projectile_num, dir) {}
};

class PlayerLaunchedTorpedo : public Torpedo_Projectile
{
public:
	PlayerLaunchedTorpedo(StudentWorld* studentWorld_ptr, int imageID, double startX, double startY)  
		: Torpedo_Projectile(studentWorld_ptr, imageID, startX, startY, 3) {}
	virtual void doSomething();
};

class AlienLaunchedTorpedo : public Torpedo_Projectile
{
public:
	AlienLaunchedTorpedo(StudentWorld* studentWorld_ptr, int imageID, double startX, double startY, Direction dir = 180) 
		: Torpedo_Projectile(studentWorld_ptr, imageID, startX, startY, 4, dir) {}
	virtual void doSomething();
};

#endif //ACTOR_H_INCLUDED
