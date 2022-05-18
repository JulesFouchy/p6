#include "Image.h"
#include <img/img.hpp>
#include <stdexcept>
#include "details/make_absolute_path.h"

namespace p6 {

Image::Image(ImageSize size, const uint8_t* data, glpp::TextureLayout texture_layout)
{
    _texture.upload_data(size, data, texture_layout);
}

Image load_image(std::filesystem::path file_path)
{
    try
    {
        const auto image_data = img::load(details::make_absolute_path(file_path), 4);
        return Image{{static_cast<GLsizei>(image_data.size().width()),
                      static_cast<GLsizei>(image_data.size().height())},
                     image_data.data()};
    }
    catch (const std::runtime_error& e)
    {
        throw std::runtime_error{std::string{e.what()}
                                 + "\nMaybe you forgot to call p6_copy_folder() in your CMakeLists.txt? See https://julesfouchy.github.io/p6-docs/tutorials/images#loading-an-image"};
    }
}

} // namespace p6