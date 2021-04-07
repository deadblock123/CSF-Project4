#include <cstdlib>
#include <iostream>
#include <vector>
#include <dirent.h>
#include <dlfcn.h>
#include <iomanip>
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

	char * pluginDirectory = getenv("PLUGIN_DIR");
	if (pluginDirectory == NULL) {
		pluginDirectory = "./plugins";
	}
	dirp = opendir(pluginDirectory);

	std::vector<struct Plugin*> pluginList;
	
	while((dp = readdir(dirp)) != NULL) {
		std::string nameOfLibrary(dp->d_name);
	
		if (nameOfLibrary.length() > 3 && nameOfLibrary.substr(nameOfLibrary.length() - 3) == ".so") {
			struct Plugin * newPlugin = (struct Plugin *)malloc(sizeof(struct Plugin));
			newPlugin->handle = dlopen((std::string(pluginDirectory)+std::string("/")+std::string(dp->d_name)).c_str(), RTLD_LAZY);
			*(void **)(&newPlugin->get_plugin_name) = dlsym(newPlugin->handle, "get_plugin_name");
			*(void **)(&newPlugin->get_plugin_desc) = dlsym(newPlugin->handle, "get_plugin_desc");
			*(void **)(&newPlugin->parse_arguments) = dlsym(newPlugin->handle, "parse_arguments");
			*(void **)(&newPlugin->transform_image) = dlsym(newPlugin->handle, "transform_image");
			pluginList.push_back(newPlugin);
		}
	}

	closedir(dirp);

	if (argc < 2) {
		std::cout << "Usage: imgproc <command> [<command args...>]\n";
		std::cout << "Commands are:\n";
		std::cout << "list\n";
		std::cout << "exec <plugin> <input img> <output img> [<plugin args>]\n";

		while (pluginList.empty() == 0) {
                        free(pluginList.back());
			pluginList.pop_back();
                }

	}
	else if (std::string(argv[1]).compare("list") == 0) { 
		
		struct Plugin * currentPlugin;
			
		std::cout << "Loaded " << pluginList.size() << " plugin(s)\n";
		while (pluginList.empty() == 0) {
			currentPlugin = pluginList.back();

			std::cout << std::setw(8) << currentPlugin->get_plugin_name();
			std::cout << ": ";
			std::cout << currentPlugin->get_plugin_desc();
			std::cout << "\n";

			pluginList.pop_back();
			free(currentPlugin);
		}

	}
	else if (std::string(argv[1]).compare("exec") == 0) {
		std::string requestedPlugin(argv[2]);
		std::string inputImage(argv[3]);
		std::string outputImage(argv[4]);

		for (unsigned i = 0; i < pluginList.size(); i++) {
			if (requestedPlugin.compare(pluginList.at(i)->get_plugin_name()) == 0) {
				struct Arguments* parsedArguments = (struct Arguments *) pluginList.at(i)->parse_arguments(argc - 5, argv + 5);
				if (parsedArguments == NULL) {
					free(parsedArguments);
					free(dirp);

					while (pluginList.empty() == 0) {
                                        	free(pluginList.back());
                                        	pluginList.pop_back();
                                	}

					std::cout << "ERROR: wrong amount of plugin parameters";
					exit (1);	
				}
				Image * inputImagePointer = img_read_png(inputImage.c_str());
				Image * output = pluginList.at(i)->transform_image(inputImagePointer, pluginList.at(i)->parse_arguments(argc - 5, argv + 5));
				img_write_png(output, outputImage.c_str());
				free(inputImagePointer->data);
				free(inputImagePointer);
				free(output->data);
				free(output);
				free(parsedArguments);

				while (pluginList.empty() == 0) {
                        		free(pluginList.back());
                        		pluginList.pop_back();
		                }

				return 0;	
			}
		}

		while (pluginList.empty() == 0) {
			free(pluginList.back());
			pluginList.pop_back();
                }

		std::cout << "ERROR: not an availible plugin";
                exit (1);
	}
	else {
		while (pluginList.empty() == 0) {
                        free(pluginList.back());
                        pluginList.pop_back();
                }

	
		std::cout << "ERROR: not an availible command";
                exit (1);

	}

}
