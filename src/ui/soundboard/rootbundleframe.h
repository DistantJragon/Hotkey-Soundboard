#ifndef ROOTBUNDLEFRAME_H
#define ROOTBUNDLEFRAME_H

#include "core/soundboard/playableentry.h"
#include <QDragEnterEvent>
#include <QFrame>
#include <QUrl>
#include <QVBoxLayout>

class RootBundleFrame : public QFrame {
  Q_OBJECT
public:
  RootBundleFrame(QWidget* parent = nullptr);

  void refreshChildrenDisplay(const std::vector<sb::PlayableEntry*>& children);

signals:
  void deleteRequested(sb::EntryHandle entry);
  void entryDropped(int oldIndex, int newIndex);
  void filesDropped(const QList<QUrl>& urls, int index);
  void playRequested(sb::EntryHandle entry);
  void syncWeightSumChangeRequested(sb::EntryHandle entry, bool sync);
  void weightChangeRequested(sb::EntryHandle parent, int index,
                             unsigned int weight);

protected:
  void dragEnterEvent(QDragEnterEvent* event) override;
  void dragLeaveEvent(QDragLeaveEvent* event) override;
  void dragMoveEvent(QDragMoveEvent* event) override;
  void dropEvent(QDropEvent* event) override;

private:
  QVBoxLayout* entryLayout = nullptr;
  int dividerIndex = -1;
  int grabbedIndex = -1;

  /*!
   * \brief Converts a vertical position to an index in the layout.
   * The index corresponds to the top of the widget at that position or the
   * bottom of the final widget.
   * \param yPos The vertical position in pixels.
   * \return The index of the widget at that position, or -1 if the layout is
   * not set.
   */

  int positionToIndex(int yPos) const;

  /*!
   * \brief Moves the drag divider to a new index in the layout.
   * Indexing does not include the drag divider itself if it exists.
   * \param index The new index for the drag divider.
   */
  void moveDragDivider(int index);

  /*!
   * \brief Removes the drag divider from the layout.
   */
  void removeDragDivider();
};

#endif // ROOTBUNDLEFRAME_H
