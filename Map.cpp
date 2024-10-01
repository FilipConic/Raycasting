#include "Map.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <cassert>

using namespace std;

#define assertm(condition, msg) assert(((void)msg, condition))

Map::Map(const Scene& _scene, const string& file_path) : scene(_scene), width(0), height(0), cell_size(0), cells(nullptr) { read_from_map_file(file_path); }
Map::~Map() { delete[] cells; }

void seperate_word(const string& input, string& wall, string& floor, string& ceil) {
    char curr = 0;
    wall = ""; floor = ""; ceil = "";
    for (string::const_iterator it = input.cbegin(); it != input.cend(); ++it) {
        switch (curr) {
        case 0: {
            if (*it == '/' || *it == '|' || *it == '\\') { ++curr; continue; }
            wall += *it;
            break;
        }
        case 1: {
            if (*it == '/' || *it == '|' || *it == '\\') { ++curr; continue; }
            floor += *it;
            break;
        }
        case 2: {
            if (*it == '/' || *it == '|' || *it == '\\') { ++curr; continue; }
            ceil += *it;
            break;
        }
        default: return;
        }
    }
}
void Map::read_from_map_file(const string& map_file_path) {
    ifstream file(map_file_path, ios::in);
    assertm(file.is_open(), "Unable to open map file!\n");

    stringstream stream;
    string line, word;

    auto word_to_map_color = [](string& word) -> MapElement {
        transform(word.begin(), word.end(), word.begin(), [](unsigned char c){ return std::tolower(c); });
        if      (word == "black")      return M_BLACK;
        else if (word == "dark_gray")  return M_DARK_GRAY;
        else if (word == "gray")       return M_GRAY;
        else if (word == "light_gray") return M_LIGHT_GRAY;
        else if (word == "white")      return M_WHITE;
        else if (word == "red")        return M_RED;
        else if (word == "green")      return M_GREEN;
        else if (word == "blue")       return M_BLUE;
        else if (word == "yellow")     return M_YELLOW;
        else if (word == "purple")     return M_PURPLE;
        else if (word == "cyan")       return M_CYAN;
        else if (word == "orange")     return M_ORANGE;
        else if (word == "pink")       return M_PINK;
        else if (word == "brick")      return M_BRICK;
        else if (word == "gravel")     return M_GRAVEL;
        else if (word == "cobble")     return M_COBBLE;
        else                           return M_NONE;
    };

    int w;
    while (getline(file, line)) {
        w = 0;
        stream.clear();
        stream << line;
        while (!stream.eof()) {
            stream >> word;
            ++w;
        }
        width = (w >= width ? w : width);
        ++height;
    }
    cells = new unsigned[width * height];
    memset(cells, 0, sizeof(MapElement) * width * height);

    file.close();
    file.open(map_file_path, ios::in);
    assertm(file.is_open(), "Unable to open map file!\n");

    string wall, floor, ceil;
    int i, j = 0;
    while (getline(file, line)) {
        i = 0; 
        stream.clear();
        stream << line;
        while (!stream.eof()) {
            stream >> word;
            seperate_word(word, wall, ceil, floor);
            cells[i++ + j * width] = word_to_map_color(wall) | (word_to_map_color(floor) << 8) | (word_to_map_color(ceil) << 16);
        }
        ++j;
    }

    file.close();
}
const Color& map_element_to_color(Map::MapElement m_clr) {
    switch (m_clr) {
    case Map::M_BLACK:      return BLACK;
    case Map::M_DARK_GRAY:  return DARK_GRAY;
    case Map::M_GRAY:       return GRAY;
    case Map::M_LIGHT_GRAY: return LIGHT_GRAY;
    case Map::M_WHITE:      return WHITE;
    case Map::M_RED:        return RED;
    case Map::M_GREEN:      return GREEN;
    case Map::M_BLUE:       return BLUE;
    case Map::M_YELLOW:     return YELLOW;
    case Map::M_PURPLE:     return PURPLE;
    case Map::M_CYAN:       return CYAN;
    case Map::M_ORANGE:     return ORANGE;
    case Map::M_PINK:       return PINK;
    case Map::M_BRICK:      return RED;
    case Map::M_GRAVEL:     return GRAY;
    case Map::M_COBBLE:     return LIGHT_GRAY;
    default:                return NO_COLOR;
    }
} 
void Map::draw(int map_size, const Vec2<int>& translation_vec) {
    cell_size = map_size / (height > width ? height : width);
    SDL_Rect rect{translation_vec.x, translation_vec.y, width * cell_size,  height * cell_size};

    scene.draw_rect(rect, WHITE);
    rect.w = cell_size;
    rect.h = cell_size;
    MapElement curr_cell;
    for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {
            curr_cell = (MapElement)(cells[i + j * width] & WALL_MASK);
            if (curr_cell)
                scene.draw_rect(rect, map_element_to_color(curr_cell));
            scene.draw_rect_border(rect);
            rect.x += cell_size;
        }
        rect.x = translation_vec.x;
        rect.y += cell_size;
    }
        
}
