/*
 * USBWorker.cpp
 *
 *  Created on: 17.10.2015
 *      Author: cem
 */

#include <USBWorker.h>
#include <stddef.h>
#include "Board.h"
#include "MAVLinkLayer.h"
#include <Configuration.h>
#include <StorageFlashSPI.h>

#include "libopencm3/cm3/scb.h"

namespace App
{

void USBWorker::Init()
{
	m_CDC.Init();
}

void USBWorker::Run()
{
	uint16_t dataLen = 0;
	uint8_t currentItem = 0;
	CONFIG_COMMAND cmd = CONFIG_COMMAND_NACK;
	TransferType type = TransferType::FW_Update;
	for (;;)
	{
		if (m_channels.pop(m_msg, delay_ms(m_delay_ms)))
		{
			m_delay_ms = USB_WORKER_DELAY_CONNECTED;
			established = 1;

			switch (m_msg.msgid)
			{
			case MAVLINK_MSG_ID_CONFIGURATION_CONTROL:
				cmd = (CONFIG_COMMAND) mavlink_msg_configuration_control_get_command(&m_msg);

				switch (cmd)
				{
				case CONFIG_COMMAND_GET_VERSION:
					dataLen = m_mavlink.PackConfigurationVersion3(&m_msg, FIRMWARE_VERSION, HARDWARE_VERSION);
					break;
				case CONFIG_COMMAND_GET_CONFIGURATION:
					dataLen = m_mavlink.PackConfigurationData(&m_msg, Config.GetConfiguration().Data);
					break;
				default:
					break;
				}

				break;
			case MAVLINK_MSG_ID_DATA_TRANSMISSION_HANDSHAKE:
				type = (TransferType) mavlink_msg_data_transmission_handshake_get_type(&m_msg);

				switch (type)
				{
				case TransferType::FW_Update:
					uint32_t size = mavlink_msg_data_transmission_handshake_get_size(&m_msg);
					ReceiveUpdate(size);
					break;
				}

				break;
			case MAVLINK_MSG_ID_CONFIGURATION_DATA:
				ConfigurationData& cfg = Config.GetConfiguration();
				mavlink_msg_configuration_data_get_data(&m_msg, cfg.Data);
				Config.SetConfiguration(cfg);

				dataLen = m_mavlink.PackCommandAck(&m_msg, MAV_CMD_ACK_OK);
				break;
			}
		}
		else if (established == 1)
		{
			switch (currentItem)
			{
			case 0:
				dataLen = m_mavlink.PackGPS(&m_msg);
				break;
			case 1:
				dataLen = m_mavlink.PackAttitude(&m_msg);
				break;
			case 2:
				dataLen = m_mavlink.PackBatteryPack(&m_msg);
				break;
			case 3:
				dataLen = m_mavlink.PackHeartbeat(&m_msg);
				break;
			case 4:
				dataLen = m_mavlink.PackRCOut(&m_msg);
				break;
			case 5:
				dataLen = m_mavlink.PackSystemStatus(&m_msg);
				break;
			case 6:
				dataLen = m_mavlink.PackVFRHud(&m_msg);
				break;
			}

			currentItem = (currentItem + 1) % 7;
		}

		if (dataLen > 0)
		{
			dataLen = m_mavlink.FillBytes(&m_msg, m_buffer);
			m_CDC.SendData(m_buffer, dataLen);
			dataLen = 0;
		}
	}
}

void USBWorker::ReceiveUpdate(uint32_t size)
{
	uint16_t chunkSize = 240;
	MAV_CMD_ACK ack = MAV_CMD_ACK_OK;

	Storage::StorageFlashSPI::Remove("tmp");
	Storage::StorageFlashSPI::Remove("fw");

	spiffs_file fd = Storage::StorageFlashSPI::Open("tmp", SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR, 0);

	if (fd < 0)
		ack = MAV_CMD_ACK_ERR_FAIL;

	while (size > 0 && ack == MAV_CMD_ACK_OK)
	{
		SendAck(ack);

		if (m_channels.pop(m_msg, delay_ms(5000)))
		{
			if (m_msg.msgid == MAVLINK_MSG_ID_ENCAPSULATED_DATA)
			{
				mavlink_msg_encapsulated_data_get_data(&m_msg, m_data_buffer);

				if (size < chunkSize)
					chunkSize = size;

				if (Storage::StorageFlashSPI::Write(fd, m_data_buffer, chunkSize) < 0)
					ack = MAV_CMD_ACK_ERR_FAIL;

				size -= chunkSize;
			}
			else
				ack = MAV_CMD_ACK_ERR_FAIL;
		}
		else
			ack = MAV_CMD_ACK_ERR_FAIL;
	}

	if (ack != MAV_CMD_ACK_OK)
	{
		if (fd >= 0)
			Storage::StorageFlashSPI::Close(fd);

		Storage::StorageFlashSPI::Remove("tmp");

		SendAck(ack);
		return;
	}

	if (Storage::StorageFlashSPI::Close(fd) < 0)
	{
		Storage::StorageFlashSPI::Remove("tmp");
		SendAck(MAV_CMD_ACK_ERR_FAIL);
		return;
	}

	if (Storage::StorageFlashSPI::Rename("tmp", "fw") < 0)
	{
		SendAck(MAV_CMD_ACK_ERR_FAIL);
		return;
	}

	SendAck(MAV_CMD_ACK_OK);

	OSAL::Timer::SleepMS(300);

	Board::InitBootLoader();
}

void USBWorker::SendAck(MAV_CMD_ACK ack)
{
	uint16_t dataLen = m_mavlink.PackCommandAck(&m_msg, ack);
	dataLen = m_mavlink.FillBytes(&m_msg, m_buffer);
	m_CDC.SendData(m_buffer, dataLen);
}

void USBWorker::DeviceConnected()
{
	m_delay_ms = USB_WORKER_DELAY_CONNECTED;
}
void USBWorker::DeviceDisconnected()
{
	m_delay_ms = USB_WORKER_DELAY_DISCONNECTED;
	established = 0;
}

void USBWorker::DataRX(uint8_t* data, uint8_t len)
{
	for (uint8_t i = 0; i < len; i++)
	{

		if (m_mavlink.Decode(data[i], &m_msg_rcv))
		{
			if (m_channels.get_free_size() > 0)
				m_channels.push(m_msg_rcv);
		}
	}
}

}
/* namespace Utils */
