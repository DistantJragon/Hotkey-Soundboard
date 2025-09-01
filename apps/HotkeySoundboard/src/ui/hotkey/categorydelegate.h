#ifndef CATEGORYDELEGATE_H
#define CATEGORYDELEGATE_H

#include <QStyledItemDelegate>

class CategoryDelegate : public QStyledItemDelegate {
  Q_OBJECT
public:
  explicit CategoryDelegate(QObject* parent = nullptr);
};

#endif // CATEGORYDELEGATE_H
