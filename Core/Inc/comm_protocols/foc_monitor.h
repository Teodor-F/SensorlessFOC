#ifndef FOC_MONITOR_H
#define FOC_MONITOR_H

#define FOC_FRAME_HEADER 0xA55A

typedef struct foc_monitor_frame foc_monitor_frame_t;

struct __attribute__((packed)) foc_monitor_frame {
    uint16_t	header;
    int32_t  	ia_mA;
    int32_t  	ib_mA;
    int32_t  	ic_mA;
    int32_t		id_mA;
    int32_t		iq_mA;
    float_t  	emf_alpha;
    float_t		emf_beta;
    float_t		theta_pll_rad;
    float_t		theta_ref_log_rad;
    float_t		theta_real_rad;
    float_t  	velocity_pll_rpm;
    float_t		velocity_setpoint;
    uint16_t 	crc;
};

#endif // FOC_MONITOR_H
