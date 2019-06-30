#include "AsteroidsVR.hh"

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

  if (forward)
      shipNode->translate(Ogre::Vector3(0, 0, -500) * tdelta, Ogre::Node::TransformSpace::TS_LOCAL);
  else if (backward)
      shipNode->translate(Ogre::Vector3(0, 0, 500) * tdelta, Ogre::Node::TransformSpace::TS_LOCAL);

  if (left)
    shipNode->yaw(Ogre::Radian( 0.5 * tdelta));
  else if (right)
    shipNode->yaw(Ogre::Radian(-0.5 * tdelta));

  asteroid->update(tdelta);
  for (auto shot : shots)
    shot->translate(Ogre::Vector3(0, 0, -1000) * tdelta, Ogre::Node::TransformSpace::TS_LOCAL);

  renderFrame();
}

bool AsteroidsVRApp::handleKeyDown(int key)
{
  switch(key)
    {
    case AKEYCODE_A:
      left = true;
      break;
    case AKEYCODE_D:
      right = true;
      break;
    case AKEYCODE_W:
      forward = true;
      break;
    case AKEYCODE_S:
      backward = true;
      break;
    case AKEYCODE_F:
      {
        Ogre::SceneNode *shot = sceneManager->getRootSceneNode()->createChildSceneNode();
        std::stringstream name("shot");
        name << shots.size();
        Ogre::ParticleSystem *sps = sceneManager->createParticleSystem(name.str(), "Shot");
        shot->attachObject(sps);
        shot->setPosition(shipNode->getPosition());
        shot->setOrientation(shipNode->getOrientation());
        shot->translate(Ogre::Vector3(0, 5, (shots.size() & 1 ? -1.0 : 1.0) * 10));
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
    case AKEYCODE_A:
      left = false;
      break;
    case AKEYCODE_D:
      right = false;
      break;
    case AKEYCODE_W:
      forward = false;
      break;
    case AKEYCODE_S:
      backward = false;
      break;
    default:
      return false;
      break;
    }
  return true;
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
