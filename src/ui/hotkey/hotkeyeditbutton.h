#ifndef HOTKEYEDITBUTTON_H
#define HOTKEYEDITBUTTON_H

#include "core/hotkey/hotkey.h"
#include <QPushButton>

class HotkeyEditButton : public QPushButton {
  Q_OBJECT
public:
  HotkeyEditButton(QWidget* parent = nullptr);

  sb::hotkey::Hotkey hotkey() const { return hotkey_; }

public slots:
  void openHotkeyCaptureDialog();
  void setHotkey(const sb::hotkey::Hotkey& hotkey) { hotkey_ = hotkey; }

signals:
  void hotkeyChanged(const sb::hotkey::Hotkey& hotkey);

private:
  sb::hotkey::Hotkey hotkey_;
};

#endif // HOTKEYEDITBUTTON_H
