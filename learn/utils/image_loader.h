//
// Created by tangs on 2020/9/8.
//

#ifndef GLFW_IMAGE_LOADER_H
#define GLFW_IMAGE_LOADER_H

void image_loader_init();
unsigned char *image_loader_load (char const *filename, int *x, int *y, int *channels_in_file, int desired_channels);
void image_loader_image_free(unsigned char *data);

#endif //GLFW_IMAGE_LOADER_H
