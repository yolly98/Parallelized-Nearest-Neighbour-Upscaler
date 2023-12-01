#ifndef _HEADERS_H
#define _HEADERS_H

#include <cstdint>
#include <string>
#include <vector>

enum UpscalerType {
    UpscaleFromOriginalImage,
    UpscaleFromUpscaledImage,
    UpscaleWithSingleThread
};

struct Settings {
    uint32_t threadsPerBlockX;
    uint32_t threadsPerBlockY;
    uint32_t threadsPerBlockZ;

    uint32_t blocksPerGridX;
    uint32_t blocksPerGridY;
    uint32_t blocksPerGridZ;

    UpscalerType upscalerType;

    Settings()
        : threadsPerBlockX(0), threadsPerBlockY(0), threadsPerBlockZ(0), blocksPerGridX(0), blocksPerGridY(0), blocksPerGridZ(0), upscalerType(UpscalerType::UpscaleFromOriginalImage) {}
    Settings(uint32_t tpbX, uint32_t tpbY, uint32_t tpbZ, uint32_t bpgX, uint32_t bpgY, uint32_t bpgZ, UpscalerType ut)
        : threadsPerBlockX(tpbX), threadsPerBlockY(tpbY), threadsPerBlockZ(tpbZ), blocksPerGridX(bpgX), blocksPerGridY(bpgY), blocksPerGridZ(bpgZ), upscalerType(ut) {}
    Settings(uint32_t tpbX, uint32_t tpbZ, UpscalerType ut, uint32_t width, uint32_t height, uint8_t upscaleFactor)
        : threadsPerBlockX(tpbX), threadsPerBlockY(1), threadsPerBlockZ(tpbZ), blocksPerGridY(1), blocksPerGridZ(1), upscalerType(ut) {
        // compute the number of blocks per grid on x-axis
        switch (ut) 
        {
            case UpscalerType::UpscaleFromOriginalImage:
                blocksPerGridX = ((width * height) + threadsPerBlockX - 1) / threadsPerBlockX;
                break;
            case UpscalerType::UpscaleFromUpscaledImage:
                blocksPerGridX = ((width * height * threadsPerBlockZ * upscaleFactor * upscaleFactor) + (threadsPerBlockX * threadsPerBlockZ) - 1) / (threadsPerBlockX * threadsPerBlockZ);
                break;
            case UpscalerType::UpscaleWithSingleThread:
                blocksPerGridX = 1;
                break;
        }
    }


    void print() {
        std::cout << "\n[+] GPU Upscale Settings: " << std::endl;

        switch (upscalerType)
        {
            case UpscalerType::UpscaleFromOriginalImage:
                std::cout << "--> Upscaler Type: UpscaleFromOriginalImage" << std::endl;
                break;
            case UpscalerType::UpscaleFromUpscaledImage:
                std::cout << "--> Upscaler Type: UpscaleFromUpscaledImage" << std::endl;
                break;
            case UpscalerType::UpscaleWithSingleThread:
                std::cout << "--> Upscaler Type: UpscaleWithSingleThread" << std::endl;
                break;
        }

        printf("--> Threads per Block: (%d, %d, %d)\n", threadsPerBlockX, threadsPerBlockY, threadsPerBlockZ);
        printf("--> Blocks per Grid: (%d, %d, %d)\n", blocksPerGridX, blocksPerGridY, blocksPerGridZ);
    }

    std::string toString() {
        std::string str;

        str = str + std::to_string(threadsPerBlockX) + ";" + std::to_string(threadsPerBlockY) + ";" + std::to_string(threadsPerBlockZ);
        str = str + ";" + std::to_string(blocksPerGridX) + ";" + std::to_string(blocksPerGridY) + ";" + std::to_string(blocksPerGridZ);

        switch (upscalerType)
        {
        case UpscalerType::UpscaleFromOriginalImage:
            str = str + ";" + "\"UpscaleFromOriginalImage\"";
            break;
        case UpscalerType::UpscaleFromUpscaledImage:
            str = str + ";" + "\"UpscaleFromUpscaledImage\"";
            break;
        case UpscalerType::UpscaleWithSingleThread:
            str = str + ";" + "\"UpscaleWithSingleThread\"";
            break;
        }

        return str;
    }
};

enum Channels {
    ALL = 0,
    GREY = 1,
    GREY_ALPHA = 2,
    RGB = 3,
    RGB_ALPHA = 4
};

float cpuUpscaler(uint8_t upscaleFactor, uint8_t* originalImage, size_t width, size_t height, uint32_t bytePerPixel, std::string imageName = "");
float cpuMultithreadUpscaler(uint32_t numThread, uint8_t upscaleFactor, uint8_t* originalImage, size_t width, size_t height, uint32_t bytePerPixel, std::string imageName = "");
float gpuUpscaler(size_t originalSize, size_t upscaledSize, uint8_t upscaleFactor, Settings settings, uint8_t* data, uint32_t width, uint32_t height, uint32_t bytePerPixel, std::string imageName = "");
std::pair<float, float> computeCI(const std::vector<float>& values);

#endif  // _HEADERS_H
