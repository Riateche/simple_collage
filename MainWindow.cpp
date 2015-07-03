#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDebug>
#include <QShortcut>
#include <QFileDialog>
#include <QSettings>
#include "Resizable_rubber_band.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  ui->graphicsView->setScene(&m_scene);
  ui->graphicsView->viewport()->installEventFilter(this);
  ui->crop_notice->hide();
  setAcceptDrops(true);
  new QShortcut(QKeySequence::Delete, this, SLOT(delete_selected()));
  m_cropTarget = 0;
  m_cropDisplay = 0;

  QStringList args = qApp->arguments();
  if (args.count() > 1) {
    QSettings settings(args[1], QSettings::IniFormat);
    ui->graphicsView->setTransform(settings.value("transform").value<QTransform>());
    QVariantList items = settings.value("items").toList();
    foreach(QVariant itemData, items) {
      QGraphicsPixmapItem *item = addFile(itemData.toMap()["filename"].toString());
      if (item) {
        item->setTransform(itemData.toMap()["transform"].value<QTransform>());
        item->setPos(itemData.toMap()["pos"].toPointF());
      }
    }
  }
}

MainWindow::~MainWindow()
{
  delete ui;
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

QGraphicsPixmapItem *MainWindow::addFile(const QString &filename) {
  QPixmap pixmap;
  if (pixmap.load(filename)) {
    QGraphicsPixmapItem *item = m_scene.addPixmap(pixmap);
    item->setOffset(-pixmap.width() / 2, -pixmap.height() / 2);
    item->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
    item->setTransformationMode(Qt::SmoothTransformation);
    item->setData(KEY_FILENAME, filename);
    return item;
  }
  return 0;
}


void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
  if (event->mimeData()->hasUrls()) {
    event->acceptProposedAction();
  }
}

void MainWindow::dropEvent(QDropEvent *event) {
  foreach(QUrl url, event->mimeData()->urls()) {
    QGraphicsPixmapItem *item = addFile(url.toLocalFile());
    if (item) {
      item->setPos(ui->graphicsView->mapToScene(ui->graphicsView->viewport()->mapFrom(this, event->pos())));
    }
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

void MainWindow::on_save_clicked() {
  QString filename = QFileDialog::getSaveFileName(this, QString(), QString(), tr("Simple collage (*.scl)"));
  if (filename.isEmpty()) {
    return;
  }
  QSettings settings(filename, QSettings::IniFormat);
  QVariantList list;
  foreach(QGraphicsItem *item, m_scene.items(Qt::AscendingOrder)) {
    QVariantMap v;
    v["filename"] = item->data(KEY_FILENAME);
    v["transform"] = QVariant::fromValue(item->transform());
    v["pos"] = item->pos();
    list << v;
  }
  settings.setValue("items", list);
  settings.setValue("transform", QVariant::fromValue(ui->graphicsView->transform()));
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
  if (m_cropTarget) {
    on_cancel_crop_clicked();
  }
  if (m_scene.selectedItems().isEmpty()) {
    QMessageBox::information(this, "", tr("No selection"));
    return;
  }
  m_cropTarget = dynamic_cast<QGraphicsPixmapItem*>(m_scene.selectedItems()[0]);
  if (!m_cropTarget) { return; }

  m_cropDisplay = m_scene.addRect(m_cropTarget->boundingRect());
  m_cropDisplay->setParentItem(m_cropTarget);
  m_cropDisplay->setBrush(Qt::green);
  m_cropDisplay->setOpacity(0.3);
  ui->crop_notice->show();
}


void MainWindow::mousePressEvent(QMouseEvent * event) {
  QMainWindow::mousePressEvent(event);
}


bool MainWindow::eventFilter(QObject * object, QEvent *event) {
  if (object == ui->graphicsView->viewport() &&
      (event->type() == QEvent::MouseButtonPress ||
       event->type() == QEvent::MouseMove)) {
    QMouseEvent * e = static_cast<QMouseEvent*>(event);
    qDebug() << "ok1";
    if (m_cropTarget) {
      qDebug() << "ok2";
      QPointF pos = ui->graphicsView->mapToScene(e->pos());
      pos = m_cropTarget->mapFromScene(pos);
      if (m_cropTarget->boundingRect().contains(pos)) {
        qDebug() << "ok3";
        QRectF rect = m_cropDisplay->rect();
        if (e->buttons() & Qt::LeftButton) {
          rect.setTopLeft(pos);
        } else if (e->buttons() & Qt::RightButton) {
          rect.setBottomRight(pos);
        }
        m_cropDisplay->setRect(rect);
        return true;
      }
    }
  }
  return false;
}

void MainWindow::on_cancel_crop_clicked() {
  if (!m_cropTarget) { return; }
  m_cropTarget = 0;
  delete m_cropDisplay;
  m_cropDisplay = 0;
  ui->crop_notice->hide();
}

void MainWindow::on_apply_crop_clicked() {
  QRect rect = m_cropDisplay->rect().translated(m_cropTarget->offset()).toAlignedRect();
  m_cropTarget->setData(KEY_CROP, rect);
  m_cropTarget->setPixmap(QPixmap(m_cropTarget->data(KEY_FILENAME).toString()).copy(rect));


  on_cancel_crop_clicked();
}
