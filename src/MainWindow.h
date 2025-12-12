#pragma once
#include <QMainWindow>

class GLView;

class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  explicit MainWindow(QWidget* parent = nullptr);
private:
  GLView* m_view = nullptr;
};

