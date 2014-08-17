#include "SpaceCamera.h"

SpaceCamera::SpaceCamera(double ncamx,double ncamy,double ncamz,
						 double ncampitch,double ncamyaw)
						 : camX(ncamx),camY(ncamy),camZ(ncamz),
						   camPitch(ncampitch),camYaw(ncamyaw){ 
}

SpaceCamera::SpaceCamera() {
	camX=0.0;
	camY=0.0;
	camZ=0.0;

    camPitch=0.0;
	camYaw=0.0;
}
void SpaceCamera::lockCamera() {	
	if(camPitch>90)
		camPitch=90;
	if(camPitch<-90)
		camPitch=-90;
	if(camYaw<0.0)
		camYaw+=360.0;
	if(camYaw>360.0)
		camYaw-=360;
}

void SpaceCamera::moveCamera(double dist,double dir) {
	double rad=(camYaw+dir)*M_PI/180.0;	
	camX-=sin(rad)*dist;	           
	camZ-=cos(rad)*dist;	         
}

void SpaceCamera::moveCameraUp(double dist,double dir) {
	double rad=(camPitch+dir)*M_PI/180.0;
	camY+=sin(rad)*dist;	
}

void SpaceCamera::Control(SDL_Window *window,double movevel,double mousevel,bool mi) {
	if(mi)	
	{
		int MidX=320;	
		int MidY=240;
		SDL_ShowCursor(SDL_DISABLE);	
		int tmpx,tmpy;
		SDL_GetMouseState(&tmpx,&tmpy);	
		camYaw+=mousevel*(MidX-tmpx);	
		camPitch+=mousevel*(MidY-tmpy);	
		//lockCamera();

		SDL_WarpMouseInWindow(window,MidX,MidY);
		
		const Uint8 *state = SDL_GetKeyboardState(NULL);

		if(state[SDL_SCANCODE_Q]) {
			if(camPitch!=90 && camPitch!=-90) {	
				moveCamera(movevel,0.0);	
				moveCameraUp(movevel,0.0);	  
			}
		}
		else if(state[SDL_SCANCODE_S]) {
			if(camPitch!=90 && camPitch!=-90) {
				moveCamera(movevel,180.0);
				moveCameraUp(movevel,180.0);  
			}
		}	
		if(state[SDL_SCANCODE_A]) {	
			moveCamera(movevel,90.0);  
		}
		else if(state[SDL_SCANCODE_D]) {
			moveCamera(movevel,270);
		}
	}
	glRotated(-camPitch,1.0,0.0,0.0);	
	glRotated(-camYaw,0.0,1.0,0.0);
}

void SpaceCamera::UpdateCamera() {
	glTranslated(-camX,-camY,-camZ);	
}

double SpaceCamera::getCamX() {
	return camX;
}
double SpaceCamera::getCamY() {
	return camY;
}
double SpaceCamera::getCamZ() {
	return camZ;
}
double SpaceCamera::getCamPitch() {
	return camPitch;
}
double SpaceCamera::getCamYaw() {
	return camYaw;
}

