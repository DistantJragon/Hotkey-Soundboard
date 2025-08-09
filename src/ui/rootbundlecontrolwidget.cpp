#include "ui/rootbundlecontrolwidget.h"
#include "ui_rootbundlecontrolwidget.h"

RootBundleControlWidget::RootBundleControlWidget(QWidget* parent,
                                                 sb::EntryHandle entry)
    : QFrame(parent), ui(new Ui::RootBundleControlWidget), entry{entry} {
  ui->setupUi(this);

  connect(ui->nameLabel, &ClickableLabel::doubleClicked, this,
          [this]() { emit renameRequested(this->entry); });
  connect(ui->rootBundleFrame, &RootBundleFrame::filesDropped, this,
          [this](const QList<QUrl>& urls, int index) {
            emit filesDropped(this->entry, urls, index);
          });
  connect(ui->rootBundleFrame, &RootBundleFrame::playRequested, this,
          &RootBundleControlWidget::playRequested);

  hotkeyLayout = new QVBoxLayout(ui->hotkeyFrame);
}

RootBundleControlWidget::~RootBundleControlWidget() { delete ui; }

void RootBundleControlWidget::refreshRootBundleDisplay(
    const sb::BundleEntry& entry) {
  ui->nameLabel->setText(QString::fromStdString(entry.getName()));
  ui->randomPlayCheckBox->setChecked(entry.isRandomPlay());
  ui->rootBundleFrame->refreshChildrenDisplay(entry.getChildren());
  this->updateGeometry();
}
