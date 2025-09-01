#ifndef HOTKEYMANAGERDIALOG_H
#define HOTKEYMANAGERDIALOG_H

#include "core/soundboard/soundboard.h"
#include "ui/hotkey/hotkeyguitypes.h"
#include "ui/hotkey/hotkeytablemodel.h"
#include <QDialog>

namespace Ui {
class HotkeyManagerDialog;
}

class HotkeyManagerDialog : public QDialog {
  Q_OBJECT

public:
  explicit HotkeyManagerDialog(QWidget* parent = nullptr,
                               sb::Soundboard* soundboard = nullptr,
                               HotkeyTableModel* model = nullptr);
  ~HotkeyManagerDialog();

  HotkeyTableModel* getHotkeyModel() const { return hotkeyModel; }

public slots:
  void updateSortedCategoryNames(QList<CategoryHandle> added,
                                 QList<CategoryHandle> removed);

signals:
  void applyRequested(HotkeyTableModel* model);

private slots:
  void newHotkeyRow();
  // void openCategoryEditor();
  void updateHotkeyEditor();
  void applyHotkeyChanges();
  void deleteSelectedHotkeyRows();
  void fillHotkeyEditor(const HotkeyRow& row = {});
  void fillCategoryCombo(CategoryHandle selected = InvalidCategoryHandle);
  void
  fillActionTypeCombo(HotkeyAction::Type action = HotkeyAction::Type::None);
  void fillTargetCategoryCombo(CategoryHandle selected = InvalidCategoryHandle);
  void fillTargetEntryCombo(sb::EntryHandle selected = sb::InvalidEntryHandle);
  void loadSortedCategoryNames();
  void updateActionTargetComboBox();
  // void accept() override;
  // void reject() override;

private:
  std::vector<std::string> sortedCategoryNames;
  Ui::HotkeyManagerDialog* ui;
  sb::Soundboard* const soundboard = nullptr;
  HotkeyTableModel* finalHotkeyModel = nullptr;
  HotkeyTableModel* hotkeyModel = nullptr;

  bool compareCategoryNames(const std::string& name1,
                            const std::string& name2) const;
  void setupHotkeyEditor();
  void setHotkeyEditorEnabled(bool enabled);
};

#endif // HOTKEYMANAGERDIALOG_H
