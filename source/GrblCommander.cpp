// Copyright 2018 EtlamGit

#include "GrblCommander.h"
#include "ui_EasyGrblSetup.h"

#include <QSerialPortInfo>
#include <QMessageBox>


GrblCommander::GrblCommander(Ui::EasyGrblSetup *ui)
  : ui(ui)
  , m_statusCounter(0)
  , m_grblBufferLength(0)
  , m_grblPins("")
  , m_axesInStatus(-1)
  , m_reg2axes{6, 6, 6, 6, 6, 6}
  , m_WCO{0.0, 0.0, 0.0, 0.0, 0.0, 0.0}
{
  comDisconnect();
  comRescan();

  // COM asynchronous methods
  connect( &m_serialPort, &QSerialPort::readyRead,
           this,          &GrblCommander::comHandleReadyRead);
//  connect( &m_serialPort, &QSerialPort::errorOccured,
//           this,          &SerialCommands::comHandleError);

  // setup timer
  connect( &m_timer, &QTimer::timeout,
           this,     &GrblCommander::handleTimer );
  m_timer.setSingleShot(false);
  m_timer.setInterval(250);
}


// ---------- ---------- ---------- ---------- ---------- ---------- ----------
// Timer
// ---------- ---------- ---------- ---------- ---------- ---------- ----------

void GrblCommander::handleTimer()
{
  requestStatus();

  if (m_statusCounter++ < 40) {
    writeCommands();
  } else if (ui->label_grbl_status->text() != "Home") {
    m_statusCounter = 0;
    appendCommand("$G (no-logging)");  // request gcode parser state
  }
}

void GrblCommander::handleConnectTimeout()
{
  if (!m_grblFound) {
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText("No grbl controller found!");
    msgBox.setInformativeText("For safety reasons no communication is send via COM until this message is received.\nYou can disable this on your own risk.");
    QAbstractButton* pUnsafe = msgBox.addButton(tr("Unsafe"), QMessageBox::YesRole);
    msgBox.addButton(QMessageBox::Ok);
    msgBox.exec();
    if (msgBox.clickedButton() == pUnsafe) {
      m_grblFound = true;
      sendRealtime('\x18'); // soft reset
    } else
      ui->toolButton_disconnect->click();
  }
}

void GrblCommander::requestFirstStatus()
{
  // start regular status timer
  m_timer.start();

  sendRealtime('?');                 // request first status
  appendCommand("$I");               // request build info
  appendCommand("$$ (no-logging)");  // request settings
  appendCommand("$G (no-logging)");  // request gcode parser state
  appendCommand("$# (no-logging)");  // request gcode parameters

  // enable GUI elements
  emit machineConnected();
}


// ---------- ---------- ---------- ---------- ---------- ---------- ----------
// command methods
// ---------- ---------- ---------- ---------- ---------- ---------- ----------

void GrblCommander::requestStatus() { sendRealtime('?'); }
void GrblCommander::sendReset()     { sendRealtime('\x18'); }
void GrblCommander::sendHold()      { sendRealtime('!'); }
void GrblCommander::sendResume()    { sendRealtime('~'); }

void GrblCommander::jogStop()
{
  // remove Jog commands from our queue (just in case)
  m_mutex.lock();
  for (int i = 0; i < m_commandQueue.length();) {
    if (m_commandQueue[i].startsWith("$J="))
      m_commandQueue.removeAt(i);
    else i++;
  }
  m_mutex.unlock();
  // remove Jog commands from Grbl queue
  sendRealtime('\x85');
}

bool GrblCommander::sendRealtime(char command)
{
  if (m_serialPort.isOpen()) {
    m_serialPort.write(&command,1);
    m_serialPort.flush();
    return true;
  }
  return false;
}


void GrblCommander::appendCommandDirectWrite(QString command)
{
  appendCommand(command);
  // and try to write immediatly
  writeCommands();
}


void GrblCommander::appendCommand(QString command)
{
  // ignore empty lines
  if (command.length() == 0)
    return;

  // append "line feed" if missing
  if (!command.endsWith("\n"))
    command.append('\n');

  // append command to command queue
  m_mutex.lock();
  m_commandQueue.append( command );
  m_mutex.unlock();
}


bool GrblCommander::isCommandBufferEmpty()
{
  m_mutex.lock();
  bool empty = (m_commandQueue.length() == 0);
  m_mutex.unlock();
  return empty;
}


void GrblCommander::writeCommands()
{
  // write queued commands
  m_mutex.lock();
  while ((m_serialPort.isOpen()) && (!m_commandQueue.isEmpty()) &&
         (m_grblBufferLength + m_commandQueue.at(0).length() <= 128)) {
    // use the first command from buffer
    QString command(m_commandQueue.takeFirst());

    if (!command.contains("(no-logging)") && !command.startsWith("$J="))
      emit writingCommand("> " + command.left(command.length()-1));

    // remove spaces at end
    while (command.endsWith(' '))
      command.chop(1);

    // filter command for forbidden characters
    command = command.toUtf8();
    QByteArray filtered;
    for (int i = 0; i < command.length(); i++) {
      QChar q = command[i];
      char  c = command[i].toLatin1();
      if (q < 128) { // filter everything outside standard ASCII
        switch (c) {
        case '\x18':  // soft reset
        case '?':     // query status
        case '~':     // resume
        case '!':     // hold
          sendRealtime(c);
          break;
        default:
          filtered.append(c);
        }
      }
    }

    // write filtered command to serial port
    m_grblBufferLength += filtered.length();
    m_grblCommandList.append(filtered);
    m_serialPort.write(filtered,filtered.length());
    m_serialPort.flush();
  }
  m_mutex.unlock();

  // update command buffer label
  if ((m_grblBufferLength > 0) || (m_grblCommandList.length() > 0 )) {
    ui->label_grbl_buffer->setText(QString("%1/%2").arg(m_grblBufferLength).arg(m_grblCommandList.length()));
  } else {
    ui->label_grbl_buffer->setText("");
  }
}

// ---------- ---------- ---------- ---------- ---------- ---------- ----------
// COM methods called from interface
// ---------- ---------- ---------- ---------- ---------- ---------- ----------

bool GrblCommander::comRescan()
{
  ui->comboBox_com->clear();

  const auto serialPortInfos = QSerialPortInfo::availablePorts();
  int num = serialPortInfos.count();

  if (num > 0) {
    for (const QSerialPortInfo &serialPortInfo : serialPortInfos) {
      ui->comboBox_com->addItem( serialPortInfo.portName() );
    }

    ui->comboBox_com->setCurrentIndex( num-1 );
    ui->toolButton_connect->setEnabled(true);
  } else {
    ui->toolButton_connect->setDisabled(true);
    return false;
  }

  return true;
}


bool GrblCommander::comConnect()
{
  m_serialPort.setPortName( ui->comboBox_com->currentText() );
  m_serialPort.setBaudRate( 115200 );
  m_serialPort.setDataBits( QSerialPort::Data8 );
  m_serialPort.setParity  ( QSerialPort::NoParity );
  m_serialPort.setStopBits( QSerialPort::OneStop );

  if (m_serialPort.open(QIODevice::ReadWrite)) {
    m_serialPort.setDataTerminalReady(true);  // force Arduino Reset

    ui->toolButton_rescan ->setVisible(false);
    ui->comboBox_com      ->setVisible(false);
    ui->toolButton_connect->setVisible(false);

    ui->label_grbl_version   ->setVisible(true);
    ui->label_grbl_buffer    ->setVisible(true);
    ui->toolButton_disconnect->setVisible(true);

    ui->plainTextEdit_log->clear();

    // reset machine knowledge
    m_grblFound = false;
    m_axesInStatus = -1;
    for (int i = 0; i < 6; i++) {
      m_reg2axes[i] = 6;
    }

    // setup single shot timer to detect timeout at connect
    QTimer::singleShot(5000, this, &GrblCommander::handleConnectTimeout);

    return true;
  }

  return false;
}


void GrblCommander::comDisconnect()
{
  m_timer.stop();
  m_serialPort.close();

  ui->label_grbl_version   ->setVisible(false);
  ui->label_grbl_buffer    ->setVisible(false);
  ui->toolButton_disconnect->setVisible(false);

  ui->toolButton_rescan ->setVisible(true);
  ui->comboBox_com      ->setVisible(true);
  ui->toolButton_connect->setVisible(true);

  showGrblStatus("?");
  emit machineDisconnected();

  m_commandQueue.clear();
  m_grblBufferLength = 0;
  m_grblCommandList.clear();
}


// ---------- ---------- ---------- ---------- ---------- ---------- ----------
// COM asynchronous read handler
// ---------- ---------- ---------- ---------- ---------- ---------- ----------

void GrblCommander::comHandleReadyRead()
{
  m_readData.append(m_serialPort.readAll());

  // process complete lines
  int index = m_readData.indexOf("\n");
  while ( index >= 0) {
    // extract first line
    QString line(QString::fromLatin1(m_readData.left(index-1)));
    m_readData.remove(0, index+1);
    index = m_readData.indexOf("\n");

    // remove "carriage return"
    if (line.endsWith("\r"))
      line.chop(1);

    // parse line
    parseLine(line);
  }
}


void GrblCommander::comHandleError(QSerialPort::SerialPortError serialPortError)
{
  if (serialPortError == QSerialPort::ReadError) {

  }
}


// ---------- ---------- ---------- ---------- ---------- ---------- ----------
// interface to query compile time options
// ---------- ---------- ---------- ---------- ---------- ---------- ----------
bool GrblCommander::isOption(QString option)
{
  if (option.length() == 1) {
    // "option" is found as character in first string
    if (m_grblOptions.at(0).contains(option, Qt::CaseInsensitive)) return true;
  } else {
    // "option" is found as complete string
    if (m_grblOptions.contains(option, Qt::CaseInsensitive)) return true;
  }

  return false;
}



// ---------- ---------- ---------- ---------- ---------- ---------- ----------
// interface to query configuration parameters
// ---------- ---------- ---------- ---------- ---------- ---------- ----------
bool GrblCommander::settingIsChecked(int num, int mask)
{
  return (m_grblSettings[num].toInt() & mask) != 0;
}

int GrblCommander::settingIValue(int num)
{
  return m_grblSettings[num].toInt();
}

double GrblCommander::settingDValue(int num)
{
  return m_grblSettings[num].toDouble();
}



// ---------- ---------- ---------- ---------- ---------- ---------- ----------
// simple pre-PARSER for grbl responses
// ---------- ---------- ---------- ---------- ---------- ---------- ----------

void GrblCommander::parseLine(const QString &line)
{
  if (line.length() == 0) return;

  if (line == "ok")             return parseOK();
  if (line.left(6) == "error:") return parseError(line);
  if (line.left(6) == "ALARM:") return parseAlarm(line);
  if (line.left(1) == "<")      return parseStatus(line);
  if (line.left(1) == "$")      return parseSetting(line);
  if (line.left(1) == "[")      return parseMessage(line);

  // search for welcome message
  if (line.startsWith("Grbl") && line.contains("['$' for help]")) {
    // grbl welcome message => reqeust some more information
    m_grblFound = true;
    requestFirstStatus();
  }

  // output all the rest to log ...
  emit receivedMessage(line);
}

void GrblCommander::updateGrblBuffer()
{
  if (m_grblCommandList.isEmpty()) {
    m_grblBufferLength = 0;
  } else {
    m_grblBufferLength -= m_grblCommandList.takeFirst().length();
  }
}

void GrblCommander::parseOK()
{
  if (m_grblCommandList.first().startsWith("$J=")) {
    emit receivedJogOK();
  } else {
    emit receivedOK();
  }
  updateGrblBuffer();
}

void GrblCommander::parseError(const QString &line)
{
  updateGrblBuffer();
  QStringList list = line.split(':');
  emit receivedError(list.at(1).toInt());
}

void GrblCommander::parseAlarm(const QString &line)
{
  QStringList list = line.split(':');
  showGrblStatus("Alarm");
  emit receivedAlarm(list.at(1).toInt());
  emit receivedMessage(line);
}

void GrblCommander::parseStatus(const QString &line)
{
  if (m_axesInStatus<0) {
    // detect number of axes reported in status
    QStringList status = line.mid(1,line.length()-2).split('|');
    QStringList mpos = status.at(1).split(':').at(1).split(',');
    m_axesInStatus = mpos.length();
  }
  parseGrblStatusList(line);  // we parse it on our own
  emit receivedStatus(line);  // and leave others the chance to connets also
}

void GrblCommander::parseSetting(const QString &line)
{
  QStringList setting = line.split('=');
  QString sNum = setting.at(0);
  int     iNum = sNum.remove(0,1).toInt();

  m_grblSettings[iNum] = setting.at(1);

  emit receivedSetting(line);
  if ( m_grblCommandList.isEmpty() ||
      !m_grblCommandList.at(0).contains("no-logging")) {
    emit receivedMessage(line);
  }
}

void GrblCommander::parseMessage(const QString &line)
{
  QStringList message = line.mid(1,line.length()-2).split(':');

  if (message.at(0) == "VER") {
    if (line.contains("BUILD:")) {
      // extra handling for Carsten Meyer
      // [VER:1.1f3,BUILD:DD.MM.YYYYoptionalString]
      QString version   = line.mid(1,line.length()-2).split(',').at(0).split(':').at(1);
      QString builddate = line.mid(1,line.length()-2).split(',').at(1).split(':').at(1);
      if (builddate.length()>10) {
        emit foundMachineName(builddate.right(builddate.length()-10));
      } else {
        emit foundMachineName(version + "." + builddate.mid(6,4) + builddate.mid(3,2) + builddate.left(2));
      }
    } else {
      // [VER:1.1f3.YYYYMMDD:optionalString]
      if (message.at(2).length() > 0) {
        emit foundMachineName(message.at(2));
      } else {
        emit foundMachineName(message.at(1));
      }
    }

  } else if (message.at(0) == "OPT") {
    // [OPT:VMH,AVR_644,SPI_SR,SPI_DISP,PANEL,C_AXIS]
    m_grblOptions = message.at(1).split(',');

    // pre-process irregular axes
    int maxis = m_axesInStatus;
    if (m_grblOptions.contains("A_AXIS")) maxis--;
    if (m_grblOptions.contains("B_AXIS")) maxis--;
    if (m_grblOptions.contains("C_AXIS")) maxis--;

    // process regular axes
    int i = 0;
    for (i = 0; i < maxis; i++) {
      m_reg2axes[i] = i;
      emit foundAxis(i,true);
    }
    for (; i < 6; i++)
      emit foundAxis(i,false);

    // process irregular axes
    if (m_grblOptions.contains("A_AXIS")) {
      m_reg2axes[maxis++] = 3;
      emit foundAxis(3,true);
    }
    if (m_grblOptions.contains("B_AXIS")) {
      m_reg2axes[maxis++] = 4;
      emit foundAxis(4,true);
    }
    if (m_grblOptions.contains("C_AXIS")) {
      m_reg2axes[maxis++] = 5;
      emit foundAxis(5,true);
    }

  } else if (message.at(0) == "GC") { // $G G-code Parser State Message
    parseParserStateMessage(message.at(1));
  } else if (message.at(0) == "G54") {
  } else if (message.at(0) == "G55") {
  } else if (message.at(0) == "G56") {
  } else if (message.at(0) == "G57") {
  } else if (message.at(0) == "G58") {
  } else if (message.at(0) == "G59") {

  } else if (message.at(0) == "G28") {
  } else if (message.at(0) == "G30") {
  } else if (message.at(0) == "G92") {

  } else if (message.at(0) == "TLO") {
  } else if (message.at(0) == "PRB") {

  }

  if ( m_grblCommandList.isEmpty() ||
      !m_grblCommandList.at(0).contains("no-logging")) {
    emit receivedMessage(line);
  }
}


void GrblCommander::parseParserStateMessage(const QString &message)
{
  // [GC:G0 G54 G17 G21 G90 G94 M0 M5 M9 T0 S0.0 F500.0]
  // https://github.com/gnea/grbl/wiki/Grbl-v1.1-Commands#g---view-gcode-parser-state
  QStringList states = message.split(' ');
  foreach (const QString &state, states) {
    QChar  code = state[0];
    double val  = state.mid(1,99).toDouble();
    int    ival = floor(val);
    int    fval = 10*(val-ival);

    if (code == 'G') {
      switch (ival) {
      case 0:
      case 1:
      case 2:
      case 3:
      case 80:
        if (m_grblStatus.MotionMode != ival) {
          m_grblStatus.MotionMode = ival;
          emit changedMotionMode(ival);
        }
        break;
      case 38:
        if (m_grblStatus.Probing != ival) {
          m_grblStatus.Probing = ival;
          emit changedProbing(fval);
          }
        break;
      case 54:
      case 55:
      case 56:
      case 57:
      case 58:
      case 59:
        if (m_grblStatus.WorkCoordinateSystem != ival) {
          m_grblStatus.WorkCoordinateSystem = ival;
          emit changedWorkCoordinateSystem(ival);
        }
        break;
      case 17:
      case 18:
      case 19:
        if (m_grblStatus.PlaneSelect != ival) {
          m_grblStatus.PlaneSelect = ival;
          emit changedPlaneSelect(ival);
        }
        break;
      case 90:
      case 91:
        if (m_grblStatus.DistanceMode != ival) {
          m_grblStatus.DistanceMode = ival;
          emit changedDistanceMode(ival);
        }
        break;
      case 93:
      case 94:
        if (m_grblStatus.FeedRateMode != ival) {
          m_grblStatus.FeedRateMode = ival;
          emit changedFeedRateMode(ival);
        }
        break;
      case 20:
      case 21:
        if (m_grblStatus.UnitsMode != ival) {
          m_grblStatus.UnitsMode = ival;
          emit changedUnitsMode(ival);
        }
        break;
      case 40:
        if (m_grblStatus.CutterRadiusCompensation != ival) {
          m_grblStatus.CutterRadiusCompensation = ival;
          emit changedCutterRadiusCompensation(ival);
        }
        break;
      case 43:  // 43.1
      case 49:
        if (m_grblStatus.ToolLengthOffset != ival) {
          m_grblStatus.ToolLengthOffset = ival;
          emit changedToolLengthOffset(ival);
        }
        break;
      }

    } else if (code == 'M') {
      switch (ival) {
      case 0:
      case 1:
      case 2:
      case 30:
        if (m_grblStatus.ProgramMode != ival) {
          m_grblStatus.ProgramMode = ival;
          emit changedProgramMode(ival);
        }
        break;
      case 3:
      case 4:
      case 5:
        if (m_grblStatus.SpindleState != ival) {
          m_grblStatus.SpindleState = ival;
          emit changedSpindleState(ival);
        }
        break;
      case 7:
      case 8:
      case 9:
        if (m_grblStatus.CoolantState != ival) {
          m_grblStatus.CoolantState = ival;
          emit changedCoolantState(ival);
        }
        break;
      }

    } else if (code == 'T') {
      if (m_grblStatus.ToolNumber != ival) {
        m_grblStatus.ToolNumber = ival;
        emit changedToolNumber(ival);
      }

    } else if (code == 'F') {
      if (m_grblStatus.FeedRate != ival) {
        m_grblStatus.FeedRate = ival;
        emit changedFeedRate(val);
      }

    } else if (code == 'S') {
      if (m_grblStatus.SpindleSpeed != ival) {
        m_grblStatus.SpindleSpeed = ival;
        emit changedSpindleSpeed(ival);
      }
    }
  }
}


// ---------- ---------- ---------- ---------- ---------- ---------- ----------
// <Status> Parser
// ---------- ---------- ---------- ---------- ---------- ---------- ----------

// example <Idle|MPos:0.000,0.000,0.000|FS:0.0,0>
void GrblCommander::parseGrblStatusList(const QString & line)
{
  QStringList list = line.mid(1,line.length()-2).split('|');

  showGrblStatus(list.takeFirst());

  foreach (const QString &pos, list) {
    parseGrblStatus(pos);
  }

  if (list.filter("Pn:").length() == 0) {
    // when no pin state is reported, no pins are active
    parseGrblStatusPins("");
  }
}

// valid states types: Idle, Run, Hold, Jog, Alarm, Door, Check, Home, Sleep
void GrblCommander::showGrblStatus(const QString & status)
{
  m_grblStatus.Status = status;
  QStringList list = status.split(':');

  // copy status to label
  ui->label_grbl_status->setText(status);

  // do some color tweaking based on status
  if        (list.at(0) == "Idle") {
    ui->label_grbl_status->setStyleSheet("");
    ui->toolButton_unlock->setEnabled(false);
  } else if (list.at(0) == "Run") {
    ui->label_grbl_status->setStyleSheet("QLabel { background-color:limegreen; }");
  } else if (list.at(0) == "Hold") {
    ui->label_grbl_status->setStyleSheet("QLabel { background-color:cornflowerblue; }");
  } else if (list.at(0) == "Jog") {
    ui->label_grbl_status->setStyleSheet("QLabel { background-color:cyan; }");
  } else if (list.at(0) == "Alarm") {
    ui->label_grbl_status->setStyleSheet("QLabel { background-color:orangered; }");
    ui->toolButton_unlock->setEnabled(true);
  } else if (list.at(0) == "Door") {
  } else if (list.at(0) == "Check") {
  } else if (list.at(0) == "Home") {
    ui->label_grbl_status->setStyleSheet("QLabel { background-color:yellow; }");
  } else if (list.at(0) == "Sleep") {
    ui->label_grbl_status->setStyleSheet("QLabel { background-color:darkgray; }");
  } else {
    ui->label_grbl_status->setStyleSheet("");
  }

}


const QString & GrblCommander::getGrblStatus()
{
  return m_grblStatus.Status;
}


// valid parameters: MPos, WPos, WCO, Bf, Ln, F, FS, Pn, Ov, A
void GrblCommander::parseGrblStatus(const QString & status)
{
  QStringList plist = status.split(':');

  if (plist.length() == 2) {
    if (plist.at(0) == "MPos")  return parseGrblStatusMPos(plist.at(1));
    if (plist.at(0) == "WPos")  return parseGrblStatusWPos(plist.at(1));
    if (plist.at(0) == "WCO")   return parseGrblStatusWCO (plist.at(1));
    if (plist.at(0) == "Pn")    return parseGrblStatusPins(plist.at(1));
    if (plist.at(0) == "Ov")    return parseGrblStatusOverride (plist.at(1));
    if (plist.at(0) == "FS")    return parseGrblStatusFeedSpeed(plist.at(1));
    if (plist.at(0) == "F")     return parseGrblStatusFeedRate (plist.at(1));
    if (plist.at(0) == "Bf")    return;  // ignore
    if (plist.at(0) == "Ln")    return;  // ignore
    if (plist.at(0) == "A")     return;  // ignore
  }

  // log everything not parsable
  ui->plainTextEdit_log->appendPlainText(status);
}

void GrblCommander::parseGrblStatusMPos(const QString & status)
{
  QStringList list = status.split(',');

  for (int i=0; i < list.length(); i++) {
    int idx = m_reg2axes[i];

    switch (idx) {
    case 0:  ui->doubleSpinBox_MposX->setValue(list.at(i).toDouble());  break;
    case 1:  ui->doubleSpinBox_MposY->setValue(list.at(i).toDouble());  break;
    case 2:  ui->doubleSpinBox_MposZ->setValue(list.at(i).toDouble());  break;
    case 3:  ui->doubleSpinBox_MposA->setValue(list.at(i).toDouble());  break;
    case 4:  ui->doubleSpinBox_MposB->setValue(list.at(i).toDouble());  break;
    case 5:  ui->doubleSpinBox_MposC->setValue(list.at(i).toDouble());  break;
    }

    switch (idx) {
    case 0:  ui->doubleSpinBox_WposX->setValue( list.at(i).toDouble() - m_WCO[i] );  break;
    case 1:  ui->doubleSpinBox_WposY->setValue( list.at(i).toDouble() - m_WCO[i] );  break;
    case 2:  ui->doubleSpinBox_WposZ->setValue( list.at(i).toDouble() - m_WCO[i] );  break;
    case 3:  ui->doubleSpinBox_WposA->setValue( list.at(i).toDouble() - m_WCO[i] );  break;
    case 4:  ui->doubleSpinBox_WposB->setValue( list.at(i).toDouble() - m_WCO[i] );  break;
    case 5:  ui->doubleSpinBox_WposC->setValue( list.at(i).toDouble() - m_WCO[i] );  break;
    }
  }
}

void GrblCommander::parseGrblStatusWPos(const QString & status)
{
  QStringList list = status.split(',');

  for (int i=0; i < list.length(); i++) {
    int idx = m_reg2axes[i];

    switch (idx) {
    case 0:  ui->doubleSpinBox_WposX->setValue(list.at(i).toDouble());  break;
    case 1:  ui->doubleSpinBox_WposY->setValue(list.at(i).toDouble());  break;
    case 2:  ui->doubleSpinBox_WposZ->setValue(list.at(i).toDouble());  break;
    case 3:  ui->doubleSpinBox_WposA->setValue(list.at(i).toDouble());  break;
    case 4:  ui->doubleSpinBox_WposB->setValue(list.at(i).toDouble());  break;
    case 5:  ui->doubleSpinBox_WposC->setValue(list.at(i).toDouble());  break;
    }

    switch (idx) {
    case 0:  ui->doubleSpinBox_MposX->setValue( list.at(i).toDouble() + m_WCO[i] );  break;
    case 1:  ui->doubleSpinBox_MposY->setValue( list.at(i).toDouble() + m_WCO[i] );  break;
    case 2:  ui->doubleSpinBox_MposZ->setValue( list.at(i).toDouble() + m_WCO[i] );  break;
    case 3:  ui->doubleSpinBox_MposA->setValue( list.at(i).toDouble() + m_WCO[i] );  break;
    case 4:  ui->doubleSpinBox_MposB->setValue( list.at(i).toDouble() + m_WCO[i] );  break;
    case 5:  ui->doubleSpinBox_MposC->setValue( list.at(i).toDouble() + m_WCO[i] );  break;
    }
  }
}

void GrblCommander::parseGrblStatusWCO(const QString & status)
{
  QStringList list = status.split(',');

  for (int i=0; i < list.length(); i++) {
    m_WCO[i] = list.at(i).toDouble();
  }
}


void GrblCommander::parseGrblStatusPins(const QString & status)
{
  // XYZ ABC P D HRS
  foreach (const QChar &pin, QString("XYZABCPDHRS")) {
    if (status.contains(pin) && !m_grblPins.contains(pin)) {
      // Pin activated
      m_grblPins.append(pin);
      switch (pin.toLatin1()) {
      case 'X':
        ui->doubleSpinBox_MposX->setStyleSheet("QDoubleSpinBox { background-color:orangered; }");
        ui->doubleSpinBox_WposX->setStyleSheet("QDoubleSpinBox { background-color:orangered; }");
        break;
      case 'Y':
        ui->doubleSpinBox_MposY->setStyleSheet("QDoubleSpinBox { background-color:orangered; }");
        ui->doubleSpinBox_WposY->setStyleSheet("QDoubleSpinBox { background-color:orangered; }");
        break;
      case 'Z':
        ui->doubleSpinBox_MposZ->setStyleSheet("QDoubleSpinBox { background-color:orangered; }");
        ui->doubleSpinBox_WposZ->setStyleSheet("QDoubleSpinBox { background-color:orangered; }");
        break;
      case 'A':
        ui->doubleSpinBox_MposA->setStyleSheet("QDoubleSpinBox { background-color:red; }");
        ui->doubleSpinBox_WposA->setStyleSheet("QDoubleSpinBox { background-color:red; }");
        break;
      case 'B':
        ui->doubleSpinBox_MposB->setStyleSheet("QDoubleSpinBox { background-color:red; }");
        ui->doubleSpinBox_WposB->setStyleSheet("QDoubleSpinBox { background-color:red; }");
        break;
      case 'C':
        ui->doubleSpinBox_MposC->setStyleSheet("QDoubleSpinBox { background-color:red; }");
        ui->doubleSpinBox_WposC->setStyleSheet("QDoubleSpinBox { background-color:red; }");
        break;

      case 'P':
        break;
      case 'D':
        break;

      case 'H':
        ui->pushButton_hold->setStyleSheet("QPushButton { background-color:cornflowerblue; }");
        ui->pushButton_hold->setDown(true);
        break;
      case 'R':
        ui->pushButton_reset->setStyleSheet("QPushButton { background-color:orangered; }");
        ui->pushButton_reset->setDown(true);
        break;
      case 'S':
        ui->pushButton_resume->setStyleSheet("QPushButton { background-color:limegreen; }");
        ui->pushButton_resume->setDown(true);
        break;
      }
    } else if (!status.contains(pin) && m_grblPins.contains(pin)) {
      // Pin deactivated
      m_grblPins.remove(pin);
      switch (pin.toLatin1()) {
      case 'X':
        ui->doubleSpinBox_MposX->setStyleSheet("");
        ui->doubleSpinBox_WposX->setStyleSheet("");
        break;
      case 'Y':
        ui->doubleSpinBox_MposY->setStyleSheet("");
        ui->doubleSpinBox_WposY->setStyleSheet("");
        break;
      case 'Z':
        ui->doubleSpinBox_MposZ->setStyleSheet("");
        ui->doubleSpinBox_WposZ->setStyleSheet("");
        break;
      case 'A':
        ui->doubleSpinBox_MposA->setStyleSheet("");
        ui->doubleSpinBox_WposA->setStyleSheet("");
        break;
      case 'B':
        ui->doubleSpinBox_MposB->setStyleSheet("");
        ui->doubleSpinBox_WposB->setStyleSheet("");
        break;
      case 'C':
        ui->doubleSpinBox_MposC->setStyleSheet("");
        ui->doubleSpinBox_WposC->setStyleSheet("");
        break;

      case 'P':
        break;
      case 'D':
        break;

      case 'H':
        ui->pushButton_hold->setStyleSheet("");
        ui->pushButton_hold->setDown(false);
        break;
      case 'R':
        ui->pushButton_reset->setStyleSheet("");
        ui->pushButton_reset->setDown(false);
        break;
      case 'S':
        ui->pushButton_resume->setStyleSheet("");
        ui->pushButton_resume->setDown(false);
        break;
      }
    }
  }
}

void GrblCommander::parseGrblStatusOverride(const QString & status)
{
  QStringList list = status.split(',');

  if (list.length() == 3) {
    // ignore Override
  } else
    ui->plainTextEdit_log->appendPlainText(status);
}

void GrblCommander::parseGrblStatusFeedSpeed(const QString & status)
{
  QStringList list = status.split(',');

  if (list.length() == 2) {
      ui->spinBox_feedrate    ->setValue( list.at(0).toInt() );
      ui->spinBox_spindlespeed->setValue( list.at(1).toInt() );
  } else
    ui->plainTextEdit_log->appendPlainText(status);
}

void GrblCommander::parseGrblStatusFeedRate(const QString & status)
{
  QStringList list = status.split(',');

  if (list.length() == 1) {
    ui->spinBox_feedrate->setValue( list.at(0).toInt() );
  } else
    ui->plainTextEdit_log->appendPlainText(status);
}


