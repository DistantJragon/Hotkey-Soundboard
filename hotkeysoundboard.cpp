#include "hotkeysoundboard.h"
#include "ui_hotkeysoundboard.h"

HotkeySoundboard::HotkeySoundboard(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HotkeySoundboard)
{
    ui->setupUi(this);
}

HotkeySoundboard::~HotkeySoundboard()
{
    delete ui;
}
