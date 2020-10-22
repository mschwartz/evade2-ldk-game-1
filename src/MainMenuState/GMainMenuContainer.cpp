#include "GMainMenuContainer.h"
#include "GStartWidget.h"
#include "common/GButtonWidget.h"
#include "GMainMenuProcess.h"

GMainMenuContainer::GMainMenuContainer(TInt aX, TInt aY, GMainMenuProcess *aProcess) : GDialogWidget("Menu", aX, aY) {
  mProcess = aProcess;
  AddWidget((BWidget &) *new GStartWidget());

  AddWidget((BWidget &) *new GButtonWidget("Options", GAME_STATE_MAIN_OPTIONS));
  AddWidget((BWidget &) *new GButtonWidget("Quit", GAME_STATE_QUIT));
  //gSoundPlayer.PlayMusic(MAIN_MENU_XM);

}

GMainMenuContainer::~GMainMenuContainer() = default;

TBool GMainMenuContainer::OnNavigate(TUint16 bits) {
  GDialogWidget::OnNavigate(bits);
  mProcess->ResetTimer();
  return ETrue;
}
