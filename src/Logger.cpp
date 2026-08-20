#include "Logger.h"

namespace vision
{
    Logger::Logger(const std::string& filename)
    {
        file.open(filename);
    }

    Logger::~Logger()
    {
        if (file.is_open())
            file.close();
    }

    void Logger::log(const std::string& message)
    {
        if (file.is_open())
            file << message << std::endl;
    }

    bool Logger::isOpen() const
    {
        return file.is_open();
    }
}
