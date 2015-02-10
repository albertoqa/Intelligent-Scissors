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
 * File: MainProgram.cpp
 */

#include "IntelligentScissors.h"
#include "Window.h"
#include <string>

using namespace std;

Window MainWindow;

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
 	MainWindow.touch(event, x, y);
}

int main()
{
	string filePath = "images/lena.jpg";
	MainWindow = Window(filePath);
	MainWindow.start();
}
