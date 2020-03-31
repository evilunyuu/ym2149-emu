/*
 * main.cc
 *
 *  Created on: 30 мар. 2020 г.
 *      Author: unyuu
 */

#include <iostream>
#include <fstream>
#include <cstdint>
#include <netinet/in.h>
#include <chrono>

#include "globals.h"
#include "AudioDriver.h"
#include "YM2149Emu.h"


#pragma pack(push, 1)
struct YM6HDR
{
	char file_id[4];
	char check[8];
	uint32_t frames;
	uint32_t attrs;
	uint16_t drums;
	uint32_t ymfreq;
	uint16_t fps;
	uint32_t loop;
	uint16_t add;
};
#pragma pack(pop)


int main(int argc, char ** argv)
{

	try {
		SDL_Init(SDL_INIT_EVERYTHING);

		AudioDriver drv;

		drv.resume();

		auto t =
				std::make_shared<YM2149EmuMonoSource>();

		drv.set_source(t);
		drv.set_master_volume(20000);

		std::fstream ymfile;
		ymfile.open(argv[1],
				std::ios::in | std::ios::binary);
		YM6HDR hdr;
		ymfile.read(reinterpret_cast<char*>(&hdr),
				sizeof(hdr));
		hdr.frames = ntohl(hdr.frames);
		std::cout << "ID: " <<
				hdr.file_id[0] <<
				hdr.file_id[1] <<
				hdr.file_id[2] <<
				hdr.file_id[3] << std::endl;
		std::cout << "Кадров в файле: " << hdr.frames << std::endl;

		for (unsigned i = 0; i < 3; i++) {
			char c;
			do {
				ymfile.read(&c, 1);
				std::cout << c;
			} while (c != '\0');
			std::cout << std::endl;
		}
		std::cout.flush();

		uint8_t data[16][hdr.frames];
		ymfile.read(reinterpret_cast<char*>(
				&data[0][0]),
				16*hdr.frames);

		typedef std::chrono::steady_clock Clk;

		auto t1 = Clk::now();
		auto t2 = t1 + std::chrono::milliseconds(20);

		for (unsigned pos = 0; pos <  hdr.frames; pos++) {
			do {
				t1 = Clk::now();
			} while (t1 < t2);
			t2 += std::chrono::milliseconds(20);

			for (int reg = 0; reg < 14; reg++) {
				if (reg == 13 and data[reg][pos] == 0xff)
					continue;
				t->write(reg, data[reg][pos]);
			}
		}


		ymfile.close();

	} catch (const std::exception &e) {
		std::cerr << "Произошло исключение:" << std::endl <<
				e.what() << std::endl;
	} catch (...) {
		std::cerr << "Произошло неизвестное исключение!" << std::endl;
	}




	return 0;
}

