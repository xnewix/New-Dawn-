#ifndef SPACE_CAMERA_H
#define SPACE_CAMERA_H

#include "SDL_Video.h"

// Functions for Rolling plane are still needed here.

class SpaceCamera {
private:
	double camX,camY,camZ;
	double camPitch;
	double camYaw;
	double camRoll;
public:
	SpaceCamera(double ncamx,double ncamy,double ncamz,double ncampitch,double ncamyaw);
	SpaceCamera();
	void lockCamera();
	void moveCamera(double dist,double dir);
	void moveCameraUp(double dist,double dir);
	void Control(SDL_Window *window,double movevel,double mousevel,bool mi);
    void UpdateCamera();

	double getCamX();
	double getCamY();
	double getCamZ();

	double getCamPitch();
	double getCamYaw();
};

#endif
