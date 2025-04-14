# CROSS COMPILER SETTING
set(CMAKE_SYSTEM_NAME Generic)
cmake_minimum_required(VERSION 3.10.0)

# THE VERSION NUMBER
SET (MCUXPRESSO_CMAKE_FORMAT_MAJOR_VERSION 2)
SET (MCUXPRESSO_CMAKE_FORMAT_MINOR_VERSION 0)

include(ide_overrides.cmake OPTIONAL)

if(CMAKE_SCRIPT_MODE_FILE)
  message("${MCUXPRESSO_CMAKE_FORMAT_MAJOR_VERSION}")
  return()
endif()


set(CMAKE_STATIC_LIBRARY_PREFIX)
set(CMAKE_STATIC_LIBRARY_SUFFIX)

set(CMAKE_EXECUTABLE_LIBRARY_PREFIX)
set(CMAKE_EXECUTABLE_LIBRARY_SUFFIX)

# CURRENT DIRECTORY
set(ProjDirPath ${CMAKE_CURRENT_SOURCE_DIR})

set(EXECUTABLE_OUTPUT_PATH ${ProjDirPath}/${CMAKE_BUILD_TYPE})
set(LIBRARY_OUTPUT_PATH ${ProjDirPath}/${CMAKE_BUILD_TYPE})


project(ese_shieldv3_examples_projects_waffle_bake_timer)

enable_language(ASM)

set(MCUX_BUILD_TYPES debug release)

set(MCUX_SDK_PROJECT_NAME ese_shieldv3_examples_projects_waffle_bake_timer.elf)

if (NOT DEFINED SdkRootDirPath)
    SET(SdkRootDirPath ${ProjDirPath}/../../../../../..)
endif()

include(${ProjDirPath}/flags.cmake)

include(${ProjDirPath}/config.cmake)

add_executable(${MCUX_SDK_PROJECT_NAME} 
"${ProjDirPath}/../bitmaps.c"
"${ProjDirPath}/../bitmaps.h"
"${ProjDirPath}/../eeprom.c"
"${ProjDirPath}/../eeprom.h"
"${ProjDirPath}/../encoder.c"
"${ProjDirPath}/../encoder.h"
"${ProjDirPath}/../fifo.c"
"${ProjDirPath}/../fifo.h"
"${ProjDirPath}/../fonts.c"
"${ProjDirPath}/../fonts.h"
"${ProjDirPath}/../gpio_input_interrupt.c"
"${ProjDirPath}/../gpio_input_interrupt.h"
"${ProjDirPath}/../gpio_output.c"
"${ProjDirPath}/../gpio_output.h"
"${ProjDirPath}/../hc595.c"
"${ProjDirPath}/../hc595.h"
"${ProjDirPath}/../lm35d_interrupt.c"
"${ProjDirPath}/../lm35d_interrupt.h"
"${ProjDirPath}/../lpi2c0_controller_dma.c"
"${ProjDirPath}/../lpi2c0_controller_dma.h"
"${ProjDirPath}/../lpspi1_master.c"
"${ProjDirPath}/../lpspi1_master.h"
"${ProjDirPath}/../lpuart0_interrupt.c"
"${ProjDirPath}/../lpuart0_interrupt.h"
"${ProjDirPath}/../main.c"
"${ProjDirPath}/../mcux_config.h"
"${ProjDirPath}/../neopixel_flexpwm.c"
"${ProjDirPath}/../neopixel_flexpwm.h"
"${ProjDirPath}/../retarget.c"
"${ProjDirPath}/../ssd1306.c"
"${ProjDirPath}/../ssd1306.h"
"${ProjDirPath}/../fsm/fsm.c"
"${ProjDirPath}/../fsm/fsm.h"
)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE
"${ProjDirPath}/../"
"${ProjDirPath}/../fsm"
)

include(${SdkRootDirPath}/devices/MCXA153/all_lib_device.cmake)

IF(NOT DEFINED TARGET_LINK_SYSTEM_LIBRARIES)  
    SET(TARGET_LINK_SYSTEM_LIBRARIES "-lm -lc -lgcc -lnosys")  
ENDIF()  

TARGET_LINK_LIBRARIES(${MCUX_SDK_PROJECT_NAME} PRIVATE -Wl,--start-group)

target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE ${TARGET_LINK_SYSTEM_LIBRARIES})

TARGET_LINK_LIBRARIES(${MCUX_SDK_PROJECT_NAME} PRIVATE -Wl,--end-group)

ADD_CUSTOM_COMMAND(TARGET ${MCUX_SDK_PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_OBJCOPY}
-Obinary ${EXECUTABLE_OUTPUT_PATH}/${MCUX_SDK_PROJECT_NAME} ${EXECUTABLE_OUTPUT_PATH}/ese_shieldv3_examples_projects_waffle_bake_timer.bin)

set_target_properties(${MCUX_SDK_PROJECT_NAME} PROPERTIES ADDITIONAL_CLEAN_FILES "output.map;${EXECUTABLE_OUTPUT_PATH}/ese_shieldv3_examples_projects_waffle_bake_timer.bin")

# wrap all libraries with -Wl,--start-group -Wl,--end-group to prevent link order issue
group_link_libraries()

