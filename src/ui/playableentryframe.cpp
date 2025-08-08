#include "playableentryframe.h"
#include "ui_playableentryframe.h"

PlayableEntryFrame::PlayableEntryFrame(QWidget* parent,
                                       std::unique_ptr<sb::PlayableEntry> entry)
    : QFrame(parent), ui(new Ui::PlayableEntryFrame) {
  ui->setupUi(this);
  if (entry) {
    ui->nameLabel->setText(QString::fromStdString(entry->getName()));
    ui->weightSpinBox->setValue(entry->getWeight());
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
