#include "hotkeymanagerdialog.h"
#include "ui/hotkey/hotkeyguitypes.h"
#include "ui_hotkeymanagerdialog.h"

HotkeyManagerDialog::HotkeyManagerDialog(QWidget* parent, sb::Soundboard* sb,
                                         HotkeyTableModel* model)
    : QDialog(parent), ui(new Ui::HotkeyManagerDialog), soundboard(sb),
      finalHotkeyModel(model) {
  ui->setupUi(this);
  hotkeyModel = new HotkeyTableModel(this, soundboard);
  if (finalHotkeyModel) {
    hotkeyModel->loadFromRows(finalHotkeyModel->getRows());
    hotkeyModel->setCategoryNames(finalHotkeyModel->getCategoryNames());
  }
  loadSortedCategoryNames();
  setupHotkeyEditor();
  ui->removeButton->setEnabled(false);

  ui->tableView->setModel(hotkeyModel);

  connect(ui->tableView->selectionModel(),
          &QItemSelectionModel::selectionChanged, this,
          &HotkeyManagerDialog::updateHotkeyEditor);
  connect(hotkeyModel, &HotkeyTableModel::categoryNamesChanged, this,
          &HotkeyManagerDialog::updateSortedCategoryNames);

  connect(ui->addButton, &QPushButton::clicked, this,
          &HotkeyManagerDialog::newHotkeyRow);
  connect(ui->removeButton, &QPushButton::clicked, this,
          &HotkeyManagerDialog::deleteSelectedHotkeyRows);
  connect(ui->saveHotkeyButton, &QPushButton::clicked, this,
          &HotkeyManagerDialog::applyHotkeyChanges);
  connect(ui->actionTypeComboBox, &QComboBox::currentIndexChanged, this,
          &HotkeyManagerDialog::updateActionTargetComboBox);
  connect(ui->buttonBox, &QDialogButtonBox::accepted, this,
          &HotkeyManagerDialog::accept);
  connect(ui->buttonBox, &QDialogButtonBox::rejected, this,
          &HotkeyManagerDialog::reject);
  // Get the apply button in the button box
  auto applyButton = ui->buttonBox->button(QDialogButtonBox::Apply);
  connect(applyButton, &QPushButton::clicked, this,
          [this]() { emit applyRequested(hotkeyModel); });

  connect(ui->hotkeyEditButton, &HotkeyEditButton::hotkeyChanged, this,
          [this](const sb::hotkey::Hotkey& hk) {
            ui->hotkeyKeyDataLabel->setText(
                QString::fromStdString(hk.humanReadable));
          });
  ui->hotkeyKeyDataLabel->setText("<None>");
}

HotkeyManagerDialog::~HotkeyManagerDialog() {
  if (hotkeyModel) {
    delete hotkeyModel;
  }
  delete ui;
}

void HotkeyManagerDialog::newHotkeyRow() {
  bool result = hotkeyModel->insertRows(hotkeyModel->rowCount({}), 1);
  if (!result) {
    return;
  }
  // Select the new row
  auto newIndex = hotkeyModel->index(hotkeyModel->rowCount({}) - 1, 0);
  ui->tableView->selectionModel()->clearSelection();
  ui->tableView->selectionModel()->select(
      newIndex, QItemSelectionModel::Select | QItemSelectionModel::Rows);
}

void HotkeyManagerDialog::updateHotkeyEditor() {
  auto selection = ui->tableView->selectionModel()->selectedRows();
  if (selection.empty()) {
    fillHotkeyEditor();
    setHotkeyEditorEnabled(false);
    ui->removeButton->setEnabled(false);
    return;
  } else if (selection.size() == 1) {
    int row = selection.first().row();
    const auto& r = hotkeyModel->getRows().at(row);
    fillHotkeyEditor(r);
    setHotkeyEditorEnabled(true);
    ui->removeButton->setEnabled(true);
    return;
  } else {
    fillHotkeyEditor();
    setHotkeyEditorEnabled(false);
    ui->removeButton->setEnabled(true);
    return;
  }
}

void HotkeyManagerDialog::applyHotkeyChanges() {
  auto selection = ui->tableView->selectionModel()->selectedRows();
  if (selection.size() != 1) {
    return;
  }
  int row = selection.first().row();
  hotkeyModel->setData(
      hotkeyModel->index(row, static_cast<int>(HotkeyColumn::Enabled)),
      ui->enabledCheckBox->isChecked() ? Qt::Checked : Qt::Unchecked,
      Qt::CheckStateRole);
  std::string catName = ui->categoryComboBox->currentText().toStdString();
  hotkeyModel->setCategoryAt(row, catName);
  hotkeyModel->setKeyDataAt(row, ui->hotkeyEditButton->hotkey());
  HotkeyAction action;
  action.type = static_cast<HotkeyAction::Type>(
      ui->actionTypeComboBox->currentData().toInt());
  switch (action.type) {
  case HotkeyAction::Type::ChangeCategory:
    action.targetCategory = hotkeyModel->nameToCategoryHandle(
        ui->actionTargetComboBox->currentText().toStdString());
    break;
  case HotkeyAction::Type::PlayEntry:
  case HotkeyAction::Type::StopEntry:
    action.targetEntry = ui->actionTargetComboBox->currentData().toULongLong();
    break;
  default:
    break;
  }
  hotkeyModel->setActionAt(row, action);
}

void HotkeyManagerDialog::deleteSelectedHotkeyRows() {
  auto selection = ui->tableView->selectionModel()->selectedRows();
  QList<int> rows;
  for (const auto& idx : selection) {
    rows.append(idx.row());
  }
  std::sort(rows.begin(), rows.end(), std::greater<int>());
  for (const auto& r : rows) {
    hotkeyModel->removeRows(r, 1);
  }
}

void HotkeyManagerDialog::fillHotkeyEditor(const HotkeyRow& row) {
  ui->enabledCheckBox->setChecked(row.enabled);
  fillCategoryCombo(row.category);
  ui->hotkeyKeyDataLabel->setText(
      QString::fromStdString(row.hotkey.humanReadable));
  ui->hotkeyEditButton->setHotkey(row.hotkey);
  fillActionTypeCombo(row.action.type);
  updateActionTargetComboBox();
}

void HotkeyManagerDialog::fillCategoryCombo(CategoryHandle selected) {
  ui->categoryComboBox->clear();
  auto it = hotkeyModel->getCategoryNames().find(selected);
  std::string catName;
  if (it != hotkeyModel->getCategoryNames().end()) {
    catName = it->second;
  }
  for (const auto& name : sortedCategoryNames) {
    ui->categoryComboBox->addItem(QString::fromStdString(name));
    if (it != hotkeyModel->getCategoryNames().end() && name == catName) {
      ui->categoryComboBox->setCurrentIndex(ui->categoryComboBox->count() - 1);
    }
  }
}

void HotkeyManagerDialog::fillActionTypeCombo(HotkeyAction::Type action) {
  ui->actionTypeComboBox->clear();
  ui->actionTypeComboBox->addItem("None",
                                  static_cast<int>(HotkeyAction::Type::None));
  ui->actionTypeComboBox->addItem(
      "Change Category", static_cast<int>(HotkeyAction::Type::ChangeCategory));
  ui->actionTypeComboBox->addItem(
      "Play Entry", static_cast<int>(HotkeyAction::Type::PlayEntry));
  ui->actionTypeComboBox->addItem(
      "Stop Entry", static_cast<int>(HotkeyAction::Type::StopEntry));
  ui->actionTypeComboBox->addItem(
      "Stop All", static_cast<int>(HotkeyAction::Type::StopAll));
  switch (action) {
  case HotkeyAction::Type::None:
    ui->actionTypeComboBox->setCurrentIndex(0);
    break;
  case HotkeyAction::Type::ChangeCategory:
    ui->actionTypeComboBox->setCurrentIndex(1);
    break;
  case HotkeyAction::Type::PlayEntry:
    ui->actionTypeComboBox->setCurrentIndex(2);
    break;
  case HotkeyAction::Type::StopEntry:
    ui->actionTypeComboBox->setCurrentIndex(3);
    break;
  case HotkeyAction::Type::StopAll:
    ui->actionTypeComboBox->setCurrentIndex(4);
    break;
  default:
    ui->actionTypeComboBox->setCurrentIndex(0);
    break;
  }
}

void HotkeyManagerDialog::fillTargetCategoryCombo(CategoryHandle selected) {
  ui->actionTargetComboBox->clear();
  auto it = hotkeyModel->getCategoryNames().find(selected);
  std::string catName;
  if (it != hotkeyModel->getCategoryNames().end()) {
    catName = it->second;
  }
  for (const auto& name : sortedCategoryNames) {
    ui->actionTargetComboBox->addItem(QString::fromStdString(name));
    if (it != hotkeyModel->getCategoryNames().end() && name == catName) {
      ui->actionTargetComboBox->setCurrentIndex(
          ui->actionTargetComboBox->count() - 1);
    }
  }
}

void HotkeyManagerDialog::fillTargetEntryCombo(sb::EntryHandle selected) {
  ui->actionTargetComboBox->clear();
  if (!soundboard) {
    ui->actionTargetComboBox->setEditText("<Error: No Soundboard>");
    return;
  }
  const auto& entries = soundboard->getEntries();
  for (const auto& [handle, entry] : entries) {
    if (!entry) {
      continue;
    }
    if (entry->getParentHandle() != sb::InvalidEntryHandle) {
      // Skip non-root entries
      continue;
    }
    ui->actionTargetComboBox->addItem(
        QString::fromStdString(entry->getName()),
        static_cast<unsigned long long>(entry->getHandle()));
    if (handle == selected) {
      ui->actionTargetComboBox->setCurrentIndex(
          ui->actionTargetComboBox->count() - 1);
    }
  }
}

void HotkeyManagerDialog::updateActionTargetComboBox() {
  HotkeyAction::Type action = static_cast<HotkeyAction::Type>(
      ui->actionTypeComboBox->currentData().toInt());
  switch (action) {
  case HotkeyAction::Type::ChangeCategory:
    fillTargetCategoryCombo();
    ui->actionTargetComboBox->show();
    break;
  case HotkeyAction::Type::PlayEntry:
  case HotkeyAction::Type::StopEntry:
    fillTargetEntryCombo();
    ui->actionTargetComboBox->show();
    break;
  default:
    ui->actionTargetComboBox->clear();
    ui->actionTargetComboBox->setEditText("<N/A>");
    ui->actionTargetComboBox->hide();
    break;
  }
}

bool HotkeyManagerDialog::compareCategoryNames(const std::string& name1,
                                               const std::string& name2) const {
  if (name1 == "Global") {
    return true;
  } else if (name2 == "Global") {
    return false;
  } else {
    return name1 < name2;
  }
}

void HotkeyManagerDialog::setupHotkeyEditor() {
  fillCategoryCombo();
  fillActionTypeCombo();
  ui->actionTargetComboBox->hide();
  setHotkeyEditorEnabled(false);
}

void HotkeyManagerDialog::setHotkeyEditorEnabled(bool enabled) {
  ui->hotkeyDataWidget->setEnabled(enabled);
  ui->saveHotkeyButton->setEnabled(enabled);
}
void HotkeyManagerDialog::loadSortedCategoryNames() {
  sortedCategoryNames.clear();
  const auto& catNamesMap = hotkeyModel->getCategoryNames();
  for (const auto& [handle, name] : catNamesMap) {
    sortedCategoryNames.push_back(name);
  }
  std::sort(sortedCategoryNames.begin(), sortedCategoryNames.end(),
            [this](const std::string& a, const std::string& b) {
              return compareCategoryNames(a, b);
            });
  fillCategoryCombo();
  if (ui->actionTypeComboBox->currentData().toInt() ==
      static_cast<int>(HotkeyAction::Type::ChangeCategory)) {
    fillTargetCategoryCombo();
  }
}

void HotkeyManagerDialog::updateSortedCategoryNames(
    QList<CategoryHandle> added, QList<CategoryHandle> removed) {
  const auto& catNamesMap = hotkeyModel->getCategoryNames();
  for (const auto& handle : removed) {
    auto it = catNamesMap.find(handle);
    if (it != catNamesMap.end()) {
      auto nameIt = std::find(sortedCategoryNames.begin(),
                              sortedCategoryNames.end(), it->second);
      if (nameIt != sortedCategoryNames.end()) {
        sortedCategoryNames.erase(nameIt);
      }
    }
  }
  for (const auto& handle : added) {
    auto it = catNamesMap.find(handle);
    if (it != catNamesMap.end()) {
      auto nameIt = std::find(sortedCategoryNames.begin(),
                              sortedCategoryNames.end(), it->second);
      auto lower = std::lower_bound(
          sortedCategoryNames.begin(), sortedCategoryNames.end(), it->second,
          [this](const std::string& a, const std::string& b) {
            return compareCategoryNames(a, b);
          });
      if (nameIt == sortedCategoryNames.end()) {
        sortedCategoryNames.insert(lower, it->second);
      }
    }
  }
  fillCategoryCombo();
  if (ui->actionTypeComboBox->currentData().toInt() ==
      static_cast<int>(HotkeyAction::Type::ChangeCategory)) {
    fillTargetCategoryCombo();
  }
}
