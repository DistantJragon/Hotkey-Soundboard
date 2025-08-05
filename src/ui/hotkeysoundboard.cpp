#include "ui/hotkeysoundboard.h"
#include "core/soundboard/exceptions.h"
#include "core/soundboard/soundboardtypes.h"
#include "ui/soundgroupdefaults.h"
#include "ui_hotkeysoundboard.h"

HotkeySoundboard::HotkeySoundboard(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::HotkeySoundboard) {
  engine = std::make_unique<sb::adapters::qt::BasicAudioEngine>();
  soundboard = std::make_unique<sb::Soundboard>(engine.get());
  ui->setupUi(this);
  setupSoundGroupContainerWidget();
  setupSoundGroupRenameDialog();
#ifndef HKSBNDEBUG
  for (int i = 0; i < 10; ++i)
    newSoundGroup();
#endif // HKSBNDEBUG
}

HotkeySoundboard::~HotkeySoundboard() {
  // TODO: Investigate possible double freeing of SoundGroupWidgets due to
  // soundGroupWidgets and soundGroupContainerWidget
  for (auto& pair : soundGroupWidgets) {
    pair.second.setParent(nullptr);
  }
  delete ui;
  delete soundGroupContainerWidget;
  delete renameSoundGroupDialog;
}

void HotkeySoundboard::setupSoundGroupContainerWidget() {
  soundGroupContainerWidget = new QWidget(this->ui->scrollArea);
  soundGroupFlowLayout = new FlowLayout(soundGroupContainerWidget);
  soundGroupContainerWidget->setLayout(soundGroupFlowLayout);

  ui->scrollArea->setWidget(soundGroupContainerWidget);
  ui->scrollArea->setWidgetResizable(true);
}

void HotkeySoundboard::setupSoundGroupRenameDialog() {
  renameSoundGroupDialog = new RenameSoundGroupDialog(this);
  connect(renameSoundGroupDialog,
          &RenameSoundGroupDialog::renameDialogTextEdited, this,
          &HotkeySoundboard::checkNewSoundGroupName);
}

bool HotkeySoundboard::isSoundGroupNameValid(const std::string& name) const {
  return name == SoundGroupDefaults::NAME ||
         soundGroupNames.find(name) == soundGroupNames.end();
}

void HotkeySoundboard::newSoundGroup() {
  sb::GroupHandle nextHandle = soundboard->getNextHandle();
  auto result = soundGroupWidgets.try_emplace(
      nextHandle, soundGroupContainerWidget,
      soundboard->newSoundGroup(SoundGroupDefaults::NAME));
  if (result.second) {
    try {
      const sb::SoundGroup& group =
          soundboard->getSoundGroup(result.first->first);
      SoundGroupWidget& newWidget = result.first->second;
      connect(&newWidget, &SoundGroupWidget::renameRequested, this,
              &HotkeySoundboard::openRenameSoundGroupDialog);
      connect(&newWidget, &SoundGroupWidget::deleteRequested, this,
              &HotkeySoundboard::deleteSoundGroup);
      connect(&newWidget, &SoundGroupWidget::hideRequested, this,
              &HotkeySoundboard::hideSoundGroup);
      connect(&newWidget, &SoundGroupWidget::refreshRequested, this,
              &HotkeySoundboard::refreshSoundGroupDisplay);
      soundGroupFlowLayout->insertWidget(0, &newWidget);
      newWidget.refreshSoundGroupDisplay(group);
      newWidget.show();
    } catch (const sb::exceptions::SoundGroupIdExists& e) {
      qWarning("Failed to create a new sound group: %s", e.what());
      soundGroupWidgets.erase(result.first->first);
      return;
    }
  } else {
    qWarning("Failed to create a new sound group. ID already exists.");
  }
}

void HotkeySoundboard::refreshSoundGroupDisplay(sb::GroupHandle soundGroup) {
  if (!soundboard->isValidGroup(soundGroup)) {
    qWarning("Cannot refresh an invalid sound group.");
    return;
  }
  auto it = soundGroupWidgets.find(soundGroup);
  if (it == soundGroupWidgets.end()) {
    qWarning("Failed to find sound group widget to refresh.");
    return;
  }
  try {
    it->second.refreshSoundGroupDisplay(soundboard->getSoundGroup(soundGroup));
  } catch (const sb::exceptions::MissingSoundGroup& e) {
    qWarning("Sound group with handle '%d' is missing: %s", e.getHandle(),
             e.what());
    return;
  }
}

void HotkeySoundboard::renameSoundGroup(sb::GroupHandle soundGroup,
                                        const std::string newName) {
  if (!soundboard->isValidGroup(soundGroup)) {
    qWarning("Cannot rename an invalid sound group.");
    return;
  }
  bool result = removeSoundGroupWidget(soundGroup);
  if (!result) {
    qWarning("Failed to remove sound group widget before renaming.");
    return;
  }
  try {
    const sb::SoundGroup& group = soundboard->getSoundGroup(soundGroup);
    const std::string oldName = group.getName();
    soundGroupNames.erase(oldName);
    soundboard->renameSoundGroup(soundGroup, newName);
    auto it = soundGroupWidgets.find(soundGroup);
    if (it == soundGroupWidgets.end()) {
      qWarning("Failed to find sound group widget after renaming.");
      return;
    }
    SoundGroupWidget& widget = it->second;
    soundGroupFlowLayout->insertWidget(soundGroupWidgetLowerBound(soundGroup),
                                       &widget);
    soundGroupNames.insert(newName);
    widget.refreshSoundGroupDisplay(group);
    widget.show();
  } catch (const sb::exceptions::MissingSoundGroup& e) {
    qWarning("Sound group with handle '%d' is missing: %s", e.getHandle(),
             e.what());
    return;
  }
}

void HotkeySoundboard::deleteSoundGroup(sb::GroupHandle soundGroup) {
  if (!soundboard->isValidGroup(soundGroup)) {
    qWarning("Cannot delete an invalid sound group.");
    return;
  }
  bool removeResult = removeSoundGroupWidget(soundGroup);
  if (!removeResult) {
    qWarning("Failed to find sound group widget to delete.");
    return;
  }
  soundGroupFlowLayout->invalidate();
  int eraseResult = soundGroupWidgets.erase(soundGroup);
  if (eraseResult == 0) {
    qWarning("Failed to remove sound group widget from the map.");
    return;
  }
  soundboard->deleteSoundGroup(soundGroup);
}

void HotkeySoundboard::hideSoundGroup(sb::GroupHandle soundGroup) {
  if (!soundboard->isValidGroup(soundGroup)) {
    qWarning("Cannot hide an invalid sound group.");
    return;
  }
  auto it = soundGroupWidgets.find(soundGroup);
  if (it == soundGroupWidgets.end()) {
    qWarning("Failed to find sound group widget to hide.");
    return;
  }
  it->second.hide();
  soundGroupFlowLayout->invalidate();
}

void HotkeySoundboard::showSoundGroup(sb::GroupHandle soundGroup) {
  if (!soundboard->isValidGroup(soundGroup)) {
    qWarning("Cannot show an invalid sound group.");
    return;
  }
  auto it = soundGroupWidgets.find(soundGroup);
  if (it == soundGroupWidgets.end()) {
    qWarning("Failed to find sound group widget to show.");
    return;
  }
  it->second.show();
  soundGroupFlowLayout->invalidate();
}

void HotkeySoundboard::openRenameSoundGroupDialog(sb::GroupHandle soundGroup) {
  if (!soundboard->isValidGroup(soundGroup)) {
    qWarning("Cannot open rename dialog for an invalid sound group.");
    return;
  }
  if (!renameSoundGroupDialog) {
    qWarning("Rename dialog is not initialized.");
    return;
  }
  try {
    const sb::SoundGroup& group = soundboard->getSoundGroup(soundGroup);
    renameSoundGroupDialog->setCurrentName(group.getName());
    renameSoundGroupDialog->setValidName(true);
    if (renameSoundGroupDialog->exec() == QDialog::Accepted) {
      std::string newName =
          renameSoundGroupDialog->getNameLineEditText().toStdString();
      if (!isSoundGroupNameValid(newName)) {
        qWarning("Invalid sound group name: '%s'.", newName.c_str());
        return;
      }
      renameSoundGroup(soundGroup, newName);
    } else {
      qDebug("Rename dialog was canceled.");
    }
  } catch (const sb::exceptions::MissingSoundGroup& e) {
    qWarning("Sound group with handle '%d' is missing: %s", e.getHandle(),
             e.what());
    return;
  }
}

void HotkeySoundboard::checkNewSoundGroupName(const QString& name) {
  renameSoundGroupDialog->setValidName(
      isSoundGroupNameValid(name.toStdString()));
}

bool HotkeySoundboard::soundGroupNameCompare(const std::string& name1,
                                             const std::string& name2) const {
  if (name1 == SoundGroupDefaults::NAME) {
    return true;
  } else if (name2 == SoundGroupDefaults::NAME) {
    return false;
  } else {
    return name1 < name2;
  }
}

int HotkeySoundboard::soundGroupWidgetLowerBound(
    const sb::GroupHandle handle) const {
  if (!soundboard->isValidGroup(handle)) {
    return -1; // Invalid sound group
  }
  try {
    const sb::SoundGroup& group = soundboard->getSoundGroup(handle);
    std::string name = group.getName();
    if (name == SoundGroupDefaults::NAME) {
      return 0; // Sound group with default name is always at the start
    }
    if (soundGroupFlowLayout->count() == 0) {
      return 0; // No items in the layout, insert at the start
    }
    int left = 0;
    int right = soundGroupFlowLayout->count();
    while (left < right) {
      int mid = left + (right - left) / 2;
      QLayoutItem* item = soundGroupFlowLayout->itemAt(mid);
      SoundGroupWidget* widget = static_cast<SoundGroupWidget*>(item->widget());
      const std::string& otherName =
          soundboard->getSoundGroup(widget->getSoundGroupHandle()).getName();
      if (soundGroupNameCompare(otherName, name)) {
        left = mid + 1;
      } else {
        right = mid;
      }
    }
    return left;
  } catch (const sb::exceptions::MissingSoundGroup& e) {
    qWarning("Sound group with handle '%d' is missing: %s", e.getHandle(),
             e.what());
    return -1; // Handle missing sound group
  }
}

int HotkeySoundboard::soundGroupWidgetIndexOf(const sb::GroupHandle soundGroup,
                                              int from) const {
  if (!soundGroup || from < 0 || from >= soundGroupFlowLayout->count()) {
    return -1; // Invalid sound group or out of bounds
  }
  for (int i = from; i < soundGroupFlowLayout->count(); ++i) {
    QLayoutItem* item = soundGroupFlowLayout->itemAt(i);
    // Decision between comparing pointers or comparing handles
    SoundGroupWidget* widget = static_cast<SoundGroupWidget*>(item->widget());
    if (widget->getSoundGroupHandle() == soundGroup) {
      return i;
    }
  }
  return -1; // Not found
}

bool HotkeySoundboard::removeSoundGroupWidget(
    const sb::GroupHandle soundGroup) {
  if (!soundboard->isValidGroup(soundGroup)) {
    qWarning("Cannot remove an invalid sound group widget.");
    return false;
  }
  auto it = soundGroupWidgets.find(soundGroup);
  if (it == soundGroupWidgets.end()) {
    qWarning("Sound group widget not found in the map.");
    return false;
  }
  SoundGroupWidget& widget = it->second;
  int index = soundGroupWidgetIndexOf(soundGroup,
                                      soundGroupWidgetLowerBound(soundGroup));
  if (index == -1) {
    qWarning("Sound group widget not found in the flow layout.");
    return false;
  }
  QLayoutItem* item = soundGroupFlowLayout->takeAt(index);
  if (!item) {
    qWarning("Failed to take item from the flow layout.");
    return false;
  }
  delete item;
  widget.hide();
#ifndef HKSBNDEBUG
  // Check if the widget is still in the QList of soundGroupFlowLayout
  if (soundGroupWidgetIndexOf(soundGroup) != -1) {
    qWarning("Widget was not removed from the FlowLayout.");
  }
#endif // HKSBNDEBUG
  return true;
}
