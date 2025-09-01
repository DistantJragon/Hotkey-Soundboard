#ifndef HOTKEYCAPTUREDIALOG_H
#define HOTKEYCAPTUREDIALOG_H

#include "core/hotkey/hotkey.h"
#include <QDialog>

namespace Ui {
class HotkeyCaptureDialog;
}

class HotkeyCaptureDialog : public QDialog {
  Q_OBJECT
public:
  explicit HotkeyCaptureDialog(QWidget* parent = nullptr);
  ~HotkeyCaptureDialog();

  sb::hotkey::Hotkey hotkey() const;

private:
  Ui::HotkeyCaptureDialog* ui;
};

#endif // HOTKEYCAPTUREDIALOG_H
