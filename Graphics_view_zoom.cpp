#include "Graphics_view_zoom.h"
#include <QMouseEvent>
#include <QDebug>
#include <QApplication>
#include <QScrollBar>

Graphics_view_zoom::Graphics_view_zoom(QGraphicsView* view)
  : QObject(view), _view(view)
{
  _view->viewport()->installEventFilter(this);
  animation = 0;
  animation_previous_value = 0;
  animation_final_value = 0;
  animation_in_progress = false;
}

void Graphics_view_zoom::gentle_zoom(double factor) {
  _view->scale(factor, factor);
  _view->centerOn(target_scene_pos);
  QPointF delta_viewport_pos = target_viewport_pos - QPointF(_view->viewport()->width() / 2.0,
                                                             _view->viewport()->height() / 2.0);
  QPointF viewport_center = _view->mapFromScene(target_scene_pos) - delta_viewport_pos;
  _view->centerOn(_view->mapToScene(viewport_center.toPoint()));
  emit zoomed();
}

void Graphics_view_zoom::animate_zoom(double factor) {
  if (animation) {
    gentle_zoom(animation_final_value / animation_previous_value);
    delete animation;
  }
  animation_previous_value = 1.0;
  animation_final_value = factor;
  animation = new QVariantAnimation();
  animation->setStartValue(1.0);
  animation->setEndValue(factor);
  animation->setDuration(300);
  connect(animation, &QVariantAnimation::valueChanged,
          this, &Graphics_view_zoom::animate);
  animation->start(QVariantAnimation::DeleteWhenStopped);
  connect(animation, &QVariantAnimation::destroyed,
          this, &Graphics_view_zoom::animation_stopped);
}

bool Graphics_view_zoom::eventFilter(QObject *object, QEvent *event) {
  if (event->type() == QEvent::MouseMove) {
    QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);
    QPointF delta = target_viewport_pos - mouse_event->pos();
    if (qAbs(delta.x()) > 5 || qAbs(delta.y()) > 5) {
      target_viewport_pos = mouse_event->pos();
      target_scene_pos = _view->mapToScene(mouse_event->pos());
    }
  } else if (event->type() == QEvent::Wheel) {
    QWheelEvent* wheel_event = static_cast<QWheelEvent*>(event);
    if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
      if (wheel_event->orientation() == Qt::Vertical) {
        int delta = wheel_event->delta() / 100;
        double factor = 1.0;
        while(delta != 0) {
          if (delta > 0) {
            delta--;
            factor *= 1.2;
          } else {
            delta++;
            factor /= 1.2;
          }
        }
        gentle_zoom(factor);
        return true;
      }
    }
  }
  return false;
}

void Graphics_view_zoom::animate(const QVariant &value) {
  double factor = value.toDouble();
  if (!factor) {
    return;
  }
  gentle_zoom(factor / animation_previous_value);
  animation_previous_value = factor;
}

void Graphics_view_zoom::animation_stopped() {
  animation = 0;
}
