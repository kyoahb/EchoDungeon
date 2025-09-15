#pragma once
#include <spdlog/spdlog.h>
#include <filesystem>
#include <thread>
#include <sstream>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

class Logger {

public:
	static void init(); // Initialise logger, set up files and sinks and allow logging

	static void log(spdlog::level::level_enum level, const std::string& message); // Log a message with a specific level

private:
	static std::string folder_name; // Folder to store log files in
	static bool is_initialised; // Whether init() has executed

	static void ensure_folder_exists(const std::string& folder_path); // Ensure a folder exists, create it if it doesn't
	static std::string generate_filename(); // Generates a file name for the current program instance's log file
};

#define ADD_FUNCHEADER_TO_MSG(message) \
    (std::string("[") + __FUNCTION__ + "] [" + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())).substr(0, 5) + "] " + (message))

#define CRITICAL(message) Logger::log(spdlog::level::critical, ADD_FUNCHEADER_TO_MSG(message))
#define TRACE(message) Logger::log(spdlog::level::trace, ADD_FUNCHEADER_TO_MSG(message))
#define INFO(message) Logger::log(spdlog::level::info, ADD_FUNCHEADER_TO_MSG(message))
#define WARNING(message) Logger::log(spdlog::level::warn, ADD_FUNCHEADER_TO_MSG(message))
#define ERROR(message) Logger::log(spdlog::level::err, ADD_FUNCHEADER_TO_MSG(message))