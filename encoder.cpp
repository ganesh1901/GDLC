#include "encoder.h"
#include <QSerialPort>
#include <mainwindow.h>

#include <QMetaType>
#include <QMessageBox>
#include <QObject>

#include <QDebug>

#include <QDir>
#include <QtCore/qiodevice.h>
#include <map>
#include "qserialport.h"

QSerialPort *EncoderPort;
QSerialPort *EncoderPort2;

extern unsigned char encodePort1[16];



Encoder::Encoder(QObject *parent) : QObject(parent)
{
    popMsg1 = new QMessageBox();
}

//************************************************************************//
//This function Initialize the Encoder serialport settings //
//************************************************************************//
void Encoder::initialize_Encoder_Socket()
{
    QByteArray Array;

    EncoderPort = new QSerialPort(encodePort1,this);
    bool b = EncoderPort->open(QIODevice::ReadWrite);


    if(b == true)
    {
        MainWindow::e1Status = 1;
        emit send_Encoder_Data_Display(Array,13,3);
        EncoderPort->setPortName("Ground");
        EncoderPort->setBaudRate(QSerialPort::Baud115200);
        EncoderPort->setDataBits(QSerialPort::Data8);
        EncoderPort->setParity(QSerialPort::NoParity);
        EncoderPort->setFlowControl(QSerialPort::NoFlowControl);
        connect(EncoderPort, SIGNAL(readyRead()),this, SLOT(read_Encoder_Data()));
    }
    else
    {
        MainWindow::e1Status = 0;
        emit send_Encoder_Data_Display(Array,13,1);
    }

}


void Encoder::read_Encoder_Data()
{
    unsigned char c;
    char *dataBuffer;
    dataBuffer = new char[2];

    while(EncoderPort->bytesAvailable())
    {
        c = EncoderPort->read(dataBuffer, 1);

        if(c){

            if(dataBuffer[0] == 0x02)
            {
                e1count = 0;
                e1EFlag = 0;
                e1SFlag = 1;
            }
            if((e1SFlag == 1) && (e1EFlag == 0))
            {

                e1Buf[e1count] = dataBuffer[0];
                e1count++;
            }
            if(dataBuffer[0] == 0x03)
            {
                e1SFlag = 0;
                e1EFlag = 1;
            }


            if((e1count == 17) && (e1EFlag == 1))
            {
                memcpy(&dec_hlth_buff, &e1Buf, e1count);
                fwrite(&dec_hlth_buff, e1count, 1, dec_dig_fp_raw);

                for (int i=0;i<e1count;i++)
                {
                    if (dec_hlth_buff[i]  < 0x40)
                        dec_hlth_buff[i] = dec_hlth_buff[i] - 0x30;
                    else
                        dec_hlth_buff[i] = dec_hlth_buff[i] - 0x41 +0x0a;
                }
                memcpy(&gtr_health.gtr_health_buff[0], &dec_hlth_buff[0], sizeof(gtr_health.L));

            }
            else if((e1count == 58) && (e1EFlag == 1))
            {

                memcpy(&dec_sta_buff, &e1Buf, e1count);
                fwrite(&dec_sta_buff, e1count, 1 , dec_status_fp_raw);

                for (int i=0;i<e1count;i++)
                {
                    if (dec_sta_buff[i]  < 0x40)
                        dec_sta_buff[i] = dec_sta_buff[i] - 0x30;
                    else
                        dec_sta_buff[i] = dec_sta_buff[i] - 0x41 +0x0a;
                }

                //printf(" 2-%d 3-%d 38-%d 39-%d 43-%d 44-%d 45-%d \n", dec_sta_buff[2], dec_sta_buff[3], dec_sta_buff[38], dec_sta_buff[39], dec_sta_buff[43], dec_sta_buff[44], dec_sta_buff[45]);
                memcpy(&gtr_status.gtr_status_buff, &dec_sta_buff[0], sizeof(gtr_status.L));

                //fprintf(stdout, "PA  %d %d \n", gtr_status.L.tx_power_amp_st , gtr_status.L.tx_power_level);
                e1count = 0;


            }

            else if((e1count == 28) && (e1EFlag == 1))
            {

                memcpy(&dec_dig_buff, &e1Buf, e1count);
                fwrite(&dec_dig_buff, e1count, 1, dec_dig_fp_raw);


                for (int i=1;i<e1count;i++)
                {
                    if (dec_dig_buff[i]  < 0x40)
                        dec_dig_buff[i] = dec_dig_buff[i] - 0x30;
                    else
                        dec_dig_buff[i] = dec_dig_buff[i] - 0x41 +0x0a;
                }
                memcpy(&gtr_diagnosys.gtr_diagnosys_buff[0], &dec_dig_buff[0], sizeof(gtr_diagnosys.L));

                updata_health_data();
                updata_status_data();

              }
        }//end of byte available
    }//end of while
}

//************************************************************************/
//This function does the faloowing
//This function assigns the all the parameters of GDL Health
// emits signals for Display and sending GDLU HEALTH data to RADAR
//************************************************************************//
void Encoder::updata_health_data()
{

    //qDebug() << " Processing Encoder Data  for health  " << endl;

    static unsigned int msg_seq = 0;
    unsigned char Buffer[8], Buff[29];
    unsigned short CRC=0;
    long time_tag ;



    gettimeofday(&t2, NULL);
    time_tag = t2.tv_sec*1000000 + t2.tv_usec;

    gdl_health.L.time_tag = htonl(0x12345678);
    gdl_health.L.reserved = 0;
    gdl_health.L.msg_id = CCU_HEALTH_CODE;
    gdl_health.L.msg_seq_num = htons(msg_seq++);

    memcpy(Buffer, &gdl_health.L, 8);
    Compute_CRCDL(Buffer, sizeof Buffer, &CRC);

    gdl_health.L.header_csum = htons(CRC);

    gdl_health.L.dual_rx_index=1;     //No field found in ICD from DTM to GDLC for dual index


    if( gtr_status.L.rx1_c_band_ld_st == 1  && gtr_status.L.rx2_c_band_ld_st == 1)
        gdl_health.L.rx_pll_lock = 0xff;
    else if ( gtr_status.L.rx1_c_band_ld_st == 1  && gtr_status.L.rx2_c_band_ld_st == 0)
        gdl_health.L.rx_pll_lock = 0xf0;
    else if ( gtr_status.L.rx1_c_band_ld_st == 0  && gtr_status.L.rx2_c_band_ld_st == 1) 
        gdl_health.L.rx_pll_lock = 0x0f;
    else
        gdl_health.L.rx_pll_lock = 0x0;
	
    if ( gtr_status.L.tx_c_band_ld_st == 1)
        gdl_health.L.tx_pll_lock = 0xf0;
    else
        gdl_health.L.tx_pll_lock = 0;

    if(gtr_status.L.rx_ch1_track_status)
        gdl_health.L.decoder1_track_acq = 0xff;
    else
        gdl_health.L.decoder1_track_acq = 0x0;

    if(gtr_status.L.rx_ch2_track_status)
        gdl_health.L.decoder2_track_acq = 0xff;
    else
        gdl_health.L.decoder2_track_acq = 0;

    gdl_health.L.rx1_rssi = (gtr_health.L.agc1_rssi[1]<<4|gtr_health.L.agc1_rssi[2]);
    gdl_health.L.rx2_rssi = (gtr_health.L.agc2_rssi[1]<<4|gtr_health.L.agc2_rssi[2]);


    if (gtr_status.L.tx_power_amp_st)
    {
        switch(gtr_status.L.tx_power_level){
            case 1:
                gdl_health.L.txpwr_mode = htons(0xff01);
                    break;
            case 2:
                gdl_health.L.txpwr_mode = htons(0xff02);
                break;
            case 3:
                gdl_health.L.txpwr_mode = htons(0xff03);
                break;

            default:
                gdl_health.L.txpwr_mode = 0x0000;
                break;
        }

    }
    else{
        gdl_health.L.txpwr_mode = 0x0000;
    }

    if (gtr_diagnosys.L.channel_no==0x31)
    {
        gdl_health.L.rx1_dopler=atoi((const char*)gtr_diagnosys.L.rx_frequency);
        gdl_health.L.rx2_dopler=0;
    }
    else
    {
        gdl_health.L.rx2_dopler=atoi((const char*)gtr_diagnosys.L.rx_frequency);
        gdl_health.L.rx1_dopler=0;
    }


    if(gtr_health.L.correlator1_status)
        gdl_health.L.rx1_correlation= 0xff;
    else
        gdl_health.L.rx1_correlation= 0x0;

    if(gtr_health.L.correlator2_status)
         gdl_health.L.rx2_correlation  = 0xff;
    else
        gdl_health.L.rx2_correlation  = 0x0;

    if (gtr_status.L.tx_antenna_selection)
        gdl_health.L.rf_switch_status = htons(0xffff);
    else
        gdl_health.L.rf_switch_status = 0x0;

    emit send_Radar_Encoder_Data(gdl_health.GDL_Health_buff, sizeof gdl_health.GDL_Health_buff);




    fprintf(dec_hlth_fp, "%08x,%02x,%02x,%04d,%04x,%02x,%02x,%02x,%02x,%02x,%02d,%02d,%04x,%04x,%04x,%04x,%04x,%04x\n",
               ntohl(gdl_health.L.time_tag), gdl_health.L.reserved, gdl_health.L.msg_id, ntohs(gdl_health.L.msg_seq_num), ntohs(gdl_health.L.header_csum), gdl_health.L.dual_rx_index,
               gdl_health.L.rx_pll_lock, gdl_health.L.tx_pll_lock, gdl_health.L.decoder1_track_acq, gdl_health.L.decoder2_track_acq, gdl_health.L.rx1_rssi,
               gdl_health.L.rx2_rssi, ntohs(gdl_health.L.txpwr_mode), ntohs(gdl_health.L.rx1_dopler), ntohs(gdl_health.L.rx2_dopler), ntohs(gdl_health.L.rx1_correlation),
               ntohs(gdl_health.L.rx2_correlation), ntohs(gdl_health.L.rf_switch_status));
    fflush(dec_hlth_fp);


    value_count = sprintf(dummy_buff, "%08x,%02x,%02x,%04d,%04x,%02x,%02x,%02x,%02x,%02x,%02d,%02d,%04x,%04x,%04x,%04x,%04x,%04x\n",
               ntohl(gdl_health.L.time_tag), gdl_health.L.reserved, gdl_health.L.msg_id, ntohs(gdl_health.L.msg_seq_num), ntohs(gdl_health.L.header_csum), gdl_health.L.dual_rx_index,
               gdl_health.L.rx_pll_lock, gdl_health.L.tx_pll_lock, gdl_health.L.decoder1_track_acq, gdl_health.L.decoder2_track_acq, gdl_health.L.rx1_rssi,
               gdl_health.L.rx2_rssi, ntohs(gdl_health.L.txpwr_mode), ntohs(gdl_health.L.rx1_dopler), ntohs(gdl_health.L.rx2_dopler), ntohs(gdl_health.L.rx1_correlation),
               ntohs(gdl_health.L.rx2_correlation), ntohs(gdl_health.L.rf_switch_status));

    dummy_buff[value_count] = '\0';

    QByteArray t1 ;
    t1 = QByteArray( reinterpret_cast< char *>(dummy_buff), value_count);
    emit send_Encoder_Data_Display(t1, 1, 1);


}


void Encoder::updata_status_data()
{

    static unsigned short msg_seq_num = 0;

	memset(&gdl_status.L, 0x00, sizeof gdl_status.L);

	unsigned char Buffer[8], Buff[57];
	unsigned short CRC=0;
	long time_tag ;


	gettimeofday(&t2, NULL);
	time_tag = t2.tv_sec*1000000+ t2.tv_usec;

    gdl_status.L.time_tag = htonl(time_tag);
    gdl_status.L.reserved = 0;
    gdl_status.L.msg_id = CCU_STATUS_CODE;

	gdl_status.L.msg_seq_num = htons(msg_seq_num++);

	memcpy(Buffer, &gdl_status.L, 8);
	Compute_CRCDL(Buffer, sizeof Buffer, &CRC);

	gdl_status.L.header_csum=CRC;

    gdl_status.L.dual_rx_index = 1;

	unsigned char *tmp2 = gtr_status.L.tx_pll_index;
	
	gdl_status.L.uplink_freq[0] = get_code(tmp2);
//	printf(" &&&&&&&&&& %d [%d[%d] %d[%d]]", get_code(tmp2), tmp2[0], gtr_status.L.tx_pll_index[0],  tmp2[1], gtr_status.L.tx_pll_index[1] );
	gdl_status.L.uplink_freq[1] = 0;

	tmp2 = gtr_status.L.rx1_pll_index;
	gdl_status.L.dnlink_freq[0] = get_code(tmp2);
//	printf(" %d[%d[%d] %d[%d]] \n", get_code(tmp2), tmp2[0], gtr_status.L.rx1_pll_index[0], tmp2[1], gtr_status.L.rx1_pll_index[1]);

	tmp2 = gtr_status.L.rx2_pll_index;
	gdl_status.L.dnlink_freq[1] = get_code(tmp2);
//	printf(" %d[%d[%d] %d[%d]] \n", get_code(tmp2), tmp2[0], gtr_status.L.rx2_pll_index[0],  tmp2[1], gtr_status.L.rx2_pll_index[1]);

	for(int i=2; i<10; i++)
	{
		gdl_status.L.dnlink_freq[i] = 0;
	}
	
	unsigned char *tmp1 = gtr_status.L.rx_cdma[0];
	gdl_status.L.dnlink_cdma = htons(get_dncdma_code(tmp1));
	

	for(int i=0; i<10; i++)
	{
		unsigned char *tmp = gtr_status.L.tx_cdma[i];
		//printf(" %d %d %d", gtr_status.L.tx_cdma[i][0], gtr_status.L.tx_cdma[i][1], gtr_status.L.tx_cdma[i][2]); 
		gdl_status.L.uplink_cdma[i] = htons(get_cdma_code(tmp));
	}

	if(gtr_status.L.tx_antenna_selection)
		gdl_status.L.tx_antenna_status = 0xFFFF;
	else
		gdl_status.L.tx_antenna_status = 0x0000;


	if (gtr_status.L.tx_power_amp_st)
	{
        switch(gtr_status.L.tx_power_level){
            case 1:
                gdl_status.L.pa_status = htons(0xff01);
                break;
            case 2:
                gdl_status.L.pa_status = htons(0xff02);
                break;
            case 3:
                gdl_status.L.pa_status = htons(0xff04);
                break;

            default:
                gdl_status.L.pa_status = 0x0000;
                break;
        }

	}
	else{
		gdl_status.L.pa_status = 0x0000;
	}


	//printf(" Ant %d, power -- %d %d  %04x \n", gtr_status.L.tx_antenna_selection ,gtr_status.L.tx_power_amp_st, gtr_status.L.tx_power_level, gdl_status.L.pa_status );
    gdl_status.L.rx1_rssi = (gtr_health.L.agc1_rssi[1] << 4 | gtr_health.L.agc1_rssi[2]);
    gdl_status.L.rx2_rssi = (gtr_health.L.agc2_rssi[1] << 4 | gtr_health.L.agc2_rssi[2]);


	if( gtr_status.L.rx1_c_band_ld_st == 1  && gtr_status.L.rx2_c_band_ld_st == 1)
		gdl_status.L.rx_pll_lock = 0xff;
	else if ( gtr_status.L.rx1_c_band_ld_st == 1  && gtr_status.L.rx2_c_band_ld_st == 0)
		gdl_status.L.rx_pll_lock = 0xf0;
	else if ( gtr_status.L.rx1_c_band_ld_st == 0  && gtr_status.L.rx2_c_band_ld_st == 1)
		gdl_status.L.rx_pll_lock = 0x0f;
	else
		gdl_status.L.rx_pll_lock = 0x0;


	if(gtr_status.L.tx_c_band_ld_st)
        gdl_status.L.tx_pll_lock = 0xf0;
	else
		gdl_status.L.tx_pll_lock = 0;

	if(gtr_status.L.rx_ch1_track_status)
		gdl_status.L.decoder1_track_acq = 0xff; 
	else
		gdl_status.L.decoder1_track_acq = 0; 

	if(gtr_status.L.rx_ch2_track_status)
		gdl_status.L.decoder2_track_acq = 0xff;
	else
		gdl_status.L.decoder2_track_acq = 0;

	if(gtr_status.L.rx_fec)
		gdl_status.L.dnlink_fec_sel = 0xff;
	else   
		gdl_status.L.dnlink_fec_sel = 0;

	if(gtr_status.L.tx_fec)
		gdl_status.L.uplink_fec_sel = 0xff;
	else
		gdl_status.L.uplink_fec_sel = 0;

	memcpy(Buff, &gdl_status.L, sizeof(Buff));

	emit send_Radar_Encoder_Data(Buff, sizeof Buff);




    fprintf(dec_status_fp, "%08x,%02x,%02x,%04d,%04x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,"
                        "%04x,%04x,%04x,%04x,%04x,%04x,%04x,%04x,%04x,%04x,%04x,%04x, %04x,%02d,%02d,%02x,%02x,%02x,%02x,"
                        "%02x,%02x \n",
                ntohl(gdl_status.L.time_tag), gdl_status.L.reserved, gdl_status.L.msg_id, ntohs(gdl_status.L.msg_seq_num), ntohs(gdl_status.L.header_csum), gdl_status.L.dual_rx_index,
                gdl_status.L.uplink_freq[0], gdl_status.L.uplink_freq[1], gdl_status.L.dnlink_freq[0], gdl_status.L.dnlink_freq[1], gdl_status.L.dnlink_freq[2],
                gdl_status.L.dnlink_freq[3], gdl_status.L.dnlink_freq[4], gdl_status.L.dnlink_freq[5], gdl_status.L.dnlink_freq[6], gdl_status.L.dnlink_freq[7],
                gdl_status.L.dnlink_freq[8], gdl_status.L.dnlink_freq[9], ntohs(gdl_status.L.dnlink_cdma), ntohs(gdl_status.L.uplink_cdma[0]), ntohs(gdl_status.L.uplink_cdma[1]),
                ntohs(gdl_status.L.uplink_cdma[2]), ntohs(gdl_status.L.uplink_cdma[3]), ntohs(gdl_status.L.uplink_cdma[4]), ntohs(gdl_status.L.uplink_cdma[5]), ntohs(gdl_status.L.uplink_cdma[6]),
                ntohs(gdl_status.L.uplink_cdma[7]), ntohs(gdl_status.L.uplink_cdma[8]), ntohs(gdl_status.L.uplink_cdma[9]), ntohs(gdl_status.L.tx_antenna_status), ntohs(gdl_status.L.pa_status),
                gdl_status.L.rx1_rssi, gdl_status.L.rx2_rssi, gdl_status.L.rx_pll_lock, gdl_status.L.tx_pll_lock, gdl_status.L.decoder1_track_acq, gdl_status.L.decoder2_track_acq,
                gdl_status.L.dnlink_fec_sel, gdl_status.L.uplink_fec_sel);


    value_count = sprintf(dummy_buff, "%08x,%02x,%02x,%04d,%04x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,"
                                        "%04x,%04x,%04x,%04x,%04x,%04x,%04x,%04x,%04x,%04x,%04x,%04x, %04x,%02d,%02d,%02x,%02x,%02x,%02x,"
                                        "%02x,%02x \n",
                                ntohl(gdl_status.L.time_tag), gdl_status.L.reserved, gdl_status.L.msg_id, ntohs(gdl_status.L.msg_seq_num), ntohs(gdl_status.L.header_csum), gdl_status.L.dual_rx_index,
                                gdl_status.L.uplink_freq[0], gdl_status.L.uplink_freq[1], gdl_status.L.dnlink_freq[0], gdl_status.L.dnlink_freq[1], gdl_status.L.dnlink_freq[2],
                                gdl_status.L.dnlink_freq[3], gdl_status.L.dnlink_freq[4], gdl_status.L.dnlink_freq[5], gdl_status.L.dnlink_freq[6], gdl_status.L.dnlink_freq[7],
                                gdl_status.L.dnlink_freq[8], gdl_status.L.dnlink_freq[9], ntohs(gdl_status.L.dnlink_cdma), ntohs(gdl_status.L.uplink_cdma[0]), ntohs(gdl_status.L.uplink_cdma[1]),
                                ntohs(gdl_status.L.uplink_cdma[2]), ntohs(gdl_status.L.uplink_cdma[3]), ntohs(gdl_status.L.uplink_cdma[4]), ntohs(gdl_status.L.uplink_cdma[5]), ntohs(gdl_status.L.uplink_cdma[6]),
                                ntohs(gdl_status.L.uplink_cdma[7]), ntohs(gdl_status.L.uplink_cdma[8]), ntohs(gdl_status.L.uplink_cdma[9]), ntohs(gdl_status.L.tx_antenna_status), ntohs(gdl_status.L.pa_status),
                                gdl_status.L.rx1_rssi, gdl_status.L.rx2_rssi, gdl_status.L.rx_pll_lock, gdl_status.L.tx_pll_lock, gdl_status.L.decoder1_track_acq, gdl_status.L.decoder2_track_acq,
                                gdl_status.L.dnlink_fec_sel, gdl_status.L.uplink_fec_sel);
     dummy_buff[value_count] = '\0';

     emit send_Encoder_Display(gdl_status.sta_buff, 1);

     QByteArray t1 ;
     t1 = QByteArray(reinterpret_cast< char *>(dummy_buff), value_count);
     emit send_Encoder_Data_Display(t1, 2, 1);

}


//************************************************************************//
//This function opens the log files fro seril data //
//************************************************************************//
void Encoder::openLogFiles()
{
    QString filePath;

    filePath.clear();
    filePath.append(MainWindow::dirPath+"/Dec_HEALTH.txt");
    dec_hlth_fp = fopen(filePath.toUtf8(),"w+");
    if(dec_hlth_fp  == NULL)
    {
        popMsg1->setText("FILE OPEN ERROR "+ filePath);
        popMsg1->show();
    }


    filePath.clear();
    filePath.append(MainWindow::dirPath+"/Dec_STATUS.txt");
    dec_status_fp = fopen(filePath.toUtf8(),"w+");
    if(dec_status_fp  == NULL)
    {
        popMsg1->setText("FILE OPEN ERROR "+ filePath);
        popMsg1->show();
    }


    filePath.clear();
    filePath.append(MainWindow::dirPath+"/Dec_Dignostics.txt");
    dec_dig_fp = fopen(filePath.toUtf8(),"w+");
    if(dec_dig_fp  == NULL)
    {
        popMsg1->setText("FILE OPEN ERROR "+ filePath);
        popMsg1->show();
    }


    filePath.clear();
    filePath.append(MainWindow::dirPath+"/Diag_Raw.bin");
    dec_dig_fp_raw = fopen(filePath.toUtf8(),"wb");
    if(dec_dig_fp_raw  == NULL)
    {
        popMsg1->setText("FILE OPEN ERROR "+ filePath);
        popMsg1->show();
    }


    filePath.clear();
    filePath.append(MainWindow::dirPath+"/Health_Raw.bin");
    dec_hlth_fp_raw = fopen(filePath.toUtf8(),"wb");
    if(dec_hlth_fp_raw  == NULL)
    {
        popMsg1->setText("FILE OPEN ERROR "+ filePath);
        popMsg1->show();
    }



    filePath.clear();
    filePath.append(MainWindow::dirPath+"/Status_Raw.bin");
    dec_status_fp_raw = fopen(filePath.toUtf8(),"wb");
    if(dec_status_fp_raw  == NULL)
    {
        popMsg1->setText("FILE OPEN ERROR "+ filePath);
        popMsg1->show();
    }


    filePath.clear();
    filePath.append(MainWindow::dirPath+"/gtr_gdl_status.txt");
    gtr_gdl_status_fp = fopen(filePath.toUtf8(),"w+");
    if(gtr_gdl_status_fp  == NULL)
    {
        popMsg1->setText("FILE OPEN ERROR "+ filePath);
        popMsg1->show();
    }
	
	filePath.clear();
    filePath.append(MainWindow::dirPath+"/gtr_gdl_health.txt");
    gtr_gdl_health_fp = fopen(filePath.toUtf8(),"w+");
    if(gtr_gdl_health_fp  == NULL)
    {
        popMsg1->setText("FILE OPEN ERROR "+ filePath);
        popMsg1->show();
    }

    filePath.clear();
    filePath.append(MainWindow::dirPath+"/encoder_raw.bin");
    encoder_fp = fopen(filePath.toUtf8(),"wb");
    if(encoder_fp  == NULL)
    {
        popMsg1->setText("FILE OPEN ERROR "+ filePath);
        popMsg1->show();
    }


}
//************************************************************************//
//This function closes the log files //
//************************************************************************//
void Encoder::close_log_Files()
{

    if(dec_dig_fp!= NULL)
        fclose(dec_dig_fp);

    if(dec_hlth_fp!= NULL)
        fclose(dec_hlth_fp);

    if(dec_status_fp!= NULL)
        fclose(dec_status_fp);

    if(dec_dig_fp_raw!= NULL)
        fclose(dec_dig_fp_raw);

    if(dec_hlth_fp_raw!= NULL)
        fclose(dec_hlth_fp_raw);

    if(dec_status_fp_raw!= NULL)
        fclose(dec_status_fp_raw);

    if(gtr_gdl_status_fp != NULL)
        fclose(gtr_gdl_status_fp);

    if(gtr_gdl_health_fp != NULL)
        fclose(gtr_gdl_health_fp);

    if(encoder_fp)
        fclose(encoder_fp);

}

unsigned short Encoder::get_cdma_code(unsigned char *tmp){
	if(tmp[0] == 0 && tmp[1] == 0 && tmp[2] == 0)
		return 0;
	else if(tmp[0] == 0 && tmp[1] == 0 && tmp[2] == 1)
		return 1;
	else if (tmp[0] == 0 && tmp[1] == 0 && tmp[2] == 3)
		return 2;
	else if (tmp[0] == 0 && tmp[1] == 0 && tmp[2] == 5)
		return 3;
	else if (tmp[0] == 0 && tmp[1] == 0 && tmp[2] == 7)
		return 4;
	else if (tmp[0] == 0 && tmp[1] == 0 && tmp[2] == 9)
		return 5;
	else if (tmp[0] == 0 && tmp[1] == 0 && tmp[2] == 0xb)
		return 6;
	else if (tmp[0] == 0 && tmp[1] == 0 && tmp[2] == 0xd)
		return 7;
	else if (tmp[0] == 0 && tmp[1] == 0 && tmp[2] == 0xf)
		return 8;
	else if (tmp[0] == 0 && tmp[1] == 1 && tmp[2] == 1)
		return 9;
	else if (tmp[0] == 0 && tmp[1] == 1 && tmp[2] == 3)
		return 10;
}



unsigned short Encoder::get_dncdma_code(unsigned char *tmp){
	if(tmp[0] == 0 && tmp[1] == 0 )
		return 0;
	else if(tmp[0] == 0 && tmp[1] == 1 )
		return 1;
	else if (tmp[0] == 0 && tmp[1] == 2 )
		return 2;
	else if (tmp[0] == 0 && tmp[1] == 3 )
		return 3;
	else if (tmp[0] == 0 && tmp[1] == 4 )
		return 4;
	else if (tmp[0] == 0 && tmp[1] == 5 )
		return 5;
	else if (tmp[0] == 0 && tmp[1] == 6 )
		return 6;
	else if (tmp[0] == 0 && tmp[1] == 7)
		return 7;
	else if (tmp[0] == 0 && tmp[1] == 8 )
		return 8;
	else if (tmp[0] == 0 && tmp[1] == 9 )
		return 9;
	else if (tmp[0] == 1 && tmp[1] == 0)
		return 10;
}
unsigned char Encoder::get_code(unsigned char *tmp){
	if(tmp[0] == 0 && tmp[1] == 0 )
		return 0;
	else if(tmp[0] == 0 && tmp[1] == 1 )
		return 1;
	else if (tmp[0] == 0 && tmp[1] == 2 )
		return 2;
	else if (tmp[0] == 0 && tmp[1] == 3 )
		return 3;
	else if (tmp[0] == 0 && tmp[1] == 4 )
		return 4;
	else if (tmp[0] == 0 && tmp[1] == 5 )
		return 5;
	else if (tmp[0] == 0 && tmp[1] == 6 )
		return 6;
	else if (tmp[0] == 0 && tmp[1] == 7)
		return 7;
	else if (tmp[0] == 0 && tmp[1] == 8 )
		return 8;
	else if (tmp[0] == 0 && tmp[1] == 9 )
		return 9;
	else if (tmp[0] == 0 && tmp[1] == 0xa)
		return 10;
	else if (tmp[0] == 0 && tmp[1] == 0xb)
		return 11;
	else if (tmp[0] == 0 && tmp[1] == 0xc)
		return 12;


}


void Encoder::write_to_Encoder(unsigned char aEncArray[],int aEncSize )
{
    int wCount1 = -1;
    bool openStatus = EncoderPort->isWritable();

    if(openStatus == true)
    {
         wCount1 = EncoderPort->write(aEncArray,aEncSize);
         fwrite(aEncArray, aEncSize, 1, encoder_fp);
         if(wCount1 != aEncSize)
         {
             emit send_Encoder_Data_Display(aEncArray,13,1);
         }
    }
    else
    {
        emit send_Encoder_Data_Display(aEncArray,13,2);
        qDebug()<<"PLEASE CHECK E1 SERIAL CONNECTION";
    }
}

//************************************************************************//
//This function closes the Encoder port //
//************************************************************************//
void Encoder::close_Encoder_Socket()
{
    if(EncoderPort->isOpen())
        EncoderPort->close();
    if(EncoderPort2->isOpen())
        EncoderPort2->close();
}


void Encoder::Compute_CRCDL(unsigned char *data, int size, unsigned short* CRC)
{
    int i, j;
    unsigned short crc = 0x1d0f;
    for (i = 0; i < size; i++) {
      unsigned short xr = data[i] << 8;
      crc = crc ^ xr;

      for (j = 0; j < 8; j++)
      {
        if (crc & 0x8000) {
          crc = (crc << 1);
          crc = crc ^ 0x1021;
        }
        else {
          crc = crc << 1;
        }
      }
    }
    crc = crc & 0xFFFF;
    *CRC = crc;
}
