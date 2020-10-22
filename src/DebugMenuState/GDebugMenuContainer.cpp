#include "GDebugMenuContainer.h"
#include "GDebugWidget.h"
#include "GLevelUpPlayer.h"
#include "GDebugButtonWidget.h"
#include "GGameState.h"

static const TUint8 WIDGETS_PER_SCREEN = 12;

GDebugMenuContainer::GDebugMenuContainer(TInt aX, TInt aY, BGameEngine *aEngine) : GDialogWidget("", aX, aY) {
  mTimer = 30;
  mEngine = aEngine;
  SetState(DEBUG_MAIN);
}

GDebugMenuContainer::~GDebugMenuContainer() = default;

void GDebugMenuContainer::SetState(TInt aState) {
  mSkip = 0;
  mDir = 0;
  mBuffer = 0;
  mState = aState;

  mList.Reset();
  mLength = 0;

  if (mCurrentWidget) {
    mCurrentWidget->Deactivate();
    delete mCurrentWidget;
  }

  if (mState == DEBUG_MAIN) {
    mTitle = (char*)"DEBUG MODE";
    AddWidget((BWidget &) *new GDebugWidget());
    AddWidget((BWidget &) *new GLevelUpPlayer());
  }

  mCurrentWidget = mList.First();
  mCurrentWidget->Activate();
}

void GDebugMenuContainer::AddWidget(BWidget &aWidget) {
  mLength++;
  GDialogWidget::AddWidget(aWidget);
}

void GDebugMenuContainer::Run() {
  GDialogWidget::Run();

  if (gControls.WasPressed(JOYLEFT) && OnNavigate(JOYLEFT) && mCurrentWidget->OnNavigate(JOYLEFT)) {
    mCurrentWidget->Deactivate();
    mCurrentWidget = mList.First();
    mCurrentWidget->Activate();
    gControls.dKeys = 0;
  }

  if (gControls.WasPressed(JOYRIGHT) && OnNavigate(JOYRIGHT) && mCurrentWidget->OnNavigate(JOYRIGHT)) {
    mCurrentWidget->Deactivate();
    mCurrentWidget = mList.Last();
    mCurrentWidget->Activate();
    gControls.dKeys = 0;
  }
}

TBool GDebugMenuContainer::OnNavigate(TUint16 aBits) {
  if (aBits & JOYUP) {
    if (mDir > 0) {
      mBuffer = mBuffer ? WIDGETS_PER_SCREEN - mBuffer : WIDGETS_PER_SCREEN;
    }
    if (mBuffer > 0) {
      mBuffer--;
    } else {
      mSkip--;
    }
    mDir = -1;
  }
  else if (aBits & JOYDOWN) {
    if (mDir < 0) {
      mBuffer = mBuffer ? WIDGETS_PER_SCREEN - mBuffer : WIDGETS_PER_SCREEN;
    }
    if (mBuffer > 0) {
      mBuffer--;
    } else {
      mSkip++;
    }
    mDir = 1;
  }

  return ETrue;
}

TInt GDebugMenuContainer::Render(TInt aX, TInt aY) {
  TUint8 color = gWidgetTheme.GetInt(WIDGET_TEXT_BG);
  const BFont *f = gWidgetTheme.GetFont(WIDGET_TITLE_FONT);
  const TInt x = (SCREEN_WIDTH - (strlen(mTitle) * f->mWidth)) / 2;

  if (--mTimer < 0) {
    mTimer = 30;
  } else if (mTimer >= 15) {
    color = gWidgetTheme.GetInt(WIDGET_TEXT_BG);
  } else if (mTimer < 15) {
    color = gWidgetTheme.GetInt(WIDGET_TITLE_FG);
  }

  gDisplay.renderBitmap->DrawStringShadow(
      ENull,
      mTitle,
      f,
      aX + x, aY + 16,
      color,
      COLOR_TEXT_SHADOW,
      COLOR_TEXT_TRANSPARENT,
      -6
      );

  if (mCurrentWidget == mList.First()) {
    mBuffer = 0;
    mSkip = 0;
    mDir = 0;
  } else if (mCurrentWidget == mList.Last()) {
    mBuffer = WIDGETS_PER_SCREEN;
    mSkip = mLength - 1;
    mDir = 0;
  }

  TInt h = 0;
  TInt i = 0;
  for (BWidget *w = mList.First(); !mList.End(w); w = mList.Next(w), i++) {
    if (i + WIDGETS_PER_SCREEN < mSkip) {
      continue;
    }

    TInt dy = w->RenderTitle(aX + mX, aY + mY, w == mCurrentWidget);
    aY += dy;
    h += dy;

    dy =w->Render(aX + mX, aY + mY);  // render relative to mY
    aY += dy;
    h += dy;
  }

  return h;
}
