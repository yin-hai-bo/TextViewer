find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test Widgets)

add_executable(ConfigTests
    test_config.cpp
    ../config.cpp
)

add_executable(RecentListTests
    test_recentlist.cpp
    ../recentlist.cpp
)

add_executable(ViewerTests
    test_viewer.cpp
    ../viewer.cpp
    ../res/textviewer.qrc
)

add_executable(MainWindowTests
    test_mainwindow.cpp
    ../mainwindow.cpp
    ../viewer.cpp
    ../recentlist.cpp
    ../config.cpp
    ../aboutbox.cpp
    ../lineheightdialog.cpp
    ../res/textviewer.qrc
)

foreach(test_target ConfigTests RecentListTests ViewerTests MainWindowTests)
    target_include_directories(${test_target} PRIVATE ${CMAKE_SOURCE_DIR})
    target_link_libraries(${test_target} PRIVATE Qt${QT_VERSION_MAJOR}::Test Qt${QT_VERSION_MAJOR}::Widgets)
    deploy_qt_runtime(${test_target})
endforeach()

add_test(NAME ConfigTests COMMAND ConfigTests)
add_test(NAME RecentListTests COMMAND RecentListTests)
add_test(NAME ViewerTests COMMAND ViewerTests)
add_test(NAME MainWindowTests COMMAND MainWindowTests)
