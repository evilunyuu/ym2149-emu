/*
 * globals.h
 *
 *  Created on: 30 мар. 2020 г.
 *      Author: unyuu
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <stdexcept>
#include <memory>
#include <sstream>
#include <algorithm>
#include <SDL2/SDL.h>

constexpr int AUDIO_SAMPLING_FREQ = 44100;

static inline void DIE(
		const char * reason,
		const char * file = nullptr,
		int line = -1)
{
	std::stringstream ss;
	if (file != nullptr) {
		ss << file;
		if (line > 0)
			ss << ':' << line;
		ss << ": ";
	}
	ss << reason;

	throw std::runtime_error(ss.str());
}

static inline void SDL_DIE(
		const char * reason,
		const char * file = nullptr,
		int line = -1)
{
	std::stringstream ss;
	if (file != nullptr) {
		ss << file;
		if (line > 0)
			ss << ':' << line;
		ss << ": ";
	}
	ss << reason << ": " << SDL_GetError();

	throw std::runtime_error(ss.str());
}

template <typename T>
static inline T clamp(T min, T max, T val)
{
	return std::max(min, std::min(val, max));
}


#endif /* GLOBALS_H_ */
