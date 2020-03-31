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

void YM2149Emu::write(int reg, int val)
{
	switch(reg) {
	case  0: _tone[0].set_period_lo(val); break;
	case  1: _tone[0].set_period_hi(val); break;
	case  2: _tone[1].set_period_lo(val); break;
	case  3: _tone[1].set_period_hi(val); break;
	case  4: _tone[2].set_period_lo(val); break;
	case  5: _tone[2].set_period_hi(val); break;
	case  6: _noise.set_period(val); break;
	case  7: _mixer = val; break;
	case  8: _level[0] = val; break;
	case  9: _level[1] = val; break;
	case 10: _level[2] = val; break;
	case 11: _env.set_period_lo(val); break;
	case 12: _env.set_period_hi(val); break;
	case 13: _env.set_mode(val); break;
	default:
		;
	}
}

void YM2149Emu::advance(int cycles)
{
	_tone[0].advance(cycles);
	_tone[1].advance(cycles);
	_tone[2].advance(cycles);
	_noise.advance(cycles);
	_env.advance(cycles);
}

static const double s_fix_lev[16] = {
		0.000, 0.008, 0.011, 0.016,
		0.022, 0.031, 0.044, 0.063,
		0.088, 0.125, 0.177, 0.250,
		0.354, 0.500, 0.707, 1.000,
};

static const double s_env_lev[32] = {
		0.005, 0.006, 0.007, 0.008,
		0.009, 0.011, 0.013, 0.016,
		0.019, 0.022, 0.026, 0.031,
		0.037, 0.044, 0.053, 0.063,
		0.074, 0.088, 0.105, 0.125,
		0.149, 0.177, 0.210, 0.250,
		0.297, 0.354, 0.420, 0.500,
		0.595, 0.707, 0.841, 1.000,
};

double YM2149Emu::sample_ch(int ch) const
{
	int t_out = _tone[ch].output() |
			!!(_mixer & (1<<ch));
	int n_out = _noise.output() |
			!!(_mixer & (8<<ch));
	int out = t_out & n_out;

	if (!!(_mixer & (1<<ch)) &
		!!(_mixer & (8<<ch)))
			return 0.;
	double outv = out ? 1. : -1.;

	if (_level[ch] & 0x10)
		outv *= s_env_lev[_env.output() & 0x1f];
	else
		outv *= s_fix_lev[_level[ch] & 0x0f];

	return outv;
}

double YM2149Emu::sample_a() const
{
	return sample_ch(0);
}

double YM2149Emu::sample_b() const
{
	return sample_ch(1);
}

double YM2149Emu::sample_c() const
{
	return sample_ch(2);
}

double YM2149EmuMonoSource::sample() const
{
	return (_ym.sample_a() +
			_ym.sample_b() +
			_ym.sample_c()) / 3.;
}

void YM2149EmuMonoSource::advance()
{
	double total_cycles =
			double(YM2149_FREQ) /
			double(AUDIO_SAMPLING_FREQ) +
			_clock_error;
	int whole_cycles = int(floor(total_cycles));
	_clock_error = total_cycles - whole_cycles;
	_ym.advance(whole_cycles);
}

