// Copyright 2017 EtlamGit

#ifndef EASYGRBLSETUP_H
#define EASYGRBLSETUP_H

#include <QMainWindow>
#include <QMenu>
#include <QLabel>

#include "ConfigWatcher.h"
#include "AxesVisibility.h"
#include "GrblCommander.h"
#include "JogController.h"


namespace Ui {
  class EasyGrblSetup;
}

class EasyGrblSetup : public QMainWindow
{
  Q_OBJECT

public:
  explicit EasyGrblSetup(QWidget *parent = 0);
  ~EasyGrblSetup();

private:
  Ui::EasyGrblSetup *ui;

private slots:
  void enableElementsAfterConnect();
  void disableElementsAfterDisonnect();

  void normalLog (QString text);
  void verboseLog(QString text);

  void decodeError(int code);
  void decodeAlarm(int code);

  void setZeroX();
  void setZeroY();
  void setZeroZ();
  void setZeroA();
  void setZeroB();
  void setZeroC();

  void createContextMenus();
  void showContextMenuAxis(int axis, const QPoint& pos);
  void showContextMenuAxisX(const QPoint& pos);
  void showContextMenuAxisY(const QPoint& pos);
  void showContextMenuAxisZ(const QPoint& pos);
  void showContextMenuAxisA(const QPoint& pos);
  void showContextMenuAxisB(const QPoint& pos);
  void showContextMenuAxisC(const QPoint& pos);

  void buttonConnect();
  void buttonDisconnect();
  void buttonUnlock();
  void buttonHome();
  void buttonReset();
  void buttonHold();
  void buttonResume();

  void buttonEnableHoming();
  void buttonDisableHoming();
  void buttonEnableSoftlimit();
  void buttonDisableSoftlimit();
  void buttonEnableHardlimit();
  void buttonDisableHardlimit();
  void buttonDirectionTestX();
  void buttonDirectionTestY();
  void buttonDirectionTestZ();
  void buttonDirectionTestA();
  void buttonDirectionTestB();
  void buttonDirectionTestC();
  void buttonHomeX();
  void buttonHomeY();
  void buttonHomeZ();
  void buttonHomeA();
  void buttonHomeB();
  void buttonHomeC();
  void buttonSpeedTestX();
  void buttonSpeedTestY();
  void buttonSpeedTestZ();
  void buttonSpeedTestA();
  void buttonSpeedTestB();
  void buttonSpeedTestC();


private:
  ConfigWatcher  *m_configWatcher;
  AxesVisibility *m_axesHide;       // pointer axes visibility controller
  GrblCommander  *m_grblCom;        // pointer GRBL Commander
  JogController  *m_jogController;  // pointer Jog Controller

  QMenu  *m_axesMenu;
  QLabel *m_errorLabel;
  QLabel *m_alarmLabel;
};

#endif // EASYGRBLSETUP_H
