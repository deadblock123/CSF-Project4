//
// Example plugin: breaks it up into n*n different seperate tiles, where the functions end up changing
//

#include <stdlib.h>
#include <iostream>
#include "image_plugin.h"

struct Arguments {
        int tiles;
};

const char *get_plugin_name(void) {
        return "tile";
}

const char *get_plugin_desc(void) {
        return "tile source image in an NxN arrangement";
}

void *parse_arguments(int num_args, char *args[]) {
        struct Arguments *arguments = (struct Arguments *) malloc(sizeof(struct Arguments));

	if (num_args != 1) {
		return NULL;
	}

	arguments->tiles = atoi(args[0]);

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

	unsigned number_of_transformations_width = (source->width + args->tiles- 1) / args->tiles;
	unsigned number_of_transformations_height = (source->height + args->tiles- 1) / args->tiles;

	int values[number_of_transformations_height][number_of_transformations_width];
	for(int n = 0; n < (int) number_of_transformations_height; n++) {
		for(int m = 0; m < (int) number_of_transformations_width; m++) {
                         values[n][m] = source->data[n * source->width * args->tiles + m * args->tiles];

		}
	}
	
	int tileX;
	int tileY;
	for (int x = 0; x < source->width; x++) {
		for (int y = 0; y < source->height; y++) { 
			tileX = x % number_of_transformations_width;
			tileY = y % number_of_transformations_height;

			out->data[(y * source->width) + x] = values[tileY][tileX];
		}
	}

        free(args);

        return out;
}

