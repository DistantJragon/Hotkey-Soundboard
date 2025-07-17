#ifndef HOTKEYSOUNDBOARD_H
#define HOTKEYSOUNDBOARD_H

#include "core/soundboard.h"
#include "ui/flowlayout.h"
#include "ui/renamesoundgroupdialog.h"
#include "ui/soundgroupwidget.h"
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

  bool isSoundGroupNameValid(const std::string& name) const;

public slots:
  void newSoundGroup();
  void renameSoundGroup(SoundGroup* soundGroup, const std::string newName);
  void deleteSoundGroup(SoundGroup* soundGroup);
  void hideSoundGroup(SoundGroup* soundGroup);
  void showSoundGroup(SoundGroup* soundGroup);
  void openRenameSoundGroupDialog(SoundGroup* soundGroup);
  void checkNewSoundGroupName(const QString& name);

private:
  Ui::HotkeySoundboard* ui;
  Soundboard soundboard;
  QWidget* soundGroupContainerWidget = nullptr;
  FlowLayout* soundGroupFlowLayout = nullptr;
  RenameSoundGroupDialog* renameSoundGroupDialog = nullptr;
  std::unordered_map<int, std::unique_ptr<SoundGroupWidget>> soundGroupWidgets;
  std::unordered_set<std::string> soundGroupNames;

  void setupSoundGroupContainerWidget();

  void setupSoundGroupRenameDialog();

  bool soundGroupNameCompare(const std::string& name1,
                             const std::string& name2) const;

  /*! \brief Returns the lower bound index of the sound group widget
   *
   * This function performs a binary search to find the lower bound index of a
   * sound group widget
   * \param soundGroup The sound group to find the index of.
   * \return The index of the sound group widget in the flow layout.
   * If the sound group is not found, it returns -1.
   */
  int soundGroupWidgetLowerBound(const SoundGroup* soundGroup) const;

  int soundGroupWidgetIndexOf(const SoundGroup* soundGroup, int from = 0) const;

  /*! \brief Removes a sound group widget from the flow layout
   *
   * This function removes a sound group widget from the flow layout.
   * \param soundGroup The sound group to remove.
   * \return true if the sound group widget was successfully removed,
   * false otherwise.
   */
  bool removeSoundGroupWidget(const SoundGroup* soundGroup);
};
#endif // HOTKEYSOUNDBOARD_H
