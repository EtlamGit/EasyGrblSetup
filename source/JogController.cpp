// Copyright 2018 EtlamGit

#include <cmath>

#include "JogController.h"
#include "ui_EasyGrblSetup.h"



JogController::JogController(Ui::EasyGrblSetup *ui)
  : ui(ui)
  , m_accelaration{1000, 1000, 1000, 1000, 1000, 1000}
  , m_deltaT{250, 250, 250, 250, 250, 250}
{
  connect( ui->dial_jog_speed, &QDial::valueChanged,
           this,               &JogController::updateJogSpeedLabel);
  updateJogSpeedLabel(ui->dial_jog_speed->value());

  // pressed()
  connect( ui->toolButton_jog_N,  &QToolButton::pressed,
           this,                  &JogController::jogLoop);
  connect( ui->toolButton_jog_NE, &QToolButton::pressed,
           this,                  &JogController::jogLoop);
  connect( ui->toolButton_jog_E,  &QToolButton::pressed,
           this,                  &JogController::jogLoop);
  connect( ui->toolButton_jog_SE, &QToolButton::pressed,
           this,                  &JogController::jogLoop);
  connect( ui->toolButton_jog_S,  &QToolButton::pressed,
           this,                  &JogController::jogLoop);
  connect( ui->toolButton_jog_SW, &QToolButton::pressed,
           this,                  &JogController::jogLoop);
  connect( ui->toolButton_jog_W,  &QToolButton::pressed,
           this,                  &JogController::jogLoop);
  connect( ui->toolButton_jog_NW, &QToolButton::pressed,
           this,                  &JogController::jogLoop);
  connect( ui->toolButton_jog_Zp, &QToolButton::pressed,
           this,                  &JogController::jogLoop);
  connect( ui->toolButton_jog_Zm, &QToolButton::pressed,
           this,                  &JogController::jogLoop);
  connect( ui->toolButton_jog_Ap, &QToolButton::pressed,
           this,                  &JogController::jogLoop);
  connect( ui->toolButton_jog_Am, &QToolButton::pressed,
           this,                  &JogController::jogLoop);
  connect( ui->toolButton_jog_Bp, &QToolButton::pressed,
           this,                  &JogController::jogLoop);
  connect( ui->toolButton_jog_Bm, &QToolButton::pressed,
           this,                  &JogController::jogLoop);
  connect( ui->toolButton_jog_Cp, &QToolButton::pressed,
           this,                  &JogController::jogLoop);
  connect( ui->toolButton_jog_Cm, &QToolButton::pressed,
           this,                  &JogController::jogLoop);

  // released()
  connect( ui->toolButton_jog_N,  &QToolButton::released,
           this,                  &JogController::jogLoop);
  connect( ui->toolButton_jog_NE, &QToolButton::released,
           this,                  &JogController::jogLoop);
  connect( ui->toolButton_jog_E,  &QToolButton::released,
           this,                  &JogController::jogLoop);
  connect( ui->toolButton_jog_SE, &QToolButton::released,
           this,                  &JogController::jogLoop);
  connect( ui->toolButton_jog_S,  &QToolButton::released,
           this,                  &JogController::jogLoop);
  connect( ui->toolButton_jog_SW, &QToolButton::released,
           this,                  &JogController::jogLoop);
  connect( ui->toolButton_jog_W,  &QToolButton::released,
           this,                  &JogController::jogLoop);
  connect( ui->toolButton_jog_NW, &QToolButton::released,
           this,                  &JogController::jogLoop);
  connect( ui->toolButton_jog_Zp, &QToolButton::released,
           this,                  &JogController::jogLoop);
  connect( ui->toolButton_jog_Zm, &QToolButton::released,
           this,                  &JogController::jogLoop);
  connect( ui->toolButton_jog_Ap, &QToolButton::released,
           this,                  &JogController::jogLoop);
  connect( ui->toolButton_jog_Am, &QToolButton::released,
           this,                  &JogController::jogLoop);
  connect( ui->toolButton_jog_Bp, &QToolButton::released,
           this,                  &JogController::jogLoop);
  connect( ui->toolButton_jog_Bm, &QToolButton::released,
           this,                  &JogController::jogLoop);
  connect( ui->toolButton_jog_Cp, &QToolButton::released,
           this,                  &JogController::jogLoop);
  connect( ui->toolButton_jog_Cm, &QToolButton::released,
           this,                  &JogController::jogLoop);

}

void JogController::updateMaxAcceleration(int axis, double value)
{
  m_accelaration[axis] = value;
}


void JogController::updateJogSpeedLabel(int value)
{
  m_speed = 60.0 * pow(10,value);
  ui->label_jog_speed->setText( QString("%1\nmm/min").arg(m_speed) );

  // https://github.com/gnea/grbl/wiki/Grbl-v1.1-Jogging#how-to-compute-incremental-distances
  for (int i=0; i<6; i++) {
    double speed = m_speed/60;
    m_deltaT[i] = (speed*speed) / (2 * m_accelaration[i] * (15-1));
    m_deltaT[i] = std::max<double>(0.050, m_deltaT[i]);
  }
  m_deltaT[1] = std::max<double>(m_deltaT[1], m_deltaT[0]);
  m_deltaT[0] = std::max<double>(m_deltaT[1], m_deltaT[0]);
}


void JogController::jogLoop()
{
  double  dT = 0.0;
  QString jCom;

  if      (ui->toolButton_jog_Cm->isDown()) { jCom = "C-%1";  dT = m_deltaT[5]; }
  else if (ui->toolButton_jog_Cp->isDown()) { jCom = "C+%1";  dT = m_deltaT[5]; }
  else if (ui->toolButton_jog_Bm->isDown()) { jCom = "B-%1";  dT = m_deltaT[4]; }
  else if (ui->toolButton_jog_Bp->isDown()) { jCom = "B+%1";  dT = m_deltaT[4]; }
  else if (ui->toolButton_jog_Am->isDown()) { jCom = "A-%1";  dT = m_deltaT[3]; }
  else if (ui->toolButton_jog_Ap->isDown()) { jCom = "A+%1";  dT = m_deltaT[3]; }
  else if (ui->toolButton_jog_Zm->isDown()) { jCom = "Z-%1";  dT = m_deltaT[2]; }
  else if (ui->toolButton_jog_Zp->isDown()) { jCom = "Z+%1";  dT = m_deltaT[2]; }
  else if (ui->toolButton_jog_N->isDown())  { jCom = "X0 Y+%1";   dT = m_deltaT[1]; }
  else if (ui->toolButton_jog_NE->isDown()) { jCom = "X+%1 Y+%1"; dT = m_deltaT[1]; }
  else if (ui->toolButton_jog_E->isDown())  { jCom = "X+%1 Y0";   dT = m_deltaT[1]; }
  else if (ui->toolButton_jog_SE->isDown()) { jCom = "X+%1 Y-%1"; dT = m_deltaT[1]; }
  else if (ui->toolButton_jog_S->isDown())  { jCom = "X0 Y-%1";   dT = m_deltaT[1]; }
  else if (ui->toolButton_jog_SW->isDown()) { jCom = "X-%1 Y-%1"; dT = m_deltaT[1]; }
  else if (ui->toolButton_jog_W->isDown())  { jCom = "X-%1 Y0";   dT = m_deltaT[1]; }
  else if (ui->toolButton_jog_NW->isDown()) { jCom = "X-%1 Y+%1"; dT = m_deltaT[1]; }

  if (dT >= 0.01) {
    double distance = m_speed/60.0 * dT;
    emit jogCommand("$J=G91 G21 " + jCom.arg(distance) + QString(" F%1").arg(m_speed));
  } else {
    emit jogStop();
  }
}


