#include "ui/hotkey/hotkeyeditbutton.h"
#include "ui/hotkey/hotkeycapturedialog.h"

HotkeyEditButton::HotkeyEditButton(QWidget* parent) : QPushButton(parent) {
  connect(this, &QPushButton::clicked, this,
          &HotkeyEditButton::openHotkeyCaptureDialog);
}

void HotkeyEditButton::openHotkeyCaptureDialog() {
  HotkeyCaptureDialog dlg;
  if (dlg.exec() == QDialog::Accepted) {
    hotkey_ = dlg.hotkey();
  }
  emit hotkeyChanged(hotkey_);
}
