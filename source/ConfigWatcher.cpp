// Copyright 2017 EtlamGit

#include "ConfigWatcher.h"
#include "ui_EasyGrblSetup.h"

#include <QStringList>
#include <QObjectList>

#if QT_VERSION < 0x050700
  #include "qt_before_5_7_0.h"  // QOverload
#endif


ConfigWatcher::ConfigWatcher(Ui::EasyGrblSetup *pui)
  : ui(pui)
  , m_enabled(false)
  , m_reg2axes{6}   // start with invalid address !
  , m_axes2reg{6}   // start with invalid address !
  , m_axes2mask{0}  // start with all disabled !
{
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
           this,           &ConfigWatcher::updateConfig10);

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
}

void ConfigWatcher::setAxis(int axis, bool flag)
{
  m_axes[axis] = flag;
  int regnum = 0;
  for (int i = 0; i < 6; i++) {
    if (m_axes[i]) { // used axis
      m_reg2axes[regnum] = i;
      m_axes2reg[i]      = regnum;
      m_axes2mask[i]     = 1 << regnum;
      regnum++;
    }
  }
}


// ---------- ---------- ---------- ---------- ---------- ----------
// generic configuration update methods
// ---------- ---------- ---------- ---------- ---------- ----------

void ConfigWatcher::writeConfig(int pnum, int value)
{
  if (m_enabled) {
    emit writeConfigCommand( QString("$%1=%2").arg(pnum).arg(value) );
  }
}

void ConfigWatcher::writeConfig(int pnum, double value)
{
  if (m_enabled) {
    emit writeConfigCommand( QString("$%1=%2").arg(pnum).arg(value) );
  }
}


// ---------- ---------- ---------- ---------- ---------- ----------
// configuration parser
// ---------- ---------- ---------- ---------- ---------- ----------

void ConfigWatcher::parseGrblSetting(QString line)
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

    default:   ui->plainTextEdit_log->appendPlainText(line);  break;
    }
  }

  // re-enable config update slot
  m_enabled = true;
}


// ---------- ---------- ---------- ---------- ---------- ----------
// dedicated configuration update methods
// ---------- ---------- ---------- ---------- ---------- ----------

void ConfigWatcher::updateConfig0()  { writeConfig(  0, ui->spinBox_0->value() ); }
void ConfigWatcher::updateConfig1()  { writeConfig(  1, ui->spinBox_1->value() ); }

void ConfigWatcher::updateConfig2()
{
  writeConfig( 2, (ui->checkBox_2X->isChecked() * m_axes2mask[0]) +
                  (ui->checkBox_2Y->isChecked() * m_axes2mask[1]) +
                  (ui->checkBox_2Z->isChecked() * m_axes2mask[2]) +
                  (ui->checkBox_2A->isChecked() * m_axes2mask[3]) +
                  (ui->checkBox_2B->isChecked() * m_axes2mask[4]) +
                  (ui->checkBox_2C->isChecked() * m_axes2mask[5]) );
}
void ConfigWatcher::updateConfig3()
{
  writeConfig( 2, (ui->checkBox_3X->isChecked() * m_axes2mask[0]) +
                  (ui->checkBox_3Y->isChecked() * m_axes2mask[1]) +
                  (ui->checkBox_3Z->isChecked() * m_axes2mask[2]) +
                  (ui->checkBox_3A->isChecked() * m_axes2mask[3]) +
                  (ui->checkBox_3B->isChecked() * m_axes2mask[4]) +
                  (ui->checkBox_3C->isChecked() * m_axes2mask[5]) );
}

void ConfigWatcher::updateConfig4()  { writeConfig(  4, ui->checkBox_4->isChecked() ); }
void ConfigWatcher::updateConfig5()  { writeConfig(  5, ui->checkBox_5->isChecked() ); }
void ConfigWatcher::updateConfig6()  { writeConfig(  6, ui->checkBox_6->isChecked() ); }

void ConfigWatcher::updateConfig10() { writeConfig( 10, ui->spinBox_10->value() ); }
void ConfigWatcher::updateConfig11() { writeConfig( 11, ui->doubleSpinBox_11->value() ); }
void ConfigWatcher::updateConfig12() { writeConfig( 12, ui->doubleSpinBox_12->value() ); }
void ConfigWatcher::updateConfig13() { writeConfig( 13, ui->checkBox_13->isChecked() ); }

void ConfigWatcher::updateConfig20() { writeConfig( 20, ui->checkBox_20->isChecked() ); }
void ConfigWatcher::updateConfig21() { writeConfig( 21, ui->checkBox_21->isChecked() ); }
void ConfigWatcher::updateConfig22() { writeConfig( 22, ui->checkBox_22->isChecked() ); }

void ConfigWatcher::updateConfig23()
{
  writeConfig( 2, (ui->checkBox_23X->isChecked() * m_axes2mask[0]) +
                   (ui->checkBox_23Y->isChecked() * m_axes2mask[1]) +
                   (ui->checkBox_23Z->isChecked() * m_axes2mask[2]) +
                   (ui->checkBox_23A->isChecked() * m_axes2mask[3]) +
                   (ui->checkBox_23B->isChecked() * m_axes2mask[4]) +
                   (ui->checkBox_23C->isChecked() * m_axes2mask[5]) );
}

void ConfigWatcher::updateConfig24() { writeConfig( 24, ui->spinBox_24->value() ); }
void ConfigWatcher::updateConfig25() { writeConfig( 25, ui->spinBox_25->value() ); }
void ConfigWatcher::updateConfig26() { writeConfig( 26, ui->spinBox_26->value() ); }
void ConfigWatcher::updateConfig27() { writeConfig( 27, ui->doubleSpinBox_27->value() ); }

void ConfigWatcher::updateConfig30() { writeConfig( 30, ui->spinBox_30->value() ); }
void ConfigWatcher::updateConfig31() { writeConfig( 31, ui->spinBox_31->value() ); }
void ConfigWatcher::updateConfig32() { writeConfig( 32, ui->checkBox_32->isChecked() ); }


void ConfigWatcher::updateConfig100X() { writeConfig( 100+m_axes2reg[0], ui->doubleSpinBox_100X->value() ); }
void ConfigWatcher::updateConfig100Y() { writeConfig( 100+m_axes2reg[1], ui->doubleSpinBox_100Y->value() ); }
void ConfigWatcher::updateConfig100Z() { writeConfig( 100+m_axes2reg[2], ui->doubleSpinBox_100Z->value() ); }
void ConfigWatcher::updateConfig100A() { writeConfig( 100+m_axes2reg[3], ui->doubleSpinBox_100A->value() ); }
void ConfigWatcher::updateConfig100B() { writeConfig( 100+m_axes2reg[4], ui->doubleSpinBox_100B->value() ); }
void ConfigWatcher::updateConfig100C() { writeConfig( 100+m_axes2reg[5], ui->doubleSpinBox_100C->value() ); }

void ConfigWatcher::updateConfig110X() { writeConfig( 110+m_axes2reg[0], ui->doubleSpinBox_110X->value() ); }
void ConfigWatcher::updateConfig110Y() { writeConfig( 110+m_axes2reg[1], ui->doubleSpinBox_110Y->value() ); }
void ConfigWatcher::updateConfig110Z() { writeConfig( 110+m_axes2reg[2], ui->doubleSpinBox_110Z->value() ); }
void ConfigWatcher::updateConfig110A() { writeConfig( 110+m_axes2reg[3], ui->doubleSpinBox_110A->value() ); }
void ConfigWatcher::updateConfig110B() { writeConfig( 110+m_axes2reg[4], ui->doubleSpinBox_110B->value() ); }
void ConfigWatcher::updateConfig110C() { writeConfig( 110+m_axes2reg[5], ui->doubleSpinBox_110C->value() ); }

void ConfigWatcher::updateConfig120X() { writeConfig( 120+m_axes2reg[0], ui->doubleSpinBox_120X->value() ); }
void ConfigWatcher::updateConfig120Y() { writeConfig( 120+m_axes2reg[1], ui->doubleSpinBox_120Y->value() ); }
void ConfigWatcher::updateConfig120Z() { writeConfig( 120+m_axes2reg[2], ui->doubleSpinBox_120Z->value() ); }
void ConfigWatcher::updateConfig120A() { writeConfig( 120+m_axes2reg[3], ui->doubleSpinBox_120A->value() ); }
void ConfigWatcher::updateConfig120B() { writeConfig( 120+m_axes2reg[4], ui->doubleSpinBox_120B->value() ); }
void ConfigWatcher::updateConfig120C() { writeConfig( 120+m_axes2reg[5], ui->doubleSpinBox_120C->value() ); }

void ConfigWatcher::updateConfig130X() { writeConfig( 130+m_axes2reg[0], ui->doubleSpinBox_130X->value() ); }
void ConfigWatcher::updateConfig130Y() { writeConfig( 130+m_axes2reg[1], ui->doubleSpinBox_130Y->value() ); }
void ConfigWatcher::updateConfig130Z() { writeConfig( 130+m_axes2reg[2], ui->doubleSpinBox_130Z->value() ); }
void ConfigWatcher::updateConfig130A() { writeConfig( 130+m_axes2reg[3], ui->doubleSpinBox_130A->value() ); }
void ConfigWatcher::updateConfig130B() { writeConfig( 130+m_axes2reg[4], ui->doubleSpinBox_130B->value() ); }
void ConfigWatcher::updateConfig130C() { writeConfig( 130+m_axes2reg[5], ui->doubleSpinBox_130C->value() ); }
