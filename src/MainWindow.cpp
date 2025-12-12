#include "MainWindow.h"
#include "GLView.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
  m_view = new GLView(this);
  setCentralWidget(m_view);
  setWindowTitle("GPU Science Lab — Reaction Diffusion (CUDA + Qt)");
}

