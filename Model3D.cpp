//
//  Model3D.cpp
//  Graphics Engine
//
//  Created by Ruiqing Qiu on 4/21/15.
//  Copyright (c) 2015 Ruiqing Qiu. All rights reserved.
//
#include "SOIL.h"
#include "Model3D.h"
static float t = 0.0;
Model3D::Model3D()
{
}
void Model3D::setTextureMap(string pathname){
    texture_map = pathname;
}
void Model3D::setNormalMap(string pathname){
    normal_map = pathname;
}
void Model3D::setGlossMap(string pathname){
    gloss_map = pathname;
}
void Model3D::setMetallicMap(string pathname){
    metallic_map = pathname;
}
void Model3D::setVertexShader(string vertex){
    vertex_shader = vertex;
}
void Model3D::setFragmentShader(string fragment){
    fragment_shader = fragment;
}


GLhandleARB loadShader1(char* filename, unsigned int type)
{
    FILE *pfile;
    GLhandleARB handle;
    const GLcharARB* files[1];
    
    // shader Compilation variable
    GLint result;				// Compilation code result
    GLint errorLoglength;
    char* errorLogText;
    GLsizei actualErrorLogLength;
    
    char buffer[400000];
    memset(buffer, 0, 400000);
    
    // This will raise a warning on MS compiler
    pfile = fopen(filename, "rb");
    if (!pfile)
    {
        printf("Sorry, can't open file: '%s'.\n", filename);
        exit(0);
    }
    
    fread(buffer, sizeof(char), 400000, pfile);
    //printf("%s\n",buffer);
    
    
    fclose(pfile);
    
    handle = glCreateShaderObjectARB(type);
    if (!handle)
    {
        //We have failed creating the vertex shader object.
        printf("Failed creating vertex shader object from file: %s.", filename);
        exit(0);
    }
    
    files[0] = (const GLcharARB*)buffer;
    glShaderSourceARB(
                      handle, //The handle to our shader
                      1, //The number of files.
                      files, //An array of const char * data, which represents the source code of theshaders
                      NULL);
    
    glCompileShaderARB(handle);
    
    //Compilation checking.
    glGetObjectParameterivARB(handle, GL_OBJECT_COMPILE_STATUS_ARB, &result);
    
    // If an error was detected.
    if (!result)
    {
        //We failed to compile.
        printf("Shader '%s' failed compilation.\n", filename);
        
        //Attempt to get the length of our error log.
        glGetObjectParameterivARB(handle, GL_OBJECT_INFO_LOG_LENGTH_ARB, &errorLoglength);
        
        //Create a buffer to read compilation error message
        errorLogText = (char*)malloc(sizeof(char)* errorLoglength);
        
        //Used to get the final length of the log.
        glGetInfoLogARB(handle, errorLoglength, &actualErrorLogLength, errorLogText);
        
        // Display errors.
        printf("%s\n", errorLogText);
        
        // Free the buffer malloced earlier
        free(errorLogText);
    }
    printf("Shader loaded done\n");
    return handle;
}


Model3D::Model3D(string filename, string texture, string normal, string gloss, string metallic, string vertex, string fragment){
    setTextureMap(texture);
    setNormalMap(normal);
    setGlossMap(gloss);
    setMetallicMap(metallic);
    setVertexShader(vertex);
    setFragmentShader(fragment);
    
    std::string inputfile = filename;
    
    std::string err = tinyobj::LoadObj(shapes, materials, inputfile.c_str(), NULL);
    
    if (!err.empty()) {
        cout << err << endl;
        exit(1);
    }
    
    std::cout << "# of shapes    : " << shapes.size() << std::endl;
    std::cout << "# of materials : " << materials.size() << std::endl;
    std::cout << "# of tangents : " << shapes[0].mesh.tangent.size() << std::endl;
    for (size_t i = 0; i < shapes.size(); i++) {
        printf("shape[%ld].name = %s\n", i, shapes[i].name.c_str());
        printf("Size of shape[%ld].indices: %ld\n", i, shapes[i].mesh.indices.size());
        printf("Size of shape[%ld].material_ids: %ld\n", i, shapes[i].mesh.material_ids.size());
        for (size_t f = 0; f < shapes[i].mesh.indices.size() / 3; f++) {
            printf("  idx[%ld] = %d, %d, %d. mat_id = %d\n", f, shapes[i].mesh.indices[3 * f + 0], shapes[i].mesh.indices[3 * f + 1], shapes[i].mesh.indices[3 * f + 2], shapes[i].mesh.material_ids[f]);
        }
        
        printf("shape[%ld].vertices: %ld\n", i, shapes[i].mesh.positions.size());
        for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
            printf("  v[%ld] = (%f, %f, %f)\n", v,
                   shapes[i].mesh.positions[3 * v + 0],
                   shapes[i].mesh.positions[3 * v + 1],
                   shapes[i].mesh.positions[3 * v + 2]);
        }
    }
    
    for (size_t i = 0; i < materials.size(); i++) {
        printf("material[%ld].name = %s\n", i, materials[i].name.c_str());
        printf("  material.Ka = (%f, %f ,%f)\n", materials[i].ambient[0], materials[i].ambient[1], materials[i].ambient[2]);
        printf("  material.Kd = (%f, %f ,%f)\n", materials[i].diffuse[0], materials[i].diffuse[1], materials[i].diffuse[2]);
        printf("  material.Ks = (%f, %f ,%f)\n", materials[i].specular[0], materials[i].specular[1], materials[i].specular[2]);
        printf("  material.Tr = (%f, %f ,%f)\n", materials[i].transmittance[0], materials[i].transmittance[1], materials[i].transmittance[2]);
        printf("  material.Ke = (%f, %f ,%f)\n", materials[i].emission[0], materials[i].emission[1], materials[i].emission[2]);
        printf("  material.Ns = %f\n", materials[i].shininess);
        printf("  material.Ni = %f\n", materials[i].ior);
        printf("  material.dissolve = %f\n", materials[i].dissolve);
        printf("  material.illum = %d\n", materials[i].illum);
        printf("  material.map_Ka = %s\n", materials[i].ambient_texname.c_str());
        printf("  material.map_Kd = %s\n", materials[i].diffuse_texname.c_str());
        printf("  material.map_Ks = %s\n", materials[i].specular_texname.c_str());
        printf("  material.map_Ns = %s\n", materials[i].normal_texname.c_str());
        std::map<std::string, std::string>::const_iterator it(materials[i].unknown_parameter.begin());
        std::map<std::string, std::string>::const_iterator itEnd(materials[i].unknown_parameter.end());
        for (; it != itEnd; it++) {
            printf("  material.%s = %s\n", it->first.c_str(), it->second.c_str());
        }
        printf("\n");
        
        
        if (materials[i].diffuse_texname.c_str())
        {
            GLuint tex_2d = SOIL_load_OGL_texture
            (
             //strcat(pre,name),
             //"Texture/mygod.tga",
             materials[i].diffuse_texname.c_str(),
             SOIL_LOAD_AUTO,
             SOIL_CREATE_NEW_ID,
             SOIL_FLAG_INVERT_Y
             );
            
            /* check for an error during the load process */
            if (0 == tex_2d)
            {
                printf("SOIL loading error: '%s'\n", SOIL_last_result());
                isTextured = false;
            }
            else{
                printf("SOIL loading success %i\n", tex_2d);
                isTextured = true;
            }
        }
    }
    
    
    GLhandleARB vertexShaderHandle;
    GLhandleARB fragmentShaderHandle;
    char *v_tmp = new char[vertex_shader.length() + 1];
    strcpy(v_tmp, vertex_shader.c_str());
    char *f_tmp = new char[fragment_shader.length() + 1];
    strcpy(f_tmp, fragment_shader.c_str());
    cout << v_tmp << endl;
    cout << f_tmp << endl;
    vertexShaderHandle = loadShader1(v_tmp, GL_VERTEX_SHADER);
    fragmentShaderHandle = loadShader1(f_tmp, GL_FRAGMENT_SHADER);
    shader_id = glCreateProgramObjectARB();
    
    glAttachObjectARB(shader_id, vertexShaderHandle);
    glAttachObjectARB(shader_id, fragmentShaderHandle);
    glLinkProgramARB(shader_id);
    
    //glUseProgramObjectARB(shader_id);
    
    
    glGenTextures(3, texturaID);
    int width, height;
    //unsigned char* image;
    
    glBindTexture(GL_TEXTURE_2D, texturaID[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    texturaID[0] = SOIL_load_OGL_texture(texture_map.c_str(), SOIL_LOAD_AUTO,
                                         SOIL_CREATE_NEW_ID,
                                         SOIL_FLAG_INVERT_Y);
    if (texturaID[0] == 0)
    {
        cout << "error" << endl;
    }
    else{
        cout << texturaID[0] << endl;
        isTextured = true;
    }
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    //SOIL_free_image_data(image);
    
    
    glBindTexture(GL_TEXTURE_2D, texturaID[1]);
    texturaID[1] = SOIL_load_OGL_texture(normal_map.c_str() , SOIL_LOAD_AUTO,
                                         SOIL_CREATE_NEW_ID,
                                         SOIL_FLAG_INVERT_Y);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    //SOIL_free_image_data(image);
    if (texturaID[1] == 0)
    {
        cout << "error 1" << endl;
    }
    
    glBindTexture(GL_TEXTURE_2D, texturaID[2]);
    texturaID[2] = SOIL_load_OGL_texture(gloss_map.c_str(), SOIL_LOAD_AUTO,
                                         SOIL_CREATE_NEW_ID,
                                         SOIL_FLAG_INVERT_Y);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    //SOIL_free_image_data(image);
    if (texturaID[2] == 0)
    {
        cout << "error 2" << endl;
    }
    
    glBindTexture(GL_TEXTURE_2D, texturaID[3]);
    texturaID[3] = SOIL_load_OGL_texture(metallic_map.c_str(), SOIL_LOAD_AUTO,
                                         SOIL_CREATE_NEW_ID,
                                         SOIL_FLAG_INVERT_Y);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    //SOIL_free_image_data(image);
    if (texturaID[3] == 0)
    {
        cout << "error 3" << endl;
    }
    
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texturaID[0]);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texturaID[1]);
    
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texturaID[2]);
    
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, texturaID[3]);
    glUniform1i(glGetUniformLocationARB(shader_id, "tex"), 0);
    glUniform1i(glGetUniformLocationARB(shader_id, "norm"), 1);
    glUniform1i(glGetUniformLocationARB(shader_id, "gloss"), 2);
    glUniform1i(glGetUniformLocationARB(shader_id, "metallic"), 3);
}


Model3D::~Model3D()
{
}
/*
 typedef struct {
 std::vector<float> positions;
 std::vector<float> normals;
 std::vector<float> texcoords;
 std::vector<unsigned int> indices;
 std::vector<int> material_ids; // per-mesh material ID
 } mesh_t;
 typedef struct {
 std::string name;
 mesh_t mesh;
 } shape_t;
 typedef struct {
 std::string name;
 float ambient[3];
 float diffuse[3];
 float specular[3];
 float transmittance[3];
 float emission[3];
 float shininess;
 float ior;      // index of refraction
 float dissolve; // 1 == opaque; 0 == fully transparent
 // illumination model (see http://www.fileformat.info/format/material/)
 int illum;
 std::string ambient_texname;
 std::string diffuse_texname;
 std::string specular_texname;
 std::string normal_texname;
 std::map<std::string, std::string> unknown_parameter;
 } material_t;
 */
void Model3D::OnDraw(){
    t += 1.0;
    //Set the OpenGL Matrix mode to ModelView (used when drawing geometry)
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    //glTranslatef(0, 0, -20);
    //glutSolidSphere(5, 20, 20);
    glColor3f(1, 1, 1);
    for (size_t i = 0; i < shapes.size(); i++) {
        
        for (size_t f = 0; f < shapes[i].mesh.indices.size() / 3; f++) {
            int i1 = shapes[i].mesh.indices[3 * f + 0];
            int i2 = shapes[i].mesh.indices[3 * f + 1];
            int i3 = shapes[i].mesh.indices[3 * f + 2];
            int m1 = shapes[i].mesh.material_ids[f];
            
            if (isTextured){
                //material goes here
                //glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, whiteSpecularMaterial);
                //glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mShininess);
                glUseProgramObjectARB(shader_id);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texturaID[0]);
                
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, texturaID[1]);
                
                
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, texturaID[2]);
                
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, texturaID[3]);
                
                glUniform1i(glGetUniformLocationARB(shader_id, "tex"), 0);
                glUniform1i(glGetUniformLocationARB(shader_id, "norm"), 1);
                glUniform1i(glGetUniformLocationARB(shader_id, "gloss"), 2);
                glUniform1i(glGetUniformLocationARB(shader_id, "metallic"), 3);
                
                float value [4] = {float(shapes[i].mesh.tangent[f].x),
                                   float(shapes[i].mesh.tangent[f].y),
                                   float(shapes[i].mesh.tangent[f].z),
                                   float(shapes[i].mesh.tangent[f].w)};
                glUniform4fv(glGetUniformLocationARB(shader_id, "VertexTangent"), 1, value);
                // Make sure no bytes are padded:
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                
                // Select GL_MODULATE to mix texture with polygon color for shading:
                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                
                // Use bilinear interpolation:
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                //glEnable(GL_TEXTURE_2D);
                
                //glMaterialfv(GL_FRONT, GL_AMBIENT, materials[m1].ambient);
                //glMaterialfv(GL_FRONT, GL_DIFFUSE, materials[m1].diffuse);
                //glMaterialfv(GL_FRONT, GL_SPECULAR, materials[m1].specular);
                //glMaterialfv(GL_FRONT, GL_SHININESS, &materials[m1].shininess);
            }
            //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            //THIS LINE OF CODE MUST BE AFTER THE TEXTURE LOADING CODE
            //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            glBegin(GL_TRIANGLES);
            //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            //DONT MODIFY IF YOU DONT KNOW WHAT IT IS
            //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            if (isTextured){
                //texture
                glTexCoord2f(shapes[i].mesh.texcoords[2 * i1 + 0], shapes[i].mesh.texcoords[2 * i1 + 1]);
            }
            //avg normal goes here
            glNormal3f(shapes[i].mesh.normals[3 * i1 + 0],
                       shapes[i].mesh.normals[3 * i1 + 1],
                       shapes[i].mesh.normals[3 * i1 + 2]
                       );
            glVertex3f(shapes[i].mesh.positions[3 * i1 + 0], shapes[i].mesh.positions[3 * i1 + 1], shapes[i].mesh.positions[3 * i1 + 2]);
            
            if (isTextured){
                //texture
                glTexCoord2f(shapes[i].mesh.texcoords[2 * i2 + 0], shapes[i].mesh.texcoords[2 * i2 + 1]);
            }
            //avg normal goes here
            glNormal3f(shapes[i].mesh.normals[3 * i2 + 0],
                       shapes[i].mesh.normals[3 * i2 + 1],
                       shapes[i].mesh.normals[3 * i2 + 2]
                       );
            glVertex3f(shapes[i].mesh.positions[3 * i2 + 0], shapes[i].mesh.positions[3 * i2 + 1], shapes[i].mesh.positions[3 * i2 + 2]);
            
            if (isTextured){
                //texture
                glTexCoord2f(shapes[i].mesh.texcoords[2 * i3 + 0], shapes[i].mesh.texcoords[2 * i3 + 1]);
            }
            //avg normal goes here
            glNormal3f(shapes[i].mesh.normals[3 * i3 + 0],
                       shapes[i].mesh.normals[3 * i3 + 1],
                       shapes[i].mesh.normals[3 * i3 + 2]
                       );
            glVertex3f(shapes[i].mesh.positions[3 * i3 + 0], shapes[i].mesh.positions[3 * i3 + 1], shapes[i].mesh.positions[3 * i3 + 2]);
            
            glEnd();
            
            if (isTextured){
                glBindTexture(GL_TEXTURE_2D, 0);
                glDisable(GL_TEXTURE_2D);
            }
            
        }
        
    }
    
    //glUseProgramObjectARB(0);
    
    //glActiveTexture(GL_TEXTURE0);
    glPopMatrix();
    
    
}