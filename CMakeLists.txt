cmake_minimum_required(VERSION 3.25)

project(discord_toolbox)

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(submodules/json)
add_subdirectory(submodules/dotenv)
add_subdirectory(submodules/dpp)

add_executable(discord_toolbox
  src/Config
  src/main.cpp
)

target_include_directories(discord_toolbox PRIVATE 
  inc
)

target_link_libraries(discord_toolbox PRIVATE cpp_dotenv nlohmann_json::nlohmann_json dpp)
