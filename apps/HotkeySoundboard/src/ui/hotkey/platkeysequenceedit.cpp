#include "platkeysequenceedit.h"
#include "qevent.h"

PlatKeySequenceEdit::PlatKeySequenceEdit(QWidget* parent)
    : QKeySequenceEdit(parent) {
  setMaximumSequenceLength(1);
}

void PlatKeySequenceEdit::clear() {
  QKeySequenceEdit::clear();
  clearNativeData();
}

void PlatKeySequenceEdit::keyPressEvent(QKeyEvent* event) {
  qDebug("Key pressed: %s", qPrintable(event->text()));
  QKeySequenceEdit::keyPressEvent(event);
  // Only keep the lowest 4 bits (Ctrl, Shift, Alt, Meta)
  Qt::KeyboardModifiers emodifiers = event->modifiers();
  nativeModifiers_ = 0;
  if (emodifiers & Qt::ShiftModifier) {
    nativeModifiers_ |= 0x4;
  }
  if (emodifiers & Qt::ControlModifier) {
    // Ctrl was pressed
    nativeModifiers_ |= 0x2;
  }
  if (emodifiers & Qt::AltModifier) {
    // Alt was pressed
    nativeModifiers_ |= 0x1;
  }
  if (emodifiers & Qt::MetaModifier) {
    // Meta was pressed
    nativeModifiers_ |= 0x8;
  }
  nativeScancode_ = event->nativeScanCode();
  nativeVirtualKey_ = event->nativeVirtualKey();
}

void PlatKeySequenceEdit::clearNativeData() {
  nativeModifiers_ = 0;
  nativeScancode_ = 0;
  nativeVirtualKey_ = 0;
}
