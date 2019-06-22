#include "AsteroidField.hh"

AsteroidField *AsteroidField::field;

void AsteroidField::initialize(Ogre::SceneManager *mgr, float size)
{
  field = new AsteroidField();
  field->size = Ogre::Vector3(size, size, size);
  field->sceneManager = mgr;
}

const AsteroidField &AsteroidField::getSingleton()
{
  return *field;
}


Ogre::Vector3 AsteroidField::moduloSpace(const Ogre::Vector3 coord) const
{
  Ogre::Vector3 new_coord(coord);
  if (new_coord.x > size.x)
    new_coord.x = new_coord.x - 2 * size.x;
  else if (new_coord.x < -size.x)
    new_coord.x = new_coord.x + 2 * size.x;

  if (new_coord.y > size.y)
    new_coord.y = new_coord.y - 2 * size.y;
  else if (new_coord.y < -size.y)
    new_coord.y = new_coord.y + 2 * size.y;

  if (new_coord.z > size.z)
    new_coord.z = new_coord.z - 2 * size.z;
  else if (new_coord.z < -size.z)
    new_coord.z = new_coord.z + 2 * size.z;

  return new_coord;
}
