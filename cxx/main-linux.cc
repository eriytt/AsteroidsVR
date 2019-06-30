#include <unistd.h>
#include <ois/OIS.h>

#include "AsteroidsVR.hh"
#include "keycodes.h"

class EventHandler : public OIS::KeyListener
{
public:
  AsteroidsVRApp *app;
  EventHandler(AsteroidsVRApp *app) : app(app) {}
  bool keyPressed( const OIS::KeyEvent &arg ) {
    switch (arg.key)
      {
      case OIS::KC_W:
        app->handleKeyDown(AKEYCODE_W); break;
      case OIS::KC_S:
        app->handleKeyDown(AKEYCODE_S); break;
      case OIS::KC_A:
        app->handleKeyDown(AKEYCODE_A); break;
      case OIS::KC_D:
        app->handleKeyDown(AKEYCODE_D); break;
      case OIS::KC_F:
        app->handleKeyDown(AKEYCODE_F); break;
      case OIS::KC_SPACE:
        app->handleKeyDown(AKEYCODE_SPACE); break;
      default: break;
      }

    return true;
  }
  bool keyReleased( const OIS::KeyEvent &arg ) {
    switch (arg.key)
      {
      case OIS::KC_W:
        app->handleKeyUp(AKEYCODE_W); break;
      case OIS::KC_S:
        app->handleKeyUp(AKEYCODE_S); break;
      case OIS::KC_A:
        app->handleKeyUp(AKEYCODE_A); break;
      case OIS::KC_D:
        app->handleKeyUp(AKEYCODE_D); break;
      case OIS::KC_SPACE:
        app->handleKeyUp(AKEYCODE_SPACE); break;
      default: break;
      }
    return true;
  }
};

int main(int argc, char *argv[])
{
  AsteroidsVRApp *app = new AsteroidsVRApp();
  app->initialize();


  OIS::ParamList pl;
  std::ostringstream windowHndStr;

  windowHndStr << app->getWindowHandle();
  pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
  pl.insert(std::make_pair(std::string("XAutoRepeatOn"), "true"));
  pl.insert(std::make_pair(std::string("x11_keyboard_grab"), "false"));
  pl.insert(std::make_pair(std::string("x11_mouse_grab"), "false"));
  pl.insert(std::make_pair(std::string("x11_mouse_hide"), "false"));
  OIS::InputManager *mInputManager = OIS::InputManager::createInputSystem(pl);

  OIS::Keyboard *k =
    static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, true));

  EventHandler *ehandler = new EventHandler(app);
  k->setEventCallback(ehandler);

  while (true) {
    k->capture();
    app->mainLoop();
    usleep(50000);
  }
}
