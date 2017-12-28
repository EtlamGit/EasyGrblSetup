// Copyright 2017 EtlamGit

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSerialPortInfo>
#include <QStringList>
#include <QObjectList>
#include <QThread>
#include <math.h>

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , m_ok(true)
  , m_command_buffer(0)
{
  ui->setupUi(this);

  m_configWatcher = new ConfigWatcher(ui);

  // connect COM buttons
  connect( ui->toolButton_rescan,  &QPushButton::pressed,
           this,                   &MainWindow::comRescan );

  connect( ui->toolButton_connect, &QPushButton::pressed,
           this,                   &MainWindow::comConnect );

  // connect COM methods
  connect( &m_serialPort, &QSerialPort::readyRead,
           this,          &MainWindow::comHandleReadyRead);
//  connect( &m_serialPort, &QSerialPort::errorOccured,
//           this,          &MainWindow::comHandleError);

  // connect test buttons
  connect( ui->pushButton_softlimit_enable,  &QPushButton::pressed,
           this,                             &MainWindow::buttonEnableSoftlimit );
  connect( ui->pushButton_softlimit_disable, &QPushButton::pressed,
           this,                             &MainWindow::buttonDisableSoftlimit );
  connect( ui->pushButton_hardlimit_enable,  &QPushButton::pressed,
           this,                             &MainWindow::buttonEnableHardlimit );
  connect( ui->pushButton_hardlimit_disable, &QPushButton::pressed,
           this,                             &MainWindow::buttonDisableHardlimit );
  connect( ui->pushButton_homing_enable,     &QPushButton::pressed,
           this,                             &MainWindow::buttonEnableHoming );
  connect( ui->pushButton_homing_disable,    &QPushButton::pressed,
           this,                             &MainWindow::buttonDisableHoming );

  connect( ui->pushButton_testX, &QPushButton::pressed,
           this,                 &MainWindow::buttonDirectionTestX );
  connect( ui->pushButton_testY, &QPushButton::pressed,
           this,                 &MainWindow::buttonDirectionTestY );
  connect( ui->pushButton_testZ, &QPushButton::pressed,
           this,                 &MainWindow::buttonDirectionTestZ );
  connect( ui->pushButton_testA, &QPushButton::pressed,
           this,                 &MainWindow::buttonDirectionTestA );
  connect( ui->pushButton_testB, &QPushButton::pressed,
           this,                 &MainWindow::buttonDirectionTestB );
  connect( ui->pushButton_testC, &QPushButton::pressed,
           this,                 &MainWindow::buttonDirectionTestC );

  connect( ui->toolButton_homeX, &QPushButton::pressed,
           this,                 &MainWindow::buttonHomeX );
  connect( ui->toolButton_homeY, &QPushButton::pressed,
           this,                 &MainWindow::buttonHomeY );
  connect( ui->toolButton_homeZ, &QPushButton::pressed,
           this,                 &MainWindow::buttonHomeZ );
  connect( ui->toolButton_homeA, &QPushButton::pressed,
           this,                 &MainWindow::buttonHomeA );
  connect( ui->toolButton_homeB, &QPushButton::pressed,
           this,                 &MainWindow::buttonHomeB );
  connect( ui->toolButton_homeC, &QPushButton::pressed,
           this,                 &MainWindow::buttonHomeC );

  connect( ui->toolButton_speedX, &QPushButton::pressed,
           this,                  &MainWindow::buttonSpeedTestX );
  connect( ui->toolButton_speedY, &QPushButton::pressed,
           this,                  &MainWindow::buttonSpeedTestY );
  connect( ui->toolButton_speedZ, &QPushButton::pressed,
           this,                  &MainWindow::buttonSpeedTestZ );
  connect( ui->toolButton_speedA, &QPushButton::pressed,
           this,                  &MainWindow::buttonSpeedTestA );
  connect( ui->toolButton_speedB, &QPushButton::pressed,
           this,                  &MainWindow::buttonSpeedTestB );
  connect( ui->toolButton_speedC, &QPushButton::pressed,
           this,                  &MainWindow::buttonSpeedTestC );

  connect( ui->toolButton_unlock, &QToolButton::pressed,
           this,                  &MainWindow::buttonUnlock );
  connect( ui->toolButton_home,   &QToolButton::pressed,
           this,                  &MainWindow::buttonHome );

  // setup status bar
  statusBar()->addWidget( m_label_error_code  = new QLabel("Last Error:") );
  statusBar()->addWidget( m_label_error       = new QLabel("") );
  statusBar()->addWidget( m_label_alarm_code  = new QLabel("Last Alarm:") );
  statusBar()->addWidget( m_label_alarm       = new QLabel("") );

  // general initialization
  comRescan();

  // configure regular status timer
  connect( &m_timer, &QTimer::timeout,
           this,     &MainWindow::handleTimer );
}

MainWindow::~MainWindow()
{
  delete ui;
}


// ---------- ---------- ---------- ---------- ---------- ----------
// Timer
// ---------- ---------- ---------- ---------- ---------- ----------

void MainWindow::handleTimer()
{
  if (ui->label_grbl_version->text() == "0.00") {
    if (m_serialPort.isOpen()) {
      comConnect();  // DISCONNECT
    }
  } else {
    // get current machine status
    requestStatus();

    // put jog commands in queue
    handleJog();

    // write queued commands
    comWriteFromBuffer();

    if (ui->label_grbl_status->text() == "Idle") {

      // write configuration commands when available
      while (m_configWatcher->isCommandAvailable()) {
        comWrite( m_configWatcher->getCommand().toUtf8() );
      }

    }

    m_timer.start(250);
  }
}

void MainWindow::handleJog()
{
  double speed    = 60.0 * std::pow(10.0,ui->dial_jogspeed->value());
  double distance = speed/(60.0*4.0);

  ui->label_jogspeed->setText(QString("%1\nmm/min").arg(speed));

  if (ui->toolButton_jogXminus->isDown()) comAppend(QString("$J=G91 X-%1 F%2").arg(distance).arg(speed));
  if (ui->toolButton_jogXplus ->isDown()) comAppend(QString("$J=G91 X%1 F%2" ).arg(distance).arg(speed));
  if (ui->toolButton_jogYminus->isDown()) comAppend(QString("$J=G91 Y-%1 F%2").arg(distance).arg(speed));
  if (ui->toolButton_jogYplus ->isDown()) comAppend(QString("$J=G91 Y%1 F%2" ).arg(distance).arg(speed));
  if (ui->toolButton_jogZminus->isDown()) comAppend(QString("$J=G91 Z-%1 F%2").arg(distance).arg(speed));
  if (ui->toolButton_jogZplus ->isDown()) comAppend(QString("$J=G91 Z%1 F%2" ).arg(distance).arg(speed));
  if (ui->toolButton_jogAminus->isDown()) comAppend(QString("$J=G91 A-%1 F%2").arg(distance).arg(speed));
  if (ui->toolButton_jogAplus ->isDown()) comAppend(QString("$J=G91 A%1 F%2" ).arg(distance).arg(speed));
  if (ui->toolButton_jogBminus->isDown()) comAppend(QString("$J=G91 B-%1 F%2").arg(distance).arg(speed));
  if (ui->toolButton_jogBplus ->isDown()) comAppend(QString("$J=G91 B%1 F%2" ).arg(distance).arg(speed));
  if (ui->toolButton_jogCminus->isDown()) comAppend(QString("$J=G91 C-%1 F%2").arg(distance).arg(speed));
  if (ui->toolButton_jogCplus ->isDown()) comAppend(QString("$J=G91 C%1 F%2" ).arg(distance).arg(speed));

  // when no buttons are down => realtime cancel running jog operation
  if ( !((ui->toolButton_jogXminus->isDown()) || (ui->toolButton_jogXplus ->isDown()) ||
         (ui->toolButton_jogYminus->isDown()) || (ui->toolButton_jogYplus ->isDown()) ||
         (ui->toolButton_jogZminus->isDown()) || (ui->toolButton_jogZplus ->isDown()) ||
         (ui->toolButton_jogAminus->isDown()) || (ui->toolButton_jogAplus ->isDown()) ||
         (ui->toolButton_jogBminus->isDown()) || (ui->toolButton_jogBplus ->isDown()) ||
         (ui->toolButton_jogCminus->isDown()) || (ui->toolButton_jogCplus ->isDown())) &&
       (ui->label_grbl_status->text() == "Jog")) {
    comWriteRealtime("\x85");
  }
}


// ---------- ---------- ---------- ---------- ---------- ----------
// COM related methods
// ---------- ---------- ---------- ---------- ---------- ----------

void MainWindow::comRescan()
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
  }
}

void MainWindow::comConnect()
{
  if (m_serialPort.isOpen()) { // COM open => DISCONNECT
    m_timer.stop();
    m_serialPort.close();

    ui->toolButton_connect->setText("Connect");
    ui->toolButton_rescan ->setEnabled(true);
    ui->comboBox_com      ->setEnabled(true);

    ui->groupBox_config->setDisabled(true);
    ui->groupBox_status->setDisabled(true);
    ui->scrollAreaWidgetContents->setDisabled(true);
    ui->label_grbl_version->setText("0.00");
    ui->label_grbl_status ->setText("?");

    m_command_buffer = 0;
    m_commands.clear();
  } else {                     // COM closed => CONNECT
    m_serialPort.setPortName( ui->comboBox_com->currentText() );
    m_serialPort.setBaudRate( 115200 );
    m_serialPort.setDataBits( QSerialPort::Data8 );
    m_serialPort.setParity  ( QSerialPort::NoParity );
    m_serialPort.setStopBits( QSerialPort::OneStop );

    if (m_serialPort.open(QIODevice::ReadWrite)) {
      ui->toolButton_connect->setText("Disonnect");
      ui->toolButton_rescan ->setDisabled(true);
      ui->comboBox_com      ->setDisabled(true);

      m_timer.start(500);
    }
  }
  m_configWatcher->reset();
}

void MainWindow::comHandleReadyRead()
{
  m_readData.append(m_serialPort.readAll());

  // process complete lines
  int index = m_readData.indexOf("\r\n");
  while ( index >= 0) {
    parseLine( QString::fromLatin1(m_readData.left(index)) );
    m_readData.remove(0, index+2);
    index = m_readData.indexOf("\r\n");
  }

}

void MainWindow::comHandleError(QSerialPort::SerialPortError serialPortError)
{
  if (serialPortError == QSerialPort::ReadError) {

  }
}


// ---------- ---------- ---------- ---------- ---------- ----------
// COM writing methods
// ---------- ---------- ---------- ---------- ---------- ----------

// standard command always with ok/error responce
bool MainWindow::comWrite(QByteArray command)
{
  int timeout = 1000;

  if (m_serialPort.isOpen()) {
    while (m_command_buffer + command.length() > 127) {
      // busy wait until command buffer has space left
      QThread::msleep(1);
      timeout--;
      if (timeout == 0) {
        ui->plainTextEdit->appendPlainText("COM buffer overflow");
        return false;
      }
    }
    m_command_buffer += command.length();
    m_command_length.append(command.length());
    m_serialPort.write(command,command.length());
    m_serialPort.flush();
    return true;
  }
  return false;
}

// realtime command without ok/error responce
bool MainWindow::comWriteRealtime(QByteArray command)
{
  if (m_serialPort.isOpen()) {
    m_serialPort.write(command,command.length());
    m_serialPort.flush();
    return true;
  }
  return false;
}

void MainWindow::comAppend(QString command)
{
  if (command.right(1) != "\n")
    command.append('\n');
  m_commands.append( command );
}

void MainWindow::comWriteFromBuffer()
{
  // write queued commands
  while ((!m_commands.isEmpty()) &&
         (m_command_buffer + m_commands.at(0).length() <= 128)) {
    QString command(m_commands.takeFirst());
    if ((ui->checkBox_verbose->isChecked()) && (command != "$$\n")) {
      ui->plainTextEdit->appendPlainText(command.left(command.length()-1));
    }
    comWrite( command.toUtf8() );
  }

  // update command buffer label
  if ((m_command_buffer > 0) || (m_commands.length() > 0 )) {
    ui->label_buffer->setText(QString("%1/%2").arg(m_command_buffer).arg(m_commands.length()));
  } else {
    ui->label_buffer->setText("");
  }
}


void MainWindow::requestStatus()
{
  if (m_serialPort.isOpen()) {
    comWriteRealtime("?");
    ui->groupBox_status->setEnabled(true);
  }
}


// ---------- ---------- ---------- ---------- ---------- ----------
// COM parsing methods
// ---------- ---------- ---------- ---------- ---------- ----------

void MainWindow::parseLine(QString line)
{
  if (line.length() == 0) return;
  if (line == "ok") return parseOK();

  if (line.left(5) == "Grbl ")  return parseGrblVersion(line);
  if (line.left(6) == "error:") return parseGrblError(line);
  if (line.left(6) == "ALARM:") return parseGrblAlarm(line);

  if (line.left(1) == "<") return parseGrblStatusList(line);
  if (line.left(1) == "[") return parseGrblMessage(line);
  if (line.left(1) == "$") {
    ui->groupBox_config->setEnabled(true);
    ui->scrollAreaWidgetContents->setEnabled(true);
    return m_configWatcher->parseGrblConfig(line);
  }

  ui->plainTextEdit->appendPlainText(line);
}

void MainWindow::parseOK()
{
  m_ok = true;
  commandCompleted();

  if (m_configWatcher->handleAxesQueryOk())
  {}
}

void MainWindow::commandCompleted()
{
  if (m_command_length.isEmpty()) {
    m_command_buffer = 0;
  } else {
    m_command_buffer -= m_command_length.takeFirst();
  }
}

void MainWindow::parseGrblVersion(QString line)
{
  QStringList list = line.split(' ');
  ui->label_grbl_version->setText(list.at(1));
}

void MainWindow::parseGrblError(QString line)
{
  m_error = true;
  commandCompleted();

  if (m_configWatcher->handleAxesQueryError()) {
    if (ui->checkBox_verbose->isChecked())
      ui->plainTextEdit->appendPlainText(line);

    QStringList list = line.split(':');
    m_label_error_code->setText(QString("Error:%1").arg(list.at(1)));
    switch (list.at(1).toInt()) {
    case  1:  m_label_error->setText("G-code words consist of a letter and a value. Letter was not found.");  break;
    case  2:  m_label_error->setText("Numeric value format is not valid or missing an expected value.");  break;
    case  3:  m_label_error->setText("Grbl '$' system command was not recognized or supported.");  break;
    case  4:  m_label_error->setText("Negative value received for an expected positive value.");  break;
    case  5:  m_label_error->setText("Homing cycle is not enabled via settings.");  break;
    case  6:  m_label_error->setText("Minimum step pulse time must be greater than 3usec");  break;
    case  7:  m_label_error->setText("EEPROM read failed. Reset and restored to default values.");  break;
    case  8:  m_label_error->setText("Grbl '$' command cannot be used unless Grbl is IDLE. Ensures smooth operation during a job.");  break;
    case  9:  m_label_error->setText("G-code locked out during alarm or jog state");  break;
    case 10:  m_label_error->setText("Soft limits cannot be enabled without homing also enabled.");  break;
    case 11:  m_label_error->setText("Max characters per line exceeded. Line was not processed and executed.");  break;
    case 12:  m_label_error->setText("(Compile Option) Grbl '$' setting value exceeds the maximum step rate supported.");  break;
    case 13:  m_label_error->setText("Safety door detected as opened and door state initiated.");  break;
    case 14:  m_label_error->setText("(Grbl-Mega Only) Build info or startup line exceeded EEPROM line length limit.");  break;
    case 15:  m_label_error->setText("Jog target exceeds machine travel. Command ignored.");  break;
    case 16:  m_label_error->setText("Jog command with no '=' or contains prohibited g-code.");  break;
    case 17:  m_label_error->setText("Laser mode requires PWM output.");  break;
    case 20:  m_label_error->setText("Unsupported or invalid g-code command found in block.");  break;
    case 21:  m_label_error->setText("More than one g-code command from same modal group found in block.");  break;
    case 22:  m_label_error->setText("Feed rate has not yet been set or is undefined.");  break;
    case 23:  m_label_error->setText("G-code command in block requires an integer value.");  break;
    case 24:  m_label_error->setText("Two G-code commands that both require the use of the XYZ axis words were detected in the block.");  break;
    case 25:  m_label_error->setText("A G-code word was repeated in the block.");  break;
    case 26:  m_label_error->setText("A G-code command implicitly or explicitly requires XYZ axis words in the block, but none were detected.");  break;
    case 28:  m_label_error->setText("A G-code command was sent, but is missing some required P or L value words in the line.");  break;
    case 29:  m_label_error->setText("Grbl supports six work coordinate systems G54-G59. G59.1, G59.2, and G59.3 are not supported.");  break;
    case 30:  m_label_error->setText("The G53 G-code command requires either a G0 seek or G1 feed motion mode to be active. A different motion was active.");  break;
    case 31:  m_label_error->setText("There are unused axis words in the block and G80 motion mode cancel is active.");  break;
    case 32:  m_label_error->setText("A G2 or G3 arc was commanded but there are no XYZ axis words in the selected plane to trace the arc.");  break;
    case 33:  m_label_error->setText("The motion command has an invalid target. G2, G3, and G38.2 generates this error, if the arc is impossible to generate or if the probe target is the current position.");  break;
    case 34:  m_label_error->setText("A G2 or G3 arc, traced with the radius definition, had a mathematical error when computing the arc geometry. Try either breaking up the arc into semi-circles or quadrants, or redefine them with the arc offset definition.");  break;
    case 35:  m_label_error->setText("A G2 or G3 arc, traced with the offset definition, is missing the IJK offset word in the selected plane to trace the arc.");  break;
    case 36:  m_label_error->setText("There are unused, leftover G-code words that aren't used by any command in the block.");  break;
    case 37:  m_label_error->setText("The G43.1 dynamic tool length offset command cannot apply an offset to an axis other than its configured axis. The Grbl default axis is the Z-axis.");  break;
    }
  }
}

void MainWindow::parseGrblAlarm(QString line)
{
  if (ui->checkBox_verbose->isChecked())
    ui->plainTextEdit->appendPlainText(line);

  QStringList list = line.split(':');
  m_label_error_code->setText(QString("Alarm:%1").arg(list.at(1)));
  switch (list.at(1).toInt()) {
  case  1:  m_label_alarm->setText("Hard limit triggered. Machine position is likely lost due to sudden and immediate halt. Re-homing is highly recommended.");  break;
  case  2:  m_label_alarm->setText("G-code motion target exceeds machine travel. Machine position safely retained. Alarm may be unlocked.");  break;
  case  3:  m_label_alarm->setText("Reset while in motion. Grbl cannot guarantee position. Lost steps are likely. Re-homing is highly recommended.");  break;
  case  4:  m_label_alarm->setText("Probe fail. The probe is not in the expected initial state before starting probe cycle, where G38.2 and G38.3 is not triggered and G38.4 and G38.5 is triggered.");  break;
  case  5:  m_label_alarm->setText("Probe fail. Probe did not contact the workpiece within the programmed travel for G38.2 and G38.4.");  break;
  case  6:  m_label_alarm->setText("Homing fail. Reset during active homing cycle.");  break;
  case  7:  m_label_alarm->setText("Homing fail. Safety door was opened during active homing cycle.");  break;
  case  8:  m_label_alarm->setText("Homing fail. Cycle failed to clear limit switch when pulling off. Try increasing pull-off setting or check wiring.");  break;
  case  9:  m_label_alarm->setText("Homing fail. Could not find limit switch within search distance. Defined as 1.5 * max_travel on search and 5 * pulloff on locate phases.");  break;
  }
}

void MainWindow::parseGrblMessage(QString line)
{
  ui->plainTextEdit->appendPlainText(line);
}

void MainWindow::parseGrblStatusList(QString line)
{
  line = line.mid(1,line.length()-2); // remove <>
  QStringList list   = line.split('|');

  showGrblStatus(list.takeFirst());

  foreach (const QString &pos, list) {
    parseGrblStatus(pos);
  }

  if (list.filter("Pn:").length() == 0) {
     parseGrblStatusPins("");
  }
}

void MainWindow::showGrblStatus(QString status)
{
  QStringList list = status.split(':');

  // copy status to label
  ui->label_grbl_status->setText(status);
  // do some color tweaking based on status
  if        (list.at(0) == "Idle") {
    ui->label_grbl_status->setStyleSheet("");
    ui->toolButton_unlock->setEnabled(false);
  } else if (list.at(0) == "Run") {
    ui->label_grbl_status->setStyleSheet("QLabel { background-color:green; }");
  } else if (list.at(0) == "Hold") {
    ui->label_grbl_status->setStyleSheet("QLabel { background-color:blue; }");
  } else if (list.at(0) == "Jog") {
    ui->label_grbl_status->setStyleSheet("QLabel { background-color:cyan; }");
  } else if (list.at(0) == "Alarm") {
    ui->label_grbl_status->setStyleSheet("QLabel { background-color:red; }");
    ui->toolButton_unlock->setEnabled(true);
  } else if (list.at(0) == "Door") {
  } else if (list.at(0) == "Check") {
  } else if (list.at(0) == "Home") {
    ui->label_grbl_status->setStyleSheet("QLabel { background-color:yellow; }");
  } else if (list.at(0) == "Sleep") {
    ui->label_grbl_status->setStyleSheet("QLabel { background-color:darkgray; }");
  }

}

void MainWindow::parseGrblStatus(QString pos)
{
  QStringList plist = pos.split(':');

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

  ui->plainTextEdit->appendPlainText(pos);
}

void MainWindow::parseGrblStatusMPos(QString status)
{
//  if (m_axes_request || m_axes_query) return;  // do not parse during axes test !

  QStringList list = status.split(',');

  for (int i=0; i < list.length(); i++) {
    int idx = m_configWatcher->m_reg2axes[i];

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

void MainWindow::parseGrblStatusWPos(QString status)
{
//  if (m_axes_request || m_axes_query) return;  // do not parse during axes test !

  QStringList list = status.split(',');

  for (int i=0; i < list.length(); i++) {
    int idx = m_configWatcher->m_reg2axes[i];

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

void MainWindow::parseGrblStatusWCO(QString status)
{
//  if (m_axes_request || m_axes_query) return;  // do not parse during axes test !

  QStringList list = status.split(',');

  for (int i=0; i < list.length(); i++) {
    m_WCO[i] = list.at(i).toDouble();
  }
}


void MainWindow::parseGrblStatusPins(QString status)
{
  // XYZ ABC P DHRS
  if (status.contains('X')) ui->label_pinX->setStyleSheet("QLabel { background-color:red; }");
  else                      ui->label_pinX->setStyleSheet("");
  if (status.contains('Y')) ui->label_pinY->setStyleSheet("QLabel { background-color:red; }");
  else                      ui->label_pinY->setStyleSheet("");
  if (status.contains('Z')) ui->label_pinZ->setStyleSheet("QLabel { background-color:red; }");
  else                      ui->label_pinZ->setStyleSheet("");
  if (status.contains('A')) ui->label_pinA->setStyleSheet("QLabel { background-color:red; }");
  else                      ui->label_pinA->setStyleSheet("");
  if (status.contains('B')) ui->label_pinB->setStyleSheet("QLabel { background-color:red; }");
  else                      ui->label_pinB->setStyleSheet("");
  if (status.contains('C')) ui->label_pinC->setStyleSheet("QLabel { background-color:red; }");
  else                      ui->label_pinC->setStyleSheet("");
  if (status.contains('P')) ui->label_pinProbe->setStyleSheet("QLabel { background-color:red; }");
  else                      ui->label_pinProbe->setStyleSheet("");
  if (status.contains('D')) ui->label_pinDoor ->setStyleSheet("QLabel { background-color:red; }");
  else                      ui->label_pinDoor ->setStyleSheet("");
  if (status.contains('H')) ui->label_pinHold ->setStyleSheet("QLabel { background-color:red; }");
  else                      ui->label_pinHold ->setStyleSheet("");
  if (status.contains('R')) ui->label_pinReset->setStyleSheet("QLabel { background-color:red; }");
  else                      ui->label_pinReset->setStyleSheet("");
  if (status.contains('S')) ui->label_pinStart->setStyleSheet("QLabel { background-color:red; }");
  else                      ui->label_pinStart->setStyleSheet("");
}

void MainWindow::parseGrblStatusOverride(QString status)
{
  QStringList list = status.split(',');

  if (list.length() == 3) {
    // ignore Override
  } else
    ui->plainTextEdit->appendPlainText(status);
}

void MainWindow::parseGrblStatusFeedSpeed(QString status)
{
  QStringList list = status.split(',');

  if (list.length() == 2) {
      ui->spinBox_feedrate   ->setValue( list.at(0).toInt() );
      ui->spinBox_spindlerate->setValue( list.at(1).toInt() );
  } else
    ui->plainTextEdit->appendPlainText(status);
}

void MainWindow::parseGrblStatusFeedRate(QString status)
{
  QStringList list = status.split(',');

  if (list.length() == 1) {
    ui->spinBox_feedrate->setValue( list.at(0).toInt() );
  } else
    ui->plainTextEdit->appendPlainText(status);
}


// ---------- ---------- ---------- ---------- ---------- ----------
// button event handlers
// ---------- ---------- ---------- ---------- ---------- ----------

void MainWindow::buttonEnableSoftlimit()  { comAppend("$20=1\n"); }
void MainWindow::buttonDisableSoftlimit() { comAppend("$20=0\n"); }
void MainWindow::buttonEnableHardlimit()  { comAppend("$21=1\n"); }
void MainWindow::buttonDisableHardlimit() { comAppend("$21=0\n"); }
void MainWindow::buttonEnableHoming()     { comAppend("$22=1\n"); }
void MainWindow::buttonDisableHoming()    { comAppend("$22=0\n"); }

void MainWindow::buttonUnlock()
{
  comAppend("$X\n");
  m_label_alarm_code->setText("Unlocked");
  m_label_alarm->setText("");
}

void MainWindow::buttonHome()
{
  if (m_configWatcher->isConfigured()) {
    showGrblStatus("Home");
    comAppend("$H\n");
  } else buttonUnlock();
}


void MainWindow::buttonDirectionTestX()
{
  if (ui->label_grbl_status->text() == "Idle") {
    for (int i = 0; i<5; i++) {
      comAppend("G91 G1 X+20 F300\n");
      comAppend("G91 G0 X-20\n");
    }
  }
}

void MainWindow::buttonDirectionTestY()
{
  if (ui->label_grbl_status->text() == "Idle") {
    for (int i = 0; i<5; i++) {
      comAppend("G91 G1 Y+20 F300\n");
      comAppend("G91 G0 Y-20\n");
    }
  }
}

void MainWindow::buttonDirectionTestZ()
{
  if (ui->label_grbl_status->text() == "Idle") {
    for (int i = 0; i<5; i++) {
      comAppend("G91 G1 Z+20 F300\n");
      comAppend("G91 G0 Z-20\n");
    }
  }
}

void MainWindow::buttonDirectionTestA()
{
  if (ui->label_grbl_status->text() == "Idle") {
    for (int i = 0; i<5; i++) {
      comAppend("G1 A+20 F300\n");
      comAppend("G0 A0\n");
    }
  }
}

void MainWindow::buttonDirectionTestB()
{
  if (ui->label_grbl_status->text() == "Idle") {
    for (int i = 0; i<5; i++) {
      comAppend("G1 B+20 F300\n");
      comAppend("G0 B0\n");
    }
  }
}

void MainWindow::buttonDirectionTestC()
{
  if (ui->label_grbl_status->text() == "Idle") {
    for (int i = 0; i<5; i++) {
      comAppend("G1 C+20 F300\n");
      comAppend("G0 C0\n");
    }
  }
}


void MainWindow::buttonHomeX()
{
  if (ui->checkBox_Xplus->isChecked()) {
    ui->checkBox_23X->setChecked(false);
  } else if (ui->checkBox_Xminus->isChecked()) {
    ui->checkBox_23X->setChecked(true);
  } else return;
  // run home cycle only when a limit switch is mounted
  showGrblStatus("Home");
  comAppend("$hx\n");
}

void MainWindow::buttonHomeY()
{
  if (ui->checkBox_Yplus->isChecked()) {
    ui->checkBox_23Y->setChecked(false);
  } else if (ui->checkBox_Yminus->isChecked()) {
    ui->checkBox_23Y->setChecked(true);
  } else return;
  // run home cycle only when a limit switch is mounted
  showGrblStatus("Home");
  comAppend("$hy\n");
}

void MainWindow::buttonHomeZ()
{
  if (ui->checkBox_Zplus->isChecked()) {
    ui->checkBox_23Z->setChecked(false);
  } else if (ui->checkBox_Zminus->isChecked()) {
    ui->checkBox_23Z->setChecked(true);
  } else return;
  // run home cycle only when a limit switch is mounted
  showGrblStatus("Home");
  comAppend("$hz\n");
}

void MainWindow::buttonHomeA()
{
  if (ui->checkBox_Aplus->isChecked()) {
    ui->checkBox_23A->setChecked(false);
  } else if (ui->checkBox_Aminus->isChecked()) {
    ui->checkBox_23A->setChecked(true);
  } else return;
  // run home cycle only when a limit switch is mounted
  showGrblStatus("Home");
  comAppend("$ha\n");
}

void MainWindow::buttonHomeB()
{
  if (ui->checkBox_Bplus->isChecked()) {
    ui->checkBox_23B->setChecked(false);
  } else if (ui->checkBox_Bminus->isChecked()) {
    ui->checkBox_23B->setChecked(true);
  } else return;
  // run home cycle only when a limit switch is mounted
  showGrblStatus("Home");
  comAppend("$hb\n");
}

void MainWindow::buttonHomeC()
{
  if (ui->checkBox_Cplus->isChecked()) {
    ui->checkBox_23C->setChecked(false);
  } else if (ui->checkBox_Cminus->isChecked()) {
    ui->checkBox_23C->setChecked(true);
  } else return;
  // run home cycle only when a limit switch is mounted
  showGrblStatus("Home");
  comAppend("$hc\n");
}


void MainWindow::buttonSpeedTestX()
{
  if ((ui->label_grbl_status->text() == "Idle") &&
      (ui->checkBox_20->isChecked()) && (ui->checkBox_21->isChecked()) && (ui->checkBox_22->isChecked())) {
    int maxspeed = ui->doubleSpinBox_110X->value();
    int speed = std::max(500, maxspeed - 5000);
    int move  = 10 - ui->doubleSpinBox_130X->value();
    comAppend("G1 X-10 F1000\n");
    for (int i = 0; speed <= maxspeed; i++) {
      comAppend(QString("G1 X%1 F%2\n").arg(move).arg(speed));
      comAppend(QString("G1 X%1 F%2\n").arg(-10).arg(speed));
      speed += 500;
    }
  }
}

void MainWindow::buttonSpeedTestY()
{
  if ((ui->label_grbl_status->text() == "Idle") &&
      (ui->checkBox_20->isChecked()) && (ui->checkBox_21->isChecked()) && (ui->checkBox_22->isChecked())) {
    int maxspeed = ui->doubleSpinBox_110Y->value();
    int speed = std::max(500, maxspeed - 5000);
    int move  = 10 - ui->doubleSpinBox_130Y->value();
    comAppend("G1 Y-10 F1000\n");
    for (int i = 0; speed <= maxspeed; i++) {
      comAppend(QString("G1 Y%1 F%2\n").arg(move).arg(speed));
      comAppend(QString("G1 Y%1 F%2\n").arg(-10).arg(speed));
      speed += 500;
    }
  }
}

void MainWindow::buttonSpeedTestZ()
{
  if ((ui->label_grbl_status->text() == "Idle") &&
      (ui->checkBox_20->isChecked()) && (ui->checkBox_21->isChecked()) && (ui->checkBox_22->isChecked())) {
    int maxspeed = ui->doubleSpinBox_110Z->value();
    int speed = std::max(500, maxspeed - 5000);
    int move  = 10 - ui->doubleSpinBox_130Z->value();
    comAppend("G1 Z-10 F1000\n");
    for (int i = 0; speed <= maxspeed; i++) {
      comAppend(QString("G1 Z%1 F%2\n").arg(move).arg(speed));
      comAppend(QString("G1 Z%1 F%2\n").arg(-10).arg(speed));
      speed += 500;
    }
  }
}

void MainWindow::buttonSpeedTestA()
{
  if ((ui->label_grbl_status->text() == "Idle") &&
      (ui->checkBox_20->isChecked()) && (ui->checkBox_21->isChecked()) && (ui->checkBox_22->isChecked())) {
    int maxspeed = ui->doubleSpinBox_110A->value();
    int speed = std::max(500, maxspeed - 5000);
    int move  = 10 - ui->doubleSpinBox_130A->value();
    comAppend("G1 A-10 F1000\n");
    for (int i = 0; speed <= maxspeed; i++) {
      comAppend(QString("G1 A%1 F%2\n").arg(move).arg(speed));
      comAppend(QString("G1 A%1 F%2\n").arg(-10).arg(speed));
      speed += 500;
    }
  }
}

void MainWindow::buttonSpeedTestB()
{
  if ((ui->label_grbl_status->text() == "Idle") &&
      (ui->checkBox_20->isChecked()) && (ui->checkBox_21->isChecked()) && (ui->checkBox_22->isChecked())) {
    int maxspeed = ui->doubleSpinBox_110B->value();
    int speed = std::max(500, maxspeed - 5000);
    int move  = 10 - ui->doubleSpinBox_130B->value();
    comAppend("G1 B-10 F1000\n");
    for (int i = 0; speed <= maxspeed; i++) {
      comAppend(QString("G1 B%1 F%2\n").arg(move).arg(speed));
      comAppend(QString("G1 B%1 F%2\n").arg(-10).arg(speed));
      speed += 500;
    }
  }
}

void MainWindow::buttonSpeedTestC()
{
  if ((ui->label_grbl_status->text() == "Idle") &&
      (ui->checkBox_20->isChecked()) && (ui->checkBox_21->isChecked()) && (ui->checkBox_22->isChecked())) {
    int maxspeed = ui->doubleSpinBox_110C->value();
    int speed = std::max(500, maxspeed - 5000);
    int move  = 10 - ui->doubleSpinBox_130C->value();
    comAppend("G1 C-10 F1000\n");
    for (int i = 0; speed <= maxspeed; i++) {
      comAppend(QString("G1 C%1 F%2\n").arg(move).arg(speed));
      comAppend(QString("G1 C%1 F%2\n").arg(-10).arg(speed));
      speed += 500;
    }
  }
}
