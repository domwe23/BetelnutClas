cmake_minimum_required(VERSION 3.20)
project(BetelnutClas)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_PREFIX_PATH "D:/Qt/Qt5.12.12/5.12.12/msvc2017_64")
set(QT_VERSION_MAJOR 5)

# 找到 Qt 模块
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets Concurrent Sql SerialPort Network REQUIRED)





add_definitions("-DUNICODE" "-D_UNICODE")



set(SOURCES
        main.cpp
        MainWindow.cpp
        MainWindow.h
        MainWindow.ui
        boardcard.cpp
        boardcard.h
        boardcard.ui
        ComWindow.cpp
        ComWindow.h
        ComWindow.ui
        vfdwindow.cpp
        vfdwindow.h
        vfdwindow.ui
        meswindow.cpp
        meswindow.h
        meswindow.ui
        userwindow.cpp
        userwindow.h
        userwindow.ui
        calibrationwindow.cpp
        calibrationwindow.h
        calibrationwindow.ui
        BlSizeWindow.cpp
        BlSizeWindow.h
        BlSizeWindow.ui
        #MCDLL_NET.h
)


qt5_add_resources(RESOURCE ./resources.qrc)


add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${RESOURCE} App.rc)

target_link_libraries(${PROJECT_NAME}
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Sql
        Qt${QT_VERSION_MAJOR}::SerialPort
        Qt${QT_VERSION_MAJOR}::Network


)

target_link_libraries(${PROJECT_NAME} ${VpNamespace}CApplication
        ${VpNamespace}CMainUi
        ${VpNamespace}VpWindowKit
        ${VpNamespace}VpBase
        ${VpNamespace}VpLog
        ${VpNamespace}VisionCore
        ${VpNamespace}VpVarUi
        ${VpNamespace}VpBase
        ${VpNamespace}CImage
        ${VpNamespace}CCameraUi
        "${DEVELOP_PATH}/HeanderFile/MCDLL_NET.lib"
        "${DEVELOP_PATH}/ProfinetFile/ProfinetCLR.lib"

)

target_link_libraries(${PROJECT_NAME} ${VpNamespace}CImageWidget)

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})




set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${DEVELOP_PATH}/bin
)
