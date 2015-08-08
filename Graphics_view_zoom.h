#ifndef GRAPHICS_VIEW_ZOOM_H
#define GRAPHICS_VIEW_ZOOM_H

#include <QObject>
#include <QGraphicsView>
#include <QSequentialAnimationGroup>
#include <QVariantAnimation>

/*!
 * Этот класс добавляет в QGraphicsView возможность масштабирования мышью по ctrl+прокрутка.
 * При этом точка под курсором остается по возможности неподвижной. Корректно обрабатывается
 * случай, когда из-за маленького размера изображения нельзя отскроллить точку под курсор.
 *
 * При изменении масштаба мышью с помощью этого класса посылается сигнал zoomed.
 *
 * Использование:
 *
 *   new Graphics_view_zoom(view);
 *
 * Объект удаляется автоматически вместе с QGraphicsView.
 */
class Graphics_view_zoom : public QObject {
  Q_OBJECT
public:
  Graphics_view_zoom(QGraphicsView* view);
  void gentle_zoom(double factor);
  void animate_zoom(double factor);

private:
  QGraphicsView* _view;
  QPointF target_scene_pos, target_viewport_pos;
  bool eventFilter(QObject* object, QEvent* event);

  double animation_previous_value;
  double animation_final_value;
  bool animation_in_progress;
  QVariantAnimation* animation;

private slots:
  void animate(const QVariant& value);
  void animation_stopped();

signals:
  void zoomed();
};

#endif // GRAPHICS_VIEW_ZOOM_H
