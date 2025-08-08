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
  explicit PlayableEntryFrame(
      QWidget* parent = nullptr,
      std::unique_ptr<sb::PlayableEntry> entry = nullptr);
  ~PlayableEntryFrame();

private:
  Ui::PlayableEntryFrame* ui;
};

#endif // PLAYABLEENTRYFRAME_H
