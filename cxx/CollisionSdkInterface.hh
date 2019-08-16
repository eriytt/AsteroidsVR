#ifndef COLLISION_SDK_INTERFACE_H
#define COLLISION_SDK_INTERFACE_H

#include <btBulletCollisionCommon.h>

#define PL_DECLARE_HANDLE(name) \
	typedef struct name##__     \
	{                           \
		int unused;             \
	} * name

#ifdef BT_USE_DOUBLE_PRECISION
typedef double plReal;
#else
typedef float plReal;
#endif


#ifdef __cplusplus
extern "C"
{
#endif

  /**     Particular collision SDK (C-API) */
  PL_DECLARE_HANDLE(plCollisionSdkHandle);

  /**     Collision world, belonging to some collision SDK (C-API)*/
  PL_DECLARE_HANDLE(plCollisionWorldHandle);

  /** Collision object that can be part of a collision World (C-API)*/
  //PL_DECLARE_HANDLE(plCollisionObjectHandle);

  /**     Collision Shape/Geometry, property of a collision object (C-API)*/
  PL_DECLARE_HANDLE(plCollisionShapeHandle);

  struct lwContactPoint
  {
    btVector3 m_ptOnAWorld;
    btVector3 m_ptOnBWorld;
    btVector3 m_normalOnB;
    plReal m_distance;
  };

  typedef void (*plNearCallback)(plCollisionSdkHandle sdkHandle,
                                 plCollisionWorldHandle worldHandle,
                                 void* userData,
                                 btCollisionObject *objA,
                                 btCollisionObject *objB);


#ifdef __cplusplus
}
#endif

#endif  //COLLISION_SDK_INTERFACE_H
