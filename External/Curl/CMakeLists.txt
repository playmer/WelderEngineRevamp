option(BUILD_TESTING
       "Turn off test suite for libcurl."
       OFF)

option(BUILD_CURL_EXE
       "Turn off the generation of an executable for curl."
       OFF)
       
option(USE_MANUAL
       "Turn off generating the manual for curl."
       OFF)

add_subdirectory(Repo)

add_library(Curl STATIC IMPORTED GLOBAL)
