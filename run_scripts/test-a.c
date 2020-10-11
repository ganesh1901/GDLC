#include <stdio.h>
#include <string.h>


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
        unsigned long FU:6;
        unsigned long MLV:2;
        unsigned char reserved1;

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


float get_scaled( long t1, float scale_vale, float offset){

    return ( (t1 * scale_vale) - offset);
}



int main(){

	unsigned char buff[] = { 0x9e,0x76,0x33,0x75,0x00,0x01,0x00,0x17,0x10,0x30,0x75,0x33,0x76,0x9e,0x01,0x01,0x00,0x04,0x01,0x00,0x68,0x08,0x32,0x66,0x13,0x33,0x36,0x60,0x46,0x65,0xe1,0x2e,0xe8,0x1d,0x64,0x4b,0x90,0x00,0x00,0x9c,0x54,0x00,0x01,0x01,0x29,0xe1};
	
	memcpy(&d1, buff, sizeof buff);

	fprintf(stdout, "%d, %02d, %02d, %02d, %01d, %d, %6.4f, %6.4f, %6.4f, %4.2f, %4.2f, %4.2f, %4.2f, %4.2f, %4.2f, %4.2f, %4.2f, %4.2f, %4.2f, %4.2f, %4.2f, %d, %d, %04x \n",
                            d1.L.time_tag, d1.L.msg_id, d1.L.missile_id, d1.L.FU, d1.L.MLV, ntohs(d1.L.msg_seq_no),
                            get_scaled(d1.L.x_t, 1, 150000), get_scaled(d1.L.y_t, 1, 150000), get_scaled(d1.L.z_t, 1, 60000),
                            get_scaled(d1.L.vx_t, 1, 1500), get_scaled(d1.L.vy_t,  1, 1500), get_scaled( d1.L.vz_t, 1, 1500),
                            get_scaled(d1.L.tgt_ax_t, 1, 100), get_scaled(d1.L.tgt_ay_t, 1, 100), get_scaled(d1.L.tgt_az_t, 1, 100),
                            get_scaled(d1.L.tgt_pos_x, 1, 500), get_scaled(d1.L.tgt_pos_y, 1, 500), get_scaled(d1.L.tgt_pos_z, 1, 500),
                            get_scaled(d1.L.tgt_vel_x, 1, 50), get_scaled(d1.L.tgt_vel_y, 1, 50), get_scaled(d1.L.tgt_vel_z, 1, 50),
                            d1.L.target_type, d1.L.target_RCS, d1.L.crc );
}

