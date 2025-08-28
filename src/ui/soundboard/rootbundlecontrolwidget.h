#ifndef ROOTBUNDLECONTROLWIDGET_H
#define ROOTBUNDLECONTROLWIDGET_H

#include "core/soundboard/bundleentry.h"
#include "core/soundboard/soundboardtypes.h"
#include <QFrame>
#include <QList>
#include <QUrl>
#include <QVBoxLayout>

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
  void changeRandomPlayRequested(sb::EntryHandle entry, bool randomPlay);
  void changeRecursiveRequested(sb::EntryHandle entry, bool recursive);
  void deleteRequested(sb::EntryHandle entry);
  void entryDropped(sb::EntryHandle parent, int oldIndex, int newIndex);
  void filesDropped(sb::EntryHandle entry, const QList<QUrl>& urls,
                    int index = -1);
  void hideRequested(sb::EntryHandle entry);
  void playRequested(sb::EntryHandle entry);
  void refreshRequested(sb::EntryHandle entry);
  void renameRequested(sb::EntryHandle entry);
  void syncWeightSumChangeRequested(sb::EntryHandle entry, bool syncWeightSum);
  void weightChangeRequested(sb::EntryHandle parent, int index,
                             unsigned int weight);

private:
  Ui::RootBundleControlWidget* ui;
  QVBoxLayout* playableEntryLayout = nullptr;
  const sb::EntryHandle entry;

private slots:
  void confirmDelete();
};

#endif // ROOTBUNDLECONTROLWIDGET_H
