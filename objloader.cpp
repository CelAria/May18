
#include "stdafx.h"
#include <vector>
#include <stdio.h>
#include <cstring>

#include <glm.hpp>

#include "objloader.hpp"

// .OBJ Loader from http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/

#pragma warning(disable:4996)

bool loadOBJ(const char * path, std::vector<GLfloat> & out_vertices, std::vector<GLuint> & out_indices, std::vector<GLfloat> & out_normals, std::vector<GLuint> & out_normalindices)
{
	printf("Loading OBJ file %s...\n", path);

	FILE * file = fopen(path, "r");

	if( file == NULL ){
		printf("Impossible to open the file ! Are you in the right path ?\n");
		getchar();
		return false;
	}

	while( 1 ){

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		if ( strcmp( lineHeader, "v" ) == 0 ){
			glm::vec3 vertex;

			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );

			out_vertices.push_back(vertex.x);
			out_vertices.push_back(vertex.y);
			out_vertices.push_back(vertex.z);

		
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;

			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);

			out_normals.push_back(normal.x);
			out_normals.push_back(normal.y);
			out_normals.push_back(normal.z);


		}
		else if (strcmp(lineHeader, "f") == 0) {

			GLuint idx1, idx2, idx3, idx4, idx5, idx6, idx7, idx8, idx9; //3 indices per line

			int matches = fscanf(file, "%i %i %i \n", &idx1, &idx2, &idx3);

			if (matches != 3) {
				matches = fscanf(file, "/%i %i/%i %i/%i \n", &idx4, &idx2, &idx5, &idx3, &idx6);
			}

			if (matches == 3 || matches == 5) {
				out_indices.push_back(idx1 - 1); //why -1? hint: look at the first index in teddy.obj
				out_indices.push_back(idx2 - 1);
				out_indices.push_back(idx3 - 1);
				continue;
			}
			
			matches = fscanf(file, "/%i %i/%i/%i %i/%i/%i \n", &idx7, &idx2, &idx5, &idx8, &idx3, &idx6, &idx9);
			if (matches == 7) {
				out_indices.push_back(idx1 - 1); //why -1? hint: look at the first index in teddy.obj
				out_indices.push_back(idx2 - 1);
				out_indices.push_back(idx3 - 1);
				out_normalindices.push_back(idx7 - 1); 
				out_normalindices.push_back(idx8 - 1);
				out_normalindices.push_back(idx9 - 1);

				continue;
			}
}else{

			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}
	return true;
}
