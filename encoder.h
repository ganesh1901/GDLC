#ifndef ENCODER_H
#define ENCODER_H

#include <QObject>
#include <variables.h>
#include <QMetaType>
#include <QMessageBox>
#include <encoder.h>
#include <sys/time.h>


#define CCU_HEALTH_CODE 0x55
#define CCU_STATUS_CODE 0x66



class Encoder : public QObject
{
    Q_OBJECT

    QMessageBox *popMsg1;

    GDL_Status_union gdl_status;
    GDL_Health_Union gdl_health;

    GTR_Status_Union gtr_status;
    GTR_Health_Union gtr_health;
    GTR_Diagnosys_Union gtr_diagnosys;


    unsigned char dec_sta_buff[58], dec_hlth_buff[17], dec_dig_buff[28];
    struct timeval t2;


    char e1Buf[103];   //to receive coming data from encoder
    int e1count=0;    //count bytes in packets
    int e1EFlag=0;    //end flag tells last byte has come
    int e1SFlag=0;    //start flag tells the first byte has come


    unsigned char dummy_buff[512];
     int value_count = 0;


public:
    explicit Encoder(QObject *parent = 0);

    void Compute_CRCDL(unsigned char * , int ,unsigned short *);
    void close_Encoder_Socket();

    FILE *dec_hlth_fp, *dec_status_fp, *dec_dig_fp;
    FILE *dec_hlth_fp_raw, *dec_status_fp_raw, *dec_dig_fp_raw;
    FILE *gtr_gdl_status_fp, *gtr_gdl_health_fp;
    FILE *encoder_fp;

signals:

    void send_Radar_Encoder_Data(unsigned char *, int aSize);
    void send_Encoder_Data_Display(QByteArray array,int msg, int id);
    void send_Encoder_Display(unsigned char*, int id);
    void send_Encoder_Data_Parameteres(QByteArray array,int msg, int id);
    void send_to_radar(unsigned char *,int);

public slots:
    void initialize_Encoder_Socket();
    void close_log_Files();
    void openLogFiles();
    void write_to_Encoder(unsigned char *,int );


    void updata_health_data();
    void updata_status_data();



private slots:
    void read_Encoder_Data();    
    unsigned short get_cdma_code(unsigned char *);
    unsigned short get_dncdma_code(unsigned char *);
    unsigned char get_code(unsigned char *);


};

#endif // ENCODER_H
