#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  app.setOrganizationName("riateche");
  app.setApplicationName("simple_collage");
  MainWindow w;
  w.show();

  return app.exec();
}
