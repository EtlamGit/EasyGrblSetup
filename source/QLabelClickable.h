#ifndef QLABELCLICKABLE_H
#define QLABELCLICKABLE_H

#include <QLabel>
#include <QMouseEvent>

class QLabelClickable : public QLabel
{
  Q_OBJECT
public:
  QLabelClickable( QWidget * parent = 0 ) : QLabel(parent) {}
  ~QLabelClickable() {}

signals:
  void clicked();

protected:
  void mousePressEvent ( QMouseEvent * event )
  {
    if (event->buttons() & Qt::LeftButton) {
      emit clicked();
    }
  }
};

#endif // QLABELCLICKABLE_H
