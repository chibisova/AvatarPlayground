#pragma once

#include <memory>
#include <unordered_map>

#include "ImageProcessor.h"
#include "processors/IImageProcessor.h"

namespace vision
{
    class ProcessorManager
    {
    public:
        ProcessorManager();

        IImageProcessor* getProcessor(ProcessingMode mode);

    private:
        std::unordered_map<ProcessingMode, std::unique_ptr<IImageProcessor>> processors;
    };
}

