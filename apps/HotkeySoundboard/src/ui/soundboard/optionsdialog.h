#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>

namespace Ui {
class OptionsDialog;
}

class OptionsDialog : public QDialog {
  Q_OBJECT
public:
  explicit OptionsDialog(QMap<QString, QVariant> initialSettings,
                         QWidget* parent = nullptr);
  ~OptionsDialog();

  const QMap<QString, QVariant>& getOptionsMap() const { return optionsMap; }

signals:
  void applyRequested(const QMap<QString, QVariant>& optionsMap);

private:
  QMap<QString, QVariant> optionsMap;
  Ui::OptionsDialog* ui;

  void initializeMapAndUI();
  void unFlattenPageButtons();
};

#endif // OPTIONSDIALOG_H
