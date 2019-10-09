#pragma once

#include <string>
#include <sstream>
#include<cmath>
#include <iostream>
#include <vector>

using namespace std;


#define HEADER_LINES 3
typedef struct Location
{
    float x;
    float y;
    float z;
}Location_;

class pgm3D {
public:
    pgm3D()
    {
        x_rows = 0;
        y_cols = 0;
        z_layers = 0;
        total_cubes = 0;
        max_color = 0;
    }

    std::vector<float> voxel_val;

    int x_rows, y_cols, z_layers, total_cubes;
    float max_color;

    Location get_coordinates(int location)
    {
        Location coord;
    float offset_x, offset_y, offset_z;
    offset_x = 0.5*((x_rows+1)%2);
    offset_y = 0.5*((y_cols+1)%2);
    offset_z = 0.5*((z_layers+1)%2);
        coord.x = location%(x_rows) - (x_rows/2) + offset_x;
        coord.y = std::floor((float)((location%(x_rows*y_cols))/(y_cols))) - (y_cols/2) + offset_y;
        coord.z = std::floor((float)location/(x_rows*y_cols)) - (z_layers/2) + offset_z;
        return coord;
    }
    int get_location(Location coord)
    {
    float offset_x, offset_y, offset_z;
    offset_x = 0.5*((x_rows+1)%2);
    offset_y = 0.5*((y_cols+1)%2);
    offset_z = 0.5*((z_layers+1)%2);
        coord.x += (x_rows/2) - offset_x;
        coord.y += (y_cols/2) - offset_y;
        coord.z += (z_layers/2) - offset_z;
        return (coord.z*x_rows*y_cols)+(coord.y*y_cols)+coord.x;
    }

    Face compute_face(Location coord, Location normal)
    {

        Face face;
        face.c = voxel_val[get_location(coord)]/max_color; //Normalize color [0,1]/
        QVector3D normal3D;
        normal3D = QVector3D(normal.x, normal.y, normal.z);
        face.normal = normal3D;

        QVector3D v1, v2, v3, v4;
        if(abs((double)normal.x)==1)
        {
            float x_ = normal.x*0.5;
            v1 = QVector3D(coord.x+x_, coord.y+0.5f, coord.z+0.5f);


            v2 = QVector3D(coord.x+x_, coord.y-0.5f, coord.z+0.5f);


            v3 = QVector3D(coord.x+x_, coord.y+0.5f, coord.z-0.5f);


            v4 = QVector3D(coord.x+x_, coord.y-0.5f, coord.z-0.5f);

        }


        if(abs((double)normal.y)==1)
        {
            float y_ = normal.y*0.5;
            v1 = QVector3D(coord.x+0.5f,coord.y+y_, coord.z+0.5f) ;

            v2 = QVector3D(coord.x-0.5f, coord.y+y_, coord.z+0.5f);

            v3 = QVector3D(coord.x+0.5f,coord.y+y_,coord.z-0.5f) ;


            v4 = QVector3D(coord.x-0.5f,  coord.y+y_, coord.z-0.5f);

        }


        if(abs((double)normal.z)==1)
        {
            float z_ = normal.z*0.5;
            v1 = QVector3D(coord.x+0.5f, coord.y+0.5f, coord.z+z_);

            v2= QVector3D(coord.x+0.5f, coord.y-0.5f,  coord.z+z_);

            v3 = QVector3D(coord.x-0.5f, coord.y+0.5f,  coord.z+z_);


            v4 = QVector3D(coord.x-0.5f, coord.y-0.5f, coord.z+z_);

        }


        face.vertices.push_back(v3);
        face.vertices.push_back(v4);
        face.vertices.push_back(v2);
        face.vertices.push_back(v1);

        return face;

    }
    bool check_neighbours(int location,std::vector<Face> &faces)
    {

        Location coord = get_coordinates(location);

        Location n1 = {coord.x+1,coord.y,coord.z};
        Location normal1 = {1,0,0};
        float neighbour_value = 0;
        if(n1.x < (x_rows - (x_rows/2)))
        {
            neighbour_value = voxel_val[get_location(n1)];

        }

        if(voxel_val[get_location(coord)] != neighbour_value)
        {

            faces.push_back(compute_face(coord,normal1));
        }


        Location n2 = {coord.x-1,coord.y,coord.z};
        Location normal2 = {-1,0,0};
        neighbour_value = 0;
        if(n2.x >= (0- (x_rows/2)))
        {

            neighbour_value = voxel_val[get_location(n2)];
        }
        if(voxel_val[get_location(coord)] != neighbour_value)
        {

            faces.push_back(compute_face(coord,normal2));
        }


        Location n3 = {coord.x,coord.y+1,coord.z};
        Location normal3 = {0,1,0};
        neighbour_value = 0;
        if(n3.y <  (y_cols - (y_cols/2)))
        {

            neighbour_value = voxel_val[get_location(n3)];
        }
        if(voxel_val[get_location(coord)] != neighbour_value)
        {

            faces.push_back(compute_face(coord,normal3));
        }


        Location n4 = {coord.x,coord.y-1,coord.z};
        Location normal4 = {0,-1,0};
        neighbour_value = 0;
        if(n4.y >= (0- (y_cols/2)))
        {

            neighbour_value = voxel_val[get_location(n4)];
        }
        if(voxel_val[get_location(coord)] != neighbour_value)
        {

            faces.push_back(compute_face(coord,normal4));
        }


        Location n5 = {coord.x,coord.y,coord.z+1};
        Location normal5 = {0,0,1};
        neighbour_value = 0;
        if(n5.z < (z_layers - (z_layers/2)))
        {

            neighbour_value = voxel_val[get_location(n5)];
        }
        if(voxel_val[get_location(coord)] != neighbour_value)
        {

            faces.push_back(compute_face(coord,normal5));
        }


        Location n6 = {coord.x,coord.y,coord.z-1};
        Location normal6 = {0,0,-1};
        neighbour_value = 0;
        if(n6.z >= (0 - (z_layers/2)))
        {

            neighbour_value = voxel_val[get_location(n6)];
        }
        if(voxel_val[get_location(coord)] != neighbour_value)
        {

            faces.push_back(compute_face(coord,normal6));
        }

        return true;
    }

    void get_all_faces(std::vector<Face> &faces)
    {

        vector<Face> result;
        for(int index = 0;index<total_cubes;index++)
        {
            check_neighbours(index,faces);
        }
    }


};

