#ifndef CROPHANDLE_H
#define CROPHANDLE_H

#include <QGraphicsRectItem>

class MainWindow;

class CropHandle : public QGraphicsRectItem {
public:
  CropHandle(MainWindow* owner, Qt::Alignment alignment);
  Qt::Alignment alignment() { return m_alignment; }

private:
  MainWindow* m_owner;
  Qt::Alignment m_alignment;

protected:
  QVariant itemChange(GraphicsItemChange change, const QVariant& value);
};

#endif // CROPHANDLE_H
