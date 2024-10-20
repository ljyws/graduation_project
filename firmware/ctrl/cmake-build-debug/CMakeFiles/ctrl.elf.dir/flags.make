# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.29

# compile ASM with D:/Tools/arm-none-eabi-13/13.3 rel1/bin/arm-none-eabi-gcc.exe
# compile C with D:/Tools/arm-none-eabi-13/13.3 rel1/bin/arm-none-eabi-gcc.exe
# compile CXX with D:/Tools/arm-none-eabi-13/13.3 rel1/bin/arm-none-eabi-g++.exe
ASM_DEFINES = -DARM_MATH_CM4 -DARM_MATH_MATRIX_CHECK -DARM_MATH_ROUNDING -DDEBUG -DFPU_FPV4 -DSTM32F405xx -DUSE_HAL_DRIVER

ASM_INCLUDES = -IG:\ljyws\graduation_project\firmware\ctrl\Core\Inc -IG:\ljyws\graduation_project\firmware\ctrl\Drivers\STM32F4xx_HAL_Driver\Inc -IG:\ljyws\graduation_project\firmware\ctrl\Drivers\STM32F4xx_HAL_Driver\Inc\Legacy -IG:\ljyws\graduation_project\firmware\ctrl\Drivers\CMSIS\Device\ST\STM32F4xx\Include -IG:\ljyws\graduation_project\firmware\ctrl\Drivers\CMSIS\Include -IG:\ljyws\graduation_project\firmware\ctrl\Middlewares\Third_Party\FreeRTOS\Source\include -IG:\ljyws\graduation_project\firmware\ctrl\Middlewares\Third_Party\FreeRTOS\Source\portable\GCC\ARM_CM4F -IG:\ljyws\graduation_project\firmware\ctrl\Middlewares\Third_Party\FreeRTOS\Source\CMSIS_RTOS_V2 -IG:\ljyws\graduation_project\firmware\ctrl\Middlewares\ST\ARM\DSP\Inc -IG:\ljyws\graduation_project\firmware\ctrl\USB_DEVICE\App -IG:\ljyws\graduation_project\firmware\ctrl\USB_DEVICE\Target -IG:\ljyws\graduation_project\firmware\ctrl\Middlewares\ST\STM32_USB_Device_Library\Core\Inc -IG:\ljyws\graduation_project\firmware\ctrl\Middlewares\ST\STM32_USB_Device_Library\Class\CDC\Inc -IG:\ljyws\graduation_project\firmware\ctrl\Application -IG:\ljyws\graduation_project\firmware\ctrl\Ctrl -IG:\ljyws\graduation_project\firmware\ctrl\Stm32_Interface -IG:\ljyws\graduation_project\firmware\ctrl\Communication -IG:\ljyws\graduation_project\firmware\ctrl\Tools

ASM_FLAGS = -g -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mcpu=cortex-m4 -mthumb -mthumb-interwork -ffunction-sections -fdata-sections -fno-common -fmessage-length=0 -x assembler-with-cpp -Og -g

C_DEFINES = -DARM_MATH_CM4 -DARM_MATH_MATRIX_CHECK -DARM_MATH_ROUNDING -DDEBUG -DFPU_FPV4 -DSTM32F405xx -DUSE_HAL_DRIVER

C_INCLUDES = -IG:\ljyws\graduation_project\firmware\ctrl\Core\Inc -IG:\ljyws\graduation_project\firmware\ctrl\Drivers\STM32F4xx_HAL_Driver\Inc -IG:\ljyws\graduation_project\firmware\ctrl\Drivers\STM32F4xx_HAL_Driver\Inc\Legacy -IG:\ljyws\graduation_project\firmware\ctrl\Drivers\CMSIS\Device\ST\STM32F4xx\Include -IG:\ljyws\graduation_project\firmware\ctrl\Drivers\CMSIS\Include -IG:\ljyws\graduation_project\firmware\ctrl\Middlewares\Third_Party\FreeRTOS\Source\include -IG:\ljyws\graduation_project\firmware\ctrl\Middlewares\Third_Party\FreeRTOS\Source\portable\GCC\ARM_CM4F -IG:\ljyws\graduation_project\firmware\ctrl\Middlewares\Third_Party\FreeRTOS\Source\CMSIS_RTOS_V2 -IG:\ljyws\graduation_project\firmware\ctrl\Middlewares\ST\ARM\DSP\Inc -IG:\ljyws\graduation_project\firmware\ctrl\USB_DEVICE\App -IG:\ljyws\graduation_project\firmware\ctrl\USB_DEVICE\Target -IG:\ljyws\graduation_project\firmware\ctrl\Middlewares\ST\STM32_USB_Device_Library\Core\Inc -IG:\ljyws\graduation_project\firmware\ctrl\Middlewares\ST\STM32_USB_Device_Library\Class\CDC\Inc -IG:\ljyws\graduation_project\firmware\ctrl\Application -IG:\ljyws\graduation_project\firmware\ctrl\Ctrl -IG:\ljyws\graduation_project\firmware\ctrl\Stm32_Interface -IG:\ljyws\graduation_project\firmware\ctrl\Communication -IG:\ljyws\graduation_project\firmware\ctrl\Tools

C_FLAGS = -g -std=gnu11 -fdiagnostics-color=always -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mcpu=cortex-m4 -mthumb -mthumb-interwork -ffunction-sections -fdata-sections -fno-common -fmessage-length=0 -Og -g

CXX_DEFINES = -DARM_MATH_CM4 -DARM_MATH_MATRIX_CHECK -DARM_MATH_ROUNDING -DDEBUG -DFPU_FPV4 -DSTM32F405xx -DUSE_HAL_DRIVER

CXX_INCLUDES = -IG:\ljyws\graduation_project\firmware\ctrl\Core\Inc -IG:\ljyws\graduation_project\firmware\ctrl\Drivers\STM32F4xx_HAL_Driver\Inc -IG:\ljyws\graduation_project\firmware\ctrl\Drivers\STM32F4xx_HAL_Driver\Inc\Legacy -IG:\ljyws\graduation_project\firmware\ctrl\Drivers\CMSIS\Device\ST\STM32F4xx\Include -IG:\ljyws\graduation_project\firmware\ctrl\Drivers\CMSIS\Include -IG:\ljyws\graduation_project\firmware\ctrl\Middlewares\Third_Party\FreeRTOS\Source\include -IG:\ljyws\graduation_project\firmware\ctrl\Middlewares\Third_Party\FreeRTOS\Source\portable\GCC\ARM_CM4F -IG:\ljyws\graduation_project\firmware\ctrl\Middlewares\Third_Party\FreeRTOS\Source\CMSIS_RTOS_V2 -IG:\ljyws\graduation_project\firmware\ctrl\Middlewares\ST\ARM\DSP\Inc -IG:\ljyws\graduation_project\firmware\ctrl\USB_DEVICE\App -IG:\ljyws\graduation_project\firmware\ctrl\USB_DEVICE\Target -IG:\ljyws\graduation_project\firmware\ctrl\Middlewares\ST\STM32_USB_Device_Library\Core\Inc -IG:\ljyws\graduation_project\firmware\ctrl\Middlewares\ST\STM32_USB_Device_Library\Class\CDC\Inc -IG:\ljyws\graduation_project\firmware\ctrl\Application -IG:\ljyws\graduation_project\firmware\ctrl\Ctrl -IG:\ljyws\graduation_project\firmware\ctrl\Stm32_Interface -IG:\ljyws\graduation_project\firmware\ctrl\Communication -IG:\ljyws\graduation_project\firmware\ctrl\Tools

CXX_FLAGS = -g -fdiagnostics-color=always -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mcpu=cortex-m4 -mthumb -mthumb-interwork -ffunction-sections -fdata-sections -fno-common -fmessage-length=0 -Og -g

