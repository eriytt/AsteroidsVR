#include "Asteroid.hh"

#include "AsteroidField.hh"

Asteroid::Asteroid(Ogre::Entity *ent, const Ogre::Vector3 &pos)
{
  node = AsteroidField::getSingleton().sceneManager->getRootSceneNode()->createChildSceneNode();
  node->attachObject(ent);
  node->setPosition(pos);
}

void Asteroid::update(Ogre::Real time_step)
{
  node->translate(Ogre::Vector3(100.1, 1.1, 1.1) * time_step);
  node->yaw(Ogre::Radian(0.5) * time_step);
  node->pitch(Ogre::Radian(0.5) * time_step);
  node->setPosition(AsteroidField::getSingleton().moduloSpace(node->getPosition()));
}
