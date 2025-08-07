#include "playableentryframe.h"
#include "ui_playableentryframe.h"

PlayableEntryFrame::PlayableEntryFrame(QWidget* parent)
    : QFrame(parent), ui(new Ui::PlayableEntryFrame) {
  ui->setupUi(this);
}

PlayableEntryFrame::~PlayableEntryFrame() { delete ui; }
