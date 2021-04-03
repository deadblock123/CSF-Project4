#include <cstdlib>
#include <iostream>
#include <vector>
#include <dirent.h>
#include <dlfcn.h>
#include "pnglite.h"
#include "image.h"


struct Plugin {
	void *handle;
	const char *(*get_plugin_name)(void);
	const char *(*get_plugin_desc)(void);
	void *(*parse_arguments)(int num_args, char*args[]);
	struct Image *(*transform_image)(struct Image *source, void *arg_data);
};

int main(int argc, char** argv) {
	DIR *dirp;
	struct dirent *dp;

	dirp = opendir("./plugins");

	std::vector<struct Plugin*> pluginList;
	dp = readdir(dirp);

	while(dp != NULL) {
		struct Plugin * newPlugin = (struct Plugin *)malloc(sizeof(struct Plugin));
                newPlugin->handle = dlopen(dp->d_name, RTLD_LAZY);

		if (std::string(dp->d_name).compare(".") != 0 && std::string(dp->d_name).compare("..") != 0) {
			*(void **)(&newPlugin->get_plugin_name) = dlsym(newPlugin->handle, "get_plugin_name");
			*(void **)(&newPlugin->get_plugin_desc) = dlsym(newPlugin->handle, "get_plugin_desc");
			*(void **)(&newPlugin->parse_arguments) = dlsym(newPlugin->handle, "parse_arguments");
			*(void **)(&newPlugin->transform_image) = dlsym(newPlugin->handle, "transform_image");
			pluginList.push_back(newPlugin);
		}

		dp = readdir(dirp);

	}

	if (argc < 2) {
		std::cout << "Usage: imgproc <command> [<command args...>]\n";
		std::cout << "Commands are:\n";
		std::cout << "list\n";
		std::cout << "exec <plugin> <input img> <output img> [<plugin args>]\n";
	}
	else if (std::string(argv[1]).compare("list") == 0) { 
		std::cout << "top";
		struct Plugin * currentPlugin;
		std::cout << "mid bottom";
		std::cout << std::to_string(pluginList.size());	
		while (pluginList.empty() == 0) {
			currentPlugin = pluginList.back();
			pluginList.pop_back();
			std::cout << currentPlugin->get_plugin_name();
			std::cout << currentPlugin->get_plugin_desc();
			std::cout << "\n";
		}
	}
	else if (std::string(argv[1]).compare("exec") == 0) {
		std::string requestedPlugin(argv[2]);

		for (int i = 0; i < pluginList.size(); i++) {
			if (requestedPlugin.compare(pluginList.at(i)->get_plugin_name()) == 0) {
				Image * output = pluginList.at(i)->transform_image(img_read_png("kitten.png"), pluginList.at(i)->parse_arguments(argc, argv));
				std::string fileName = std::string("kitten_") + pluginList.at(i)->get_plugin_name();
				img_write_png(output, fileName.c_str());
				return 0;		
			}
		}
		//throw error for second parameter
	}
	else {
	//throw error for first parameter
	}

}
