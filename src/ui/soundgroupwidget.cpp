#include "ui/soundgroupwidget.h"
#include "ui_soundgroupwidget.h"

SoundGroupWidget::SoundGroupWidget(QWidget* parent, SoundGroup* soundGroup)
    : QWidget(parent), ui(new Ui::SoundGroupWidget), soundGroup{soundGroup} {
  ui->setupUi(this);
  refreshSoundGroupDisplay();
}

SoundGroupWidget::~SoundGroupWidget() { delete ui; }

void SoundGroupWidget::refreshSoundGroupDisplay() {
  if (!soundGroup) {
    return;
  }
  ui->nameLabel->setText(QString::fromStdString(soundGroup->getName()));
  ui->randomPlayCheckBox->setChecked(soundGroup->isRandomPlay());
  this->updateGeometry();
}
