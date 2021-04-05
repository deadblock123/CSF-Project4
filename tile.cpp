//
// Example plugin: breaks it up into n*n different seperate tiles, where the functions end up changing
//

#include <stdlib.h>
#include <iostream>
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
        struct Arguments *arguments = (struct Arguments *) malloc(sizeof(struct Arguments));

	arguments->tiles = atof(args[num_args-1]);

	return arguments;
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

	int values[number_of_transformations_height][number_of_transformations_width];
	for(int n = 0; n < (int) number_of_transformations_height; n++) {
		for(int m = 0; m < (int) number_of_transformations_width; m++) {
                         values[n][m] = source->data[n * source->width * args->tiles + m * args->tiles];

		}
	}

	int placement = 0;
	for(int i = 0; i < args->tiles; i++) {
	    for(int k = 0; k < (int) number_of_transformations_height; k++) {
		for(int j = 0; j < args->tiles; j++) {
                    for(int l = 0; l < (int) number_of_transformations_width; l++) {
		       out->data[placement] = values[k][l];
		       placement++;
		    }
		}
	    }
	}

        free(args);

        return out;
}

