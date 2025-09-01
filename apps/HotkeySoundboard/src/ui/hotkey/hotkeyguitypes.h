#ifndef HOTKEYGUITYPES_H
#define HOTKEYGUITYPES_H

#include "core/hotkey/hotkey.h"
#include "core/soundboard/soundboardtypes.h"
#include <Qt>

using CategoryHandle = unsigned int;
static constexpr CategoryHandle GlobalCategoryHandle =
    static_cast<CategoryHandle>(0);
static constexpr CategoryHandle InvalidCategoryHandle =
    static_cast<CategoryHandle>(-1);

enum class HotkeyColumn { Enabled = 0, Category, Hotkey, Action };

struct HotkeyAction {
  enum class Type {
    None,
    ChangeCategory,
    PlayEntry,
    StopEntry,
    StopAll
  } type = Type::None;
  CategoryHandle targetCategory = InvalidCategoryHandle;
  sb::EntryHandle targetEntry = sb::InvalidEntryHandle;
};

enum class HotkeyRoles {
  HumanReadableRole = Qt::UserRole + 1,
  PortableModifiersRole,
  PortableKeyRole,
  NativeModifiersRole,
  NativeVirtualKeyRole,
  NativeScanCodeRole,
  ActionTypeRole,
  ActionTargetCategoryRole,
  ActionTargetEntryRole,
};

struct HotkeyRow {
  sb::hotkey::Hotkey hotkey;
  HotkeyAction action;
  sb::hotkey::HotkeyHandle managerHandle = sb::hotkey::InvalidHotkeyHandle;
  CategoryHandle category = InvalidCategoryHandle;
  bool enabled = true;
  bool conflict = false;
};

#endif // HOTKEYGUITYPES_H
