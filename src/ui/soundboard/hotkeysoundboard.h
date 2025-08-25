#ifndef HOTKEYSOUNDBOARD_H
#define HOTKEYSOUNDBOARD_H

#include "adapters/qt/audio/basicaudioengine.h"
#include "core/soundboard/soundboard.h"
#include "core/soundboard/soundboardtypes.h"
#include "ui/flowlayout.h"
#include "ui/hotkey/hotkeytablemodel.h"
#include "ui/soundboard/renamerootbundledialog.h"
#include "ui/soundboard/rootbundlecontrolwidget.h"
#include <QMainWindow>

#ifdef Q_OS_WIN
#include "adapters/qt/hotkey/winhotkeymanager.h"
#else
// TODO: D-Bus
#endif // Q_OS_WIN

QT_BEGIN_NAMESPACE
namespace Ui {
class HotkeySoundboard;
}
QT_END_NAMESPACE

class HotkeySoundboard : public QMainWindow {
  Q_OBJECT

public:
  HotkeySoundboard(QWidget* parent = nullptr);
  ~HotkeySoundboard();

  void changeCategory(CategoryHandle category);
  bool isRootBundleNameValid(const std::string& name) const;

public slots:
  void addEntriesFromFiles(sb::EntryHandle entry, const QList<QUrl>& urls,
                           int index = -1);
  void checkNewRootBundleName(const QString& name);
  void deleteRootBundle(sb::EntryHandle entry);
  void hideRootBundle(sb::EntryHandle entry);
  void loadHotkeyModel(HotkeyTableModel* model);
  void newRootBundle();
  void onCategoriesChanged(QList<CategoryHandle> added,
                           QList<CategoryHandle> removed);
  void openHotkeyManagerDialog();
  void openRenameRootBundleDialog(sb::EntryHandle entry);
  void playEntry(sb::EntryHandle entry);
  void refreshRootBundleDisplay(sb::EntryHandle entry);
  void renameRootBundle(sb::EntryHandle entry, const std::string newName);
  void showRootBundle(sb::EntryHandle entry);

private slots:
  void on_actionCreate_New_Bundle_triggered();

  void on_actionOpen_Hotkey_Manager_triggered();

private:
  Ui::HotkeySoundboard* ui;
  std::unique_ptr<sb::adapters::qt::BasicAudioEngine> engine;
  std::unique_ptr<sb::Soundboard> soundboard;
#ifdef Q_OS_WIN
  std::unique_ptr<sb::adapters::qt::WinHotkeyManager> hotkeyManager;
#else
  // TODO: D-Bus
#endif // Q_OS_WIN
  CategoryHandle currentCategory = InvalidCategoryHandle;
  QWidget* rootBundleContainerWidget = nullptr;
  FlowLayout* rootBundleFlowLayout = nullptr;
  RenameRootBundleDialog* renameRootBundleDialog = nullptr;
  HotkeyTableModel* hotkeyModel = nullptr;
  std::unordered_map<sb::EntryHandle, RootBundleControlWidget>
      rootBundleControlWidgets;
  std::unordered_set<std::string> rootBundleNames;

  void setupHotkeyModel();

  void setupRootBundleContainerWidget();

  void setupRootBundleRenameDialog();

  bool addSoundFileFromFile(sb::EntryHandle entry,
                            const std::filesystem::path& path, int index);

  bool addDirectoryFromFile(sb::EntryHandle entry,
                            const std::filesystem::path& path, int index,
                            bool recursive = false);

  bool rootBundleNameCompare(const std::string& name1,
                             const std::string& name2) const;

  /*! \brief Returns the lower bound index of the root bundle widget
   *
   * This function performs a binary search to find the lower bound index of a
   * root bundle widget
   * \param rootBundle The root bundle to find.
   * \return The index of the root bundle widget in the flow layout.
   * If the root bundle is not found, it returns -1.
   */
  int rootBundleWidgetLowerBound(const sb::EntryHandle rootBundle) const;

  int rootBundleWidgetIndexOf(const sb::EntryHandle rootBundle,
                              int from = 0) const;

  /*! \brief Removes a root bundle widget from the flow layout
   *
   * This function removes a root bundle widget from the flow layout.
   * \param rootBundle The root bundle to remove.
   * \return true if the root bundle widget was successfully removed,
   * false otherwise.
   */
  bool removeRootBundleWidget(const sb::EntryHandle rootBundle);

  void reloadAllHotkeys();

  /*! \brief Converts a HotkeyAction to a callable function
   *
   * This function converts a HotkeyAction to a callable function that can be
   * used as a hotkey callback.
   * \param action The HotkeyAction to convert.
   * \return A std::function that takes no parameters and returns void.
   */
  std::function<void(void*)> hotkeyActionToFunction(const HotkeyAction& action);
};
#endif // HOTKEYSOUNDBOARD_H
