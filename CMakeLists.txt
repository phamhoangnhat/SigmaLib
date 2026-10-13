cmake_minimum_required(VERSION 3.21)
project(SigmaLib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)

add_library(SigmaLib SHARED
    AccountManager.cpp
    ChangeCase.cpp
    CharsetRegistryWatcher.cpp
    Clipboard.cpp
    ConfigUi.cpp
    CustomConfirmBox.cpp
    CustomInputMethod.cpp
    CustomMessageBox.cpp
    Feedback.cpp
    GeneralConfig.cpp
    KeyAPIManage.cpp
    LangRegistryWatcher.cpp
    Listener.cpp
    MessageApiKeyBox.cpp
    NoticeUi.cpp
    RenameAccountDialog.cpp
    RenameDialog.cpp
    RenameSnippetDialog.cpp
    ShortcutKeyEditor.cpp
    SigmaLib.cpp
    SnippetEditor.cpp
    TabRightDelegate.cpp
    TaskAI.cpp
    TaskAIDatabase.cpp
    TaskAIEditor.cpp
    TaskAIResult.cpp
    TrayIcon.cpp
    TypeWord.cpp
    Util.cpp
    Variable.cpp
    Word.cpp

    AccountManager.h
    CharsetRegistryWatcher.h
    ConfigUi.h
    CustomConfirmBox.h
    CustomInputMethod.h
    CustomMessageBox.h
    Feedback.h
    GeneralConfig.h
    KeyAPIManage.h
    LangRegistryWatcher.h
    MessageApiKeyBox.h
    NoticeUi.h
    RenameAccountDialog.h
    RenameDialog.h
    RenameSnippetDialog.h
    ShortcutKeyEditor.h
    SigmaLib.h
    SnippetEditor.h
    TaskAI.h
    TaskAIEditor.h
    TaskAIResult.h
    TrayIcon.h

    SigmaLib.qrc
    SigmaLib.ui
    SigmaLib.rc
)

target_compile_definitions(SigmaLib PRIVATE SIGMALIB_LIBRARY UNICODE _UNICODE)
# Codebase tron "..." va <...> cho header local — can source dir tren include path
target_include_directories(SigmaLib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(SigmaLib PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network)
