/*
 *
 * Visión por computador
 * Trabajo Final
 *
 * Created on: 25/01/2015
 *      Author: Javier Moreno Vega <jmorenov@correo.ugr.es>
 *		Author: Alberto Quesada Aranda <qa.alberto@gmail.com>	
 *
 * Last modified on: 11/02/2015
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

int main(int argc, char **argv)
{
	string filePath = "images/lena.jpg";
	if(argc > 1)
		filePath = argv[1];

	MainWindow = Window(filePath);
	MainWindow.start();
}
