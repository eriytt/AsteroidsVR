#include "AsteroidsVR.hh"

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
}
#else
void AsteroidsVRApp::setupResources(Ogre::ResourceGroupManager &rgm)
{
  rgm.addResourceLocation("../project/assets/models", "FileSystem");
}

#endif

void AsteroidsVRApp::initialize()
{
  OgreCardboardApp::initialize();

  forBothCamerasAndViewports([](Ogre::Camera *c, Ogre::Viewport *vp){
    c->setPosition(Ogre::Vector3(80.0f, 80.0f, 80.0f));
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

  camNode = sceneManager->getRootSceneNode()->createChildSceneNode();
  camNode->setPosition(lcam->getDerivedPosition());

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
    {
      Ogre::Vector3 d(lcam->getDerivedDirection());
      forBothCameras([&](Ogre::Camera *cam){cam->move(d * 5.0 * tdelta);});
    }
  else if (backward)
    {
      Ogre::Vector3 d(lcam->getDerivedDirection());
      forBothCameras([&](Ogre::Camera *cam){cam->move(d * -5.0 * tdelta);});
    }

  if (left)
    forBothCameras([&](Ogre::Camera *cam){cam->yaw(Ogre::Radian( 0.5 * tdelta));});
  else if (right)
    forBothCameras([&](Ogre::Camera *cam){cam->yaw(Ogre::Radian(-0.5 * tdelta));});

  auto n = lcam->getDerivedPosition();
  // camNode->setPosition(n);
  // camNode->yaw(Ogre::Radian(0.1));

  asteroid->update(tdelta);

  renderFrame();
}

void AsteroidsVRApp::handleKeyDown(int key)
{
  switch(key)
    {
    case 1:
      left = true;
      break;
    case 2:
      right = true;
      break;
    case 3:
      forward = true;
      break;
    case 4:
      backward = true;
      break;
    default:
      break;
    }
}

void AsteroidsVRApp::handleKeyUp(int key)
{
  switch(key)
    {
    case 1:
      left = false;
      break;
    case 2:
      right = false;
      break;
    case 3:
      forward = false;
      break;
    case 4:
      backward = false;
      break;
    default:
      break;
    }
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
