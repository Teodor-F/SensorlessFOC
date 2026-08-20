#include <config/motor_cfg.h>
//
//#include <adc.h>
//#include <foc.h>
//#include <foc_monitor.h>
//#include <mc_svm.h>
//#include <pi_cntrl.h>
//#include <sliding_mode_observer.h>
//#include <current_measure.h>
//#include <pll.h>
//#include <math.h>
//#include <mcpwm.h>
//#include <stdint.h>
//#include <target.h>
//#include <motor.h>
//#include <numeric_constants.h>
///*---------------- Private defines -------------------------------------------*/
//
////========================================================================
//	// Open-loop params
//#define FOC_TS_S                	((float)(1.0f / (float)PWM_FREQ_HZ))
//#define FOC_OPEN_LOOP_FREQ_HZ		10.0f
//#define FOC_OPEN_LOOP_THETA_STEP	((float)(CONSTANT_TWO_PI * FOC_OPEN_LOOP_FREQ_HZ*FOC_TS_S))
//#define OPEN_LOOP_VQ_MV				((float)(2000.0f))
//#define PI_ID_IQ_OUT_LIM_V			((float)(MOTOR_NOM_MV * ONE_BY_SQRT_THREE))
//
////========================================================================
//	// Rotor alignment
//#define FOC_ALIGN_VD_MV 			1000.0f
//#define FOC_ALIGN_DURATION_MS   	50
//#define FOC_ALIGN_TICKS         	((uint32_t)(FOC_ALIGN_DURATION_MS * PWM_FREQ_HZ / 1000u))
//#define EMF_THRESHOLD_FOR_CL		500.0f
//
//
///*---------------- Private typedefs ------------------------------------------*/
//
///*---------------- Private enums ---------------------------------------------*/
//
///*---------------- Private macros --------------------------------------------*/
//
///*---------------- Private structs -------------------------------------------*/
//
///*---------------- Private variables & constants -----------------------------*/
//static foc_mode_t   mode            = MODE_DISABLED;
//static pi_cntrl_t   current_d_pi    = {0};
//static pi_cntrl_t   current_q_pi    = {0};
//static smo_t        smo             = {0};
//static pll_t        pll             = {0};
//static uint32_t     align_tick_cnt  = 0u;
//
//
//static float vd = FOC_ALIGN_VD_MV;
//static float vq = OPEN_LOOP_VQ_MV;
//
//static float i_alpha = 0.0f, i_beta = 0.0f;
//
//static int32_t current_a_ma, current_b_ma, current_c_ma;
//static float theta_forced = 0.0f;
//static float theta_pll = 0.0f;
//static float theta_used = 0.0f;
//static float omega_pll = 0.0f;
//static float last_v_alpha = 0.0f;
//static float last_v_beta  = 0.0f;
//static smo_emf_est_t emf = {0};
//
//
//float open_loop_freq = 15.0f;
//float target_freq = 50.0f;
//float open_loop_accel = 200.0f;
//
//
//
///*---------------- Public module variable & constants definitions ------------*/
//volatile bool foc_telemetry_ready = false;
//volatile foc_frame_t foc_telemetry_frame = {0};
//
///*---------------- Private function declarations -----------------------------*/
//static inline void apply_voltage(float vd, float vq, float voltage, float theta);
//
///*---------------- Private function definitions -----------------------------*/
//static inline void apply_voltage(float vd, float vq, float voltage, float theta)
//{
//    float s = sinf(theta);
//    float c = cosf(theta);
//    inv_park_transform(vd, vq, s, c, &last_v_alpha, &last_v_beta);
//
//    float du, dv, dw;
//    mc_svm(last_v_alpha, last_v_beta, voltage, &du, &dv, &dw);
//    hal_pwm_set_duties(du, dv, dw);
//}
//
//void foc_init(void)
//{
////========================================================================
//	// PI-controllers for Id and Iq initialization
//	pi_cntrl_cfg_t current_pi_cntrl_cfg = {
//		    .kp = 2100.0f,
//			.ki = 2000.0f,
//			.out_limit = PI_ID_IQ_OUT_LIM_V,
//			.ts = FOC_TS_S
//	};
//	pi_cntrl_init(&current_d_pi, &current_pi_cntrl_cfg);
//	pi_cntrl_init(&current_q_pi, &current_pi_cntrl_cfg);
//
////========================================================================
//	// Sliding-mode observer initialization
//	smo_cfg_t smo_cfg = {
//		.rs = RS_MILLI_OHM,
//		.ls = LS_UH,
//		.ts = FOC_TS_S,
//		.boundary = 150.0f,
//	    .k_sliding_gain = 50.0f,
//	    .g_emf_gain = 0.05f
//
//	};
//	smo_init(&smo, &smo_cfg);
//
////========================================================================
//	// Phase-locked-loop initialization
//	pll_cfg_t pll_cfg = {
//	    .kp = 60.0f,
//	    .ki = 1200.0f,
//	    .ts = FOC_TS_S,
//	    .omega_max = 1200.0f,
//	};
//	pll_init(&pll, &pll_cfg);
//}
//
//void foc_callback(void *param)
//{
//	static uint16_t telem_log_cnt = 0u;
//
//	//========================================================================
//	// 1. Currents
//	current_measure_act_curr_t curr_meas_currs = current_measure_get_currents();
//	current_a_ma = curr_meas_currs.curr_a;
//	current_b_ma = curr_meas_currs.curr_b;
//	current_c_ma = curr_meas_currs.curr_c;
//
//	//========================================================================
//	// 2. Clarke
//	clarke_transform(current_a_ma, current_b_ma, current_c_ma, &i_alpha, &i_beta);
//
//	//========================================================================
//	// 3. SMO
//	smo_process(&smo, last_v_alpha, last_v_beta, i_alpha, i_beta);
//	emf = smo_get_est_emfs(&smo);
//
//	//========================================================================
//	// 4. PLL
//	pll_process(&pll, emf.e_alpha, emf.e_beta);
//	theta_pll = pll_get_est_theta(&pll);
//	omega_pll = pll_get_est_omega(&pll);
//
//	//========================================================================
//	// 5. STATE MACHINE
//
//	switch (mode)
//	{
//
//		// -------------------------------------------------
//		case MODE_ROTOR_ALIGN:
//		{
//			if (align_tick_cnt < FOC_ALIGN_TICKS)
//			{
//				align_tick_cnt++;
//			}
//			else
//			{
//				foc_set_mode(MODE_OPEN_LOOP);
//
//				theta_forced = 0.0f;
//				open_loop_freq = 15.0f;
//				vq = 1500.0f;
//
//				pll_reset(&pll);
//				smo_reset(&smo);
//			}
//
//			apply_voltage(vd, 0.0f, MOTOR_NOM_MV, theta_forced);
//			break;
//		}
//
//			// -------------------------------------------------
//		case MODE_OPEN_LOOP:
//		{
//			float theta_step = CONSTANT_TWO_PI * open_loop_freq * FOC_TS_S;
//			theta_forced += theta_step;
//			if (theta_forced >= CONSTANT_TWO_PI)
//				theta_forced -= CONSTANT_TWO_PI;
//			theta_used = theta_forced;
//			apply_voltage(0.0f, vq, VBUS_MV, theta_used);
//			break;
//		}
//		default:
//		{
//			hal_pwm_set_duties(0.0f, 0.0f, 0.0f);
//			break;
//		}
//	}
//
//    //========================================================================
//    // telemetry
//    if (telem_log_cnt >= 60u)
//    {
//        telem_log_cnt = 0;
//
//        foc_telemetry_frame.header = FOC_FRAME_HEADER;
//        foc_telemetry_frame.ia_mA = current_a_ma;
//        foc_telemetry_frame.ib_mA = current_b_ma;
//        foc_telemetry_frame.ic_mA = current_c_ma;
//        foc_telemetry_frame.emf_alpha = emf.e_alpha;
//        foc_telemetry_frame.emf_beta = emf.e_beta;
//        foc_telemetry_frame.omega_mrad_s = omega_pll;
//        foc_telemetry_frame.theta_mrad = theta_pll;
//        foc_telemetry_frame.theta_forced = theta_forced;
//
//        foc_telemetry_ready = true;
//    }
//
//    telem_log_cnt++;
//}
//
//void foc_set_mode(foc_mode_t new_mode)
//{
//    if (mode == new_mode)
//        return;
//
//    switch (new_mode)
//    {
//
//    // ---------------------------------
//    case MODE_DISABLED:
//    {
//        hal_pwm_stop();
//        break;
//    }
//
//    // ---------------------------------
//    case MODE_ROTOR_ALIGN:
//    {
//        align_tick_cnt = 0u;
//        theta_forced = 0.0f;
//        break;
//    }
//
//    // ---------------------------------
//    case MODE_OPEN_LOOP:
//    {
//        theta_forced = 0.0f;
//        open_loop_freq = 5.0f;
//        vq = 1500.0f;
//
//        pll_reset(&pll);
//        smo_reset(&smo);
//
//        break;
//    }
//    // ---------------------------------
//    default:
//        break;
//    }
//
//    mode = new_mode;
//}
