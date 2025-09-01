#ifndef PLAYABLEENTRYFRAME_H
#define PLAYABLEENTRYFRAME_H

#include "core/soundboard/playableentry.h"
#include <QFrame>

namespace Ui {
class PlayableEntryFrame;
}

class PlayableEntryFrame : public QFrame {
  Q_OBJECT

public:
  explicit PlayableEntryFrame(QWidget* parent = nullptr,
                              sb::PlayableEntry* entry = nullptr,
                              int indexInParent = -1);
  ~PlayableEntryFrame();

  sb::PlayableEntry* getEntry() const { return entry; }

signals:
  void deleteRequested(sb::EntryHandle entry);
  void playRequested(sb::EntryHandle entry);
  void renameRequested(sb::EntryHandle entry, const std::string& newName);
  void syncWeightSumChangeRequested(sb::EntryHandle entry, bool sync);
  void weightChangeRequested(int index, unsigned int weight);

protected:
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;

#ifndef QT_NO_CONTEXTMENU
  void contextMenuEvent(QContextMenuEvent* event) override;
#endif // QT_NO_CONTEXTMENU

private slots:
  void confirmDelete();

private:
  Ui::PlayableEntryFrame* ui;
  sb::PlayableEntry* entry = nullptr;
  int indexInParent = -1;
  QPointF dragStartPosition;
  QAction* toggleRecursiveAction = nullptr;
  QAction* toggleSyncWeightSumAction = nullptr;

  void createActions(sb::PlayableEntry::Type type);
};

#endif // PLAYABLEENTRYFRAME_H
