//
// Example plugin: change the expoure values of these specific values
//

#include <stdlib.h>
#include "image_plugin.h"

struct Arguments {
        float exposureRadius;
};

const char *get_plugin_name(void) {
        return "expose";
}

const char *get_plugin_desc(void) {
        return "change the exposure values of pixels";
}

void *parse_arguments(int num_args, char *args[]) {
        (voi) args; 

        if (num_args != 0) {
                return NULL;
        }
        return calloc(1, sizeof(struct Arguments));
}

// Helper function to swap the blue and green color component values.
static uint32_t exposure(float expVal) {
        uint8_t r, g, b, a;
        img_unpack_pixel(pix, &r, &g, &b, &a);
        return img_pack_pixel(exposedValues(expVal, r), exposedValues(expVal, g), exposedValues(expVal, b), a);
}

uint8_t exposedValues(float exposureValue, uint8_t color) {
	return exposureValue * color > 255 ? 255 : exposureValue * color;
}

struct Image *transform_image(struct Image *source, void *arg_data) {
        struct Arguments *args = arg_data;
        
        // Allocate a result Image
        struct Image *out = img_create(source->width, source->height);
        if (!out) {
                free(args);
                return NULL;
        } else if(args->exposure < 0) {
		free(args);
		return NULL;
	}

        unsigned num_pixels = source->width * source->height;

        for (unsigned i = 0; i < num_pixels; i++) {
                out->data[i] = exposure(args->exposureRadius);
        }

        free(args);

        return out;
}

