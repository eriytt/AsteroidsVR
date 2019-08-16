#include "AsteroidsVR.hh"

#include "Bullet2CollisionSdk.hh"

#include "keycodes.h"
#include "AsteroidField.hh"

void AsteroidsVRApp::setupCamera()
{
  forBothCameras([&](Ogre::Camera *cam){
      cam->setNearClipDistance(NearClip);
      cam->setFarClipDistance(FarClip);
    });
}

#if defined(ANDROID)
void AsteroidsVRApp::setupResources(Ogre::ResourceGroupManager &rgm)
{
  rgm.addResourceLocation("/models", "APKFileSystem");
  rgm.addResourceLocation("/particles", "APKFileSystem");
  rgm.addResourceLocation("/materials", "APKFileSystem");
}
#else
void AsteroidsVRApp::setupResources(Ogre::ResourceGroupManager &rgm)
{
  rgm.addResourceLocation("../project/assets/models", "FileSystem");
  rgm.addResourceLocation("../project/assets/particles", "FileSystem");
  rgm.addResourceLocation("host-assets/materials", "FileSystem");
  rgm.addResourceLocation("../project/assets/materials", "FileSystem");
}

#endif

void AsteroidsVRApp::initialize()
{
  OgreCardboardApp::initialize();


  btSdk = new Bullet2CollisionSdk;
  btSdk->createCollisionWorld(0, 0, 0);


  forBothCamerasAndViewports([](Ogre::Camera *c, Ogre::Viewport *vp){
    c->setPosition(Ogre::Vector3(0.0f, 0.0f, 0.0f));
    c->lookAt(Ogre::Vector3(-5000.0f, 0.0f, 0.0f));
    vp->setBackgroundColour(Ogre::ColourValue::Black);
    });

  Ogre::Vector3 lightdir(0.55, -0.3, 0.75);
  lightdir.normalise();
  Ogre::Light *l = sceneManager->createLight("tstLight");
  l->setType(Ogre::Light::LT_DIRECTIONAL);
  l->setDirection(lightdir);
  l->setDiffuseColour(Ogre::ColourValue::White);
  l->setSpecularColour(Ogre::ColourValue(0.4, 0.4, 0.4));

  sceneManager->setAmbientLight(Ogre::ColourValue(0.2, 0.2, 0.2));

  Ogre::HighLevelGpuProgramManager &programMgr = Ogre::HighLevelGpuProgramManager::getSingleton();
  programMgr.setVerbose(true);
  Ogre::HighLevelGpuProgramPtr vertexProgram =
    programMgr.createProgram("vTest", "General", "glsles", Ogre::GPT_VERTEX_PROGRAM);
  vertexProgram->setSource(VertexShader);
  vertexProgram->setVertexTextureFetchRequired(false);

  Ogre::HighLevelGpuProgramPtr fragmentProgram =
    programMgr.createProgram("fTest", "General", "glsles", Ogre::GPT_FRAGMENT_PROGRAM);
  fragmentProgram->setSource(FragmentShader);

  auto scriptStream = OGRE_NEW Ogre::MemoryDataStream("materialStream",
                                                      (void*)ShaderMaterialScript,
                                                      strlen(ShaderMaterialScript));
  Ogre::SharedPtr<Ogre::DataStream> ptr =
    Ogre::SharedPtr<Ogre::DataStream>(scriptStream, Ogre::SPFM_DELETE);
  Ogre::MaterialManager::getSingleton().parseScript(ptr, "General");


  AsteroidField::initialize(sceneManager, SpaceSize);
  Ogre::Entity* asteroid_ent = sceneManager->createEntity("asteroid.mesh");
  asteroid_ent->setMaterialName("myshadermaterial");
  asteroid = new Asteroid(asteroid_ent, Ogre::Vector3(-5000.0f, 0.0f, 0.0f));

  auto apos = asteroid->getSceneNode()->_getDerivedPosition();
  auto asteroid_shape = btSdk->createSphereShape(150);
  asteroidColObj = btSdk->createCollisionObject(nullptr, 0, asteroid_shape,
                                                btVector3(apos.x, apos.y, apos.z),
                                                btQuaternion(0, 0, 0, 1));
  btSdk->addCollisionObject(asteroidColObj);

  shipNode = sceneManager->getRootSceneNode()->createChildSceneNode();
  shipNode->setPosition(lcam->getDerivedPosition());
  shipNode->setOrientation(lcam->getDerivedOrientation());
  forBothCameras([&](Ogre::Camera *c){
      shipNode->attachObject(c);
      c->setPosition(Ogre::Vector3(0.0f, 0.0f, 0.0f));
      c->lookAt(Ogre::Vector3(-1.0f, 0.0f, 0.0f));
    });

  sceneManager->addRenderQueueListener(this);

  timer = new Ogre::Timer();
  timer->reset();
  lastFrameTime_us = timer->getMicroseconds();
}

void AsteroidsVRApp::mainLoop()
{
  unsigned long frame_time = timer->getMicroseconds();
  Ogre::Real tdelta = (frame_time - lastFrameTime_us) / Ogre::Real(1000000);
  lastFrameTime_us = frame_time;

  Ogre::Vector3 direction(shipNode->_getDerivedOrientation() * Ogre::Vector3::NEGATIVE_UNIT_Z);
  Ogre::Vector3 acceleration(direction * throttle / shipMass);
  shipVelocity += acceleration / 2.0 * tdelta;

  shipNode->yaw(Ogre::Radian(yaw * tdelta));
  shipNode->pitch(Ogre::Radian(pitch * tdelta));
  shipNode->roll(Ogre::Radian(roll * tdelta));

  shipNode->translate(shipVelocity * tdelta);
  Ogre::Vector3 shipPosition(shipNode->_getDerivedPosition());

  asteroid->update(tdelta);

  auto apos = asteroid->getSceneNode()->_getDerivedPosition();
  btSdk->setCollisionObjectTransform(asteroidColObj,
                                     btVector3(apos.x, apos.y, apos.z),
                                     btQuaternion(0, 0, 0, 1));
  btSdk->updateBoundingBoxes();

  auto si = shots.begin();
  for (; si != shots.end();)
    {
      auto shot = *si;
      Ogre::Vector3 shotPosition(shot->_getDerivedPosition());
      if (shotPosition.squaredDistance(shipPosition) > 25000000.0f)
        {
          auto ps = static_cast<Ogre::ParticleSystem*>(shot->detachObject((unsigned short)0));
          sceneManager->destroyParticleSystem(ps);
          si = shots.erase(si);
          continue;
        }

      shot->translate(Ogre::Vector3(0, 0, -1000) * tdelta, Ogre::Node::TransformSpace::TS_LOCAL);
      Ogre::Vector3 newShotPosition(shot->_getDerivedPosition());

      auto result = btSdk->shootRay(btVector3(shotPosition.x, shotPosition.y, shotPosition.z),
                                    btVector3(newShotPosition.x, newShotPosition.y, newShotPosition.z));
      if (result.m_collisionObject != nullptr)
        {
          auto ps = static_cast<Ogre::ParticleSystem*>(shot->detachObject((unsigned short)0));
          sceneManager->destroyParticleSystem(ps);


          Ogre::SceneNode *hit = sceneManager->getRootSceneNode()->createChildSceneNode();
          std::stringstream name;
          name << "hit" << shotCtr++;
          Ogre::ParticleSystem *sps = sceneManager->createParticleSystem(name.str(), "Hit");
          hit->attachObject(sps);
          hit->setPosition(Ogre::Vector3(result.m_hitPointWorld.x(),
                                         result.m_hitPointWorld.y(),
                                         result.m_hitPointWorld.z()));
          queue.postEvent(lastFrameTime_us + (sps->getEmitter(0)->getDuration() + 1) * 1000000,
                          [&, hit, sps](unsigned long at, unsigned long et, eid_t id) {
                            sceneManager->getRootSceneNode()->removeChild(hit);
                            hit->detachObject(sps);
                            sceneManager->destroyParticleSystem(sps);
                            sceneManager->destroySceneNode(hit);
                          });

          si = shots.erase(si);
          continue;
        }

        ++si;
    }

  queue.advance(lastFrameTime_us);

  renderFrame();
}

bool AsteroidsVRApp::handleKeyDown(int key)
{
  std::cout << "Key: " << (key == AKEYCODE_W) << std::endl;
  switch(key)
    {
    case AKEYCODE_W:
      throttle = MaxThrottle;
      break;
    case AKEYCODE_A:
      roll = MaxRoll;
      break;
    case AKEYCODE_D:
      roll = -MaxRoll;
      break;
    case AKEYCODE_J:
      yaw = MaxYaw;
      break;
    case AKEYCODE_L:
      yaw = -MaxYaw;
      break;
    case AKEYCODE_I:
      pitch = MaxPitch;
      break;
    case AKEYCODE_K:
      pitch = -MaxPitch;
      break;

    case AKEYCODE_BUTTON_R1:
    case AKEYCODE_F:
      {
        std::cout << "Velocity: " << shipVelocity.length() << " m/s" << std::endl;
        Ogre::SceneNode *shot = sceneManager->getRootSceneNode()->createChildSceneNode();
        std::stringstream name;
        name << "shot" << shotCtr++;
        Ogre::ParticleSystem *sps = sceneManager->createParticleSystem(name.str(), "Shot");
        shot->attachObject(sps);
        shot->setPosition(shipNode->getPosition());
        shot->setOrientation(shipNode->getOrientation());
        shot->translate(Ogre::Vector3((shotCtr & 1 ? -1.0 : 1.0) * 10, 5, 0),
                        Ogre::Node::TransformSpace::TS_LOCAL);
        shots.push_back(shot);
      }
      break;
    default:
      return false;
      break;
    }
  return true;
}

bool AsteroidsVRApp::handleKeyUp(int key)
{
  switch(key)
    {
    case AKEYCODE_W:
      throttle = 0.0;
      break;
    case AKEYCODE_A:
    case AKEYCODE_D:
      roll = 0.0;
      break;
    case AKEYCODE_J:
    case AKEYCODE_L:
      yaw = 0.0;
      break;
    case AKEYCODE_I:
    case AKEYCODE_K:
      pitch = 0.0;
      break;
    default:
      return false;
      break;
    }
  return true;
}

void AsteroidsVRApp::handleJoystick(float new_throttle, float new_yaw, float new_pitch, float new_roll)
{
  throttle = -std::min(new_throttle, 0.0f) * MaxThrottle;
  yaw = -new_yaw;
  pitch = -new_pitch;
  roll = -new_roll;
}


void AsteroidsVRApp::renderQueueStarted(Ogre::uint8 queueGroupId, const Ogre::String &invocation, bool &skipThisInvocation)
{
  if (queueGroupId == 51)
    {
      auto rs = root->getRenderSystem();
      auto vp = rs->_getViewport();
      auto cam = vp->getCamera();

      cam->setNearClipDistance(0.2f);
      cam->setFarClipDistance(5.0f);
#if defined(ANDROID)
      gvr::Rectf fp = OgreCardboardApp::GVRFOV2FrustumExtents(cam == lcam ? lFOV : rFOV, cam->getNearClipDistance());
      cam->setFrustumExtents(fp.left, fp.right, fp.top, fp.bottom);
#endif
      rs->_setProjectionMatrix(cam->getProjectionMatrixRS());
      vp->clear(Ogre::FrameBufferType::FBT_DEPTH);
    }
}

void AsteroidsVRApp::renderQueueEnded(Ogre::uint8 queueGroupId, const Ogre::String &invocation, bool &repeatThisInvocation)
{
  if (queueGroupId == 51)
    {
      auto rs = root->getRenderSystem();
      auto cam = rs->_getViewport()->getCamera();
      cam->setNearClipDistance(NearClip);
      cam->setFarClipDistance(FarClip);
#if defined(ANDROID)
      gvr::Rectf fp = OgreCardboardApp::GVRFOV2FrustumExtents(cam == lcam ? lFOV : rFOV, cam->getNearClipDistance());
      cam->setFrustumExtents(fp.left, fp.right, fp.top, fp.bottom);
#endif
      root->getRenderSystem()->_setProjectionMatrix(cam->getProjectionMatrixRS());
    }
}
