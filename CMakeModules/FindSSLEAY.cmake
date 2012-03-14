# find SSLEAY includes and library
#
# SSLEAY_LIBRARY     - full path to the SSLEAY library
# EAY_LIBRARY        - full path to the EAY library
# SSLEAY_FOUND       - TRUE if SSLEAY was found

IF (NOT SSLEAY_FOUND)

  FIND_LIBRARY(SSLEAY_LIBRARY
    NAMES ssleay ssleay32 libssleay32 libssleay
    PATHS
    /usr/lib
    /usr/local/lib
  )

  IF(SSLEAY_LIBRARY)
    MESSAGE(STATUS "Found SSLEAY library: ${SSLEAY_LIBRARY}")
  ELSE(SSLEAY_LIBRARY)
    MESSAGE(STATUS "Could NOT find SSLEAY library.")
  ENDIF(SSLEAY_LIBRARY)
  
  FIND_LIBRARY(EAY_LIBRARY
    NAMES eay eay32 libeay32 libeay
    PATHS
    /usr/lib
    /usr/local/lib
  )
  IF(EAY_LIBRARY)
    MESSAGE(STATUS "Found EAY library: ${EAY_LIBRARY}")
  ELSE(EAY_LIBRARY)
    MESSAGE(STATUS "Could NOT find EAY library.")
  ENDIF(EAY_LIBRARY)

  IF(EAY_LIBRARY AND SSLEAY_LIBRARY)
     SET(SSLEAY_FOUND TRUE CACHE STRING "Whether SSLEAY was found or not")
   ELSE(EAY_LIBRARY AND SSLEAY_LIBRARY)
     SET(SSLEAY_FOUND FALSE)
     IF(SSLEAY_FIND_REQUIRED)
       MESSAGE(FATAL_ERROR "Could not find SSLEAY. Please install SSLEAY")
     ENDIF(SSLEAY_FIND_REQUIRED)
   ENDIF(EAY_LIBRARY AND SSLEAY_LIBRARY)
ENDIF (NOT SSLEAY_FOUND)

