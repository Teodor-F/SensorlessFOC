#ifndef FOC_MONITOR_H
#define FOC_MONITOR_H

#define FOC_FRAME_HEADER 0xA55A

typedef enum foc_cmd foc_cmd_t;
typedef union foc_cmd_param foc_cmd_param_t;
typedef struct foc_monitor_frame foc_monitor_frame_t;
typedef struct foc_command_frame foc_command_frame_t;

enum foc_cmd {
	FOC_CMD_START = 0u,
	FOC_CMD_STOP,
	FOC_CMD_SET_SPEED
};



struct __attribute__((packed)) foc_monitor_frame {
    uint16_t	header;
    int32_t  	ia_mA;
    int32_t  	ib_mA;
    int32_t  	ic_mA;
    int32_t		i_alfa_ma;
    int32_t		i_beta_ma;
    int32_t		id_mA;
    int32_t		iq_mA;
    float_t  	emf_alpha;
    float_t		emf_beta;
    float_t		theta_observer_rad;
    float_t		theta_ref_rad;
    uint32_t  	velocity_observer_rpm;
    uint32_t	velocity_ref_rpm;
    uint16_t 	crc;
};

struct __attribute__((packed)) foc_command_frame {
    uint16_t	header;
    foc_cmd_t 	foc_command;
    uint32_t	foc_param;
};


#endif // FOC_MONITOR_H
