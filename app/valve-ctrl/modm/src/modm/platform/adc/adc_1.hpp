/*
 * Copyright (c) 2013-2014, Kevin Läufer
 * Copyright (c) 2013-2018, Niklas Hauser
 * Copyright (c) 2016, Fabian Greif
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_STM32_ADC1_HPP
#define MODM_STM32_ADC1_HPP

#include <stdint.h>
#include "../device.hpp"
#include <modm/architecture/interface/adc.hpp>
#include <modm/architecture/interface/register.hpp>
#include <modm/platform/gpio/connector.hpp>

namespace modm
{

namespace platform
{

/**
 * Analog/Digital-Converter module (ADC1).
 *
 * The 12-bit ADC is a successive approximation analog-to-digital
 * converter. It has up to 18 multiplexed channels allowing it to measure
 * signals from 16 external and two internal sources.
 * The result of the ADC is stored in a left-aligned or right-aligned
 * 16-bit data register.
 *
 * @author	Stephan Kugelmann
 * @author	David Hebbeker
 * @author	ekiwi
 * @ingroup	modm_platform_adc modm_platform_adc_1
 */
class Adc1 : public modm::Adc
{
public:
	static constexpr uint8_t Resolution = 12;

public:
	/**
	 * Channels, which can be used with this ADC.
	 *
	 * You can specify the channel by using a pin-name, like PIN_C0, an
	 * internal sensor, like TEMPERATURE_SENSOR or just the plain
	 * channel number, like CHANNEL_0.
	 */
	enum class
	Channel : uint8_t
	{
		Channel0 = 0,
		Channel1 = 1,
		Channel2 = 2,
		Channel3 = 3,
		Channel4 = 4,
		Channel5 = 5,
		Channel6 = 6,
		Channel7 = 7,
		Channel8 = 8,
		Channel9 = 9,
		Channel10 = 10,
		Channel11 = 11,
		Channel12 = 12,
		Channel13 = 13,
		Channel14 = 14,
		Channel15 = 15,
		Channel16 = 16,
		Channel17 = 17,
		Channel18 = 18,

		BatDiv2 = 18, ///< Half the V_BAT voltage.

		/** Measure the ambient temperature of the device.
		 *
		 * @li Supported temperature range: -40 to 125 C
		 * @li Precision: +-1.5 C
		 *
		 * @see Reference manual (i.e. RM0090) for the formula for the
		 * 	calculation of the actual temperature.
		 * @note The TSVREFE bit must be set to enable the conversion of
		 * 	this internal channel.
		 */
		TemperatureSensor = 16,

		/** Internal reference voltage.
		 *
		 * @note The TSVREFE bit must be set to enable the conversion of
		 * 	this internal channel.
		 */
		InternalReference = 17,
	};

private:
	/**
	 * Programmable prescaler to divide the APB2 clock frequency, which
	 * is used for the analog circuitry (not the digital interface which
	 * is used for registers).
	 */
	enum class
	Prescaler : uint8_t
	{
		Div2 = 0b00,	///< PCLK2 divided by 2
		Div4 = 0b01,	///< PCLK2 divided by 4
		Div6 = 0b10,	///< PCLK2 divided by 6
		Div8 = 0b11		///< PCLK2 divided by 8
	};

public:
	/**
	 * Sampling time of the input voltage.
	 *
	 * Total conversion time is T_con = Sampling time + 12 cycles
	 */
	enum class
	SampleTime : uint8_t
	{
		Cycles3 	= 0b000,	///< 3 ADCCLK cycles
		Cycles15 	= 0b001,	///< 15 ADCCLK cycles
		Cycles28 	= 0b010,	///< 28 ADCCLK cycles
		Cycles56 	= 0b011,	///< 56 ADCCLK cycles
		Cycles84 	= 0b100,	///< 84 ADCCLK cycles
		Cycles112 	= 0b101,	///< 112 ADCCLK cycles
		Cycles144 	= 0b110,	///< 144 ADCCLK cycles
		Cycles480 	= 0b111		///< 480 ADCCLK cycles
	};

	enum class ExternalTriggerPolarity
	{
		NoTriggerDetection = 0x0u,
		RisingEdge = 0x1u,
		FallingEdge = 0x2u,
		RisingAndFallingEdge = 0x3u,
	};
	/**
	 * Enum mapping all events on a external trigger converter.
	 * The source mapped to each event varies on controller family,
	 * refer to the ADC external trigger section on reference manual
	 * of your controller for more information
	 */
	enum class RegularConversionExternalTrigger
	{
		Event0 = 0x0u,
		Event1 = 0x1u,
		Event2 = 0x2u,
		Event3 = 0x3u,
		Event4 = 0x4u,
		Event5 = 0x5u,
		Event6 = 0x6u,
		Event7 = 0x7u,
		Event8 = 0x8u,
		Event9 = 0x9u,
		Event10 = 0xAu,
		Event11 = 0xBu,
		Event12 = 0xCu,
		Event13 = 0xDu,
		Event14 = 0xEu,
		Event15 = 0xFu,
	};

	/**
	 * Possible interrupts.
	 *
	 * An interrupt can be produced on the end of conversion for regular
	 * and injected groups, when the analog watchdog status bit is set
	 * and when the overrun status bit is set.
	 */
	enum class
	Interrupt : uint32_t
	{
		/// Analog watchdog status bit is set
		AnalogWatchdog			= ADC_CR1_AWDIE,
		/// End of conversion of a regular group
		EndOfRegularConversion	= ADC_CR1_EOCIE,
		/// End of conversion of an injected group
		EndOfInjectedConversion	= ADC_CR1_JEOCIE,
		/// Overrun (if data are lost)
		Overrun					= ADC_CR1_OVRIE,
	};
	MODM_FLAGS32(Interrupt);

	enum class
	InterruptFlag : uint32_t
	{
		/// Analog watchdog status bit is set
		AnalogWatchdog			= ADC_SR_AWD,
		/// End of conversion of a regular group
		EndOfRegularConversion	= ADC_SR_EOC,
		/// End of conversion of an injected group
		EndOfInjectedConversion	= ADC_SR_JEOC,
		/// Overrun (if data are lost)
		Overrun					= ADC_SR_OVR,
		/// All InterruptFlags
		All				= ADC_SR_AWD | ADC_SR_EOC | ADC_SR_JEOC | ADC_SR_OVR,
	};
	MODM_FLAGS32(InterruptFlag);

public:
	// start inherited documentation
	template< class... Signals >
	static void
	connect()
	{
		using Connector = GpioConnector<Peripheral::Adc1, Signals...>;
		Connector::connect();
	}

	/**
	 * Initialize and enable the A/D converter.
	 *
	 * Enables the ADC clock and switches on the ADC. The ADC clock
	 * prescaler will be set as well.
	 *
	 * The ADC clock must not exceed 36 MHz for Vdd > 2.4V, or 18MHz for Vdd < 2.4V.
	 */
	template< class SystemClock, frequency_t frequency=MHz(10), percent_t tolerance=pct(10) >
	static void
	initialize();

	static inline void
	enable();

	static inline void
	disable();

	/**
	 * Start a new conversion or continuous conversions.
	 *
	 * @pre A ADC channel must be selected with setChannel(). When using
	 * 	a STM32F10x a delay of at least t_STAB after initialize() must
	 * 	be waited!
	 *
	 * @post The result can be fetched with getValue()
	 * @attention When using a STM32F10x, the application should allow a delay of t_STAB between
	 * 	power up and start of conversion. Refer to Reference Manual
	 * 	(RM0008) ADC_CR2_ADON.
	 */
	static inline void
	startConversion();

	static inline bool
	isConversionFinished();

	static inline uint16_t
	getValue();

	static inline uint16_t
	readChannel(Channel channel);


	/**
	 * Analog channel selection.
	 *
	 * This not for scan mode. The number of channels will be set to 1,
	 * the channel selected and the corresponding pin will be set to
	 * analog input.
	 * If the the channel is modified during a conversion, the current
	 * conversion is reset and a new start pulse is sent to the ADC to
	 * convert the new chosen channnel / group of channels.
	 *
	 *
	 * @param channel		The channel which shall be read.
	 * @param sampleTime	The sample time to sample the input voltage.
	 *
	 * @pre The ADC clock must be started and the ADC switched on with
	 * 		initialize()
	 */
	static inline bool
	setChannel(const Channel channel,
			   const SampleTime sampleTime = static_cast<SampleTime>(0b000));

	/// Setting the channel for a Pin
	template< class Gpio >
	static inline bool
	setPinChannel(SampleTime sampleTime = static_cast<SampleTime>(0b000))
	{
		return setChannel(getPinChannel<Gpio>(), sampleTime);
	}
	/// Get the channel for a Pin
	template< class Gpio >
	static inline constexpr Channel
	getPinChannel()
	{
		constexpr int8_t channel{detail::AdcChannel<typename Gpio::Data, Peripheral::Adc1>};
		static_assert(channel >= 0, "Adc1 does not have a channel for this pin!");
		return Channel(channel);
	}

	static inline Channel
	getChannel();

	static inline void
	enableFreeRunningMode();

	static inline void
	disableFreeRunningMode();

	static inline void
	setLeftAdjustResult();

	static inline void
	setRightAdjustResult();

	/// Switch on temperature- and V_REF measurement.
	static inline void
	enableTemperatureRefVMeasurement();

	/// Switch on temperature- and V_REF measurement.
	static inline void
	disableTemperatureRefVMeasurement();
	/// Add a channel to conversion group.
	static inline bool
	addChannel(const Channel channel,
		const SampleTime sampleTime = static_cast<SampleTime>(0b000));

	/// change sample time of ADC channel
	static inline void
	setSampleTime(const Channel channel,
		const SampleTime sampleTime = static_cast<SampleTime>(0b000));


	static inline void
	enableInterruptVector(const uint32_t priority, const bool enable = true);

	/**
	 * Enables the ADC Conversion Complete Interrupt.
	 *
	 * You could catch the interrupt using this example function:
	 * @li for STM32F4XX: `MODM_ISR(ADC)`
	 * @li for STM32F10X: `MODM_ISR(ADC1_2)`
	 *
	 * @pre The ADC clock must be started and the ADC switched on with
	 * 	initialize(). Also the Interrupt Vector needs to be enabled first.
	 *
	 * @param priority Priority to set
	 * @param interrupt The interrupt, which shall be enabled. See
	 * 	Interrupt for available interrupts.
	 *
	 * @note ADC1 and ADC2 interrupts are mapped onto the same interrupt
	 * 	vector. ADC3 interrupts are mapped onto a separate interrupt
	 * 	vector.
	 */
	static inline void
	enableInterrupt(const Interrupt_t interrupt);

	/**
	 * Disables the ADC Conversion Complete Interrupt.
	 */
	static inline void
	disableInterrupt(const Interrupt_t interrupt);


	/**
	 * Returns if the th interrupt flags set.
	 *
	 * @pre The ADC clock must be started and the ADC switched on with
	 * 	initialize()
	 */
	static inline InterruptFlag_t
	getInterruptFlags();

	/**
	 * Clears the specified interrupt flag.
	 *
	 * @param flag
	 * 		The interrupt flag, which shall be cleared.
	 *
	 * @pre The ADC clock must be started and the ADC switched on with
	 * 		initialize().
	 */
	static inline void
	acknowledgeInterruptFlags(const InterruptFlag_t flags);

	static inline uintptr_t
	getDataRegisterAddress();

	static inline void
	enableRegularConversionExternalTrigger(
		ExternalTriggerPolarity externalTriggerPolarity,
		RegularConversionExternalTrigger regularConversionExternalTrigger);

	/**
	 * Enable Dma mode for the ADC
	 */
	static inline void
	enableDmaMode();

	/**
	 * Disable Dma mode for the ADC
	 */
	static inline void
	disableDmaMode();

	/**
	 * get if adc is enabled
	 * @return true if ADC_CR2_ADON bit is set, false otherwise
	 */
	static inline bool
	getAdcEnabled();

	/**
	* enable DMA selection for the ADC. If this is enabled DMA
	* requests are issued as long as data are converted and
	* the adc has dma enabled
	 */
	static inline void
	enableDmaRequests();

	/**
	* disable dma selection for the ADC. If this is disabled
	* no new DMA requests are issued after last transfer
	 */
	static inline void
	disableDmaRequests();

	/**
	 * Enables scan mode
	 */
	static inline void
	enableScanMode();

	/**
	 * Disables scan mode
	 */
	static inline void
	disableScanMode();

private:
	/**
	 * Select the frequency of the clock to the ADC. The clock is common
	 * for all the ADCs (ADC1, ADC2, ADC3) and all channels.
	 *
	 * @pre The ADC clock must be started and the ADC switched on with
	 * 	initialize()
	 *
	 * @param prescaler
	 * 		The prescaler specifies by which factor the system clock
	 * 		will be divided.
	 */
	static inline void
	setPrescaler(const Prescaler prescaler = Prescaler::Div8);
};

} // namespace platform

} // namespace modm

#include "adc_1_impl.hpp"

#endif	// MODM_STM32_ADC1_HPP