#include <unistd.h>
#include <ois/OIS.h>

#include "AsteroidsVR.hh"
#include "keycodes.h"

#define O2A_LETTER(x) case OIS::KC_##x: return AKEYCODE_##x

static inline int OIS2AndroidKey(int key)
{
  switch (key)
    {
      O2A_LETTER(A);
      O2A_LETTER(B);
      O2A_LETTER(C);
      O2A_LETTER(D);
      O2A_LETTER(E);
      O2A_LETTER(F);
      O2A_LETTER(G);
      O2A_LETTER(H);
      O2A_LETTER(I);
      O2A_LETTER(J);
      O2A_LETTER(K);
      O2A_LETTER(L);
      O2A_LETTER(M);
      O2A_LETTER(N);
      O2A_LETTER(O);
      O2A_LETTER(P);
      O2A_LETTER(Q);
      O2A_LETTER(R);
      O2A_LETTER(S);
      O2A_LETTER(T);
      O2A_LETTER(U);
      O2A_LETTER(V);
      O2A_LETTER(W);
      O2A_LETTER(X);
      O2A_LETTER(Y);
      O2A_LETTER(Z);
      O2A_LETTER(SPACE);
      default: return 0;
    }

}

class EventHandler : public OIS::KeyListener
{
public:
  AsteroidsVRApp *app;
  EventHandler(AsteroidsVRApp *app) : app(app) {}
  bool keyPressed( const OIS::KeyEvent &arg ) {
    return app->handleKeyDown(OIS2AndroidKey(arg.key));
  }
  bool keyReleased( const OIS::KeyEvent &arg ) {
    return app->handleKeyUp(OIS2AndroidKey(arg.key));
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
