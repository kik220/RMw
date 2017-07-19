LIST(FIND ChibiOS_FIND_COMPONENTS nil ChibiOS_FIND_COMPONENTS_nil)
LIST(FIND ChibiOS_FIND_COMPONENTS rt ChibiOS_FIND_COMPONENTS_rt)

IF((${ChibiOS_FIND_COMPONENTS_nil} LESS 0) AND (${ChibiOS_FIND_COMPONENTS_rt} LESS 0))
  MESSAGE(STATUS "No kernel component selected, using Nil kernel")
  LIST(APPEND ChibiOS_FIND_COMPONENTS nil)
  SET(CHIBIOS_KERNEL nil)
ELSE()
  IF((NOT (${ChibiOS_FIND_COMPONENTS_nil} LESS 0)) AND (NOT (${ChibiOS_FIND_COMPONENTS_rt} LESS 0)))
    MESSAGE(FATAL_ERROR "Cannot use RT and Nil kernel at the same time")
  ENDIF()
  IF(NOT (${ChibiOS_FIND_COMPONENTS_nil} LESS 0))
    SET(CHIBIOS_KERNEL nil)
  ELSE()
    SET(CHIBIOS_KERNEL rt)
  ENDIF()
ENDIF()

SET(CHIBIOS_kernel_SEARCH_PATH 
  ${CHIBIOS_ROOT}/os/license
  ${CHIBIOS_ROOT}/os/common/ports/ARMCMx
  ${CHIBIOS_ROOT}/os/common/ports/ARMCMx/compilers/GCC
  ${CHIBIOS_ROOT}/os/common/startup/ARMCMx/compilers/GCC
  ${CHIBIOS_ROOT}/os/common/oslib/src
  ${CHIBIOS_ROOT}/os/common/oslib/include
  ${CHIBIOS_ROOT}/os/common/ext/CMSIS/include
  ${CHIBIOS_ROOT}/os/common/ext/CMSIS/include
)

SET(CHIBIOS_kernel_SEARCH_HEADERS 
  ch.h
  chcore.h
  chlicense.h
  chtypes.h
  cmparams.h
  chbsem.h
  chheap.h
  chmboxes.h
  chmemcore.h
  chmempools.h
)

SET(CHIBIOS_kernel_SOURCES
  chcore.c
  crt1.c
  vectors.c
  chheap.c
  chmboxes.c
  chmemcore.c
  chmempools.c
)

IF(STM32_FAMILY STREQUAL "F0")
  SET(CHIBIOS_kernel_SEARCH_PATH 
    ${CHIBIOS_kernel_SEARCH_PATH} 
    ${CHIBIOS_ROOT}/os/common/startup/ARMCMx/devices/STM32F0xx
    ${CHIBIOS_ROOT}/os/common/ext/CMSIS/ST/STM32F0xx
  )
  SET(CHIBIOS_kernel_SEARCH_HEADERS
    ${CHIBIOS_kernel_SEARCH_HEADERS}
    core_cm0.h
    stm32f0xx.h
  )
  SET(CHIBIOS_kernel_SOURCES  
    ${CHIBIOS_kernel_SOURCES} 
    crt0_v6m.S 
    chcore_v6m.c
    chcoreasm_v6m.S
  )
ELSEIF(STM32_FAMILY STREQUAL "F1")
  SET(CHIBIOS_kernel_SEARCH_PATH 
    ${CHIBIOS_kernel_SEARCH_PATH} 
    ${CHIBIOS_ROOT}/os/common/startup/ARMCMx/devices/STM32F1xx
    ${CHIBIOS_ROOT}/os/common/ext/CMSIS/ST/STM32F1xx
  )
  SET(CHIBIOS_kernel_SEARCH_HEADERS
    ${CHIBIOS_kernel_SEARCH_HEADERS}
    core_cm3.h
    stm32f1xx.h
  )
  SET(CHIBIOS_kernel_SOURCES  
    ${CHIBIOS_kernel_SOURCES} 
    crt0_v7m.S 
    chcore_v7m.c
    chcoreasm_v7m.S
  )
ELSEIF(STM32_FAMILY STREQUAL "F4")
  SET(CHIBIOS_kernel_SEARCH_PATH 
    ${CHIBIOS_kernel_SEARCH_PATH} 
    ${CHIBIOS_ROOT}/os/common/startup/ARMCMx/devices/STM32F4xx
    ${CHIBIOS_ROOT}/os/common/ext/CMSIS/ST/STM32F4xx
  )
  SET(CHIBIOS_kernel_SEARCH_HEADERS
    ${CHIBIOS_kernel_SEARCH_HEADERS}
    core_cm4.h
    stm32f4xx.h
  )
  SET(CHIBIOS_kernel_SOURCES  
    ${CHIBIOS_kernel_SOURCES} 
    crt0_v7m.S 
    chcore_v7m.c
    chcoreasm_v7m.S
  )
ELSEIF(STM32_FAMILY STREQUAL "L0")
  SET(CHIBIOS_kernel_SEARCH_PATH 
    ${CHIBIOS_kernel_SEARCH_PATH} 
    ${CHIBIOS_ROOT}/os/common/startup/ARMCMx/devices/STM32L0xx
    ${CHIBIOS_ROOT}/os/common/ext/CMSIS/ST/STM32L0xx
  )
  SET(CHIBIOS_kernel_SEARCH_HEADERS
    ${CHIBIOS_kernel_SEARCH_HEADERS}
    core_cm0.h
    stm32l0xx.h
  )
  SET(CHIBIOS_kernel_SOURCES  
    ${CHIBIOS_kernel_SOURCES} 
    crt0_v6m.S 
    chcore_v6m.c
    chcoreasm_v6m.S
  )
ENDIF()

SET(CHIBIOS_nil_SEARCH_PATH 
  ${CHIBIOS_kernel_SEARCH_PATH}
  ${CHIBIOS_ROOT}/os/nil/src
  ${CHIBIOS_ROOT}/os/nil/include
)

SET(CHIBIOS_rt_SEARCH_PATH 
  ${CHIBIOS_kernel_SEARCH_PATH}
  ${CHIBIOS_ROOT}/os/rt/src
  ${CHIBIOS_ROOT}/os/rt/include
)

SET(CHIBIOS_nil_SEARCH_HEADERS
  ${CHIBIOS_kernel_SEARCH_HEADERS}
)

SET(CHIBIOS_rt_SEARCH_HEADERS
  ${CHIBIOS_kernel_SEARCH_HEADERS}
  ch.h
  chalign.h
  chchecks.h
  chcond.h
  chdebug.h
  chdynamic.h
  chevents.h
  chmsg.h
  chmtx.h
  chregistry.h
  chschd.h
  chsem.h
  chstats.h
  chsys.h
  chsystypes.h
  chthreads.h
  chtm.h
  chtrace.h
  chvt.h
)

SET(CHIBIOS_nil_SOURCES  
  ${CHIBIOS_kernel_SOURCES}
  ch.c
)

SET(CHIBIOS_rt_SOURCES  
  ${CHIBIOS_kernel_SOURCES}
  chcond.c
  chdebug.c
  chdynamic.c
  chevents.c
  chmsg.c
  chmtx.c
  chregistry.c
  chschd.c
  chsem.c
  chstats.c
  chsys.c
  chthreads.c
  chtm.c
  chtrace.c
  chvt.c
)
