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
	
	while((dp = readdir(dirp)) != NULL) {
		struct Plugin * newPlugin = (struct Plugin *)malloc(sizeof(struct Plugin));
		std::string nameOfLibrary(dp->d_name);

		if (nameOfLibrary.length() > 3 && nameOfLibrary.substr(nameOfLibrary.length() - 3) == ".so") {
			newPlugin->handle = dlopen((std::string("./plugins/")+std::string(dp->d_name)).c_str(), RTLD_LAZY);
          		//check for null
			*(void **)(&newPlugin->get_plugin_name) = dlsym(newPlugin->handle, "get_plugin_name");
			*(void **)(&newPlugin->get_plugin_desc) = dlsym(newPlugin->handle, "get_plugin_desc");
			*(void **)(&newPlugin->parse_arguments) = dlsym(newPlugin->handle, "parse_arguments");
			*(void **)(&newPlugin->transform_image) = dlsym(newPlugin->handle, "transform_image");
			pluginList.push_back(newPlugin);
		}

	}

	if (argc < 2) {
		std::cout << "Usage: imgproc <command> [<command args...>]\n";
		std::cout << "Commands are:\n";
		std::cout << "list\n";
		std::cout << "exec <plugin> <input img> <output img> [<plugin args>]\n";
	}
	else if (std::string(argv[1]).compare("list") == 0) { 
		
		struct Plugin * currentPlugin;
			
		while (pluginList.empty() == 0) {
			currentPlugin = pluginList.back();
			pluginList.pop_back();
			std::cout << currentPlugin->get_plugin_name();
			std::cout << "\n";
			std::cout << currentPlugin->get_plugin_desc();
			std::cout << "\n";
			std::cout << "\n";

		}
	}
	else if (std::string(argv[1]).compare("exec") == 0) {
		std::string requestedPlugin(argv[2]);
		std::string inputImage(argv[3]);
		std::string outputImage(argv[4]);

		for (unsigned i = 0; i < pluginList.size(); i++) {
			if (requestedPlugin.compare(pluginList.at(i)->get_plugin_name()) == 0) {
				Image * output = pluginList.at(i)->transform_image(img_read_png(inputImage.c_str()), pluginList.at(i)->parse_arguments(argc, argv));
				img_write_png(output, outputImage.c_str());
				return 0;		
			}
		}
		//throw error for second parameter
	}
	else {
	//throw error for first parameter
	}

}
