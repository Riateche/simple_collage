#include "Resizable_rubber_band.h"

#include <QHBoxLayout>
#include <QSizeGrip>

Resizable_rubber_band::Resizable_rubber_band(QWidget *parent) : QWidget(parent) {
  //tell QSizeGrip to resize this widget instead of top-level window
  setWindowFlags(Qt::SubWindow);
  QHBoxLayout* layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  QSizeGrip* grip1 = new QSizeGrip(this);
  QSizeGrip* grip2 = new QSizeGrip(this);
  layout->addWidget(grip1, 0, Qt::AlignLeft | Qt::AlignTop);
  layout->addWidget(grip2, 0, Qt::AlignRight | Qt::AlignBottom);
  rubberband = new QRubberBand(QRubberBand::Rectangle, this);
  rubberband->move(0, 0);
  rubberband->show();

  grip1->hide();
}

void Resizable_rubber_band::resizeEvent(QResizeEvent *) {
  rubberband->resize(size());
}
