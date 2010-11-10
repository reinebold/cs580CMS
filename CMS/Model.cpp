/**
    \file   Model.cpp
    \brief
*/

#include <gl/glew.h>
#include <string>
#include <iostream>
using namespace std;

#include "Model.h"

Model::Model()
{
    
}

void Model::loadModel(string filename)
{
    numFaces = 0;

    model = lib3ds_file_open(filename.c_str());
    if (!model) 
    {
        cout << "Error: Unable to load: " << filename.c_str();
    }

    lib3ds_file_bounding_box_of_objects(model,1,0,0,boundingBoxMin,boundingBoxMax);

    if (model->nmaterials) 
    {
        printf("%d materials:\n",model->nmaterials);
#ifdef DEBUG_MESSAGES
        for (int i = 0; i < model->nmaterials; ++i){
            Lib3dsMaterial *m = model->materials[i];
            printf("[Material %d: \"%s\"]\n",i+1, m->name);
            printf( "Ka %f %f %f\n", m->ambient[0], m->ambient[1], m->ambient[2]);
            printf( "Kd %f %f %f\n", m->diffuse[0], m->diffuse[1], m->diffuse[2]);
            printf( "Ks %f %f %f\n", m->specular[0], m->specular[1], m->specular[2]);
            printf( "Ns %f\n", pow(2, 10 * m->shininess + 1));
            printf( "d %f\n", 1.0 - m->transparency);
            printf( "map_Kd %s\n", m->texture1_map.name);
            printf( "map_bump %s\n", m->bump_map.name);
            printf( "map_d %s\n", m->opacity_map.name);
            printf( "refl %s\n", m->reflection_map.name);
            printf( "map_KS %s\n", m->specular_map.name);
            printf( "\n");
        }
        printf("\n");
#endif
        
        if(model->ncameras) 
        {
            printf("%d cameras:\n",model->ncameras);
#ifdef DEBUG_MESSAGES
            for (int i = 0; i < model->ncameras; ++i){
                printf("[Camera %d]\n",i+1);
                camera_dump(model->cameras[i]);
            }
            printf("\n");
#endif
        }
        if (model->nlights) {
            printf("%d lights:\n",model->nlights);
#ifdef DEBUG_MESSAGES
            for (int i = 0; i < model->nlights; ++i){ 
                printf("[Light %d]\n",i+1);
                light_dump(model->lights[i]);
            }
            printf("\n");
#endif
        }
    }

    // Calculate the number of faces we have in total
    // Loop through every mesh
    for (int i = 0; i < model->nmeshes; ++i)
    {
        // Add the number of faces this mesh has to the total faces
        numFaces += model->meshes[i]->nfaces;
    }

 

    // Allocate memory for our vertices and normals
    vertices = (float(*)[3])malloc(sizeof(float) * 3 * numFaces * 3);
    normals = (float(*)[3])malloc(sizeof(float) * 3 * numFaces * 3);
    unsigned int FinishedFaces = 0;
    // Loop through all the meshes
    for (int i = 0; i < model->nmeshes; ++i)
    {
        Lib3dsMesh * mesh = model->meshes[i];
        //lib3ds_mesh_calculate_normals
        lib3ds_mesh_calculate_vertex_normals(mesh, &normals[FinishedFaces*3]);
        // Loop through every face
        for(unsigned int cur_face = 0; cur_face < mesh->nfaces; cur_face++)
        {
            Lib3dsFace * face = &mesh->faces[cur_face];
            for(unsigned int j = 0;j < 3;j++)
            {
                memcpy(&vertices[FinishedFaces*3 + j], 
                    mesh->vertices[face->index[ j ]], 
                    sizeof(float)*3);

                
            }
            FinishedFaces++;
        }
    }

    // Generate a Vertex Buffer Object and store it with our vertices
    glGenBuffers(1, &vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
    glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float) * 3 * numFaces, vertices, GL_STATIC_DRAW);

    // Generate another Vertex Buffer Object and store the normals in it
    glGenBuffers(1, &vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float) * 3 * numFaces, normals, GL_STATIC_DRAW);

    // Clean up our allocated memory
    delete [] vertices;
    delete [] normals;
}

Model::~Model()
{
    //lib3ds_file_free(model);
    model = NULL;
}

// Render the model using Vertex Buffer Objects
void Model:: render()
{
    //assert(numFaces != 0);

    // Enable vertex and normal arrays
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    // Bind the vbo with the normals
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    // The pointer for the normals is NULL which means that OpenGL will use the currently bound vbo
    glNormalPointer(GL_FLOAT, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
    glVertexPointer(3, GL_FLOAT, 0, NULL); 
    // Render the triangles
    glDrawArrays(GL_TRIANGLES, 0, numFaces * 3);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

void Model::getCamera( float *camera )
{
    if(model->ncameras > 0)
    {
        camera[0] = model->cameras[0]->position[0];
        camera[1] = model->cameras[0]->position[1];
        camera[2] = model->cameras[0]->position[2];
        
        camera[3] = model->cameras[0]->target[0];
        camera[4] = model->cameras[0]->target[1];
        camera[5] = model->cameras[0]->target[2];
        
        camera[6] = model->cameras[0]->fov;
        camera[7] = model->cameras[0]->near_range;
        camera[8] = model->cameras[0]->far_range;
    }
}