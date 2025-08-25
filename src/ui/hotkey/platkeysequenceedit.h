#ifndef PLATKEYSEQUENCEEDIT_H
#define PLATKEYSEQUENCEEDIT_H

#include <QKeySequenceEdit>

class PlatKeySequenceEdit : public QKeySequenceEdit {
  Q_OBJECT
public:
  PlatKeySequenceEdit(QWidget* parent = nullptr);

  quint32 nativeModifiers() const { return nativeModifiers_; }
  quint32 nativeScancode() const { return nativeScancode_; }
  quint32 nativeVirtualKey() const { return nativeVirtualKey_; }

public slots:
  void clear();

protected:
  void keyPressEvent(QKeyEvent* event) override;

private slots:
  void clearNativeData();

private:
  quint32 nativeModifiers_ = 0;
  quint32 nativeScancode_ = 0;
  quint32 nativeVirtualKey_ = 0;
};

#endif // PLATKEYSEQUENCEEDIT_H
