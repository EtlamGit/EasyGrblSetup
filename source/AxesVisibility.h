// Copyright 2018 EtlamGit

#ifndef AXESVISIBILITY_H
#define AXESVISIBILITY_H

#include <QObject>

// Hide or Show UI elements based on the axes supported by the controller


namespace Ui {
class EasyGrblSetup;
}


class AxesVisibility : public QObject
{
  Q_OBJECT

public:
  AxesVisibility(Ui::EasyGrblSetup *ui);

public slots:
  void axisEnable (int index, bool flag = true);
  void axisDisable(int index, bool flag = true);

private:
  void setVisibleAxisX(bool flag);
  void setVisibleAxisY(bool flag);
  void setVisibleAxisZ(bool flag);
  void setVisibleAxisA(bool flag);
  void setVisibleAxisB(bool flag);
  void setVisibleAxisC(bool flag);

private:
  Ui::EasyGrblSetup *ui;  // pointer to main UI
};

#endif // AXESVISIBILITY_H
