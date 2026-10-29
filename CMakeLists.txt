cmake_minimum_required(VERSION 3.14)
project(EWalletCorePlugin LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include the Logos Module CMake helper
if(DEFINED ENV{LOGOS_MODULE_BUILDER_ROOT})
    include($ENV{LOGOS_MODULE_BUILDER_ROOT}/cmake/LogosModule.cmake)
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/cmake/LogosModule.cmake")
    include(cmake/LogosModule.cmake)
else()
    message(FATAL_ERROR "LogosModule.cmake not found")
endif()

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/metadata.json" METADATA_JSON)
string(JSON MODULE_NAME GET ${METADATA_JSON} name)

# Define the module with external library
logos_module(
    NAME ${MODULE_NAME}
    SOURCES
        src/e_wallet_core_impl.h
        src/e_wallet_core_impl.cpp
        src/wallet_engine.h
        src/wallet_engine.cpp
        src/wallet_accounts.h
        src/wallet_accounts.cpp
        src/wallet_transfers.h
        src/wallet_transfers.cpp
        src/wallet_sessions.h
        src/wallet_sessions.cpp
        src/wallet_approval.h
        src/wallet_approval.cpp
        src/wallet_identity.h
        src/wallet_identity.cpp
        src/wallet_names.h
        src/wallet_names.cpp
        src/wallet_contracts.h
        src/wallet_contracts.cpp
        src/wallet_helpers.h
    EXTERNAL_LIBS
        lez_wallet_sdk
    LINK_LIBRARIES
        pcsclite
)
