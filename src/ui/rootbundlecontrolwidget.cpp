#include "ui/rootbundlecontrolwidget.h"
#include "ui_rootbundlecontrolwidget.h"

RootBundleControlWidget::RootBundleControlWidget(QWidget* parent,
                                                 sb::EntryHandle entry)
    : QFrame(parent), ui(new Ui::RootBundleControlWidget), entry{entry} {
  ui->setupUi(this);

  connect(ui->nameLabel, &ClickableLabel::doubleClicked, this,
          [this]() { emit renameRequested(this->entry); });

  hotkeyLayout = new QVBoxLayout(ui->hotkeyFrame);
}

RootBundleControlWidget::~RootBundleControlWidget() { delete ui; }

void RootBundleControlWidget::refreshRootBundleDisplay(
    const sb::BundleEntry& entry) {
  ui->nameLabel->setText(QString::fromStdString(entry.getName()));
  ui->randomPlayCheckBox->setChecked(entry.isRandomPlay());
  this->updateGeometry();
}
