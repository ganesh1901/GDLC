#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <radar.h>
#include <encoder.h>
#include <decoder.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <QStandardItemModel>
#include <list>


namespace Ui {
class MainWindow;

}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    static QString dirPath,txIpStr,rxIpStr;

    Radar *radar_Class;
    Encoder *encoder_Class;
    Decoder *decoder_Class;

    GDL_Health_Union gui_gdl_health;
    GDL_Status_union gui_gdl_status;



    QThread *radar_Thd,*Encoder_Thd,*Decoder_Thd;

    static char Missile_Id;
    static unsigned short pLevel;
    static unsigned short txSelect;
    static unsigned int e1Status;
    static unsigned int e2Status;
    static unsigned int d1Status;
    static unsigned int d2Status;
    static unsigned int nwStatus;




    void start_Application();

    unsigned int calculateFileChecksum();
    unsigned int computeChecksum(char *buff, unsigned int buffLen);
    void displayChecksum(char *cStr);
    void getConfig();

    void AddTableView();


 signals:
    void test(unsigned char *,  int, int);

public slots:

    void display_Radar_Data(QByteArray recArray, int arecArraySize);
    void receive_Encoder_Display(QByteArray recArray, int msg, int id);
    void receive_Decoder_Display(QByteArray recArray, int msg);
    void receive_display(unsigned char *, int);

  

private slots:
    //   void on_pb_start_clicked();
    void on_pb_close_clicked();
    void on_pb_radar_windw_clicked();
    void on_pb_ground_windw_clicked();
    void on_pb_parameters_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pb_radar_clicked();

    void on_pb_ground_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
