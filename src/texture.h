#ifndef TEXTURE_H
#define TEXTURE_H

#include <unordered_map>
#include <filesystem>

#include <QImage>

class Texture
{
public:

    static void Init();


private:

    static std::unordered_map<std::filesystem::path, QImage> textures;


};

#endif // TEXTURE_H
