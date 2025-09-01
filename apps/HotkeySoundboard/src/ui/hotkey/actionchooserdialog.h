#ifndef ACTIONCHOOSERDIALOG_H
#define ACTIONCHOOSERDIALOG_H

#include <QDialog>

namespace Ui {
class ActionChooserDialog;
}

class ActionChooserDialog : public QDialog {
  Q_OBJECT
public:
  ActionChooserDialog(QWidget* parent = nullptr);
  ~ActionChooserDialog();

private:
  Ui::ActionChooserDialog* ui;
};

#endif // ACTIONCHOOSERDIALOG_H
