#include "hotkeytablemodel.h"
#include <QBrush>
#include <unordered_set>

HotkeyTableModel::HotkeyTableModel(QObject* parent, sb::Soundboard* soundboard)
    : QAbstractTableModel(parent), soundboard(soundboard) {
  categoryNames[GlobalCategoryHandle] = "Global";
}

QVariant HotkeyTableModel::data(const QModelIndex& index, int role) const {
  const auto& r = rows.at(index.row());
  switch (role) {
  case Qt::DisplayRole:
    switch (static_cast<HotkeyColumn>(index.column())) {
    case HotkeyColumn::Enabled:
      return QVariant();
    case HotkeyColumn::Category:
      if (categoryNames.count(r.category)) {
        return QString::fromStdString(categoryNames.at(r.category));
      } else {
        return QString("<Unknown>");
      }
    case HotkeyColumn::Hotkey:
      return QString::fromStdString(r.hotkey.humanReadable);
    case HotkeyColumn::Action:
      return actionToString(r.action);
    }
    break;
  case Qt::CheckStateRole:
    if (static_cast<HotkeyColumn>(index.column()) == HotkeyColumn::Enabled) {
      return r.enabled ? Qt::Checked : Qt::Unchecked;
    }
    break;
  case Qt::BackgroundRole:
    if (r.conflict) {
      return QBrush(QColor(255, 0, 0, 255 * 0.2));
    }
    break;
  case Qt::TextAlignmentRole:
    if (static_cast<HotkeyColumn>(index.column()) == HotkeyColumn::Enabled) {
      return Qt::AlignCenter;
    } else {
      return int(Qt::AlignVCenter | Qt::AlignLeft);
    }
    break;
  }
  if (static_cast<HotkeyColumn>(index.column()) == HotkeyColumn::Hotkey) {
    switch (static_cast<HotkeyRoles>(role)) {
    case HotkeyRoles::HumanReadableRole:
      return QString::fromStdString(r.hotkey.humanReadable);
      break;
    case HotkeyRoles::PortableModifiersRole:
      return static_cast<unsigned long long>(r.hotkey.portableModifiers);
      break;
    case HotkeyRoles::PortableKeyRole:
      return static_cast<unsigned long long>(r.hotkey.portableKey);
      break;
    case HotkeyRoles::NativeModifiersRole:
      return static_cast<unsigned long long>(r.hotkey.nativeModifiers);
      break;
    case HotkeyRoles::NativeVirtualKeyRole:
      return static_cast<unsigned long long>(r.hotkey.nativeVirtualKey);
      break;
    case HotkeyRoles::NativeScanCodeRole:
      return static_cast<unsigned long long>(r.hotkey.nativeScanCode);
      break;
    default:
      break;
    }
  } else if (static_cast<HotkeyColumn>(index.column()) ==
             HotkeyColumn::Action) {
    switch (static_cast<HotkeyRoles>(role)) {
    case HotkeyRoles::ActionTypeRole:
      return static_cast<int>(r.action.type);
      break;
    case HotkeyRoles::ActionTargetCategoryRole:
      return static_cast<unsigned long long>(r.action.targetCategory);
      break;
    case HotkeyRoles::ActionTargetEntryRole:
      return static_cast<unsigned long long>(r.action.targetEntry);
      break;
    default:
      break;
    }
  }
  return QVariant();
}

bool HotkeyTableModel::setData(const QModelIndex& idx, const QVariant& value,
                               int role) {
  if (!idx.isValid()) {
    return false;
  }
  auto& r = rows[idx.row()];
  switch (role) {
  case Qt::CheckStateRole:
    if (static_cast<HotkeyColumn>(idx.column()) == HotkeyColumn::Enabled) {
      r.enabled = (value.toInt() == Qt::Checked);
      emit dataChanged(idx, idx);
      return true;
    }
    break;
  }
  if (static_cast<HotkeyColumn>(idx.column()) == HotkeyColumn::Hotkey) {
    switch (static_cast<HotkeyRoles>(role)) {
    case HotkeyRoles::HumanReadableRole:
      r.hotkey.humanReadable = value.toString().toStdString();
      emit dataChanged(idx, idx);
      return true;
      break;
    case HotkeyRoles::PortableModifiersRole:
      r.hotkey.portableModifiers = value.toUInt();
      emit dataChanged(idx, idx);
      return true;
      break;
    case HotkeyRoles::PortableKeyRole:
      r.hotkey.portableKey = value.toUInt();
      emit dataChanged(idx, idx);
      return true;
      break;
    case HotkeyRoles::NativeModifiersRole:
      r.hotkey.nativeModifiers = value.toUInt();
      emit dataChanged(idx, idx);
      return true;
      break;
    case HotkeyRoles::NativeVirtualKeyRole:
      r.hotkey.nativeVirtualKey = value.toUInt();
      emit dataChanged(idx, idx);
      return true;
      break;
    case HotkeyRoles::NativeScanCodeRole:
      r.hotkey.nativeScanCode = value.toUInt();
      emit dataChanged(idx, idx);
      return true;
      break;
    default:
      break;
    }
  } else if (static_cast<HotkeyColumn>(idx.column()) == HotkeyColumn::Action) {
    switch (static_cast<HotkeyRoles>(role)) {
    case HotkeyRoles::ActionTypeRole:
      r.action.type = static_cast<HotkeyAction::Type>(value.toInt());
      emit dataChanged(idx, idx);
      return true;
      break;
    case HotkeyRoles::ActionTargetCategoryRole:
      r.action.targetCategory = value.toUInt();
      emit dataChanged(idx, idx);
      return true;
      break;
    case HotkeyRoles::ActionTargetEntryRole:
      r.action.targetEntry = value.toUInt();
      emit dataChanged(idx, idx);
      return true;
      break;
    default:
      break;
    }
  }
  return false;
}

Qt::ItemFlags HotkeyTableModel::flags(const QModelIndex& index) const {
  Qt::ItemFlags f = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
  if (static_cast<HotkeyColumn>(index.column()) == HotkeyColumn::Enabled) {
    f |= Qt::ItemIsUserCheckable;
  }
  return f;
}

QVariant HotkeyTableModel::headerData(int section, Qt::Orientation orientation,
                                      int role) const {
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    switch (static_cast<HotkeyColumn>(section)) {
    case HotkeyColumn::Enabled:
      return tr("Enabled");
    case HotkeyColumn::Category:
      return tr("Category");
    case HotkeyColumn::Hotkey:
      return tr("Hotkey");
    case HotkeyColumn::Action:
      return tr("Action");
    }
  }
  return QVariant();
}

bool HotkeyTableModel::insertRows(int row, int count,
                                  const QModelIndex& parent) {
  if (row < 0 || row > rows.size()) {
    return false;
  }
  if (count < 0) {
    return false;
  }
  if (count == 0) {
    return true;
  }
  beginInsertRows(parent, row, row + count - 1);
  for (int i = 0; i < count; ++i) {
    rows.insert(row, HotkeyRow());
  }
  endInsertRows();
  return true;
}

bool HotkeyTableModel::removeRows(int row, int count,
                                  const QModelIndex& parent) {
  if (row < 0 || row + count > rows.size()) {
    return false;
  }
  if (count < 0) {
    return false;
  }
  if (count == 0) {
    return true;
  }
  beginRemoveRows(parent, row, row + count - 1);
  for (int i = 0; i < count; ++i) {
    rows.removeAt(row);
  }
  endRemoveRows();
  return true;
}

void HotkeyTableModel::setCategoryAt(int row, CategoryHandle category) {
  if (row < 0 || row >= rows.size() || !categoryNames.count(category)) {
    return;
  }
  if (rows[row].category == category) {
    return;
  }
  if (category == InvalidCategoryHandle) {
    return;
  }
  rows[row].category = category;
  cleanCategories();
  checkAllConflicts();
  int col = static_cast<int>(HotkeyColumn::Category);
  emit dataChanged(index(row, col), index(row, col));
}

void HotkeyTableModel::setKeyDataAt(int row, const sb::hotkey::Hotkey& hotkey) {
  if (row < 0 || row >= rows.size()) {
    return;
  }
  rows[row].hotkey.setKeyData(hotkey);
  checkAllConflicts();
  int col = static_cast<int>(HotkeyColumn::Hotkey);
  emit dataChanged(index(row, col), index(row, col));
}

void HotkeyTableModel::setCategoryAt(int row, const std::string& categoryName) {
  if (row < 0 || row >= rows.size()) {
    return;
  }
  if (categoryName.empty()) {
    return;
  }
  CategoryHandle handle = nameToCategoryHandle(categoryName);
  if (handle != InvalidCategoryHandle) {
    setCategoryAt(row, handle);
    return;
  }
  // Create a new category
  CategoryHandle newHandle = 1;
  while (categoryNames.count(newHandle)) {
    ++newHandle;
  }
  categoryNames[newHandle] = categoryName;
  rows[row].category = newHandle;
  emit categoryNamesChanged({newHandle}, {});
  cleanCategories();
  checkAllConflicts();
  int col = static_cast<int>(HotkeyColumn::Category);
  emit dataChanged(index(row, col), index(row, col));
}

CategoryHandle HotkeyTableModel::addCategoryName(const std::string& name) {
  if (name.empty()) {
    return InvalidCategoryHandle;
  }
  for (const auto& [handle, catName] : categoryNames) {
    if (catName == name) {
      return handle;
    }
  }
  CategoryHandle handle = 1;
  while (categoryNames.count(handle)) {
    ++handle;
  }
  categoryNames[handle] = name;
  emit categoryNamesChanged({handle}, {});
  return handle;
}

CategoryHandle HotkeyTableModel::addCategoryName(const std::string& name,
                                                 CategoryHandle hint) {
  if (name.empty()) {
    return InvalidCategoryHandle;
  }
  if (categoryNames.count(hint)) {
    return hint;
  }
  CategoryHandle handle = hint;
  categoryNames[handle] = name;
  emit categoryNamesChanged({handle}, {});
  return handle;
}

void HotkeyTableModel::cleanCategories() {
  std::unordered_set<CategoryHandle> usedCategories;
  for (const auto& r : rows) {
    usedCategories.insert(r.category);
  }
  QList<CategoryHandle> removed;
  for (const auto& [handle, name] : categoryNames) {
    if (!usedCategories.count(handle) && handle != GlobalCategoryHandle) {
      categoryNames.erase(handle);
      removed.append(handle);
    }
  }
  if (!removed.empty()) {
    for (auto& row : rows) {
      if (removed.contains(row.action.targetCategory)) {
        row.action.targetCategory = GlobalCategoryHandle;
      }
    }
    emit categoryNamesChanged({}, removed);
  }
}

void HotkeyTableModel::setActionAt(int row, const HotkeyAction& action) {
  if (row < 0 || row >= rows.size()) {
    return;
  }
  rows[row].action = action;
  int col = static_cast<int>(HotkeyColumn::Action);
  emit dataChanged(index(row, col), index(row, col));
}

void HotkeyTableModel::checkAllConflicts() {
  // Check each hotkey against all others with double for loop
  // TODO: Optimize this
  for (auto& r : rows) {
    r.conflict = false;
  }
  for (int i = 0; i < rows.size(); ++i) {
    if (!rows[i].enabled) {
      continue;
    }
    for (int j = i + 1; j < rows.size(); ++j) {
      if (!rows[j].enabled || (rows[i].category != rows[j].category &&
                               rows[i].category != GlobalCategoryHandle &&
                               rows[j].category != GlobalCategoryHandle)) {
        continue;
      }
      if (checkConflictAt(i, j)) {
        rows[i].conflict = true;
        rows[j].conflict = true;
        emit dataChanged(index(i, 0), index(i, columnCount({}) - 1));
        emit dataChanged(index(j, 0), index(j, columnCount({}) - 1));
      }
    }
  }
}

#ifdef Q_OS_WIN
bool HotkeyTableModel::checkConflictAt(int a, int b) const {
  if (a < 0 || a >= rows.size() || b < 0 || b >= rows.size()) {
    return false;
  }
  return rows[a].hotkey.nativeEquals(rows[b].hotkey);
}
#else
bool HotkeyTableModel::checkConflictAt(int a, int b) const {
  if (a < 0 || a >= rows.size() || b < 0 || b >= rows.size()) {
    return false;
  }
  return rows[a].hotkey.humanReadable == rows[b].hotkey.humanReadable;
}
#endif // Q_OS_WIN

QString HotkeyTableModel::actionToString(const HotkeyAction& action) const {
  switch (action.type) {
  case HotkeyAction::Type::ChangeCategory:
    if (categoryNames.count(action.targetCategory)) {
      return QString("Switch to \"%1\"")
          .arg(QString::fromStdString(categoryNames.at(action.targetCategory)));
    } else {
      return QString("Switch to <Unknown>");
    }
  case HotkeyAction::Type::PlayEntry:
    if (soundboard) {
      auto entry = soundboard->getEntry(action.targetEntry);
      if (entry) {
        return QString("Play \"%1\"")
            .arg(QString::fromStdString(entry->getName()));
      } else {
        return QString("Play <Unknown>");
      }
    } else {
      return QString("Play <No Soundboard>");
    }
  case HotkeyAction::Type::StopEntry:
    if (soundboard) {
      auto entry = soundboard->getEntry(action.targetEntry);
      if (entry) {
        return QString("Stop \"%1\"")
            .arg(QString::fromStdString(entry->getName()));
      } else {
        return QString("Stop <Unknown>");
      }
    } else {
      return QString("Stop <No Soundboard>");
    }
  case HotkeyAction::Type::StopAll:
    return QString("Stop All Entries");
  case HotkeyAction::Type::None:
    return QString("<No Action>");
  default:
    return QString("<Invalid Action>");
  }
}

void HotkeyTableModel::loadFromRows(const QList<HotkeyRow>& newRows) {
  if (!rows.empty()) {
    removeRows(0, rows.size());
  }
  if (newRows.empty()) {
    return;
  }
  beginInsertRows(QModelIndex(), 0, newRows.size() - 1);
  rows = newRows;
  endInsertRows();
  checkAllConflicts();
}
CategoryHandle
HotkeyTableModel::nameToCategoryHandle(const std::string& name) const {
  for (const auto& [handle, catName] : categoryNames) {
    if (catName == name) {
      return handle;
    }
  }
  return InvalidCategoryHandle;
}
void HotkeyTableModel::loadFromModel(HotkeyTableModel* model) {
  if (model) {
    loadFromRows(model->getRows());
    setCategoryNames(model->getCategoryNames());
    cleanCategories();
  }
}
void HotkeyTableModel::setCallbackAt(
    int row, const std::function<void(void*)>& callback) {
  if (row < 0 || row >= rows.size()) {
    return;
  }
  rows[row].hotkey.callback = callback;
}
