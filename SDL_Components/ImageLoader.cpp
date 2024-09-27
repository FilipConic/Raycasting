#include "ImageLoader.h"

#include <cassert>
#include <regex>
#include <iostream>

using namespace std;

#define assertm(condition, msg) assert(((void)msg, condition))

ImageLoader::ImageLoader(const Scene& _scene) : scene(_scene), images(nullptr), num_images(0) {
    int result = IMG_Init(IMG_INIT_PNG);
    assertm(result & IMG_INIT_PNG, "Unable to initialize PNG loading!\n");
}
ImageLoader::ImageLoader(const Scene& _scene, const vector<string>& image_paths) : scene(_scene), images(nullptr), num_images(0) {
    int result = IMG_Init(IMG_INIT_PNG);
    assertm(result & IMG_INIT_PNG, "Unable to initialize PNG loading!\n");
    load_images(image_paths);
}
ImageLoader::~ImageLoader() {
    for (int i = 0; i < num_images; ++i) SDL_DestroyTexture(images[i].texture);
    if (num_images) delete[] images;
}

void ImageLoader::load_image(string image_path) {
    SDL_Surface* surface = IMG_Load(image_path.c_str());
    assertm(surface != NULL, "Surface path looks to be wrong!\n");
    SDL_Texture* new_texture = SDL_CreateTextureFromSurface(scene, surface);
    assertm(new_texture != NULL, "Unable to create a texture!\n");
    
    Image* holder = images;
    images = new Image[num_images + 1];
    for (int i = 0; i < num_images; ++i)
        images[i] = holder[i];
    delete[] holder;

    smatch matched;
    regex_search(image_path, matched, regex("\\w+\\.png"));
    assertm(!matched.empty(), "Unable to read image name!");
    images[num_images].name = matched[0];

    images[num_images].texture = new_texture;
    SDL_QueryTexture(new_texture, NULL, NULL, &images[num_images].width, &images[num_images].height);
    ++num_images;

    SDL_FreeSurface(surface);
}
void ImageLoader::load_images(const vector<string>& image_paths){
    SDL_Surface* surface;
    SDL_Texture* new_texture;

    Image* holder = images;
    images = new Image[num_images + image_paths.size()];
    for (int i = 0; i < num_images; ++i)
        images[i] = holder[i];
    delete[] holder;
    
    for (string path : image_paths) {
        surface = IMG_Load(path.c_str());
        assertm(surface != NULL, "Surface path looks to be wrong!\n");
        new_texture = SDL_CreateTextureFromSurface(scene, surface);
        assertm(new_texture != NULL, "Unable to create a texture!\n");

        smatch matched;
        regex_search(path, matched, regex("\\w+\\.png"));
        assertm(!matched.empty(), "Unable to read image name!");
        images[num_images].name = matched[0];

        images[num_images].texture = new_texture;
        SDL_QueryTexture(new_texture, NULL, NULL, &images[num_images].width, &images[num_images].height);
        ++num_images;
        
        SDL_FreeSurface(surface);
    }
}
const Image& ImageLoader::get_image(int value) const {
    assertm(num_images > 0, "There are no images!");
    return images[value >= 0 ? (value < num_images ? value : num_images - 1) : 0];
}
const Image& ImageLoader::get_image(const string& name) const {
    assertm(num_images > 0, "There are no images!");

    for (int i = 0; i < num_images; ++i)
        if (images[i].name == name) return images[i];

    return images[0];
}
void ImageLoader::draw_image(int value, const SDL_Rect& source_rect, const SDL_Rect& destination_rect) const {
    SDL_RenderCopy(scene, get_image(value).texture, &source_rect, &destination_rect);
}
void ImageLoader::draw_image(const string& name, const SDL_Rect& source_rect, const SDL_Rect& destination_rect) const {
    SDL_RenderCopy(scene, get_image(name).texture, &source_rect, &destination_rect);
}
void ImageLoader::draw_image(const Image& img, const SDL_Rect& source_rect, const SDL_Rect& destination_rect) const {
    SDL_RenderCopy(scene, img.texture, &source_rect, &destination_rect);
}
