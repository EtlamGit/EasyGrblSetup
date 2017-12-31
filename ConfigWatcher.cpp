// Copyright 2017 EtlamGit

#include "ConfigWatcher.h"
#include "ui_mainwindow.h"

#include <QStringList>
#include <QObjectList>


ConfigWatcher::ConfigWatcher(Ui::MainWindow *pui)
  : ui(pui)
  , m_enabled(false)
  , m_reg2axes{6}   // start with invalid address !
  , m_axes2reg{6}   // start with invalid address !
  , m_axes2mask{0}  // start with all disabled !
{
  // hide extra axes at program start

  setVisibleA(false);
  setVisibleB(false);
  setVisibleC(false);

  // connect value change of all config elements

  connect( ui->spinBox_0, QOverload<int>::of(&QSpinBox::valueChanged),
           this,          &ConfigWatcher::updateConfig0);
  connect( ui->spinBox_1, QOverload<int>::of(&QSpinBox::valueChanged),
           this,          &ConfigWatcher::updateConfig1);
  connect( ui->checkBox_2X, &QCheckBox::toggled,
           this,            &ConfigWatcher::updateConfig2);
  connect( ui->checkBox_2Y, &QCheckBox::toggled,
           this,            &ConfigWatcher::updateConfig2);
  connect( ui->checkBox_2Z, &QCheckBox::toggled,
           this,            &ConfigWatcher::updateConfig2);
  connect( ui->checkBox_2C, &QCheckBox::toggled,
           this,            &ConfigWatcher::updateConfig2);
  connect( ui->checkBox_3X, &QCheckBox::toggled,
           this,            &ConfigWatcher::updateConfig3);
  connect( ui->checkBox_3Y, &QCheckBox::toggled,
           this,            &ConfigWatcher::updateConfig3);
  connect( ui->checkBox_3Z, &QCheckBox::toggled,
           this,            &ConfigWatcher::updateConfig3);
  connect( ui->checkBox_3C, &QCheckBox::toggled,
           this,            &ConfigWatcher::updateConfig3);
  connect( ui->checkBox_4,  &QCheckBox::toggled,
           this,            &ConfigWatcher::updateConfig4);
  connect( ui->checkBox_5,  &QCheckBox::toggled,
           this,            &ConfigWatcher::updateConfig5);
  connect( ui->checkBox_6,  &QCheckBox::toggled,
           this,            &ConfigWatcher::updateConfig6);

  connect( ui->spinBox_10, QOverload<int>::of(&QSpinBox::valueChanged),
           this,          &ConfigWatcher::updateConfig10);

  connect( ui->doubleSpinBox_11, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
           this,                 &ConfigWatcher::updateConfig11);

  connect( ui->doubleSpinBox_12, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
           this,                 &ConfigWatcher::updateConfig12);
  connect( ui->checkBox_13, &QCheckBox::toggled,
           this,            &ConfigWatcher::updateConfig13);

  connect( ui->checkBox_20, &QCheckBox::toggled,
           this,            &ConfigWatcher::updateConfig20);
  connect( ui->checkBox_21, &QCheckBox::toggled,
           this,            &ConfigWatcher::updateConfig21);
  connect( ui->checkBox_22, &QCheckBox::toggled,
           this,            &ConfigWatcher::updateConfig22);
  connect( ui->checkBox_23X, &QCheckBox::toggled,
           this,             &ConfigWatcher::updateConfig23);
  connect( ui->checkBox_23Y, &QCheckBox::toggled,
           this,             &ConfigWatcher::updateConfig23);
  connect( ui->checkBox_23Z, &QCheckBox::toggled,
           this,             &ConfigWatcher::updateConfig23);
  connect( ui->checkBox_23C, &QCheckBox::toggled,
           this,             &ConfigWatcher::updateConfig23);
  connect( ui->spinBox_24, QOverload<int>::of(&QSpinBox::valueChanged),
           this,           &ConfigWatcher::updateConfig24);
  connect( ui->spinBox_25, QOverload<int>::of(&QSpinBox::valueChanged),
           this,           &ConfigWatcher::updateConfig25);
  connect( ui->spinBox_26, QOverload<int>::of(&QSpinBox::valueChanged),
           this,           &ConfigWatcher::updateConfig26);
  connect( ui->doubleSpinBox_27, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
           this,                 &ConfigWatcher::updateConfig27);

  connect( ui->spinBox_30, QOverload<int>::of(&QSpinBox::valueChanged),
           this,           &ConfigWatcher::updateConfig30);
  connect( ui->spinBox_31, QOverload<int>::of(&QSpinBox::valueChanged),
           this,           &ConfigWatcher::updateConfig31);
  connect( ui->checkBox_32, &QCheckBox::toggled,
           this,            &ConfigWatcher::updateConfig32);

  connect( ui->doubleSpinBox_100X, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
           this,                   &ConfigWatcher::updateConfig100X);
  connect( ui->doubleSpinBox_100Y, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
           this,                   &ConfigWatcher::updateConfig100Y);
  connect( ui->doubleSpinBox_100Z, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
           this,                   &ConfigWatcher::updateConfig100Z);
  connect( ui->doubleSpinBox_100A, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
           this,                   &ConfigWatcher::updateConfig100A);
  connect( ui->doubleSpinBox_100B, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
           this,                   &ConfigWatcher::updateConfig100B);
  connect( ui->doubleSpinBox_100C, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
           this,                   &ConfigWatcher::updateConfig100C);

  connect( ui->doubleSpinBox_110X, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
           this,                   &ConfigWatcher::updateConfig110X);
  connect( ui->doubleSpinBox_110Y, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
           this,                   &ConfigWatcher::updateConfig110Y);
  connect( ui->doubleSpinBox_110Z, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
           this,                   &ConfigWatcher::updateConfig110Z);
  connect( ui->doubleSpinBox_110A, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
           this,                   &ConfigWatcher::updateConfig110A);
  connect( ui->doubleSpinBox_110B, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
           this,                   &ConfigWatcher::updateConfig110B);
  connect( ui->doubleSpinBox_110C, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
           this,                   &ConfigWatcher::updateConfig110C);

  connect( ui->doubleSpinBox_120X, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
           this,                   &ConfigWatcher::updateConfig120X);
  connect( ui->doubleSpinBox_120Y, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
           this,                   &ConfigWatcher::updateConfig120Y);
  connect( ui->doubleSpinBox_120Z, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
           this,                   &ConfigWatcher::updateConfig120Z);
  connect( ui->doubleSpinBox_120A, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
           this,                   &ConfigWatcher::updateConfig120A);
  connect( ui->doubleSpinBox_120B, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
           this,                   &ConfigWatcher::updateConfig120B);
  connect( ui->doubleSpinBox_120C, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
           this,                   &ConfigWatcher::updateConfig120C);

  connect( ui->doubleSpinBox_130X, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
           this,                   &ConfigWatcher::updateConfig130X);
  connect( ui->doubleSpinBox_130Y, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
           this,                   &ConfigWatcher::updateConfig130Y);
  connect( ui->doubleSpinBox_130Z, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
           this,                   &ConfigWatcher::updateConfig130Z);
  connect( ui->doubleSpinBox_130A, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
           this,                   &ConfigWatcher::updateConfig130A);
  connect( ui->doubleSpinBox_130B, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
           this,                   &ConfigWatcher::updateConfig130B);
  connect( ui->doubleSpinBox_130C, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
           this,                   &ConfigWatcher::updateConfig130C);

  reset();
}

ConfigWatcher::~ConfigWatcher()
{
  delete ui;
}


// ---------- ---------- ---------- ---------- ---------- ----------
// axes idenfication methods
// ---------- ---------- ---------- ---------- ---------- ----------

void ConfigWatcher::reset()
{
  // bring back into disabled (save) state
  for (int i=0; i<6; i++) {
    m_reg2axes[i]  = 6;
    m_axes2reg[i]  = 6;
    m_axes2mask[i] = 0;
  }
  m_axes_configured = false;
  m_axes_query_send = false;
  m_axes_query      = false;
  m_axes_test       = 0;
  m_reg_num         = 0;
}

void ConfigWatcher::setAxisIndex(int regnum, int axesnum)
{
  m_reg2axes[regnum]   = axesnum;  //
  m_axes2reg[axesnum]  = regnum;
  m_axes2mask[axesnum] = 1 << regnum;
}


bool ConfigWatcher::handleAxesQueryOk()
{
  if (m_axes_query) {
    // found a axis
    setAxisIndex(m_reg_num, m_axes_test);
    m_reg_num++;
    // succeded -> show
    switch (m_axes_test) {
    case 0:  setVisibleX(true);  break;
    case 1:  setVisibleY(true);  break;
    case 2:  setVisibleZ(true);  break;
    case 3:  setVisibleA(true);  break;
    case 4:  setVisibleB(true);  break;
    case 5:  setVisibleC(true);
             m_axes_query      = false;
             m_axes_configured = true;
             break;
    }
    m_axes_test++;  // done a test
    return false;
  }
  return true;
}

bool ConfigWatcher::handleAxesQueryError()
{
  if (m_axes_query) {
    // failed -> hide
    switch (m_axes_test) {
    case 0:  setVisibleX(false);  break;
    case 1:  setVisibleY(false);  break;
    case 2:  setVisibleZ(false);  break;
    case 3:  setVisibleA(false);  break;
    case 4:  setVisibleB(false);  break;
    case 5:  setVisibleC(false);
             m_axes_query      = false;
             m_axes_configured = true;
             break;
    }
    m_axes_test++;  // done a test
    return false;
  }
  return true;
}


// ---------- ---------- ---------- ---------- ---------- ----------
// generic configuration update methods
// ---------- ---------- ---------- ---------- ---------- ----------

bool ConfigWatcher::isConfigured()
{
  return m_axes_configured;
}

bool ConfigWatcher::isCommandAvailable()
{
  if (!m_axes_query_send) {
    // first commands with unknown axes configuration
    // => try to move axes to current position and observe response
    m_commands.append(QString("G53 G1 X%1 F100 (EasyGrblSetup query axis X)\n").arg(ui->doubleSpinBox_MposX->value()));
    m_commands.append(QString("G53 G1 Y%1 F100 (EasyGrblSetup query axis Y)\n").arg(ui->doubleSpinBox_MposY->value()));
    m_commands.append(QString("G53 G1 Z%1 F100 (EasyGrblSetup query axis Z)\n").arg(ui->doubleSpinBox_MposZ->value()));
    m_commands.append(QString("G53 G1 A%1 F100 (EasyGrblSetup query axis A)\n").arg(ui->doubleSpinBox_MposA->value()));
    m_commands.append(QString("G53 G1 B%1 F100 (EasyGrblSetup query axis B)\n").arg(ui->doubleSpinBox_MposB->value()));
    m_commands.append(QString("G53 G1 C%1 F100 (EasyGrblSetup query axis C)\n").arg(ui->doubleSpinBox_MposC->value()));
    m_status_count = 0;
    m_commands.append("$$\n");
    m_axes_query_send = true;
    m_axes_query      = true;
  }
  // read current configuration every 5 seconds
  if (++m_status_count>(1000/250*5)) {
    m_status_count = 0;
    m_commands.append("$$\n");
  }
  return !m_commands.isEmpty();
}

QString ConfigWatcher::getCommand()
{
  return m_commands.takeFirst();
}

void ConfigWatcher::appendConfig(int pnum, int value)
{
  if (m_enabled) {
    m_commands.append( QString("$%1=%2\n").arg(pnum).arg(value) );
  }
}

void ConfigWatcher::appendConfig(int pnum, double value)
{
  if (m_enabled) {
    m_commands.append( QString("$%1=%2\n").arg(pnum).arg(value) );
  }
}


// ---------- ---------- ---------- ---------- ---------- ----------
// configuration parser
// ---------- ---------- ---------- ---------- ---------- ----------

void ConfigWatcher::parseGrblConfig(QString line)
{
  // disable config update slot
  m_enabled = false;

  QStringList parameter = line.split('=');
  if (2 == parameter.length()) {
    int    pindex = parameter.at(0).mid(1,3).toInt();
    double fvalue = parameter.at(1).toDouble();
    int    ivalue = round(fvalue);
    int    aindex;

    switch (pindex) {
    case   0:  ui->spinBox_0->setValue(ivalue);  break;
    case   1:  ui->spinBox_1->setValue(ivalue);  break;
    case   2:  ui->checkBox_2X->setChecked(0 !=(ivalue & m_axes2mask[0]));
               ui->checkBox_2Y->setChecked(0 !=(ivalue & m_axes2mask[1]));
               ui->checkBox_2Z->setChecked(0 !=(ivalue & m_axes2mask[2]));
               ui->checkBox_2A->setChecked(0 !=(ivalue & m_axes2mask[3]));
               ui->checkBox_2B->setChecked(0 !=(ivalue & m_axes2mask[4]));
               ui->checkBox_2C->setChecked(0 !=(ivalue & m_axes2mask[5]));
               break;
    case   3:  ui->checkBox_3X->setChecked(0 !=(ivalue & m_axes2mask[0]));
               ui->checkBox_3Y->setChecked(0 !=(ivalue & m_axes2mask[1]));
               ui->checkBox_3Z->setChecked(0 !=(ivalue & m_axes2mask[2]));
               ui->checkBox_3A->setChecked(0 !=(ivalue & m_axes2mask[3]));
               ui->checkBox_3B->setChecked(0 !=(ivalue & m_axes2mask[4]));
               ui->checkBox_3C->setChecked(0 !=(ivalue & m_axes2mask[5]));
               break;
    case   4:  ui->checkBox_4->setChecked(ivalue);  break;
    case   5:  ui->checkBox_5->setChecked(ivalue);  break;
    case   6:  ui->checkBox_6->setChecked(ivalue);  break;

    case  10:  ui->spinBox_10->setValue(ivalue);  break;
    case  11:  ui->doubleSpinBox_11->setValue(fvalue);  break;
    case  12:  ui->doubleSpinBox_12->setValue(fvalue);  break;
    case  13:  ui->checkBox_13->setChecked(ivalue);  break;

    case  20:  ui->checkBox_20->setChecked(ivalue);  break;
    case  21:  ui->checkBox_21->setChecked(ivalue);  break;
    case  22:  ui->checkBox_22->setChecked(ivalue);  break;
    case  23:  ui->checkBox_23X->setChecked(0 !=(ivalue & m_axes2mask[0]));
               ui->checkBox_23Y->setChecked(0 !=(ivalue & m_axes2mask[1]));
               ui->checkBox_23Z->setChecked(0 !=(ivalue & m_axes2mask[2]));
               ui->checkBox_23A->setChecked(0 !=(ivalue & m_axes2mask[3]));
               ui->checkBox_23B->setChecked(0 !=(ivalue & m_axes2mask[4]));
               ui->checkBox_23C->setChecked(0 !=(ivalue & m_axes2mask[5]));
               break;
    case  24:  ui->spinBox_24->setValue(ivalue);  break;
    case  25:  ui->spinBox_25->setValue(ivalue);  break;
    case  26:  ui->spinBox_26->setValue(ivalue);  break;
    case  27:  ui->doubleSpinBox_27->setValue(fvalue);  break;

    case  30:  ui->spinBox_30->setValue(ivalue);  break;
    case  31:  ui->spinBox_31->setValue(ivalue);  break;
    case  32:  ui->checkBox_32->setChecked(ivalue);  break;

    case 100:
    case 101:
    case 102:
    case 103:
    case 104:
    case 105:
      aindex = m_reg2axes[ pindex - 100 ];
      switch (aindex){
      case 0:  ui->doubleSpinBox_100X->setValue(fvalue);  break;
      case 1:  ui->doubleSpinBox_100Y->setValue(fvalue);  break;
      case 2:  ui->doubleSpinBox_100Z->setValue(fvalue);  break;
      case 3:  ui->doubleSpinBox_100A->setValue(fvalue);  break;
      case 4:  ui->doubleSpinBox_100B->setValue(fvalue);  break;
      case 5:  ui->doubleSpinBox_100C->setValue(fvalue);  break;
      }
      break;

    case 110:
    case 111:
    case 112:
    case 113:
    case 114:
    case 115:
      aindex = m_reg2axes[ pindex - 110 ];
      switch (aindex){
      case 0:  ui->doubleSpinBox_110X->setValue(fvalue);  break;
      case 1:  ui->doubleSpinBox_110Y->setValue(fvalue);  break;
      case 2:  ui->doubleSpinBox_110Z->setValue(fvalue);  break;
      case 3:  ui->doubleSpinBox_110A->setValue(fvalue);  break;
      case 4:  ui->doubleSpinBox_110B->setValue(fvalue);  break;
      case 5:  ui->doubleSpinBox_110C->setValue(fvalue);  break;
      }
      break;

    case 120:
    case 121:
    case 122:
    case 123:
    case 124:
    case 125:
      aindex = m_reg2axes[ pindex - 120 ];
      switch (aindex){
      case 0:  ui->doubleSpinBox_120X->setValue(fvalue);  break;
      case 1:  ui->doubleSpinBox_120Y->setValue(fvalue);  break;
      case 2:  ui->doubleSpinBox_120Z->setValue(fvalue);  break;
      case 3:  ui->doubleSpinBox_120A->setValue(fvalue);  break;
      case 4:  ui->doubleSpinBox_120B->setValue(fvalue);  break;
      case 5:  ui->doubleSpinBox_120C->setValue(fvalue);  break;
      }
      break;

    case 130:
    case 131:
    case 132:
    case 133:
    case 134:
    case 135:
      aindex = m_reg2axes[ pindex - 130 ];
      switch (aindex){
      case 0:  ui->doubleSpinBox_130X->setValue(fvalue);  break;
      case 1:  ui->doubleSpinBox_130Y->setValue(fvalue);  break;
      case 2:  ui->doubleSpinBox_130Z->setValue(fvalue);  break;
      case 3:  ui->doubleSpinBox_130A->setValue(fvalue);  break;
      case 4:  ui->doubleSpinBox_130B->setValue(fvalue);  break;
      case 5:  ui->doubleSpinBox_130C->setValue(fvalue);  break;
      }
      break;

    default:   ui->plainTextEdit->appendPlainText(line);  break;
    }
  }

  // re-enable config update slot
  m_enabled = true;
}


// ---------- ---------- ---------- ---------- ---------- ----------
// dedicated configuration update methods
// ---------- ---------- ---------- ---------- ---------- ----------

void ConfigWatcher::updateConfig0()  { appendConfig(  0, ui->spinBox_0->value() ); }
void ConfigWatcher::updateConfig1()  { appendConfig(  1, ui->spinBox_1->value() ); }

void ConfigWatcher::updateConfig2()
{
  appendConfig( 2, (ui->checkBox_2X->isChecked() * m_axes2mask[0]) +
                   (ui->checkBox_2Y->isChecked() * m_axes2mask[1]) +
                   (ui->checkBox_2Z->isChecked() * m_axes2mask[2]) +
                   (ui->checkBox_2A->isChecked() * m_axes2mask[3]) +
                   (ui->checkBox_2B->isChecked() * m_axes2mask[4]) +
                   (ui->checkBox_2C->isChecked() * m_axes2mask[5]) );
}
void ConfigWatcher::updateConfig3()
{
  appendConfig( 2, (ui->checkBox_3X->isChecked() * m_axes2mask[0]) +
                   (ui->checkBox_3Y->isChecked() * m_axes2mask[1]) +
                   (ui->checkBox_3Z->isChecked() * m_axes2mask[2]) +
                   (ui->checkBox_3A->isChecked() * m_axes2mask[3]) +
                   (ui->checkBox_3B->isChecked() * m_axes2mask[4]) +
                   (ui->checkBox_3C->isChecked() * m_axes2mask[5]) );
}

void ConfigWatcher::updateConfig4()  { appendConfig(  4, ui->checkBox_4->isChecked() ); }
void ConfigWatcher::updateConfig5()  { appendConfig(  5, ui->checkBox_5->isChecked() ); }
void ConfigWatcher::updateConfig6()  { appendConfig(  6, ui->checkBox_6->isChecked() ); }

void ConfigWatcher::updateConfig10() { appendConfig( 10, ui->spinBox_10->value() ); }
void ConfigWatcher::updateConfig11() { appendConfig( 11, ui->doubleSpinBox_11->value() ); }
void ConfigWatcher::updateConfig12() { appendConfig( 12, ui->doubleSpinBox_12->value() ); }
void ConfigWatcher::updateConfig13() { appendConfig( 13, ui->checkBox_13->isChecked() ); }

void ConfigWatcher::updateConfig20() { appendConfig( 20, ui->checkBox_20->isChecked() ); }
void ConfigWatcher::updateConfig21() { appendConfig( 21, ui->checkBox_21->isChecked() ); }
void ConfigWatcher::updateConfig22() { appendConfig( 22, ui->checkBox_22->isChecked() ); }

void ConfigWatcher::updateConfig23()
{
  appendConfig( 2, (ui->checkBox_23X->isChecked() * m_axes2mask[0]) +
                   (ui->checkBox_23Y->isChecked() * m_axes2mask[1]) +
                   (ui->checkBox_23Z->isChecked() * m_axes2mask[2]) +
                   (ui->checkBox_23A->isChecked() * m_axes2mask[3]) +
                   (ui->checkBox_23B->isChecked() * m_axes2mask[4]) +
                   (ui->checkBox_23C->isChecked() * m_axes2mask[5]) );
}

void ConfigWatcher::updateConfig24() { appendConfig( 24, ui->spinBox_24->value() ); }
void ConfigWatcher::updateConfig25() { appendConfig( 25, ui->spinBox_25->value() ); }
void ConfigWatcher::updateConfig26() { appendConfig( 26, ui->spinBox_26->value() ); }
void ConfigWatcher::updateConfig27() { appendConfig( 27, ui->doubleSpinBox_27->value() ); }

void ConfigWatcher::updateConfig30() { appendConfig( 30, ui->spinBox_30->value() ); }
void ConfigWatcher::updateConfig31() { appendConfig( 31, ui->spinBox_31->value() ); }
void ConfigWatcher::updateConfig32() { appendConfig( 32, ui->checkBox_32->isChecked() ); }


void ConfigWatcher::updateConfig100X() { appendConfig( 100+m_axes2reg[0], ui->doubleSpinBox_100X->value() ); }
void ConfigWatcher::updateConfig100Y() { appendConfig( 100+m_axes2reg[1], ui->doubleSpinBox_100Y->value() ); }
void ConfigWatcher::updateConfig100Z() { appendConfig( 100+m_axes2reg[2], ui->doubleSpinBox_100Z->value() ); }
void ConfigWatcher::updateConfig100A() { appendConfig( 100+m_axes2reg[3], ui->doubleSpinBox_100A->value() ); }
void ConfigWatcher::updateConfig100B() { appendConfig( 100+m_axes2reg[4], ui->doubleSpinBox_100B->value() ); }
void ConfigWatcher::updateConfig100C() { appendConfig( 100+m_axes2reg[5], ui->doubleSpinBox_100C->value() ); }

void ConfigWatcher::updateConfig110X() { appendConfig( 110+m_axes2reg[0], ui->doubleSpinBox_110X->value() ); }
void ConfigWatcher::updateConfig110Y() { appendConfig( 110+m_axes2reg[1], ui->doubleSpinBox_110Y->value() ); }
void ConfigWatcher::updateConfig110Z() { appendConfig( 110+m_axes2reg[2], ui->doubleSpinBox_110Z->value() ); }
void ConfigWatcher::updateConfig110A() { appendConfig( 110+m_axes2reg[3], ui->doubleSpinBox_110A->value() ); }
void ConfigWatcher::updateConfig110B() { appendConfig( 110+m_axes2reg[4], ui->doubleSpinBox_110B->value() ); }
void ConfigWatcher::updateConfig110C() { appendConfig( 110+m_axes2reg[5], ui->doubleSpinBox_110C->value() ); }

void ConfigWatcher::updateConfig120X() { appendConfig( 120+m_axes2reg[0], ui->doubleSpinBox_120X->value() ); }
void ConfigWatcher::updateConfig120Y() { appendConfig( 120+m_axes2reg[1], ui->doubleSpinBox_120Y->value() ); }
void ConfigWatcher::updateConfig120Z() { appendConfig( 120+m_axes2reg[2], ui->doubleSpinBox_120Z->value() ); }
void ConfigWatcher::updateConfig120A() { appendConfig( 120+m_axes2reg[3], ui->doubleSpinBox_120A->value() ); }
void ConfigWatcher::updateConfig120B() { appendConfig( 120+m_axes2reg[4], ui->doubleSpinBox_120B->value() ); }
void ConfigWatcher::updateConfig120C() { appendConfig( 120+m_axes2reg[5], ui->doubleSpinBox_120C->value() ); }

void ConfigWatcher::updateConfig130X() { appendConfig( 130+m_axes2reg[0], ui->doubleSpinBox_130X->value() ); }
void ConfigWatcher::updateConfig130Y() { appendConfig( 130+m_axes2reg[1], ui->doubleSpinBox_130Y->value() ); }
void ConfigWatcher::updateConfig130Z() { appendConfig( 130+m_axes2reg[2], ui->doubleSpinBox_130Z->value() ); }
void ConfigWatcher::updateConfig130A() { appendConfig( 130+m_axes2reg[3], ui->doubleSpinBox_130A->value() ); }
void ConfigWatcher::updateConfig130B() { appendConfig( 130+m_axes2reg[4], ui->doubleSpinBox_130B->value() ); }
void ConfigWatcher::updateConfig130C() { appendConfig( 130+m_axes2reg[5], ui->doubleSpinBox_130C->value() ); }


// ---------- ---------- ---------- ---------- ---------- ----------
// hide/visible axes
// ---------- ---------- ---------- ---------- ---------- ----------

void ConfigWatcher::setVisibleX(bool flag)
{
  ui->label_MposX->setVisible(flag);
  ui->label_WposX->setVisible(flag);
  ui->doubleSpinBox_MposX->setVisible(flag);
  ui->doubleSpinBox_WposX->setVisible(flag);
  ui->label_pinX->setVisible(flag);
  ui->toolButton_jogXminus->setVisible(flag);
  ui->toolButton_jogXplus ->setVisible(flag);
  ui->checkBox_2X->setVisible(flag);
  ui->checkBox_3X->setVisible(flag);
  ui->checkBox_23X->setVisible(flag);
  ui->doubleSpinBox_100X->setVisible(flag);
  ui->doubleSpinBox_110X->setVisible(flag);
  ui->doubleSpinBox_120X->setVisible(flag);
  ui->doubleSpinBox_130X->setVisible(flag);
  ui->checkBox_Xminus->setVisible(flag);
  ui->checkBox_Xplus->setVisible(flag);
  ui->pushButton_testX->setVisible(flag);
  ui->toolButton_homeX->setVisible(flag);
  ui->toolButton_speedX->setVisible(flag);
}

void ConfigWatcher::setVisibleY(bool flag)
{
  ui->label_MposY->setVisible(flag);
  ui->label_WposY->setVisible(flag);
  ui->doubleSpinBox_MposY->setVisible(flag);
  ui->doubleSpinBox_WposY->setVisible(flag);
  ui->label_pinY->setVisible(flag);
  ui->toolButton_jogYminus->setVisible(flag);
  ui->toolButton_jogYplus ->setVisible(flag);
  ui->checkBox_2Y->setVisible(flag);
  ui->checkBox_3Y->setVisible(flag);
  ui->checkBox_23Y->setVisible(flag);
  ui->doubleSpinBox_100Y->setVisible(flag);
  ui->doubleSpinBox_110Y->setVisible(flag);
  ui->doubleSpinBox_120Y->setVisible(flag);
  ui->doubleSpinBox_130Y->setVisible(flag);
  ui->checkBox_Yminus->setVisible(flag);
  ui->checkBox_Yplus->setVisible(flag);
  ui->pushButton_testY->setVisible(flag);
  ui->toolButton_homeY->setVisible(flag);
  ui->toolButton_speedY->setVisible(flag);
}

void ConfigWatcher::setVisibleZ(bool flag)
{
  ui->label_MposZ->setVisible(flag);
  ui->label_WposZ->setVisible(flag);
  ui->doubleSpinBox_MposZ->setVisible(flag);
  ui->doubleSpinBox_WposZ->setVisible(flag);
  ui->label_pinZ->setVisible(flag);
  ui->toolButton_jogZminus->setVisible(flag);
  ui->toolButton_jogZplus ->setVisible(flag);
  ui->checkBox_2Z->setVisible(flag);
  ui->checkBox_3Z->setVisible(flag);
  ui->checkBox_23Z->setVisible(flag);
  ui->doubleSpinBox_100Z->setVisible(flag);
  ui->doubleSpinBox_110Z->setVisible(flag);
  ui->doubleSpinBox_120Z->setVisible(flag);
  ui->doubleSpinBox_130Z->setVisible(flag);
  ui->checkBox_Zminus->setVisible(flag);
  ui->checkBox_Zplus->setVisible(flag);
  ui->pushButton_testZ->setVisible(flag);
  ui->toolButton_homeZ->setVisible(flag);
  ui->toolButton_speedZ->setVisible(flag);
}

void ConfigWatcher::setVisibleA(bool flag)
{
  ui->label_MposA->setVisible(flag);
  ui->label_WposA->setVisible(flag);
  ui->doubleSpinBox_MposA->setVisible(flag);
  ui->doubleSpinBox_WposA->setVisible(flag);
  ui->label_pinA->setVisible(flag);
  ui->toolButton_jogAminus->setVisible(flag);
  ui->toolButton_jogAplus ->setVisible(flag);
  ui->checkBox_2A->setVisible(flag);
  ui->checkBox_3A->setVisible(flag);
  ui->checkBox_23A->setVisible(flag);
  ui->doubleSpinBox_100A->setVisible(flag);
  ui->doubleSpinBox_110A->setVisible(flag);
  ui->doubleSpinBox_120A->setVisible(flag);
  ui->doubleSpinBox_130A->setVisible(flag);
  ui->checkBox_Aminus->setVisible(flag);
  ui->checkBox_Aplus->setVisible(flag);
  ui->pushButton_testA->setVisible(flag);
  ui->toolButton_homeA->setVisible(flag);
  ui->toolButton_speedA->setVisible(flag);
}

void ConfigWatcher::setVisibleB(bool flag)
{
  ui->label_MposB->setVisible(flag);
  ui->label_WposB->setVisible(flag);
  ui->doubleSpinBox_MposB->setVisible(flag);
  ui->doubleSpinBox_WposB->setVisible(flag);
  ui->label_pinB->setVisible(flag);
  ui->toolButton_jogBminus->setVisible(flag);
  ui->toolButton_jogBplus ->setVisible(flag);
  ui->checkBox_2B->setVisible(flag);
  ui->checkBox_3B->setVisible(flag);
  ui->checkBox_23B->setVisible(flag);
  ui->doubleSpinBox_100B->setVisible(flag);
  ui->doubleSpinBox_110B->setVisible(flag);
  ui->doubleSpinBox_120B->setVisible(flag);
  ui->doubleSpinBox_130B->setVisible(flag);
  ui->checkBox_Bminus->setVisible(flag);
  ui->checkBox_Bplus->setVisible(flag);
  ui->pushButton_testB->setVisible(flag);
  ui->toolButton_homeB->setVisible(flag);
  ui->toolButton_speedB->setVisible(flag);
}

void ConfigWatcher::setVisibleC(bool flag)
{
  ui->label_MposC->setVisible(flag);
  ui->label_WposC->setVisible(flag);
  ui->doubleSpinBox_MposC->setVisible(flag);
  ui->doubleSpinBox_WposC->setVisible(flag);
  ui->label_pinC->setVisible(flag);
  ui->toolButton_jogCminus->setVisible(flag);
  ui->toolButton_jogCplus ->setVisible(flag);
  ui->checkBox_2C->setVisible(flag);
  ui->checkBox_3C->setVisible(flag);
  ui->checkBox_23C->setVisible(flag);
  ui->doubleSpinBox_100C->setVisible(flag);
  ui->doubleSpinBox_110C->setVisible(flag);
  ui->doubleSpinBox_120C->setVisible(flag);
  ui->doubleSpinBox_130C->setVisible(flag);
  ui->checkBox_Cminus->setVisible(flag);
  ui->checkBox_Cplus->setVisible(flag);
  ui->pushButton_testC->setVisible(flag);
  ui->toolButton_homeC->setVisible(flag);
  ui->toolButton_speedC->setVisible(flag);
}
