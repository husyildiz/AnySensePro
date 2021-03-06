/*
 * USART.h
 *
 *  Created on: 27.10.2015
 *      Author: cem
 */

#ifndef HAL_INCLUDE_USART_H_
#define HAL_INCLUDE_USART_H_

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>

#include "Pin.h"
#include "DMA.h"

namespace HAL
{

class USART
{
private:
	const uint32_t m_usart;
	const rcc_periph_clken m_rcc_clock;
	const Pin& m_rx;
	const Pin& m_tx;
	const uint8_t m_alt_func_num;
	const DMA& m_rx_dma;
	const DMA& m_tx_dma;
public:
	USART(uint32_t usart, rcc_periph_clken rcc_clock, Pin& rx, Pin& tx, uint8_t alt_func_num, uint8_t nvic_irqn, DMA& rx_dma,
			DMA& tx_dma) :
			m_usart(usart), m_rcc_clock(rcc_clock), m_rx(rx), m_tx(tx), m_alt_func_num(alt_func_num), m_rx_dma(rx_dma), m_tx_dma(
					tx_dma), NVIC_IRQn(nvic_irqn)
	{
	}

	const uint8_t NVIC_IRQn;

	inline void Init(uint8_t pull_up_down) const
	{
		rcc_periph_clock_enable(m_rcc_clock);

		m_rx.PowerUp();
		m_rx.ModeSetup(GPIO_MODE_AF, pull_up_down);
		m_rx.SetOutputOptions(GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ);
		m_rx.Alternate(m_alt_func_num);

		m_tx.PowerUp();
		m_tx.ModeSetup(GPIO_MODE_AF, pull_up_down);
		m_tx.SetOutputOptions(GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ);
		m_tx.Alternate(m_alt_func_num);
	}

	inline void DeInit() const
	{
		USART_CR1(m_usart) = 0x0000;
		USART_CR2(m_usart) = 0x0000;
		USART_CR3(m_usart) = 0x0000;
		USART_BRR(m_usart) = 0x0000;
		USART_GTPR(m_usart) = 0x0000;
		USART_RTOR(m_usart) = 0x0000;
		USART_RQR(m_usart) = 0x0000;
		USART_ICR(m_usart) = 0x0000;
	}

	inline void SetBaudrate(uint32_t baud) const
	{
		usart_set_baudrate(m_usart, baud);
	}
	inline void SetDatabits(uint32_t bits) const
	{
		usart_set_databits(m_usart, bits);
	}
	inline void SetStopbits(uint32_t stopbits) const
	{
		usart_set_stopbits(m_usart, stopbits);
	}
	inline void SetParity(uint32_t parity) const
	{
		usart_set_parity(m_usart, parity);
	}
	inline void SetMode(uint32_t mode) const
	{
		usart_set_mode(m_usart, mode);
	}
	inline void SetFlowControl(uint32_t flowcontrol) const
	{
		usart_set_flow_control(m_usart, flowcontrol);
	}
	inline void Enable() const
	{
		usart_enable(m_usart);
	}
	inline void Disable() const
	{
		usart_disable(m_usart);
	}
	inline void Send(uint16_t data) const
	{
		usart_send(m_usart, data);
	}
	inline uint16_t Receive() const
	{
		return usart_recv(m_usart);
	}
	inline void WaitSendReady() const
	{
		usart_wait_send_ready(m_usart);
	}
	inline void WaitReceiveReady() const
	{
		usart_wait_recv_ready(m_usart);
	}
	inline void SendBlocking(uint16_t data) const
	{
		usart_send_blocking(m_usart, data);
	}
	inline uint16_t ReceiveBlocking() const
	{
		return usart_recv_blocking(m_usart);
	}
	inline void EnableRxDma() const
	{
		usart_enable_rx_dma(m_usart);
	}
	inline void DisableRxDma() const
	{
		usart_disable_rx_dma(m_usart);
	}
	inline void EnableTxDma() const
	{
		usart_enable_tx_dma(m_usart);
	}
	inline void DisableTxDma() const
	{
		usart_disable_tx_dma(m_usart);
	}
	inline void EnableRxInterrupt() const
	{
		usart_enable_rx_interrupt(m_usart);
	}
	inline void DisableRxInterrupt() const
	{
		usart_disable_rx_interrupt(m_usart);
	}
	inline void EnableTxInterrupt() const
	{
		usart_enable_tx_interrupt(m_usart);
	}
	inline void DisableTxInterrupt() const
	{
		usart_disable_tx_interrupt(m_usart);
	}
	inline void EnableTcInterrupt() const
	{
		USART_CR1(m_usart) |= USART_CR1_TCIE;
	}
	inline void DisableTcInterrupt() const
	{
		USART_CR1(m_usart) &= ~USART_CR1_TCIE;
	}
	inline void EnableErrorInterrupt() const
	{
		usart_enable_error_interrupt(m_usart);
	}
	inline void DisableErrorInterrupt() const
	{
		usart_disable_error_interrupt(m_usart);
	}
	inline void EnableTXInversion() const
	{
		usart_enable_tx_inversion(m_usart);
	}
	inline void DisableTXInversion() const
	{
		usart_disable_tx_inversion(m_usart);
	}
	inline void EnableRXInversion() const
	{
		usart_enable_rx_inversion(m_usart);
	}
	inline void DisableRXInversion() const
	{
		usart_disable_rx_inversion(m_usart);
	}
	inline void EnableHalfduplex() const
	{
		usart_enable_halfduplex(m_usart);
	}
	inline void DisableHalfduplex() const
	{
		usart_disable_halfduplex(m_usart);
	}
	inline void DisableOverrunDetection() const
	{
		USART_CR3(m_usart) |= USART_CR3_OVRDIS;
	}
	inline void SetReceiveTimeout(uint32_t value) const
	{
		usart_set_rx_timeout_value(m_usart, value);
	}
	inline void EnableRxTimeout() const
	{
		usart_enable_rx_timeout(m_usart);
	}
	inline void DisableRxTimeout() const
	{
		usart_disable_rx_timeout(m_usart);
	}
	inline void EnableRxTimeoutInterrupt() const
	{
		usart_enable_rx_timeout_interrupt(m_usart);
	}
	inline void DisableRxTimeoutInterrupt() const
	{
		usart_disable_rx_timeout_interrupt(m_usart);
	}
	inline bool GetFlag(uint32_t flag) const
	{
		return usart_get_flag(m_usart, flag);
	}
	inline bool GetInterruptSource(uint32_t flag) const
	{
		if (flag == USART_ISR_RTOF)
			return GetFlag(flag) && (USART_CR1(m_usart) & USART_CR1_RTOIE);

		return usart_get_interrupt_source(m_usart, flag);
	}

	inline void ClearInterruptFlag(uint32_t flag) const
	{
		USART_ICR(m_usart) |= flag;
	}

	inline const DMA& GetRXDMA()
	{
		return m_rx_dma;
	}

	inline const DMA& GetTXDMA()
	{
		return m_tx_dma;
	}
	inline const Pin& GetTXPin()
	{
		return m_tx;
	}

	inline void SendDma(const uint8_t* address, uint16_t size)
	{
		m_tx_dma.DisableChannel();

		m_tx_dma.SetMemoryAddress((uint32_t) address);
		m_tx_dma.SetNumerOfData(size);

		m_tx_dma.EnableChannel();
	}

	inline void ReadDma(const uint8_t* address, uint16_t size)
	{
		m_rx_dma.DisableChannel();

		m_rx_dma.SetMemoryAddress((uint32_t) address);
		m_rx_dma.SetNumerOfData(size);

		m_rx_dma.EnableChannel();
	}

	inline void SetupTXDMA()
	{
		m_tx_dma.EnableClock();
		m_tx_dma.ChannelReset();
		m_tx_dma.SetPeriperalSize(DMA_CCR_PSIZE_8BIT);
		m_tx_dma.SetMemorySize(DMA_CCR_MSIZE_8BIT);
		m_tx_dma.EnableMemoryIncrementMode();
		m_tx_dma.DisablePeripheralIncremtnMode();
		m_tx_dma.SetReadFromMemory();
		m_tx_dma.SetPeripheralAddress((uint32_t) &USART_TDR(m_usart));

		m_tx_dma.EnableTransferCompleteInterrupt();

		EnableTxDma();
	}

	inline void SetupRXDMA()
	{
		m_rx_dma.EnableClock();
		m_rx_dma.ChannelReset();
		m_rx_dma.SetPeriperalSize(DMA_CCR_PSIZE_8BIT);
		m_rx_dma.SetMemorySize(DMA_CCR_MSIZE_8BIT);
		m_rx_dma.EnableMemoryIncrementMode();
		m_rx_dma.DisablePeripheralIncremtnMode();
		m_rx_dma.SetReadFromPeripheral();
		m_rx_dma.SetPeripheralAddress((uint32_t) &USART_RDR(m_usart));

		EnableRxDma();
	}

	inline void EnableRxDMAChannel()
	{
		m_rx_dma.EnableChannel();
	}

	inline void DisableRxDMAChannel()
	{
		m_rx_dma.DisableChannel();
	}

	inline void ClearTXDma()
	{
		m_tx_dma.ChannelReset();
	}

	inline void ClearRXDma()
	{
		m_rx_dma.ChannelReset();
	}
};

} /* namespace HAL */

#endif /* HAL_INCLUDE_USART_H_ */
