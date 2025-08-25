#ifndef HOTKEYDELEGATE_H
#define HOTKEYDELEGATE_H

#include <QStyledItemDelegate>

class HotkeyDelegate : public QStyledItemDelegate
{
  Q_OBJECT
public:
  explicit HotkeyDelegate (QObject *parent = nullptr);
};

#endif // HOTKEYDELEGATE_H
