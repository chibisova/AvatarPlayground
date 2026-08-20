#pragma once

#include <fstream>
#include <string>

namespace vision
{
    class Logger
    {
    public:
        explicit Logger(const std::string& filename);
        ~Logger();

        void log(const std::string& message);

        bool isOpen() const;

    private:
        std::ofstream file;
    };
}
