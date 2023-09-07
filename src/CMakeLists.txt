target_sources(
    ${CMAKE_PROJECT_NAME}
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/TCP_server.cpp
    ${CMAKE_CURRENT_LIST_DIR}/TCP_client.cpp
    ${CMAKE_CURRENT_LIST_DIR}/TCP_chat_server.cpp
    ${CMAKE_CURRENT_LIST_DIR}/TCP_chat_clients.cpp
    ${CMAKE_CURRENT_LIST_DIR}/UDP_server.cpp
    ${CMAKE_CURRENT_LIST_DIR}/UDP_client.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Logger.hpp
)

target_include_directories(
    ${CMAKE_PROJECT_NAME}
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)