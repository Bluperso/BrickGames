#include <QApplication>

#include "start_window.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  my_nsp::MainWindow window;
  window.show();

  return app.exec();
}