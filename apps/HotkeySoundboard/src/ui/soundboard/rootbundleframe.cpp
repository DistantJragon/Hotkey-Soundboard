#include "ui/soundboard/rootbundleframe.h"
#include "ui/soundboard/playableentryframe.h"
#include <QDropEvent>
#include <QLayoutItem>
#include <QMimeData>

RootBundleFrame::RootBundleFrame(QWidget* parent) : QFrame(parent) {
  entryLayout = new QVBoxLayout(this);
}

void RootBundleFrame::refreshChildrenDisplay(
    const std::vector<sb::PlayableEntry*>& children) {
  // Clear the layout and re-add entries
  removeDragDivider();
  while (QLayoutItem* item = entryLayout->takeAt(0)) {
    if (QWidget* itemWidget = item->widget()) {
      delete itemWidget;
    }
    delete item;
  }
  sb::EntryHandle parentHandle = sb::InvalidEntryHandle;
  if (!children.empty()) {
    parentHandle = children[0]->getParentHandle();
  }
  for (size_t i = 0; i < children.size(); ++i) {
    sb::PlayableEntry* entry = children[i];
    if (entry) {
      auto* entryFrame =
          new PlayableEntryFrame(this, entry, static_cast<int>(i));
      connect(entryFrame, &PlayableEntryFrame::deleteRequested, this,
              &RootBundleFrame::deleteRequested);
      connect(entryFrame, &PlayableEntryFrame::playRequested, this,
              &RootBundleFrame::playRequested);
      connect(entryFrame, &PlayableEntryFrame::weightChangeRequested, this,
              [this, parentHandle](int indexInParent, unsigned int weight) {
                emit weightChangeRequested(parentHandle, indexInParent, weight);
              });
      connect(entryFrame, &PlayableEntryFrame::syncWeightSumChangeRequested,
              this, &RootBundleFrame::syncWeightSumChangeRequested);
      entryLayout->addWidget(entryFrame);
    }
  }
}

void RootBundleFrame::dragEnterEvent(QDragEnterEvent* event) {

#ifndef HKSBNDEBUG
  // auto formats = event->mimeData()->formats();
  // QString mimeTypes = "Mime types:";
  // for (const auto& format : formats) {
  //   mimeTypes += "\n" + format;
  // }
  // qDebug("%s", mimeTypes.toStdString().c_str());
  // if (event->mimeData()->hasUrls()) {
  //   QString paths = "Paths:";
  //   for (const auto& url : event->mimeData()->urls()) {
  //     paths += "\n" + url.toLocalFile();
  //   }
  //   qDebug("%s", paths.toStdString().c_str());
  // }
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
    return;
  }
  if (event->mimeData()->hasFormat("application/x-HKSB-playable-entry")) {
    // Reject entries that are not in this frame
    QByteArray handleData =
        event->mimeData()->data("application/x-HKSB-playable-entry");
    // Get handle and index from the data
    if (handleData.size() != sizeof(quint32) + sizeof(qint32)) {
      qWarning("Dragged entry data has an invalid size.");
      event->ignore();
      return;
    }
    QDataStream dataStream(&handleData, QIODevice::ReadOnly);
    quint32 handle;
    qint32 index;
    dataStream >> handle;
    dataStream >> index;
    if (index < 0 || index >= entryLayout->count()) {
      qWarning("Dragged entry index is out of bounds.");
      event->ignore();
      return;
    }
    PlayableEntryFrame* entryFrame =
        static_cast<PlayableEntryFrame*>(entryLayout->itemAt(index)->widget());
    if (!entryFrame) {
      qWarning("Failed to retrieve the dragged entry widget.");
      event->ignore();
      return;
    }
    if (entryFrame->getEntry()->getHandle() != handle) {
      qWarning("Dragged entry does not belong to this frame.");
      event->ignore();
      return;
    }
    // Hiding the widget causes issues when dragging at the bottom (crying)
    // entryFrame->hide();
    grabbedIndex = index;
    event->acceptProposedAction();
    return;
  }
}

void RootBundleFrame::dragMoveEvent(QDragMoveEvent* event) {
  int index = positionToIndex(event->position().y());
  moveDragDivider(index);
  qDebug("Drag move at index: %d", index);
}

void RootBundleFrame::dragLeaveEvent(QDragLeaveEvent* event) {
  removeDragDivider();
  if (grabbedIndex >= 0 && grabbedIndex < entryLayout->count()) {
    // Disable hiding the widget, see comment in dragEnterEvent
    // QWidget* oldWidget = entryLayout->itemAt(grabbedIndex)->widget();
    // if (oldWidget) {
    //   oldWidget->show();
    // }
    grabbedIndex = -1;
  }
  event->accept();
}

void RootBundleFrame::dropEvent(QDropEvent* event) {
  int index = positionToIndex(event->position().y());
  removeDragDivider();
  grabbedIndex = -1;
  if (event->mimeData()->hasUrls()) {
    QList<QUrl> urls = event->mimeData()->urls();
    qDebug("Files dropped at index: %d", index);
    emit filesDropped(urls, index);
    event->acceptProposedAction();
  } else if (event->mimeData()->hasFormat(
                 "application/x-HKSB-playable-entry")) {
    QByteArray handleData =
        event->mimeData()->data("application/x-HKSB-playable-entry");
    QDataStream dataStream(&handleData, QIODevice::ReadOnly);
    quint32 handle;
    qint32 oldIndex;
    dataStream >> handle;
    dataStream >> oldIndex;
    if (index > oldIndex) {
      index -= 1;
    }
    if (oldIndex == index) {
      // Disable hiding the widget, see comment in dragEnterEvent
      // QWidget* oldWidget = entryLayout->itemAt(oldIndex)->widget();
      // if (oldWidget) {
      //   oldWidget->show();
      // }
      event->ignore();
      return;
    }
    event->acceptProposedAction();
    emit entryDropped(oldIndex, index);
  } else {
    event->ignore();
  }
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
  int grabbedIndexLocal = grabbedIndex;
  if (dividerIndex >= 0 && grabbedIndexLocal >= 0 &&
      grabbedIndexLocal > dividerIndex) {
    grabbedIndexLocal += 1;
  }
  for (int i = 0; i < entryLayout->count(); ++i) {
    if (i == dividerIndex || i == grabbedIndexLocal) {
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

void RootBundleFrame::moveDragDivider(int index) {
  if (!entryLayout) {
    return;
  }
  if (index == dividerIndex) {
    return;
  }
  removeDragDivider();
  if (index < 0 || index > entryLayout->count()) {
    return;
  }
  int space = entryLayout->count() ? 5 : 0;
  entryLayout->insertSpacing(index, space);
  dividerIndex = index;
}
