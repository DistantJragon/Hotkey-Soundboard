#include "ui/renamesoundgroupdialog.h"
#include "ui_renamesoundgroupdialog.h"

RenameSoundGroupDialog::RenameSoundGroupDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::RenameSoundGroupDialog) {
  ui->setupUi(this);
  connect(ui->nameLineEdit, &QLineEdit::textEdited, this,
          &RenameSoundGroupDialog::onNameLineTextEdited);
}

RenameSoundGroupDialog::~RenameSoundGroupDialog() { delete ui; }

QString RenameSoundGroupDialog::getNameLineEditText() const {
  return ui->nameLineEdit->text();
}

void RenameSoundGroupDialog::setNameLineEditText(const std::string& text) {
  ui->nameLineEdit->setText(QString::fromStdString(text));
}

void RenameSoundGroupDialog::setCurrentName(const std::string& name) {
  currentName = name;
  setNameLineEditText(name);
}

void RenameSoundGroupDialog::setValidName(bool valid) {
  validName = valid;
  if (valid) {
    ui->nameLineEdit->setStyleSheet("QLineEdit { border: 1px solid black; }");
  } else {
    ui->nameLineEdit->setStyleSheet("QLineEdit { border: 1px solid red; }");
  }
}

void RenameSoundGroupDialog::onNameLineTextEdited(const QString& name) {
  if (getNameLineEditText().toStdString() == currentName) {
    setValidName(true); // No change, valid by default
    return;
  }
  emit renameDialogTextEdited(name);
}

void RenameSoundGroupDialog::accept() {
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
