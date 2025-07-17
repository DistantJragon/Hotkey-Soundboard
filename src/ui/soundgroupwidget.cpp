#include "ui/soundgroupwidget.h"
#include "ui_soundgroupwidget.h"

SoundGroupWidget::SoundGroupWidget(QWidget* parent, SoundGroup* soundGroup)
    : QFrame(parent), ui(new Ui::SoundGroupWidget), soundGroup{soundGroup} {
  ui->setupUi(this);

  connect(ui->nameLabel, &ClickableLabel::doubleClicked, this,
          [this]() { emit renameRequested(this->soundGroup); });

  playableEntryLayout = new QVBoxLayout(ui->playableEntryFrame);
  ui->playableEntryFrame->setLayout(playableEntryLayout);
  hotkeyLayout = new QVBoxLayout(ui->hotkeyFrame);
  ui->hotkeyFrame->setLayout(hotkeyLayout);

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

void SoundGroupWidget::updateNameLabel() {
  ui->nameLabel->setText(QString::fromStdString(soundGroup->getName()));
  this->updateGeometry();
}
