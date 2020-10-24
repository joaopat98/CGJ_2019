#ifndef TEXTURE_H
#define TEXTURE_H
#include <GL/glew.h>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>


#include "stb_image.h"


namespace engine {

	class Texture
	{
	public:

		Texture();
		Texture(const char* filepath);

		int width, height, nrChannels;
		unsigned char* data;

		unsigned int texture;

	private:

		void loadImage(const char * filepath);
	};
}

#endif TEXTURE_H