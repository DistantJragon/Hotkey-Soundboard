#include "ui/soundboard/hotkeysoundboard.h"
#include "core/soundboard/bundleentry.h"
#include "core/soundboard/playableentry.h"
#include "core/soundboard/soundboardtypes.h"
#include "ui/soundboard/bundledefaults.h"
#include "ui_hotkeysoundboard.h"

HotkeySoundboard::HotkeySoundboard(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::HotkeySoundboard) {
  ui->setupUi(this);
  engine = std::make_unique<sb::adapters::qt::BasicAudioEngine>();
  hotkeyManager = std::make_unique<sb::adapters::qt::WinHotkeyManager>(this);
  soundboard = std::make_unique<sb::Soundboard>(engine.get());
  setupRootBundleContainerWidget();
  setupRootBundleRenameDialog();
#ifndef HKSBNDEBUG

#endif // HKSBNDEBUG
}

HotkeySoundboard::~HotkeySoundboard() {
  // TODO: Investigate possible double freeing of RootBundleControlWidgets due
  // to RootBundleControlWidget map and child of rootBundleContainerWidget
  for (auto& pair : rootBundleControlWidgets) {
    pair.second.setParent(nullptr);
  }
  delete rootBundleContainerWidget;
  delete renameRootBundleDialog;
  delete ui;
}

void HotkeySoundboard::setupRootBundleContainerWidget() {
  rootBundleContainerWidget = new QWidget(this->ui->scrollArea);
  rootBundleFlowLayout = new FlowLayout(rootBundleContainerWidget);

  ui->scrollArea->setWidget(rootBundleContainerWidget);
  ui->scrollArea->setWidgetResizable(true);
}

void HotkeySoundboard::setupRootBundleRenameDialog() {
  renameRootBundleDialog = new RenameRootBundleDialog(this);
  connect(renameRootBundleDialog,
          &RenameRootBundleDialog::renameDialogTextEdited, this,
          &HotkeySoundboard::checkNewRootBundleName);
}

bool HotkeySoundboard::isRootBundleNameValid(const std::string& name) const {
  return name == BundleDefaults::NAME ||
         rootBundleNames.find(name) == rootBundleNames.end();
}

void HotkeySoundboard::newRootBundle() {
  sb::EntryHandle nextHandle = soundboard->getNextHandle();
  auto result = rootBundleControlWidgets.try_emplace(
      nextHandle, rootBundleContainerWidget,
      soundboard->newBundle(BundleDefaults::NAME));
  if (result.second) {
    RootBundleControlWidget& newWidget = result.first->second;
    sb::BundleEntry* bundleEntry =
        static_cast<sb::BundleEntry*>(soundboard->getEntry(nextHandle));
    if (!bundleEntry) {
      qWarning("Failed to retrieve bundle entry for handle '%d'.", nextHandle);
      rootBundleControlWidgets.erase(nextHandle);
      return;
    }
    connect(&newWidget, &RootBundleControlWidget::renameRequested, this,
            &HotkeySoundboard::openRenameRootBundleDialog);
    connect(&newWidget, &RootBundleControlWidget::deleteRequested, this,
            &HotkeySoundboard::deleteRootBundle);
    connect(&newWidget, &RootBundleControlWidget::hideRequested, this,
            &HotkeySoundboard::hideRootBundle);
    connect(&newWidget, &RootBundleControlWidget::refreshRequested, this,
            &HotkeySoundboard::refreshRootBundleDisplay);
    connect(&newWidget, &RootBundleControlWidget::filesDropped, this,
            &HotkeySoundboard::addEntriesFromFiles);
    connect(&newWidget, &RootBundleControlWidget::playRequested, this,
            &HotkeySoundboard::playEntry);
    rootBundleFlowLayout->insertWidget(0, &newWidget);
    newWidget.refreshRootBundleDisplay(*bundleEntry);
    newWidget.show();
  } else {
    qWarning("Failed to create a new sound group. ID already exists.");
  }
}

void HotkeySoundboard::refreshRootBundleDisplay(sb::EntryHandle entry) {
  if (!soundboard->isValidEntry(entry)) {
    qWarning("Cannot refresh an invalid sound group.");
    return;
  }
  auto it = rootBundleControlWidgets.find(entry);
  if (it == rootBundleControlWidgets.end()) {
    qWarning("Failed to find root bundle control widget to refresh.");
    return;
  }
  sb::BundleEntry* bundleEntry =
      static_cast<sb::BundleEntry*>(soundboard->getEntry(entry));
  if (!bundleEntry) {
    qWarning("Failed to retrieve bundle entry for handle '%d'.", entry);
    return;
  }
  it->second.refreshRootBundleDisplay(*bundleEntry);
  return;
}

void HotkeySoundboard::renameRootBundle(sb::EntryHandle entry,
                                        const std::string newName) {
  if (!soundboard->isValidEntry(entry)) {
    qWarning("Cannot rename an invalid entry.");
    return;
  }
  bool result = removeRootBundleWidget(entry);
  if (!result) {
    qWarning("Failed to remove root bundle widget before renaming.");
    return;
  }
  const sb::BundleEntry* bundlePtr =
      static_cast<sb::BundleEntry*>(soundboard->getEntry(entry));
  if (!bundlePtr) {
    qWarning("Failed to retrieve bundle entry for handle '%d'.", entry);
    return;
  }
  const sb::BundleEntry& bundle = *bundlePtr;
  const std::string oldName = bundle.getName();
  rootBundleNames.erase(oldName);
  soundboard->renameEntry(entry, newName);
  auto it = rootBundleControlWidgets.find(entry);
  if (it == rootBundleControlWidgets.end()) {
    qWarning("Failed to find root bundle widget after renaming.");
    return;
  }
  RootBundleControlWidget& widget = it->second;
  rootBundleFlowLayout->insertWidget(rootBundleWidgetLowerBound(entry),
                                     &widget);
  rootBundleNames.insert(newName);
  widget.refreshRootBundleDisplay(bundle);
  widget.show();
}

void HotkeySoundboard::deleteRootBundle(sb::EntryHandle rootBundle) {
  if (!soundboard->isValidEntry(rootBundle)) {
    qWarning("Cannot delete an invalid sound group.");
    return;
  }
  bool removeResult = removeRootBundleWidget(rootBundle);
  if (!removeResult) {
    qWarning("Failed to find sound group widget to delete.");
    return;
  }
  rootBundleFlowLayout->invalidate();
  int eraseResult = rootBundleControlWidgets.erase(rootBundle);
  if (eraseResult == 0) {
    qWarning("Failed to remove sound group widget from the map.");
    return;
  }
  soundboard->deleteEntry(rootBundle);
}

void HotkeySoundboard::hideRootBundle(sb::EntryHandle rootBundle) {
  if (!soundboard->isValidEntry(rootBundle)) {
    qWarning("Cannot hide an invalid sound group.");
    return;
  }
  auto it = rootBundleControlWidgets.find(rootBundle);
  if (it == rootBundleControlWidgets.end()) {
    qWarning("Failed to find sound group widget to hide.");
    return;
  }
  it->second.hide();
  rootBundleFlowLayout->invalidate();
}

void HotkeySoundboard::showRootBundle(sb::EntryHandle rootBundle) {
  if (!soundboard->isValidEntry(rootBundle)) {
    qWarning("Cannot show an invalid sound group.");
    return;
  }
  auto it = rootBundleControlWidgets.find(rootBundle);
  if (it == rootBundleControlWidgets.end()) {
    qWarning("Failed to find sound group widget to show.");
    return;
  }
  it->second.show();
  rootBundleFlowLayout->invalidate();
}

void HotkeySoundboard::openRenameRootBundleDialog(sb::EntryHandle rootBundle) {
  if (!soundboard->isValidEntry(rootBundle)) {
    qWarning("Cannot open rename dialog for an invalid sound group.");
    return;
  }
  if (!renameRootBundleDialog) {
    qWarning("Rename dialog is not initialized.");
    return;
  }
  const sb::BundleEntry* bundlePtr =
      static_cast<const sb::BundleEntry*>(soundboard->getEntry(rootBundle));
  if (!bundlePtr) {
    qWarning("Failed to retrieve bundle entry for handle '%d'.", rootBundle);
    return;
  }
  const sb::BundleEntry& group = *bundlePtr;
  renameRootBundleDialog->setCurrentName(group.getName());
  renameRootBundleDialog->setValidName(true);
  if (renameRootBundleDialog->exec() == QDialog::Accepted) {
    std::string newName =
        renameRootBundleDialog->getNameLineEditText().toStdString();
    if (!isRootBundleNameValid(newName)) {
      qWarning("Invalid sound group name: '%s'.", newName.c_str());
      return;
    }
    renameRootBundle(rootBundle, newName);
  } else {
    qDebug("Rename dialog was canceled.");
  }
}

void HotkeySoundboard::addEntriesFromFiles(sb::EntryHandle entry,
                                           const QList<QUrl>& urls, int index) {
  qDebug("Adding files to sound group with handle: %d at index: %d", entry,
         index);
  if (!soundboard->isValidEntry(entry)) {
    qWarning("Cannot add entries to an invalid sound group.");
    return;
  }
  if (urls.isEmpty()) {
    qWarning("No files to add to the sound group.");
    return;
  }
  if (index < 0) {
    qWarning("Invalid index for adding files to the sound group.");
    return;
  }
  for (const QUrl& url : urls) {
    if (url.isLocalFile()) {
      std::filesystem::path path = url.toLocalFile().toStdString();
      if (std::filesystem::is_directory(path)) {
        // TODO: Handle directories
      } else {
        sb::EntryHandle newHandle = soundboard->newSoundFile(
            path, path.filename().string(), entry, index);
        if (newHandle == sb::InvalidEntryHandle) {
          qWarning("Failed to add sound file '%s' to the sound group.",
                   path.string().c_str());
        } else {
          index++;
          auto it = rootBundleControlWidgets.find(entry);
          if (it != rootBundleControlWidgets.end()) {
            sb::BundleEntry* bundlePtr =
                static_cast<sb::BundleEntry*>(soundboard->getEntry(entry));
            it->second.refreshRootBundleDisplay(*bundlePtr);
            qDebug("Added sound file '%s' to the sound group.",
                   path.string().c_str());
          }
        }
      }
    } else {
      qWarning("Only local files are supported for adding to the sound group.");
    }
  }
}

void HotkeySoundboard::checkNewRootBundleName(const QString& name) {
  renameRootBundleDialog->setValidName(
      isRootBundleNameValid(name.toStdString()));
}

bool HotkeySoundboard::rootBundleNameCompare(const std::string& name1,
                                             const std::string& name2) const {
  if (name1 == BundleDefaults::NAME) {
    return true;
  } else if (name2 == BundleDefaults::NAME) {
    return false;
  } else {
    return name1 < name2;
  }
}

int HotkeySoundboard::rootBundleWidgetLowerBound(
    const sb::EntryHandle handle) const {
  if (!soundboard->isValidEntry(handle)) {
    return -1; // Invalid sound group
  }
  const sb::PlayableEntry* entryPtr = soundboard->getEntry(handle);
  if (!entryPtr) {
    qWarning("Failed to retrieve bundle entry for handle '%d'.", handle);
    return -1; // Handle not found
  }
  if (rootBundleFlowLayout->count() == 0) {
    return 0; // No items in the layout, insert at the start
  }
  std::string name = entryPtr->getName();
  if (name == BundleDefaults::NAME) {
    return 0; // Sound group with default name is always at the start
  }
  int left = 0;
  int right = rootBundleFlowLayout->count();
  while (left < right) {
    int mid = left + (right - left) / 2;
    QLayoutItem* item = rootBundleFlowLayout->itemAt(mid);
    RootBundleControlWidget* widget =
        static_cast<RootBundleControlWidget*>(item->widget());
    sb::PlayableEntry* entry =
        soundboard->getEntry(widget->getRootBundleHandle());
    const std::string& otherName = entry->getName();
    if (rootBundleNameCompare(otherName, name)) {
      left = mid + 1;
    } else {
      right = mid;
    }
  }
  return left;
}

int HotkeySoundboard::rootBundleWidgetIndexOf(const sb::EntryHandle rootBundle,
                                              int from) const {
  if (!soundboard->isValidEntry(rootBundle) || from < 0 ||
      from >= rootBundleFlowLayout->count()) {
    return -1; // Invalid sound group or out of bounds
  }
  for (int i = from; i < rootBundleFlowLayout->count(); ++i) {
    QLayoutItem* item = rootBundleFlowLayout->itemAt(i);
    // Decision between comparing pointers or comparing handles
    RootBundleControlWidget* widget =
        static_cast<RootBundleControlWidget*>(item->widget());
    if (widget->getRootBundleHandle() == rootBundle) {
      return i;
    }
  }
  return -1; // Not found
}

bool HotkeySoundboard::removeRootBundleWidget(
    const sb::EntryHandle rootBundle) {
  if (!soundboard->isValidEntry(rootBundle)) {
    qWarning("Cannot remove an invalid sound group widget.");
    return false;
  }
  auto it = rootBundleControlWidgets.find(rootBundle);
  if (it == rootBundleControlWidgets.end()) {
    qWarning("Sound group widget not found in the map.");
    return false;
  }
  RootBundleControlWidget& widget = it->second;
  int index = rootBundleWidgetIndexOf(rootBundle,
                                      rootBundleWidgetLowerBound(rootBundle));
  if (index == -1) {
    qWarning("Sound group widget not found in the flow layout.");
    return false;
  }
  QLayoutItem* item = rootBundleFlowLayout->takeAt(index);
  if (!item) {
    qWarning("Failed to take item from the flow layout.");
    return false;
  }
  delete item;
  widget.hide();
#ifndef HKSBNDEBUG
  // Check if the widget is still in the QList of soundGroupFlowLayout
  if (rootBundleWidgetIndexOf(rootBundle) != -1) {
    qWarning("Widget was not removed from the FlowLayout.");
  }
#endif // HKSBNDEBUG
  return true;
}

void HotkeySoundboard::playEntry(sb::EntryHandle entry) {
  if (soundboard->isValidEntry(entry)) {
    soundboard->playEntry(entry);
  } else {
    qWarning("Cannot play an invalid entry.");
  }
}

void HotkeySoundboard::on_actionCreate_New_Bundle_triggered() {
  newRootBundle();
}
