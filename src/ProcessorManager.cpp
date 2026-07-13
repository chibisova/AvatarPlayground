#include "ProcessorManager.h"

#include "processors/OriginalProcessor.h"
#include "processors/GrayProcessor.h"
#include "processors/BlurProcessor.h"
#include "processors/CannyProcessor.h"
#include "processors/ThresholdProcessor.h"
#include "processors/HarrisProcessor.h"
#include "processors/ShiTomasiProcessor.h"
#include "processors/OpticalFlowProcessor.h"
#include "processors/ORBProcessor.h"
#include "processors/BFMatcherProcessor.h"
#include "processors/KNNMatcherProcessor.h"

namespace vision
{
    ProcessorManager::ProcessorManager ()
    {
        processors[ProcessingMode::Original] = 
            std::make_unique<OriginalProcessor>();

        processors[ProcessingMode::Gray] =
            std::make_unique<GrayProcessor>();

        processors[ProcessingMode::Blur] =
            std::make_unique<BlurProcessor>();
        
        processors[ProcessingMode::Canny] =
            std::make_unique<CannyProcessor>();

        processors[ProcessingMode::Threshold] =
            std::make_unique<ThresholdProcessor>();

        processors[ProcessingMode::HarrisCorners] =
            std::make_unique<HarrisProcessor>();

        processors[ProcessingMode::ShiTomasiCorners] =
            std::make_unique<ShiTomasiProcessor>();
        
        processors[ProcessingMode::OpticalFlow] =
            std::make_unique<OpticalFlowProcessor>();

        processors[ProcessingMode::ORB] =
            std::make_unique<ORBProcessor>();

        processors[ProcessingMode::BFMatcher] = 
            std::make_unique<BFMatcherProcessor>();

        processors[ProcessingMode::KNNMatcher] = 
            std::make_unique<KNNMatcherProcessor>();
    }

    IImageProcessor*
    ProcessorManager::getProcessor(ProcessingMode mode)
    {
        return processors.at(mode).get();
    }
}


