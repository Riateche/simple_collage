#ifndef CROPFORM_H
#define CROPFORM_H

#include <QGraphicsScene>
#include <QWidget>
#include <QGraphicsPixmapItem>

namespace Ui {
class CropForm;
}

class CropForm : public QWidget
{
  Q_OBJECT
public:
  explicit CropForm(QPixmap pixmap, QWidget *parent = 0);
  ~CropForm();
  QGraphicsScene m_scene;
  QGraphicsPixmapItem * m_pixmap;
  QRect m_rect;

private:
  Ui::CropForm *ui;
};

#endif // CROPFORM_H
