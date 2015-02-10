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
 * File: Window.cpp
 */

#include "Window.h"

Window::Window()
{

}

Window::Window(string filePath, string windowName)
{
	this->name = windowName;
	this->is = IntelligentScissors(filePath);
}

void Window::drawImage()
{
	namedWindow(name);
  	//set the callback function for any mouse event
  	setMouseCallback(name, CallBackFunc, NULL);
  	imshow(name, is.getImage());
  	waitKey(0);
	destroyWindow(name);
}

void Window::refreshImage()
{
  	imshow(name, is.getImage());
}

void Window::touch(int event, int x, int y)
{
	if(event == EVENT_LBUTTONDOWN)
	{
		is.setPoint(x, y);
		refreshImage();
	}
}

void Window::start()
{
	drawImage();
}
