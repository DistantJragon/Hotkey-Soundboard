#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include "core/soundboard/soundboard.h"
#include "ui/hotkey/hotkeyguitypes.h"
#include <QJsonObject>
#include <vector>

static constexpr unsigned int CONFIG_VERSION = 1;

class ConfigManager {
public:
  ConfigManager() = default;
  void readConfigFile();

  bool
  loadConfig(sb::Soundboard& soundboard, QList<HotkeyRow>& hotkeyRows,
             std::unordered_map<CategoryHandle, std::string>& categoryNames);

  bool parseSoundboardObject(sb::Soundboard& soundboard,
                             const QJsonArray& soundboardArray);

  QJsonArray createSoundboardArray(const sb::Soundboard& soundboard) const;

  bool parseEntryObject(sb::Soundboard& soundboard, const QJsonObject& entryObj,
                        sb::EntryHandle parent);

  QJsonObject createEntryObject(const sb::PlayableEntry* entry) const;

  bool parseHotkeyArrayObject(
      QList<HotkeyRow>& hotkeyRows, const QJsonArray& hotkeysArray,
      std::unordered_map<CategoryHandle, std::string>& categoryNames);

  QJsonArray
  createHotkeyArray(const QList<HotkeyRow>& hotkeyRows,
                    const std::unordered_map<CategoryHandle, std::string>&
                        categoryNames) const;

  bool parseHotkeyRowObject(
      HotkeyRow& hotkeyRow, const QJsonObject& hotkeyObj,
      std::unordered_map<CategoryHandle, std::string>& categoryNames);

  QJsonObject
  createHotkeyRowObject(const HotkeyRow& hotkeyRow,
                        const std::unordered_map<CategoryHandle, std::string>&
                            categoryNames) const;

  bool parseHotkeyObject(sb::hotkey::Hotkey& hotkey,
                         const QJsonObject& hotkeyObj);

  QJsonObject createHotkeyObject(const sb::hotkey::Hotkey& hotkey) const;

  bool parseHotkeyActionObject(
      HotkeyAction& action, const QJsonObject& actionObj,
      std::unordered_map<CategoryHandle, std::string>& categoryNames);

  QJsonObject createHotkeyActionObject(
      const HotkeyAction& action,
      const std::unordered_map<CategoryHandle, std::string>& categoryNames)
      const;

  QJsonObject toJsonObject(
      const sb::Soundboard& soundboard, const QList<HotkeyRow>& hotkeyRows,
      const std::unordered_map<CategoryHandle, std::string>& categoryNames)
      const;

  bool saveConfig(
      const sb::Soundboard& soundboard, const QList<HotkeyRow>& hotkeyRows,
      const std::unordered_map<CategoryHandle, std::string>& categoryNames);

  const QJsonDocument& getConfigDoc() const { return configDoc_; }

private:
  QJsonDocument configDoc_;
  std::unordered_map<sb::EntryHandle, sb::EntryHandle> handleMap_;
};

#endif // CONFIGMANAGER_H
