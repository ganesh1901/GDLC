#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <arpa/inet.h>

union targetdwell_union
{
    unsigned char TargetBuf[36];
    #pragma pack(1)
    struct TargetDwell
    {
        unsigned int time_tag;
        unsigned char msg_id;
        unsigned char missile_id;
	unsigned char reserved;
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

union targetdwell_union d1;


union Missile_data_union
{
    unsigned char  MslDataBuf[36];
    #pragma pack(1)
    struct MslDwell
    {
        unsigned int time_tag;
        unsigned char msg_id;
        unsigned char missile_id;
        unsigned char reserved;
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

union Missile_data_union d2;



union INSData_union
{
    unsigned char insDataBuf[46];
    #pragma pack(1)
    struct INSData
    {

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

union INSData_union d3;


union Packet_D{
    unsigned char data[46];
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


union Packet_D d4;



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

union Packet_E d5;



float get_scaled(unsigned long t1, float scale_vale, float offset){
	
	return ( (t1 * scale_vale) - offset);
}


unsigned char buffer[] = { 0x78,0x15,0x57,0x43,0x00,0x01,0x00,0x01,0xb6,0xe3,0x00,0x00,0x00,0x00,0x01,0x01,0x00,0x05,0x00,0x01,0x68,0x08,0x32,0x66,0x13,0x33,0x36,0x60,0x46,0x65,0xe1,0x2e,0xe8,0x1d,0x64,0x4b,0x90,0x00,0x00,0x84,0x46,0x00,0x01,0x01,0x91,0x55};
//unsigned char buffer[] = {0x67,0x7e,0xa8,0x9c,0x00,0x01,0x3e,0x40,0xc2,0x47,0x00,0x00,0x00,0x00,0x01,0x01,0x00,0x05,0x3e,0x34,0x68,0x08,0x32,0x66,0x13,0x33,0x36,0x60,0x46,0x65,0xe1,0x2e,0xe8,0x1d,0x64,0x4b,0x90,0x00,0x00,0x84,0x46,0x00,0x01,0x01,0x91,0x55};

unsigned char buffer2[] = {0x78,0x15,0x57,0x5b,0x00,0x02,0x00,0x02,0x3d,0xc7,0x00,0x00,0x00,0x00,0x01,0x02,0x00,0x05,0x00,0x01,0x68,0x08,0x32,0x66,0x13,0x33,0x36,0x60,0x46,0x65,0xe1,0x2e,0x68,0x08,0x32,0x66,0x13,0x33,0x36,0x60,0x46,0x65,0xe1,0x2e,0x6d,0x61};

unsigned char buffer3[] = {0x12,0x34,0x56,0x78,0x00,0x53,0x00,0x5d,0x42,0x00,0x34,0x12,0x05,0x00,0x74,0x05,0x29,0x7a,0x82,0x2c,0x15,0xf8,0x3d,0x57,0x86,0xb1,0x99,0x2f,0x8f,0x81,0x0c,0x64,0x90,0x79,0xcc,0x63,0xb9,0x00,0x83,0x17,0xde,0xf7,0x81,0x00,0x64,0x04};

unsigned char buffer4[] = {0x12,0x34,0x56,0x78,0x00,0x33,0x02,0xec,0x13,0x00,0x34,0x12,0x05,0x00,0xd4,0x03,0x29,0x7a,0x82,0x2c,0x15,0xf8,0x3d,0x57,0x86,0xb1,0x99,0x2f,0x8f,0x81,0x0c,0x64,0x90,0x79,0xcc,0x63,0xb9,0x00,0x83,0x17,0xde,0xf7,0x81,0x00,0x86,0x51};

unsigned char buffer5[] = {0x12,0x34,0x56,0x78,0x00,0x43,0x02,0xdd,0x9f,0x00,0x34,0x12,0x05,0x00,0x21,0x04,0x29,0x7a,0x82,0x2c,0x15,0xf8,0x3d,0x57,0x86,0xb1,0x99,0x2f,0x8f,0x81,0x0c,0x64,0x90,0x79,0xcc,0x63,0xb9,0x00,0x83,0x17,0xde,0xf7,0x81,0x00,0xb6,0x3f};

int main(){


#if 0
        FILE *fp = fopen("CCU_RAW.bin", "rb");

        if(fp == NULL)
            printf(" exit \n");

        fseek(fp, 10, SEEK_SET);
        fread(&d1.TargetBuf, 46, 1, fp);
        fclose(fp);
#endif

        memcpy(&d1.TargetBuf, &buffer[10], sizeof d1.TargetBuf);

        fprintf(stdout, "PacketA- %d, %02d, %02d, %02d, %01d, %d, %6.4f, %6.4f, %6.4f, %4.2f, %4.2f, %4.2f, %4.2f, %4.2f, %4.2f, %4.2f, %4.2f, %4.2f, %4.2f, %4.2f, %4.2f, %d, %d, %04x \n",
                d1.L.time_tag, d1.L.msg_id, d1.L.missile_id, d1.L.FU, d1.L.MLV, (d1.L.msg_seq_no),
                get_scaled(d1.L.x_t, 1, 150000), get_scaled(d1.L.y_t, 1, 150000), get_scaled(d1.L.z_t, 1, 60000),
                get_scaled(d1.L.vx_t, 1, 1500), get_scaled(d1.L.vy_t,  1, 1500), get_scaled( d1.L.vz_t, 1, 1500),
                get_scaled(d1.L.tgt_ax_t, 1, 100), get_scaled(d1.L.tgt_ay_t, 1, 100), get_scaled(d1.L.tgt_az_t, 1, 100),
                get_scaled(d1.L.tgt_pos_x, 1, 500), get_scaled(d1.L.tgt_pos_y, 1, 500), get_scaled(d1.L.tgt_pos_z, 1, 500),
                get_scaled(d1.L.tgt_vel_x, 1, 50), get_scaled(d1.L.tgt_vel_y, 1, 50), get_scaled(d1.L.tgt_vel_z, 1, 50),
                d1.L.target_type, d1.L.target_RCS, d1.L.crc );

#if 0

        memcpy(&d2.MslDataBuf, &buffer2[10], sizeof d2.MslDataBuf);
        fprintf(stdout, "PacketB- %d, %02d, %02d, %02d, %01d, %d, %6.4f, %6.4f, %6.4f, %5.2f, %5.2f, %5.2f, %6.4f, %6.4f, %6.4f, %5.2f, %5.2f, %5.2f, %04x \n",
                d2.L.time_tag, d2.L.msg_id, d2.L.missile_id, d2.L.FU, d2.L.MLV, d2.L.msg_seq_no,
                get_scaled(d2.L.tgt_pos_x, 1, 150000), get_scaled(d2.L.tgt_pos_y, 1, 150000), get_scaled(d2.L.tgt_pos_z, 1, 60000),
                get_scaled(d2.L.tgt_vel_x, 1, 1500), get_scaled(d2.L.tgt_vel_y,  1, 1500), get_scaled( d2.L.tgt_vel_z, 1, 1500),
                get_scaled(d2.L.msl_pos_x, 1, 150000), get_scaled(d2.L.msl_pos_y, 1, 150000), get_scaled(d2.L.msl_pos_z, 1, 60000),
                get_scaled(d2.L.msl_vel_x, 1, 1500), get_scaled(d2.L.msl_vel_y, 1, 1500), get_scaled(d2.L.msl_vel_z, 1, 1500),
                d2.L.crc );


        memcpy(&d3.insDataBuf[0], &buffer4[0], sizeof d3.insDataBuf);

        fprintf(stdout, "PacketC- %d, %02d, %02d, %02d, %01d, %d, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %d, %d, %04X \n",
                d3.L.time_tag, d3.L.msgid, d3.L.missile_id, d3.L.FU, d3.L.MLV, d3.L.msg_seq_no,
                get_scaled(d3.L.msl_pos_x, 1, 150000), get_scaled(d3.L.msl_pos_y, 1, 150000), get_scaled(d3.L.msl_pos_z, 1, 60000),
                get_scaled(d3.L.msl_vel_x, 1, 1500), get_scaled(d3.L.msl_vel_y, 1, 1500), get_scaled(d3.L.msl_vel_z, 1, 1500),
                get_scaled(d3.L.msl_acc_x, 1, 400), get_scaled(d3.L.msl_acc_y, 1, 400), get_scaled(d3.L.msl_acc_z, 1, 400),
                get_scaled(d3.L.body_rates_x, 1, 400), get_scaled(d3.L.body_rates_y, 1, 400), get_scaled(d3.L.body_rates_z, 1, 400),
                get_scaled(d3.L.euler_angle_x, 1, 180), get_scaled(d3.L.euler_angle_y, 1, 90), get_scaled(d3.L.euler_angle_z, 1, 90),
                get_scaled(d3.L.deg1, 1, 30), get_scaled(d3.L.deg2, 1, 30), get_scaled(d3.L.deg3, 1, 30), get_scaled(d3.L.deg4, 1, 30),
                d3.L.msl_st, d3.L.seker_st, d3.L.crc);


        memcpy(&d4.data, buffer5, sizeof d4.data);
        fprintf(stdout, "PacketD- %d, %02d, %02d, %02d, %01d, %d, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %d, %d, %6.2f, %6.2f, %6.2f, %6.2f, %6.2f, %6.2f, %6.2f, %6.2f, %6.2f, %04X \n",
                d4.L.time_tag, d4.L.msgid, d4.L.msl_id, d4.L.FU, d4.L.MLV, d4.L.msg_seq_no,
                get_scaled(d4.L.msl_acc_x, 1, 400), get_scaled(d4.L.msl_acc_y, 1, 400), get_scaled(d4.L.msl_acc_z, 1, 400),
                get_scaled(d4.L.body_rates_x, 1, 400), get_scaled(d4.L.body_rates_y, 1, 400), get_scaled(d4.L.body_rates_z, 1, 400),
                get_scaled(d4.L.euler_angle_x, 1, 180), get_scaled(d4.L.euler_angle_y, 1, 90), get_scaled(d4.L.euler_angle_z, 1, 90),
                get_scaled(d4.L.deg1, 1, 30), get_scaled(d4.L.deg2, 1, 30), get_scaled(d4.L.deg3, 1, 30), get_scaled(d4.L.deg4, 1, 30),
                d4.L.msl_st, d4.L.seker_st,
                get_scaled(d4.L.seek1, 1, 0), get_scaled(d4.L.seek2, 1, 2000), get_scaled(d4.L.seek3, 1, 50), get_scaled(d4.L.seek4, 1, 50),
                get_scaled(d4.L.seek5, 0.1, 5), get_scaled(d4.L.seek6, 0.1, 5), get_scaled(d4.L.seek7, 0.1, 5), get_scaled(d4.L.seek8, 0.1, 5),
                get_scaled(d4.L.seek9, 0.1, 5), d4.L.crc);



        memcpy(&d5.buffer[0], &buffer3[0], sizeof d5.buffer);

        fprintf(stdout, "PacketE- %d, %02d, %02d, %02d, %01d, %d, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %6.4f, %d, %d, %04X \n",
                d5.L.time_tag, d5.L.msgid, d5.L.msl_id, d5.L.FU, d5.L.MLV, d5.L.msg_seq_no,
                get_scaled(d5.L.msl_pos_x, 1, 150000), get_scaled(d5.L.msl_pos_y, 1, 150000), get_scaled(d5.L.msl_pos_z, 1, 60000),
                get_scaled(d5.L.msl_vel_x, 1, 1500), get_scaled(d5.L.msl_vel_y, 1, 1500), get_scaled(d5.L.msl_vel_z, 1, 1500),
                get_scaled(d5.L.msl_acc_x, 1, 400), get_scaled(d5.L.msl_acc_y, 1, 400), get_scaled(d5.L.msl_acc_z, 1, 400),
                get_scaled(d5.L.body_rates_x, 1, 400), get_scaled(d5.L.body_rates_y, 1, 400), get_scaled(d5.L.body_rates_z, 1, 400),
                get_scaled(d5.L.euler_angle_x, 1, 180), get_scaled(d5.L.euler_angle_y, 1, 90), get_scaled(d5.L.euler_angle_z, 1, 90),
                get_scaled(d5.L.deg1, 1, 30), get_scaled(d5.L.deg2, 1, 30), get_scaled(d5.L.deg3, 1, 30), get_scaled(d5.L.deg4, 1, 30),
                d5.L.msl_st, d5.L.seker_st, d5.L.crc);
#endif
}
