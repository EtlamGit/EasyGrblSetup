// Copyright 2017 EtlamGit

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ConfigWatcher.h"

#include <QMainWindow>
#include <QByteArray>
#include <QSerialPort>
#include <QTimer>
#include <QList>
#include <QLabel>


namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private:
  Ui::MainWindow *ui;

private slots:
  void handleTimer();
  void handleJog();

  void comRescan();
  void comConnect();
  void comHandleReadyRead();
  void comHandleError(QSerialPort::SerialPortError error);
  bool comWrite(QByteArray command);
  bool comWriteRealtime(QByteArray command);
  void comWriteFromBuffer();
  void comAppend(QString command);

  void parseLine(QString line);
  void parseOK();
  void commandCompleted();
  void parseGrblVersion(QString line);
  void parseGrblError(QString line);
  void parseGrblAlarm(QString line);
  void parseGrblMessage(QString line);

  void parseGrblStatusList(QString line);
  void parseGrblStatus(QString line);
  void showGrblStatus(QString status);
  void parseGrblStatusMPos(QString line);
  void parseGrblStatusWPos(QString line);
  void parseGrblStatusWCO(QString line);
  void parseGrblStatusPins(QString line);
  void parseGrblStatusOverride(QString line);
  void parseGrblStatusFeedSpeed(QString line);
  void parseGrblStatusFeedRate(QString line);

  void requestStatus();

  void buttonUnlock();
  void buttonHome();
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
  ConfigWatcher *m_configWatcher;
  QSerialPort  m_serialPort;
  QByteArray   m_readData;
  QTimer       m_timer;
  QStringList  m_commands;       // buffer for config write commands

  bool         m_ok;             // flag if ok response is received
  bool         m_error;          // flag if error response is received

  int          m_command_buffer; // estimated number of bytes in Grbl buffer
  QList<int>   m_command_length; // list with length of latest commands
  double       m_WCO[6];

  QLabel *m_label_error_code;
  QLabel *m_label_error;
  QLabel *m_label_alarm_code;
  QLabel *m_label_alarm;

};

#endif // MAINWINDOW_H
