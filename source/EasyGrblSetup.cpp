// Copyright 2017 EtlamGit

#include "EasyGrblSetup.h"
#include "ui_EasyGrblSetup.h"

#include <QSerialPortInfo>
#include <QStringList>
#include <QObjectList>
#include <QThread>
#include <math.h>

EasyGrblSetup::EasyGrblSetup(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::EasyGrblSetup)
{
  ui->setupUi(this);

  m_configWatcher = new ConfigWatcher(ui);
  m_axesHide      = new AxesVisibility(ui);
  m_grblCom       = new GrblCommander(ui);
  m_jogController = new JogController(ui);


  // axes identification
  connect( m_grblCom,       &GrblCommander::foundAxis,
           m_configWatcher, &ConfigWatcher::setAxis);
  connect( m_grblCom,       &GrblCommander::foundAxis,
           m_axesHide,      &AxesVisibility::axisEnable);

  // configuration updates
  connect( m_grblCom,       &GrblCommander::receivedSetting,
           m_configWatcher, &ConfigWatcher::parseGrblSetting);
  connect( m_configWatcher, &ConfigWatcher::writeConfigCommand,
           m_grblCom,       &GrblCommander::appendCommand);

  // buttons: Connect & Disconnect & Rescan
  connect( ui->toolButton_connect,    &QToolButton::pressed,
           this,                      &EasyGrblSetup::buttonConnect);
  connect( ui->toolButton_disconnect, &QToolButton::pressed,
           this,                      &EasyGrblSetup::buttonDisconnect);
  connect( ui->toolButton_rescan,     &QToolButton::pressed,
           m_grblCom,                 &GrblCommander::comRescan);

  // buttons: Unlock & Home
  connect( ui->toolButton_unlock,     &QToolButton::pressed,
           this,                      &EasyGrblSetup::buttonUnlock);
  connect( ui->toolButton_home,       &QToolButton::pressed,
           this,                      &EasyGrblSetup::buttonHome);

  // buttons: Rest & Hold & Resume
  connect( ui->toolButton_reset,      &QToolButton::pressed,
           this,                      &EasyGrblSetup::buttonReset);
  connect( ui->toolButton_hold,       &QToolButton::pressed,
           this,                      &EasyGrblSetup::buttonHold);
  connect( ui->toolButton_resume,     &QToolButton::pressed,
           this,                      &EasyGrblSetup::buttonResume);

  // logging
  connect( m_grblCom,             &GrblCommander::writingCommand,
           this,                  &EasyGrblSetup::verboseLog);
  connect( m_grblCom,             &GrblCommander::receivedMessage,
           ui->plainTextEdit_log, &QPlainTextEdit::appendPlainText);


  // Jogging
  connect( m_grblCom,       &GrblCommander::receivedJogOK,
           m_jogController, &JogController::jogLoop);
  connect( m_jogController, &JogController::jogCommand,
           m_grblCom,       &GrblCommander::appendCommand);
  connect( m_jogController, &JogController::jogStop,
           m_grblCom,       &GrblCommander::jogStop);

  // set ZERO "buttons"
  connect( ui->label_X, &QLabelClickable::clicked,
           this,        &EasyGrblSetup::setZeroX);
  connect( ui->label_Y, &QLabelClickable::clicked,
           this,        &EasyGrblSetup::setZeroY);
  connect( ui->label_Z, &QLabelClickable::clicked,
           this,        &EasyGrblSetup::setZeroZ);
  connect( ui->label_A, &QLabelClickable::clicked,
           this,        &EasyGrblSetup::setZeroA);
  connect( ui->label_B, &QLabelClickable::clicked,
           this,        &EasyGrblSetup::setZeroB);
  connect( ui->label_C, &QLabelClickable::clicked,
           this,        &EasyGrblSetup::setZeroC);


  // connect test buttons
  connect( ui->pushButton_softlimit_enable,  &QPushButton::pressed,
           this,                             &EasyGrblSetup::buttonEnableSoftlimit );
  connect( ui->pushButton_softlimit_disable, &QPushButton::pressed,
           this,                             &EasyGrblSetup::buttonDisableSoftlimit );
  connect( ui->pushButton_hardlimit_enable,  &QPushButton::pressed,
           this,                             &EasyGrblSetup::buttonEnableHardlimit );
  connect( ui->pushButton_hardlimit_disable, &QPushButton::pressed,
           this,                             &EasyGrblSetup::buttonDisableHardlimit );
  connect( ui->pushButton_homing_enable,     &QPushButton::pressed,
           this,                             &EasyGrblSetup::buttonEnableHoming );
  connect( ui->pushButton_homing_disable,    &QPushButton::pressed,
           this,                             &EasyGrblSetup::buttonDisableHoming );

  connect( ui->pushButton_testX, &QPushButton::pressed,
           this,                 &EasyGrblSetup::buttonDirectionTestX );
  connect( ui->pushButton_testY, &QPushButton::pressed,
           this,                 &EasyGrblSetup::buttonDirectionTestY );
  connect( ui->pushButton_testZ, &QPushButton::pressed,
           this,                 &EasyGrblSetup::buttonDirectionTestZ );
  connect( ui->pushButton_testA, &QPushButton::pressed,
           this,                 &EasyGrblSetup::buttonDirectionTestA );
  connect( ui->pushButton_testB, &QPushButton::pressed,
           this,                 &EasyGrblSetup::buttonDirectionTestB );
  connect( ui->pushButton_testC, &QPushButton::pressed,
           this,                 &EasyGrblSetup::buttonDirectionTestC );

  connect( ui->toolButton_homeX, &QPushButton::pressed,
           this,                 &EasyGrblSetup::buttonHomeX );
  connect( ui->toolButton_homeY, &QPushButton::pressed,
           this,                 &EasyGrblSetup::buttonHomeY );
  connect( ui->toolButton_homeZ, &QPushButton::pressed,
           this,                 &EasyGrblSetup::buttonHomeZ );
  connect( ui->toolButton_homeA, &QPushButton::pressed,
           this,                 &EasyGrblSetup::buttonHomeA );
  connect( ui->toolButton_homeB, &QPushButton::pressed,
           this,                 &EasyGrblSetup::buttonHomeB );
  connect( ui->toolButton_homeC, &QPushButton::pressed,
           this,                 &EasyGrblSetup::buttonHomeC );

  connect( ui->toolButton_speedX, &QPushButton::pressed,
           this,                  &EasyGrblSetup::buttonSpeedTestX );
  connect( ui->toolButton_speedY, &QPushButton::pressed,
           this,                  &EasyGrblSetup::buttonSpeedTestY );
  connect( ui->toolButton_speedZ, &QPushButton::pressed,
           this,                  &EasyGrblSetup::buttonSpeedTestZ );
  connect( ui->toolButton_speedA, &QPushButton::pressed,
           this,                  &EasyGrblSetup::buttonSpeedTestA );
  connect( ui->toolButton_speedB, &QPushButton::pressed,
           this,                  &EasyGrblSetup::buttonSpeedTestB );
  connect( ui->toolButton_speedC, &QPushButton::pressed,
           this,                  &EasyGrblSetup::buttonSpeedTestC );

  // setup status bar
  statusBar()->addWidget( m_errorLabel = new QLabel("") );
  statusBar()->addWidget( m_alarmLabel = new QLabel("") );
  connect( m_grblCom, &GrblCommander::receivedError,
           this,      &EasyGrblSetup::decodeError);
  connect( m_grblCom, &GrblCommander::receivedAlarm,
           this,      &EasyGrblSetup::decodeAlarm);

  createContextMenus();
}

EasyGrblSetup::~EasyGrblSetup()
{
  delete ui;
}

// ---------- ---------- ---------- ---------- ---------- ---------- ---------- ----------
// Logging
// ---------- ---------- ---------- ---------- ---------- ---------- ---------- ----------

void EasyGrblSetup::normalLog(QString text)
{
  ui->plainTextEdit_log->appendPlainText(text);
}


void EasyGrblSetup::verboseLog(QString text)
{
  if (ui->checkBox_verbose->isChecked()) {
    ui->plainTextEdit_log->appendPlainText(text);
  }
}


// ---------- ---------- ---------- ---------- ---------- ----------
// parse Error & Alarm messages to human readable text
// ---------- ---------- ---------- ---------- ---------- ----------

void EasyGrblSetup::decodeError(int code)
{
  normalLog(QString("Error:%1").arg(code));
  switch (code) {
  case  1:  m_errorLabel->setText(QString("Error:%1 ").arg(code)+"G-code words consist of a letter and a value. Letter was not found.");  break;
  case  2:  m_errorLabel->setText(QString("Error:%1 ").arg(code)+"Numeric value format is not valid or missing an expected value.");  break;
  case  3:  m_errorLabel->setText(QString("Error:%1 ").arg(code)+"Grbl '$' system command was not recognized or supported.");  break;
  case  4:  m_errorLabel->setText(QString("Error:%1 ").arg(code)+"Negative value received for an expected positive value.");  break;
  case  5:  m_errorLabel->setText(QString("Error:%1 ").arg(code)+"Homing cycle is not enabled via settings.");  break;
  case  6:  m_errorLabel->setText(QString("Error:%1 ").arg(code)+"Minimum step pulse time must be greater than 3usec");  break;
  case  7:  m_errorLabel->setText(QString("Error:%1 ").arg(code)+"EEPROM read failed. Reset and restored to default values.");  break;
  case  8:  m_errorLabel->setText(QString("Error:%1 ").arg(code)+"Grbl '$' command cannot be used unless Grbl is IDLE. Ensures smooth operation during a job.");  break;
  case  9:  m_errorLabel->setText(QString("Error:%1 ").arg(code)+"G-code locked out during alarm or jog state");  break;
  case 10:  m_errorLabel->setText(QString("Error:%1 ").arg(code)+"Soft limits cannot be enabled without homing also enabled.");  break;
  case 11:  m_errorLabel->setText(QString("Error:%1 ").arg(code)+"Max characters per line exceeded. Line was not processed and executed.");  break;
  case 12:  m_errorLabel->setText(QString("Error:%1 ").arg(code)+"(Compile Option) Grbl '$' setting value exceeds the maximum step rate supported.");  break;
  case 13:  m_errorLabel->setText(QString("Error:%1 ").arg(code)+"Safety door detected as opened and door state initiated.");  break;
  case 14:  m_errorLabel->setText(QString("Error:%1 ").arg(code)+"(Grbl-Mega Only) Build info or startup line exceeded EEPROM line length limit.");  break;
  case 15:  m_errorLabel->setText(QString("Error:%1 ").arg(code)+"Jog target exceeds machine travel. Command ignored.");  break;
  case 16:  m_errorLabel->setText(QString("Error:%1 ").arg(code)+"Jog command with no '=' or contains prohibited g-code.");  break;
  case 17:  m_errorLabel->setText(QString("Error:%1 ").arg(code)+"Laser mode requires PWM output.");  break;
  case 20:  m_errorLabel->setText(QString("Error:%1 ").arg(code)+"Unsupported or invalid g-code command found in block.");  break;
  case 21:  m_errorLabel->setText(QString("Error:%1 ").arg(code)+"More than one g-code command from same modal group found in block.");  break;
  case 22:  m_errorLabel->setText(QString("Error:%1 ").arg(code)+"Feed rate has not yet been set or is undefined.");  break;
  case 23:  m_errorLabel->setText(QString("Error:%1 ").arg(code)+"G-code command in block requires an integer value.");  break;
  case 24:  m_errorLabel->setText(QString("Error:%1 ").arg(code)+"Two G-code commands that both require the use of the XYZ axis words were detected in the block.");  break;
  case 25:  m_errorLabel->setText(QString("Error:%1 ").arg(code)+"A G-code word was repeated in the block.");  break;
  case 26:  m_errorLabel->setText(QString("Error:%1 ").arg(code)+"A G-code command implicitly or explicitly requires XYZ axis words in the block, but none were detected.");  break;
  case 28:  m_errorLabel->setText(QString("Error:%1 ").arg(code)+"A G-code command was sent, but is missing some required P or L value words in the line.");  break;
  case 29:  m_errorLabel->setText(QString("Error:%1 ").arg(code)+"Grbl supports six work coordinate systems G54-G59. G59.1, G59.2, and G59.3 are not supported.");  break;
  case 30:  m_errorLabel->setText(QString("Error:%1 ").arg(code)+"The G53 G-code command requires either a G0 seek or G1 feed motion mode to be active. A different motion was active.");  break;
  case 31:  m_errorLabel->setText(QString("Error:%1 ").arg(code)+"There are unused axis words in the block and G80 motion mode cancel is active.");  break;
  case 32:  m_errorLabel->setText(QString("Error:%1 ").arg(code)+"A G2 or G3 arc was commanded but there are no XYZ axis words in the selected plane to trace the arc.");  break;
  case 33:  m_errorLabel->setText(QString("Error:%1 ").arg(code)+"The motion command has an invalid target. G2, G3, and G38.2 generates this error, if the arc is impossible to generate or if the probe target is the current position.");  break;
  case 34:  m_errorLabel->setText(QString("Error:%1 ").arg(code)+"A G2 or G3 arc, traced with the radius definition, had a mathematical error when computing the arc geometry. Try either breaking up the arc into semi-circles or quadrants, or redefine them with the arc offset definition.");  break;
  case 35:  m_errorLabel->setText(QString("Error:%1 ").arg(code)+"A G2 or G3 arc, traced with the offset definition, is missing the IJK offset word in the selected plane to trace the arc.");  break;
  case 36:  m_errorLabel->setText(QString("Error:%1 ").arg(code)+"There are unused, leftover G-code words that aren't used by any command in the block.");  break;
  case 37:  m_errorLabel->setText(QString("Error:%1 ").arg(code)+"The G43.1 dynamic tool length offset command cannot apply an offset to an axis other than its configured axis. The Grbl default axis is the Z-axis.");  break;
  }
}

void EasyGrblSetup::decodeAlarm(int code)
{
  switch (code) {
  case  1:  m_alarmLabel->setText(QString("Alarm:%1 ").arg(code)+"Hard limit triggered. Machine position is likely lost due to sudden and immediate halt. Re-homing is highly recommended.");  break;
  case  2:  m_alarmLabel->setText(QString("Alarm:%1 ").arg(code)+"G-code motion target exceeds machine travel. Machine position safely retained. Alarm may be unlocked.");  break;
  case  3:  m_alarmLabel->setText(QString("Alarm:%1 ").arg(code)+"Reset while in motion. Grbl cannot guarantee position. Lost steps are likely. Re-homing is highly recommended.");  break;
  case  4:  m_alarmLabel->setText(QString("Alarm:%1 ").arg(code)+"Probe fail. The probe is not in the expected initial state before starting probe cycle, where G38.2 and G38.3 is not triggered and G38.4 and G38.5 is triggered.");  break;
  case  5:  m_alarmLabel->setText(QString("Alarm:%1 ").arg(code)+"Probe fail. Probe did not contact the workpiece within the programmed travel for G38.2 and G38.4.");  break;
  case  6:  m_alarmLabel->setText(QString("Alarm:%1 ").arg(code)+"Homing fail. Reset during active homing cycle.");  break;
  case  7:  m_alarmLabel->setText(QString("Alarm:%1 ").arg(code)+"Homing fail. Safety door was opened during active homing cycle.");  break;
  case  8:  m_alarmLabel->setText(QString("Alarm:%1 ").arg(code)+"Homing fail. Cycle failed to clear limit switch when pulling off. Try increasing pull-off setting or check wiring.");  break;
  case  9:  m_alarmLabel->setText(QString("Alarm:%1 ").arg(code)+"Homing fail. Could not find limit switch within search distance. Defined as 1.5 * max_travel on search and 5 * pulloff on locate phases.");  break;
  }
}


// ---------- ---------- ---------- ---------- ---------- ----------
// button event handlers
// ---------- ---------- ---------- ---------- ---------- ----------

void EasyGrblSetup::buttonConnect()
{
  m_grblCom->comConnect();
  ui->groupBox_config->setEnabled(true);
  ui->scrollAreaWidgetContents->setEnabled(true);
}

void EasyGrblSetup::buttonDisconnect()
{
  m_grblCom->comDisconnect();
}

void EasyGrblSetup::buttonUnlock()
{
  ui->plainTextEdit_log->appendPlainText("Machine unlocked, Caution!");
  m_alarmLabel->setText("");
  m_grblCom->appendCommand("$X (no-logging)");
}

void EasyGrblSetup::buttonHome()
{
  ui->plainTextEdit_log->appendPlainText("Homing Cycle started...");
  m_grblCom->showGrblStatus("Home");
  m_grblCom->appendCommand("$H (no-logging)");
}

void EasyGrblSetup::buttonReset()  { m_grblCom->sendReset(); }
void EasyGrblSetup::buttonHold()   { m_grblCom->sendHold(); }
void EasyGrblSetup::buttonResume() { m_grblCom->sendResume(); }

void EasyGrblSetup::setZeroX() { m_grblCom->appendCommand("G92 X0"); }
void EasyGrblSetup::setZeroY() { m_grblCom->appendCommand("G92 Y0"); }
void EasyGrblSetup::setZeroZ() { m_grblCom->appendCommand("G92 Z0"); }
void EasyGrblSetup::setZeroA() { m_grblCom->appendCommand("G92 A0"); }
void EasyGrblSetup::setZeroB() { m_grblCom->appendCommand("G92 B0"); }
void EasyGrblSetup::setZeroC() { m_grblCom->appendCommand("G92 C0"); }

void EasyGrblSetup::buttonEnableSoftlimit()  { m_grblCom->appendCommand("$20=1"); }
void EasyGrblSetup::buttonDisableSoftlimit() { m_grblCom->appendCommand("$20=0"); }
void EasyGrblSetup::buttonEnableHardlimit()  { m_grblCom->appendCommand("$21=1"); }
void EasyGrblSetup::buttonDisableHardlimit() { m_grblCom->appendCommand("$21=0"); }
void EasyGrblSetup::buttonEnableHoming()     { m_grblCom->appendCommand("$22=1"); }
void EasyGrblSetup::buttonDisableHoming()    { m_grblCom->appendCommand("$22=0"); }

void EasyGrblSetup::buttonDirectionTestX()
{
  if (ui->label_grbl_status->text() == "Idle") {
    for (int i = 0; i<5; i++) {
      m_grblCom->appendCommand("G91 G1 X+20 F300");
      m_grblCom->appendCommand("G91 G0 X-20");
    }
  }
}

void EasyGrblSetup::buttonDirectionTestY()
{
  if (ui->label_grbl_status->text() == "Idle") {
    for (int i = 0; i<5; i++) {
      m_grblCom->appendCommand("G91 G1 Y+20 F300");
      m_grblCom->appendCommand("G91 G0 Y-20");
    }
  }
}

void EasyGrblSetup::buttonDirectionTestZ()
{
  if (ui->label_grbl_status->text() == "Idle") {
    for (int i = 0; i<5; i++) {
      m_grblCom->appendCommand("G91 G1 Z+20 F300");
      m_grblCom->appendCommand("G91 G0 Z-20");
    }
  }
}

void EasyGrblSetup::buttonDirectionTestA()
{
  if (ui->label_grbl_status->text() == "Idle") {
    for (int i = 0; i<5; i++) {
      m_grblCom->appendCommand("G1 A+20 F300");
      m_grblCom->appendCommand("G0 A0");
    }
  }
}

void EasyGrblSetup::buttonDirectionTestB()
{
  if (ui->label_grbl_status->text() == "Idle") {
    for (int i = 0; i<5; i++) {
      m_grblCom->appendCommand("G1 B+20 F300");
      m_grblCom->appendCommand("G0 B0");
    }
  }
}

void EasyGrblSetup::buttonDirectionTestC()
{
  if (ui->label_grbl_status->text() == "Idle") {
    for (int i = 0; i<5; i++) {
      m_grblCom->appendCommand("G1 C+20 F300");
      m_grblCom->appendCommand("G0 C0");
    }
  }
}


void EasyGrblSetup::buttonHomeX()
{
  if (ui->checkBox_Xplus->isChecked()) {
    ui->checkBox_23X->setChecked(false);
  } else if (ui->checkBox_Xminus->isChecked()) {
    ui->checkBox_23X->setChecked(true);
  } else return;
  // run home cycle only when a limit switch is mounted
  m_grblCom->showGrblStatus("Home");
  m_grblCom->appendCommand("$HX");
}

void EasyGrblSetup::buttonHomeY()
{
  if (ui->checkBox_Yplus->isChecked()) {
    ui->checkBox_23Y->setChecked(false);
  } else if (ui->checkBox_Yminus->isChecked()) {
    ui->checkBox_23Y->setChecked(true);
  } else return;
  // run home cycle only when a limit switch is mounted
  m_grblCom->showGrblStatus("Home");
  m_grblCom->appendCommand("$HY");
}

void EasyGrblSetup::buttonHomeZ()
{
  if (ui->checkBox_Zplus->isChecked()) {
    ui->checkBox_23Z->setChecked(false);
  } else if (ui->checkBox_Zminus->isChecked()) {
    ui->checkBox_23Z->setChecked(true);
  } else return;
  // run home cycle only when a limit switch is mounted
  m_grblCom->showGrblStatus("Home");
  m_grblCom->appendCommand("$HZ");
}

void EasyGrblSetup::buttonHomeA()
{
  if (ui->checkBox_Aplus->isChecked()) {
    ui->checkBox_23A->setChecked(false);
  } else if (ui->checkBox_Aminus->isChecked()) {
    ui->checkBox_23A->setChecked(true);
  } else return;
  // run home cycle only when a limit switch is mounted
  m_grblCom->showGrblStatus("Home");
  m_grblCom->appendCommand("$HA");
}

void EasyGrblSetup::buttonHomeB()
{
  if (ui->checkBox_Bplus->isChecked()) {
    ui->checkBox_23B->setChecked(false);
  } else if (ui->checkBox_Bminus->isChecked()) {
    ui->checkBox_23B->setChecked(true);
  } else return;
  // run home cycle only when a limit switch is mounted
  m_grblCom->showGrblStatus("Home");
  m_grblCom->appendCommand("$HB");
}

void EasyGrblSetup::buttonHomeC()
{
  if (ui->checkBox_Cplus->isChecked()) {
    ui->checkBox_23C->setChecked(false);
  } else if (ui->checkBox_Cminus->isChecked()) {
    ui->checkBox_23C->setChecked(true);
  } else return;
  // run home cycle only when a limit switch is mounted
  m_grblCom->showGrblStatus("Home");
  m_grblCom->appendCommand("$HC");
}


void EasyGrblSetup::buttonSpeedTestX()
{
  if ((ui->label_grbl_status->text() == "Idle") &&
      (ui->checkBox_20->isChecked()) && (ui->checkBox_21->isChecked()) && (ui->checkBox_22->isChecked())) {
    int maxspeed = ui->doubleSpinBox_110X->value();
    int speed = std::max(500, maxspeed/2);
    int move  = 10 - ui->doubleSpinBox_130X->value();
    m_grblCom->appendCommand("G53 G1 X-10 F1000");
    while (speed <= maxspeed) {
      m_grblCom->appendCommand(QString("G53 G1 X%1 F%2").arg(move).arg(speed));
      m_grblCom->appendCommand(QString("G53 G1 X%1 F%2").arg(-10).arg(speed));
      speed += maxspeed/20;
    }
  }
}

void EasyGrblSetup::buttonSpeedTestY()
{
  if ((ui->label_grbl_status->text() == "Idle") &&
      (ui->checkBox_20->isChecked()) && (ui->checkBox_21->isChecked()) && (ui->checkBox_22->isChecked())) {
    int maxspeed = ui->doubleSpinBox_110Y->value();
    int speed = std::max(500, maxspeed/2);
    int move  = 10 - ui->doubleSpinBox_130Y->value();
    m_grblCom->appendCommand("G53 G1 Y-10 F1000");
    while (speed <= maxspeed) {
      m_grblCom->appendCommand(QString("G53 G1 Y%1 F%2").arg(move).arg(speed));
      m_grblCom->appendCommand(QString("G53 G1 Y%1 F%2").arg(-10).arg(speed));
      speed += maxspeed/20;
    }
  }
}

void EasyGrblSetup::buttonSpeedTestZ()
{
  if ((ui->label_grbl_status->text() == "Idle") &&
      (ui->checkBox_20->isChecked()) && (ui->checkBox_21->isChecked()) && (ui->checkBox_22->isChecked())) {
    int maxspeed = ui->doubleSpinBox_110Z->value();
    int speed = std::max(500, maxspeed/2);
    int move  = 10 - ui->doubleSpinBox_130Z->value();
    m_grblCom->appendCommand("G53 G1 Z-10 F1000");
    while (speed <= maxspeed) {
      m_grblCom->appendCommand(QString("G53 G1 Z%1 F%2").arg(move).arg(speed));
      m_grblCom->appendCommand(QString("G53 G1 Z%1 F%2").arg(-10).arg(speed));
      speed += maxspeed/20;
    }
  }
}

void EasyGrblSetup::buttonSpeedTestA()
{
  if ((ui->label_grbl_status->text() == "Idle") &&
      (ui->checkBox_20->isChecked()) && (ui->checkBox_21->isChecked()) && (ui->checkBox_22->isChecked())) {
    int maxspeed = ui->doubleSpinBox_110A->value();
    int speed = std::max(500, maxspeed/2);
    int move  = 10 - ui->doubleSpinBox_130A->value();
    m_grblCom->appendCommand("G53 G1 A-10 F1000");
    while (speed <= maxspeed) {
      m_grblCom->appendCommand(QString("G53 G1 A%1 F%2").arg(move).arg(speed));
      m_grblCom->appendCommand(QString("G53 G1 A%1 F%2").arg(-10).arg(speed));
      speed += maxspeed/20;
    }
  }
}

void EasyGrblSetup::buttonSpeedTestB()
{
  if ((ui->label_grbl_status->text() == "Idle") &&
      (ui->checkBox_20->isChecked()) && (ui->checkBox_21->isChecked()) && (ui->checkBox_22->isChecked())) {
    int maxspeed = ui->doubleSpinBox_110B->value();
    int speed = std::max(500, maxspeed/2);
    int move  = 10 - ui->doubleSpinBox_130B->value();
    m_grblCom->appendCommand("G53 G1 B-10 F1000");
    while (speed <= maxspeed) {
      m_grblCom->appendCommand(QString("G53 G1 B%1 F%2").arg(move).arg(speed));
      m_grblCom->appendCommand(QString("G53 G1 B%1 F%2").arg(-10).arg(speed));
      speed += maxspeed/20;
    }
  }
}

void EasyGrblSetup::buttonSpeedTestC()
{
  if ((ui->label_grbl_status->text() == "Idle") &&
      (ui->checkBox_20->isChecked()) && (ui->checkBox_21->isChecked()) && (ui->checkBox_22->isChecked())) {
    int maxspeed = ui->doubleSpinBox_110C->value();
    int speed = std::max(500, maxspeed/2);
    int move  = 10 - ui->doubleSpinBox_130C->value();
    m_grblCom->appendCommand("G53 G1 C-10 F1000");
    while (speed <= maxspeed) {
      m_grblCom->appendCommand(QString("G53 G1 C%1 F%2").arg(move).arg(speed));
      m_grblCom->appendCommand(QString("G53 G1 C%1 F%2").arg(-10).arg(speed));
      speed += maxspeed/20;
    }
  }
}


// ---------- ---------- ---------- ---------- ---------- ---------- ---------- ----------
// Context Menus
// ---------- ---------- ---------- ---------- ---------- ---------- ---------- ----------

void EasyGrblSetup::createContextMenus()
{
  m_axesMenu = new QMenu(this);
  m_axesMenu->addAction("MOVE to zero");
  m_axesMenu->addAction("SET to zero (temporary)");
  m_axesMenu->addAction("HOME this axis");

  ui->label_X            ->setContextMenuPolicy(Qt::CustomContextMenu);
  ui->doubleSpinBox_WposX->setContextMenuPolicy(Qt::CustomContextMenu);
  ui->doubleSpinBox_MposX->setContextMenuPolicy(Qt::CustomContextMenu);
  ui->label_Y            ->setContextMenuPolicy(Qt::CustomContextMenu);
  ui->doubleSpinBox_WposY->setContextMenuPolicy(Qt::CustomContextMenu);
  ui->doubleSpinBox_MposY->setContextMenuPolicy(Qt::CustomContextMenu);
  ui->label_Z            ->setContextMenuPolicy(Qt::CustomContextMenu);
  ui->doubleSpinBox_WposZ->setContextMenuPolicy(Qt::CustomContextMenu);
  ui->doubleSpinBox_MposZ->setContextMenuPolicy(Qt::CustomContextMenu);
  ui->label_A            ->setContextMenuPolicy(Qt::CustomContextMenu);
  ui->doubleSpinBox_WposA->setContextMenuPolicy(Qt::CustomContextMenu);
  ui->doubleSpinBox_MposA->setContextMenuPolicy(Qt::CustomContextMenu);
  ui->label_B            ->setContextMenuPolicy(Qt::CustomContextMenu);
  ui->doubleSpinBox_WposB->setContextMenuPolicy(Qt::CustomContextMenu);
  ui->doubleSpinBox_MposB->setContextMenuPolicy(Qt::CustomContextMenu);
  ui->label_C            ->setContextMenuPolicy(Qt::CustomContextMenu);
  ui->doubleSpinBox_WposC->setContextMenuPolicy(Qt::CustomContextMenu);
  ui->doubleSpinBox_MposC->setContextMenuPolicy(Qt::CustomContextMenu);

  connect( ui->label_X,             &QLabelClickable::customContextMenuRequested,
           this,                    &EasyGrblSetup::showContextMenuAxisX );
  connect( ui->doubleSpinBox_WposX, &QDoubleSpinBox::customContextMenuRequested,
           this,                    &EasyGrblSetup::showContextMenuAxisX );
  connect( ui->doubleSpinBox_MposX, &QDoubleSpinBox::customContextMenuRequested,
           this,                    &EasyGrblSetup::showContextMenuAxisX );
  connect( ui->label_Y,             &QLabelClickable::customContextMenuRequested,
           this,                    &EasyGrblSetup::showContextMenuAxisY );
  connect( ui->doubleSpinBox_WposY, &QDoubleSpinBox::customContextMenuRequested,
           this,                    &EasyGrblSetup::showContextMenuAxisY );
  connect( ui->doubleSpinBox_MposY, &QDoubleSpinBox::customContextMenuRequested,
           this,                    &EasyGrblSetup::showContextMenuAxisY );
  connect( ui->label_Z,             &QLabelClickable::customContextMenuRequested,
           this,                    &EasyGrblSetup::showContextMenuAxisZ );
  connect( ui->doubleSpinBox_WposZ, &QDoubleSpinBox::customContextMenuRequested,
           this,                    &EasyGrblSetup::showContextMenuAxisZ );
  connect( ui->doubleSpinBox_MposZ, &QDoubleSpinBox::customContextMenuRequested,
           this,                    &EasyGrblSetup::showContextMenuAxisZ );
  connect( ui->label_A,             &QLabelClickable::customContextMenuRequested,
           this,                    &EasyGrblSetup::showContextMenuAxisA );
  connect( ui->doubleSpinBox_WposA, &QDoubleSpinBox::customContextMenuRequested,
           this,                    &EasyGrblSetup::showContextMenuAxisA );
  connect( ui->doubleSpinBox_MposA, &QDoubleSpinBox::customContextMenuRequested,
           this,                    &EasyGrblSetup::showContextMenuAxisA );
  connect( ui->label_B,             &QLabelClickable::customContextMenuRequested,
           this,                    &EasyGrblSetup::showContextMenuAxisB );
  connect( ui->doubleSpinBox_WposB, &QDoubleSpinBox::customContextMenuRequested,
           this,                    &EasyGrblSetup::showContextMenuAxisB );
  connect( ui->doubleSpinBox_MposB, &QDoubleSpinBox::customContextMenuRequested,
           this,                    &EasyGrblSetup::showContextMenuAxisB );
  connect( ui->label_C,             &QLabelClickable::customContextMenuRequested,
           this,                    &EasyGrblSetup::showContextMenuAxisC );
  connect( ui->doubleSpinBox_WposC, &QDoubleSpinBox::customContextMenuRequested,
           this,                    &EasyGrblSetup::showContextMenuAxisC );
  connect( ui->doubleSpinBox_MposC, &QDoubleSpinBox::customContextMenuRequested,
           this,                    &EasyGrblSetup::showContextMenuAxisC );

}

void EasyGrblSetup::showContextMenuAxis(int axis, const QPoint& pos)
{
  // get position of the calling widget
  QPoint  globalPos;
  QString axisChar;
  switch (axis) {
  case 0:  globalPos = ui->label_X->mapToGlobal(pos);  axisChar = "X";  break;
  case 1:  globalPos = ui->label_Y->mapToGlobal(pos);  axisChar = "Y";  break;
  case 2:  globalPos = ui->label_Z->mapToGlobal(pos);  axisChar = "Z";  break;
  case 3:  globalPos = ui->label_A->mapToGlobal(pos);  axisChar = "A";  break;
  case 4:  globalPos = ui->label_B->mapToGlobal(pos);  axisChar = "B";  break;
  case 5:  globalPos = ui->label_C->mapToGlobal(pos);  axisChar = "C";  break;
  }

  // execute menu
  QAction* selectedItem = m_axesMenu->exec(globalPos);
  if (selectedItem) {
    if        (selectedItem->text().contains("MOVE")) {
      ui->plainTextEdit_log->appendPlainText( "Move " + axisChar + "-axis to zero...");
      m_grblCom->appendCommand(QString("G0 %110").arg(axisChar));
    } else if (selectedItem->text().contains("SET to")) {
      ui->plainTextEdit_log->appendPlainText( "Set current position to " + axisChar + "-axis temporary zero");
      m_grblCom->appendCommand(QString("G92 %110").arg(axisChar));
    } else if (selectedItem->text().contains("HOME")) {
      ui->plainTextEdit_log->appendPlainText( "Home " + axisChar + "-axis...");
      m_grblCom->appendCommand(QString("$H%1").arg(axisChar));
    }
  } else {
    // no menu entry selected
  }
}

void EasyGrblSetup::showContextMenuAxisX(const QPoint& pos) { showContextMenuAxis(0, pos); }
void EasyGrblSetup::showContextMenuAxisY(const QPoint& pos) { showContextMenuAxis(1, pos); }
void EasyGrblSetup::showContextMenuAxisZ(const QPoint& pos) { showContextMenuAxis(2, pos); }
void EasyGrblSetup::showContextMenuAxisA(const QPoint& pos) { showContextMenuAxis(3, pos); }
void EasyGrblSetup::showContextMenuAxisB(const QPoint& pos) { showContextMenuAxis(4, pos); }
void EasyGrblSetup::showContextMenuAxisC(const QPoint& pos) { showContextMenuAxis(5, pos); }
