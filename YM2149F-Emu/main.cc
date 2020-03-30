/*
 * main.cc
 *
 *  Created on: 30 мар. 2020 г.
 *      Author: unyuu
 */

#include <iostream>

#include "globals.h"
#include "AudioDriver.h"
#include "YM2149Emu.h"

int main(int, char **)
{

	try {
		SDL_Init(SDL_INIT_EVERYTHING);

		AudioDriver drv;

		drv.resume();

		auto t =
				std::make_shared<YM2149_Tone_Tester>();

		drv.set_source(t);
		drv.set_master_volume(10000);

		for (unsigned i = 0; i < 4096; i+= 128) {
			t->set_period(i);
			SDL_Delay(100);
		}

	} catch (const std::exception &e) {
		std::cerr << "Произошло исключение:" << std::endl <<
				e.what() << std::endl;
	} catch (...) {
		std::cerr << "Произошло неизвестное исключение!" << std::endl;
	}




	return 0;
}

