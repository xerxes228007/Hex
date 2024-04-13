#ifndef TEXTURE_H
#define TEXTURE_H

#include <unordered_map>
#include <filesystem>

#include <QImage>
#include <QDebug>
#include <QString>

class Texture
{
public:

    static void init(std::string_view path);
    inline static std::shared_ptr<QImage> getTexture(std::string_view texture) {return textures[QString(texture.data())];};

private:

    static std::unordered_map<QString, std::shared_ptr<QImage>> textures;


};

#endif // TEXTURE_H
