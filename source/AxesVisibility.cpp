// Copyright 2018 EtlamGit

#include "AxesVisibility.h"
#include "ui_EasyGrblSetup.h"


AxesVisibility::AxesVisibility(Ui::EasyGrblSetup *ui)
  : ui(ui)
{
  // disable axes at startup: Z A B C
  for (int i=2; i<6; i++)
    axisDisable(i);
}

void AxesVisibility::axisEnable(int index, bool flag)
{
  switch (index) {
  case 0:  setVisibleAxisX(flag);  break;
  case 1:  setVisibleAxisY(flag);  break;
  case 2:  setVisibleAxisZ(flag);  break;
  case 3:  setVisibleAxisA(flag);  break;
  case 4:  setVisibleAxisB(flag);  break;
  case 5:  setVisibleAxisC(flag);  break;
  }
}

void AxesVisibility::axisDisable(int index, bool flag)
{
  axisEnable(index, !flag);
}

// ---------- ---------- ---------- ---------- ---------- ---------- ----------
// hide/unhide
// ---------- ---------- ---------- ---------- ---------- ---------- ----------

void AxesVisibility::setVisibleAxisX(bool flag)
{
  ui->label_X            ->setVisible(flag);
  ui->frame_X            ->setVisible(flag);
  ui->doubleSpinBox_WposX->setVisible(flag);
  ui->doubleSpinBox_MposX->setVisible(flag);

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

void AxesVisibility::setVisibleAxisY(bool flag)
{
  ui->label_Y            ->setVisible(flag);
  ui->frame_Y            ->setVisible(flag);
  ui->doubleSpinBox_WposY->setVisible(flag);
  ui->doubleSpinBox_MposY->setVisible(flag);

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

void AxesVisibility::setVisibleAxisZ(bool flag)
{
  ui->label_Z            ->setVisible(flag);
  ui->frame_Z            ->setVisible(flag);
  ui->doubleSpinBox_WposZ->setVisible(flag);
  ui->doubleSpinBox_MposZ->setVisible(flag);
  ui->toolButton_jog_Zp  ->setVisible(flag);
  ui->toolButton_jog_Zm  ->setVisible(flag);

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
  ui->toolButton_speedZ->setVisible(flag);}

void AxesVisibility::setVisibleAxisA(bool flag)
{
  ui->label_A            ->setVisible(flag);
  ui->frame_A            ->setVisible(flag);
  ui->doubleSpinBox_WposA->setVisible(flag);
  ui->doubleSpinBox_MposA->setVisible(flag);

  ui->toolButton_jog_Ap  ->setVisible(flag);
  ui->toolButton_jog_Am  ->setVisible(flag);

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

void AxesVisibility::setVisibleAxisB(bool flag)
{
  ui->label_B            ->setVisible(flag);
  ui->frame_B            ->setVisible(flag);
  ui->doubleSpinBox_WposB->setVisible(flag);
  ui->doubleSpinBox_MposB->setVisible(flag);

  ui->toolButton_jog_Bp  ->setVisible(flag);
  ui->toolButton_jog_Bm  ->setVisible(flag);

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

void AxesVisibility::setVisibleAxisC(bool flag)
{
  ui->label_C            ->setVisible(flag);
  ui->frame_C            ->setVisible(flag);
  ui->doubleSpinBox_WposC->setVisible(flag);
  ui->doubleSpinBox_MposC->setVisible(flag);

  ui->toolButton_jog_Cp  ->setVisible(flag);
  ui->toolButton_jog_Cm  ->setVisible(flag);

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
