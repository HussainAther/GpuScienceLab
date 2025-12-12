#pragma once
#include <QMainWindow>

class GLView;
class QLabel;
class QComboBox;
class QPushButton;

class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  explicit MainWindow(QWidget* parent = nullptr);

private:
  QWidget* makeControlPanel();
  QWidget* makeSliderRow(const QString& name, int min, int max, int value,
                         QLabel*& valueLabel, class QSlider*& slider);

  void applyParamsFromUI();

  GLView* m_view = nullptr;

  // UI
  QLabel* m_fpsLabel = nullptr;
  QComboBox* m_presetBox = nullptr;
  QPushButton* m_playPauseBtn = nullptr;

  // Sliders + readouts
  class QSlider* sDu = nullptr;
  class QSlider* sDv = nullptr;
  class QSlider* sF  = nullptr;
  class QSlider* sK  = nullptr;
  class QSlider* sDt = nullptr;

  QLabel* lDu = nullptr;
  QLabel* lDv = nullptr;
  QLabel* lF  = nullptr;
  QLabel* lK  = nullptr;
  QLabel* lDt = nullptr;

  bool m_running = true;
};

