#include "texture.h"


void Texture::init(std::string_view path)
{
    for(const auto& i : std::filesystem::directory_iterator(path))
    {
        if(i.path().extension() != ".png")
        {
            qDebug() << "File on " << i.path().u8string() << " is not png";
            continue;
        }

        qDebug() << "File on " << i.path().u8string() << " was successfully loaded";

        textures[QString::fromUtf8(i.path().stem().u8string())] = std::make_shared<QImage>(QString::fromUtf8(i.path().u8string()));

    }
}


std::unordered_map<QString, std::shared_ptr<QImage>> Texture::textures;
