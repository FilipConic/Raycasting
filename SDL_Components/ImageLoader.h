#pragma once

#include "Scene.h"
#include <SDL2/SDL_image.h>
#include <initializer_list>

struct Image {
    SDL_Texture* texture;
    std::string name;
    int width, height;
};

class ImageLoader {

    const Scene& scene;

    Image* images;
    int num_images;

    ImageLoader() = delete;
    ImageLoader(const ImageLoader&) = delete;
public:
    ImageLoader(const Scene& _scene);
    ImageLoader(const Scene& _scene, const std::initializer_list<std::string>& image_paths);
    ~ImageLoader();

    void load_image(std::string image_path);
    void load_images(const std::initializer_list<std::string>& image_paths);
    const Image& get_image(int value) const;
    const Image& get_image(const std::string& name) const;
    void draw_image(int value, const SDL_Rect& source_rect, const SDL_Rect& destination_rect) const;
    void draw_image(const std::string& name, const SDL_Rect& source_rect, const SDL_Rect& destination_rect) const;
    void draw_image(const Image& img, const SDL_Rect& source_rect, const SDL_Rect& destination_rect) const;
};
