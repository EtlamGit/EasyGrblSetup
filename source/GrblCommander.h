// Copyright 2018 EtlamGit

#ifndef GRBLCOMMANDER_H
#define GRBLCOMMANDER_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QTimer>
#include <QList>
#include <QMap>
#include <QSerialPort>


namespace Ui {
class EasyGrblSetup;
}

class GrblStatusStorage
{
public:
  GrblStatusStorage()
    : MotionMode(-1)
    , Probing(-1)
    , WorkCoordinateSystem(-1)
    , PlaneSelect(-1)
    , DistanceMode(-1)
    , FeedRateMode(-1)
    , UnitsMode(-1)
    , CutterRadiusCompensation(-1)
    , ToolLengthOffset(-1)
    , ProgramMode(-1)
    , SpindleState(-1)
    , CoolantState(-1)
    , ToolNumber(-1)
    , FeedRate(-1)
    , SpindleSpeed(-1) {}

  int    MotionMode;
  int    Probing;
  int    WorkCoordinateSystem;
  int    PlaneSelect;
  int    DistanceMode;
  int    FeedRateMode;
  int    UnitsMode;
  int    CutterRadiusCompensation;
  int    ToolLengthOffset;
  int    ProgramMode;
  int    SpindleState;
  int    CoolantState;
  int    ToolNumber;
  double FeedRate;
  double SpindleSpeed;
};


class GrblCommander : public QObject
{
  Q_OBJECT

public:
  GrblCommander(Ui::EasyGrblSetup *ui);

public slots:

  // realtime commands
  void requestStatus();
  void jogStop();
  void sendReset();
  void sendHold();
  void sendResume();

  // normal queued commands
  void appendCommand(QString command);

  // COM methods
  bool comRescan();
  bool comConnect();
  void comDisconnect();

  // show text in status label and adapt background accordingly
  void showGrblStatus(const QString & status);

  // interface to query compile time options
  bool isOption(QString option);

  // interface to query "Settings" parameters
  bool   settingIsChecked(int num, int mask = 0x01);
  int    settingIValue(int num);
  double settingDValue(int num);

signals: // for information gathered during connection, used for GUI setup
  void foundAxis(int index, bool flag);

signals: // for all commands written (except some filtered)
  void writingCommand(QString command);

signals: // for receiving dedicated responses
  void receivedOK();                   // only those from commands
  void receivedJogOK();                // only those from JOG commands
  void receivedError(int code);        // error code
  void receivedAlarm(int code);        // alarm code
  void receivedStatus (QString line);  // complete <status> message
  void receivedMessage(QString line);  // complete [MSG:] message and everything not-parsable
  void receivedSetting(QString line);  // single Setting e.g. $1=123

signals: // for parsing G-Code Parser State, emitted when the State changes
  void changedMotionMode(int code);
  void changedProbing(int code);
  void changedWorkCoordinateSystem(int code);
  void changedPlaneSelect(int code);
  void changedDistanceMode(int code);
  void changedFeedRateMode(int code);
  void changedUnitsMode(int code);
  void changedCutterRadiusCompensation(int code);
  void changedToolLengthOffset(int code);
  void changedProgramMode(int code);
  void changedSpindleState(int code);
  void changedCoolantState(int code);
  void changedToolNumber(int code);
  void changedFeedRate(double code);
  void changedSpindleSpeed(int code);



private slots:
  // COM methods
  bool sendRealtime(char command);
  void writeCommands();
  void comHandleReadyRead();
  void comHandleError(QSerialPort::SerialPortError serialPortError);

  // handle stuff at regular timer (every 250ms)
  void handleTimer();
  void handleFirstConnect();
  void requestFirstStatus();

  // parse received stuff
  void parseLine(const QString &line);
  void updateGrblBuffer();
  void parseOK();
  void parseError  (const QString & line);
  void parseAlarm  (const QString & line);
  void parseStatus (const QString & line);
  void parseMessage(const QString & line);
  void parseSetting(const QString & line);

  void parseParserStateMessage(const QString &message);

  void parseGrblStatusList(const QString & status);
  void parseGrblStatus(const QString & status);
  void parseGrblStatusMPos(const QString & pos);
  void parseGrblStatusWPos(const QString & pos);
  void parseGrblStatusWCO (const QString & pos);
  void parseGrblStatusPins(const QString & pos);
  void parseGrblStatusOverride (const QString & pos);
  void parseGrblStatusFeedSpeed(const QString & pos);
  void parseGrblStatusFeedRate (const QString & pos);

private:
  Ui::EasyGrblSetup *ui;                  // pointer to main UI

  QTimer            m_timer;              // timer for regular status query and command writing
  int               m_statusCounter;      // counter for parameters updated not so often

  QSerialPort       m_serialPort;         // serial port used for communication
  QByteArray        m_readData;           // buffer for asynchronous reading
  QStringList       m_commandQueue;       // buffer for write commands

  int               m_grblBufferLength;   // estimated number of bytes in Grbl buffer
  QList<QString>    m_grblCommandList;    // list with length of latest commands

  bool              m_grblFound;          // flag if a grbl message was received
  int               m_axesInStatus;       // number of axes reportet in status messages
  int               m_reg2axes[6];        // translate status register index to axis number
  double            m_WCO[6];             // Work Coordinate Offset (for each axis)
  QStringList       m_grblOptions;        // list with GRBL compile time options
  QMap<int,QString> m_grblSettings;       // grbl settings accessable by their number
  GrblStatusStorage m_grblStatus;         // grbl Status
  QString           m_grblPins;           // grbl PIN status


};

#endif // GRBLCOMMANDER_H
