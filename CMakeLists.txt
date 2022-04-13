cmake_minimum_required(VERSION 3.15)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum OS X deployment target")
project(TS-M1N3 VERSION 1.1.0)

set(CMAKE_CXX_STANDARD 17)

find_package(JUCE REQUIRED)

add_subdirectory(modules)
include_directories(modules)


set(JUCE_FORMATS LV2)

juce_add_plugin(DarkStar
    COMPANY_NAME GuitarML
    PLUGIN_MANUFACTURER_CODE GtML
    PLUGIN_CODE Tsm3 
    FORMATS ${JUCE_FORMATS}
    ProductName "DarkStar"
    LV2_URI https://github.com/GuitarML/DarkStar
    ICON_BIG resources/logo.png
    MICROPHONE_PERMISSION_ENABLED TRUE
)

# create JUCE header
juce_generate_juce_header(DarkStar)

# add sources
add_subdirectory(Source)
include_directories(Source)
add_subdirectory(resources)

target_compile_definitions(DarkStar
    PUBLIC
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_REPORT_APP_USAGE=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(DarkStar PUBLIC
    juce_plugin_modules
)

