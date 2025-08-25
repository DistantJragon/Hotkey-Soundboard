#ifndef CATEGORYCHOOSERDIALOG_H
#define CATEGORYCHOOSERDIALOG_H

#include <QDialog>

namespace Ui {
class CategoryChooserDialog;
}

class CategoryChooserDialog : public QDialog {
  Q_OBJECT
public:
  CategoryChooserDialog(QWidget* parent = nullptr);
  ~CategoryChooserDialog();

private:
  Ui::CategoryChooserDialog* ui;
};

#endif // CATEGORYCHOOSERDIALOG_H
