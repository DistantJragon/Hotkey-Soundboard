#include "clickablelabel.h"

ClickableLabel::ClickableLabel(QWidget* parent) : QLabel(parent) {
  setCursor(Qt::PointingHandCursor);
}

void ClickableLabel::mousePressEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    emit clicked();
  }
  QLabel::mousePressEvent(event);
}

void ClickableLabel::mouseDoubleClickEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    emit doubleClicked();
  }
  QLabel::mouseDoubleClickEvent(event);
}
