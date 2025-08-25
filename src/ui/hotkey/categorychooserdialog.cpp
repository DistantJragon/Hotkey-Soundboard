#include "ui/hotkey/categorychooserdialog.h"
#include "ui_categorychooserdialog.h"

CategoryChooserDialog::CategoryChooserDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::CategoryChooserDialog) {
  ui->setupUi(this);
}

CategoryChooserDialog::~CategoryChooserDialog() { delete ui; }
