#include "ui/hotkeysoundboard.h"
#include "core/soundboard/exceptions.h"
#include "ui/soundgroupdefaults.h"
#include "ui_hotkeysoundboard.h"

HotkeySoundboard::HotkeySoundboard(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::HotkeySoundboard) {
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
    SoundGroupWidget* widget = pair.second.get();
    if (widget) {
      widget->setParent(nullptr);
    }
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
  try {
    int nextId = soundboard.getNextId();
    auto result = soundGroupWidgets.emplace(
        nextId, std::make_unique<SoundGroupWidget>(
                    soundGroupContainerWidget,
                    soundboard.newSoundGroup(SoundGroupDefaults::NAME)));
    if (result.second) {
      SoundGroupWidget* newWidget = result.first->second.get();
      connect(newWidget, &SoundGroupWidget::renameRequested, this,
              &HotkeySoundboard::openRenameSoundGroupDialog);
      soundGroupFlowLayout->insertWidget(0, newWidget);
      newWidget->show();
    } else {
      qWarning("Failed to create a new sound group. ID already exists.");
    }
  } catch (const SbExceptions::SoundGroupIdExists& e) {
    qWarning("Failed to create a new sound group: %s", e.what());
  }
}

void HotkeySoundboard::renameSoundGroup(SoundGroup* soundGroup,
                                        const std::string newName) {
  if (!soundGroup) {
    qWarning("Cannot rename a sound group that does not exist.");
    return;
  }
  bool result = removeSoundGroupWidget(soundGroup);
  if (!result) {
    qWarning("Failed to remove sound group widget before renaming.");
    return;
  }
  const std::string oldName = soundGroup->getName();
  soundGroupNames.erase(oldName);
  try {
    soundboard.renameSoundGroup(soundGroup, newName);
  } catch (const SbExceptions::SoundGroupDoesNotExist& e) {
    qWarning("Failed to rename sound group: %s", e.what());
  }
  auto it = soundGroupWidgets.find(soundGroup->getId());
  if (it != soundGroupWidgets.end()) {
    SoundGroupWidget* widget = it->second.get();
    soundGroupFlowLayout->insertWidget(soundGroupWidgetLowerBound(soundGroup),
                                       widget);
    soundGroupNames.insert(newName);
    widget->refreshSoundGroupDisplay();
    widget->show();
  } else {
    qWarning("Failed to find sound group widget after renaming.");
  }
}

void HotkeySoundboard::deleteSoundGroup(SoundGroup* soundGroup) {
  if (!soundGroup) {
    qWarning("Cannot delete a sound group that does not exist.");
    return;
  }
  try {
    int index = removeSoundGroupWidget(soundGroup);
    if (index < 0) {
      qWarning("Failed to find sound group widget to delete.");
      return;
    }
    soundGroupFlowLayout->invalidate();
    int result = soundGroupWidgets.erase(soundGroup->getId());
    if (result == 0) {
      qWarning("Failed to remove sound group widget from the map.");
      return;
    }
    soundboard.deleteSoundGroup(soundGroup);
  } catch (const SbExceptions::SoundGroupDoesNotExist& e) {
    qWarning("Failed to delete sound group: %s", e.what());
  }
}

void HotkeySoundboard::hideSoundGroup(SoundGroup* soundGroup) {
  if (!soundGroup) {
    qWarning("Cannot hide a sound group that does not exist.");
    return;
  }
  auto it = soundGroupWidgets.find(soundGroup->getId());
  if (it == soundGroupWidgets.end()) {
    qWarning("Failed to find sound group widget to hide.");
    return;
  }
  it->second->hide();
  soundGroupFlowLayout->invalidate();
}

void HotkeySoundboard::showSoundGroup(SoundGroup* soundGroup) {
  if (!soundGroup) {
    qWarning("Cannot show a sound group that does not exist.");
    return;
  }
  auto it = soundGroupWidgets.find(soundGroup->getId());
  if (it == soundGroupWidgets.end()) {
    qWarning("Failed to find sound group widget to show.");
    return;
  }
  it->second->show();
  soundGroupFlowLayout->invalidate();
}

void HotkeySoundboard::openRenameSoundGroupDialog(SoundGroup* soundGroup) {
  if (!soundGroup) {
    qWarning(
        "Cannot open rename dialog for a sound group that does not exist.");
    return;
  }
  if (!renameSoundGroupDialog) {
    qWarning("Rename dialog is not initialized.");
    return;
  }
  renameSoundGroupDialog->setCurrentName(soundGroup->getName());
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
    const SoundGroup* soundGroup) const {
  if (!soundGroup) {
    return -1; // Invalid sound group
  }
  std::string name = soundGroup->getName();
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
    if (soundGroupNameCompare(widget->getSoundGroup()->getName(), name)) {
      left = mid + 1;
    } else {
      right = mid;
    }
  }
  return left;
}

int HotkeySoundboard::soundGroupWidgetIndexOf(const SoundGroup* soundGroup,
                                              int from) const {
  if (!soundGroup || from < 0 || from >= soundGroupFlowLayout->count()) {
    return -1; // Invalid sound group or out of bounds
  }
  int id = soundGroup->getId();
  for (int i = from; i < soundGroupFlowLayout->count(); ++i) {
    QLayoutItem* item = soundGroupFlowLayout->itemAt(i);
    SoundGroupWidget* widget = static_cast<SoundGroupWidget*>(item->widget());
    if (widget->getSoundGroup()->getId() == id) {
      return i;
    }
  }
  return -1; // Not found
}

bool HotkeySoundboard::removeSoundGroupWidget(const SoundGroup* soundGroup) {
  if (!soundGroup) {
    qWarning("Cannot remove a sound group widget that does not exist.");
    return false;
  }
  auto it = soundGroupWidgets.find(soundGroup->getId());
  if (it == soundGroupWidgets.end()) {
    qWarning("Sound group widget not found in the map.");
    return false;
  }
  SoundGroupWidget* widget = it->second.get();
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
  widget->hide();
#ifndef HKSBNDEBUG
  // Check if the widget is still in the QList of soundGroupFlowLayout
  if (soundGroupWidgetIndexOf(soundGroup) != -1) {
    qWarning("Widget was not removed from the FlowLayout.");
  }
#endif // HKSBNDEBUG
  return true;
}
