// Copyright 2018 EtlamGit

#ifndef JOGCONTROLLER_H
#define JOGCONTROLLER_H

#include <QObject>
#include <QTimer>

// create Jog commands based on button state
//  used buttons are automatically connected from *ui


namespace Ui {
class EasyGrblSetup;
}


class JogController : public QObject
{
  Q_OBJECT

public:
  JogController(Ui::EasyGrblSetup *ui);

public slots:
  void updateMaxAcceleration(int axis, double value);
  void jogLoop();

signals:
  void jogCommand(QString command);
  void jogStop();

private slots:
  void updateJogSpeedLabel(int value);


private:
  Ui::EasyGrblSetup *ui;                  // pointer to main UI
  double            m_accelaration[6];    // max acceleration (per axis)
  double            m_deltaT[6];          // minimum delta time for a jog command (per axis)
  double            m_speed;
};

#endif // JOGCONTROLLER_H
