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
  connect(ui->save, SIGNAL(clicked()), this, SLOT(on_action_save_triggered()));
  ui->graphicsView->setScene(&m_scene);
  ui->graphicsView->viewport()->installEventFilter(this);
  ui->crop_notice->hide();
  setAcceptDrops(true);
  new QShortcut(QKeySequence::Delete, this, SLOT(delete_selected()));
  m_cropTarget = 0;
  m_cropDisplay = 0;

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

QGraphicsPixmapItem *MainWindow::add_image(const QString &filename) {
  QPixmap pixmap;
  if (!pixmap.load(filename)) { return 0; }
  QGraphicsPixmapItem *item = m_scene.addPixmap(pixmap);
  item->setOffset(-pixmap.width() / 2, -pixmap.height() / 2);
  item->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
  item->setTransformationMode(Qt::SmoothTransformation);
  item->setData(KEY_FILENAME, filename);
  return item;
}

void MainWindow::open_project(const QString& filename) {
  m_scene.clear();
  QSettings settings(filename, QSettings::IniFormat);
  ui->graphicsView->setTransform(settings.value("transform").value<QTransform>());
  QVariantList items = settings.value("items").toList();
  foreach(QVariant itemData, items) {
    QGraphicsPixmapItem *item = add_image(itemData.toMap()["filename"].toString());
    if (item) {
      item->setTransform(itemData.toMap()["transform"].value<QTransform>());
      item->setPos(itemData.toMap()["pos"].toPointF());
    }
  }
  remember_current(filename);
}

void MainWindow::save_project(const QString& filename) {
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
  remember_current(filename);
}

void MainWindow::remember_current(const QString& filename) {
  current_filename = filename;
  QSettings s;
  s.setValue("last_project", current_filename);
  s.setValue("geometry", saveGeometry());
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
  if (event->mimeData()->hasUrls()) {
    event->acceptProposedAction();
  }
}

void MainWindow::dropEvent(QDropEvent *event) {
  foreach(QUrl url, event->mimeData()->urls()) {
    QGraphicsPixmapItem *item = add_image(url.toLocalFile());
    if (item) {
      item->setPos(ui->graphicsView->mapToScene(ui->graphicsView->viewport()->mapFrom(this, event->pos())));
    }
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
    if (m_cropTarget) {
      QPointF pos = ui->graphicsView->mapToScene(e->pos());
      pos = m_cropTarget->mapFromScene(pos);
      if (m_cropTarget->boundingRect().contains(pos)) {
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

void MainWindow::on_action_open_triggered() {
  QString filename = QFileDialog::getOpenFileName(this, QString(), QString(), tr("Simple collage (*.scl)"));
  if (filename.isEmpty()) { return; }
  open_project(filename);
}

void MainWindow::on_action_save_triggered() {
  if (current_filename.isEmpty()) {
    on_action_save_as_triggered();
  } else {
    save_project(current_filename);
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
