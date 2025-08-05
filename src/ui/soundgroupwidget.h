#ifndef SOUNDGROUPWIDGET_H
#define SOUNDGROUPWIDGET_H

#include "core/soundboard/soundboardtypes.h"
#include "core/soundboard/soundgroup.h"
#include "qboxlayout.h"
#include <QFrame>

namespace Ui {
class SoundGroupWidget;
}

class SoundGroupWidget : public QFrame {
  Q_OBJECT

public:
  explicit SoundGroupWidget(
      QWidget* parent = nullptr,
      sb::GroupHandle soundGroup = sb::InvalidGroupHandle);
  ~SoundGroupWidget();
  void refreshSoundGroupDisplay(const sb::SoundGroup& soundGroup);
  void updateNameLabel();

  sb::GroupHandle getSoundGroupHandle() const { return soundGroup; }

signals:
  void deleteRequested(sb::GroupHandle soundGroup);
  void hideRequested(sb::GroupHandle soundGroup);
  void refreshRequested(sb::GroupHandle soundGroup);
  void renameRequested(sb::GroupHandle soundGroup);

private:
  Ui::SoundGroupWidget* ui;
  const sb::GroupHandle soundGroup;

  QVBoxLayout* playableEntryLayout = nullptr;
  QVBoxLayout* hotkeyLayout = nullptr;
};

#endif // SOUNDGROUPWIDGET_H
