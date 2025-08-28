#include "ui/soundboard/hotkeysoundboard.h"
#include "core/soundboard/bundleentry.h"
#include "core/soundboard/playableentry.h"
#include "core/soundboard/soundboardtypes.h"
#include "ui/hotkey/hotkeymanagerdialog.h"
#include "ui/soundboard/bundledefaults.h"
#include "ui_hotkeysoundboard.h"
#include <QMessageBox>

HotkeySoundboard::HotkeySoundboard(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::HotkeySoundboard) {
  ui->setupUi(this);
  engine = std::make_unique<sb::adapters::qt::BasicAudioEngine>();
  // TODO: load soundboard state from file
  soundboard = std::make_unique<sb::Soundboard>(engine.get());
#ifdef Q_OS_WIN
  hotkeyManager = std::make_unique<sb::adapters::qt::WinHotkeyManager>();
#else
  // TODO: D-Bus
#endif // Q_OS_WIN
  setupHotkeyModel();
  setupRootBundleContainerWidget();
  setupRootBundleRenameDialog();
#ifndef HKSBNDEBUG
  // sb::hotkey::Hotkey hotkey;
  // hotkey.humanReadable = "Shift+B";
  // hotkey.nativeModifiers = 0x4;
  // hotkey.nativeVirtualKey = 0x42;
  // hotkey.callback = [](void*) { qDebug("debug hotkey triggered!"); };
  // sb::hotkey::HotkeyHandle handle = hotkeyManager->registerHotkey(hotkey);
  // qDebug("Registered debug hotkey with handle: %d", handle);
#endif // HKSBNDEBUG
}

HotkeySoundboard::~HotkeySoundboard() {
  // TODO: Investigate possible double freeing of RootBundleControlWidgets due
  // to RootBundleControlWidget map and child of rootBundleContainerWidget
  for (auto& pair : rootBundleControlWidgets) {
    pair.second.setParent(nullptr);
  }
  delete hotkeyModel;
  delete rootBundleContainerWidget;
  delete renameRootBundleDialog;
  delete ui;
}

void HotkeySoundboard::setupHotkeyModel() {
  if (!hotkeyModel) {
    hotkeyModel = new HotkeyTableModel(this, soundboard.get());
    connect(hotkeyModel, &HotkeyTableModel::categoryNamesChanged, this,
            &HotkeySoundboard::onCategoriesChanged);
    changeCategory(GlobalCategoryHandle);
  }
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

void HotkeySoundboard::changeCategory(CategoryHandle category) {
  if (currentCategory == category) {
    return;
  }
  if (!hotkeyModel->getCategoryNames().count(category)) {
    qWarning("Cannot change to an unknown category.");
    return;
  }
  currentCategory = category;
  hotkeyManager->unregisterAllHotkeys();
  const auto& rows = hotkeyModel->getRows();
  for (int i = 0; i < hotkeyModel->rowCount({}); ++i) {
    const HotkeyRow& row = rows.at(i);
    if (row.enabled && !row.conflict &&
        (row.category == currentCategory ||
         row.category == GlobalCategoryHandle)) {
      hotkeyModel->setCallbackAt(i, hotkeyActionToFunction(row.action));
      sb::hotkey::HotkeyHandle handle =
          hotkeyManager->registerHotkey(hotkeyModel->getRows().at(i).hotkey);
      qDebug("%d", handle);
    } else {
      hotkeyModel->setCallbackAt(i, nullptr);
    }
  }
}

void HotkeySoundboard::loadRootBundleControlWidgetFromEntry(
    sb::EntryHandle entry) {
  if (!soundboard->isValidEntry(entry)) {
    qWarning("Cannot load a root bundle control widget from an invalid entry.");
    return;
  }
  auto it = rootBundleControlWidgets.find(entry);
  if (it != rootBundleControlWidgets.end()) {
    qWarning("Root bundle control widget for entry already exists.");
    return;
  }
  sb::PlayableEntry* entryPtr = soundboard->getEntry(entry);
  if (!entryPtr) {
    qWarning("Failed to retrieve entry for handle '%d'.", entry);
    return;
  }
  if (entryPtr->type != sb::PlayableEntry::Type::Bundle ||
      entryPtr->getParentHandle() != sb::InvalidEntryHandle) {
    qWarning("Entry is not a root bundle.");
    return;
  }
  sb::BundleEntry* bundleEntry = static_cast<sb::BundleEntry*>(entryPtr);
  auto result = rootBundleControlWidgets.try_emplace(
      entry, rootBundleContainerWidget, entry);
  if (result.second) {
    RootBundleControlWidget& newWidget = result.first->second;
    connect(&newWidget, &RootBundleControlWidget::renameRequested, this,
            &HotkeySoundboard::openRenameRootBundleDialog);
    connect(&newWidget, &RootBundleControlWidget::hideRequested, this,
            &HotkeySoundboard::hideRootBundle);
    connect(&newWidget, &RootBundleControlWidget::deleteRequested, this,
            &HotkeySoundboard::deleteEntry);
    connect(&newWidget, &RootBundleControlWidget::changeRandomPlayRequested,
            this, &HotkeySoundboard::changeRandomPlay);
    connect(&newWidget, &RootBundleControlWidget::refreshRequested, this,
            &HotkeySoundboard::refreshRootBundleDisplay);
    connect(&newWidget, &RootBundleControlWidget::filesDropped, this,
            &HotkeySoundboard::addEntriesFromFiles);
    connect(&newWidget, &RootBundleControlWidget::entryDropped, this,
            &HotkeySoundboard::moveEntry);
    connect(&newWidget, &RootBundleControlWidget::playRequested, this,
            &HotkeySoundboard::playEntry);
    connect(&newWidget, &RootBundleControlWidget::weightChangeRequested, this,
            &HotkeySoundboard::changeEntryWeight);
    std::string bundleName = bundleEntry->getName();
    if (isRootBundleNameValid(bundleName)) {
      rootBundleNames.insert(bundleName);
    } else {
      bundleEntry->setName(BundleDefaults::NAME);
    }
    rootBundleFlowLayout->insertWidget(rootBundleWidgetLowerBound(entry),
                                       &newWidget);
    newWidget.refreshRootBundleDisplay(*bundleEntry);
    newWidget.show();
  } else {
    qWarning("Failed to create a new root bundle control widget. ID already "
             "exists.");
  }
}

void HotkeySoundboard::reloadAllHotkeys() {
  hotkeyManager->unregisterAllHotkeys();
  CategoryHandle cat = currentCategory;
  currentCategory = InvalidCategoryHandle;
  changeCategory(cat);
}

std::function<void(void*)>
HotkeySoundboard::hotkeyActionToFunction(const HotkeyAction& action) {
  switch (action.type) {
  case HotkeyAction::Type::ChangeCategory:
    return [this, action](void*) { changeCategory(action.targetCategory); };
  case HotkeyAction::Type::PlayEntry:
    qDebug("Playing entry with handle: %d", action.targetEntry);
    return [this, action](void*) { playEntry(action.targetEntry); };
  case HotkeyAction::Type::StopEntry:
    // TODO
    return [](void*) {};
  case HotkeyAction::Type::StopAll:
    return [this](void*) { soundboard->stopAllEntries(); };
  case HotkeyAction::Type::None:
  default:
    return [](void*) {};
  }
}

void HotkeySoundboard::onCategoriesChanged(QList<CategoryHandle> added,
                                           QList<CategoryHandle> removed) {
  Q_UNUSED(added);
  for (CategoryHandle cat : removed) {
    if (cat == currentCategory) {
      changeCategory(GlobalCategoryHandle);
      break;
    }
  }
}

void HotkeySoundboard::changeEntryWeight(sb::EntryHandle parent, int index,
                                         unsigned int newWeight) {
  if (!soundboard->isValidEntry(parent)) {
    qWarning("Cannot change weight of an invalid entry.");
    return;
  }
  soundboard->setEntryWeightViaParent(parent, index, newWeight);
}

void HotkeySoundboard::changeRandomPlay(sb::EntryHandle entry,
                                        bool randomPlay) {
  if (!soundboard->isValidEntry(entry)) {
    qWarning("Cannot change random play of an invalid entry.");
    return;
  }
  sb::PlayableEntry* entryPtr = soundboard->getEntry(entry);
  if (!entryPtr) {
    qWarning("Failed to retrieve entry for handle '%d'.", entry);
    return;
  }
  if (entryPtr->type != sb::PlayableEntry::Type::Bundle) {
    qWarning("Entry is not a bundle.");
    return;
  }
  sb::BundleEntry* bundleEntry = static_cast<sb::BundleEntry*>(entryPtr);
  bundleEntry->setRandomPlay(randomPlay);
}

void HotkeySoundboard::newRootBundle() {
  sb::EntryHandle newBundleHandle = soundboard->newBundle(BundleDefaults::NAME);
  if (newBundleHandle == sb::InvalidEntryHandle) {
    qWarning("Failed to create a new root bundle.");
    return;
  } else {
    loadRootBundleControlWidgetFromEntry(newBundleHandle);
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

void HotkeySoundboard::deleteEntry(sb::EntryHandle entry) {
  if (!soundboard->isValidEntry(entry)) {
    qWarning("Cannot delete an invalid sound group.");
    return;
  }
  sb::PlayableEntry* entryPtr = soundboard->getEntry(entry);
  sb::EntryHandle parentHandle = entryPtr->getParentHandle();
  soundboard->deleteEntry(entry);
  bool removeResult = removeRootBundleWidget(entry);
  if (removeResult) {
    rootBundleFlowLayout->invalidate();
    int eraseResult = rootBundleControlWidgets.erase(entry);
    if (eraseResult == 0) {
      qWarning("Failed to remove sound group widget from the map.");
      return;
    }
  } else {
    if (!soundboard->isValidEntry(parentHandle)) {
      qWarning("Failed to remove root bundle widget before deleting entry.");
      return;
    }
    auto parentPairIt = rootBundleControlWidgets.find(parentHandle);
    if (parentPairIt == rootBundleControlWidgets.end()) {
      return;
    }
    parentPairIt->second.refreshRootBundleDisplay(
        *static_cast<sb::BundleEntry*>(soundboard->getEntry(parentHandle)));
  }
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
  // TODO: Implement hide functionality
  // it->second.hide();
  // rootBundleFlowLayout->invalidate();
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

void HotkeySoundboard::openHotkeyManagerDialog() {
  if (!hotkeyModel) {
    qWarning("Hotkey model is not initialized.");
    return;
  }
  hotkeyManager->unregisterAllHotkeys();
  HotkeyManagerDialog hotkeyManagerDialog(this, soundboard.get(), hotkeyModel);
  if (hotkeyManagerDialog.exec() == QDialog::Accepted) {
    loadHotkeyModel(hotkeyManagerDialog.getHotkeyModel());
  }
  reloadAllHotkeys();
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
  bool allPressed = false;
  bool yesToAll = false;
  for (const QUrl& url : urls) {
    if (url.isLocalFile()) {
      std::filesystem::path path = url.toLocalFile().toStdString();
      if (std::filesystem::is_directory(path)) {
        bool recursive = false;
        if (!allPressed) {
          QMessageBox::StandardButton reply;
          reply = QMessageBox::question(
              this, "Add Directory",
              QString("Do you want to recursively add all files from '%1'?")
                  .arg(QString::fromStdString(path.string())),
              QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::No |
                  QMessageBox::NoToAll | QMessageBox::Cancel);
          switch (reply) {
          case QMessageBox::YesToAll:
            allPressed = true;
            yesToAll = true;
            [[fallthrough]];
          case QMessageBox::Yes:
            recursive = true;
            break;
          case QMessageBox::NoToAll:
            allPressed = true;
            yesToAll = false;
            [[fallthrough]];
          case QMessageBox::No:
            recursive = false;
            break;
          case QMessageBox::Cancel:
            qDebug("Add entries operation was canceled.");
            return;
          default:
            qWarning("Unexpected response from message box.");
            return;
          }
        } else {
          recursive = yesToAll;
        }
        if (addDirectoryFromFile(entry, path, index, recursive)) {
          ++index;
        }
      } else if (addSoundFileFromFile(entry, path, index)) {
        ++index;
      }
    } else {
      qWarning("Only local files are supported for adding to the sound group.");
    }
  }
}

bool HotkeySoundboard::addSoundFileFromFile(sb::EntryHandle entry,
                                            const std::filesystem::path& path,
                                            int index) {
  if (!soundboard->isValidEntry(entry)) {
    qWarning("Cannot add a sound file to an invalid sound group.");
    return false;
  }
  if (!std::filesystem::is_regular_file(path)) {
    qWarning("Path '%s' is not a valid file.", path.string().c_str());
    return false;
  }
  if (index < 0) {
    qWarning("Invalid index for adding a sound file to the sound group.");
    return false;
  }
  sb::EntryHandle newHandle =
      soundboard->newSoundFile(path, path.filename().string(), entry, index);
  if (newHandle == sb::InvalidEntryHandle) {
    qWarning("Failed to add sound file '%s' to the sound group.",
             path.string().c_str());
    return false;
  }
  auto it = rootBundleControlWidgets.find(entry);
  if (it != rootBundleControlWidgets.end()) {
    sb::BundleEntry* bundlePtr =
        static_cast<sb::BundleEntry*>(soundboard->getEntry(entry));
    it->second.refreshRootBundleDisplay(*bundlePtr);
    qDebug("Added sound file '%s' to the sound group.", path.string().c_str());
  } else {
#ifndef HKSBNDEBUG
    sb::BundleEntry* bundlePtr =
        static_cast<sb::BundleEntry*>(soundboard->getEntry(entry));
    if (bundlePtr->getParentHandle() == sb::InvalidEntryHandle) {
      qWarning("Failed to find root bundle control widget to refresh.");
    }
#endif // HKSBNDEBUG
  }
  return true;
}

bool HotkeySoundboard::addDirectoryFromFile(sb::EntryHandle entry,
                                            const std::filesystem::path& path,
                                            int index, bool recursive) {
  if (!soundboard->isValidEntry(entry)) {
    qWarning("Cannot add a directory to an invalid sound group.");
    return false;
  }
  if (!std::filesystem::is_directory(path)) {
    qWarning("Path '%s' is not a valid directory.", path.string().c_str());
    return false;
  }
  if (index < 0) {
    qWarning("Invalid index for adding a directory to the sound group.");
    return false;
  }
  sb::EntryHandle newHandle = soundboard->newBundle(
      path.filename().string(), entry, index, path.string(), recursive);
  if (newHandle == sb::InvalidEntryHandle) {
    qWarning("Failed to add new bundle for directory '%s' to the sound group.",
             path.string().c_str());
    return false;
  }
  auto it = rootBundleControlWidgets.find(entry);
  if (it != rootBundleControlWidgets.end()) {
    sb::BundleEntry* bundlePtr =
        static_cast<sb::BundleEntry*>(soundboard->getEntry(entry));
    it->second.refreshRootBundleDisplay(*bundlePtr);
    qDebug("Added directory '%s' to the sound group.", path.string().c_str());
  } else {
#ifndef HKSBNDEBUG
    sb::BundleEntry* bundlePtr =
        static_cast<sb::BundleEntry*>(soundboard->getEntry(entry));
    if (bundlePtr->getParentHandle() == sb::InvalidEntryHandle) {
      qWarning("Failed to find root bundle control widget to refresh.");
    }
#endif // HKSBNDEBUG
  }
  return true;
}

void HotkeySoundboard::loadHotkeyModel(HotkeyTableModel* model) {
  if (!model) {
    qWarning("Cannot load a null hotkey model.");
    return;
  }
  hotkeyModel->loadFromRows(model->getRows());
  hotkeyModel->setCategoryNames(model->getCategoryNames());
}

void HotkeySoundboard::checkNewRootBundleName(const QString& name) {
  renameRootBundleDialog->setValidName(
      isRootBundleNameValid(name.toStdString()));
}

void HotkeySoundboard::moveEntry(sb::EntryHandle parent, int oldIndex,
                                 int newIndex) {
  if (!soundboard->isValidEntry(parent)) {
    qWarning("Cannot move an invalid entry.");
    return;
  }
  if (newIndex < 0 || oldIndex < 0) {
    qWarning("Invalid index for moving an entry.");
    return;
  }
  if (parent == sb::InvalidEntryHandle) {
    qWarning("Entry has no valid parent.");
    return;
  }
  sb::ContainerEntry* parentEntry =
      static_cast<sb::ContainerEntry*>(soundboard->getEntry(parent));
  if (oldIndex < newIndex) {
    parentEntry->rotateEntries(oldIndex, oldIndex + 1, newIndex + 1);
  } else {
    parentEntry->rotateEntries(newIndex, oldIndex, oldIndex + 1);
  }
  auto it = rootBundleControlWidgets.find(parent);
  if (it != rootBundleControlWidgets.end()) {
    sb::BundleEntry* bundlePtr =
        static_cast<sb::BundleEntry*>(soundboard->getEntry(parent));
    it->second.refreshRootBundleDisplay(*bundlePtr);
    qDebug("Moved entry from index %d to %d in the bundle.", oldIndex,
           newIndex);
  } else {
#ifndef HKSBNDEBUG
    sb::BundleEntry* bundlePtr =
        static_cast<sb::BundleEntry*>(soundboard->getEntry(parent));
    if (bundlePtr->getParentHandle() == sb::InvalidEntryHandle) {
      qWarning("Failed to find root bundle control widget to refresh.");
    }
#endif // HKSBNDEBUG
  }
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

void HotkeySoundboard::on_actionOpen_Hotkey_Manager_triggered() {
  openHotkeyManagerDialog();
}

void HotkeySoundboard::reloadRootBundleControlWidgets() {
  // Remove all existing widgets from layout and rootBundleControlWidgets map,
  // then rebuild
  for (auto& pair : rootBundleControlWidgets) {
    removeRootBundleWidget(pair.first);
  }
  rootBundleControlWidgets.clear();
  rootBundleNames.clear();
  for (const auto& [handle, entryPtr] : soundboard->getEntries()) {
    if (entryPtr->type == sb::PlayableEntry::Type::Bundle &&
        entryPtr->getParentHandle() == sb::InvalidEntryHandle) {
      loadRootBundleControlWidgetFromEntry(handle);
    }
  }
}
