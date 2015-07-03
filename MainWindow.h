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



class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();


  static const int KEY_FILENAME = 1, KEY_CROP = 2;

private:
  Ui::MainWindow *ui;
  QGraphicsScene m_scene;
  QString current_filename;

  void applyTransform(const QTransform& transform);

  QGraphicsPixmapItem *add_image(const QString& filename);

  QGraphicsPixmapItem * m_cropTarget;
  QGraphicsRectItem * m_cropDisplay;

  void open_project(const QString& filename);
  void save_project(const QString& filename);

  void remember_current(const QString& filename);


  // QWidget interface
protected:
  void dragEnterEvent(QDragEnterEvent* event);
  void dropEvent(QDropEvent* event);
  void closeEvent(QCloseEvent* event);
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

protected:
  void mousePressEvent(QMouseEvent *);

  // QObject interface
public:
  bool eventFilter(QObject *object, QEvent *event);
};

#endif // MAINWINDOW_H
