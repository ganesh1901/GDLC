#ifndef VARIABLES_H
#define VARIABLES_H
#include <qserialport.h>
#include <QDir>
#include <QTime>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDialog>
#include <QThread>
#include <QList>
#include <QMetaType>
#include <map>
#include <iterator>
#include <string.h>


#include <QMutex>
#include <QWaitCondition>


using namespace std;


extern unsigned char rxAddr[32];
extern unsigned char txAddr[32];
extern unsigned short txPort;
extern unsigned short rxPort;
extern unsigned char encodePort1[16];
extern unsigned char encodePort2[16];
extern unsigned char decodePort1[16];
extern unsigned char decodePort2[16];


union targetdwell_union
{
    unsigned char buffer[46];
    #pragma pack(1)
    struct TargetDwell
    {


        unsigned int timetag;
        unsigned char reserved;
        unsigned char msgid;
        unsigned short seq_no;
        unsigned short header_crc;

        unsigned int time_tag;
        unsigned char msg_id;
        unsigned char missile_id;
        unsigned char reserved1;
        unsigned long FU:6;
        unsigned long MLV:2;
        unsigned short msg_seq_no;

        unsigned long x_t:19;
        unsigned long y_t:19;
        unsigned long z_t:18;

        unsigned long vx_t:14;
        unsigned long vy_t:13;
        unsigned long vz_t:13;

        unsigned long tgt_ax_t:8;
        unsigned long tgt_ay_t:8;
        unsigned long tgt_az_t:8;

        unsigned long tgt_pos_x:11;
        unsigned long tgt_pos_y:11;
        unsigned long tgt_pos_z:10;

        unsigned long tgt_vel_x:8;
        unsigned long tgt_vel_y:8;
        unsigned long tgt_vel_z:8;

        unsigned char target_type;
        unsigned char target_RCS;
        unsigned short crc;
    }L;
};


union Missile_data_union
{
    unsigned char  buffer[46];
    #pragma pack(1)
    struct MslDwell
    {

        unsigned int timetag;
        unsigned char reserved;
        unsigned char msgid;
        unsigned short seq_no;
        unsigned short header_crc;

        unsigned int time_tag;
        unsigned char msg_id;
        unsigned char missile_id;
        unsigned char reserved1;
        unsigned char FU:6;
        unsigned char MLV:2;
        unsigned short msg_seq_no;


        unsigned long tgt_pos_x:19;
        unsigned long tgt_pos_y:19;
        unsigned long tgt_pos_z:18;

        unsigned long tgt_vel_x:14;
        unsigned long tgt_vel_y:13;
        unsigned long tgt_vel_z:13;

        unsigned long msl_pos_x:19;
        unsigned long msl_pos_y:19;
        unsigned long msl_pos_z:18;

        unsigned long msl_vel_x:14;
        unsigned long msl_vel_y:13;
        unsigned long msl_vel_z:13;

        unsigned short crc;
    }L;
};

union INSData_union
{
    unsigned char insDataBuf[36];
    #pragma pack(1)
    struct INSData
    {
        unsigned short time_tag;
        unsigned char missile_id;
        unsigned char FU_MLV_ID;
        unsigned char msg_seq_no;
        unsigned char msg_id;

        unsigned long msl_pos_x:19;
        unsigned long msl_pos_y:19;
        unsigned long msl_pos_z:18;

        unsigned long msl_vel_x:14;
        unsigned long msl_vel_y:13;
        unsigned long msl_vel_z:13;


        unsigned long msl_acc_x:11;
        unsigned long msl_acc_y:11;
        unsigned long msl_acc_z:10;

        unsigned long body_rates_x:11;
        unsigned long body_rates_y:11;
        unsigned long body_rates_z:10;

        unsigned long euler_angle_x:11;
        unsigned long euler_angle_y:11;
        unsigned long euler_angle_z:10;

        unsigned long deg1:6;
        unsigned long deg2:6;
        unsigned long deg3:6;
        unsigned long deg4:6;

        unsigned long msl_st:4;
        unsigned long seker_st:4;

        unsigned short crc;
    }L;
};




typedef union {
    unsigned char buffer[10];
    struct {
        unsigned int timetag;
        unsigned char reserved;
        unsigned char msgid;
        unsigned short seq_no;
        unsigned short header_crc;
    }L;
}header_radar_gdl_t;






typedef struct {
    header_radar_gdl_t ccu_hdr;
    unsigned char buffer[36];
}downlink_CCU_Packet;




union Packet_C{
	     unsigned char buffer[46];
	#pragma pack(1)
         struct t12{

             unsigned int timetag;
             unsigned char reserved;
             unsigned char msgid;
             unsigned short seq_no;
             unsigned short header_crc;


		     unsigned short time_tag;
		     unsigned char missile_id;
             unsigned long FU:6;
             unsigned long MLV:2;

		     unsigned char msg_seq_no;
		     unsigned char msg_id;

		     unsigned long msl_pos_x:19;
		     unsigned long msl_pos_y:19;
		     unsigned long msl_pos_z:18;

		     unsigned long msl_vel_x:14;
		     unsigned long msl_vel_y:13;
		     unsigned long msl_vel_z:13;


		     unsigned long msl_acc_x:11;
		     unsigned long msl_acc_y:11;
		     unsigned long msl_acc_z:10;

		     unsigned long body_rates_x:11;
		     unsigned long body_rates_y:11;
		     unsigned long body_rates_z:10;

		     unsigned long euler_angle_x:11;
		     unsigned long euler_angle_y:11;
		     unsigned long euler_angle_z:10;

		     unsigned long deg1:6;
		     unsigned long deg2:6;
		     unsigned long deg3:6;
		     unsigned long deg4:6;

		     unsigned long msl_st:4;
		     unsigned long seker_st:4;

		     unsigned short crc;

	     }L;
     };

union Packet_D{
    unsigned char buffer[46];
#pragma pack(1)
    struct {

        unsigned int timetag;
        unsigned char reserved;
        unsigned char msgid;
        unsigned short seq_no;
        unsigned short header_crc;

        unsigned short time_tag;
        unsigned char msl_id;
        unsigned long FU:6;
        unsigned long MLV:2;
        unsigned char msg_seq_no;
        unsigned char msg_id;

        unsigned long msl_acc_x:11;
        unsigned long msl_acc_y:11;
        unsigned long msl_acc_z:10;

        unsigned long body_rates_x:11;
        unsigned long body_rates_y:11;
        unsigned long body_rates_z:10;

        unsigned long euler_angle_x:11;
        unsigned long euler_angle_y:11;
        unsigned long euler_angle_z:10;

        unsigned long deg1:6;
        unsigned long deg2:6;
        unsigned long deg3:6;
        unsigned long deg4:6;

        unsigned long msl_st:4;
        unsigned long seker_st:4;

        unsigned long seek1:15;
        unsigned long seek2:13;
        unsigned long seek3:8;
        unsigned long seek4:8;
        unsigned long seek5:9;
        unsigned long seek6:9;
        unsigned long seek7:9;
        unsigned long seek8:9;
        unsigned long seek9:16;

        unsigned short crc;
    }L;
};


union Packet_E{
    unsigned char buffer[46];
    struct {

        unsigned int timetag;
        unsigned char reserved;
        unsigned char msgid;
        unsigned short seq_no;
        unsigned short header_crc;


        unsigned short time_tag;
        unsigned char msl_id;
        unsigned long FU:6;
        unsigned long MLV:2;
        unsigned char msg_seq_no;
        unsigned char msg_id;

        unsigned long msl_pos_x:19;
        unsigned long msl_pos_y:19;
        unsigned long msl_pos_z:18;

        unsigned long msl_vel_x:14;
        unsigned long msl_vel_y:13;
        unsigned long msl_vel_z:13;


        unsigned long msl_acc_x:11;
        unsigned long msl_acc_y:11;
        unsigned long msl_acc_z:10;

        unsigned long body_rates_x:11;
        unsigned long body_rates_y:11;
        unsigned long body_rates_z:10;

        unsigned long euler_angle_x:11;
        unsigned long euler_angle_y:11;
        unsigned long euler_angle_z:10;

        unsigned long deg1:6;
        unsigned long deg2:6;
        unsigned long deg3:6;
        unsigned long deg4:6;

        unsigned long msl_st:4;
        unsigned long seker_st:4;

        unsigned short crc;
   }L;
};


union GDL_Status_union
{
    unsigned char sta_buff[57];
    #pragma pack(1)
    struct GDL_Status
    {
        unsigned int time_tag;
        unsigned char reserved;
        unsigned char msg_id;
        unsigned short msg_seq_num;
        unsigned short header_csum;

        unsigned char dual_rx_index;
        unsigned char uplink_freq[2];
        unsigned char dnlink_freq[10];
        unsigned short dnlink_cdma;
        unsigned short uplink_cdma[10];
        unsigned short tx_antenna_status;
        unsigned short pa_status;
        unsigned char rx1_rssi;
        unsigned char rx2_rssi;
        unsigned char rx_pll_lock;
        unsigned char tx_pll_lock;
        unsigned char decoder1_track_acq;
        unsigned char decoder2_track_acq;
        unsigned char dnlink_fec_sel;
        unsigned char uplink_fec_sel;
    }L;
};


#pragma pack(1)
union GDL_Health_Union
{
    unsigned char GDL_Health_buff[29];
    struct GDL_Health
    {
        unsigned int time_tag;
        unsigned char reserved;
        unsigned char msg_id;
        unsigned short msg_seq_num;
        unsigned short header_csum;

        unsigned char dual_rx_index;
        unsigned char rx_pll_lock;
        unsigned char tx_pll_lock;
        unsigned char decoder1_track_acq;
        unsigned char decoder2_track_acq;
        unsigned char rx1_rssi;                //radio signal strength index
        unsigned char rx2_rssi;
        unsigned short txpwr_mode;
        unsigned short rx1_dopler;
        unsigned short rx2_dopler;
        unsigned short rx1_correlation;
        unsigned short rx2_correlation;
        unsigned short rf_switch_status;
    }L;
};

union GDL_CONFIG_Union
{
    unsigned char gdl_cfg_buff[44];
    struct DLconfig
    {
        unsigned int time_tag;
        unsigned char msg_id;
        unsigned char msl_id;
        unsigned short msg_seq_no;
        unsigned char up_freq;
        unsigned char down_freq;
        unsigned short uplink_cdma_code;
        unsigned short downlink_cdma_code;
        unsigned char uplink_fec;
        unsigned char dwnlnk_fec;
        unsigned short pa_config;
        unsigned short tx_sel;
        unsigned short checksum;
    }L;
};


#pragma pack(1)
union GTR_Diagnosys_Union
{
    unsigned char gtr_diagnosys_buff[28];
    struct GTR_Diagnosys
    {
        unsigned char start;
        unsigned char msg_id;
        unsigned char channel_no;
        unsigned char rx_frequency[8];
        unsigned char average_power[3];
        unsigned char max_power[3];
        unsigned char min_power[3];
        unsigned char correlation_factor[3];
        unsigned char v_avg[2];
        unsigned short crc;
        unsigned char end;
    }L;
};


#pragma pack(1)
union GTR_Health_Union
{
    unsigned char gtr_health_buff[16];
    struct GTR_Health
    {
        unsigned char start;
        unsigned char msg_id;
        unsigned char rx1_pll_lock_status;
        unsigned char rx2_pll_lock_status;
        unsigned char pa_on_off_status;
        unsigned char reserved;
        unsigned char agc1_rssi[3];
        unsigned char agc2_rssi[3];
        unsigned char correlator1_status;
        unsigned char correlator2_status;
        unsigned short crc;
        unsigned char end;
    }L;
};

union GTR_Status_Union
{
    unsigned char gtr_status_buff[58];
    #pragma pack(1)
    struct GTR_Status
    {
        unsigned char start;
        unsigned char msg_id;
        unsigned char rx_ch1_track_status;
        unsigned char rx_ch2_track_status;
        unsigned char rx_cdma[2][2];
        unsigned char tx_cdma[10][3];
        unsigned char rx_fec;
        unsigned char tx_fec;
        unsigned char data_post_ch1;
        unsigned char data_post_ch2;
        unsigned char sigstatus_indicator;
        unsigned char rx1_c_band_ld_st;
        unsigned char rx2_c_band_ld_st;
        unsigned char tx_c_band_ld_st;
        unsigned char tx_power_amp_st;
        unsigned char tx_power_level;
        unsigned char tx_antenna_selection;
        unsigned char rx1_pll_index[2];
        unsigned char rx2_pll_index[2];
        unsigned char tx_pll_index[2];
        unsigned short crc;
        unsigned char end;
    }L;
};

#endif // VARIABLES_H

