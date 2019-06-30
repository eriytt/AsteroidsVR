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

  Ogre::SceneNode *shipNode = nullptr;
  Asteroid *asteroid = nullptr;
  std::vector<Ogre::SceneNode *> shots;
  bool forward = false, backward = false, left = false, right = false;

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

  // Ogre::RenderQueueListener
  virtual void renderQueueStarted(Ogre::uint8 queueGroupId, const Ogre::String &invocation, bool &skipThisInvocation);
  virtual void renderQueueEnded(Ogre::uint8 queueGroupId, const Ogre::String &invocation, bool &repeatThisInvocation);
};
