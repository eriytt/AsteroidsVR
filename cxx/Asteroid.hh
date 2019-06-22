#ifndef ASTEROID_HH
#define ASTEROID_HH

#include <Ogre.h>

class Asteroid
{
private:
  Ogre::SceneNode *node;
  Ogre::Entity *ent;

public:
  Asteroid(Ogre::Entity *ent, const Ogre::Vector3 &pos);
  void update(Ogre::Real time_step);
};

#endif // ASTEROID_HH
