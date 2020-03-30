/*
 * AudioDriver.cc
 *
 *  Created on: 30 мар. 2020 г.
 *      Author: unyuu
 */

#include "AudioDriver.h"


void ad_callback(void *userdata, Uint8 * stream,
        int len)
{
	reinterpret_cast<AudioDriver*>(userdata)->callback(
			reinterpret_cast<short *>(stream),
			size_t(len / sizeof(short)));
}

AudioDriver::AudioDriver()
{
	_spec.userdata = this;
	_spec.format = AUDIO_S16;
	_spec.freq = AUDIO_SAMPLING_FREQ;
	_spec.channels = 1;
	_spec.samples = 2048;
	_spec.callback = ad_callback;

	decltype(_spec) obt;

	_dev = SDL_OpenAudioDevice(nullptr, 0, &_spec, &obt,
			SDL_AUDIO_ALLOW_ANY_CHANGE);
	if (_dev < 2)
		SDL_DIE("SDL_OpenAudioDevice()", __FILE__, __LINE__-3);

	_source = std::make_shared<DummySoundSource>();
}

AudioDriver::~AudioDriver()
{
	pause();
	SDL_CloseAudioDevice(_dev);
}

void AudioDriver::pause()
{
	SDL_PauseAudioDevice(_dev, 1);
}

void AudioDriver::resume()
{
	SDL_PauseAudioDevice(_dev, 0);
}

void AudioDriver::callback(short * buffer, size_t length)
{
	for (unsigned ptr = 0; ptr < length; ptr++) {
		_source->advance();
		buffer[ptr] = short(_source->sample() * _master_volume);
	}
}
