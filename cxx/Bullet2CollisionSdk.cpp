#include "Bullet2CollisionSdk.hh"
#include "btBulletCollisionCommon.h"

struct Bullet2CollisionSdkInternalData
{
	btCollisionConfiguration* m_collisionConfig;
	btCollisionDispatcher* m_dispatcher;
	btBroadphaseInterface* m_aabbBroadphase;
	btCollisionWorld* m_collisionWorld;

	Bullet2CollisionSdkInternalData()
		: m_collisionConfig(0),
		  m_dispatcher(0),
		  m_aabbBroadphase(0),
		  m_collisionWorld(0)
	{
	}
};

Bullet2CollisionSdk::Bullet2CollisionSdk()
{
	m_internalData = new Bullet2CollisionSdkInternalData;
}

Bullet2CollisionSdk::~Bullet2CollisionSdk()
{
	delete m_internalData;
	m_internalData = 0;
}

plCollisionWorldHandle Bullet2CollisionSdk::createCollisionWorld(int /*maxNumObjsCapacity*/, int /*maxNumShapesCapacity*/, int /*maxNumPairsCapacity*/)
{
  m_internalData->m_collisionConfig = new btDefaultCollisionConfiguration;

  m_internalData->m_dispatcher = new btCollisionDispatcher(m_internalData->m_collisionConfig);
  m_internalData->m_aabbBroadphase = new btDbvtBroadphase();
  m_internalData->m_collisionWorld = new btCollisionWorld(m_internalData->m_dispatcher,
                                                          m_internalData->m_aabbBroadphase,
                                                          m_internalData->m_collisionConfig);
  return (plCollisionWorldHandle)m_internalData->m_collisionWorld;
}

void Bullet2CollisionSdk::deleteCollisionWorld()
{
  btCollisionWorld *world = m_internalData->m_collisionWorld;

  if (m_internalData->m_collisionWorld == world)
    {
      delete m_internalData->m_collisionWorld;
      m_internalData->m_collisionWorld = 0;
      delete m_internalData->m_aabbBroadphase;
      m_internalData->m_aabbBroadphase = 0;
      delete m_internalData->m_dispatcher;
      m_internalData->m_dispatcher = 0;
      delete m_internalData->m_collisionConfig;
      m_internalData->m_collisionConfig = 0;
    }
}

plCollisionShapeHandle Bullet2CollisionSdk::createSphereShape(plReal radius)
{
  btSphereShape* sphereShape = new btSphereShape(radius);
  return (plCollisionShapeHandle)sphereShape;
}

plCollisionShapeHandle Bullet2CollisionSdk::createPlaneShape(plReal planeNormalX,
                                                             plReal planeNormalY,
                                                             plReal planeNormalZ,
                                                             plReal planeConstant)
{
  btStaticPlaneShape* planeShape =
    new btStaticPlaneShape(btVector3(planeNormalX, planeNormalY, planeNormalZ), planeConstant);
  return (plCollisionShapeHandle)planeShape;
}

plCollisionShapeHandle Bullet2CollisionSdk::createCapsuleShape(plReal radius,
                                                               plReal height,
                                                               int capsuleAxis)
{
  btCapsuleShape* capsule = 0;

  switch (capsuleAxis)
    {
    case 0:
      {
        capsule = new btCapsuleShapeX(radius, height);
        break;
      }
    case 1:
      {
        capsule = new btCapsuleShape(radius, height);
        break;
      }
    case 2:
      {
        capsule = new btCapsuleShapeZ(radius, height);
        break;
      }
    default:
      {
        btAssert(0);
      }
    }
  return (plCollisionShapeHandle)capsule;
}

plCollisionShapeHandle Bullet2CollisionSdk::createCompoundShape()
{
	return (plCollisionShapeHandle) new btCompoundShape();
}

void Bullet2CollisionSdk::addChildShape(plCollisionShapeHandle compoundShapeHandle, plCollisionShapeHandle childShapeHandle, const btVector3 &childPos, const btQuaternion &childOrn)
{
  btCompoundShape* compound = (btCompoundShape*)compoundShapeHandle;
  btCollisionShape* childShape = (btCollisionShape*)childShapeHandle;
  btTransform localTrans;
  localTrans.setOrigin(childPos);
  localTrans.setRotation(childOrn);
  compound->addChildShape(localTrans, childShape);
}

void Bullet2CollisionSdk::deleteShape(plCollisionShapeHandle shapeHandle)
{
  btCollisionShape* shape = (btCollisionShape*)shapeHandle;
  delete shape;
}

void Bullet2CollisionSdk::addCollisionObject(btCollisionObject *objectHandle)
{
  btCollisionWorld *world = m_internalData->m_collisionWorld;
  btCollisionObject* colObj = (btCollisionObject*)objectHandle;
  btAssert(world && colObj);
  if (world == m_internalData->m_collisionWorld && colObj)
    {
      world->addCollisionObject(colObj);
    }
}

void Bullet2CollisionSdk::removeCollisionObject(btCollisionObject *objectHandle)
{
  btCollisionWorld *world = m_internalData->m_collisionWorld;
  btCollisionObject* colObj = (btCollisionObject*)objectHandle;
  if (colObj)
    world->removeCollisionObject(colObj);
}

btCollisionObject *Bullet2CollisionSdk::createCollisionObject(void* userPointer,
                                                              int userIndex,
                                                              plCollisionShapeHandle shapeHandle,
                                                              const btVector3 &startPosition,
                                                              const btQuaternion &startOrientation)

{
  btCollisionShape* colShape = (btCollisionShape*)shapeHandle;
  btAssert(colShape);
  if (colShape)
    {
      btCollisionObject* colObj = new btCollisionObject;
      colObj->setUserIndex(userIndex);
      colObj->setUserPointer(userPointer);
      colObj->setCollisionShape(colShape);
      btTransform tr;
      tr.setOrigin(startPosition);
      tr.setRotation(startOrientation);
      colObj->setWorldTransform(tr);
      return colObj;
    }
  return 0;
}


void Bullet2CollisionSdk::deleteCollisionObject(btCollisionObject  *bodyHandle)
{
  btCollisionObject* colObj = (btCollisionObject*)bodyHandle;
  delete colObj;
}

void Bullet2CollisionSdk::setCollisionObjectTransform(btCollisionObject *bodyHandle,
                                                      const btVector3 &position,
                                                      const btQuaternion &orientation)
{
  btCollisionObject* colObj = (btCollisionObject*)bodyHandle;
  btTransform tr;
  tr.setOrigin(position);
  tr.setRotation(orientation);
  colObj->setWorldTransform(tr);
}

struct Bullet2ContactResultCallback : public btCollisionWorld::ContactResultCallback
{
  int m_numContacts;
  lwContactPoint* m_pointsOut;
  int m_pointCapacity;

  Bullet2ContactResultCallback(lwContactPoint* pointsOut, int pointCapacity) : m_numContacts(0),
                                                                               m_pointsOut(pointsOut),
                                                                               m_pointCapacity(pointCapacity)
  {
  }
  virtual btScalar addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1)
  {
    if (m_numContacts < m_pointCapacity)
      {
        lwContactPoint& ptOut = m_pointsOut[m_numContacts];
        ptOut.m_distance = cp.m_distance1;
        ptOut.m_normalOnB[0] = cp.m_normalWorldOnB.getX();
        ptOut.m_normalOnB[1] = cp.m_normalWorldOnB.getY();
        ptOut.m_normalOnB[2] = cp.m_normalWorldOnB.getZ();
        ptOut.m_ptOnAWorld[0] = cp.m_positionWorldOnA[0];
        ptOut.m_ptOnAWorld[1] = cp.m_positionWorldOnA[1];
        ptOut.m_ptOnAWorld[2] = cp.m_positionWorldOnA[2];
        ptOut.m_ptOnBWorld[0] = cp.m_positionWorldOnB[0];
        ptOut.m_ptOnBWorld[1] = cp.m_positionWorldOnB[1];
        ptOut.m_ptOnBWorld[2] = cp.m_positionWorldOnB[2];
        m_numContacts++;
      }

    return 1.f;
  }
};

int Bullet2CollisionSdk::collide(btCollisionObject  *colA,
                                 btCollisionObject *colB,
                                 lwContactPoint* pointsOut,
                                 int pointCapacity)
{
  btCollisionWorld *world = m_internalData->m_collisionWorld;
  btCollisionObject* colObjA = (btCollisionObject*)colA;
  btCollisionObject* colObjB = (btCollisionObject*)colB;
  btAssert(world && colObjA && colObjB);
  if (world == m_internalData->m_collisionWorld && colObjA && colObjB)
    {
      Bullet2ContactResultCallback cb(pointsOut, pointCapacity);
      world->contactPairTest(colObjA, colObjB, cb);
      return cb.m_numContacts;
    }
  return 0;
}

static plNearCallback gTmpFilter;
static int gNearCallbackCount = 0;
static plCollisionSdkHandle gCollisionSdk = 0;
static plCollisionWorldHandle gCollisionWorldHandle = 0;

static void* gUserData = 0;

void Bullet2NearCallback(btBroadphasePair& collisionPair, btCollisionDispatcher& dispatcher, const btDispatcherInfo& dispatchInfo)
{
  btCollisionObject* colObj0 = (btCollisionObject*)collisionPair.m_pProxy0->m_clientObject;
  btCollisionObject* colObj1 = (btCollisionObject*)collisionPair.m_pProxy1->m_clientObject;
  btCollisionObject * obA = colObj0;
  btCollisionObject * obB = colObj1;
  if (gTmpFilter)
    {
      gTmpFilter(gCollisionSdk, gCollisionWorldHandle, gUserData, obA, obB);
      gNearCallbackCount++;
    }
}

void Bullet2CollisionSdk::collideWorld(plNearCallback filter, void* userData)
{
  btCollisionWorld *world = m_internalData->m_collisionWorld;
  //chain the near-callback
  gTmpFilter = filter;
  gNearCallbackCount = 0;
  gUserData = userData;
  gCollisionSdk = (plCollisionSdkHandle)this;
  gCollisionWorldHandle = reinterpret_cast<plCollisionWorldHandle>(world);
  m_internalData->m_dispatcher->setNearCallback(Bullet2NearCallback);
  world->performDiscreteCollisionDetection();
  gTmpFilter = 0;
}


btCollisionWorld::ClosestRayResultCallback Bullet2CollisionSdk::shootRay(const btVector3 &from,
                                                                         const btVector3 &direction,
                                                                         plReal length)
{
  btVector3 to = from + (direction * length);
  return shootRay(from, to);
}

btCollisionWorld::ClosestRayResultCallback Bullet2CollisionSdk::shootRay(const btVector3 &from,
                                  const btVector3 &to)
{
  btCollisionWorld *world = m_internalData->m_collisionWorld;

  btCollisionWorld::ClosestRayResultCallback rayResult(from, to);
  // TODO: How to make sure the ray does not hit ourselves?
  world->rayTest(from, to, rayResult);
  return rayResult;
}

void Bullet2CollisionSdk::updateBoundingBoxes()
{
  m_internalData->m_collisionWorld->updateAabbs();
}
