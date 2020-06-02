#ifndef RADAR_H
#define RADAR_H
#include<iostream>
#include <QObject>
#include<qudpsocket.h>
#include <variables.h>
#include <QMessageBox>
#include <QMetaType>
#include <QChar>
#include<map>
#include<iterator>
#include<string.h>
#include<sys/time.h>
using namespace std;



#define CONF_FILE "config.ini"

#include <arpa/inet.h>

#define POS_SCALE  0.000006667
extern unsigned int app_Chksum;
extern unsigned int app_size;







class Radar : public QObject
{
    Q_OBJECT
public:
    explicit Radar(QObject *parent = 0);

    #pragma pack(1)
    // AKASH-NG CONFIG DATA STRUCTURE
    struct InitMsg2
    {
        unsigned int time_tag;
        unsigned char reserved;
        unsigned char msg_id;
        unsigned short msg_seq_no;
        unsigned short header_crc;
        unsigned char up_freq_chain[2];
        unsigned char down_freq[10];
        unsigned short downlink_cdma_code;
        unsigned short uplink_cdma_code[10];
        unsigned char uplink_fec_selection;
        unsigned char dwnlnk_fec_selection;
        unsigned char power_amplifier;
        unsigned char power_level;
        unsigned short tx_sel;
     } *Init_Msg2;


	union union_version{
        unsigned char buffer[24];
		#pragma pack(1)
		struct {
			unsigned int timetag;
			unsigned char reserved;
			unsigned char msg_id;
            unsigned short seq_no;
            unsigned short msg_crc;

			unsigned int app_csum;
            unsigned char major_version;
            unsigned char minor_version;
			unsigned char date;
			unsigned char month;
            unsigned short year;
            unsigned int app_size;

		}L;
		
	};


    struct timeval t1;



    FILE *ccu_uplink, *packetA, *packetB, *ccu_config, *dnlink;
    unsigned char temp_buffer[512];
    int value_count;


    map<int , vector<unsigned char> >uplink_cdma_index_values_map;
    map<int , vector<unsigned char> >dnlink_cdma_index_values_map;
    map<int , vector<unsigned char> >uplink_freq_values_map;
    map<int , vector<unsigned char> >dnlink_freq_values_map;
    map<int , vector<unsigned char> > :: iterator freq_values_map_itr;
    map<int , vector<unsigned char> > :: iterator cdma_index_values_map_itr;



    QMessageBox *popMsg;
    QUdpSocket *radarSocket,*radar_Txsocket;
    QByteArray enc_b_Array;

    unsigned char pa_config_mode[6];
    unsigned short CRC;

    targetdwell_union  d1;
    Missile_data_union  d2;
    GDL_Health_Union gdl_health;
    GDL_Status_union gdl_status;
    union_version app_version;


    void process_radar_Data(unsigned char aRadarData[], int aRadarSize);
    void processData(QByteArray byteArray,int size);
    void openFiles();
    void closeFiles();
    void close_Sockets();
    void process_to_Config(unsigned char aConfigBuff[],int aConfigBuffSize);
    void process_target_data(unsigned char aTgtBuff[], int aTgtBuffSize);
    void process_missile_data(unsigned char aMslBuff[], int aMslBuffSize);


signals:

     void send_Radar_Data_to_Gui(QByteArray aRadarArray, int aSize);


    void send_Radar_Data_to_Decoder(unsigned char *, int, int);

    void RadarToEncoderData(unsigned char *, int);

    void send_Radar_Ins_Data_to_Gui(QByteArray sendArray);

    void send_RadarData_Encoder(QByteArray array);

    void send_Char_RadarData_Encoder(char array[],int aSize);

    void send_Radar_Data_Decoder(QByteArray array);

    void process_health_request_data();
    void process_status_request_data();


public slots:

    void initialize_Radar();
    void readRadarData();
    void receive_Encoder_Data(unsigned char *, int );
    void receive_Decoder_Data(unsigned char *, int );
    void ComputeCRCDL(unsigned char* , int , unsigned short *);
    void CSUM(unsigned char* , int , unsigned short *);
    unsigned char ChecksumValidity(unsigned char *, unsigned int);
    void Load_map(void);
    float get_scaled(unsigned long, float, float);


};



#endif // RADAR_H
