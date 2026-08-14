#include "Map3D.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include "Config.h"
#include <iostream>
#include <fstream>

namespace vision 
{
    void Map3D::addPoints(const std::vector<MapPoint>& points)
    {
        // check descriptores 
        for (const auto& point : points) 
        {
            bool isDuplicate = false;

            for (const auto& existingPoint : mapPoints)
            { 
                int distance = cv::norm(
                    point.descriptor,
                    existingPoint.descriptor,
                    cv::NORM_HAMMING
                );

                if (distance < Config::DESCRIPTOR_DISTANCE_THRESHOLD) 
                {
                    isDuplicate = true;
                    break;
                }
            }
            if (!isDuplicate) 
            {
                mapPoints.push_back(point);
            }
        }
    }

    const std::vector<MapPoint>& Map3D::getPoints() const
    {
        return mapPoints;
    }

    void Map3D::clear() 
    {
        mapPoints.clear();
    }

    std::vector<cv::DMatch> Map3D::matchDescriptors(const cv::Mat& currentDescriptors) const
    {
        std::vector<cv::DMatch> matches;

        if (mapPoints.empty() || currentDescriptors.empty())
            return matches;
        
        // Create a matrix of descriptors from the map points
        cv::Mat mapDescriptors(mapPoints.size(), currentDescriptors.cols, currentDescriptors.type());
        for (size_t i = 0; i < mapPoints.size(); ++i) 
        {
            mapPoints[i].descriptor.copyTo(mapDescriptors.row(i));
        }

        // Use BFMatcher to find matches between current descriptors and map descriptors
        cv::BFMatcher matcher(cv::NORM_HAMMING, true);
        matcher.match(mapDescriptors, currentDescriptors, matches);

        return matches;
    }

    void Map3D::updateLandmarks(const std::vector<MapPoint>& points)
    {
        currentFrame++;
        int matchedLandmarks = 0;
        int newLandmarks = 0;

        for (const auto& point : points)
        {
            bool found = false;

            for (auto& existingPoint : mapPoints)
            {
                int distance = cv::norm(
                    point.descriptor,
                    existingPoint.descriptor,
                    cv::NORM_HAMMING
                );

                if (distance < Config::DESCRIPTOR_DISTANCE_THRESHOLD)
                {
                    existingPoint.observations++;
                    existingPoint.lastSeenFrame = currentFrame;

                    matchedLandmarks++;
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                MapPoint newPoint = point;
                newPoint.observations = 1;
                newPoint.lastSeenFrame = currentFrame;
                newPoint.isBad = false;

                mapPoints.push_back(newPoint);

                newLandmarks++;
            }
        }
    }

    void Map3D::savePLY(
        const std::string& filename,
        int minObservations
    ) const
    {
        std::vector<const MapPoint*> validPoints;
    
        for (const auto& point : mapPoints)
        {
            if (point.isBad)
                continue;
    
            if (point.observations < minObservations)
                continue;
    
            validPoints.push_back(&point);
        }
    
        std::ofstream file(filename);
    
        if (!file.is_open())
        {
            std::cerr << "Failed to open PLY file: "
                      << filename << '\n';
            return;
        }
    
        file << "ply\n";
        file << "format ascii 1.0\n";
        file << "element vertex " << validPoints.size() << "\n";
        file << "property float x\n";
        file << "property float y\n";
        file << "property float z\n";
        file << "end_header\n";
    
        for (const auto* point : validPoints)
        {
            file << point->position.x << " "
                 << point->position.y << " "
                 << point->position.z << "\n";
        }
    
        file.close();
    
        std::cout << "Saved "
                  << validPoints.size()
                  << " map points to "
                  << filename
                  << '\n';
    }

    void Map3D::addCameraPosition(const cv::Point3f& position)
    {
        cameraPositions.push_back(position);
    }

    void Map3D::saveCameraTrajectoryPLY(const std::string& filename) const
    {
        std::ofstream file(filename);

        if (!file.is_open())
        {
            std::cerr << "Failed to open trajectory PLY file: "
                    << filename << '\n';
            return;
        }

        file << "ply\n";
        file << "format ascii 1.0\n";
        file << "element vertex " << cameraPositions.size() << "\n";
        file << "property float x\n";
        file << "property float y\n";
        file << "property float z\n";
        file << "end_header\n";

        for (const auto& position : cameraPositions)
        {
            file << position.x << " "
                << position.y << " "
                << position.z << "\n";
        }

        file.close();

        std::cout << "Saved "
                << cameraPositions.size()
                << " camera positions to "
                << filename
                << '\n';
    }
}
