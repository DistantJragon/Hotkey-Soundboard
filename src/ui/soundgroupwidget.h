#ifndef SOUNDGROUPWIDGET_H
#define SOUNDGROUPWIDGET_H

#include "src/core/soundgroup.h"
#include <QWidget>

class SoundGroupWidget : public QWidget {
  Q_OBJECT
public:
  explicit SoundGroupWidget(QWidget *parent = nullptr);

signals:

private:
  SoundGroup *soundGroup;
};

#endif // SOUNDGROUPWIDGET_H
