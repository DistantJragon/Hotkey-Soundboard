#ifndef HOTKEYSOUNDBOARD_H
#define HOTKEYSOUNDBOARD_H

#include <QMainWindow>
#include "src/core/soundboard.h"
#include "src/ui/flowlayout.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class HotkeySoundboard;
}
QT_END_NAMESPACE

class HotkeySoundboard : public QMainWindow {
    Q_OBJECT

public:
    HotkeySoundboard(QWidget *parent = nullptr);
    ~HotkeySoundboard();

    void setupSoundGroupContainerWidget();

private:
    Ui::HotkeySoundboard *ui;
    Soundboard soundboard;
    QWidget *soundGroupContainerWidget = nullptr;
    FlowLayout *soundGroupFlowLayout = nullptr;
};
#endif // HOTKEYSOUNDBOARD_H
