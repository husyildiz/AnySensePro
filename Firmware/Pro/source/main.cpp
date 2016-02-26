/*
 * main.cpp
 *
 *  Created on: 09.09.2015
 *      Author: cem
 */

#include <DJIController.h>

#include <MAVLinkComm.h>

#include <Stopwatch.h>
#include <scmRTOS.h>
#include <OSAL.h>

#include <Configuration.h>
#include <SensorStore.h>
#include <USBWorker.h>

#include "Board.h"

using namespace App;

USBWorker usb_worker(CDCDevice);
DJIController djiController(Board::FC::CAN);
MAVLinkComm mavLinkComm(Board::OSD::USART);

// Process types
typedef OS::process<OS::pr0, 1024> TProc0;
typedef OS::process<OS::pr1, 1024> TProc1;
typedef OS::process<OS::pr2, 2048> TProc2;
typedef OS::process<OS::pr3, 1024> TProc3;
typedef OS::process<OS::pr4, 300> TProc4;

// Process objects
TProc0 Proc0;
TProc1 Proc1;
TProc2 Proc2;
TProc3 Proc3;
TProc4 Proc4;

#define BUFFER		2048
#define F10MB		10485760/BUFFER
#define F100MB		104857600/BUFFER

spiffs fs;
FIL file;
char content[BUFFER] = { 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd' };

uint32_t get_fattime(void)
{
	uint32_t time = 0;
	return time;
}

int main()
{
	Board::Init();

	OS::run();
}

namespace OS
{
template<>
OS_PROCESS void TProc0::exec()
{
//	u32_t total = 0, used = 0;
//
//
//	flashStorage.Mount(&fs);
//	SPIFFS_unmount(&fs);
//	SPIFFS_format(&fs);
//	flashStorage.Mount(&fs);
//
//	SPIFFS_info(&fs, &total, &used);
//	char buf[12];
//
//	// Surely, I've mounted spiffs before entering here
//
//	spiffs_file fd = SPIFFS_open(&fs, "my_file", SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR, 0);
//
//	for (int i = 0; i < 65; i++)
//		SPIFFS_write(&fs, fd, content, BUFFER);
//	SPIFFS_close(&fs, fd);
//
//	SPIFFS_info(&fs, &total, &used);
//
//	fd = SPIFFS_open(&fs, "my_file", SPIFFS_RDWR, 0);
//	SPIFFS_read(&fs, fd, (u8_t *) buf, 12);
//
//	SPIFFS_close(&fs, fd);

	djiController.Init();
	djiController.Run();
}

template<>
OS_PROCESS void TProc1::exec()
{
	mavLinkComm.Init();
	mavLinkComm.Run();
}

template<>
OS_PROCESS void TProc2::exec()
{
	//Utils::Stopwatch sp;
	uint32_t read = 0;
	uint32_t written = 0;

	volatile uint32_t stop[5] = { 0 };

	FRESULT fr = FR_TIMEOUT;
	for (;;)
	{
		//Write 10.485.760 Bytes
		//sp.Reset();
		fr = f_open(&file, "SD:10.bin", FA_READ | FA_WRITE | FA_CREATE_ALWAYS);
		if (fr == FR_OK)
		{
			for (uint32_t i = 0; (i < F10MB && fr == FR_OK); i++)
			{
				fr = f_write(&file, content, BUFFER, (UINT*) &written);
			}

			f_close(&file);
		}

		//if (stop[1] == 0)
		//	stop[1] = sp.ElapsedTime();
		//stop[1] = (sp.ElapsedTime() + stop[1]) / 2;

		//Write 104.857.600 Bytes
		//sp.Reset();
		fr = f_open(&file, "SD:100.bin", FA_READ | FA_WRITE | FA_CREATE_ALWAYS);
		if (fr == FR_OK)
		{
			for (uint32_t i = 0; (i < F100MB && fr == FR_OK); i++)
			{
				fr = f_write(&file, content, BUFFER, (UINT*) &written);
			}

			f_close(&file);
		}
		//if (stop[2] == 0)
		//	stop[2] = sp.ElapsedTime();
		//stop[2] = (sp.ElapsedTime() + stop[2]) / 2;

		//Read 10.485.760 Bytes
		//sp.Reset();
		fr = f_open(&file, "SD:10.bin", FA_READ | FA_OPEN_EXISTING);
		if (fr == FR_OK)
		{
			do
			{
				fr = f_read(&file, content, BUFFER, (UINT*) &read);
			} while (fr == FR_OK && read == BUFFER);

			f_close(&file);
		}
		//if (stop[3] == 0)
		//	stop[3] = sp.ElapsedTime();
		//stop[3] = (sp.ElapsedTime() + stop[3]) / 2;

		//Read 104.857.600 Bytes
		//sp.Reset();
		fr = f_open(&file, "SD:100.bin", FA_READ | FA_OPEN_EXISTING);
		if (fr == FR_OK)
		{
			do
			{
				fr = f_read(&file, content, BUFFER, (UINT*) &read);
			} while (fr == FR_OK && read == BUFFER);

			f_close(&file);
		}
		//if (stop[4] == 0)
		//	stop[4] = sp.ElapsedTime();
		//stop[4] = (sp.ElapsedTime() + stop[4]) / 2;

		sleep(delay_ms(500));
	}
}

template<>
OS_PROCESS void TProc3::exec()
{
	usb_worker.Run();
}

template<>
OS_PROCESS void TProc4::exec()
{
	Board::LedError.PowerUp();
	Board::LedError.ModeSetup(GPIO_MODE_OUTPUT, GPIO_PUPD_NONE);
	Board::LedError.SetOutputOptions(GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ);

	Board::LedActivity.PowerUp();
	Board::LedActivity.ModeSetup(GPIO_MODE_OUTPUT, GPIO_PUPD_NONE);
	Board::LedActivity.SetOutputOptions(GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ);

	for (;;)
	{
		if (djiController.err)
			Board::LedError.On();

		Board::LedActivity.Toggle();
		OSAL::Timer::SleepMS(20);
	}
}

}
