#include <iostream>
#include <filesystem>
#include <fstream>
#include <cmath>
#include <chrono>  // For timing
#include <iomanip> // For formatting output
#include <ctime>   // For timestamp
#include <sstream> // For string stream
#include <openssl/md5.h>
#include "SceneLoader.hpp"
#include "../src/lodepng/lodepng.h"

// Helper function to get timestamp string
std::string getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
    return ss.str();
}

// Helper function to ensure directory exists
void ensureDirectoryExists(const std::string& path) {
    if (!std::filesystem::exists(path)) {
        std::filesystem::create_directories(path);
    }
}

// Helper function to convert MD5 digest to string
std::string md5ToString(unsigned char* md) {
    char buf[33];
    for(int i = 0; i < 16; i++)
        sprintf(buf + i*2, "%02x", md[i]);
    buf[32] = 0;
    return std::string(buf);
}

// Generate MD5 hash from Image
std::string getImageHash(Image* image) {
    MD5_CTX md5Context;
    unsigned char digest[MD5_DIGEST_LENGTH];
    
    MD5_Init(&md5Context);
    
    for(unsigned int y = 0; y < image->height; y++) {
        for(unsigned int x = 0; x < image->width; x++) {
            Color pixel = image->getPixel(x, y);
            // Convert all components to 0-255 range integers
            unsigned char r = static_cast<unsigned char>(pixel.r * 255);
            unsigned char g = static_cast<unsigned char>(pixel.g * 255);
            unsigned char b = static_cast<unsigned char>(pixel.b * 255);
            MD5_Update(&md5Context, &r, sizeof(unsigned char));
            MD5_Update(&md5Context, &g, sizeof(unsigned char));
            MD5_Update(&md5Context, &b, sizeof(unsigned char));
        }
    }
    
    MD5_Final(digest, &md5Context);
    return md5ToString(digest);
}

// Save hash to file
void saveHashToFile(const std::string& referenceFile, const std::string& hash) {
    std::string hashFile = referenceFile.substr(0, referenceFile.find_last_of('.')) + ".hash";
    std::ofstream outFile(hashFile);
    if (outFile.is_open()) {
        outFile << hash;
        outFile.close();
        std::cout << "Saved reference hash to: " << hashFile << std::endl;
    }
}

// Load hash from file
std::string loadHashFromFile(const std::string& referenceFile) {
    std::string hashFile = referenceFile.substr(0, referenceFile.find_last_of('.')) + ".hash";
    std::ifstream inFile(hashFile);
    std::string hash;
    if (inFile.is_open()) {
        std::getline(inFile, hash);
        inFile.close();
    }
    return hash;
}

bool testScene(const std::string& sceneFile, const std::string& referenceFile) {
    try {
        // Start timing
        auto start_time = std::chrono::high_resolution_clock::now();
        
        std::cout << "Testing scene: " << sceneFile << std::endl;
        
        // Create output directory
        std::string outputDir = "output_images";
        ensureDirectoryExists(outputDir);
        
        // Extract scene name from path
        std::string sceneName = std::filesystem::path(sceneFile).stem().string();
        
        // Generate timestamp
        std::string timestamp = getTimestamp();
        
        // Check if files exist
        if (!std::filesystem::exists(sceneFile)) {
            std::cerr << "Scene file not found: " << sceneFile << std::endl;
            return false;
        }
        if (!std::filesystem::exists(referenceFile)) {
            std::cerr << "Reference file not found: " << referenceFile << std::endl;
            return false;
        }
        
        // Load and render test scene
        std::cout << "Loading and rendering scene..." << std::endl;
        auto [scene, camera, image] = SceneLoader::Load(sceneFile);
        camera->render(*image, *scene);
        
        // Save rendered image with timestamp
        std::string renderedImagePath = outputDir + "/" + sceneName + "_rendered_" + timestamp + ".png";
        image->writeFile(renderedImagePath);
        std::cout << "Saved rendered image to: " << renderedImagePath << std::endl;
        
        // Calculate hash of rendered image
        std::string renderedHash = getImageHash(image);
        std::cout << "Rendered image hash: " << renderedHash << std::endl;
        
        // Load reference PNG
        std::cout << "Loading reference image..." << std::endl;
        unsigned refWidth, refHeight;
        std::vector<unsigned char> refPixels;
        unsigned error = lodepng::decode(refPixels, refWidth, refHeight, referenceFile);
        
        if(error) {
            std::cerr << "Failed to load reference PNG: " << lodepng_error_text(error) << std::endl;
            return false;
        }
        
        // Save reference image with timestamp
        std::string referenceImagePath = outputDir + "/" + sceneName + "_reference_" + timestamp + ".png";
        lodepng::encode(referenceImagePath, refPixels, refWidth, refHeight);
        std::cout << "Saved reference image to: " << referenceImagePath << std::endl;
        
        // Verify dimensions match
        if(refWidth != image->width || refHeight != image->height) {
            std::cerr << "Image dimension mismatch!" << std::endl;
            std::cerr << "Reference: " << refWidth << "x" << refHeight << std::endl;
            std::cerr << "Rendered : " << image->width << "x" << image->height << std::endl;
            return false;
        }

        // Convert reference PNG data to our Image format for hash comparison
        Image* referenceImage = new Image(refWidth, refHeight);
        for(unsigned y = 0; y < refHeight; y++) {
            for(unsigned x = 0; x < refWidth; x++) {
                size_t idx = 4 * (y * refWidth + x);  // idx points to RGBA values
                Color color;
                color.r = static_cast<int>(refPixels[idx]) / 255.0f;
                color.g = static_cast<int>(refPixels[idx + 1]) / 255.0f;
                color.b = static_cast<int>(refPixels[idx + 2]) / 255.0f;
                referenceImage->setPixel(x, y, color);
            }
        }

        // Calculate hash of reference image and check storage
        std::string referenceHash = getImageHash(referenceImage);
        std::cout << "Reference image hash: " << referenceHash << std::endl;

        // Check if we need to store the reference hash
        std::string storedHash = loadHashFromFile(referenceFile);
        if(storedHash.empty()) {
            std::cout << "No stored hash found. Saving reference hash..." << std::endl;
            saveHashToFile(referenceFile, referenceHash);
            delete referenceImage;
            delete scene;
            delete camera;
            delete image;
            return true;  // First run creates reference
        }

        // Use stored hash for comparison
        std::cout << "Stored hash    : " << storedHash << std::endl;

        // Compare hashes
        if (renderedHash != storedHash) {
            std::cerr << "Hash mismatch!" << std::endl;
            std::cerr << "Expected: " << storedHash << std::endl;
            std::cerr << "Got     : " << renderedHash << std::endl;
            delete referenceImage;
            delete scene;
            delete camera;
            delete image;
            return false;
        }

        std::cout << "Hashes match!" << std::endl;
        
        // Record timing and write to file
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        std::string timingFile = "execution_times.txt";
        std::ofstream timeLog(timingFile, std::ios::app);
        if (timeLog.is_open()) {
            timeLog << std::left << std::setw(30) << std::filesystem::path(sceneFile).filename().string()
                   << std::right << std::setw(10) << duration.count() 
                   << " ms" << std::endl;
            timeLog.close();
        } else {
            std::cerr << "Failed to open timing file for writing!" << std::endl;
        }
        
        std::cout << "Execution time: " << duration.count() << " ms" << std::endl;
        
        // Cleanup
        delete scene;
        delete camera;
        delete image;
        delete referenceImage;
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return false;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Error: Please provide scene file" << std::endl;
        return 1;
    }

    // Initialize timing file with header
    std::string timingFile = "execution_times.txt";
    std::ofstream timeLog(timingFile, std::ios::trunc);  // Open in truncate mode to clear file
    if (timeLog.is_open()) {
        timeLog << std::left << std::setw(30) << "Scene File" 
               << std::right << std::setw(10) << "Time (ms)" << std::endl;
        timeLog << std::string(42, '-') << std::endl;
        timeLog.close();
    } else {
        std::cerr << "Failed to create timing file!" << std::endl;
    }

    std::string sceneFile = "../scenes/" + std::string(argv[1]);
    std::string referenceFile = "../references/" + std::string(argv[1]);
    referenceFile.replace(referenceFile.find(".json"), 5, ".png");
    
    std::cout << "Testing with:" << std::endl;
    std::cout << "Scene file: " << sceneFile << std::endl;
    std::cout << "Reference: " << referenceFile << std::endl;
    
    bool result = testScene(sceneFile, referenceFile);
    std::cout << "Test result: " << (result ? "PASS" : "FAIL") << std::endl;
    return result ? 0 : 1;
}