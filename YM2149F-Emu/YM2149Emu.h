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

class YM2149_Envelope
{
protected:
	enum class State { NORM, INV, HOLD };

	int _output { 0 };
	int _cycle_counter { 0 };
	int _period { 0 };
	int _period_counter { 0 };
	int _mode { 0 };
	State _state { State::NORM };

	void reset();
	bool cont() const { return _mode & 8; }
	bool att() const  { return _mode & 4; }
	bool alt() const  { return _mode & 2; }
	bool hold() const { return _mode & 1; }
public:
	YM2149_Envelope() = default;

	void advance(int cycles);
	int output() const { return _output; }

	void set_period_hi(int v) {
		_period = (_period & 0x00ff) | ((v & 0x00ff) << 8);
	}
	void set_period_lo(int v) {
		_period = (_period & 0xff00) | v;
	}
	void set_mode(int v) {
		_mode = v & 0x0f;
		reset();
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

class YM2149Emu
{
protected:
	YM2149_Tone _tone[3];
	YM2149_Noise _noise;
	YM2149_Envelope _env;
	int _mixer { 0x3f };
	int _level[3] { 0, 0, 0 };

	double sample_ch(int ch) const;
public:
	YM2149Emu() = default;

	void write(int reg, int val);
	void advance(int cycles);
	double sample_a() const;
	double sample_b() const;
	double sample_c() const;
};

class YM2149EmuMonoSource: public SoundSource
{
protected:
	YM2149Emu _ym;
	double _clock_error { 0. };
public:
	YM2149EmuMonoSource() = default;
	virtual ~YM2149EmuMonoSource() = default;

	virtual void advance();
	virtual double sample() const;
};


#endif /* YM2149EMU_H_ */
