#include <opencv2\opencv.hpp>
#include <string>
#include <iostream>
#include <fstream>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;
using namespace System;
using namespace cv;

//Struct para los costes de los vecinos a un pixel dado.
struct costes_vecinos{
	float costes[8];
};

//Struct para la lista de pixeles activos
struct pixel_coste{
	Point pixel;
	float coste;
};

//Funciones útiles
void MarshalString(System::String ^ s, string & os);
bool ordenaDecreciente (const pixel_coste  & first, const pixel_coste & second);
//Inicialización de las estructuras necesarias
void ini_expandidos(Mat &im, vector< vector <bool> > & expandidos);
void ini_costes(Mat &im, vector< vector <costes_vecinos> > & m_costes);
void ini_costes_final(Mat &im, vector< vector <float> > & m_costes_final);
void ini_p(Mat &im, vector< vector <Point> > & p);
void actualiza_estructuras(Mat &im, vector< vector <float> > & m_costes_final, vector< vector <Point> > & p, vector< vector <bool> > & expandidos);
//Funciones parciales del algoritmo
bool e(vector< vector <bool> > &expandidos , Point q);
int eliminarPunto(vector <pixel_coste> & L, Point p);
void pintaPunto(Mat & im, Point p);
//Funciones principales del algoritmo
void cal_m_costes(const Mat &im, vector< vector <costes_vecinos> > & m_costes);
void calculo_camino_optimo(Mat & im, vector< vector <costes_vecinos> > & m_costes, Point s, vector< vector <bool> > & expandidos, vector< vector <Point> > & p, vector< vector <float> > & m_costes_final);
void pintar_camino(Mat & im, vector< vector <Point> > & p, Point t);