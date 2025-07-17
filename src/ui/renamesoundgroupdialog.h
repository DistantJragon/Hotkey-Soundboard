#ifndef RENAMESOUNDGROUPDIALOG_H
#define RENAMESOUNDGROUPDIALOG_H

#include <QDialog>

namespace Ui {
class RenameSoundGroupDialog;
}

class RenameSoundGroupDialog : public QDialog {
  Q_OBJECT

public:
  explicit RenameSoundGroupDialog(QWidget* parent = nullptr);
  ~RenameSoundGroupDialog();

  QString getNameLineEditText() const;
  void setNameLineEditText(const std::string& text);
  void setCurrentName(const std::string& name);
  bool isValidName() const { return validName; }
  void setValidName(bool valid);

signals:
  void renameDialogTextEdited(const QString& name);

public slots:
  void onNameLineTextEdited(const QString& name);

protected:
  void accept() override;

private:
  Ui::RenameSoundGroupDialog* ui;
  std::string currentName;
  bool validName = true;
};

#endif // RENAMESOUNDGROUPDIALOG_H
