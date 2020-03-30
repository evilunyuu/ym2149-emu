/*
 * YM2149Emu.h
 *
 *  Created on: 30 мар. 2020 г.
 *      Author: unyuu
 */

#ifndef YM2149EMU_H_
#define YM2149EMU_H_

#include "SoundSource.h"
#include "globals.h"

constexpr int YM2149_FREQ = 3579545 / 2;

class YM2149_Noise
{
protected:
	int _sr { 0 };
	int _cycle_counter { 0 };
	int _period { 0 };
	int _period_counter { 0 };
public:
	YM2149_Noise() = default;

	void advance(int cycles);
	int output() const { return _sr & 1; }

	void set_period(int v) {
		_period = clamp(0, 31, v);
	}
};

class YM2149_Tone
{
protected:
	int _output { 0 };
	int _cycle_counter { 0 };
	int _period { 0 };
	int _period_counter { 0 };
public:
	YM2149_Tone() = default;

	void advance(int cycles);
	int output() const { return _output; }

	void set_period_hi(int v) {
		_period = (_period & 0x0ff) | ((v & 0x0f) << 8);
	}
	void set_period_lo(int v) {
		_period = (_period & 0xf00) | v;
	}
};

class YM2149_Noise_Tester: public SoundSource
{
protected:
	YM2149_Noise _noise;
public:
	YM2149_Noise_Tester() = default;
	virtual ~YM2149_Noise_Tester() = default;

	virtual void advance() {
		_noise.advance(YM2149_FREQ /
				AUDIO_SAMPLING_FREQ);
	}
	virtual double sample() const {
		return (_noise.output() - 0.5) * 2.;
	}

	void set_period(int per) {
		_noise.set_period(per);
	}
};

class YM2149_Tone_Tester: public SoundSource
{
protected:
	YM2149_Tone _noise;
public:
	YM2149_Tone_Tester() = default;
	virtual ~YM2149_Tone_Tester() = default;

	virtual void advance() {
		_noise.advance(YM2149_FREQ /
				AUDIO_SAMPLING_FREQ);
	}
	virtual double sample() const {
		return (_noise.output() - 0.5) * 2.;
	}

	void set_period(int per) {
		_noise.set_period_hi(per >> 8);
		_noise.set_period_lo(per & 0xff);
	}
};


#endif /* YM2149EMU_H_ */
