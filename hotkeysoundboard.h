#ifndef HOTKEYSOUNDBOARD_H
#define HOTKEYSOUNDBOARD_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class HotkeySoundboard;
}
QT_END_NAMESPACE

class HotkeySoundboard : public QMainWindow
{
    Q_OBJECT

public:
    HotkeySoundboard(QWidget *parent = nullptr);
    ~HotkeySoundboard();

private:
    Ui::HotkeySoundboard *ui;
};
#endif // HOTKEYSOUNDBOARD_H
