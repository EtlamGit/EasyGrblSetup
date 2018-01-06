// Copyright 2017 EtlamGit

#ifndef CONFIGWATCHER_H
#define CONFIGWATCHER_H

#include <QObject>
#include <QStringList>


namespace Ui {
  class MainWindow;
}

class ConfigWatcher : public QObject
{
  Q_OBJECT

public:
  explicit ConfigWatcher(Ui::MainWindow *pui);
  ~ConfigWatcher();

  bool    isConfigured();
  bool    isCommandAvailable();
  QString getCommand();

public slots:
//  void setEnabled(bool flag);
//  void resetAxisIndex();
  void reset();
  void setAxisIndex(int axis, int value);

  bool handleAxesQueryOk();
  bool handleAxesQueryError();

  void parseGrblConfig(QString line);

private slots:
  void updateConfig0();
  void updateConfig1();
  void updateConfig2();
  void updateConfig3();
  void updateConfig4();
  void updateConfig5();
  void updateConfig6();
  void updateConfig10();
  void updateConfig11();
  void updateConfig12();
  void updateConfig13();
  void updateConfig20();
  void updateConfig21();
  void updateConfig22();
  void updateConfig23();
  void updateConfig24();
  void updateConfig25();
  void updateConfig26();
  void updateConfig27();
  void updateConfig30();
  void updateConfig31();
  void updateConfig32();

  void updateConfig100X();
  void updateConfig100Y();
  void updateConfig100Z();
  void updateConfig100A();
  void updateConfig100B();
  void updateConfig100C();
  void updateConfig110X();
  void updateConfig110Y();
  void updateConfig110Z();
  void updateConfig110A();
  void updateConfig110B();
  void updateConfig110C();
  void updateConfig120X();
  void updateConfig120Y();
  void updateConfig120Z();
  void updateConfig120A();
  void updateConfig120B();
  void updateConfig120C();
  void updateConfig130X();
  void updateConfig130Y();
  void updateConfig130Z();
  void updateConfig130A();
  void updateConfig130B();
  void updateConfig130C();

  void appendConfig(int pnum, int value);
  void appendConfig(int pnum, double value);

  void setVisibleX(bool flag);
  void setVisibleY(bool flag);
  void setVisibleZ(bool flag);
  void setVisibleA(bool flag);
  void setVisibleB(bool flag);
  void setVisibleC(bool flag);

private:
  Ui::MainWindow *ui;
  bool           m_axes_configured;  // flag if axes are configured
  bool           m_axes_query_send;  // flag if axes query commmands are send
  bool           m_axes_query;       // flag if axes query is running
  int            m_axes_test;        // number which axis is tested at the moment
  int            m_reg_num;          // number of axes estimated for machine
  bool           m_enabled;          // flag to temporarily disable when reading a config
  QStringList    m_commands;         // buffer for config write commands
  int            m_status_count;     // counter to query config only every Nth status update

public:
  int m_reg2axes[6];                 // axes number for 6 possible register numbers (reg num in => axes num out)
  int m_axes2reg[6];                 // register number offset for 6 possible axes  (axes num in => reg num out)
  int m_axes2mask[6];                // mask for 6 possible axes
};

#endif // CONFIGWRITER_H
