#if defined(ANDROID)
#  include "ogre-android.hh"
#else
#  include "ogre-linux.hh"
#endif

#include "Asteroid.hh"

class AsteroidsVRApp:  public OgreCardboardApp,  public Ogre::RenderQueueListener
{
public:
  const float SpaceSize = 100000.0;
  const float FarClip = 100000.0;
  const float NearClip = 100.0;

  const float MaxThrottle = 10000000.0;
  const float MaxYaw = 0.5;
  const float MaxPitch = 0.5;
  const float MaxRoll = 0.5;

private:
  const char *VertexShader =
    "precision mediump float;\n"

    "varying vec4 v_Color;\n"
    "uniform mat4 u_MVP;\n"
    "attribute vec4 vertex;\n"
    // "attribute vec4 foobar;\n"

    "void main()\n"
    "{\n"
    "	v_Color = vec4(0.0, 1.0, 0.5, 1.0) + vertex;\n"
    "   gl_Position = u_MVP * vertex + vec4(0.1, 0.1, 0.0, 0.0);\n"
    "}\n";

  const char *FragmentShader =
    "precision mediump float;\n"

    "varying vec4 v_Color;\n"

    "void main()\n"
    "{\n"
    "	gl_FragColor = v_Color;\n"
    "}\n";

  const char *ShaderMaterialScript =
    "material myshadermaterial\n"
    "{\n"
    "    // first, preferred technique\n"
    "    technique\n"
    "    {\n"
    "        // first pass\n"
    "        pass\n"
    "        {\n"
    "            vertex_program_ref vTest\n"
    "            {\n"
    "                param_named_auto u_MVP worldviewproj_matrix\n"
    "            }\n"
    "            fragment_program_ref fTest\n"
    "            {\n"
    "            }\n"
    "        }\n"
    "    }\n"
    "}\n";

  Ogre::Timer * timer = nullptr;
  unsigned long lastFrameTime_us;

  float shipMass = 75000;
  Ogre::Vector3 shipVelocity = Ogre::Vector3::ZERO;
  Ogre::SceneNode *shipNode = nullptr;
  Asteroid *asteroid = nullptr;
  std::vector<Ogre::SceneNode *> shots;
  float throttle = 0.0, yaw = 0.0, pitch = 0.0, roll = 0.0;

protected:
  void setupCamera();
  void setupResources(Ogre::ResourceGroupManager &rgm);

public:
#if defined(ANDROID)
  AsteroidsVRApp(JNIEnv *env, jobject androidSurface, gvr_context *gvr, AAssetManager* amgr)
    : OgreCardboardApp(env, androidSurface, gvr, amgr) {}
#else
    AsteroidsVRApp() : OgreCardboardApp() {}
#endif
  void initialize();
  void mainLoop();
  bool handleKeyDown(int key);
  bool handleKeyUp(int key);
  void handleJoystick(float throttle, float yaw, float pitch, float roll);

  // Ogre::RenderQueueListener
  virtual void renderQueueStarted(Ogre::uint8 queueGroupId, const Ogre::String &invocation, bool &skipThisInvocation);
  virtual void renderQueueEnded(Ogre::uint8 queueGroupId, const Ogre::String &invocation, bool &repeatThisInvocation);
};
