/*
 *
 * Visión por computador
 * Trabajo Final
 *
 * Created on: 25/01/2015
 *      Author: Javier Moreno Vega <jmorenov@correo.ugr.es>
 * Last modified on: 9/01/2015
 * 	Modified by: Javier Moreno Vega <jmorenov@correo.ugr.es>
 * 
 * File: Window.h
 */

#ifndef WINDOW_H_
#define WINDOW_H_

#include "IntelligentScissors.h"

class Window
{
private:
	IntelligentScissors is;
	string name;
public:
	Window();	
	Window(string filePath, string windowName = "IntelligentScissors");
	void drawImage();
	void refreshImage();
	void start();	
	void touch(int event, int x, int y);
};
void CallBackFunc(int event, int x, int y, int flags, void* userdata);

#endif /* WINDOW_H_ */
