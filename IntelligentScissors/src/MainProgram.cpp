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

/*
 * Variable necesaria para gestionar la interfaz.
*/
Window MainWindow;

/*
 * Función que controla los eventos de los clicks del ratón. 
*/
void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
 	MainWindow.touch(event, x, y);
}

/*
 * Función principal del programa.
*/
int main(int argc, char **argv)
{
	MainWindow = Window();
	MainWindow.start();
}
