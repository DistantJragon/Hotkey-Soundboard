#include "ui/soundgroupwidget.h"
#include "ui_soundgroupwidget.h"

SoundGroupWidget::SoundGroupWidget(QWidget* parent, sb::GroupHandle soundGroup)
    : QFrame(parent), ui(new Ui::SoundGroupWidget), soundGroup{soundGroup} {
  ui->setupUi(this);

  connect(ui->nameLabel, &ClickableLabel::doubleClicked, this,
          [this]() { emit renameRequested(this->soundGroup); });

  playableEntryLayout = new QVBoxLayout(ui->playableEntryFrame);
  ui->playableEntryFrame->setLayout(playableEntryLayout);
  hotkeyLayout = new QVBoxLayout(ui->hotkeyFrame);
  ui->hotkeyFrame->setLayout(hotkeyLayout);
}

SoundGroupWidget::~SoundGroupWidget() { delete ui; }

void SoundGroupWidget::refreshSoundGroupDisplay(const sb::SoundGroup& group) {
  ui->nameLabel->setText(QString::fromStdString(group.getName()));
  ui->randomPlayCheckBox->setChecked(group.getBundle().isRandomPlay());
  this->updateGeometry();
}
