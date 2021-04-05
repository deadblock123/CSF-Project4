//
// Example plugin: change the expoure values of these specific values
//

#include <stdlib.h>
#include <iostream>
#include "image_plugin.h"

struct Arguments {
        double exposureRadius;
};

const char *get_plugin_name(void) {
        return "expose";
}

const char *get_plugin_desc(void) {
        return "change the exposure values of pixels";
}

void *parse_arguments(int num_args, char *args[]) {

	struct Arguments *arguments = (struct Arguments *) malloc(sizeof(struct Arguments));
	
	arguments->exposureRadius = atof(args[num_args-1]);

        return arguments;
}

uint8_t exposedValues(float exposureValue, uint8_t color) {
	return exposureValue * color > 255 ? 255 : (uint8_t) (exposureValue * color);
}

// Helper function to swap the blue and green color component values.
static uint32_t exposure(uint32_t pix, float expVal) {
        uint8_t r, g, b, a;
        img_unpack_pixel(pix, &r, &g, &b, &a);
        return img_pack_pixel(exposedValues(expVal, r), exposedValues(expVal, g), exposedValues(expVal, b), a);
}

struct Image *transform_image(struct Image *source, void *arg_data) {
        struct Arguments *args = (struct Arguments *) arg_data;
        

        // Allocate a result Image
        struct Image *out = img_create(source->width, source->height);
        if (!out) {
                free(args);
                return NULL;
        } else if(args->exposureRadius < 0) {
		free(args);
		return NULL;
	}

        unsigned num_pixels = source->width * source->height;

        for (unsigned i = 0; i < num_pixels; i++) {
                out->data[i] = exposure(source->data[i], args->exposureRadius);
        }

        free(args);

        return out;
}

