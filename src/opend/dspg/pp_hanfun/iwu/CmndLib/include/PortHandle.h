#ifndef PORT_HANDLE_H
#define PORT_HANDLE_H

#include "TypeDefs.h"
#include <stddef.h>

extern_c_begin

#ifdef __linux__
	typedef int t_Handle;

#elif _WIN32
	#include <Windows.h>
	typedef HANDLE t_Handle;

#elif defined(__STM32__) || defined(__UVISION_VERSION)
	#include <stddef.h>
	#if defined(STM32F4)
		#include "stm32f4xx.h"
	#elif defined(STM32L4)
		#include "stm32l4xx.h"
        #include "stm32l4xx_hal.h"
	#else
		#error Platform not supported
	#endif
	typedef UART_HandleTypeDef* t_Handle;
#elif ARDUINO
	typedef int t_Handle;

#else
	#error Unknown environment, sorry
#endif

extern_c_end

#endif // PORT_HANDLE_H
