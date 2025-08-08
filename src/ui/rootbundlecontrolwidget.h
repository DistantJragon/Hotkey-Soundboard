#ifndef ROOTBUNDLECONTROLWIDGET_H
#define ROOTBUNDLECONTROLWIDGET_H

#include "core/soundboard/bundleentry.h"
#include "core/soundboard/soundboardtypes.h"
#include "qboxlayout.h"
#include <QFrame>

namespace Ui {
class RootBundleControlWidget;
}

class RootBundleControlWidget : public QFrame {
  Q_OBJECT

public:
  explicit RootBundleControlWidget(
      QWidget* parent = nullptr,
      sb::EntryHandle entry = sb::InvalidEntryHandle);
  ~RootBundleControlWidget();
  void refreshRootBundleDisplay(const sb::BundleEntry& entry);
  void updateNameLabel();

  sb::EntryHandle getRootBundleHandle() const { return entry; }

signals:
  void deleteRequested(sb::EntryHandle entry);
  void hideRequested(sb::EntryHandle entry);
  void refreshRequested(sb::EntryHandle entry);
  void renameRequested(sb::EntryHandle entry);

private:
  Ui::RootBundleControlWidget* ui;
  QVBoxLayout* playableEntryLayout = nullptr;
  QVBoxLayout* hotkeyLayout = nullptr;
  const sb::EntryHandle entry;
};

#endif // ROOTBUNDLECONTROLWIDGET_H
