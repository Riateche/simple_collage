#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDebug>
#include <QShortcut>
#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>
#include "CropHandle.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  connect(ui->save, SIGNAL(clicked()), this, SLOT(on_action_save_triggered()));
  ui->graphicsView->setScene(&m_scene);
  ui->crop_notice->hide();
  setAcceptDrops(true);
  new QShortcut(QKeySequence::Delete, this, SLOT(delete_selected()));
  m_crop_target = 0;
  m_crop_display = 0;
  m_crop_handles_are_updating = false;

  QSettings settings;
  restoreGeometry(settings.value("geometry").toByteArray());
  QStringList args = qApp->arguments();
  if (args.count() > 1) {
    open_project(args[1]);
  } else {
    QString path = settings.value("last_project").toString();
    if (!path.isEmpty() && QFile::exists(path)) {
      open_project(path);
    }
  }
}

MainWindow::~MainWindow() {
  on_cancel_crop_clicked();
  delete ui;
}

void MainWindow::crop_handle_moved(Qt::Alignment alignment, QPointF pos) {
  QRectF rect = m_crop_display->rect();
  if (alignment & Qt::AlignLeft) {
    rect.setLeft(pos.x());
  }
  if (alignment & Qt::AlignRight) {
    rect.setRight(pos.x());
  }
  if (alignment & Qt::AlignTop) {
    rect.setTop(pos.y());
  }
  if (alignment & Qt::AlignBottom) {
    rect.setBottom(pos.y());
  }
  qDebug() << "new rect" << rect;
  m_crop_display->setRect(rect);
  update_crop_handles();
}

void MainWindow::applyTransform(const QTransform &transform) {
  if (m_scene.selectedItems().isEmpty()) {
    QTransform t = ui->graphicsView->transform();
    t *= transform;
    ui->graphicsView->setTransform(t);
  } else {
    foreach(QGraphicsItem * item, m_scene.selectedItems()) {
      QTransform t = item->transform();
      t *= transform;
      item->setTransform(t);

    }
  }
}

QGraphicsPixmapItem *MainWindow::add_image(const QString &filename, QRect crop) {
  QGraphicsPixmapItem* item = m_scene.addPixmap(QPixmap());
  item->setData(KEY_FILENAME, filename);
  item->setData(KEY_CROP, crop);
  item->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
  item->setTransformationMode(Qt::SmoothTransformation);
  update_pixmap(item);
  return item;
}

void MainWindow::open_project(const QString& filename) {
  m_scene.clear();
  QSettings settings(filename, QSettings::IniFormat);
  ui->graphicsView->setTransform(settings.value("transform").value<QTransform>());
  QVariantList items = settings.value("items").toList();
  foreach(QVariant itemData, items) {
    QGraphicsPixmapItem *item = add_image(
          itemData.toMap()["filename"].toString(),
          itemData.toMap()["crop"].toRect());
    item->setTransform(itemData.toMap()["transform"].value<QTransform>());
    item->setPos(itemData.toMap()["pos"].toPointF());
  }
  remember_current(filename);
}

void MainWindow::save_project(const QString& filename) {
  QSettings settings(filename, QSettings::IniFormat);
  QVariantList list;
  foreach(QGraphicsItem *item, m_scene.items(Qt::AscendingOrder)) {
    if (QGraphicsPixmapItem* pixmap_item = dynamic_cast<QGraphicsPixmapItem*>(item)) {
      if (!pixmap_item->pixmap().isNull()) {
        QVariantMap v;
        v["filename"] = item->data(KEY_FILENAME);
        v["crop"] = item->data(KEY_CROP);
        v["transform"] = QVariant::fromValue(item->transform());
        v["pos"] = item->pos();
        list << v;
      }
    }
  }
  settings.setValue("items", list);
  settings.setValue("transform", QVariant::fromValue(ui->graphicsView->transform()));
  remember_current(filename);
}

void MainWindow::remember_current(const QString& filename) {
  m_current_filename = filename;
  QSettings s;
  s.setValue("last_project", m_current_filename);
  s.setValue("geometry", saveGeometry());
}

void MainWindow::update_crop_handles() {
  qDebug() << "update handles";
  m_crop_handles_are_updating = true;
  QRectF rect = m_crop_display->rect();
  foreach(CropHandle* handle, m_crop_handles) {
    QPointF pos = handle->pos();
    if (handle->alignment() & Qt::AlignLeft) {
      pos.setX(rect.left());
    }
    if (handle->alignment() & Qt::AlignRight) {
      pos.setX(rect.right());
    }
    if (handle->alignment() & Qt::AlignTop) {
      pos.setY(rect.top());
    }
    if (handle->alignment() & Qt::AlignBottom) {
      pos.setY(rect.bottom());
    }
    if (handle->alignment() & Qt::AlignVCenter) {
      pos.setY(rect.center().y());
    }
    if (handle->alignment() & Qt::AlignHCenter) {
      pos.setX(rect.center().x());
    }
    handle->setPos(pos);
  }
  m_crop_handles_are_updating = false;
}

void MainWindow::update_pixmap(QGraphicsPixmapItem* item) {
  QString path = item->data(KEY_FILENAME).toString();
  QPixmap pixmap(path);
  if (pixmap.isNull()) {
    QMessageBox::critical(this, QString(), tr("Failed to load image file: '%1'").arg(path));
  }
  item->setPixmap(pixmap.copy(item->data(KEY_CROP).toRect()));
  item->setOffset(-item->pixmap().rect().center());
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
  if (event->mimeData()->hasUrls()) {
    event->acceptProposedAction();
  }
}

void MainWindow::dropEvent(QDropEvent *event) {
  foreach(QUrl url, event->mimeData()->urls()) {
    QGraphicsPixmapItem *item = add_image(url.toLocalFile());
    item->setPos(ui->graphicsView->mapToScene(ui->graphicsView->viewport()->mapFrom(this, event->pos())));
  }
}

void MainWindow::closeEvent(QCloseEvent* event) {
  if (QMessageBox::question(this, QString(), tr("Quit?"), QMessageBox::Yes, QMessageBox::No) != QMessageBox::Yes) {
    event->ignore();
  } else {
    event->accept();
  }
}

void MainWindow::on_rotate_clicked() {
  QTransform t;
  t.rotate(5);
  applyTransform(t);
}

void MainWindow::on_rotate2_clicked() {
  QTransform t;
  t.rotate(-5);
  applyTransform(t);
}

void MainWindow::on_zoom_clicked() {
  QTransform t;
  t.scale(1.1, 1.1);
  applyTransform(t);
}

void MainWindow::on_zoom2_clicked() {
  QTransform t;
  t.scale(0.9, 0.9);
  applyTransform(t);
}

void MainWindow::on_reflect_x_clicked() {
  QTransform t;
  t.scale(-1, 1);
  applyTransform(t);
}

void MainWindow::on_reflect_y_clicked() {
  QTransform t;
  t.scale(1, -1);
  applyTransform(t);
}

void MainWindow::delete_selected() {
  foreach(QGraphicsItem * item, m_scene.selectedItems()) {
    delete item;
  }
}

void MainWindow::on_reset_clicked() {
  if (m_scene.selectedItems().isEmpty()) {
    ui->graphicsView->setTransform(QTransform());
  } else {
    foreach(QGraphicsItem * item, m_scene.selectedItems()) {
      item->setTransform(QTransform());
    }
  }
}

void MainWindow::on_crop_clicked() {
  if (m_crop_target) {
    on_cancel_crop_clicked();
  }
  if (m_scene.selectedItems().isEmpty()) {
    QMessageBox::information(this, QString(), tr("No selection"));
    return;
  }
  m_crop_target = dynamic_cast<QGraphicsPixmapItem*>(m_scene.selectedItems()[0]);
  if (!m_crop_target) { return; }

  m_crop_target->setData(KEY_CROP, QRect());
  update_pixmap(m_crop_target);

  QRectF rect = m_crop_target->sceneBoundingRect();

  m_crop_display = m_scene.addRect(rect);
  //m_crop_display->setFlag(QGraphicsItem::ItemIgnoresTransformations);
  //m_crop_display->setParentItem(m_crop_target);
  QPen pen(Qt::green);
  pen.setCosmetic(true);
  m_crop_display->setPen(pen);
  //m_crop_display->setOpacity(0.3);

  foreach(Qt::Alignment alignment, QList<Qt::Alignment>() <<
          (Qt::AlignTop | Qt::AlignLeft) <<
          (Qt::AlignRight | Qt::AlignBottom) <<
          (Qt::AlignTop | Qt::AlignRight) <<
          (Qt::AlignBottom | Qt::AlignLeft) <<
          (Qt::AlignLeft | Qt::AlignVCenter) <<
          (Qt::AlignRight | Qt::AlignVCenter) <<
          (Qt::AlignTop | Qt::AlignHCenter) <<
          (Qt::AlignBottom | Qt::AlignHCenter)
    ) {
    CropHandle* handle = new CropHandle(this, alignment);
    m_crop_handles << handle;
  }
  update_crop_handles();
  foreach(CropHandle* handle, m_crop_handles) {
    m_scene.addItem(handle);
  }
  m_scene.clearSelection();

  ui->crop_notice->show();
  ui->crop->setEnabled(false);
}


void MainWindow::mousePressEvent(QMouseEvent * event) {
  QMainWindow::mousePressEvent(event);
}

void MainWindow::on_cancel_crop_clicked() {
  if (!m_crop_target) { return; }
  m_crop_target = 0;
  delete m_crop_display;
  m_crop_display = 0;
  qDeleteAll(m_crop_handles);
  m_crop_handles.clear();
  ui->crop_notice->hide();
  ui->crop->setEnabled(true);
}

void MainWindow::on_apply_crop_clicked() {
  QRect rect = m_crop_target->mapFromScene(m_crop_display->rect())
      .boundingRect()
      .translated(-m_crop_target->offset())
      .toAlignedRect()
      .intersected(m_crop_target->pixmap().rect());

  m_crop_target->setData(KEY_CROP, rect);
  update_pixmap(m_crop_target);
  on_cancel_crop_clicked();
}

void MainWindow::on_action_open_triggered() {
  QString filename = QFileDialog::getOpenFileName(this, QString(), QString(), tr("Simple collage (*.scl)"));
  if (filename.isEmpty()) { return; }
  open_project(filename);
}

void MainWindow::on_action_save_triggered() {
  if (m_current_filename.isEmpty()) {
    on_action_save_as_triggered();
  } else {
    save_project(m_current_filename);
  }
}

void MainWindow::on_action_save_as_triggered() {
  QString filename = QFileDialog::getSaveFileName(this, QString(), QString(), tr("Simple collage (*.scl)"));
  if (filename.isEmpty()) {
    return;
  }
  if (!filename.endsWith(".scl")) {
    filename += ".scl";
  }
  save_project(filename);
}

void MainWindow::on_action_add_image_triggered() {
  QString filename = QFileDialog::getOpenFileName(this, QString(), QString(), tr("Images (*.png *.jpg *.jpeg)"));
  if (filename.isEmpty()) { return; }
  add_image(filename);
}
