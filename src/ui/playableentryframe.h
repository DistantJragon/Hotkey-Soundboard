#ifndef PLAYABLEENTRYFRAME_H
#define PLAYABLEENTRYFRAME_H

#include <QFrame>

namespace Ui {
class PlayableEntryFrame;
}

class PlayableEntryFrame : public QFrame {
  Q_OBJECT

public:
  explicit PlayableEntryFrame(QWidget* parent = nullptr);
  ~PlayableEntryFrame();

private:
  Ui::PlayableEntryFrame* ui;
};

#endif // PLAYABLEENTRYFRAME_H
