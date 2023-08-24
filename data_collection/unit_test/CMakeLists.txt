project(data_collection_test)
#option(HAS_RABBITMQ "whether cmake in HAS_RABBITMQ" ON)
add_definitions(-DHAS_RABBITMQ)
add_executable(data_collection_test
                ../rabbitmq_client.cpp
                ../cJson.c
                main.cpp)

#            include_directories()
#            link_directories()

target_link_libraries(data_collection_test  rabbitmq)
