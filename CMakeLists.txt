
cmake_minimum_required(VERSION 3.20)

Project(VpSdk)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>") #解决中文字符

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>") #解决中文字符

set(VpNamespace "Vp::")
set(CMAKE_CXX_STANDARD 20)
set(QT_VERSION_MAJOR 5)
#设置开发包路径
set(DEVELOP_PATH "E:/Project5/RelWithDebInfo")
#设置qt路径
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} D:/Qt/Qt5.12.12/5.12.12/msvc2017_64)
#设置vpsdk路径
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "${DEVELOP_PATH}/lib/cmake")


set(PLUGIN_DIR "${DEVELOP_PATH}/include/ToolInterface/Plugin")
file(GLOB Plugin ${PLUGIN_DIR}/*.cpp ${PLUGIN_DIR}/*.h) #Tool

message(Plugin ${Plugin})

set(OpenCV_DIR "${DEVELOP_PATH}/opencv/build")
#[[set(PROFINET "${DEVELOP_PATH}/Profinet")
set(PROFINET "${DEVELOP_PATH}/ProfinetFile")]]
find_package(OpenCV REQUIRED)

set(3rd ${CMAKE_CURRENT_SOURCE_DIR}/3rd)

find_package(ToolInterface REQUIRED)
find_package(CImage REQUIRED)
find_package(VpBase REQUIRED)
find_package(VpLog REQUIRED)
find_package(VisionCore REQUIRED)
find_package(VpWindowKit REQUIRED)
find_package(CCameraUi REQUIRED)
find_package(VpVarUi REQUIRED)


find_package(CLinkWidget REQUIRED)
find_package(CImageWidget REQUIRED)
find_package(VisionAlgo REQUIRED)
find_package(CApplication  REQUIRED)
find_package(CMainUi  REQUIRED)
find_package(VisionKey REQUIRED)





add_subdirectory(Separator)
#add_subdirectory(Comm)
#add_subdirectory(VisionPlus)



