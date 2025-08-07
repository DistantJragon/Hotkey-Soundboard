#include "rootbundleframe.h"
#include <QDragEnterEvent>
#include <QMimeData>

#ifndef HKSBNDEBUG
#include "ui/playableentryframe.h"
#endif

RootBundleFrame::RootBundleFrame(QWidget* parent) : QFrame(parent) {
  entryLayout = new QVBoxLayout(this);
#ifndef HKSBNDEBUG
  for (int i = 0; i < 3; ++i)
    entryLayout->addWidget(new PlayableEntryFrame(this));
#endif // HKSBNDEBUG
}

void RootBundleFrame::dragEnterEvent(QDragEnterEvent* event) {

#ifndef HKSBNDEBUG
  auto formats = event->mimeData()->formats();
  QString mimeTypes = "Mime types:";
  for (const auto& format : formats) {
    mimeTypes += "\n" + format;
  }
  qDebug("%s", mimeTypes.toStdString().c_str());
  if (event->mimeData()->hasUrls()) {
    QString paths = "Paths:";
    for (const auto& url : event->mimeData()->urls()) {
      paths += "\n" + url.toLocalFile();
    }
    qDebug("%s", paths.toStdString().c_str());
  }
#endif // HKSBNDEBUG

  // TODO: Check if this works on non-Windows platforms
  if (event->mimeData()->hasUrls()) {
    for (const auto& url : event->mimeData()->urls()) {
      if (!url.isLocalFile()) {
        qWarning("Only local files are supported for drag and drop.");
        event->ignore();
        return;
      }
    }
    event->acceptProposedAction();
  }
}

void RootBundleFrame::dragMoveEvent(QDragMoveEvent* event) {
  int index = positionToIndex(event->position().y());
  if (dividerIndex != index) {
    if (dividerIndex >= 0) {
      QLayoutItem* dividerItem = entryLayout->takeAt(dividerIndex);
      entryLayout->insertItem(index, dividerItem);
    } else {
      int space = entryLayout->count() ? 5 : 0;
      entryLayout->insertSpacing(index, space);
    }
    dividerIndex = index;
  }
}

void RootBundleFrame::dragLeaveEvent(QDragLeaveEvent* event) {
  removeDragDivider();
  event->accept();
}

void RootBundleFrame::dropEvent(QDropEvent* event) {
  if (event->mimeData()->hasUrls()) {
    QList<QUrl> urls = event->mimeData()->urls();
    int index = positionToIndex(event->position().y());
    qDebug("Files dropped at index: %d", index);
    emit filesDropped(urls, index);
    event->acceptProposedAction();
  } else {
    event->ignore();
  }
  removeDragDivider();
}
int RootBundleFrame::positionToIndex(int yPos) const {
  if (!entryLayout) {
    return -1;
  }
  if (entryLayout->count() == 0) {
    return 0;
  }
  // TODO: Could be optimized by first guessing an index based on
  // relYPos/height*count
  int index = 0;
  QRect firstItemRect = entryLayout->itemAt(0)->geometry();
  int leastDistance = std::abs(yPos - firstItemRect.top());
  for (int i = 0; i < entryLayout->count(); ++i) {
    if (i == dividerIndex) {
      continue;
    }
    QRect itemRect = entryLayout->itemAt(i)->geometry();
    int distance = std::abs(yPos - itemRect.bottom());
    if (distance < leastDistance) {
      leastDistance = distance;
      index = i + 1;
    } else {
      break;
    }
  }
  if (dividerIndex >= 0 && index > dividerIndex) {
    index -= 1;
  }
  return index;
}
void RootBundleFrame::removeDragDivider() {
  if (dividerIndex >= 0) {
    QLayoutItem* dividerItem = entryLayout->takeAt(dividerIndex);
    delete dividerItem;
    dividerIndex = -1;
  }
}
