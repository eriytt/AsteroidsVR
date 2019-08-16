#ifndef UTILS_H
#define UTILS_H

#include <algorithm>

#include <vector>

#include <cmath>

#include <btBulletDynamicsCommon.h>

#include <Ogre.h>

namespace AVRUtils {

  static const float PI = M_PI;
  static const float PI_2 = M_PI_2;
  static const float PI_4 = M_PI_4;


  template < typename T >
  inline T POW2(const T &x)
  {
    return x * x;
  }

  template < typename T >
  inline T Deg2Rad(const T &x)
  {
    return x / 360.0 * 2.0 * PI;
  }

  template < typename T >
  inline T Rad2Deg(const T &x)
  {
    return x * 360.0 / (2.0 * PI);
  }

  inline void Bullet2OgreVector(const btVector3 &btVec, Ogre::Vector3 &oVec)
  {
    oVec.x = btVec.getX();
    oVec.y = btVec.getY();
    oVec.z = btVec.getZ();
  }

  inline Ogre::Vector3 Bullet2OgreVector(const btVector3 &btVec)
  {
    return Ogre::Vector3(btVec.m_floats);
  }

  inline Ogre::Quaternion Bullet2OgreQuaternion(const btQuaternion &btQ)
  {
    return Ogre::Quaternion(btQ.w(), btQ.x(), btQ.y(), btQ.z());
  }

  inline btVector3 Ogre2BulletVector(const Ogre::Vector3 &oVec)
  {
    return btVector3(oVec.x, oVec.y, oVec.z);
  }

  inline void Ogre2BulletVector(const Ogre::Vector3 &oVec, btVector3 &btVec)
  {
    btVec.setX(oVec.x);
    btVec.setY(oVec.y);
    btVec.setZ(oVec.z);
  }

  template<typename T>
  class SortedList : public std::list<T>
  {
  public:
    typename std::list<T>::iterator insert(const typename std::list<T>::value_type& val)
    {
      if (std::list<T>::empty() or std::list<T>::back() < val)
	{
	  std::list<T>::push_back(val);
	  return --std::list<T>::end();
	}

      typename std::list<T>::iterator insertpos =
	std::find_if(std::list<T>::begin(),
		     std::list<T>::end(),
		     [val](T it) {return val < it;});
	return std::list<T>::insert(insertpos, val);
    }
  };

} // namespace AVRUtils
#endif // UTILS_H
