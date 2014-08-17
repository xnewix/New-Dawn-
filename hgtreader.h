#ifndef HGTREADER_H
#define HGTREADER_H

#include "SDL_Video.h"
#include <iostream>

class HGTReader {
private:
	std::string filename;

public:
	HGTReader(std::string nfilename, std::vector<double>& tmp) : filename(nfilename) {			
		tmp = ReadHGT(tmp);		
	}
	~HGTReader() {}    
	
	std::vector<double> ReadHGT(std::vector<double>& tmp) {
				
		std::ifstream file(filename, std::ios::in|std::ios::binary);
		if(!file) {		
			
		}

		unsigned char buffer[2];
		for(int i =0; i<(1201*1201); i++) {
            if(!file.read(reinterpret_cast<char*>(buffer), sizeof(buffer))) {
                std::cout << "Error reading file!" << std::endl;                
            }
			tmp.push_back(buffer[0] << 8 | buffer[1]);
        }
		file.close();

		ScaleElevation(tmp);

		return tmp;
	}
	
	void ScaleElevation(std::vector<double>& tmp) {
		for(int i=0;i<tmp.size();i++) {
			if(tmp[i]>=9000) {
				tmp[i]=tmp[i]-65535.0;
			}
			double initialv = tmp[i]/90;
			double degrees  = M_PI/2;
			double finalv   = (sin(degrees)*initialv)*2.25;	//225
			tmp[i]   = finalv;
		}		
	}

	void outputdata() {
		std::ofstream datafile;
		datafile.open("heightvaluesdata.txt");
		//datafile << "Heightdata: " << heightdata.size() << std::endl;
		/*
		for(int i=0; i<heightdata.size(); i++) {
			datafile << i << ":  " << heightdata[i] << std::endl;		
		}
		*/
		datafile.close();
	}

};

class Node {
private:
	unsigned int id;
	double lat;
	double lon;
public:
	Node(unsigned int nid, double nlat, double nlon) : id(nid), lat(nlat), lon(nlon) {
	}
	Node()  {};
	~Node() {}

	unsigned int getid()  { return id;  }
	double       getlat() { return lat; }
	double       getlon() { return lon; }
};
class Way {
private:
	unsigned int id;
	bool areastate;

	double colourvalues[3];
	

public:
	Way(unsigned int nid) : id(nid) {
		colourvalues[0]=0.5;
		colourvalues[1]=0.5;
		colourvalues[2]=0.5;
		areastate=false;
	}
	Way()  {};
	~Way() {};

	std::vector<unsigned int> noderefs;
	std::vector<std::string> tags;

	void addRef(unsigned int nref) {
		noderefs.push_back(nref);
	}
	void setareastate(bool nstate) {
		areastate = nstate;
	}
	void setcolourvalues(double nvalue, int pos) {
		colourvalues[pos] = nvalue;
	}
	unsigned int getId() { return id; }
	bool getareastate() { return areastate; }
	double getcolourvalues(int pos) {
		return colourvalues[pos];
	}

};

class MapOSM {
private:
	std::string filename;
	std::string line;	
	double minlat;
	double minlon;
public:
	MapOSM(std::string nfilename) : filename(nfilename) {		
		readfile();
	}
	MapOSM()  {};
	~MapOSM() {}

	std::vector<Node> nodes;
	std::vector<Way>  way;

	void addNode(unsigned int nid, double nlat, double nlon) {
		nodes.push_back(Node(nid,nlat,nlon));
	}
	void addWay(unsigned int nid) {
		way.push_back(Way(nid));
	}	
	void readfile() {
		std::ifstream file(filename);

		unsigned int tmpid;
		double       tmplat;
		double	     tmplon;

		bool         foundway=false;
		unsigned int tmpwayid;

		int          wayparsing=0;
		unsigned int tmpwaynodeid;

		while(getline(file,line)) {
			
			if(line.find("<bounds") !=std::string::npos) {
				int pos1   = line.find("minlat=");
				int pos2   = line.find('"',pos1+8);
				int dif1  = pos2-pos1;

				std::string latstring  = line.substr(pos1+8,dif1-8);
				std::istringstream convertlat(latstring);
			
				if(!(convertlat >> minlat)) { minlat = 0; }

				int pos3  = line.find("minlon=");
				int pos4  = line.find('"',pos3+8);
				int dif2  = pos4-pos3;

				std::string lonstring = line.substr(pos3+8,dif2-8);
				std::istringstream convertlon(lonstring);
			
				if(!(convertlon >> minlon)) { minlon = 0; }

				std::cout << pos1 << std::endl;	
				std::cout << pos2 << std::endl;	
				std::cout << latstring << std::endl;				

				std::cout << pos3 << std::endl;	
				std::cout << pos4 << std::endl;	
				std::cout << lonstring << std::endl;				
			}

			if(line.find("<node") !=std::string::npos) {
				int pos1   = line.find("id=");
				int pos2   = line.find('"',pos1+4);
				int dif1  = pos2-pos1;

				std::string idstring  = line.substr(pos1+4,dif1-4);
				std::istringstream convertid(idstring);
			
				if(!(convertid >> tmpid)) { tmpid = 0; }

				int pos3  = line.find("lat=");
				int pos4  = line.find('"',pos3+5);
				int dif2  = pos4-pos3;

				std::string latstring = line.substr(pos3+5,dif2-5);
				std::istringstream convertlat(latstring);
			
				if(!(convertlat >> tmplat)) { tmplat = 0; }

				int pos5  = line.find("lon=");
				int pos6  = line.find('"',pos5+5);
				int dif3  = pos4-pos3;

				std::string lonstring = line.substr(pos5+5,dif3-5);
				std::istringstream convertlon(lonstring);
			
				if(!(convertlon >> tmplon)) { tmplon = 0; }

				addNode(tmpid,tmplat,tmplon);			
			}	
			if(line.find("<way") !=std::string::npos) {
				foundway = true;

				int pos1   = line.find("id=");
				int pos2   = line.find('"',pos1+4);
				int dif1  = pos2-pos1;

				std::string idstring  = line.substr(pos1+4,dif1-4);
				std::istringstream convertid(idstring);
			
				if(!(convertid >> tmpwayid)) { tmpwayid = 0; }

				addWay(tmpwayid);
			}
			if(line.find("</way>") !=std::string::npos) {
				foundway = false;
				wayparsing++;
			}
			if(foundway==true && line.find("<nd") !=std::string::npos) {
				int pos1   = line.find("ref=");
				int pos2   = line.find('"',pos1+5);
				int dif1   = pos2-pos1;

				std::string idstring  = line.substr(pos1+5,dif1-5);
				std::istringstream convertid(idstring);
			
				if(!(convertid >> tmpwaynodeid)) { tmpwaynodeid = 0; }

				way[wayparsing].addRef(tmpwaynodeid);
			}
			if(foundway==true && line.find("<tag") !=std::string::npos) {
				int pos1    = line.find("building");
				int pos2    = line.find("yes");

				if((pos1 !=std::string::npos && pos1>0) && (pos2 !=std::string::npos && pos2>0)) {
					way[wayparsing].setareastate(true);
					way[wayparsing].setcolourvalues(0.647059,0);
					way[wayparsing].setcolourvalues(0.164706,1);
					way[wayparsing].setcolourvalues(0.164706,2);
				}
			}
			if(foundway==true && line.find("<tag") !=std::string::npos) {
				int pos1    = line.find("highway");
				way[wayparsing].tags.push_back("highway");

				if(pos1 !=std::string::npos && pos1>0) {
					//way[wayparsing].setareastate(true);
					way[wayparsing].setcolourvalues(0.5,0);
					way[wayparsing].setcolourvalues(0.5,1);
					way[wayparsing].setcolourvalues(0.0,2);
				}
			}
		}
		file.close();

		for(int i=0; i<way.size(); i++) {
			std::cout << "ID: " << way[i].getId() << std::endl;
			for(int j=0; j<way[i].noderefs.size(); j++) {
				std::cout << "    : " << j << " : " << way[i].noderefs[j] << std::endl;
				for(int h=0; h<nodes.size(); h++) {
					if(way[i].noderefs[j]==nodes[h].getid()) {
						way[i].noderefs[j]=h;
					}
				}
			}
		}

		/*
		// process the roads
		for(int i=0; i<way.size(); i++) {
			for(int j=0; j<way[i].tags.size(); j++) {
				if(way[i].tags[j]=="highway") {
					// established that this is a highway....
					for(int k=0; k<way[i].noderefs.size()-1;k++) {
						// find the correct points
						double ax = nodes[way[i].noderefs[k]].getlat(); // x
						double ay = nodes[way[i].noderefs[k]].getlon(); // y

						double bx = nodes[way[i].noderefs[k+1]].getlat(); // x
						double by = nodes[way[i].noderefs[k+1]].getlon(); // y

						//(ax+bx)/2 
					}
				}
			}
		}
		*/

	} //end of readfile
	void draw() {		
		for(int i=0; i<way.size(); i++) {
			if(way[i].getareastate()==true) {
				glColor3d(way[i].getcolourvalues(0),way[i].getcolourvalues(1),way[i].getcolourvalues(2));
				glBegin(GL_TRIANGLE_FAN);
					std::cout << "WAY: " << i << way[i].getareastate() << std::endl;	
					glVertex3d((nodes[way[i].noderefs[0]].getlat()-minlat)*10000,0.0,(nodes[way[i].noderefs[0]].getlon()-minlon)*10000);							
					
					for(int j=1; j<way[i].noderefs.size(); j++) {			
						std::cout << nodes[way[i].noderefs[j]].getlat() << ", " << nodes[way[i].noderefs[j]].getlon() << std::endl;
						glVertex3d((nodes[way[i].noderefs[j]].getlat()-minlat)*10000,0.0,(nodes[way[i].noderefs[j]].getlon()-minlon)*10000);							
					}
				glEnd();
			}
			else if(way[i].getareastate()==false) {
				glColor3d(way[i].getcolourvalues(0),way[i].getcolourvalues(1),way[i].getcolourvalues(2));
				glBegin(GL_LINE_STRIP);
					std::cout << "WAY: " << i << way[i].getareastate() << std::endl;				
					for(int j=0; j<way[i].noderefs.size(); j++) {			
						std::cout << nodes[way[i].noderefs[j]].getlat() << ", " << nodes[way[i].noderefs[j]].getlon() << std::endl;
						glVertex3d((nodes[way[i].noderefs[j]].getlat()-minlat)*10000,0.0,(nodes[way[i].noderefs[j]].getlon()-minlon)*10000);							
					}
				glEnd();
			}
		}
	}
};

#endif 
