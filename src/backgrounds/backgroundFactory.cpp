#include "backgroundFactory.hpp"

std::unique_ptr<Background> BackgroundFactory::createBackground(BackgroundType type) {
    switch (type) {
        case BackgroundType::DESERT:
            return std::make_unique<Desert>();
        case BackgroundType::IMAGE_PNG:
            return std::make_unique<Imagepng>();
        default:
            return nullptr;
    }
}
