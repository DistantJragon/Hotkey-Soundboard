#ifndef SOUNDENTRYWIDGET_H
#define SOUNDENTRYWIDGET_H

#include <QWidget>

class SoundEntryWidget : public QWidget {
  Q_OBJECT
public:
  explicit SoundEntryWidget(QWidget* parent = nullptr);

protected:
  // void mousePressEvent(QMouseEvent* event) override;
signals:
};

#endif // SOUNDENTRYWIDGET_H
