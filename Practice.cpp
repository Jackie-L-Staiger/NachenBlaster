/*void StudentWorld::removeDeadGameObjects()                                      
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
	}*/

/*void StudentWorld::removeDeadGameObjects()                                    
{
	list<Actor*>::iterator i = m_actors.begin();
	list<Actor*>::iterator j = m_actors.end();
	while (i != m_actors.end())
	{
		// If the pointer i "points" to points to a dead actor, remove it from the m_actors list and add it to destroyedOnes to be destroyed 
		if ((*i) != nullptr && (*i)->isAlive() == 0)
		{

			*i = m_actors.back();
			//Actor* temp = *i;
			//i = m_actors.erase(i);
			//swap(*i, *j); 
			//*i = //m_actors.back(); Actor* temp = *i;
			m_actors.pop_back();
			i--;
		}
		i++;
	}

	/*for (int k = 0; k != destroyedOnes.size(); k++)
	{
	// Delete Dead actors:
	delete destroyedOnes[k];
	destroyedOnes.erase(destroyedOnes.begin() + k);
	k--;
	}
}*/

// If Smallgon collided with NachenBlaster,
/*if (num == 1)
{
	// Damages NachenBlaster by causing it to lose 5 hitpoints                          
	NB_ptr->sufferDamage(5, HIT_BY_SHIP);
	setToDead();
	// Update player's score 
	NB_ptr->update_score(250);
	// Play sound:
	getStudentWorld()->playSound(SOUND_DEATH);
	// Create explosion object
	getStudentWorld()->addActor(2, this);
	// Add another Alien to game to replace dead one               
	addNewAlien();
}
// If Smallgon gets hit by NachenBlaster projectile (but Smallgon not necessarily destroyed),
else if (getStudentWorld()->alien_collided())
{
	// Decrement hitpoints by 2 from cabbages
	update_hitpoints(-2);
	// If Alien gets hit by projectile and is destroyed,
	if (hitpoints() <= 0)
	{
		// Increase NachenBlaster's score 
		NB_ptr->update_score(250);                                                          
		setToDead();
		getStudentWorld()->playSound(SOUND_DEATH);
		// Create explosion object
		getStudentWorld()->addActor(2, this);
		// Add another Alien to game to replace dead one       
		addNewAlien();
	}
	// If Alien gets hit, but is not destroyed,
	else
		getStudentWorld()->playSound(SOUND_BLAST);
}
// Reset alien_destroyed()
getStudentWorld()->recordAlienCollided(0);

//getStudentWorld()->addActor(3, this); // add another Smallgon*/