#ifndef HOTKEYTABLEMODEL_H
#define HOTKEYTABLEMODEL_H

#include "core/soundboard/soundboard.h"
#include "ui/hotkey/hotkeyguitypes.h"
#include <QAbstractTableModel>

class HotkeyTableModel : public QAbstractTableModel {
  Q_OBJECT
public:
  explicit HotkeyTableModel(QObject* parent = nullptr,
                            sb::Soundboard* soundboard = nullptr);
  int rowCount(const QModelIndex&) const override { return rows.size(); }
  int columnCount(const QModelIndex&) const override { return 4; }

  QVariant data(const QModelIndex& index, int role) const override;

  bool setData(const QModelIndex& idx, const QVariant& value,
               int role) override;

  QVariant headerData(int section, Qt::Orientation orientation,
                      int role) const override;

  bool insertRows(int row, int count,
                  const QModelIndex& parent = QModelIndex()) override;

  bool removeRows(int row, int count,
                  const QModelIndex& parent = QModelIndex()) override;

  Qt::ItemFlags flags(const QModelIndex& index) const override;
  const QList<HotkeyRow>& getRows() const { return rows; }

  const std::unordered_map<CategoryHandle, std::string>&
  getCategoryNames() const {
    return categoryNames;
  }
  void setCategoryNames(
      const std::unordered_map<CategoryHandle, std::string>& categories) {
    categoryNames = categories;
  }
  CategoryHandle addCategoryName(const std::string& name);

  void cleanCategories();

  CategoryHandle nameToCategoryHandle(const std::string& name) const;

  QString actionToString(const HotkeyAction& action) const;

  void setCategoryAt(int row, CategoryHandle category);
  void setCategoryAt(int row, const std::string& categoryName);
  void setKeyDataAt(int row, const sb::hotkey::Hotkey& hotkey);
  void setActionAt(int row, const HotkeyAction& action);
  void setCallbackAt(int row, const std::function<void(void*)>& callback);

  void loadFromRows(const QList<HotkeyRow>& newRows);
  void loadFromModel(HotkeyTableModel* model);

signals:
  void categoryNamesChanged(QList<CategoryHandle> added,
                            QList<CategoryHandle> removed);

private:
  QList<HotkeyRow> rows;
  sb::Soundboard* soundboard = nullptr;
  std::unordered_map<CategoryHandle, std::string> categoryNames;

  CategoryHandle addCategoryName(const std::string& name, CategoryHandle hint);
  void checkAllConflicts();
  bool checkConflictAt(int a, int b) const;
};
#endif // HOTKEYTABLEMODEL_H
