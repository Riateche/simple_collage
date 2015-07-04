#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>


namespace Ui {
class MainWindow;
}



/*
 * crop
 * mouse hotkeys
 * export to png
 * boundary fix
 * reset all
 * clear selection
 * paste from clipboard
 * undo
 *
 */

class CropHandle;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();


  static const int KEY_FILENAME = 1, KEY_CROP = 2;

  void crop_handle_moved(Qt::Alignment alignment, QPointF pos);
  bool crop_handles_are_updating() { return m_crop_handles_are_updating; }

private:
  Ui::MainWindow* ui;
  QGraphicsScene m_scene;
  QString m_current_filename;

  void applyTransform(const QTransform& transform);

  QGraphicsPixmapItem* add_image(const QString& filename, QRect crop = QRect());

  QGraphicsPixmapItem* m_crop_target;
  QGraphicsRectItem* m_crop_display;
  QList<CropHandle*> m_crop_handles;
  bool m_crop_handles_are_updating;

  void open_project(const QString& filename);
  void save_project(const QString& filename);

  void remember_current(const QString& filename);
  void update_crop_handles();

  void update_pixmap(QGraphicsPixmapItem* item);



protected:
  void dragEnterEvent(QDragEnterEvent* event);
  void dropEvent(QDropEvent* event);
  void closeEvent(QCloseEvent* event);
  void mousePressEvent(QMouseEvent *);

private slots:
  void on_zoom2_clicked();
  void on_zoom_clicked();
  void on_rotate2_clicked();
  void on_rotate_clicked();
  void on_reflect_x_clicked();
  void on_reflect_y_clicked();
  void delete_selected();
  void on_reset_clicked();
  void on_crop_clicked();
  void on_cancel_crop_clicked();
  void on_apply_crop_clicked();
  void on_action_open_triggered();

  void on_action_save_triggered();

  void on_action_save_as_triggered();

  void on_action_add_image_triggered();

};

#endif // MAINWINDOW_H
