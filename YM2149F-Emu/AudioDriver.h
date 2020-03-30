/*
 * AudioDriver.h
 *
 *  Created on: 30 мар. 2020 г.
 *      Author: unyuu
 */

#ifndef AUDIODRIVER_H_
#define AUDIODRIVER_H_

#include "globals.h"
#include "SoundSource.h"

class AudioDriver
{
protected:
	SDL_AudioSpec _spec {};
	SDL_AudioDeviceID _dev;

	std::shared_ptr<SoundSource> _source;
	int _master_volume { 0 };

	void callback(short * buffer, size_t length);
	friend void ad_callback(void *userdata, Uint8 * stream,
	        int len);
public:
	AudioDriver();
	~AudioDriver();

	void pause();
	void resume();

	void set_master_volume(int vol) {
		_master_volume = clamp(0, 32767, vol);
	}

	void set_source(const std::shared_ptr<SoundSource> & src) {
		if (src != nullptr)
			_source = src;
		else
			_source = std::make_shared<DummySoundSource>();
	}
};



#endif /* AUDIODRIVER_H_ */
