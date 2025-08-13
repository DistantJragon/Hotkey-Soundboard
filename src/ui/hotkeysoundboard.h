#ifndef HOTKEYSOUNDBOARD_H
#define HOTKEYSOUNDBOARD_H

#include "adapters/qt/audio/basicaudioengine.h"
#ifdef Q_OS_WIN
#include "adapters/qt/hotkey/winhotkeymanager.h"
#else
// TODO: D-Bus
#endif // Q_OS_WIN
#include "core/soundboard/soundboard.h"
#include "core/soundboard/soundboardtypes.h"
#include "ui/flowlayout.h"
#include "ui/renamerootbundledialog.h"
#include "ui/rootbundlecontrolwidget.h"
#include <QMainWindow>

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

  bool isRootBundleNameValid(const std::string& name) const;

public slots:
  void addEntriesFromFiles(sb::EntryHandle entry, const QList<QUrl>& urls,
                           int index = -1);
  void checkNewRootBundleName(const QString& name);
  void deleteRootBundle(sb::EntryHandle entry);
  void hideRootBundle(sb::EntryHandle entry);
  void newRootBundle();
  void openRenameRootBundleDialog(sb::EntryHandle entry);
  void playEntry(sb::EntryHandle entry);
  void refreshRootBundleDisplay(sb::EntryHandle entry);
  void renameRootBundle(sb::EntryHandle entry, const std::string newName);
  void showRootBundle(sb::EntryHandle entry);

private:
  Ui::HotkeySoundboard* ui;
  std::unique_ptr<sb::adapters::qt::BasicAudioEngine> engine;
#ifdef Q_OS_WIN
  std::unique_ptr<sb::adapters::qt::WinHotkeyManager> hotkeyManager;
#else
  // TODO: D-Bus
#endif // Q_OS_WIN
  std::unique_ptr<sb::Soundboard> soundboard;
  QWidget* rootBundleContainerWidget = nullptr;
  FlowLayout* rootBundleFlowLayout = nullptr;
  RenameRootBundleDialog* renameRootBundleDialog = nullptr;
  std::unordered_map<sb::EntryHandle, RootBundleControlWidget>
      rootBundleControlWidgets;
  std::unordered_set<std::string> rootBundleNames;

  void setupRootBundleContainerWidget();

  void setupRootBundleRenameDialog();

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
};
#endif // HOTKEYSOUNDBOARD_H
