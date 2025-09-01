#include "ui/hotkey/actionchooserdialog.h"
#include "ui_actionchooserdialog.h"

ActionChooserDialog::ActionChooserDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::ActionChooserDialog) {
  ui->setupUi(this);
}

ActionChooserDialog::~ActionChooserDialog() { delete ui; }
