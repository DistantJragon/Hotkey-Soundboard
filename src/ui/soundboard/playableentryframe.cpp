#include "ui/soundboard/playableentryframe.h"
#include "ui_playableentryframe.h"
#include <QDrag>
#include <QMessageBox>
#include <QMimeData>
#include <QMouseEvent>
#include <QPainter>

PlayableEntryFrame::PlayableEntryFrame(QWidget* parent,
                                       sb::PlayableEntry* entry,
                                       int indexInParent)
    : QFrame(parent), ui(new Ui::PlayableEntryFrame), entry(entry),
      indexInParent(indexInParent) {
  ui->setupUi(this);
  if (entry) {
    ui->nameLabel->setText(QString::fromStdString(entry->getName()));
    ui->weightSpinBox->setValue(entry->getWeight());
#ifndef HKSBNDEBUG
    connect(ui->entryButton, &QPushButton::clicked, this,
            [this, entryHandle = entry->getHandle()]() {
              emit playRequested(entryHandle);
            });
#endif // HKSBNDEBUG
    connect(ui->weightSpinBox, &QSpinBox::valueChanged, this,
            [this](int value) {
              emit weightChangeRequested(this->indexInParent,
                                         static_cast<unsigned int>(value));
            });
    connect(ui->deleteButton, &QPushButton::clicked, this,
            &PlayableEntryFrame::confirmDelete);
    switch (entry->type) {
    case sb::PlayableEntry::Type::Bundle:
      ui->entryButton->setText("B");
      break;
    case sb::PlayableEntry::Type::SoundFile:
      ui->entryButton->setText("F");
      break;
    case sb::PlayableEntry::Type::String:
      ui->entryButton->setText("S");
      break;
    }
  }
}

PlayableEntryFrame::~PlayableEntryFrame() { delete ui; }

void PlayableEntryFrame::mousePressEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    dragStartPosition = event->pos();
  }
}

void PlayableEntryFrame::mouseMoveEvent(QMouseEvent* event) {
  if (!(event->buttons() & Qt::LeftButton)) {
    return;
  }
  if ((event->pos() - dragStartPosition).manhattanLength() <
      QApplication::startDragDistance()) {
    return;
  }
  QDrag* drag = new QDrag(this);
  QMimeData* mimeData = new QMimeData;

  // Fit the handle and index into the mime data
  QByteArray data;
  QDataStream dataStream(&data, QIODevice::WriteOnly);
  dataStream << static_cast<quint32>(entry->getHandle());
  dataStream << static_cast<qint32>(indexInParent);

  mimeData->setData("application/x-HKSB-playable-entry", data);
  drag->setMimeData(mimeData);

  // The widget has no background by default
  // Temporarily set background to default color for drag pixmap
  QPixmap widgetPixmap = this->grab();
  QPixmap finalPixmap(widgetPixmap.size());
  finalPixmap.fill(Qt::transparent);
  QPainter painter(&finalPixmap);
  painter.setOpacity(0.8);
  painter.fillRect(finalPixmap.rect(), this->palette().window());
  painter.drawPixmap(0, 0, widgetPixmap);
  painter.end();
  drag->setPixmap(finalPixmap);
  drag->setHotSpot(event->pos());

  // Start the drag operation
  drag->exec(Qt::MoveAction);
}

void PlayableEntryFrame::confirmDelete() {
  QMessageBox::StandardButton reply;
  reply = QMessageBox::question(
      this, "Confirm Delete",
      "Are you sure you want to delete this entry from the bundle?",
      QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
  if (reply == QMessageBox::Yes) {
    emit deleteRequested(this->entry->getHandle());
  }
}
