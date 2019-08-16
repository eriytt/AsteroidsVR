#ifndef BULLET2_COLLISION_SDK_H
#define BULLET2_COLLISION_SDK_H

#include "CollisionSdkInterface.hh"

class Bullet2CollisionSdk
{
  struct Bullet2CollisionSdkInternalData* m_internalData;

public:
  Bullet2CollisionSdk();

  ~Bullet2CollisionSdk();

  plCollisionWorldHandle createCollisionWorld(int maxNumObjsCapacity, int maxNumShapesCapacity, int maxNumPairsCapacity);

  void deleteCollisionWorld();

  plCollisionShapeHandle createSphereShape(plReal radius);

  plCollisionShapeHandle createPlaneShape(plReal planeNormalX,
                                                  plReal planeNormalY,
                                                  plReal planeNormalZ,
                                                  plReal planeConstant);

  plCollisionShapeHandle createCapsuleShape(plReal radius,
                                                    plReal height,
                                                    int capsuleAxis);

  plCollisionShapeHandle createCompoundShape();
  void addChildShape(plCollisionShapeHandle compoundShape,
                     plCollisionShapeHandle childShape,
                     const btVector3 &childPos,
                     const btQuaternion &childOrn);

  void deleteShape(plCollisionShapeHandle shape);

  void addCollisionObject(btCollisionObject * object);
  void removeCollisionObject(btCollisionObject * object);

  btCollisionObject * createCollisionObject(void* userPointer,
                                                int userIndex,
                                                plCollisionShapeHandle cshape,
                                                const btVector3 &startPosition,
                                                const btQuaternion &startOrientation);
  void deleteCollisionObject(btCollisionObject * body);
  void setCollisionObjectTransform(btCollisionObject * body,
                                           const btVector3 &position,
                                           const btQuaternion &orientation);

  int collide(btCollisionObject * colA, btCollisionObject * colB,
                      lwContactPoint* pointsOut, int pointCapacity);

  void collideWorld(plNearCallback filter, void* userData);

  btCollisionWorld::ClosestRayResultCallback shootRay(const btVector3 &from,
                                                      const btVector3 &direction,
                                                      plReal length);

  btCollisionWorld::ClosestRayResultCallback shootRay(const btVector3 &from,
                                                      const btVector3 &to);
  void updateBoundingBoxes();
};

#endif  //BULLET2_COLLISION_SDK_H
