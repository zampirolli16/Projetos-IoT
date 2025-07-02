# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/rafael/esp/esp-idf/components/bootloader/subproject"
  "/home/rafael/esp/projects/can_send_final/build/bootloader"
  "/home/rafael/esp/projects/can_send_final/build/bootloader-prefix"
  "/home/rafael/esp/projects/can_send_final/build/bootloader-prefix/tmp"
  "/home/rafael/esp/projects/can_send_final/build/bootloader-prefix/src/bootloader-stamp"
  "/home/rafael/esp/projects/can_send_final/build/bootloader-prefix/src"
  "/home/rafael/esp/projects/can_send_final/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/rafael/esp/projects/can_send_final/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/rafael/esp/projects/can_send_final/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
