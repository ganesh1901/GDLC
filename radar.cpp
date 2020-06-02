
#include <stdio.h>

#include "radar.h"
#include <mainwindow.h>
#include <QUdpSocket>
#include <QThread>
#include <QObject>
#include <QDir>
#include <QTime>
#include <QCoreApplication>
#include <QDebug>
#include <QMessageBox>
#include <QMetaType>
#include <decoder.h>
#include <variables.h>
#include <vector>
#include <QDataStream>


#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

Encoder *Radar_to_Encoder;
Decoder *Radar_to_Decoder;

QHostAddress  RxIpAddr,TxIpAddr;
quint16 RxPort;
quint16 TxPort;
int tm,hh,mm,ss,ms;
unsigned int TimeTag;

char Msg_Id;
char Missile_Id;
unsigned short Msg_SrNo;

int nBytes;
//char srBuf[65];
char szBuf[256];
char szBuf1[256];

char Buffer[255];
char Buffer1[255];

float vx,vy,vz;
float a=1200.0/32768.0;

static unsigned char tmp_buf[74];
static char lookup[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};


Radar::Radar(QObject *parent) : QObject(parent)
{

    popMsg = new QMessageBox();




}

//************************************************************************//
//This function Initializes the Socket with ip and port from GUI
// displays error if anyting is wrong.
//************************************************************************//
void Radar::initialize_Radar()
{



    QString str = QString(reinterpret_cast<unsigned char*>(rxAddr)); //, strlen(rxAddr));
    RxIpAddr.setAddress(str);

    str = QString(reinterpret_cast<unsigned char*>(txAddr));
    TxIpAddr.setAddress(str);   //  Radar..



    RxPort = rxPort; //MainWindow::rxport; //  GDLS..
    TxPort = txPort; //nWindow::txport; //  Radar..

    radarSocket = new QUdpSocket();

    bool status = radarSocket->bind(RxIpAddr, RxPort);


    qDebug() <<"N/W stat :: " << status;

    if(status==false)
    {
        MainWindow::nwStatus = 0;
        emit send_Radar_Data_to_Gui("interNet", 8);
        qDebug()<<"Destination Host Not reachable";

    }
    else
    {
        MainWindow::nwStatus = 1;
        emit send_Radar_Data_to_Gui("Internet ", 7);
        connect(radarSocket,SIGNAL(readyRead()),this,SLOT(readRadarData()));
    }

    Load_map();
}

//************************************************************************//
//This function reads the data from Ethernet and send to process further //
//************************************************************************//
void Radar::readRadarData()
{   
    int dataGramSize;
    unsigned char radArray[256];
    while (radarSocket->hasPendingDatagrams()) //attach to thread
    {
        dataGramSize=radarSocket->pendingDatagramSize();
        radarSocket->readDatagram(radArray, dataGramSize, &RxIpAddr, &RxPort);
        if(MainWindow::e2Status == 0)
        {
#if DEBUG
            qDebug()<<"Coming from radar";
#endif
            process_radar_Data(radArray, dataGramSize);
        }
    }
}



void Radar::ComputeCRCDL(unsigned char *data, int size, unsigned short *CRC)
{
    unsigned short checksum=0X1D0F;
    unsigned char mssg=0;
    unsigned short flag = 0;
    short i=0;

    for(int j=0; j<size; j++)
    {
        if (j!=8 && j!=9)
        {
            mssg = data[j];
            for(i=0;i<8;i++)
            {
                flag=checksum & 0x8000;
                flag>>=8;
                flag=flag ^ mssg;
                flag>>=7;
                checksum<<=1;
                if(flag)
                    checksum^=0x1021;
                mssg<<=1;
            }

        }

    }
    *CRC = ntohs(checksum);
}

void Radar::CSUM(unsigned char *data, int size, unsigned short* CRC)
{
    int i;
     unsigned short crc=0;

     for(i=0; i<size; i++)
    {
        crc+=data[i];
    }
    crc = ~crc+1;
    *CRC = crc & 0x00ff;
    printf("checksum ---- %02x******************\n", *CRC);
}


unsigned char Radar::ChecksumValidity(unsigned char *tmp, unsigned int size){
    unsigned short CRCDL = 0;
    unsigned short tmp_checksum = 0;

    memcpy(&tmp_checksum, &tmp[8] , sizeof CRCDL);

    ComputeCRCDL(tmp , size, &CRCDL);
    if(CRCDL == tmp_checksum)
        return 0;
    else{
        qDebug("check sum %04x Received %04x \n", (CRCDL), (tmp_checksum));

        QByteArray t1  = QByteArray(reinterpret_cast<unsigned char * >(tmp));
        emit send_Radar_Data_to_Gui(t1, 12);

        return 1;
    }

}

//************************************************************************//
//This function does following
//converts the char data to the qByteArray and emits signal for Display
//based on Msg id calls concerned Process
//************************************************************************//
void Radar::process_radar_Data(unsigned char aRadarData[], int aRadarDataSize)
{

    char msgId;

    msgId = aRadarData[5];


    /* For casting to QString to display on the GUI */
    QByteArray b_Array;
    b_Array = QByteArray(reinterpret_cast<char*>(aRadarData+10), aRadarDataSize-10);// to convert char to qbytearray

    switch(msgId)
    {
        case 1:// for target Dwell data
            if(aRadarDataSize == 46)
            {
                if( ChecksumValidity(aRadarData, aRadarDataSize) == 0){

                        process_target_data(aRadarData+10, aRadarDataSize-10);
                        fwrite(aRadarData, aRadarDataSize, 1, ccu_uplink);

                        memset(&d1.buffer, 0, sizeof d1.buffer);
                        memcpy(&d1.buffer, &aRadarData[0], sizeof d1.buffer);

                        fprintf(packetA, "PacketA- %d, %02d, %02d, %02ld, %01ld, %d, %6.4f, %6.4f, %6.4f, %4.2f, %4.2f, %4.2f, %4.2f, %4.2f, %4.2f, %4.2f, %4.2f, %4.2f, %4.2f, %4.2f, %4.2f, %d, %d, %04x \n",
                                d1.L.time_tag, d1.L.msg_id, d1.L.missile_id, d1.L.FU, d1.L.MLV, d1.L.msg_seq_no,
                                get_scaled(d1.L.x_t, 1, 150000), get_scaled(d1.L.y_t, 1, 150000), get_scaled(d1.L.z_t, 1, 60000),
                                get_scaled(d1.L.vx_t, 1, 1500), get_scaled(d1.L.vy_t,  1, 1500), get_scaled( d1.L.vz_t, 1, 1500),
                                get_scaled(d1.L.tgt_ax_t, 1, 100), get_scaled(d1.L.tgt_ay_t, 1, 100), get_scaled(d1.L.tgt_az_t, 1, 100),
                                get_scaled(d1.L.tgt_pos_x, 1, 500), get_scaled(d1.L.tgt_pos_y, 1, 500), get_scaled(d1.L.tgt_pos_z, 1, 500),
                                get_scaled(d1.L.tgt_vel_x, 1, 50), get_scaled(d1.L.tgt_vel_y, 1, 50), get_scaled(d1.L.tgt_vel_z, 1, 50),
                                d1.L.target_type, d1.L.target_RCS, d1.L.crc );
                        fflush(packetA);

                        value_count = sprintf(temp_buffer, "PacketA- %d, %02d, %02d, %02ld, %01ld, %d, %6.4f, %6.4f, %6.4f, %4.2f, %4.2f, %4.2f, %4.2f, %4.2f, %4.2f, %4.2f, %4.2f, %4.2f, %4.2f, %4.2f, %4.2f, %d, %d, %04x \n",
                                d1.L.time_tag, d1.L.msg_id, d1.L.missile_id, d1.L.FU, d1.L.MLV, d1.L.msg_seq_no,
                                get_scaled(d1.L.x_t, 1, 150000), get_scaled(d1.L.y_t, 1, 150000), get_scaled(d1.L.z_t, 1, 60000),
                                get_scaled(d1.L.vx_t, 1, 1500), get_scaled(d1.L.vy_t,  1, 1500), get_scaled( d1.L.vz_t, 1, 1500),
                                get_scaled(d1.L.tgt_ax_t, 1, 100), get_scaled(d1.L.tgt_ay_t, 1, 100), get_scaled(d1.L.tgt_az_t, 1, 100),
                                get_scaled(d1.L.tgt_pos_x, 1, 500), get_scaled(d1.L.tgt_pos_y, 1, 500), get_scaled(d1.L.tgt_pos_z, 1, 500),
                                get_scaled(d1.L.tgt_vel_x, 1, 50), get_scaled(d1.L.tgt_vel_y, 1, 50), get_scaled(d1.L.tgt_vel_z, 1, 50),
                                d1.L.target_type, d1.L.target_RCS, d1.L.crc );
                        temp_buffer[value_count] = '\0';

                        QByteArray b_Array1;
                        b_Array1 = QByteArray(reinterpret_cast<char*>(temp_buffer), value_count);// to convert char to qbytearray

                        emit send_Radar_Data_to_Gui(b_Array1,1);

                }
            }
            break;

        case 2:
            if(aRadarDataSize == 46 )
            {
                if( ChecksumValidity(aRadarData, aRadarDataSize) == 0){
                    emit send_Radar_Data_to_Gui(b_Array,2);
                    process_missile_data(aRadarData+10 ,aRadarDataSize-10);
                    fwrite(aRadarData, aRadarDataSize, 1, ccu_uplink);

                    memset(&d2.buffer, 0, sizeof d2.buffer);
                    memcpy(&d2.buffer, aRadarData, sizeof d2.buffer);
                    fprintf(packetB, "PacketB- %d, %02d, %02d, %02ld, %01ld, %d, %6.4f, %6.4f, %6.4f, %5.2f, %5.2f, %5.2f, %6.4f, %6.4f, %6.4f, %5.2f, %5.2f, %5.2f, %04x \n",
                            d2.L.time_tag, d2.L.msg_id, d2.L.missile_id, d2.L.FU, d2.L.MLV, d2.L.msg_seq_no,
                            get_scaled(d2.L.tgt_pos_x, 1, 150000), get_scaled(d2.L.tgt_pos_y, 1, 150000), get_scaled(d2.L.tgt_pos_z, 1, 60000),
                            get_scaled(d2.L.tgt_vel_x, 1, 1500), get_scaled(d2.L.tgt_vel_y,  1, 1500), get_scaled( d2.L.tgt_vel_z, 1, 1500),
                            get_scaled(d2.L.msl_pos_x, 1, 150000), get_scaled(d2.L.msl_pos_y, 1, 150000), get_scaled(d2.L.msl_pos_z, 1, 60000),
                            get_scaled(d2.L.msl_vel_x, 1, 1500), get_scaled(d2.L.msl_vel_y, 1, 1500), get_scaled(d2.L.msl_vel_z, 1, 1500),
                            d2.L.crc );
                    fflush(packetB);

                }
            }
            break;

        case 3:
            break;


        case 4:
            //configuration
            //********************GDL CONFIGURATION******************************//

            if(aRadarDataSize == 50)
            {
               if( ChecksumValidity(aRadarData, aRadarDataSize) == 0)
                {

                   process_to_Config(aRadarData, aRadarDataSize);
                   emit send_Radar_Data_to_Gui(b_Array, 4);

                }
            }

            break;

        case 5:  //health  request

                emit send_Radar_Data_to_Gui(b_Array, 5); // sending to display

                memset(&gdl_health.GDL_Health_buff ,0x00 ,sizeof gdl_health.GDL_Health_buff);

                /* calling slot by emiting the signal for get the updated health parameters from encoder */
                emit process_health_request_data();
             break;

        case 6:  // status request

                emit send_Radar_Data_to_Gui(b_Array, 6); // sending to display

                /* calling slot by emiting the signal for get the updated status parameters from encoder */
                emit process_status_request_data();
                break;
         case 12:
                emit send_Radar_Data_to_Gui(b_Array, 9); // sending to display

                if(aRadarData[8]){

                    pa_config_mode[0] = 0x02 ;
                    pa_config_mode[1] = 0x37 ;

                    switch (aRadarData[9]) {
                    case 1:
                         pa_config_mode[2] = 0x31;
                         break;
                        /* said by Jp sir */
                    case 2:
                        pa_config_mode[2] = 0x32 ;
                        break;
                    case 3:
                        pa_config_mode[2] = 0x32 ;
                        break;
                    case 4:
                        pa_config_mode[2] = 0x33;
                        break;
                    default:
                        break;
                    }
                }
                else{
                    pa_config_mode[0] = 0x02 ;
                    pa_config_mode[1] = 0x30 ;
                    pa_config_mode[2] = 0x30 ;

                }
                CSUM(&pa_config_mode[1], 3, &CRC);
                memcpy(&pa_config_mode[3], &CRC ,2);
                pa_config_mode[5] = 0x03 ;

                emit send_Radar_Data_to_Decoder(pa_config_mode, 6 , 1);

                break;

    case 14:
        //version details
        struct timeval t2;
        gettimeofday(&t2, NULL);
        app_version.L.msg_id = 0xEE;
        app_version.L.timetag = htonl(t2.tv_sec*1000000+ t2.tv_usec);
        app_version.L.app_csum = htonl(app_Chksum);
        app_version.L.date = 03;
        app_version.L.month = 12;
        app_version.L.year = htons(2019);
        app_version.L.major_version = 1;
        app_version.L.minor_version = 1;
        app_version.L.app_size = htonl(app_size);
        qDebug("checksum -- %X  size -- %d %x size \n",(app_version.L.app_csum), app_version.L.app_size, app_version.L.app_size);
        ComputeCRCDL(app_version.buffer, sizeof app_version.buffer, &app_version.L.msg_crc);
        receive_Encoder_Data(app_version.buffer, sizeof app_version.buffer);
        break;

     default:
            qDebug("Default Case -- %d \n", msgId);
            break;
    }//end of switch
}
//************************************************************************//
//This function does fallowing
// WRITES THE DATA TO THE text file.
//converts the data to the 64 ASCII values.
//put data between 0x02 and 0x03
//sends missile Dwell data to Encoder port
//************************************************************************//


void Radar::process_missile_data(unsigned char aMslBuff[],int aMslBuffSize)
{
    unsigned char tmp[72];
    int j=0;
    unsigned char tmp1;


   /* byte swaping  */
   for(j=0; j<aMslBuffSize; j+=2){

        tmp1 = aMslBuff[j];
        aMslBuff[j] = aMslBuff[j+1];
        aMslBuff[j+1] = tmp1;
    }

    tmp_buf[0] = 0x02;
    for(int i=0; i<aMslBuffSize; i++){
            tmp[i*2] = lookup[ (aMslBuff[i] >>4 )& 0xf]; //LSB
            tmp[i*2+1] = lookup[ aMslBuff[i] & 0xf ]; //MSB
    }
    memcpy(&tmp_buf[1], &tmp, sizeof tmp);
    tmp_buf[73] = 0x03;

    //sending on Encoder port
    emit RadarToEncoderData(tmp_buf, sizeof tmp_buf);

}
//************************************************************************//
//This function does fallowing
// WRITES THE DATA TO THE text file.
//converts the data to the 64 ASCII values.
//put data between 0x02 and 0x03
//sends target Dwell data to Encoder port
//************************************************************************//
//
void Radar::process_target_data(unsigned char aTgtBuff[],int aTgtBuffSize)
{

    unsigned char tmp[72];
    unsigned char tmp1;
    int j;


#if 0
    unsigned short swap_tmp[18], tmp_swap[36];
    memcpy(&swap_tmp, aTgtBuff, sizeof swap_tmp);
    for(int ii=0,jj=0; ii< aTgtBuffSize; ii++){
        tmp_swap[jj++] = swap_tmp[ii] >> 8 ;
        tmp_swap[jj++] = swap_tmp[ii] & 0xff;
    }
#endif

    /* byte swaping */
    for(j=0; j<aTgtBuffSize; j+=2){

       tmp1 = aTgtBuff[j];
       aTgtBuff[j] = aTgtBuff[j+1];
       aTgtBuff[j+1] = tmp1;
    }

    /* converting the HDLC format for DTM */
    tmp_buf[0] = 0x02;
    for(int i=0; i<aTgtBuffSize; i++){
        tmp[i*2] = lookup[ (aTgtBuff[i] >>4 )& 0xf]; //LSB
        tmp[i*2+1] = lookup[ aTgtBuff[i] & 0xf ]; //MSB
    }
    memcpy(&tmp_buf[1], &tmp, sizeof tmp);
    tmp_buf[73] = 0x03;

    /* sending to Rs-422*/
    emit RadarToEncoderData(tmp_buf, sizeof tmp_buf);

#if 0
    /* logging to file */
    for(int k=0; k<sizeof tmp_buf; k++){
        fprintf(gdl_gtr_packeta, " %x",tmp_buf[k]);
    }
    fprintf(gdl_gtr_packeta, "\n");
#endif

#ifdef LOG

    FILE *ff = fopen("/tmp/packetA.bin", "wb");
    if(ff != NULL){
        fwrite(aTgtBuff, 1, aTgtBuffSize, ff);

        fclose(ff);
    }

#endif

}


float Radar::get_scaled(unsigned long t1, float scale_vale, float offset){

    return ( (t1 * scale_vale) - offset);
}



void Radar::Load_map(void){
    //********************TX FREQUENCY VALUES******************************//
    uplink_freq_values_map.insert(pair<int, vector<unsigned char> >(0, {0x30,0x30 }));     //checksum calculated
    uplink_freq_values_map.insert(pair<int, vector<unsigned char> >(1, {0x30,0x31 }));
    uplink_freq_values_map.insert(pair<int, vector<unsigned char> >(2, {0x30,0x32 }));
    uplink_freq_values_map.insert(pair<int, vector<unsigned char> >(3, {0x30,0x33 }));
    uplink_freq_values_map.insert(pair<int, vector<unsigned char> >(4, {0x30,0x34 }));
    uplink_freq_values_map.insert(pair<int, vector<unsigned char> >(5, {0x30,0x35 }));
    uplink_freq_values_map.insert(pair<int, vector<unsigned char> >(6, {0x30,0x36 }));
    uplink_freq_values_map.insert(pair<int, vector<unsigned char> >(7, {0x30,0x37 }));
    uplink_freq_values_map.insert(pair<int, vector<unsigned char> >(8, {0x30,0x38 }));



    //********************RX FREQUENCY VALUES******************************//
    dnlink_freq_values_map.insert(pair<int, vector<unsigned char> >(0,  {0x30,0x30 }));
    dnlink_freq_values_map.insert(pair<int, vector<unsigned char> >(1,  {0x30,0x31 }));
    dnlink_freq_values_map.insert(pair<int, vector<unsigned char> >(2,  {0x30,0x32 }));
    dnlink_freq_values_map.insert(pair<int, vector<unsigned char> >(3,  {0x30,0x33 }));
    dnlink_freq_values_map.insert(pair<int, vector<unsigned char> >(4,  {0x30,0x34 }));
    dnlink_freq_values_map.insert(pair<int, vector<unsigned char> >(5,  {0x30,0x35 }));
    dnlink_freq_values_map.insert(pair<int, vector<unsigned char> >(6,  {0x30,0x36 }));
    dnlink_freq_values_map.insert(pair<int, vector<unsigned char> >(7,  {0x30,0x37 }));
    dnlink_freq_values_map.insert(pair<int, vector<unsigned char> >(8,  {0x30,0x38 }));
    dnlink_freq_values_map.insert(pair<int, vector<unsigned char> >(9,  {0x30,0x39 }));
    dnlink_freq_values_map.insert(pair<int, vector<unsigned char> >(10, {0x31,0x30 }));
    dnlink_freq_values_map.insert(pair<int, vector<unsigned char> >(11, {0x31,0x31 }));
    dnlink_freq_values_map.insert(pair<int, vector<unsigned char> >(12, {0x31,0x32 }));




    //********************UPLINK CDMA VALUES******************************//
    uplink_cdma_index_values_map.insert(pair<int, vector<unsigned char> >(0,  {0x30, 0x30, 0x30}));
    uplink_cdma_index_values_map.insert(pair<int, vector<unsigned char> >(1,  {0x30, 0x30, 0x31}));
    uplink_cdma_index_values_map.insert(pair<int, vector<unsigned char> >(2,  {0x30, 0x30, 0x33}));
    uplink_cdma_index_values_map.insert(pair<int, vector<unsigned char> >(3,  {0x30, 0x30, 0x35}));
    uplink_cdma_index_values_map.insert(pair<int, vector<unsigned char> >(4,  {0x30, 0x30, 0x37}));
    uplink_cdma_index_values_map.insert(pair<int, vector<unsigned char> >(5,  {0x30, 0x30, 0x39}));
    uplink_cdma_index_values_map.insert(pair<int, vector<unsigned char> >(6,  {0x30, 0x30, 0x42}));
    uplink_cdma_index_values_map.insert(pair<int, vector<unsigned char> >(7,  {0x30, 0x30, 0x44}));
    uplink_cdma_index_values_map.insert(pair<int, vector<unsigned char> >(8,  {0x30, 0x30, 0x46}));
    uplink_cdma_index_values_map.insert(pair<int, vector<unsigned char> >(9,  {0x30, 0x31, 0x31}));
    uplink_cdma_index_values_map.insert(pair<int, vector<unsigned char> >(10, {0x30, 0x31, 0x33}));


    //********************DOWNLINK CDMA VALUES******************************//
    dnlink_cdma_index_values_map.insert(pair<int, vector<unsigned char> >(0,   {0x30, 0x30}));
    dnlink_cdma_index_values_map.insert(pair<int, vector<unsigned char> >(1,   {0x30, 0x31}));
    dnlink_cdma_index_values_map.insert(pair<int, vector<unsigned char> >(2,   {0x30, 0x32}));
    dnlink_cdma_index_values_map.insert(pair<int, vector<unsigned char> >(3,   {0x30, 0x33}));
    dnlink_cdma_index_values_map.insert(pair<int, vector<unsigned char> >(4,   {0x30, 0x34}));
    dnlink_cdma_index_values_map.insert(pair<int, vector<unsigned char> >(5,   {0x30, 0x35}));
    dnlink_cdma_index_values_map.insert(pair<int, vector<unsigned char> >(6,   {0x30, 0x36}));
    dnlink_cdma_index_values_map.insert(pair<int, vector<unsigned char> >(7,   {0x30, 0x37}));
    dnlink_cdma_index_values_map.insert(pair<int, vector<unsigned char> >(8,   {0x30, 0x38}));
    dnlink_cdma_index_values_map.insert(pair<int, vector<unsigned char> >(9,   {0x30, 0x39}));
    dnlink_cdma_index_values_map.insert(pair<int, vector<unsigned char> >(10,  {0x31, 0x30}));


}




//************************************************************************//
//This function will configure the Ground Data Link System when ever//
//Configuration message Arrives//
//************************************************************************//
void Radar::process_to_Config(unsigned char aConfigBuff[],int aConfigBuffSize)
{
    Init_Msg2 = (struct InitMsg2 *)aConfigBuff;

    unsigned char config_packet[50];

    //construction of 5 config packets for 5 DTM(Digital Transreceiver Module)

    for (int i=0; i<1; i++)
    {
        config_packet[0]=0x02;  //start delimeter
        config_packet[1]=0x32;  //msg id

        memset(&config_packet[2], 0x30, 50);

        //*************************************UPLINK FREQUENCY ASSIGNMENT****************************************//
        freq_values_map_itr = uplink_freq_values_map.find( Init_Msg2->up_freq_chain[0]);

        if (freq_values_map_itr != uplink_freq_values_map.end() )
        {
            for(int j=0;  j< freq_values_map_itr->second.size(); j++)
            {
                config_packet[2+j]= freq_values_map_itr->second[j];
            }
        }

        //**********************************DOWNLINK FREQUENCY ASSIGNMENT****************************************//
        for (int j=2*i; j<=2*i+1; j++)
        {
            freq_values_map_itr = dnlink_freq_values_map.find(Init_Msg2->down_freq[j]);   //  selected dwn freq 1 out of 13 down freq for AKASH-NG

            if (freq_values_map_itr != dnlink_freq_values_map.end() )
            {
                if (j%2 == 0)
                {
                    for(int k=0;  k< freq_values_map_itr->second.size(); k++)
                    {
                        config_packet[4+k] = freq_values_map_itr->second[k];
                    }
                }
                else
                {
                    for(int k=0;  k< freq_values_map_itr->second.size(); k++)
                    {

                        config_packet[6+k]= freq_values_map_itr->second[k];
                    }
                }
            }
        }
        //printf(" %x %x  %x %x \n", config_packet[4], config_packet[5], config_packet[6], config_packet[7]);

        //*************************************downlink cdma code **********************************************//
        cdma_index_values_map_itr = dnlink_cdma_index_values_map.find((ntohs(Init_Msg2->downlink_cdma_code)));

        if (cdma_index_values_map_itr != dnlink_cdma_index_values_map.end() )
        {
            for(int j=0; j< cdma_index_values_map_itr->second.size(); j++ )
            {
                config_packet[8+j]=cdma_index_values_map_itr->second[j];    //Rx CDMA Channel-1 As per ICD
                config_packet[10+j]=cdma_index_values_map_itr->second[j];   //Rx CDMA Channel-2 Field Reserved So Copied Same RX CDMA Channel-1
            }
        }

        //*************************************uplink cdma code **********************************************//
        for (int j=0; j<10; j++)
        {
            cdma_index_values_map_itr = uplink_cdma_index_values_map.find(ntohs(Init_Msg2->uplink_cdma_code[j]));
            if (cdma_index_values_map_itr != uplink_cdma_index_values_map.end() )
            {
                for(int k=0; k< cdma_index_values_map_itr->second.size(); k++ )
                {
                    //long , tx_time;
                    config_packet[12+3*j+k]=cdma_index_values_map_itr->second[k];   //3 Bytes from Uplink CDMA Map
                }
            }
        }

        //*******************************************FECs ASSIGNMENT*********************************************//


        if(Init_Msg2->dwnlnk_fec_selection == 0xff) //Rx FEC
        {
            config_packet[42]=0x31;
        }
        else
        {
            config_packet[42]=0x30;
        }

        if(Init_Msg2->uplink_fec_selection == 0xff) //Tx FEC
        {
            config_packet[43]=0x31;
        }
        else
        {
            config_packet[43]=0x30;
        }

        //*********************************************TX PA ON/OFF***************************************//

        if (Init_Msg2->power_amplifier == 0xff)
        {
            config_packet[44]=0x31;
        }
        else
        {
            config_packet[44]=0x30;
        }

        //***********************************Power mode selection or power_level(LSB)****************************//


        switch(Init_Msg2->power_level)
        {
            case 1:
                config_packet[45]=0x31;
                break;

            case 2:
                config_packet[45]=0x32;
                break;

            case 3:
                config_packet[45]=0x32;
                break;

            case 4:
                config_packet[45]=0x33;
                break;

            default:
                config_packet[45]=0x30;
                break;
        }

        if( Init_Msg2->power_amplifier == 0){
            config_packet[44]=0x30;
            config_packet[45]=0x30;

        }

        //************************************TX Antenna SELECTION ASSIGNMENT*******************************************************//

        if (Init_Msg2->tx_sel == 0xffff)

        {
            config_packet[46]=0x31;
        }
        else{
            config_packet[46]=0x30;
        }

        //*********************checksum calculation excluding first and last byte***********************************************//

        CSUM(&config_packet[1], 46, &CRC);

        config_packet[48] = lookup[ CRC & 0xf] ;
        config_packet[47] = lookup[ (CRC>>4) & 0xf];

        config_packet[49] = 0x03;


#if 0
        char temp[] = { 0x02,0x32,0x30,0x30,0x30,0x30,0x30,0x31,0x30,0x31,0x30,0x31,0x30,0x30,0x31,0x30,0x30,
                                     0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,
                                     0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x31,0x33,0x31,0x35,0x35,0x03};

#endif


#if 1
        unsigned char *temp = (unsigned char *)malloc(50);
        memcpy(temp, config_packet, sizeof config_packet);
        emit send_Radar_Data_to_Decoder(temp, 50, 0);
#endif



    }
}



//************************************************************************//
//This function opens all file to log data //
//************************************************************************//
void Radar::openFiles()
{
    QString filePath;


    filePath.clear();
    filePath.append(MainWindow::dirPath+"/PacketA.txt");
    packetA = fopen(filePath.toUtf8(),"w");
    if(packetA == NULL)
    {
        popMsg->setText("FILE OPEN ERROR"+filePath);
        popMsg->show();
    }

    filePath.clear();
    filePath.append(MainWindow::dirPath+"/PacketB.txt");
    packetB = fopen(filePath.toUtf8(),"w");
    if(packetB == NULL)
    {
        popMsg->setText("FILE OPEN ERROR" + filePath);
        popMsg->show();
    }

    filePath.clear();
    filePath.append(MainWindow::dirPath+"/CCU_RAW.bin");
    ccu_uplink = fopen(filePath.toUtf8(),"wb");
    if(ccu_uplink  == NULL)
    {
        popMsg->setText("FILE OPEN ERROR" + filePath);
        popMsg->show();
    }


    filePath.clear();
    filePath.append(MainWindow::dirPath+"/Config.txt");
    ccu_config = fopen(filePath.toUtf8(),"w");
    if(ccu_config == NULL)
    {
        popMsg->setText("FILE OPEN ERROR" + filePath);
        popMsg->show();
    }


    filePath.clear();
    filePath.append(MainWindow::dirPath+"/Downlink.bin");
    dnlink = fopen(filePath.toUtf8(),"wb");
    if(dnlink == NULL)
    {
        popMsg->setText("FILE OPEN ERROR" + filePath);
        popMsg->show();
    }

}

//************************************************************************//
//This function closes all the file pointers//
//************************************************************************//
void Radar::closeFiles()
{
   if(ccu_config != NULL )
       fclose(ccu_config);
   if(ccu_uplink != NULL)
       fclose(ccu_uplink);
   if(packetA != NULL)
       fclose(packetA);
   if(packetB != NULL)
       fclose(packetB);
   if(dnlink != NULL)
       fclose(dnlink);
}

//************************************************************************//
//This function closes the active sockets  //
//************************************************************************//
void Radar::close_Sockets()
{
    if(radarSocket->isOpen())
        radarSocket->close();
}

//************************************************************************//
//This function writes data (Health / Status) to Radar Socket
//************************************************************************//
void Radar::receive_Encoder_Data(unsigned char *array, int size)
{
    //qDebug()<<"  Sending Encoder data to RC on Request \n";
    radarSocket->writeDatagram(array, size, TxIpAddr,TxPort);

}

//************************************************************************//
//This function writes data(INS (DOWNLINK)) to Radar Socket
//************************************************************************//
void Radar::receive_Decoder_Data(unsigned char *array, int size)
{
    fwrite(array, size, 1 , dnlink);
    radarSocket->writeDatagram(array, size, TxIpAddr, TxPort);
}
