#ifndef ACTIONDELEGATE_H
#define ACTIONDELEGATE_H

#include <QStyledItemDelegate>

class ActionDelegate : public QStyledItemDelegate {
  Q_OBJECT
public:
  explicit ActionDelegate(QObject* parent = nullptr);
};

#endif // ACTIONDELEGATE_H
