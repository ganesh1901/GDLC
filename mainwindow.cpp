#include "mainwindow.h"
#include "ui_mainwindow.h"

Encoder *Main_to_Encoder;
Decoder *Main_to_Decoder;
Radar *Main_to_Radar;

QString MainWindow::dirPath;

char MainWindow::Missile_Id;
unsigned short MainWindow::pLevel;
unsigned short MainWindow::txSelect;
unsigned int MainWindow::e1Status;
unsigned int MainWindow::e2Status;
unsigned int MainWindow::d1Status;
unsigned int MainWindow::d2Status;
unsigned int MainWindow::nwStatus;

char chkSumdata[30];
unsigned int app_Chksum;
unsigned int app_size;


unsigned char rxAddr[32];
unsigned char txAddr[32];
unsigned short txPort;
unsigned short rxPort;
unsigned char encodePort1[16];
unsigned char encodePort2[16];
unsigned char decodePort1[16];
unsigned char decodePort2[16];

enum  {
    rxaddr,
    txaddr,
    rxport,
    txport,
    encodeport1,
    encodeport2,
    decoderport1,
    decoderport2
};


QString str;


void MainWindow::getConfig(){

#if 1

       FILE *fp;
       char *line = NULL;
       size_t len = 0;
       ssize_t read;

       fp = fopen(CONF_FILE, "r");

       int index = 0;
       if( fp != NULL){
           while ((read = getline(&line, &len, fp)) != -1) {
               //qDebug("Retrieved line of length %zu :\n", read);
               //qDebug("%s", line);

                line[read-1] = '\0';

               switch(index){
                case rxaddr:
                   memcpy(rxAddr, line, read-1);
                   break;
                 case txaddr:
                   memcpy(txAddr, line, read-1);
                   break;
               case rxport:
                   rxPort = atoi(line);
                   cout << "rxPort-----" << rxPort << "---"<< line << endl;
                    break;
                case txport:
                   txPort = atoi(line);
                   break;
               case encodeport1:
                   memcpy(encodePort1, line, read-1);
                   cout << " encoder1---" << encodePort1 << endl;
                   break;

               case encodeport2:
                   memcpy(encodePort2, line, read-1);
                   cout << " encoder2---" << encodePort2 << endl;
                   break;

               case decoderport1:
                   memcpy(decodePort1, line, read-1);
                   break;
               case decoderport2:
                   memcpy(decodePort2, line, read-1);
                   break;
               }
               index++;


           }

       }
       else{
           cout << "Network Cnfiguration file not able to open " << CONF_FILE << endl;
       }

#endif

}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    app_Chksum = calculateFileChecksum();
    qDebug("\n...........%4x", app_Chksum);

    sprintf(chkSumdata,"%08x",app_Chksum);

    displayChecksum(chkSumdata);


    getConfig();


    //AddTableView();

    ui->stackedWidget->setCurrentIndex(0);

    radar_Thd = new QThread();
    Encoder_Thd = new QThread();
    Decoder_Thd = new QThread();

    radar_Class = new Radar();
    encoder_Class  =  new Encoder();
    decoder_Class = new Decoder();

    //SIGNALS AND SLOTS CONNECTION

    connect(Encoder_Thd,SIGNAL(started()),encoder_Class,SLOT(initialize_Encoder_Socket()));
    connect(encoder_Class,SIGNAL(send_Radar_Encoder_Data(unsigned char *, int)), radar_Class, SLOT(receive_Encoder_Data(unsigned char *, int)),Qt::QueuedConnection);
    connect(encoder_Class,SIGNAL(send_Encoder_Data_Display(QByteArray,int, int)),this,SLOT(receive_Encoder_Display(QByteArray,int,int)),Qt::QueuedConnection);
    connect(encoder_Class,SIGNAL(send_Encoder_Display(unsigned char *, int)),this,SLOT(receive_display(unsigned char *,int)),Qt::QueuedConnection);



    connect(Decoder_Thd, SIGNAL(started()), decoder_Class, SLOT(initialize_Decoder_Socket()),Qt::QueuedConnection);
    connect(decoder_Class,SIGNAL(send_Radar_Ins_Decoder(unsigned char *, int)),radar_Class,SLOT(receive_Decoder_Data(unsigned char *, int)),Qt::QueuedConnection);
    connect(decoder_Class,SIGNAL(send_Decoder_Data_Display(QByteArray,int)),this,SLOT(receive_Decoder_Display(QByteArray,int)),Qt::QueuedConnection);
    connect(decoder_Class,SIGNAL(send_Decoder_Display(unsigned char*,int)),this,SLOT(receive_display(unsigned char*,int)),Qt::QueuedConnection);


    connect(radar_Thd,SIGNAL(started()),radar_Class,SLOT(initialize_Radar()),Qt::QueuedConnection);
    connect(radar_Class,SIGNAL(send_Radar_Data_to_Gui(QByteArray, int)),this, SLOT(display_Radar_Data(QByteArray, int)),Qt::QueuedConnection);
    connect(radar_Class,SIGNAL(send_Radar_Data_to_Decoder(unsigned char *, int, int)), decoder_Class, SLOT(write_Decoder_data(unsigned char *, int, int)),Qt::QueuedConnection);


    connect(radar_Class,SIGNAL(RadarToEncoderData(unsigned char *, int)), encoder_Class, SLOT(write_to_Encoder(unsigned char *, int)),Qt::QueuedConnection);
    connect(radar_Class,SIGNAL(process_health_request_data(void)), encoder_Class, SLOT(updata_health_data(void)),Qt::QueuedConnection);
    connect(radar_Class,SIGNAL(process_status_request_data(void)), encoder_Class, SLOT(updata_status_data(void)),Qt::QueuedConnection);

    start_Application();  // To Start The Application ...
}

//************************************************************************//
//This function cals Initialize Network and Serial Ports functions//
//************************************************************************//
int start_count = 0;
int closeflag = 0;



void MainWindow::start_Application()
{
    QDir *dir = new QDir();
    dirPath = QCoreApplication::applicationDirPath() +"/" +QDateTime::currentDateTime().toString("dd-MM-yyyy") + "/" + QTime::currentTime().toString("hh-mm-ss");
    bool a = dir->mkpath(dirPath);

    if(a!= true)
    {
        QMessageBox::information(this,"Info","Dir ERROR"+ dirPath);
    }
    else
    {
        encoder_Class->moveToThread(Encoder_Thd);
        encoder_Class->openLogFiles();
        Encoder_Thd->start();

        decoder_Class->moveToThread(Decoder_Thd);
        decoder_Class->open_Decoder_LogFiles();
        Decoder_Thd->start();

        radar_Class->moveToThread(radar_Thd);
        radar_Class->openFiles();
        radar_Thd->start();
    }
}

// TERMINATION OF APPLICATION
MainWindow::~MainWindow()
{
    if(radar_Thd->isRunning())
        radar_Thd->terminate(); //thread termination

    if(Encoder_Thd->isRunning())
        Encoder_Thd->terminate(); //thread termination

    if(Decoder_Thd->isRunning())
        Decoder_Thd->terminate(); //thread termination

    delete ui; // close application window.
}


#if 0

void MainWindow::AddTableView(){

    QStandardItemModel *model = new QStandardItemModel();

    list<char *> ver_header;
    ver_header.push_back("X");
    ver_header.push_back("Y");
    ver_header.push_back("Z");

    list<char *> col_header;
    col_header.push_back("Target Pos:");
    col_header.push_back("Target Vel:");
    col_header.push_back("Target Acc:");
    col_header.push_back("Target Pos Sigma:");
    col_header.push_back("Target Vel Sigma:");


    list<char *>::iterator it;

    auto index = 0;
    for( it = ver_header.begin(); it != ver_header.end(); ++it){

        QStandardItem *item = new QStandardItem(*it);
        cout << " item " << *it << endl;
        model->setVerticalHeaderItem(index,item);
        index++;
    }

    index = 0;
    for( it = col_header.begin(); it != col_header.end(); ++it){

        QStandardItem *item = new QStandardItem(*it);
        cout << " item " << *it << endl;
        model->setHorizontalHeaderItem(index,item);
        index++;
    }


    ui->tableView->setModel(model);


#if 0
    // vertical header setting
    for i in range(len(self.ema.model_v_header)):
       item = QtGui.QStandardItem(self.ema.model_v_header[i])
       model.setVerticalHeaderItem(i, item)

    # horizontal header setting
    for i in range(coloumn_count):
       item = QtGui.QStandardItem(coloumn_name_list[i])
       model.setHorizontalHeaderItem(i, item)
       #self.table.setColumnWidth(i, 20)
#endif

}

#endif


//************************************************************************//
//This function closes the Mainwindow after confirmation from User//
//************************************************************************//
void MainWindow::on_pb_close_clicked()
{
    closeflag =1;
    int a = QMessageBox::information(this,"INFO","CONFIRM","YES","NO");
    if(a == 0)
    {
        if(radar_Thd->isRunning())
            radar_Thd->terminate(); //thread termination

        if(Encoder_Thd->isRunning())
            Encoder_Thd->terminate(); //thread termination

        if(Decoder_Thd->isRunning())
            Decoder_Thd->terminate(); //thread termination

        this->close();
    }
}

void MainWindow::on_pb_radar_windw_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}
void MainWindow::on_pb_ground_windw_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}
void MainWindow::on_pb_parameters_clicked()
{
    ui->lineEditlRx1Correlator->setText("NA");
    ui->lineEditlRx2Correlator->setText("NA");
    ui->lineEditRx1Doppler->setText("NA");
    ui->lineEditRx2Doppler->setText("NA");

    ui->stackedWidget->setCurrentIndex(2);
}
//************************************************************************//
//This function displays radar data to the concerned windows
//************************************************************************//
void MainWindow::display_Radar_Data(QByteArray aDataArray, int aDataArraySize )
{
    str.clear();
    str= aDataArray.data();

    switch (aDataArraySize)
    {
    case 1:
        ui->pktA_data->append(aDataArray);
        break;
    case 2:
        ui->pktB_data->append(aDataArray.toHex());
        break;
    case 4: //  CONFIGURATION
        ui->request_msg_ldt->appendPlainText("CONFIGURATION REQUEST\n");
        ui->request_msg_ldt->appendPlainText(aDataArray.toHex());
        break;
    case 5: // HEALTH
        ui->request_msg_ldt->appendPlainText("HEALTH REQUEST\n");
        ui->request_msg_ldt->appendPlainText(aDataArray.toHex());
        break;

    case 6: // STATUS
        ui->request_msg_ldt->appendPlainText("STATUS REQUEST\n");
        ui->request_msg_ldt->appendPlainText(aDataArray.toHex());
        break;
    case 7: // N/W STATUS
        ui->lineEdit_NW->setStyleSheet("background-color:lightgreen");
        ui->lineEdit_NW->setText("N/W : OK");
        break;

    case 8: // N/W STATUS
        ui->lineEdit_NW->setStyleSheet("background-color:red");
        ui->lineEdit_NW->setText("N/W: CHECK");
        break;

     case 9:
        ui->request_msg_ldt->appendPlainText("PA Config  REQUEST\n");
        ui->request_msg_ldt->appendPlainText(aDataArray.toHex());
        break;

      case 12:
        ui->request_msg_ldt->appendPlainText(" Config Msg Received biuut Checksum not matching \n");

    default:
        break;
    }
}



void MainWindow::receive_display(unsigned char * tmp, int id){
	if(id == 1){
	
		memcpy(&gui_gdl_status.L, tmp, sizeof gui_gdl_status);		
	
        str.clear();
		str = QString(" %1 ").arg(tmp[11]);
		ui->lineEditTxFreq_STATUS->setText(str);
		str.clear();

		str = QString(" %1").arg(tmp[13]);
		ui->lineEditRxFreq_STATUS->setText(str);
		str.clear();
		
		str =  QString(" %1 ").arg(ntohs(gui_gdl_status.L.dnlink_cdma));
		ui->lineEditDOWNLinkCDMA_STATUS->setText(str);
		str.clear();

		str = QString(" %1 ").arg(ntohs(gui_gdl_status.L.uplink_cdma[0]));
		ui->lineEditUpLinkCDMA_STATUS->setText(str);
		str.clear();
		
		str = QString(" %1 ").arg(gui_gdl_status.L.rx1_rssi);
		ui->lineEditRSSI1_STATUS->setText(str);
		str.clear();

		str = QString(" %1 ").arg(gui_gdl_status.L.rx2_rssi);
		ui->lineEditRSSI2_STATUS->setText(str);
		str.clear();

		unsigned short t1 = ntohs(gui_gdl_status.L.pa_status);

		if( t1 == 0x0000){
            ui->lineEditPA_STATUS->setText(" OFF");
			ui->lineEditPALEVEL_STATUS->setText(" ");
		}
		else if( t1 == 0xff01){
            ui->lineEditPA_STATUS->setText(" ON");
			ui->lineEditPALEVEL_STATUS->setText(" 1 watt");
		}
		else if( t1 == 0xff02){
            ui->lineEditPA_STATUS->setText(" ON");
			ui->lineEditPALEVEL_STATUS->setText(" 5 watt");

		}
		else if(t1 == 0xff04){
            ui->lineEditPA_STATUS->setText(" ON");
			ui->lineEditPALEVEL_STATUS->setText(" 15 watt");

		}

		if( gui_gdl_status.L.tx_pll_lock == 0xff){
			ui->lineEditTXLock_STATUS->setText("LOCK");
		}
		else{
			ui->lineEditTXLock_STATUS->setText("UNLOCK");
		}

		if(gui_gdl_status.L.rx_pll_lock == 0xff){
			ui->lineEditRx1Lock_STATUS->setText("LOCK");
			ui->lineEditRx2Lock_STATUS->setText("LOCK");
		}
		else if(gui_gdl_status.L.rx_pll_lock == 0x0f){
			ui->lineEditRx1Lock_STATUS->setText("UNLOCK");
			ui->lineEditRx2Lock_STATUS->setText("LOCK");
		}
		else if(gui_gdl_status.L.rx_pll_lock == 0xf0){
			ui->lineEditRx1Lock_STATUS->setText("LOCK");
			ui->lineEditRx2Lock_STATUS->setText("UNLOCK");
		}
		else{
			ui->lineEditRx1Lock_STATUS->setText("UNLOCK");
			ui->lineEditRx2Lock_STATUS->setText("UNLOCK");
			
		}

		if(gui_gdl_status.L.decoder1_track_acq == 0xff)
			ui->lineEditDH1->setText("Track");
		else
			ui->lineEditDH1->setText("ACQ");

		if(gui_gdl_status.L.decoder2_track_acq == 0xff)
			ui->lineEditDH2->setText("TRACK");
		else	
			ui->lineEditDH2->setText("ACQ");
	
		if(gui_gdl_status.L.tx_antenna_status == 0xffff)
			ui->lineEditTXPORT_STATUS->setText(" Port 2 ");
		else
			ui->lineEditTXPORT_STATUS->setText(" Port 1 ");
		

	}

   else if(id == 3){
        ui->pktc->setStyleSheet("background-color:lightgreen");
        ui->pktd->setStyleSheet("background-color:gray");
        ui->pkte->setStyleSheet("background-color:gray");
        ui->pktf->setStyleSheet("background-color:gray");
    }
    else if(id == 4){
        ui->pktc->setStyleSheet("background-color:gray");
        ui->pktd->setStyleSheet("background-color:lightgreen");
        ui->pkte->setStyleSheet("background-color:gray");
        ui->pktf->setStyleSheet("background-color:gray");

    }

    else if(id == 5){
        ui->pktc->setStyleSheet("background-color:gray");
        ui->pktd->setStyleSheet("background-color:gray");
        ui->pkte->setStyleSheet("background-color:lightgreen");
        ui->pktf->setStyleSheet("background-color:gray");
    }

    else if(id == 6){
        ui->pktc->setStyleSheet("background-color:gray");
        ui->pktd->setStyleSheet("background-color:gray");
        ui->pkte->setStyleSheet("background-color:gray");
        ui->pktf->setStyleSheet("background-color:lighgreen");
    }


    else if(id == 10){

        str.clear();

        for(int jj=0; jj<46; jj++){
            str.append(QString("%1").arg(tmp[jj], 0, 16));
        }
        ui->dnlink_data->append(str);
    }

    else{

    }

}

//************************************************************************//
//This function Displays GDLU health and status to the concerned windows
//************************************************************************//
void MainWindow::receive_Encoder_Display(QByteArray recArray,int msg,int id)
{
    QString str;
    str= recArray.data();

    if(msg == 1)
    {
        ui->hlth_data->append(recArray);
    }
    if(msg == 2)
    {
        ui->sts_data->append(recArray);
    }
    if(id == 1)
    {
        if(msg == 3)    //TX(Uplink)Freq Index Display
        {
            ui->lineEditTxFreq_STATUS->setText(recArray.toHex());
            qDebug(" sdasdasdas");
        }

        if(msg == 4)    //RX(Downlink)Freq Index Display
        {
            ui->lineEditRxFreq_STATUS->setText(recArray.toHex());
        }

        if(msg == 5)    //Uplink CDMA Code Display
        {
            ui->lineEditUpLinkCDMA_STATUS->setText(recArray.toHex());
        }

        if(msg == 6)    //Downlink CDMA Code Display
        {
            ui->lineEditDOWNLinkCDMA_STATUS->setText(recArray.toHex());
        }

        if(msg == 7)    //PA Status Display
        {
            if((recArray[0]&0x0F) == 0x01)  //PA Power Level Display
            {
                ui->lineEditPALEVEL_STATUS->setText("LP");
            }
            else if((recArray[0]&0x0F) == 0x02)
            {
                ui->lineEditPALEVEL_STATUS->setText("MP1");
            }
            else if((recArray[0]&0x0F) == 0x03)
            {
                ui->lineEditPALEVEL_STATUS->setText("MP2");
            }
            else if((recArray[0]&0x0F) == 0x04)
            {
                ui->lineEditPALEVEL_STATUS->setText("HP");
            }
            else
            {
                ui->lineEditPALEVEL_STATUS->setText("NA");
            }

        }
        if(msg == 8)    //PLL LOCK Display
        {
            if((recArray[0]&0x0F) == 0x0F)  //Rx1 Lock Display
            {
                ui->lineEditRx1Lock_STATUS->setStyleSheet("background-color:lightgreen");
                ui->lineEditRx1Lock_STATUS->setText("LOCK");
            }
            else if((recArray[0]&0x0F) == 0x00)
            {
                ui->lineEditRx1Lock_STATUS->setStyleSheet("background-color:red");
                ui->lineEditRx1Lock_STATUS->setText("UNLOCK");
            }
            else
            {
                ui->lineEditRx1Lock_STATUS->setText("NA");
            }

            if((recArray[0]&0xF0) == 0xF0)  //Rx2 Lock Display
            {
                ui->lineEditRx2Lock_STATUS->setStyleSheet("background-color:lightgreen");
                ui->lineEditRx2Lock_STATUS->setText("LOCK");
            }
            else if((recArray[0]&0xF0) == 0x00)
            {
                ui->lineEditRx2Lock_STATUS->setStyleSheet("background-color:red");
                ui->lineEditRx2Lock_STATUS->setText("UNLOCK");
            }
            else
            {
                ui->lineEditRx2Lock_STATUS->setText("NA");
            }

            if((recArray[0]&0x0F) == 0x0F)  //TX LOCK Display
            {
                ui->lineEditTXLock_STATUS->setStyleSheet("background-color:lightgreen");
                ui->lineEditTXLock_STATUS->setText("LOCK");
            }
            else if((recArray[0]&0x0F) == 0x00)
            {
                ui->lineEditTXLock_STATUS->setStyleSheet("background-color:red");
                ui->lineEditTXLock_STATUS->setText("UNLOCK");
            }
            else
            {
                ui->lineEditTXLock_STATUS->setText("NA");
            }
        }
        if(msg == 9)    //Decoder Health1 Display
        {
            if(recArray[0] == 0xFF)
            {
                ui->lineEditDH1->setStyleSheet("background-color:lightgreen");
                ui->lineEditDH1->setText("TRACK");
            }
            else if(recArray[0] == 0x00)
            {
                ui->lineEditDH1->setStyleSheet("background-color:red");
                ui->lineEditDH1->setText("ACQUISITION");
            }
            else
            {
                ui->lineEditDH1->setText("NA");
            }
        }
        if(msg == 10)   //Decoder Health2 Display
        {
            if(recArray[0] == 0xFF)
            {
                ui->lineEditDH2->setStyleSheet("background-color:lightgreen");
                ui->lineEditDH2->setText("TRACK");
            }
            else if(recArray[0] == 0x00)
            {
                ui->lineEditDH2->setStyleSheet("background-color:red");
                ui->lineEditDH2->setText("ACQUISITION");
            }
            else
            {
                ui->lineEditDH2->setText("NA");
            }
        }
        if(msg == 11)   //RSSI1 Display
        {
            ui->lineEditRSSI1_STATUS->setText(recArray.toHex());
        }

        if(msg == 12)   //RSSI2 Display
        {
            ui->lineEditRSSI2_STATUS->setText(recArray.toHex());
        }

        if(msg == 13)   //Encoder1 LINK Status
        {
            ui->lineEdit_E1->setStyleSheet("background-color:red");
            ui->lineEdit_E1->setText("E1 : CHECK");
        }

        if(msg == 17)   //PA ON / OFF Status
        {
            if(recArray[0] == 1)  //PA ON/OFF Display
            {
                ui->lineEditPA_STATUS->setStyleSheet("background-color:red");
                ui->lineEditPA_STATUS->setText("ON");
            }
            else if(recArray[0] == 0)
            {
                ui->lineEditPA_STATUS->setStyleSheet("background-color:green");
                ui->lineEditPA_STATUS->setText("OFF");
            }
            else
            {
                ui->lineEditPA_STATUS->setText("NA");
            }

            if(MainWindow::txSelect == 0x0000)  //Tx port Selction for PA1 Status Display
            {
                ui->lineEditTXPORT_STATUS->setText("L/R");
            }
            else if(MainWindow::txSelect == 0xFFFF)
            {
                ui->lineEditTXPORT_STATUS->setText("F/B");
            }
            else
            {
                ui->lineEditTXPORT_STATUS->setText("L/R");
            }
        }
    }
    if(id == 3)
    {
        ui->lineEdit_E1->setStyleSheet("background-color:green");
        ui->lineEdit_E1->setText("E1 : OK");
    }


}

//************************************************************************//
//This function Displays INS data received from RX1 and RX2 to the concerned windows
//************************************************************************//
void MainWindow::receive_Decoder_Display(QByteArray recArray,int msg)
{
    QString str;
    str= recArray.data();
    //  qDebug()<<str;

    if(msg == 1)
    {
        ui->dn_rx1->append(str);
    }
    if(msg == 2)
    {
        ui->dn_rx2->append(str);
    }


    if(msg == 5)
    {
        ui->dnlink_data->append(str);
    }
    if(msg == 6)
    {
        ui->lineEdit_D1->setStyleSheet("background-color:lightgreen");
        ui->lineEdit_D1->setText("D1 : OK");
    }

    if(msg == 8)
    {
        ui->lineEdit_D1->setStyleSheet("background-color:red");
        ui->lineEdit_D1->setText("D1 : CHECK");
    }

}

unsigned int MainWindow::calculateFileChecksum()
{
    char buff[1000],data[30];
    FILE *inputfile;
    unsigned int checkSum = 0;
    int iReadLen;

    qDebug("\n........................................................");


    QString currfilepath;

    currfilepath = QCoreApplication::applicationDirPath();


    currfilepath.append("/AKASH-NG-GDLC");


    qDebug()  << "currfilepath :: "  << currfilepath  ;
    struct stat t11;

    if((inputfile = fopen(currfilepath.toUtf8(), "rb"))==NULL)
    {
        qDebug("File Open Error..\n");
        data[0] = 0;
        data[1] = '\0';
    }
    else
    {
        while ((iReadLen =fread(buff, 1, sizeof buff, inputfile)) > 0)
        {
            checkSum += computeChecksum(buff,iReadLen);
        }

        fstat(inputfile, &t11);
        app_size = t11.st_size;

        fclose(inputfile);
    }

    return(checkSum);
}

unsigned int MainWindow::computeChecksum(char *buff,unsigned int buffLen)
{
    unsigned int i, checkSum = 0;

    for(i = 0; i < buffLen; i++)
    {
        checkSum += *(buff+i);
    }
    return(checkSum);
}

void MainWindow::displayChecksum(char *cStr)
{
    ui->ldt_Checksum->setText(cStr);

}

void MainWindow::on_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}


#if 1
void MainWindow::on_pushButton_2_clicked()
{

    qDebug() << " ehllo" << endl;
    char tmp[] = { 0x02,0x32,0x30,0x30,0x30,0x30,0x30,0x31,0x30,0x31,0x30,0x31,0x30,0x30,0x31,0x30,0x30,
                                 0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,
                                 0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x31,0x33,0x31,0x35,0x35,0x03};


    emit test(tmp, sizeof(tmp), 1);
}
#endif

void MainWindow::on_pb_radar_clicked()
{
 ui->stackedWidget->setCurrentIndex(0);
}



void MainWindow::on_pb_ground_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


