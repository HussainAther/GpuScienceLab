#include "MainWindow.h"
#include "GLView.h"

#include <QDockWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QComboBox>
#include <QPushButton>
#include <QGroupBox>
#include <QSpacerItem>
#include <QString>

static int fToSlider(float v, float scale) { return (int)std::lround(v * scale); }
static float sliderToF(int s, float scale) { return (float)s / scale; }

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
  m_view = new GLView(this);
  setCentralWidget(m_view);

  auto* dock = new QDockWidget("Controls", this);
  dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  dock->setWidget(makeControlPanel());
  addDockWidget(Qt::RightDockWidgetArea, dock);

  setWindowTitle("GPU Science Lab — CUDA Reaction Diffusion");

  // FPS hookup
  connect(m_view, &GLView::fpsChanged, this, [this](double fps){
    if (m_fpsLabel) m_fpsLabel->setText(QString("FPS: %1").arg(fps, 0, 'f', 1));
  });

  // Initialize UI to current sim params
  applyParamsFromUI();
}

QWidget* MainWindow::makeSliderRow(const QString& name, int min, int max, int value,
                                   QLabel*& valueLabel, QSlider*& slider) {
  auto* row = new QWidget;
  auto* h = new QHBoxLayout(row);
  h->setContentsMargins(0, 0, 0, 0);

  auto* lbl = new QLabel(name);
  lbl->setMinimumWidth(28);

  slider = new QSlider(Qt::Horizontal);
  slider->setRange(min, max);
  slider->setValue(value);

  valueLabel = new QLabel(QString::number(value));
  valueLabel->setMinimumWidth(70);
  valueLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

  h->addWidget(lbl);
  h->addWidget(slider, 1);
  h->addWidget(valueLabel);

  return row;
}

QWidget* MainWindow::makeControlPanel() {
  auto* panel = new QWidget;
  auto* v = new QVBoxLayout(panel);

  // Presets + buttons
  {
    auto* top = new QGroupBox("Session");
    auto* tv = new QVBoxLayout(top);

    m_fpsLabel = new QLabel("FPS: —");

    m_presetBox = new QComboBox;
    m_presetBox->addItem("Custom", -1);
    m_presetBox->addItem("Spots (F=0.035, k=0.065)", 0);
    m_presetBox->addItem("Maze  (F=0.030, k=0.055)", 1);
    m_presetBox->addItem("Worms (F=0.022, k=0.051)", 2);

    auto* btnRow = new QWidget;
    auto* bh = new QHBoxLayout(btnRow);
    bh->setContentsMargins(0, 0, 0, 0);

    m_playPauseBtn = new QPushButton("Pause");
    auto* resetBtn = new QPushButton("Reset");

    bh->addWidget(m_playPauseBtn);
    bh->addWidget(resetBtn);

    tv->addWidget(m_fpsLabel);
    tv->addWidget(new QLabel("Preset"));
    tv->addWidget(m_presetBox);
    tv->addWidget(btnRow);

    v->addWidget(top);

    connect(m_playPauseBtn, &QPushButton::clicked, this, [this](){
      m_running = !m_running;
      m_view->setRunning(m_running);
      m_playPauseBtn->setText(m_running ? "Pause" : "Play");
    });

    connect(resetBtn, &QPushButton::clicked, this, [this](){
      m_view->resetSim();
    });

    connect(m_presetBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int idx){
      const int preset = m_presetBox->currentData().toInt();
      if (preset >= 0) {
        m_view->setPreset(preset);
        // Pull sim params after preset, reflect into sliders:
        auto p = m_view->params();

        // scaling: Du/Dv *1000, F/k *1000, dt *100
        sDu->setValue(fToSlider(p.Du, 1000.f));
        sDv->setValue(fToSlider(p.Dv, 1000.f));
        sF ->setValue(fToSlider(p.F,  1000.f));
        sK ->setValue(fToSlider(p.k,  1000.f));
        sDt->setValue(fToSlider(p.dt, 100.f));

        applyParamsFromUI();
      }
    });
  }

  // Sliders group
  {
    auto* grp = new QGroupBox("Parameters");
    auto* gv = new QVBoxLayout(grp);

    // Slider scales (so we can do precise floats with int sliders):
    // Du, Dv: 0..1.000 in steps of 0.001 => scale 1000
    // F, k  : 0..0.100 in steps of 0.001 => scale 1000 (but limit range)
    // dt    : 0..5.00 in steps of 0.01  => scale 100
    gv->addWidget(makeSliderRow("Du", 0, 1000, 160, lDu, sDu));
    gv->addWidget(makeSliderRow("Dv", 0, 1000,  80, lDv, sDv));
    gv->addWidget(makeSliderRow("F",   0,  100,  60, lF,  sF));
    gv->addWidget(makeSliderRow("k",   0,  100,  62, lK,  sK));
    gv->addWidget(makeSliderRow("dt",  0,  500, 100, lDt, sDt));

    v->addWidget(grp);

    auto onAnySlider = [this](){
      // Switch to custom when user tweaks
      if (m_presetBox && m_presetBox->currentIndex() != 0)
        m_presetBox->setCurrentIndex(0);
      applyParamsFromUI();
    };

    connect(sDu, &QSlider::valueChanged, this, onAnySlider);
    connect(sDv, &QSlider::valueChanged, this, onAnySlider);
    connect(sF,  &QSlider::valueChanged, this, onAnySlider);
    connect(sK,  &QSlider::valueChanged, this, onAnySlider);
    connect(sDt, &QSlider::valueChanged, this, onAnySlider);
  }

  v->addStretch(1);
  return panel;
}

void MainWindow::applyParamsFromUI() {
  SimulationParams p;
  p.Du = sliderToF(sDu->value(), 1000.f);
  p.Dv = sliderToF(sDv->value(), 1000.f);
  p.F  = sliderToF(sF ->value(), 1000.f);
  p.k  = sliderToF(sK ->value(), 1000.f);
  p.dt = sliderToF(sDt->value(), 100.f);

  // Pretty readouts
  lDu->setText(QString("Du=%1").arg(p.Du, 0, 'f', 3));
  lDv->setText(QString("Dv=%1").arg(p.Dv, 0, 'f', 3));
  lF ->setText(QString("F=%1").arg(p.F,  0, 'f', 3));
  lK ->setText(QString("k=%1").arg(p.k,  0, 'f', 3));
  lDt->setText(QString("dt=%1").arg(p.dt, 0, 'f', 2));

  m_view->setParams(p);
}

