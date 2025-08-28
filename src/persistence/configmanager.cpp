#include "persistence/configmanager.h"
#include "core/soundboard/bundleentry.h"
#include "core/soundboard/containerentry.h"
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QStandardPaths>

void ConfigManager::readConfigFile() {
  QString dir =
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  QDir().mkpath(dir);
  QFile configFile(dir + "/appstate.json");
  if (!configFile.open(QIODevice::ReadOnly)) {
    return;
  }
  QByteArray data = configFile.readAll();
  configFile.close();
  configDoc_ = QJsonDocument::fromJson(data);
}

bool ConfigManager::loadConfig(
    sb::Soundboard& soundboard, QList<HotkeyRow>& hotkeyRows,
    std::unordered_map<CategoryHandle, std::string>& categoryNames) {
  if (configDoc_.isNull() || !configDoc_.isObject()) {
    return false;
  }
  QJsonObject rootObj = configDoc_.object();
  if (!rootObj.contains("version") || !rootObj["version"].isDouble()) {
    return false;
  }
  unsigned int rootVersion =
      static_cast<unsigned int>(rootObj["version"].toInt());
  if (rootVersion != CONFIG_VERSION) {
    return false;
  }
  if (!rootObj.contains("soundboard") || !rootObj["soundboard"].isArray()) {
    return false;
  }
  QJsonArray soundboardArray = rootObj["soundboard"].toArray();
  if (!rootObj.contains("hotkeys") || !rootObj["hotkeys"].isArray()) {
    return false;
  }
  QJsonArray hotkeysArray = rootObj["hotkeys"].toArray();
  handleMap_.clear();
  if (!parseSoundboardObject(soundboard, soundboardArray)) {
    return false;
  }
  if (!parseHotkeyArrayObject(hotkeyRows, hotkeysArray, categoryNames)) {
    return false;
  }
  return true;
}

bool ConfigManager::parseSoundboardObject(sb::Soundboard& soundboard,
                                          const QJsonArray& soundboardArray) {
  for (const QJsonValue& entryVal : soundboardArray) {
    if (!entryVal.isObject()) {
      return false;
    }
    QJsonObject entryObj = entryVal.toObject();
    if (!parseEntryObject(soundboard, entryObj, sb::InvalidEntryHandle)) {
      return false;
    }
  }
  return true;
}

bool ConfigManager::parseEntryObject(sb::Soundboard& soundboard,
                                     const QJsonObject& entryObj,
                                     sb::EntryHandle parent) {
  if (!entryObj.contains("type") || !entryObj["type"].isString() ||
      !entryObj.contains("name") || !entryObj["name"].isString() ||
      !entryObj.contains("handle") || !entryObj["handle"].isDouble() ||
      !entryObj.contains("path") || !entryObj["path"].isString() ||
      !entryObj.contains("weight") || !entryObj["weight"].isDouble()) {
    return false;
  }
  std::string typeStr = entryObj["type"].toString().toStdString();
  std::string name = entryObj["name"].toString().toStdString();
  sb::EntryHandle oldHandle =
      static_cast<sb::EntryHandle>(entryObj["handle"].toInt());
  std::string path = entryObj["path"].toString().toStdString();
  unsigned int weight = static_cast<unsigned int>(entryObj["weight"].toInt());

  sb::PlayableEntry::Type type;
  if (typeStr == "SoundFile") {
    type = sb::PlayableEntry::Type::SoundFile;
  } else if (typeStr == "Bundle") {
    type = sb::PlayableEntry::Type::Bundle;
  } else if (typeStr == "String") {
    type = sb::PlayableEntry::Type::String;
  } else {
    return false;
  }
  sb::EntryHandle newHandle;
  switch (type) {
  case sb::PlayableEntry::Type::SoundFile: {
    if (parent == sb::InvalidEntryHandle) {
      return false;
    }
    sb::ContainerEntry* parentEntry =
        static_cast<sb::ContainerEntry*>(soundboard.getEntry(parent));
    if (!parentEntry) {
      return false;
    }
    newHandle = soundboard.newSoundFile(path, name, parent,
                                        parentEntry->getChildren().size());
    if (newHandle == sb::InvalidEntryHandle) {
      return false;
    }
    switch (parentEntry->type) {
    case sb::PlayableEntry::Type::Bundle:
      static_cast<sb::BundleEntry*>(parentEntry)
          ->setChildWeight(parentEntry->getChildren().size() - 1, weight);
      break;
    case sb::PlayableEntry::Type::String:
      // String entries are not supported yet
      return false;
    default:
      return false;
    }
    handleMap_[oldHandle] = newHandle;
    return true;
  }
  case sb::PlayableEntry::Type::Bundle: {
    if (!entryObj.contains("randomPlay") || !entryObj["randomPlay"].isBool() ||
        !entryObj.contains("recursive") || !entryObj["recursive"].isBool()) {
      return false;
    }
    bool randomPlay = entryObj["randomPlay"].toBool();
    bool recursive = entryObj["recursive"].toBool();
    if (parent == sb::InvalidEntryHandle) {
      newHandle = soundboard.newBundle(name);
    } else {
      sb::ContainerEntry* parentEntry =
          static_cast<sb::ContainerEntry*>(soundboard.getEntry(parent));
      if (!parentEntry ||
          !sb::PlayableEntry::isContainerType(parentEntry->type)) {
        return false;
      }
      if (path.empty()) {
        newHandle = soundboard.newBundle(name, parent,
                                         parentEntry->getChildren().size());
      } else {
        newHandle = soundboard.newBundle(
            name, parent, parentEntry->getChildren().size(), path, recursive);
      }
    }
    if (newHandle == sb::InvalidEntryHandle) {
      return false;
    }
    handleMap_[oldHandle] = newHandle;
    if (!path.empty()) {
      return true;
    } else {
      if (!entryObj.contains("children") || !entryObj["children"].isArray()) {
        return false;
      }
      sb::BundleEntry* bundleEntry =
          static_cast<sb::BundleEntry*>(soundboard.getEntry(newHandle));
      if (!bundleEntry) {
        return false;
      }
      bundleEntry->setRandomPlay(randomPlay);
      QJsonArray childrenArray = entryObj["children"].toArray();
      for (const QJsonValue& childVal : childrenArray) {
        if (!childVal.isObject()) {
          return false;
        }
        QJsonObject childObj = childVal.toObject();
        bool result = parseEntryObject(soundboard, childObj, newHandle);
        if (!result) {
          return false;
        }
      }
      return true;
    }
  }
  case sb::PlayableEntry::Type::String: {
    // String entries are not supported yet
    return false;
  }
  }
}

bool ConfigManager::parseHotkeyArrayObject(
    QList<HotkeyRow>& hotkeyRows, const QJsonArray& hotkeysArray,
    std::unordered_map<CategoryHandle, std::string>& categoryNames) {
  categoryNames[GlobalCategoryHandle] = "Global";
  for (const QJsonValue& hotkeyVal : hotkeysArray) {
    if (!hotkeyVal.isObject()) {
      return false;
    }
    QJsonObject hotkeyObj = hotkeyVal.toObject();
    HotkeyRow hotkeyRow;
    if (!parseHotkeyRowObject(hotkeyRow, hotkeyObj, categoryNames)) {
      return false;
    }
    hotkeyRows.push_back(hotkeyRow);
  }
  return true;
}

bool ConfigManager::parseHotkeyRowObject(
    HotkeyRow& hotkeyRow, const QJsonObject& hotkeyObj,
    std::unordered_map<CategoryHandle, std::string>& categoryNames) {
  if (!hotkeyObj.contains("category") || !hotkeyObj["category"].isString() ||
      !hotkeyObj.contains("hotkey") || !hotkeyObj["hotkey"].isObject() ||
      !hotkeyObj.contains("action") || !hotkeyObj["action"].isObject()) {
    return false;
  }
  std::string categoryName = hotkeyObj["category"].toString().toStdString();
  QJsonObject hotkeyDataObj = hotkeyObj["hotkey"].toObject();
  QJsonObject actionObj = hotkeyObj["action"].toObject();
  CategoryHandle categoryHandle = InvalidCategoryHandle;
  for (const auto& [handle, name] : categoryNames) {
    if (name == categoryName) {
      categoryHandle = handle;
      break;
    }
  }
  if (categoryHandle == InvalidCategoryHandle) {
    categoryHandle = static_cast<CategoryHandle>(categoryNames.size());
    categoryNames[categoryHandle] = categoryName;
  }
  sb::hotkey::Hotkey hotkey;
  if (!parseHotkeyObject(hotkey, hotkeyDataObj)) {
    return false;
  }
  if (!parseHotkeyActionObject(hotkeyRow.action, actionObj, categoryNames)) {
    return false;
  }
  hotkeyRow.category = categoryHandle;
  hotkeyRow.hotkey = hotkey;
  return true;
}

bool ConfigManager::parseHotkeyObject(sb::hotkey::Hotkey& hotkey,
                                      const QJsonObject& hotkeyObj) {
  if (!hotkeyObj.contains("humanReadable") ||
      !hotkeyObj["humanReadable"].isString() ||
      !hotkeyObj.contains("portableModifiers") ||
      !hotkeyObj["portableModifiers"].isDouble() ||
      !hotkeyObj.contains("portableKey") ||
      !hotkeyObj["portableKey"].isDouble() ||
      !hotkeyObj.contains("nativeModifiers") ||
      !hotkeyObj["nativeModifiers"].isDouble() ||
      !hotkeyObj.contains("nativeVirtualKey") ||
      !hotkeyObj["nativeVirtualKey"].isDouble() ||
      !hotkeyObj.contains("nativeScanCode") ||
      !hotkeyObj["nativeScanCode"].isDouble()) {
    return false;
  }
  hotkey.humanReadable = hotkeyObj["humanReadable"].toString().toStdString();
  hotkey.portableModifiers =
      static_cast<unsigned long>(hotkeyObj["portableModifiers"].toInt());
  hotkey.portableKey =
      static_cast<unsigned long>(hotkeyObj["portableKey"].toInt());
  hotkey.nativeModifiers =
      static_cast<unsigned long>(hotkeyObj["nativeModifiers"].toInt());
  hotkey.nativeVirtualKey =
      static_cast<unsigned long>(hotkeyObj["nativeVirtualKey"].toInt());
  hotkey.nativeScanCode =
      static_cast<unsigned long>(hotkeyObj["nativeScanCode"].toInt());
  return true;
}

bool ConfigManager::parseHotkeyActionObject(
    HotkeyAction& action, const QJsonObject& actionObj,
    std::unordered_map<CategoryHandle, std::string>& categoryNames) {
  if (!actionObj.contains("type") || !actionObj["type"].isString()) {
    return false;
  }
  std::string typeStr = actionObj["type"].toString().toStdString();
  HotkeyAction::Type type;
  if (typeStr == "None") {
    type = HotkeyAction::Type::None;
  } else if (typeStr == "ChangeCategory") {
    type = HotkeyAction::Type::ChangeCategory;
  } else if (typeStr == "PlayEntry") {
    type = HotkeyAction::Type::PlayEntry;
  } else if (typeStr == "StopEntry") {
    type = HotkeyAction::Type::StopEntry;
  } else if (typeStr == "StopAll") {
    type = HotkeyAction::Type::StopAll;
  } else {
    return false;
  }
  action.type = type;
  sb::EntryHandle oldTargetHandle = sb::InvalidEntryHandle;
  std::string targetCategoryName;
  switch (type) {
  case HotkeyAction::Type::None:
  case HotkeyAction::Type::StopAll:
    action.targetCategory = InvalidCategoryHandle;
    action.targetEntry = sb::InvalidEntryHandle;
    return true;
  case HotkeyAction::Type::ChangeCategory:
    if (!actionObj.contains("targetCategory") ||
        !actionObj["targetCategory"].isString()) {
      return false;
    }
    targetCategoryName = actionObj["targetCategory"].toString().toStdString();
    for (const auto& [handle, name] : categoryNames) {
      if (name == targetCategoryName) {
        action.targetCategory = handle;
        return true;
      }
    }
    if (action.targetCategory == InvalidCategoryHandle) {
      action.targetCategory = static_cast<CategoryHandle>(categoryNames.size());
      categoryNames[action.targetCategory] = targetCategoryName;
      action.targetEntry = sb::InvalidEntryHandle;
    }
    return true;
  case HotkeyAction::Type::PlayEntry:
  case HotkeyAction::Type::StopEntry:
    if (!actionObj.contains("targetEntry") ||
        !actionObj["targetEntry"].isDouble()) {
      return false;
    }
    oldTargetHandle =
        static_cast<sb::EntryHandle>(actionObj["targetEntry"].toInt());
    if (handleMap_.find(oldTargetHandle) == handleMap_.end()) {
      return false;
    }
    action.targetEntry = handleMap_[oldTargetHandle];
    action.targetCategory = InvalidCategoryHandle;
    return true;
  default:
    return false;
  }
}

bool ConfigManager::saveConfig(
    const sb::Soundboard& soundboard, const QList<HotkeyRow>& hotkeyRows,
    const std::unordered_map<CategoryHandle, std::string>& categoryNames) {
  QJsonObject rootObj = toJsonObject(soundboard, hotkeyRows, categoryNames);
  if (rootObj.isEmpty()) {
    return false;
  }
  QString dir =
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  QDir().mkpath(dir);
  QFile configFile(dir + "/appstate.json");
  if (!configFile.open(QIODevice::WriteOnly)) {
    return false;
  }
  configDoc_ = QJsonDocument(rootObj);
  configFile.write(configDoc_.toJson());
  configFile.close();
  return true;
}

QJsonObject ConfigManager::toJsonObject(
    const sb::Soundboard& soundboard, const QList<HotkeyRow>& hotkeyRows,
    const std::unordered_map<CategoryHandle, std::string>& categoryNames)
    const {
  QJsonObject rootObj;
  rootObj["version"] = static_cast<int>(CONFIG_VERSION);
  rootObj["soundboard"] = createSoundboardArray(soundboard);
  rootObj["hotkeys"] = createHotkeyArray(hotkeyRows, categoryNames);
  return rootObj;
}

QJsonArray
ConfigManager::createSoundboardArray(const sb::Soundboard& soundboard) const {
  QJsonArray soundboardArray;
  for (const auto& [handle, entry] : soundboard.getEntries()) {
    if (!entry || entry->getParentHandle() != sb::InvalidEntryHandle) {
      continue;
    }
    QJsonObject entryObj = createEntryObject(entry.get());
    if (entryObj.isEmpty()) {
      continue;
    }
    soundboardArray.append(entryObj);
  }
  return soundboardArray;
}

QJsonObject
ConfigManager::createEntryObject(const sb::PlayableEntry* entry) const {
  if (!entry) {
    return QJsonObject();
  }
  QJsonObject entryObj;
  std::string typeStr;
  switch (entry->type) {
  case sb::PlayableEntry::Type::SoundFile:
    typeStr = "SoundFile";
    break;
  case sb::PlayableEntry::Type::Bundle:
    typeStr = "Bundle";
    break;
  case sb::PlayableEntry::Type::String:
    typeStr = "String";
    break;
  default:
    return QJsonObject();
  }
  entryObj["type"] = QString::fromStdString(typeStr);
  entryObj["name"] = QString::fromStdString(entry->getName());
  entryObj["handle"] = static_cast<int>(entry->getHandle());
  entryObj["path"] = QString::fromStdString(entry->getPath());
  entryObj["weight"] = static_cast<int>(entry->getWeight());
  switch (entry->type) {
  case sb::PlayableEntry::Type::Bundle: {
    const sb::BundleEntry* bundleEntry =
        static_cast<const sb::BundleEntry*>(entry);
    entryObj["randomPlay"] = bundleEntry->isRandomPlay();
    entryObj["recursive"] = bundleEntry->isRecursive();
    if (!entry->getPath().empty()) {
      // If the bundle has a path, we don't need to save its children
      break;
    }
    QJsonArray childrenArray;
    for (const sb::PlayableEntry* child : bundleEntry->getChildren()) {
      QJsonObject childObj = createEntryObject(child);
      if (childObj.isEmpty()) {
        continue;
      }
      childrenArray.append(childObj);
    }
    entryObj["children"] = childrenArray;
    break;
  }
  case sb::PlayableEntry::Type::SoundFile:
    break;
  case sb::PlayableEntry::Type::String:
    // String entries are not supported yet
    break;
  default:
    return QJsonObject();
  }
  return entryObj;
}

QJsonArray ConfigManager::createHotkeyArray(
    const QList<HotkeyRow>& hotkeyRows,
    const std::unordered_map<CategoryHandle, std::string>& categoryNames)
    const {
  QJsonArray hotkeysArray;
  for (const HotkeyRow& hotkeyRow : hotkeyRows) {
    QJsonObject hotkeyObj = createHotkeyRowObject(hotkeyRow, categoryNames);
    if (hotkeyObj.isEmpty()) {
      continue;
    }
    hotkeysArray.append(hotkeyObj);
  }
  return hotkeysArray;
}

QJsonObject ConfigManager::createHotkeyRowObject(
    const HotkeyRow& hotkeyRow,
    const std::unordered_map<CategoryHandle, std::string>& categoryNames)
    const {
  QJsonObject hotkeyObj;
  std::string categoryName = "Global";
  if (categoryNames.find(hotkeyRow.category) != categoryNames.end()) {
    categoryName = categoryNames.at(hotkeyRow.category);
  }
  hotkeyObj["category"] = QString::fromStdString(categoryName);
  QJsonObject hotkeyDataObj = createHotkeyObject(hotkeyRow.hotkey);
  if (hotkeyDataObj.isEmpty()) {
    return QJsonObject();
  }
  hotkeyObj["hotkey"] = hotkeyDataObj;
  QJsonObject actionObj =
      createHotkeyActionObject(hotkeyRow.action, categoryNames);
  if (actionObj.isEmpty()) {
    return QJsonObject();
  }
  hotkeyObj["action"] = actionObj;
  return hotkeyObj;
}

QJsonObject
ConfigManager::createHotkeyObject(const sb::hotkey::Hotkey& hotkey) const {
  QJsonObject hotkeyObj;
  hotkeyObj["humanReadable"] = QString::fromStdString(hotkey.humanReadable);
  hotkeyObj["portableModifiers"] = static_cast<int>(hotkey.portableModifiers);
  hotkeyObj["portableKey"] = static_cast<int>(hotkey.portableKey);
  hotkeyObj["nativeModifiers"] = static_cast<int>(hotkey.nativeModifiers);
  hotkeyObj["nativeVirtualKey"] = static_cast<int>(hotkey.nativeVirtualKey);
  hotkeyObj["nativeScanCode"] = static_cast<int>(hotkey.nativeScanCode);
  return hotkeyObj;
}

QJsonObject ConfigManager::createHotkeyActionObject(
    const HotkeyAction& action,
    const std::unordered_map<CategoryHandle, std::string>& categoryNames)
    const {
  QJsonObject actionObj;
  std::string typeStr;
  switch (action.type) {
  case HotkeyAction::Type::None:
    typeStr = "None";
    break;
  case HotkeyAction::Type::ChangeCategory:
    typeStr = "ChangeCategory";
    break;
  case HotkeyAction::Type::PlayEntry:
    typeStr = "PlayEntry";
    break;
  case HotkeyAction::Type::StopEntry:
    typeStr = "StopEntry";
    break;
  case HotkeyAction::Type::StopAll:
    typeStr = "StopAll";
    break;
  default:
    return QJsonObject();
  }
  actionObj["type"] = QString::fromStdString(typeStr);
  switch (action.type) {
  case HotkeyAction::Type::None:
  case HotkeyAction::Type::StopAll:
    break;
  case HotkeyAction::Type::ChangeCategory: {
    std::string targetCategoryName = "Global";
    auto pairIt = categoryNames.find(action.targetCategory);
    if (pairIt != categoryNames.end()) {
      targetCategoryName = pairIt->second;
    }
    actionObj["targetCategory"] = QString::fromStdString(targetCategoryName);
    break;
  }
  case HotkeyAction::Type::PlayEntry:
  case HotkeyAction::Type::StopEntry:
    actionObj["targetEntry"] = static_cast<int>(action.targetEntry);
    break;
  default:
    return QJsonObject();
  }
  return actionObj;
}
