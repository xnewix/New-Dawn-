#ifndef GRID_H
#define GRID_H

#include <iostream> // and I put this here because.......
#include "CTextures.h"
#include "SpaceCamera.h"
#include "ObjectLoader.h"
#include "hgtreader.h"

#include "Vec3.h" // 3D vector class

// Currently replacing the old Normal3d and Point3d classes with a combined Vec3 class.
// This should hopefully cut down on code duplication.
// Undecided on whether to have another class which then combines these two classes 
// or to simple create two instances in the Grid.h class.

/* 
class VertexData() {
    Vec3 point3d;
    Vec3 normal3d;
    
    or 
    
    std::vector<Vec3> point3d;
    std::vector<Vec3> normal3d;
    
    currently favouring the first option, as it seams to give more flexability.
    addtovector(Vec3& v) {
        v.push_back(Vec3());
    }
}
*/

// TO DO: 22/08/14
/*
   As the old point3d and normal3d classes are being superseded by the new Vec3 class 
   a lot of the normal calculation code will be made superflicious. 
   The intention is to replace this with a new implemntation which uses vector math
   functions in the Vec3 class. This should make things a little less haphazed.
*/

class Normal3d {
private:
	double x;
	double y;
	double z;
public:
	Normal3d(double nx, double ny, double nz) : x(nx), y(ny), z(nz) {
	}
	Normal3d() {};
	~Normal3d() {}
	double getx()  { return x; }
    double gety()  { return y; }
    double getz()  { return z; }
};

class Point3d {
private:
    double x;
    double y;
    double z;

	double gradient;
	
public:
	Point3d(double nx, double ny, double nz) : x(nx), y(ny), z(nz) {
		gradient=0;
	}
	Point3d() {};
	~Point3d() {}

	double getx()  { return x; }
    double gety()  { return y; }
    double getz()  { return z; }

	double getgradient() { return gradient; }	
	void setgradient(double ng) { gradient = ng; }

	Normal3d normal[1];

	struct {
		double r;
		double g;
		double b;

		void setR(double nr) {
			r=nr;
		}
		void setG(double ng) {
			g=ng;
		}
		void setB(double nb) {
			b=nb;
		}

		double getR() { return r; }
		double getG() { return g; }
		double getB() { return b; }
	} GradientColour;
};

class Triangle3d {
private:
	int v1;
    int v2;
    int v3;

	bool drawTriangle;
public:
	Triangle3d(int nv1, int nv2, int nv3) : v1(nv1), v2(nv2), v3(nv3) {
		drawTriangle=true;
	}
	~Triangle3d() {}        
	
	int getv1()  { return v1; }
    int getv2()  { return v2; }
    int getv3()  { return v3; }

	void setDrawState(bool nstate) {
		drawTriangle = nstate;
	}
	bool getDrawState() { return drawTriangle; } 	
};

class Block {
private:
public:
	Block() {
	}
	std::vector<int> referance;
	void addReferance(int nref) { referance.push_back(nref); }
};

class CGrid {
private:
   int gridwidth;
   int gridheight;

   double tilesize;
   std::string gridfilename;
   std::string terrainfilename;   
   
public:
	CGrid(int ngridwidth, int ngridheight, double ntilesize, std::string ngridfilename, std::string nterrainfilename) : gridwidth(ngridwidth), gridheight(ngridheight), tilesize(ntilesize), gridfilename(ngridfilename), terrainfilename(nterrainfilename) {
		
		initializegrid();
		//outputdata();
	}
    ~CGrid() {}

	std::vector<Point3d> point3d;
	std::vector<Point3d> colourvector;

	std::vector<Triangle3d> triangle3d;
	SDL_Surface *surface_heightmap;
	SDL_Surface *surface_terrain;

	std::vector<Block> mesh_triangle;
	std::vector<Block> mesh_fan;
	std::vector<Block> mesh_strip;	

	std::vector<int> tempids;
	std::vector<Point3d> tempnormals;
	Point3d resultantnormal;

	std::vector<double> tempgradients;
	
	void addPoint3d(double nx, double ny, double nz) { point3d.push_back(Point3d(nx,ny,nz));          }
	void addTriangle3d(int nv1, int nv2, int nv3)    { triangle3d.push_back(Triangle3d(nv1,nv2,nv3)); }
	
	void addMeshTriangle()  { mesh_triangle.push_back(Block()); }
	void addMeshStrip()     { mesh_strip.push_back(Block());    }
	void addMeshFan()       { mesh_fan.push_back(Block());      }
	
	std::vector<double> yheightmap;

	void initializegrid() {
		HGTReader hgtreader = HGTReader(gridfilename, yheightmap);
		
		int heightmap_pos=0;
		for(int h=0; h<gridheight+1; h++) {
			for(int w=0; w<gridwidth+1; w++) {
				//addPoint3d(w*tilesize,yheightmap[heightmap_pos],h*tilesize);
				addPoint3d(w*tilesize, yheightmap[heightmap_pos],h*tilesize);
				heightmap_pos++;
			}
		}
		refinetriangles();
		vertexnormals();		
	}  	

	void refinetriangles() {

		int hcounter=1;
		int widthplusone=gridwidth+1;
		int it = (gridwidth*gridheight);


		bool south = false;
		bool east  = false;
		bool north = false;
		bool west  = false;

		bool left = false; 
		bool right = false;

		// level 0
		for(int a=0; a<it; a=a+2) {

			if((point3d[a].gety()+point3d[a+2].gety())/2==point3d[a+1].gety()) { // 2 and 3
				if((point3d[a+2].gety()+point3d[a+(gridwidth*2)+4].gety())/2==point3d[a+gridwidth+3].gety()) { // 4 and 8
					if((point3d[a].gety()+point3d[a+(gridwidth*2)+4].gety())/2==point3d[a+gridwidth+2].gety()) {
						//addTriangle3d(a+(gridwidth*2)+4,a+2,a);
						right=true;
					}
					else {
						//addTriangle3d(a,a+gridwidth+2,a+2);
						//addTriangle3d(a+2,a+gridwidth+2,a+(gridwidth*2)+4);
						south = true;
						east  = true;
					}
				}
				else {
					//addTriangle3d(a,a+gridwidth+2,a+2);
					//addTriangle3d(a+gridwidth+2,a+gridwidth+3,a+2);					  // 4
					//addTriangle3d(a+(gridwidth*2)+4,a+gridwidth+3,a+gridwidth+2);             // 8
					south     = true;					
				}
			}
			else {
				//addTriangle3d(a+gridwidth+2,a+1,a);									 // 2
				//addTriangle3d(a+2,a+1,a+gridwidth+2);								 // 3
				
				if((point3d[a+2].gety()+point3d[a+(gridwidth*2)+4].gety())/2==point3d[a+gridwidth+3].gety()) { // 4 and 8					
					//addTriangle3d(a+2,a+gridwidth+2,a+(gridwidth*2)+4);
					east = true;
				}
				else {
					//addTriangle3d(a+gridwidth+2,a+gridwidth+3,a+2);					  // 4
					//addTriangle3d(a+(gridwidth*2)+4,a+gridwidth+3,a+gridwidth+2);     // 8					
				}
			}			

			if((point3d[a+(gridwidth*2)+4].gety()+point3d[a+(gridwidth*2)+2].gety())/2==point3d[a+(gridwidth*2)+3].gety()) { // 7 and 6
				if((point3d[a+(gridwidth*2)+2].gety()+point3d[a].gety())/2==point3d[a+gridwidth+1].gety()) { // 5 and 1
					if((point3d[a+(gridwidth*2)+4].gety()+point3d[a].gety())/2==point3d[a+gridwidth+2].gety()) {
						//addTriangle3d(a,a+(gridwidth*2)+2,a+(gridwidth*2)+4);		
						left  = true;				
					}
					else {
						//addTriangle3d(a+(gridwidth*2)+4,a+gridwidth+2,a+(gridwidth*2)+2);
						//addTriangle3d(a+(gridwidth*2)+2,a+gridwidth+2,a);
						north = true;
						west  = true;
					}
				}
				else {
					//addTriangle3d(a+(gridwidth*2)+4,a+gridwidth+2,a+(gridwidth*2)+2);
					//addTriangle3d(a+gridwidth+2,a+gridwidth+1,a+(gridwidth*2)+2);        // 5
					//addTriangle3d(a,a+gridwidth+1,a+gridwidth+2);                        // 1
					north     = true;				
				}
				
			}
			else {
				//addTriangle3d(a+gridwidth+2,a+(gridwidth*2)+3,a+(gridwidth*2)+4);    // 7
				//addTriangle3d(a+(gridwidth*2)+2,a+(gridwidth*2)+3,a+gridwidth+2);    // 6				

				if((point3d[a+(gridwidth*2)+2].gety()+point3d[a].gety())/2==point3d[a+gridwidth+1].gety()) { // 5 and 1
					//addTriangle3d(a+(gridwidth*2)+2,a+gridwidth+2,a);
					west = true;
				}			
				else {
					//addTriangle3d(a+gridwidth+2,a+gridwidth+1,a+(gridwidth*2)+2);        // 5
					//addTriangle3d(a,a+gridwidth+1,a+gridwidth+2);                        // 1					
				}
			}
			
			// End of triangle refinement 
			// BUG WAS HERE!!!				

			if((left==true) && (right==true)) {
				// Then we have a triangle strip
				addMeshStrip();
				mesh_strip[mesh_strip.size()-1].addReferance(triangle3d.size());
				mesh_strip[mesh_strip.size()-1].addReferance(triangle3d.size()+1);

				//addTriangle3d(a+(gridwidth*2)+4,a+2,a);
				//addTriangle3d(a,a+(gridwidth*2)+2,a+(gridwidth*2)+4);	

				addTriangle3d(a+2,a,a+(gridwidth*2)+4);
				addTriangle3d(a+(gridwidth*2)+2,a+(gridwidth*2)+4,a);
			}
			else if((left==true) && (right==false)) {
				// Then left is a triangle
				// and right is a fan
				addMeshTriangle();
				mesh_triangle[mesh_triangle.size()-1].addReferance(triangle3d.size());

				// addTriangle3d(a,a+(gridwidth*2)+2,a+(gridwidth*2)+4);
				addTriangle3d(a,a+(gridwidth*2)+2,a+(gridwidth*2)+4);

				// construct the right hand-side fan
				// start from the top, 
				addMeshFan();

				if(east==true) { 
					// just one triangle for the east
					mesh_fan[mesh_fan.size()-1].addReferance(triangle3d.size());
					addTriangle3d(a+gridwidth+2,a+(gridwidth*2)+4,a+2);
					
				}
				else {
					// two triangles for the east
					mesh_fan[mesh_fan.size()-1].addReferance(triangle3d.size());
					mesh_fan[mesh_fan.size()-1].addReferance(triangle3d.size()+1);

					addTriangle3d(a+gridwidth+2,a+(gridwidth*2)+4,a+gridwidth+3);					 
					addTriangle3d(a+gridwidth+2,a+gridwidth+3,a+2);   

				}

				if(south==true) {
					// just one triangle for the south
					mesh_fan[mesh_fan.size()-1].addReferance(triangle3d.size());

					addTriangle3d(a+gridwidth+2,a+2,a);
				}
				else {
					// two triangles for the south
					mesh_fan[mesh_fan.size()-1].addReferance(triangle3d.size());
					mesh_fan[mesh_fan.size()-1].addReferance(triangle3d.size()+1);

					addTriangle3d(a+gridwidth+2,a+2,a+1);									
					addTriangle3d(a+gridwidth+2,a+1,a);							

				}

			}
			else if((left==false) && (right==true)) {
				// Then left is a fan
				// and right is a triangle
				addMeshTriangle();
				mesh_triangle[mesh_triangle.size()-1].addReferance(triangle3d.size());

				// addTriangle3d(a,a+(gridwidth*2)+2,a+(gridwidth*2)+4);
				addTriangle3d(a+(gridwidth*2)+4,a+2,a);

				// construct the left hand-side fan
				// start from the bottom
				addMeshFan();

				if(west==true) {
					// just one triangle for the west
					mesh_fan[mesh_fan.size()-1].addReferance(triangle3d.size());

					//addTriangle3d(a+gridwidth+2,a+(gridwidth*2)+2,a);
					addTriangle3d(a+gridwidth+2,a,a+(gridwidth*2)+2);

				}
				else {
					// two triangles for the west
					mesh_fan[mesh_fan.size()-1].addReferance(triangle3d.size());
					mesh_fan[mesh_fan.size()-1].addReferance(triangle3d.size()+1);

					//addTriangle3d(a+gridwidth+2,a+(gridwidth*2)+2,a+gridwidth+1);
					//addTriangle3d(a+gridwidth+2,a+gridwidth+1,a);  
					
					addTriangle3d(a+gridwidth+2,a,a+gridwidth+1); 
					addTriangle3d(a+gridwidth+2,a+gridwidth+1,a+(gridwidth*2)+2);
					                       
				}
				if(north==true) {
					// just one triangle for the north
					mesh_fan[mesh_fan.size()-1].addReferance(triangle3d.size());

					addTriangle3d(a+gridwidth+2,a+(gridwidth*2)+2,a+(gridwidth*2)+4);
				}
				else {
					// two triangles for the north
					mesh_fan[mesh_fan.size()-1].addReferance(triangle3d.size());
					mesh_fan[mesh_fan.size()-1].addReferance(triangle3d.size()+1);

					addTriangle3d(a+gridwidth+2,a+(gridwidth*2)+2,a+(gridwidth*2)+3);    
					addTriangle3d(a+gridwidth+2,a+(gridwidth*2)+3,a+(gridwidth*2)+4);    	

				}

			}
			else if((left==false) && (right==false)) {
				// Then everything is a fan!!!

				// construct the fan
				// start from the top, then the bottom

				addMeshFan();

				if(east==true) { 
					// just one triangle for the east
					mesh_fan[mesh_fan.size()-1].addReferance(triangle3d.size());
					addTriangle3d(a+gridwidth+2,a+(gridwidth*2)+4,a+2);
					
				}
				else {
					// two triangles for the east
					mesh_fan[mesh_fan.size()-1].addReferance(triangle3d.size());
					mesh_fan[mesh_fan.size()-1].addReferance(triangle3d.size()+1);

					addTriangle3d(a+gridwidth+2,a+(gridwidth*2)+4,a+gridwidth+3);					 
					addTriangle3d(a+gridwidth+2,a+gridwidth+3,a+2);   

				}

				if(south==true) {
					// just one triangle for the south
					mesh_fan[mesh_fan.size()-1].addReferance(triangle3d.size());

					addTriangle3d(a+gridwidth+2,a+2,a);
				}
				else {
					// two triangles for the south
					mesh_fan[mesh_fan.size()-1].addReferance(triangle3d.size());
					mesh_fan[mesh_fan.size()-1].addReferance(triangle3d.size()+1);

					addTriangle3d(a+gridwidth+2,a+2,a+1);									
					addTriangle3d(a+gridwidth+2,a+1,a);							

				}

				if(west==true) {
					// just one triangle for the west
					mesh_fan[mesh_fan.size()-1].addReferance(triangle3d.size());

					//addTriangle3d(a+gridwidth+2,a+(gridwidth*2)+2,a);
					addTriangle3d(a+gridwidth+2,a,a+(gridwidth*2)+2);

				}
				else {
					// two triangles for the west
					mesh_fan[mesh_fan.size()-1].addReferance(triangle3d.size());
					mesh_fan[mesh_fan.size()-1].addReferance(triangle3d.size()+1);

					//addTriangle3d(a+gridwidth+2,a+(gridwidth*2)+2,a+gridwidth+1);
					//addTriangle3d(a+gridwidth+2,a+gridwidth+1,a);  
					
					addTriangle3d(a+gridwidth+2,a,a+gridwidth+1); 
					addTriangle3d(a+gridwidth+2,a+gridwidth+1,a+(gridwidth*2)+2);
					                       
				}
				if(north==true) {
					// just one triangle for the north
					mesh_fan[mesh_fan.size()-1].addReferance(triangle3d.size());

					addTriangle3d(a+gridwidth+2,a+(gridwidth*2)+2,a+(gridwidth*2)+4);
				}
				else {
					// two triangles for the north
					mesh_fan[mesh_fan.size()-1].addReferance(triangle3d.size());
					mesh_fan[mesh_fan.size()-1].addReferance(triangle3d.size()+1);

					addTriangle3d(a+gridwidth+2,a+(gridwidth*2)+2,a+(gridwidth*2)+3);    
					addTriangle3d(a+gridwidth+2,a+(gridwidth*2)+3,a+(gridwidth*2)+4);    	

				}

			}
			south = false;
			east  = false;
			north = false;
			west  = false;

			left = false; 
			right = false;

			// this was the bug further up. ^^
			if(a==(widthplusone*hcounter)-3) {
				a=a+widthplusone+1;
				hcounter=hcounter+2;
			}
			
		}		
	}
	void vertexnormals() {
		
		int a=0;
		for(int h=0; h<gridheight+1; h++) {
			for(int w=0; w<gridwidth+1; w++) {

				//std::vector<int> tempids;
				//std::vector<Point3d> tempnormals;
				
				double fx=0; double fy=0; double fz=0;

				if((h==0) && (w==0)) { 
					// bottom row far left
					tempids.push_back(a);				// 0
					tempids.push_back(a+gridwidth+1);   // 1
					tempids.push_back(a+gridwidth+2);   // 2
					tempids.push_back(a+1);             // 3

					// Calculate normals for each of the tempids.
					// Specific assignment

					assignnormal(tempids[0],tempids[1],tempids[2]);
					assignnormal(tempids[0],tempids[2],tempids[3]);
					//assignnormal(tempids[0],tempids[3],4);
					//assignnormal(0,4,1);
					
					// Calculate the Sum of the normals 
					
					for(int i=0; i<2; i++) {
						fx=fx+tempnormals[i].getx();
						fy=fy+tempnormals[i].gety();
						fz=fz+tempnormals[i].getz();
					}
		
					fx=fx/2;
					fy=fy/2;
					fz=fz/2;

					// Assign final normal to the point.

					point3d[a].normal[0] = Normal3d(fx,fy,fz);		
				}
				else if((h==0) && (w<gridwidth)) { 
					// bottom row
					tempids.push_back(a);				// 0
					tempids.push_back(a+gridwidth+1);	// 1
					tempids.push_back(a+gridwidth+2);	// 2
					tempids.push_back(a+1);				// 3
					tempids.push_back(a-1);				// 4
					tempids.push_back(a+gridwidth);		// 5

					// Calculate normals for each of the tempids.
					// Specific assignment

					assignnormal(tempids[0],tempids[1],tempids[2]);
					assignnormal(tempids[0],tempids[2],tempids[3]);
					assignnormal(tempids[0],tempids[4],tempids[5]);
					assignnormal(tempids[0],tempids[5],tempids[1]);
					
					// Calculate the Sum of the normals 

					for(int i=0; i<4; i++) {
						fx=fx+tempnormals[i].getx();
						fy=fy+tempnormals[i].gety();
						fz=fz+tempnormals[i].getz();
					}
		
					fx=fx/4;
					fy=fy/4;
					fz=fz/4;

					// Assign final normal to the point.
					point3d[a].normal[0] = Normal3d(fx,fy,fz);
				}
				else if((h==0) && (w=gridwidth)) { 
					// bottom row far right
					tempids.push_back(a);				// 0
					tempids.push_back(a+gridwidth+1);   // 1
					tempids.push_back(a+gridwidth);		// 2
					tempids.push_back(a-1);				// 3
					

					// Calculate normals for each of the tempids.
					// Specific assignment
					
					assignnormal(tempids[0],tempids[1],tempids[2]);
					assignnormal(tempids[0],tempids[2],tempids[3]);
															
					// Calculate the Sum of the normals 

					for(int i=0; i<2; i++) {
						fx=fx+tempnormals[i].getx();
						fy=fy+tempnormals[i].gety();
						fz=fz+tempnormals[i].getz();
					}
		
					fx=fx/2;
					fy=fy/2;
					fz=fz/2;

					// Assign final normal to the point.
					point3d[a].normal[0] = Normal3d(fx,fy,fz);
				}
				else if((h>0) && (h<gridheight) && (w==0)) {
					// left column
					tempids.push_back(a);				// 0
					tempids.push_back(a+gridwidth+1);	// 1
					tempids.push_back(a+gridwidth+2);	// 2
					tempids.push_back(a+1);				// 3
					tempids.push_back(a-gridwidth);		// 4
					tempids.push_back(a-gridwidth-1);	// 5

					// Calculate normals for each of the tempids.
					// Specific assignment

					assignnormal(tempids[0],tempids[1],tempids[2]);
					assignnormal(tempids[0],tempids[2],tempids[3]);
					assignnormal(tempids[0],tempids[3],tempids[4]);
					assignnormal(tempids[0],tempids[4],tempids[5]);
										
					// Calculate the Sum of the normals 

					for(int i=0; i<4; i++) {
						fx=fx+tempnormals[i].getx();
						fy=fy+tempnormals[i].gety();
						fz=fz+tempnormals[i].getz();
					}
		
					fx=fx/4;
					fy=fy/4;
					fz=fz/4;

					// Assign final normal to the point.
					point3d[a].normal[0] = Normal3d(fx,fy,fz);
				}
				else if((h>0) && (h<gridheight) && (w<gridwidth)) {
					// everything in the middle
					tempids.push_back(a);				// 0
					tempids.push_back(a+gridwidth+1);	// 1
					tempids.push_back(a+gridwidth+2);	// 2
					tempids.push_back(a+1);				// 3
					tempids.push_back(a-gridwidth);		// 4
					tempids.push_back(a-gridwidth-1);	// 5
					tempids.push_back(a-gridwidth-2);	// 6
					tempids.push_back(a-1);				// 7
					tempids.push_back(a+gridwidth);		// 8

					// Calculate normals for each of the tempids.
					// Specific assignment

					assignnormal(tempids[0],tempids[1],tempids[2]);
					assignnormal(tempids[0],tempids[2],tempids[3]);
					assignnormal(tempids[0],tempids[3],tempids[4]);
					assignnormal(tempids[0],tempids[4],tempids[5]);
					assignnormal(tempids[0],tempids[5],tempids[6]);
					assignnormal(tempids[0],tempids[6],tempids[7]);
					assignnormal(tempids[0],tempids[7],tempids[8]);
					assignnormal(tempids[0],tempids[8],tempids[1]);

					// Calculate the Sum of the normals 

					for(int i=0; i<8; i++) {
						fx=fx+tempnormals[i].getx();
						fy=fy+tempnormals[i].gety();
						fz=fz+tempnormals[i].getz();
					}
		
					fx=fx/8;
					fy=fy/8;
					fz=fz/8;

					// Assign final normal to the point.
					point3d[a].normal[0] = Normal3d(fx,fy,fz);
				}
				else if((h>0) && (h<gridheight) && (w==gridwidth)) {
					// right column
					tempids.push_back(a);				// 0
					tempids.push_back(a+gridwidth+1);	// 1
					tempids.push_back(a+gridwidth);		// 2
					tempids.push_back(a-1);				// 3
					tempids.push_back(a-gridwidth-2);	// 4
					tempids.push_back(a-gridwidth-1);	// 5
					
					// Calculate normals for each of the tempids.
					// Specific assignment

					assignnormal(tempids[0],tempids[1],tempids[2]);
					assignnormal(tempids[0],tempids[2],tempids[3]);
					assignnormal(tempids[0],tempids[3],tempids[4]);
					assignnormal(tempids[0],tempids[4],tempids[5]);
										
					// Calculate the Sum of the normals 

					for(int i=0; i<4; i++) {
						fx=fx+tempnormals[i].getx();
						fy=fy+tempnormals[i].gety();
						fz=fz+tempnormals[i].getz();
					}
		
					fx=fx/4;
					fy=fy/4;
					fz=fz/4;

					// Assign final normal to the point.
					point3d[a].normal[0] = Normal3d(fx,fy,fz);
				}
				else if((h==gridheight) && (w==0)) {
					// top row far left
					tempids.push_back(a);				// 0
					tempids.push_back(a-gridwidth-1);	// 1
					tempids.push_back(a-gridwidth);		// 2
					tempids.push_back(a+1);				// 3				

					// Calculate normals for each of the tempids.
					// Specific assignment

					assignnormal(tempids[0],tempids[1],tempids[2]);
					assignnormal(tempids[0],tempids[2],tempids[3]);
															
					// Calculate the Sum of the normals 

					for(int i=0; i<2; i++) {
						fx=fx+tempnormals[i].getx();
						fy=fy+tempnormals[i].gety();
						fz=fz+tempnormals[i].getz();
					}
		
					fx=fx/2;
					fy=fy/2;
					fz=fz/2;

					// Assign final normal to the point.
					point3d[a].normal[0] = Normal3d(fx,fy,fz);
				}
				else if((h==gridheight) && (w>0) && (w<gridwidth)) {
					// top row 
					tempids.push_back(a);				// 0
					tempids.push_back(a+1);				// 1
					tempids.push_back(a-gridwidth);		// 2
					tempids.push_back(a-gridwidth-1);	// 3
					tempids.push_back(a-gridwidth-2);	// 4
					tempids.push_back(a-1);				// 5

					// Calculate normals for each of the tempids.
					// Specific assignment

					assignnormal(tempids[0],tempids[1],tempids[2]);
					assignnormal(tempids[0],tempids[2],tempids[3]);
					assignnormal(tempids[0],tempids[3],tempids[4]);
					assignnormal(tempids[0],tempids[4],tempids[5]);
															
					// Calculate the Sum of the normals 

					for(int i=0; i<4; i++) {
						fx=fx+tempnormals[i].getx();
						fy=fy+tempnormals[i].gety();
						fz=fz+tempnormals[i].getz();
					}
		
					fx=fx/4;
					fy=fy/4;
					fz=fz/4;

					// Assign final normal to the point.
					point3d[a].normal[0] = Normal3d(fx,fy,fz);
				}
				else if((h==gridheight) && (w==gridwidth)) {
					// top row far right
					tempids.push_back(a);				// 0
					tempids.push_back(a-1);				// 1
					tempids.push_back(a-gridwidth-2);	// 2
					tempids.push_back(a-gridwidth-1);	// 3					

					// Calculate normals for each of the tempids.
					// Specific assignment

					assignnormal(tempids[0],tempids[1],tempids[2]);
					assignnormal(tempids[0],tempids[2],tempids[3]);
															
					// Calculate the Sum of the normals 

					for(int i=0; i<2; i++) {
						fx=fx+tempnormals[i].getx();
						fy=fy+tempnormals[i].gety();
						fz=fz+tempnormals[i].getz();
					}
		
					fx=fx/2;
					fy=fy/2;
					fz=fz/2;

					// Assign final normal to the point.
					point3d[a].normal[0] = Normal3d(fx,fy,fz);
				}		
				
				tempids.clear();
				tempnormals.clear();
				tempgradients.clear();

				fx=0, fy=0; fz=0;
				a++;
			}
		}		
		// end of assignment				
	}
	void normals(int v1, int v2, int v3) {
		
	        // v1 centre point
	        // v2 lhs
	        // v3 rhs
	        
	        // The maths here needs checking. Although this method is correct 
	        // it might produce unintended results if data is passed incorretly
	        
	        // example where v1 10, v2 20, v3 20
	        // u = v2-v1  
	        // u = 10
	        
	        // v = v3-v1
	        // v = 10
	        
	        // if v1 where 20 and v2 10
	        // u = -10
	        // v = -10
	        
	        // dispite being the same vector?
	        
	        // u = (v2+v1) /2 
	        // v = (v3+v1) /2 
	        
	        // solves this?
	        
		double u_x = point3d[v2].getx()-point3d[v1].getx();
		double u_y = point3d[v2].gety()-point3d[v1].gety();
		double u_z = point3d[v2].getz()-point3d[v1].getz();

		double v_x = point3d[v3].getx()-point3d[v1].getx();
		double v_y = point3d[v3].gety()-point3d[v1].gety();
		double v_z = point3d[v3].getz()-point3d[v1].getz();
		
		double nx = (u_y*v_z)-(u_z*v_y);
		double ny = (u_z*v_x)-(u_x*v_z);
		double nz = (u_x*v_y)-(u_y*v_x);
		
		double l  = sqrt((nx*nx)+(ny*ny)+(nz*nz));

		nx /=l;
		ny /=l;
		nz /=l;

		resultantnormal = Point3d(nx,ny,nz);
			
	}
	void assignnormal(int v1, int v2, int v3) {
		normals(v1,v2,v3);
		tempnormals.push_back(resultantnormal);
	}
	void calculategradient(int v1, int v2) {
		double dy = (point3d[v2].gety()-point3d[v1].gety())*10000;
		tempgradients.push_back(dy);
	}	
	
	int draw(SpaceCamera &spacecamera) {
		
		
		int num = glGenLists(1);		
		glNewList(num,GL_COMPILE);			
		
		// the triangle selected is the referance provided by the referance ary vector.
		glBegin(GL_TRIANGLES);
		for(int num_triangles=0; num_triangles<mesh_triangle.size(); num_triangles++) {
			// mesh will only ever have 1 triangle
			int i=0;
			//for(int i=0; i<mesh_triangle[num_triangles].referance.size(); i++) {
				//glColor3d(point3d[triangle3d[mesh_triangle[num_triangles].referance[i]].getv1()].GradientColour.getR(),     point3d[triangle3d[mesh_triangle[num_triangles].referance[i]].getv1()].GradientColour.getG(),    point3d[triangle3d[mesh_triangle[num_triangles].referance[i]].getv1()].GradientColour.getB());
				glNormal3d(point3d[triangle3d[mesh_triangle[num_triangles].referance[i]].getv1()].normal[0].getx(), point3d[triangle3d[mesh_triangle[num_triangles].referance[i]].getv1()].normal[0].gety(), point3d[triangle3d[mesh_triangle[num_triangles].referance[i]].getv1()].normal[0].getz());
				glVertex3d(point3d[triangle3d[mesh_triangle[num_triangles].referance[i]].getv1()].getx(),           point3d[triangle3d[mesh_triangle[num_triangles].referance[i]].getv1()].gety(),           point3d[triangle3d[mesh_triangle[num_triangles].referance[i]].getv1()].getz());
			
				//glColor3d(point3d[triangle3d[mesh_triangle[num_triangles].referance[i]].getv2()].GradientColour.getR(),     point3d[triangle3d[mesh_triangle[num_triangles].referance[i]].getv2()].GradientColour.getG(),    point3d[triangle3d[mesh_triangle[num_triangles].referance[i]].getv2()].GradientColour.getB());
				glNormal3d(point3d[triangle3d[mesh_triangle[num_triangles].referance[i]].getv2()].normal[0].getx(), point3d[triangle3d[mesh_triangle[num_triangles].referance[i]].getv2()].normal[0].gety(), point3d[triangle3d[mesh_triangle[num_triangles].referance[i]].getv2()].normal[0].getz());
				glVertex3d(point3d[triangle3d[mesh_triangle[num_triangles].referance[i]].getv2()].getx(),           point3d[triangle3d[mesh_triangle[num_triangles].referance[i]].getv2()].gety(),           point3d[triangle3d[mesh_triangle[num_triangles].referance[i]].getv2()].getz());

				//glColor3d(point3d[triangle3d[mesh_triangle[num_triangles].referance[i]].getv3()].GradientColour.getR(),     point3d[triangle3d[mesh_triangle[num_triangles].referance[i]].getv3()].GradientColour.getG(),    point3d[triangle3d[mesh_triangle[num_triangles].referance[i]].getv3()].GradientColour.getB());
				glNormal3d(point3d[triangle3d[mesh_triangle[num_triangles].referance[i]].getv3()].normal[0].getx(), point3d[triangle3d[mesh_triangle[num_triangles].referance[i]].getv3()].normal[0].gety(), point3d[triangle3d[mesh_triangle[num_triangles].referance[i]].getv3()].normal[0].getz());
				glVertex3d(point3d[triangle3d[mesh_triangle[num_triangles].referance[i]].getv3()].getx(),           point3d[triangle3d[mesh_triangle[num_triangles].referance[i]].getv3()].gety(),           point3d[triangle3d[mesh_triangle[num_triangles].referance[i]].getv3()].getz());
			//}
		}
		glEnd();		
		
		for(int num_strips=0; num_strips<mesh_strip.size(); num_strips++) {
			glBegin(GL_TRIANGLE_STRIP);
			// run for 2 loops, as there can only be two triangles per strip
				//glColor3d(point3d[triangle3d[mesh_strip[num_strips].referance[0]].getv1()].GradientColour.getR(), point3d[triangle3d[mesh_strip[num_strips].referance[0]].getv1()].GradientColour.getG(), point3d[triangle3d[mesh_strip[num_strips].referance[0]].getv1()].GradientColour.getB());
				glNormal3d(point3d[triangle3d[mesh_strip[num_strips].referance[0]].getv1()].normal[0].getx(), point3d[triangle3d[mesh_strip[num_strips].referance[0]].getv1()].normal[0].gety(), point3d[triangle3d[mesh_strip[num_strips].referance[0]].getv1()].normal[0].getz());
				glVertex3d(point3d[triangle3d[mesh_strip[num_strips].referance[0]].getv1()].getx(),           point3d[triangle3d[mesh_strip[num_strips].referance[0]].getv1()].gety(),           point3d[triangle3d[mesh_strip[num_strips].referance[0]].getv1()].getz());
			
				//glColor3d(point3d[triangle3d[mesh_strip[num_strips].referance[0]].getv2()].GradientColour.getR(), point3d[triangle3d[mesh_strip[num_strips].referance[0]].getv2()].GradientColour.getG(), point3d[triangle3d[mesh_strip[num_strips].referance[0]].getv2()].GradientColour.getB());
				glNormal3d(point3d[triangle3d[mesh_strip[num_strips].referance[0]].getv2()].normal[0].getx(), point3d[triangle3d[mesh_strip[num_strips].referance[0]].getv2()].normal[0].gety(), point3d[triangle3d[mesh_strip[num_strips].referance[0]].getv2()].normal[0].getz());
				glVertex3d(point3d[triangle3d[mesh_strip[num_strips].referance[0]].getv2()].getx(),           point3d[triangle3d[mesh_strip[num_strips].referance[0]].getv2()].gety(),           point3d[triangle3d[mesh_strip[num_strips].referance[0]].getv2()].getz());
			
				//glColor3d(point3d[triangle3d[mesh_strip[num_strips].referance[0]].getv3()].GradientColour.getR(), point3d[triangle3d[mesh_strip[num_strips].referance[0]].getv3()].GradientColour.getG(), point3d[triangle3d[mesh_strip[num_strips].referance[0]].getv3()].GradientColour.getB());
				glNormal3d(point3d[triangle3d[mesh_strip[num_strips].referance[0]].getv3()].normal[0].getx(), point3d[triangle3d[mesh_strip[num_strips].referance[0]].getv3()].normal[0].gety(), point3d[triangle3d[mesh_strip[num_strips].referance[0]].getv3()].normal[0].getz());
				glVertex3d(point3d[triangle3d[mesh_strip[num_strips].referance[0]].getv3()].getx(),           point3d[triangle3d[mesh_strip[num_strips].referance[0]].getv3()].gety(),           point3d[triangle3d[mesh_strip[num_strips].referance[0]].getv3()].getz());
			
				//glColor3d(point3d[triangle3d[mesh_strip[num_strips].referance[1]].getv1()].GradientColour.getR(), point3d[triangle3d[mesh_strip[num_strips].referance[1]].getv1()].GradientColour.getG(), point3d[triangle3d[mesh_strip[num_strips].referance[1]].getv1()].GradientColour.getB());
				glNormal3d(point3d[triangle3d[mesh_strip[num_strips].referance[1]].getv1()].normal[0].getx(), point3d[triangle3d[mesh_strip[num_strips].referance[1]].getv1()].normal[0].gety(), point3d[triangle3d[mesh_strip[num_strips].referance[1]].getv1()].normal[0].getz());
				glVertex3d(point3d[triangle3d[mesh_strip[num_strips].referance[1]].getv1()].getx(),           point3d[triangle3d[mesh_strip[num_strips].referance[1]].getv1()].gety(),           point3d[triangle3d[mesh_strip[num_strips].referance[1]].getv1()].getz());
			
				glEnd();
		}	
		
		for(int num_fans=0; num_fans<mesh_fan.size(); num_fans++) {		
			glBegin(GL_TRIANGLE_FAN);
			// no fixed value, so run for mesh size
				//glColor3d(point3d[triangle3d[mesh_fan[num_fans].referance[0]].getv1()].GradientColour.getR(), point3d[triangle3d[mesh_fan[num_fans].referance[0]].getv1()].GradientColour.getG(), point3d[triangle3d[mesh_fan[num_fans].referance[0]].getv1()].GradientColour.getB());
				glNormal3d(point3d[triangle3d[mesh_fan[num_fans].referance[0]].getv1()].normal[0].getx(), point3d[triangle3d[mesh_fan[num_fans].referance[0]].getv1()].normal[0].gety(), point3d[triangle3d[mesh_fan[num_fans].referance[0]].getv1()].normal[0].getz());
				glVertex3d(point3d[triangle3d[mesh_fan[num_fans].referance[0]].getv1()].getx(),           point3d[triangle3d[mesh_fan[num_fans].referance[0]].getv1()].gety(),           point3d[triangle3d[mesh_fan[num_fans].referance[0]].getv1()].getz());
				
				//glColor3d(point3d[triangle3d[mesh_fan[num_fans].referance[0]].getv2()].GradientColour.getR(), point3d[triangle3d[mesh_fan[num_fans].referance[0]].getv2()].GradientColour.getG(), point3d[triangle3d[mesh_fan[num_fans].referance[0]].getv2()].GradientColour.getB());
				glNormal3d(point3d[triangle3d[mesh_fan[num_fans].referance[0]].getv2()].normal[0].getx(), point3d[triangle3d[mesh_fan[num_fans].referance[0]].getv2()].normal[0].gety(), point3d[triangle3d[mesh_fan[num_fans].referance[0]].getv2()].normal[0].getz());
				glVertex3d(point3d[triangle3d[mesh_fan[num_fans].referance[0]].getv2()].getx(),           point3d[triangle3d[mesh_fan[num_fans].referance[0]].getv2()].gety(),           point3d[triangle3d[mesh_fan[num_fans].referance[0]].getv2()].getz());
			
				//glColor3d(point3d[triangle3d[mesh_fan[num_fans].referance[0]].getv3()].GradientColour.getR(), point3d[triangle3d[mesh_fan[num_fans].referance[0]].getv3()].GradientColour.getG(), point3d[triangle3d[mesh_fan[num_fans].referance[0]].getv3()].GradientColour.getB());
				glNormal3d(point3d[triangle3d[mesh_fan[num_fans].referance[0]].getv3()].normal[0].getx(), point3d[triangle3d[mesh_fan[num_fans].referance[0]].getv3()].normal[0].gety(), point3d[triangle3d[mesh_fan[num_fans].referance[0]].getv3()].normal[0].getz());
				glVertex3d(point3d[triangle3d[mesh_fan[num_fans].referance[0]].getv3()].getx(),           point3d[triangle3d[mesh_fan[num_fans].referance[0]].getv3()].gety(),           point3d[triangle3d[mesh_fan[num_fans].referance[0]].getv3()].getz());
				
			for(int i=1; i<mesh_fan[num_fans].referance.size(); i++) {				
				//glColor3d(point3d[triangle3d[mesh_fan[num_fans].referance[i]].getv3()].GradientColour.getR(), point3d[triangle3d[mesh_fan[num_fans].referance[i]].getv3()].GradientColour.getG(), point3d[triangle3d[mesh_fan[num_fans].referance[i]].getv3()].GradientColour.getB());
				glNormal3d(point3d[triangle3d[mesh_fan[num_fans].referance[i]].getv3()].normal[0].getx(), point3d[triangle3d[mesh_fan[num_fans].referance[i]].getv3()].normal[0].gety(), point3d[triangle3d[mesh_fan[num_fans].referance[i]].getv3()].normal[0].getz());
				glVertex3d(point3d[triangle3d[mesh_fan[num_fans].referance[i]].getv3()].getx(),           point3d[triangle3d[mesh_fan[num_fans].referance[i]].getv3()].gety(),           point3d[triangle3d[mesh_fan[num_fans].referance[i]].getv3()].getz());
			}		
			glEnd();
		}
		glEndList();	
		
		return num;
	}

	void outputdata() {
		std::ofstream datafile;
		datafile.open("data.txt");
		datafile << "Points: " << point3d.size() << "    Triangles: " << triangle3d.size() << std::endl;
		
			double initialv = 65535.0/90;
			double degrees  = M_PI/2;
			double finalv   = (sin(degrees)*initialv)*2.25;	//225
			datafile << finalv;

		
		for(int i=0; i<point3d.size(); i++) {
			datafile << i << ":  " << point3d[i].normal[0].getx() << point3d[i].normal[0].gety() << point3d[i].normal[0].getz() << std::endl;		
			datafile << "        " << point3d[i].GradientColour.getR() << ", " << point3d[i].GradientColour.getG() << ", " << point3d[i].GradientColour.getB() << std::endl;		
		
		}
		
		/*
		for(int i=0; i<triangle3d.size(); i++) {
			datafile << i << ":  " << triangle3d[i].getv1() << triangle3d[i].getv2() << triangle3d[i].getv3() << std::endl;		
		}
		for(int i=0; i<yheightmap.size(); i++) {
			datafile << yheightmap[i] << ", ";
		}		
		for(int i=0; i<colourvector.size(); i++) {
			datafile << "RGB: " << i << " value is-> "  << colourvector[i].getx() << ", " << colourvector[i].gety() << ", " << colourvector[i].getz() << std::endl;
		}
		*/

		datafile.close();
	}

	// Redundant in current implementation
	
	Uint32 getPixels32(SDL_Surface *newsurface, int x, int y) {
		/*int bpp = newsurface->format->BytesPerPixel;
		Uint32 *pixels = (Uint32* )newsurface->pixels;		
		
		return pixels[(y*newsurface->pitch/4)+x];	*/

		int bpp = newsurface->format->BytesPerPixel;
        /* Here p is the address to the pixel we want to retrieve */
        Uint8 *p = (Uint8 *)newsurface->pixels + y * newsurface->pitch + x * bpp;

        switch(bpp) {
        case 1:
            return *p;

        case 2:
            return *(Uint16 *)p;

        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;

        case 4:
            return *(Uint32 *)p;

        default:
            return 0;       /* shouldn't happen, but avoids warnings */
        }
	}
	void getHeights(SDL_Surface *newsurface, std::string filename) {
		newsurface = SDL_LoadBMP(filename.c_str());
		Uint8 r,g,b;
		int k=0;
		        
		for(int j=0; j<newsurface->h; j++) {
		    for(int i=0; i<newsurface->w; i++) {
				SDL_LockSurface(newsurface);
				SDL_GetRGB(getPixels32(newsurface,i,j),newsurface->format,&r,&g,&b);
				SDL_UnlockSurface(newsurface);
				double rvalue = r/255.0;
				//float gvalue = g;
				//float bvalue = b;			
				
				double rad=M_PI/2;	
				double yrad =(sin(rad)*rvalue)*225;
				

				yheightmap.push_back(yrad);
				k++;
		    }
		}
		SDL_FreeSurface(newsurface);
	}    
	void getTerrain(SDL_Surface *newsurface, std::string filename) {
		newsurface = SDL_LoadBMP(filename.c_str());
		Uint8 r,g,b;
		int k=0;
		        
		for(int j=0; j<newsurface->h; j++) {
		    for(int i=0; i<newsurface->w; i++) {
				SDL_LockSurface(newsurface);
				SDL_GetRGB(getPixels32(newsurface,i,j),newsurface->format,&r,&g,&b);
				SDL_UnlockSurface(newsurface);
				double rvalue = r/255.0;
				double gvalue = g/255.0;
				double bvalue = b/255.0;		

				colourvector.push_back(Point3d(rvalue,gvalue,bvalue));

				k++;
		    }
		}
		SDL_FreeSurface(newsurface);
	}  
};



#endif
