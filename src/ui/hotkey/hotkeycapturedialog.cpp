#include "ui/hotkey/hotkeycapturedialog.h"
#include "ui_hotkeycapturedialog.h"

HotkeyCaptureDialog::HotkeyCaptureDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::HotkeyCaptureDialog) {
  ui->setupUi(this);
#ifdef Q_OS_WIN
  ui->stackedWidget->setCurrentWidget(ui->keySequenceEditPage);
#else
  ui->stackedWidget->setCurrentWidget(ui->hotkeyNamePage);
#endif // Q_OS_WIN
}

HotkeyCaptureDialog::~HotkeyCaptureDialog() { delete ui; }

sb::hotkey::Hotkey HotkeyCaptureDialog::hotkey() const {
  sb::hotkey::Hotkey hk;
  QKeySequence ks = ui->keySequenceEdit->keySequence();
  QKeyCombination kc = ks[0];
#ifdef Q_OS_WIN
  hk.humanReadable = ks.toString(QKeySequence::NativeText).toStdString();
#else
  hk.humanReadable = ui->hotkeyNameEdit->text().toStdString();
#endif // Q_OS_WIN
  hk.portableModifiers = 0;
  if (kc.keyboardModifiers() & Qt::ControlModifier) {
    hk.portableModifiers |=
        static_cast<unsigned int>(sb::hotkey::Modifier::Control);
  }
  if (kc.keyboardModifiers() & Qt::ShiftModifier) {
    hk.portableModifiers |=
        static_cast<unsigned int>(sb::hotkey::Modifier::Shift);
  }
  if (kc.keyboardModifiers() & Qt::AltModifier) {
    hk.portableModifiers |=
        static_cast<unsigned int>(sb::hotkey::Modifier::Alt);
  }
  if (kc.keyboardModifiers() & Qt::MetaModifier) {
    hk.portableModifiers |=
        static_cast<unsigned int>(sb::hotkey::Modifier::Meta);
  }
  hk.portableKey = kc.key();
  hk.nativeModifiers = ui->keySequenceEdit->nativeModifiers();
  hk.nativeVirtualKey = ui->keySequenceEdit->nativeVirtualKey();
  hk.nativeScanCode = ui->keySequenceEdit->nativeScancode();
  return hk;
}
