#include "CropHandle.h"
#include "MainWindow.h"

const int handle_size = 20;

CropHandle::CropHandle(MainWindow* owner, Qt::Alignment alignment) {
  m_owner = owner;
  m_alignment = alignment;
  setFlag(ItemIsMovable);
  setFlag(ItemSendsGeometryChanges);
  setFlag(ItemIgnoresTransformations);
  setRect(-handle_size / 2, -handle_size / 2, handle_size, handle_size);
  setPen(QPen(Qt::black));
  setBrush(Qt::white);
  setCursor(Qt::ArrowCursor);
}

QVariant CropHandle::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value) {
  if (change == ItemPositionChange && !m_owner->crop_handles_are_updating()) {
    QPointF point = value.toPointF();
    if (m_alignment & Qt::AlignVCenter) {
      point.setY(pos().y());
    }
    if (m_alignment & Qt::AlignHCenter) {
      point.setX(pos().x());
    }
    m_owner->crop_handle_moved(m_alignment, point);
    return point;
  }
  return QGraphicsRectItem::itemChange(change, value);
}
