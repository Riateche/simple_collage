#include "CropForm.h"
#include "ui_CropForm.h"

CropForm::CropForm(QPixmap pixmap, QWidget *parent) :
  QWidget(parent),
  ui(new Ui::CropForm)
{
  ui->setupUi(this);
  ui->graphicsView->setScene(&m_scene);
  m_pixmap = m_scene.addPixmap(pixmap);
}

CropForm::~CropForm() {
  delete ui;
}
