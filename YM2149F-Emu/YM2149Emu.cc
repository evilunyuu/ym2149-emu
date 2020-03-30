/*
 * YM2149Emu.cc
 *
 *  Created on: 30 мар. 2020 г.
 *      Author: unyuu
 */
#include "YM2149Emu.h"

void YM2149_Noise::advance(int cycles)
{
	_cycle_counter += cycles;
	while (_cycle_counter >= 16) {
		_cycle_counter -= 16;

		if (_period_counter > 0)
			_period_counter--;
		else {
			_period_counter = _period;
			int nb =
					!!(_sr & 1) ^
					!!(_sr & 8) ^
					(_sr == 0);
			_sr = (_sr >> 1) | (nb << 16);
		}
	}
}

void YM2149_Tone::advance(int cycles)
{
	_cycle_counter += cycles;
	while (_cycle_counter >= 8) {
		_cycle_counter -= 8;

		if (_period_counter > 0)
			_period_counter--;
		else {
			_period_counter = _period;
			_output = !_output;
		}
	}
}



