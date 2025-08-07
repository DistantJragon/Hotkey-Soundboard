#include "ui/soundgroupwidget.h"
#include "ui_soundgroupwidget.h"

SoundGroupWidget::SoundGroupWidget(QWidget* parent, sb::GroupHandle soundGroup)
    : QFrame(parent), ui(new Ui::SoundGroupWidget), soundGroup{soundGroup} {
  ui->setupUi(this);

  connect(ui->nameLabel, &ClickableLabel::doubleClicked, this,
          [this]() { emit renameRequested(this->soundGroup); });

  hotkeyLayout = new QVBoxLayout(ui->hotkeyFrame);
}

SoundGroupWidget::~SoundGroupWidget() { delete ui; }

void SoundGroupWidget::refreshSoundGroupDisplay(const sb::SoundGroup& group) {
  ui->nameLabel->setText(QString::fromStdString(group.getName()));
  ui->randomPlayCheckBox->setChecked(group.getBundle().isRandomPlay());
  this->updateGeometry();
}
