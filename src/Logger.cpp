
#include "Logger.h"

#include <filesystem>


Logger::Logger(bool showMetaInformation) : showMetaInformation_(showMetaInformation)
{
}

Logger::~Logger()
{
}

std::ostream& Logger::Log(LogLevel logLevel, const std::source_location& sourceLocation) const
{
	std::ostream& currentStream = logLevel == LogLevel::Error ? std::cerr : std::cout;

	if (logLevel == LogLevel::Debug)
	{
		currentStream << "DBG|| ";
	}
	else if (logLevel == LogLevel::Info)
	{
		currentStream << "\033[33mINF|| \033[0m";
	}
	else if (logLevel == LogLevel::Warning)
	{
		currentStream << "\033[36mWRN|| \033[0m";
	}
	else if (logLevel == LogLevel::Error)
	{
		currentStream << "\033[31mERR|| \033[0m";
	}

	if (this->showMetaInformation_ == true)
	{
		currentStream
			<< std::filesystem::path(sourceLocation.file_name()).filename().string()
			<< '(' << sourceLocation.line() << "): ";
	}

	return currentStream;
}
