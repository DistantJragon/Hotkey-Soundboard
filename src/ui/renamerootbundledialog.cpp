#include "ui/renamerootbundledialog.h"
#include "ui_renamerootbundledialog.h"

RenameRootBundleDialog::RenameRootBundleDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::RenameRootBundleDialog) {
  ui->setupUi(this);
  connect(ui->nameLineEdit, &QLineEdit::textEdited, this,
          &RenameRootBundleDialog::onNameLineTextEdited);
}

RenameRootBundleDialog::~RenameRootBundleDialog() { delete ui; }

QString RenameRootBundleDialog::getNameLineEditText() const {
  return ui->nameLineEdit->text();
}

void RenameRootBundleDialog::setNameLineEditText(const std::string& text) {
  ui->nameLineEdit->setText(QString::fromStdString(text));
}

void RenameRootBundleDialog::setCurrentName(const std::string& name) {
  currentName = name;
  setNameLineEditText(name);
}

void RenameRootBundleDialog::setValidName(bool valid) {
  validName = valid;
  if (valid) {
    ui->nameLineEdit->setStyleSheet("QLineEdit { border: 1px solid black; }");
  } else {
    ui->nameLineEdit->setStyleSheet("QLineEdit { border: 1px solid red; }");
  }
}

void RenameRootBundleDialog::onNameLineTextEdited(const QString& name) {
  if (getNameLineEditText().toStdString() == currentName) {
    setValidName(true); // No change, valid by default
    return;
  }
  emit renameDialogTextEdited(name);
}

void RenameRootBundleDialog::accept() {
  if (getNameLineEditText().toStdString() == currentName) {
    // No change, accept without validation
    QDialog::reject();
    return;
  }
  if (validName) {
    QDialog::accept();
  } else {
    qWarning("Invalid sound group name. Cannot accept the dialog.");
    QDialog::reject();
  }
}
