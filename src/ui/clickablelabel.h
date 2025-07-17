#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include "qevent.h"
#include <QLabel>

class ClickableLabel : public QLabel {
  Q_OBJECT

public:
  explicit ClickableLabel(QWidget* parent = nullptr);

signals:
  void clicked();
  void doubleClicked();

protected:
  void mousePressEvent(QMouseEvent* event) override;

  void mouseDoubleClickEvent(QMouseEvent* event) override;
};
#endif // CLICKABLELABEL_H
