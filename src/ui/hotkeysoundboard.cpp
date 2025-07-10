#include "src/ui/hotkeysoundboard.h"
#include "ui_hotkeysoundboard.h"

HotkeySoundboard::HotkeySoundboard(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::HotkeySoundboard) {
  ui->setupUi(this);
  setupSoundGroupContainerWidget();
}

HotkeySoundboard::~HotkeySoundboard() { delete ui; }

void HotkeySoundboard::setupSoundGroupContainerWidget() {
  soundGroupContainerWidget = new QWidget;
  soundGroupFlowLayout = new FlowLayout(soundGroupContainerWidget);
  soundGroupContainerWidget->setLayout(soundGroupFlowLayout);

  ui->scrollArea->setWidget(soundGroupContainerWidget);
  ui->scrollArea->setWidgetResizable(true);
}
