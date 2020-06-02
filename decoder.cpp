#include "decoder.h"
#include <QSerialPort>
#include <mainwindow.h>
#include <QDir>
#include <QTime>
#include <QCoreApplication>
#include <QDebug>
#include <radar.h>
#include <QMessageBox>
#include <QMetaType>
#include <QObject>


QSerialPort *DecoderPort, *DecoderPort2;
extern unsigned char decodePort1[16];

QByteArray display_b_Array;// to convert char to qbytearray

Decoder::Decoder(QObject *parent) : QObject(parent)
{
    popMsg = new QMessageBox();
}
//************************************************************************//
//This function Initialize the serialport settings //
//************************************************************************//
void Decoder::initialize_Decoder_Socket()
{
    QByteArray Array;
    /*******************DECODER1*************************/
    DecoderPort = new QSerialPort(decodePort1,this);
    bool a = DecoderPort->open(QIODevice::ReadWrite);
    qDebug()  <<"ccu_c status---" << a;

    if(a == true)
    {

        emit send_Decoder_Data_Display(Array,6);
        //DecoderPort->setReadBufferSize(1);
        DecoderPort->setPortName("Missile");
        DecoderPort->setBaudRate(QSerialPort::Baud115200);
        DecoderPort->setDataBits(QSerialPort::Data8);
        DecoderPort->setParity(QSerialPort::NoParity);
        DecoderPort->setStopBits(QSerialPort::OneStop);
        DecoderPort->setFlowControl(QSerialPort::NoFlowControl);

        connect(DecoderPort,SIGNAL(readyRead()),this,SLOT(read_Decoder_Data()));
    }
    else
    {

        emit send_Decoder_Data_Display(Array,8);
    }


}

void Decoder::read_Decoder_Data()
{

    char *dataBuffer;
    dataBuffer = new char[2];

#if 1
    while(DecoderPort->bytesAvailable())
    {
        DecoderPort->read(dataBuffer, 1);

        if(dataBuffer[0] == 0x02)
        {
            dcount = 0;
            EFlag = 0;
            SFlag = 1;
        }

        if((SFlag == 1) && (EFlag == 0))
        {
            Buf[dcount] = dataBuffer[0];
            dcount++;

        }
        if(dataBuffer[0] == 0x03)
        {
            SFlag = 0;
            EFlag = 1;
         }

        if((dcount == 77) && (EFlag == 1))
        {

            dcount=0;
            process_Decoder_data(Buf,77);
         }

    }
#endif

}


void Decoder::write_Decoder_data(unsigned char *aDecData, int aDecDataLen, int DTM_Device_No)
{

    DecoderPort->write((const char *)aDecData,aDecDataLen);
    memset(aDecData, 0, aDecDataLen);
    DTM_Device_No = 0;
}


float Decoder::get_scaled(unsigned long t1, float scale_vale, float offset){

    return ( (t1 * scale_vale) - offset);
}


void Decoder::process_Decoder_data(unsigned char dataBuffer[],int dec_buf_size)
{

    char msgId = 0;
	msgId = dataBuffer[1];

    display_b_Array.clear();
	display_b_Array = QByteArray(reinterpret_cast<char*>(dataBuffer),dec_buf_size);

    if( msgId == 0x31 ){
		emit send_Decoder_Data_Display(display_b_Array, 1); // to display        
        /* Raw Data logging */
        fwrite(&dataBuffer,dec_buf_size, 1, gtr_raw);
    }
    else{
		emit send_Decoder_Data_Display(display_b_Array, 2); // to display
        /* Raw Data logging */
        fwrite(&dataBuffer,dec_buf_size, 1, gtr_raw);
    }

	/* Excluding STX, Data validity, ETX */ 
    for (int i=1;i<dec_buf_size-1;i++)  // to convert ASCII TO CHAR
	{
		if (dataBuffer[i]  < 0x40)
		{
			dataBuffer[i] = dataBuffer[i] - 0x30;
		}
		else
			dataBuffer[i] = dataBuffer[i] - 0x41 + 0x0a;
	}

	/*  converting the 72 bytes to 36 bytes from decoder  */
    for(int kk=0;kk<sizeof temp; kk++){
        temp[kk] = dataBuffer[2 + (2*kk+1)]  | dataBuffer[2+ (2*kk) ] << 4 ;
    }

    /* byte swaping*/
#if 1
    unsigned char tmp1;
    /* byte swaping */
    for(int j=0; j<sizeof temp; j+=2){
        tmp1 = temp[j];
        temp[j] = temp[j+1];
        temp[j+1] = tmp1;
    }
#endif


    memset(&dnlink_packet, 0x0, sizeof dnlink_packet);

    unsigned char dummy = 'h';
    /* DL header updation to CCU */
    switch(temp[4]){
        case 3:
            dnlink_packet.ccu_hdr.L.msgid = 0x33;
            emit send_Decoder_Display(&dummy, temp[4]);
            break;
        case 4:
             dnlink_packet.ccu_hdr.L.msgid = 0x43;
            emit send_Decoder_Display(&dummy, temp[4]);
            break;
        case 5:
             dnlink_packet.ccu_hdr.L.msgid = 0x53;
            emit send_Decoder_Display(&dummy, temp[4]);
            break;

        case 6:
             dnlink_packet.ccu_hdr.L.msgid = 0x53;
             emit send_Decoder_Display(&dummy, temp[4]);
             break;

        default:
             dnlink_packet.ccu_hdr.L.msgid = 0x11;
            break;
    }


    fwrite(&temp, sizeof temp, 1, gdl_raw);

    dnlink_packet.ccu_hdr.L.timetag = htonl(0x12345678);
    dnlink_packet.ccu_hdr.L.seq_no = htons(seq_no++);

    /* copying the payload to CCU packet */
    memcpy(& dnlink_packet.buffer[10], temp, sizeof temp);
    dnlink_packet.ccu_hdr.L.header_crc = htons(CRC(dnlink_packet.buffer, sizeof dnlink_packet.buffer));

    /* sent to CCU */
    emit send_Radar_Ins_Decoder(dnlink_packet.buffer, sizeof dnlink_packet.buffer);



    /* Logging to FILES */
    switch(dnlink_packet.ccu_hdr.L.msgid){
        case 0x33:
            memset(&ccu_c.buffer, 0x00, sizeof ccu_c.buffer);
            memcpy(&ccu_c.buffer, dnlink_packet.buffer, sizeof ccu_c.buffer);
            fprintf(packetC, "%d, %02d, %02d, %02ld, %01ld, %d, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %ld, %ld, %04X \n",
                    ccu_c.L.time_tag, ccu_c.L.msg_id, ccu_c.L.missile_id, ccu_c.L.FU, ccu_c.L.MLV, ccu_c.L.msg_seq_no,
                    get_scaled(ccu_c.L.msl_pos_x, 1, 150000), get_scaled(ccu_c.L.msl_pos_y, 1, 150000), get_scaled(ccu_c.L.msl_pos_z, 1, 60000),
                    get_scaled(ccu_c.L.msl_vel_x, 1, 1500), get_scaled(ccu_c.L.msl_vel_y, 1, 1500), get_scaled(ccu_c.L.msl_vel_z, 1, 1500),
                    get_scaled(ccu_c.L.msl_acc_x, 1, 400), get_scaled(ccu_c.L.msl_acc_y, 1, 400), get_scaled(ccu_c.L.msl_acc_z, 1, 400),
                    get_scaled(ccu_c.L.body_rates_x, 1, 400), get_scaled(ccu_c.L.body_rates_y, 1, 400), get_scaled(ccu_c.L.body_rates_z, 1, 400),
                    get_scaled(ccu_c.L.euler_angle_x, 1, 180), get_scaled(ccu_c.L.euler_angle_y, 1, 90), get_scaled(ccu_c.L.euler_angle_z, 1, 90),
                    get_scaled(ccu_c.L.deg1, 1, 30), get_scaled(ccu_c.L.deg2, 1, 30), get_scaled(ccu_c.L.deg3, 1, 30), get_scaled(ccu_c.L.deg4, 1, 30),
                    ccu_c.L.msl_st, ccu_c.L.seker_st, ccu_c.L.crc);

            fflush(packetC);
            break;

        case 0x43:
            memset(&ccu_d.buffer, 0x00, sizeof ccu_d.buffer);
            memcpy(&ccu_d.buffer, dnlink_packet.buffer, sizeof ccu_d.buffer);
            fprintf(packetD, "%d, %02d, %02d, %02ld, %01ld, %d, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %ld, %ld, %6.2f, %6.2f, %6.2f, %6.2f, %6.2f, %6.2f, %6.2f, %6.2f, %6.2f, %04X \n",
                    ccu_d.L.time_tag, ccu_d.L.msg_id, ccu_d.L.msl_id, ccu_d.L.FU, ccu_d.L.MLV, ccu_d.L.msg_seq_no,
                    get_scaled(ccu_d.L.msl_acc_x, 1, 400), get_scaled(ccu_d.L.msl_acc_y, 1, 400), get_scaled(ccu_d.L.msl_acc_z, 1, 400),
                    get_scaled(ccu_d.L.body_rates_x, 1, 400), get_scaled(ccu_d.L.body_rates_y, 1, 400), get_scaled(ccu_d.L.body_rates_z, 1, 400),
                    get_scaled(ccu_d.L.euler_angle_x, 1, 180), get_scaled(ccu_d.L.euler_angle_y, 1, 90), get_scaled(ccu_d.L.euler_angle_z, 1, 90),
                    get_scaled(ccu_d.L.deg1, 1, 30), get_scaled(ccu_d.L.deg2, 1, 30), get_scaled(ccu_d.L.deg3, 1, 30), get_scaled(ccu_d.L.deg4, 1, 30),
                    ccu_d.L.msl_st, ccu_d.L.seker_st,
                    get_scaled(ccu_d.L.seek1, 1, 0), get_scaled(ccu_d.L.seek2, 1, 2000), get_scaled(ccu_d.L.seek3, 1, 50), get_scaled(ccu_d.L.seek4, 1, 50),
                    get_scaled(ccu_d.L.seek5, 0.1, 5), get_scaled(ccu_d.L.seek6, 0.1, 5), get_scaled(ccu_d.L.seek7, 0.1, 5), get_scaled(ccu_d.L.seek8, 0.1, 5),
                    get_scaled(ccu_d.L.seek9, 0.1, 5), ccu_d.L.crc);
            fflush(packetD);
            break;

        case 0x53:
            memset(&ccu_e.buffer, 0x00, sizeof ccu_e.buffer);
            memcpy(&ccu_e.buffer, dnlink_packet.buffer, sizeof ccu_e.buffer);
            fprintf(packetE, "%d, %02d, %02d, %02ld, %01ld, %d, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %ld, %ld, %04X \n",
                    ccu_e.L.time_tag, ccu_e.L.msg_id, ccu_e.L.msl_id, ccu_e.L.FU, ccu_e.L.MLV, ccu_e.L.msg_seq_no,
                    get_scaled(ccu_e.L.msl_pos_x, 1, 150000), get_scaled(ccu_e.L.msl_pos_y, 1, 150000), get_scaled(ccu_e.L.msl_pos_z, 1, 60000),
                    get_scaled(ccu_e.L.msl_vel_x, 1, 1500), get_scaled(ccu_e.L.msl_vel_y, 1, 1500), get_scaled(ccu_e.L.msl_vel_z, 1, 1500),
                    get_scaled(ccu_e.L.msl_acc_x, 1, 400), get_scaled(ccu_e.L.msl_acc_y, 1, 400), get_scaled(ccu_e.L.msl_acc_z, 1, 400),
                    get_scaled(ccu_e.L.body_rates_x, 1, 400), get_scaled(ccu_e.L.body_rates_y, 1, 400), get_scaled(ccu_e.L.body_rates_z, 1, 400),
                    get_scaled(ccu_e.L.euler_angle_x, 1, 180), get_scaled(ccu_e.L.euler_angle_y, 1, 90), get_scaled(ccu_e.L.euler_angle_z, 1, 90),
                    get_scaled(ccu_e.L.deg1, 1, 30), get_scaled(ccu_e.L.deg2, 1, 30), get_scaled(ccu_e.L.deg3, 1, 30), get_scaled(ccu_e.L.deg4, 1, 30),
                    ccu_e.L.msl_st, ccu_e.L.seker_st, ccu_e.L.crc);
            fflush(packetE);
            break;

        default :
               //qDebug() << " default case at logging\n";
               break;
    }

    emit send_Decoder_Display(dnlink_packet.buffer, 10);


}


unsigned short Decoder::CRC(unsigned char *data, int size){

    unsigned short checksum=0X1D0F;
    unsigned char mssg=0;
    unsigned short flag = 0;
    short i=0;
    //printf("\n %02x ", checksum);
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
    return checksum;
}



//************************************************************************//
//This function opens all the files of serial data //
//************************************************************************//
void Decoder::open_Decoder_LogFiles()
{
    QString filePath;

    filePath.clear();
    filePath.append(MainWindow::dirPath+"/decoder_raw.bin");
    gtr_raw = fopen(filePath.toUtf8(),"wb");
    if(gtr_raw  == NULL)
    {
        popMsg->setText("FILE OPEN ERROR "+ filePath);
        popMsg->show();
    }



    filePath.clear();
    filePath.append(MainWindow::dirPath+"/gdl_raw.bin");
    gdl_raw = fopen(filePath.toUtf8(),"wb");
    if(gdl_raw  == NULL)
    {
        popMsg->setText("FILE OPEN ERROR "+ filePath);
        popMsg->show();
    }


    filePath.clear();
    filePath.append(MainWindow::dirPath+"/PacketC.txt");
    packetC = fopen(filePath.toUtf8(),"w");
    if(packetC  == NULL)
    {
        popMsg->setText("FILE OPEN ERROR "+ filePath);
        popMsg->show();
    }


    filePath.clear();
    filePath.append(MainWindow::dirPath+"/PacketD.txt");
    packetD = fopen(filePath.toUtf8(),"w");
    if(packetD  == NULL)
    {
        popMsg->setText("FILE OPEN ERROR "+ filePath);
        popMsg->show();
    }

    filePath.clear();
    filePath.append(MainWindow::dirPath+"/PacketE.txt");
    packetE = fopen(filePath.toUtf8(),"w");
    if(packetE  == NULL)
    {
        popMsg->setText("FILE OPEN ERROR "+ filePath);
        popMsg->show();
    }

}

//************************************************************************//
//This function closes all the files of serial data //
//************************************************************************//
void Decoder::close_Dec_log_Files()
{
    if(gtr_raw != NULL)
        fclose(gtr_raw);

    if(packetC != NULL)
        fclose(packetC);

    if(packetD != NULL)
        fclose(packetD);

    if(packetE != NULL)
        fclose(packetE);

}

//************************************************************************//
//This function Closes Serial sockets //
//************************************************************************//
void Decoder::close_Dec_Srl_Socket()
{
    if(DecoderPort->isOpen())
        DecoderPort->close();
    if(DecoderPort2->isOpen())
        DecoderPort2->close();
}
