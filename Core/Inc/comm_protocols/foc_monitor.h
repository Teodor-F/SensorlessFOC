#ifndef FOC_MONITOR_H
#define FOC_MONITOR_H

#define FOC_FRAME_HEADER 0xA55A

typedef struct foc_monitor_frame foc_monitor_frame_t;

struct __attribute__((packed)) foc_monitor_frame {
    uint16_t	header;
    int32_t  	ia_mA;
    int32_t  	ib_mA;
    int32_t  	ic_mA;
    float  		emf_alpha;
    float		emf_beta;
    float		theta_pll_rad;
    float		theta_smo_rad;
    float		theta_real_rad;
    float  		omega_pll_rad_s;
    float		omega_real_rad_s;
    uint16_t crc;
};

#endif // FOC_MONITOR_H
