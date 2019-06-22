#ifndef ASTEROIDFIELD_HH
#define ASTEROIDFIELD_HH

#include <Ogre.h>

class AsteroidField
{
private:
  static AsteroidField *field;

public:
  static void initialize(Ogre::SceneManager *scene_mgr);
  static const AsteroidField &getSingleton();

public:
  Ogre::SceneManager *sceneManager = nullptr;

private:
  Ogre::Vector3 size;

public:
  Ogre::Vector3 moduloSpace(const Ogre::Vector3 coord) const;

};

#endif //ASTEROIDFIELD_HH
