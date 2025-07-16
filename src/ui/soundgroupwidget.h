#ifndef SOUNDGROUPWIDGET_H
#define SOUNDGROUPWIDGET_H

#include "core/soundgroup.h"
#include <QWidget>

namespace Ui {
class SoundGroupWidget;
}

class SoundGroupWidget : public QWidget {
  Q_OBJECT

public:
  explicit SoundGroupWidget(QWidget* parent = nullptr,
                            SoundGroup* soundGroup = nullptr);
  ~SoundGroupWidget();
  void refreshSoundGroupDisplay();
  void updateNameLabel(const QString& newName);

  const QWidget* getNameLabel() const;
  const SoundGroup* getSoundGroup() const { return soundGroup; }

private:
  Ui::SoundGroupWidget* ui;
  const SoundGroup* soundGroup;
};

#endif // SOUNDGROUPWIDGET_H
