# config to select component, the format is CONFIG_USE_${component}
# Please refer to cmake files below to get available components:
#  ${SdkRootDirPath}/devices/MCXA153/all_lib_device.cmake

set(CONFIG_COMPILER gcc)
set(CONFIG_TOOLCHAIN armgcc)
set(CONFIG_USE_COMPONENT_CONFIGURATION false)
set(CONFIG_USE_driver_clock false)
set(CONFIG_USE_driver_edma_soc false)
set(CONFIG_USE_driver_inputmux_connections false)
set(CONFIG_USE_driver_reset false)
set(CONFIG_USE_CMSIS_Include_core_cm true)
set(CONFIG_USE_CMSIS_Driver_Include_GPIO false)
set(CONFIG_USE_device_CMSIS true)
set(CONFIG_USE_device_system true)
set(CONFIG_USE_device_startup true)
set(CONFIG_USE_device_RTE false)
set(CONFIG_USE_driver_common false)
set(CONFIG_USE_driver_edma4 false)
set(CONFIG_USE_driver_gpio false)
set(CONFIG_USE_driver_inputmux false)
set(CONFIG_USE_driver_lpuart false)
set(CONFIG_USE_driver_mcx_spc false)
set(CONFIG_USE_driver_port false)
set(CONFIG_USE_driver_cmsis_gpio false)
set(CONFIG_USE_utility_assert_lite false)
set(CONFIG_USE_utilities_misc_utilities true)
set(CONFIG_USE_utility_str false)
set(CONFIG_USE_utility_debug_console_lite false)
set(CONFIG_USE_component_lpuart_adapter false)
set(CONFIG_USE_middleware_freertos-kernel false)
set(CONFIG_USE_middleware_freertos-kernel_heap_4 false)
set(CONFIG_USE_middleware_freertos-kernel_cm33_non_trustzone false)
set(CONFIG_USE_middleware_freertos-kernel_config false)
set(CONFIG_CORE cm33)
set(CONFIG_DEVICE MCXA153)
set(CONFIG_BOARD frdmmcxa153)
set(CONFIG_KIT frdmmcxa153)
set(CONFIG_DEVICE_ID MCXA153)
set(CONFIG_FPU NO_FPU)
set(CONFIG_DSP NO_DSP)
set(CONFIG_CORE_ID cm33_core0)
set(CONFIG_TRUSTZONE NO_TZ)
