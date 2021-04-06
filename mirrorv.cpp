//
// Example plugin: flip the image vertically, by using the mirrorv method
//

#include <stdlib.h>
#include "image_plugin.h"

struct Arguments {
        // This plugin doesn't accept any command line arguments;
        // just define a single dummy field.
        int dummy;
};

const char *get_plugin_name(void) {
        return "mirrorv";
}

const char *get_plugin_desc(void) {
        return "mirror image vertically";
}

void *parse_arguments(int num_args, char *args[]) {
        (void) args; // this is just to avoid a warning about an unused parameter

        if (num_args != 0) {
                return NULL;
        }
        return calloc(1, sizeof(struct Arguments));
}

struct Image *transform_image(struct Image *source, void *arg_data) {
        struct Arguments *args = (struct Arguments *) arg_data;

        struct Image *out = img_create(source->width, source->height);
        if (!out) {
                free(args);
                return NULL;
        }

        unsigned amount_of_pixels_width = source->width;
	unsigned amount_of_pixels_height = source->height;
        for(unsigned i = 0; i < amount_of_pixels_height; i++) {
		for(unsigned j = 0; j < amount_of_pixels_width; j++) {
                    out->data[source->width * i + j] = source->data[(((source->height - 1 - i) * source->width) + j)];
		}
	}

        free(args);

        return out;
}

