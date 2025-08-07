#ifndef ROOTBUNDLEFRAME_H
#define ROOTBUNDLEFRAME_H

#include <QFrame>
#include <QUrl>
#include <QVBoxLayout>

class RootBundleFrame : public QFrame {
  Q_OBJECT
public:
  RootBundleFrame(QWidget* parent = nullptr);

signals:
  void filesDropped(const QList<QUrl>& urls, int index);

protected:
  void dragEnterEvent(QDragEnterEvent* event) override;
  void dragLeaveEvent(QDragLeaveEvent* event) override;
  void dragMoveEvent(QDragMoveEvent* event) override;
  void dropEvent(QDropEvent* event) override;

private:
  QVBoxLayout* entryLayout = nullptr;
  int dividerIndex = -1;

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
   * \brief Removes the drag divider from the layout.
   */
  void removeDragDivider();
};

#endif // ROOTBUNDLEFRAME_H
