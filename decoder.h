#ifndef DECODER_H
#define DECODER_H

#include <QObject>
#include <variables.h>
#include <QMessageBox>
#include <QMetaType>







class Decoder : public QObject
{
    Q_OBJECT



    FILE *packetC,*packetD, *packetE;
    FILE *gtr_raw, *gdl_raw;
    QMessageBox *popMsg;

    Packet_C ccu_c;
    Packet_D ccu_d;
    Packet_E ccu_e;



public:
    explicit Decoder(QObject *parent = 0);
    void close_Dec_log_Files();

    unsigned short seq_no;
    int EFlag=0;
    int SFlag=0;
    unsigned char Buf[128];
    int c = 0;
    int dcount=0;
    unsigned char temp[36];
    unsigned char downlink_buffer[72];

    unsigned char temp_buff[512];

    downlink_CCU_Packet dnlink_packet;


signals:
    void send_Radar_Ins_Decoder(unsigned char *, int );
    void send_Decoder_Data_Display(QByteArray array,int msg);
    void send_Decoder_Display(unsigned char *,int msg);

public slots:
    float get_scaled(unsigned long , float , float);
    void process_Decoder_data(unsigned char *, int);
    void initialize_Decoder_Socket();
    void open_Decoder_LogFiles();
    void close_Dec_Srl_Socket();
    void write_Decoder_data(unsigned char *, int , int);
    unsigned short CRC(unsigned char *data, int);


private slots:
    void read_Decoder_Data();

};

#endif // DECODER_H
