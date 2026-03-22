cmake_minimum_required(VERSION 3.16)

project(botfunpay VERSION 0.1 LANGUAGES CXX)

# ── Стандарт C++ ───────────────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Qt autogen ─────────────────────────────────────────────────────────────────
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# ── Qt 6 ───────────────────────────────────────────────────────────────────────
set(CMAKE_PREFIX_PATH "/home/undersirable/Qt/6.10.2/gcc_64")
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# ── Boost ──────────────────────────────────────────────────────────────────────
set(BOOST_ROOT "/home/undersirable/Загрузки/boost_1_90_0")

if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

find_package(Boost CONFIG REQUIRED COMPONENTS system)

# ── OpenSSL ────────────────────────────────────────────────────────────────────
find_package(OpenSSL REQUIRED)

# ── Исходники ──────────────────────────────────────────────────────────────────
set(PROJECT_SOURCES
    main.cpp

    # GUI
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui

    dashboardwindow.cpp
    dashboardwindow.h

    addlotdialog.cpp
    addlotdialog.h
    addlotdialog.ui

    funpayapi.cpp
    funpayapi.h

    httpclient.cpp
    httpclient.h

    cookieloader.cpp
    cookieloader.h

    gamesparser.cpp
    gamesparser.h

    gamesservice.cpp
    gamesservice.h

    gamemodels.h
    models.h

    # Bot
    botworker.cpp
    botworker.h

    botengine.cpp
    botengine.h

    # Infrastructure
    logger.cpp
    logger.h

    settingsmanager.cpp
    settingsmanager.h

    resources.qrc
)

qt_add_executable(botfunpay
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

# ── Include dirs ───────────────────────────────────────────────────────────────
target_include_directories(botfunpay PRIVATE
    "${BOOST_ROOT}"
)

# ── Линковка ───────────────────────────────────────────────────────────────────
target_link_libraries(botfunpay
    PRIVATE
    Qt6::Core
    Qt6::Widgets
    Boost::system
    OpenSSL::SSL
    OpenSSL::Crypto
)

# ── Свойства цели ──────────────────────────────────────────────────────────────
set_target_properties(botfunpay PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING
        "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS botfunpay
    BUNDLE  DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(botfunpay)
