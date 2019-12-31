#ifndef _STATE_H_
#define _STATE_H_

enum state_t {
	STATE_START_SCREEN,
	STATE_ANALYZE,
	STATE_CALIBRATE_MENU,
	STATE_CALIBRATE_SETTING,
	STATE_CALIBRATE,
	STATE_ERROR,
};

enum state_ppo2_t {
	PPO2_1_4,
	PPO2_1_5,
	PPO2_1_6,
	MV,
};

enum state_yes_no_t {
	YES,
	NO,
};

#endif // _STATE_H_