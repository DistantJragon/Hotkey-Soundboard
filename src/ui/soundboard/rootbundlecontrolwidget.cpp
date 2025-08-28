#include "ui/soundboard/rootbundlecontrolwidget.h"
#include "ui_rootbundlecontrolwidget.h"
#include <QMessageBox>

RootBundleControlWidget::RootBundleControlWidget(QWidget* parent,
                                                 sb::EntryHandle entry)
    : QFrame(parent), ui(new Ui::RootBundleControlWidget), entry{entry} {
  ui->setupUi(this);

  connect(ui->nameLabel, &ClickableLabel::doubleClicked, this,
          [this]() { emit renameRequested(this->entry); });
  connect(ui->hideButton, &QPushButton::clicked, this,
          [this]() { emit hideRequested(this->entry); });
  connect(ui->deleteRootBundleButton, &QPushButton::clicked, this,
          &RootBundleControlWidget::confirmDelete);
  connect(ui->randomPlayCheckBox, &QCheckBox::checkStateChanged, this,
          [this](int state) {
            emit changeRandomPlayRequested(this->entry, state == Qt::Checked);
          });
  connect(ui->rootBundleFrame, &RootBundleFrame::filesDropped, this,
          [this](const QList<QUrl>& urls, int index) {
            emit filesDropped(this->entry, urls, index);
          });
  connect(ui->rootBundleFrame, &RootBundleFrame::entryDropped, this,
          [this](int oldIndex, int newIndex) {
            emit entryDropped(this->entry, oldIndex, newIndex);
          });
  connect(ui->rootBundleFrame, &RootBundleFrame::playRequested, this,
          &RootBundleControlWidget::playRequested);
  connect(ui->rootBundleFrame, &RootBundleFrame::weightChangeRequested, this,
          &RootBundleControlWidget::weightChangeRequested);
  connect(ui->rootBundleFrame, &RootBundleFrame::deleteRequested, this,
          &RootBundleControlWidget::deleteRequested);
}

RootBundleControlWidget::~RootBundleControlWidget() { delete ui; }

void RootBundleControlWidget::refreshRootBundleDisplay(
    const sb::BundleEntry& entry) {
  ui->nameLabel->setText(QString::fromStdString(entry.getName()));
  ui->randomPlayCheckBox->setChecked(entry.isRandomPlay());
  ui->rootBundleFrame->refreshChildrenDisplay(entry.getChildren());
  this->updateGeometry();
}

void RootBundleControlWidget::confirmDelete() {
  QMessageBox::StandardButton reply;
  reply = QMessageBox::question(
      this, "Confirm Delete",
      "Are you sure you want to delete this bundle and all its contents?",
      QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
  if (reply == QMessageBox::Yes) {
    emit deleteRequested(this->entry);
  }
}
