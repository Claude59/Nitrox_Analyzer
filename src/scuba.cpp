#include "scuba.h"

uint16_t calc_mod(uint16_t fO2, uint16_t pO2_max = 1600u)
{
	return (uint16_t)(((uint32_t)pO2_max * 10000ul) / (uint32_t)fO2) - 1000ul;
}