
typedef union {
	unsigned char buffer[10];
#pragma pack(1)
	struct {
		unsigned int timetag;
		unsigned char reserved;
		unsigned char msgid;
		unsigned short seq_no;
		unsigned short header_crc;
	}L;
}header_radar_gdl_t;


union GDL_Status_union
{
	unsigned char sta_buff[57];
#pragma pack(1)
	struct GDL_Status
	{
		header_radar_gdl_t ccu_hdr;
		unsigned char dual_rx_index;
		unsigned char uplink_freq[2];
		unsigned char dnlink_freq[10];
		unsigned short dnlink_cdma;
		unsigned short uplink_cdma[10];
		unsigned short tx_antenna_status;
		unsigned short pa_status;
		unsigned short rx_rssi;
		unsigned short pll_lock;
		unsigned short decoder_track_acq;
		unsigned short fec_sel;

	}L;
};


#pragma pack(1)
union GDL_Health_Union
{
	unsigned char GDL_Health_buff[29];
	struct GDL_Health
	{
		header_radar_gdl_t ccu_hdr;
		unsigned char dual_rx_index;
		unsigned short pll_lock;
		unsigned short decoder_track_acq;
		unsigned short rx_rssi;                //radio signal strength index
		unsigned short txpwr_mode;
		unsigned short rx1_dopler;
		unsigned short rx2_dopler;
		unsigned short rx1_correlation;
		unsigned short rx2_correlation;
		unsigned short rf_switch_status;
	}L;
};

union GDL_Status_union gdl_status;
union GDL_Health_Union gdl_health;


int main(){
	printf(" %d%d \n ", sizeof gdl_status, sizeof gdl_health);
	return 0;
}
