cmake_minimum_required(VERSION 3.13.4)
project(myssh)

FILE(GLOB SERVER_SRC ${CMAKE_CURRENT_SOURCE_DIR}/source/mysshserver/*.c)

add_executable(mysshserver
  ${SERVER_SRC}
)

target_link_libraries(mysshserver
  PRIVATE -lpam
  PRIVATE -lpam_misc
)

target_include_directories(mysshserver
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

FILE(GLOB CLIENT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/source/mysshclient/*.c)

add_executable(mysshclient
  ${CLIENT_SRC}
)

target_include_directories(mysshclient
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)
