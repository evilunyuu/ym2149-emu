/*
 * SoundSource.h
 *
 *  Created on: 30 мар. 2020 г.
 *      Author: unyuu
 */

#ifndef SOUNDSOURCE_H_
#define SOUNDSOURCE_H_

class SoundSource
{
public:
	SoundSource() = default;
	virtual ~SoundSource() = default;

	virtual void advance() = 0;
	virtual double sample() const = 0;
};

class DummySoundSource: public SoundSource
{
public:
	DummySoundSource() = default;
	virtual ~DummySoundSource() = default;

	virtual void advance() {}
	virtual double sample() const { return 0.; }
};



#endif /* SOUNDSOURCE_H_ */
