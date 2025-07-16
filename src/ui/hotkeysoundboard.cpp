#include "ui/hotkeysoundboard.h"
#include "exceptions/exceptions.h"
#include "ui/soundgroupdefaults.h"
#include "ui_hotkeysoundboard.h"

HotkeySoundboard::HotkeySoundboard(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::HotkeySoundboard) {
  ui->setupUi(this);
  setupSoundGroupContainerWidget();
#ifndef HKSBNDEBUG
  newSoundGroup();
#endif // NDEBUG
}

HotkeySoundboard::~HotkeySoundboard() {
  delete ui;
  delete soundGroupContainerWidget;
}

void HotkeySoundboard::setupSoundGroupContainerWidget() {
  soundGroupContainerWidget = new QWidget(this->ui->scrollArea);
  soundGroupFlowLayout = new FlowLayout(soundGroupContainerWidget);
  soundGroupContainerWidget->setLayout(soundGroupFlowLayout);

  ui->scrollArea->setWidget(soundGroupContainerWidget);
  ui->scrollArea->setWidgetResizable(true);
}

void HotkeySoundboard::newSoundGroup() {
  try {
    auto result = soundGroupWidgets.emplace(
        soundboard.getNextId(),
        std::make_unique<SoundGroupWidget>(
            soundGroupContainerWidget,
            soundboard.newSoundGroup(SoundGroupDefaults::NAME)));
    if (result.second) {
      SoundGroupWidget* newWidget = result.first->second.get();
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
                                        const QString& newName) {
  if (!soundGroup) {
    qWarning("Cannot rename a sound group that does not exist.");
    return;
  }
  try {
    bool result = removeSoundGroupWidget(soundGroup);
    if (!result) {
      qWarning("Failed to find sound group widget for renaming.");
      return;
    }
    soundboard.renameSoundGroup(soundGroup, newName.toStdString());
    auto it = soundGroupWidgets.find(soundGroup->getId());
    if (it != soundGroupWidgets.end()) {
      SoundGroupWidget* widget = it->second.get();
      soundGroupFlowLayout->insertWidget(soundGroupWidgetLowerBound(soundGroup),
                                         widget);
      widget->show();
      widget->updateGeometry();
    } else {
      qWarning("Failed to find sound group widget after renaming.");
    }

  } catch (const SbExceptions::SoundGroupDoesNotExist& e) {
    qWarning("Failed to rename sound group: %s", e.what());
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
  int left = 0;
  int right = soundGroupFlowLayout->count() - 1;
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
    return false;
  }
  auto it = soundGroupWidgets.find(soundGroup->getId());
  if (it == soundGroupWidgets.end()) {
    return false;
  }
  SoundGroupWidget* widget = it->second.get();
  soundGroupFlowLayout->removeWidget(widget);
  widget->hide();
#ifndef HKSBNDEBUG
  // Check if the widget is still in the QList of soundGroupFlowLayout
  if (soundGroupWidgetIndexOf(soundGroup) != -1) {
    qWarning("Widget was not removed from the FlowLayout.");
  }
#endif // NDEBUG
  return true;
}
