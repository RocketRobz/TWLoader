// twpng.h - based on SFIL
#ifndef TWLOADER_TWPNG_H
#define TWLOADER_TWPNG_H

#include <3ds.h>
#include <sf2d.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Loads a PNG image from the SD card
 * @param filename the path of the image to load
 * @param place where to allocate the texture
 * @return a pointer to the newly created texture/image
 */
sf2d_texture *twl_load_PNG_file(const char *filename, sf2d_place place);

/**
 * @brief Loads a PNG image from a memory buffer
 * @param buffer the pointer of the memory buffer to load the image from
 * @param place where to allocate the texture
 * @return a pointer to the newly created texture/image
 */
sf2d_texture *twl_load_PNG_buffer(const void *buffer, sf2d_place place);

#define sfil_load_PNG_file(filename, place) twl_load_PNG_file(filename, place)
#define sfil_load_PNG_buffer(filename, place) twl_load_PNG_buffer(filename, place)

#ifdef __cplusplus
}
#endif

#endif /* TWLOADER_TWPNG_H */
