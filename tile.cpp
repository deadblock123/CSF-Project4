//
// Example plugin: breaks it up into n*n different seperate tiles, where the functions end up changing
//

#include <stdlib.h>
#include "image_plugin.h"

struct Arguments {
        // This plugin doesn't accept any command line arguments;
        // just define a single dummy field.
        int tiles;
};

const char *get_plugin_name(void) {
        return "tile";
}

const char *get_plugin_desc(void) {
        return "n*n different images of the base will be place within the set dimensions given";
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

        // Allocate a result Image
        struct Image *out = img_create(source->width, source->height);
        if (!out) {
                free(args);
                return NULL;
        } else if(args->tiles < 1) {
		free(args);
		return NULL;
	} else if(args->tiles == 1) {
		free(args);
		return source;
        }

	unsigned number_of_transformations_width = source->width / args->tiles;
	unsigned number_of_transformations_height = source->height / args->tiles;

        for (unsigned i = 0; i < number_of_transformations_height; i++) {
		int k = 0;
                for(unsigned j = 0; j > 0; j++) {
                    out->data[source->width * i + j] = source->data[args->tiles * i + args->tiles * j];
		    if(j >= number_of_transformations_width) {
		      if(k < args->tiles) {
                       k++;
		       j = 0;
		      } else {
                       j = -1;
		      }
		    }
		}
        }

        free(args);

        return out;
}

