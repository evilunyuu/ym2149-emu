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

void YM2149_Envelope::reset()
{
	_state = State::NORM;
	_period_counter = _period;
	_output = att() ? 0 : 31;
	/*
	 * CONT	ATT	ALT	HOLD	OUT
	 * 0	0	x	x		1
	 * 0	1	x	x		0
	 * 1	0	0	0		1
	 * 1	0	0	1		1
	 * 1	0	1	0		1
	 * 1	0	1	1		1
	 * 1	1	0	0		0
	 * 1	1	0	1		0
	 * 1	1	1	0		0
	 * 1	1	1	1		0
	 */
}

void YM2149_Envelope::advance(int cycles)
{
	_cycle_counter += cycles;
	while (_cycle_counter >= 16) {
		_cycle_counter -= 16;

		if (_period_counter > 0)
			_period_counter--;
		else {
			_period_counter = _period;

			switch (_state) {
			case State::NORM:
				if (att()) _output++; else _output--;
				if (_output < 0 or _output >= 32) {
					_output = ((not att() xor alt()
								xor hold()) and cont()) ?
										31 : 0;
					if (hold()) _state = State::HOLD;
					else if (alt()) _state = State::INV;
				}
				break;
			case State::INV:
				if (att()) _output--; else _output++;

				if (_output < 0 or _output >= 32) {
					_output = att() ? 0 : 31;
					_state = State::NORM;
				}
				break;
			case State::HOLD:
				_output = ((att() xor alt()) and cont()) ?
						31 : 0;
				break;
			default:
				DIE("Всё плохо, огибающая в "
						"неизвестном состоянии!");
			}
		}
	}
}



