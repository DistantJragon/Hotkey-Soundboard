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
                              sb::PlayableEntry* entry = nullptr);
  ~PlayableEntryFrame();

signals:
  void playRequested(sb::EntryHandle entry);

private:
  Ui::PlayableEntryFrame* ui;
};

#endif // PLAYABLEENTRYFRAME_H
