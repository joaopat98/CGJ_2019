#ifndef CUBEMAP_H
#define CUBEMAP_H
#include <GL/glew.h>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "stb_image.h"


namespace engine {

	class CubeMap
	{
	public:

		CubeMap();
		CubeMap(std::vector<std::string> faces);
		CubeMap(std::vector<unsigned char*> sides, int width, int height);

		int width, height, nrChannels;
		//unsigned char* data;

		unsigned int texture;

	private:
		void loadCubemap(std::vector<std::string> faces);
	};
}

#endif CUBEMAP_H
