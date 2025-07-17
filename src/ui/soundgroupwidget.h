#ifndef SOUNDGROUPWIDGET_H
#define SOUNDGROUPWIDGET_H

#include "core/soundgroup.h"
#include "qboxlayout.h"
#include <QFrame>

namespace Ui {
class SoundGroupWidget;
}

class SoundGroupWidget : public QFrame {
  Q_OBJECT

public:
  explicit SoundGroupWidget(QWidget* parent = nullptr,
                            SoundGroup* soundGroup = nullptr);
  ~SoundGroupWidget();
  void refreshSoundGroupDisplay();
  void updateNameLabel();

  const SoundGroup* getSoundGroup() const { return soundGroup; }

signals:
  void renameRequested(SoundGroup* soundGroup);

private:
  Ui::SoundGroupWidget* ui;
  SoundGroup* const soundGroup;

  QVBoxLayout* playableEntryLayout = nullptr;
  QVBoxLayout* hotkeyLayout = nullptr;
};

#endif // SOUNDGROUPWIDGET_H
