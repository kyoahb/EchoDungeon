#include "Logger.h"
#include <chrono>
#include <iomanip>
#include <sstream>

bool Logger::is_initialised = false; // Default to not initialised
std::string Logger::folder_name = "logs"; // Default folder name

/**
 * @brief Initialises the logger by setting up file and console sinks, log format, and log level.
 */
void Logger::init() {
    if (is_initialised) {
    return; // Already initialised, do nothing
    }
    // Create log folder if it doesn't exist, else do nothing
    ensure_folder_exists(folder_name);

    // Setup spdlog with file sink AND console sink
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(folder_name + "/" + generate_filename(), true);
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto logger = std::make_shared<spdlog::logger>("Logger", spdlog::sinks_init_list{file_sink, console_sink});

    // Setup logging format of "[HH:MM:SS] [level] message"
    logger->set_pattern("[%H:%M:%S:%e] [%^%l%$] %v");

    // Set log level to debug and set as default logger
    logger->set_level(spdlog::level::trace);
    spdlog::set_default_logger(logger);

    // Set initialised flag so it cannot be re-initialised unnecessarily
    is_initialised = true;
}

/**
 * @brief Checks if a folder exists. If it does not, then creates it.
 *
 * @param folder The folder name to check
 */
void Logger::ensure_folder_exists(const std::string& folder) {
	if (!std::filesystem::exists(folder)) {
		std::filesystem::create_directory(folder);
	}
}

/**
 * @brief Logs a message to the file and console sinks.
 *
 * @param level The level of the message (eg info)
 * @param message The string to log
 */
void Logger::log(spdlog::level::level_enum level, const std::string& message) {
	if (!is_initialised) {
		throw std::runtime_error("Logger not initialised. Call Logger::init() before logging messages.");
	}
	spdlog::log(level, message); // Log the message at the specified level
}

/**
 * @brief Generates a log filename based on current day and time
 *
 * @return string The log filename
 */
std::string Logger::generate_filename() {
	// Get current date and time
	auto now = std::chrono::system_clock::now();
	std::time_t now_c = std::chrono::system_clock::to_time_t(now);
	std::tm tm_buf;
	localtime_s(&tm_buf, &now_c);

	// Format as YYYY_MM_DD_HH-MM-SS.log
	std::ostringstream oss;
	oss << std::put_time(&tm_buf, "%Y_%m_%d_%H-%M-%S") << ".log";

	return oss.str();

}