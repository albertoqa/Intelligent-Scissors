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
 * File: IntelligentScissors.cpp
 */

#include "IntelligentScissors.h"

/*
 * Constructor por defecto de la clase IntelligentScissors.
*/
IntelligentScissors::IntelligentScissors()
{}

/*
 * Constructor de la clase IntelligentScissors que recibe la imagen a procesar.
*/
IntelligentScissors::IntelligentScissors(string filePath, float wZ, float wD, float wG)
{
	this->wZ = wZ;
	this->wD = wD;
	this->wG = wG;

	imgColor = imread(filePath);
	imgGray = imread(filePath, 0);

	if(!imgColor.data || !imgGray.data)
		setError(1);
	else
		initData();
}

/*
 * Método que inicializa las estructuras de datos a usar.
*/
void IntelligentScissors::initData()
{
	// Remove noise by blurring with a Gaussian filter.
  	GaussianBlur(imgGray, imgGaussian, Size(1,1), 5, 5, BORDER_DEFAULT);

	// Apply Laplace function.
	Laplacian(imgGaussian, imgLaplacian, CV_32F, 1, 1, 0, BORDER_DEFAULT);

	// Cálculo de los gradientes en X e Y.
	Sobel(imgGray, imgGradX, CV_32F, 1, 0, 3, 1, 0, BORDER_DEFAULT);
	Sobel(imgGray, imgGradY, CV_32F, 0, 1, 3, 1, 0, BORDER_DEFAULT);

	// Calculo de la matriz G.
	G = Mat::zeros(imgGray.rows,imgGray.cols,CV_32F);
	for (int i = 0 ; i < G.rows ; i++)
	{
		for(int j = 0 ; j < G.cols ; j++)
		{
			G.at<float>(i,j) = sqrt(imgGradX.at<float>(i,j)*imgGradX.at<float>(i,j)+imgGradY.at<float>(i,j)*imgGradY.at<float>(i,j));
		}
	}
	minMaxLoc(G, &minG, &maxG);

	Pixel pix;
	pix.calculated = false;
	l = vector<vector<Pixel> >(imgGray.rows, vector<Pixel>(imgGray.cols, pix));
	P = vector<vector<Point> >(imgGray.rows, vector<Point>(imgGray.cols));
}

/*
 * Método que calcula inicia un pixel en el punto p y calcula el coste de sus 8 vecinos.
*/
void IntelligentScissors::calculateLocalCost(Point p)
{
	Pixel *pix = &l[p.x][p.y];
	Point q;
	pix->point = p;
	pix->cost = -1.0;
	for(int n=0; n<8; n++)
	{
		q = getNeighbor(p, n);
		if(checkDimensions(q))
			pix->N[n] = fZ(q) + fD(p, q) + fG(q, n);
		else
			pix->N[n] = 1000000;
	}
	pix->calculated = true;
}

/*
 * Devuelve un puntero a un pixel, si no está inicialado, lo inicializa.
*/
Pixel* IntelligentScissors::getPixel(Point p)
{
	Pixel *pix = NULL;
	if(checkDimensions(p))
	{
		pix = &l[p.x][p.y];
		if(!pix->calculated)
			calculateLocalCost(p);
	}
	return pix;
}

/*
 * Comprueba si estamos accediendo a un punto fuera de las dimensiones de la imagen.
*/
bool IntelligentScissors::checkDimensions(Point p)
{
	if(p.x < imgGray.rows && p.y < imgGray.cols && p.x >= 0 && p.y >= 0)
		return true;
	return false;
}

/*
 * Calcula el valor de fZ para el grafo de costes.
*/
double IntelligentScissors::fZ(Point q)
{
	if(imgLaplacian.at<float>(q.x, q.y) == 0.0)
		return 0.0;
	return wZ;
}

/*
 * Calcula el valor de fD para el grafo de costes.
*/
double IntelligentScissors::fD(Point p, Point q)
{
	Point Lpq;
	double dp, dq;
	Point Dp = Point(imgGradY.at<float>(p),-imgGradX.at<float>(p));
	Point Dq = Point(imgGradY.at<float>(q),-imgGradX.at<float>(q));

	if ((Dp.x*(q.x-p.x))+(Dp.y*(q.y-p.y))>=0) Lpq = Point(q.x-p.x, q.y-p.y);
	else Lpq = Point(p.x-q.x, p.y-q.y);

	NormalizePoint(Dp);
	NormalizePoint(Dq);
	NormalizePoint(Lpq);

	dp = Dp.x*Lpq.x-Dp.y*Lpq.y;
	dq = Dq.x*Lpq.x-Dq.y*Lpq.y;
										
	NormalizeSmallValues(dp);
	NormalizeSmallValues(dq);

	return wD * 1/M_PI*(acos(dp)+acos(dq));
}

/*
 * Calcula el valor de fD para el grafo de costes.
*/
double IntelligentScissors::fG(Point q, int n)
{
	if(isDiagonal(n))
		return wG * (1 - (G.at<float>(q.x,q.y) / maxG));
	else
		return wG * ((1 - (G.at<float>(q.x,q.y) / maxG)) *  (1 / sqrt(2.0)));
}

/*
 * Normaliza un punto.
*/
void IntelligentScissors::NormalizePoint(Point &p)
{
	Point normal;
	normal.x = p.x/(sqrt((float)(p.x*p.x + p.y*p.y)));
	normal.y = p.y/(sqrt((float)(p.x*p.x + p.y*p.y)));
	p = normal;
}

/*
 * Comprueba si un valor es demasiado pequeño, si es así lo iguala a 0.
*/
void IntelligentScissors::NormalizeSmallValues(double &v)
{
	if(v < 3.0e+009 && v > -3.0e+009) v = 0.0;
}

/*
 * Devuelve el n vecino de un punto p.
*/
Point IntelligentScissors::getNeighbor(Point p, int n)
{
	Point q;
	int x = p.x;
	int y = p.y;
	switch(n)
	{
		case 0: q = Point(x-1,y-1); break;
		case 1: q = Point(x-1,y); break;
		case 2: q = Point(x-1,y+1); break;
		case 3: q = Point(x,y-1); break;
		case 4: q = Point(x,y+1); break;
		case 5: q = Point(x+1,y-1); break;
		case 6: q = Point(x+1,y); break;
		case 7: q = Point(x+1,y+1); break;
	}
	return q;
}

/*
 * Comprueba si el vecino n está en la diagonal de un punto.
*/
bool IntelligentScissors::isDiagonal(int n)
{
	switch (n)
	{
		case 0:
		case 2:
		case 5:
		case 7: 
			return true; break;
		case 1:	
		case 3: 
		case 4:
		case 6: 
			return false; break;
	}
	return false;
}

/*
 * Devuelve un error y termina el programa.
*/
void IntelligentScissors::setError(int error)
{
	cout<<"[ERROR] ";
	switch(error)
	{
		case 1: cout<<"Imagen no encontrada."<<endl; break;
		default: cout<<"Algo ha fallado."<<endl; break;
	}
	exit(-1);
}

/*
 * Define el punto siguiente en el algoritmo, lo pinta y pinta el camino óptimo que encuentra.
*/
void IntelligentScissors::setPoint(int x, int y)
{
	Point s = Point(y, x);
	drawPoint(s);
	if(startPoint == Point())
		startPoint = Point(y, x);
	else
		drawPath(s);
	optimalPath(s);
}

/*
 * Dibuja el camino encontrado entre dos puntos.
*/
void IntelligentScissors::drawPath(Point s)
{
	Point aux = s;
	while(aux != Point())
	{
		P_result.push_back(aux);
		circle(imgColor, Point(aux.y, aux.x), 2, Scalar(0,255,0), -1);
		aux = P[aux.x][aux.y];
	}
}

/*
 * Dibuja un punto en la imagen.
*/
void IntelligentScissors::drawPoint(Point s)
{
	s = Point(s.y, s.x);
	circle(imgColor, s, 6, Scalar(0,255,0), -1);
}

/*
 * Algoritmo Live-Wire 2-D DP graph search.
 * Calcula el camino óptimo entre dos puntos para segmentación.
*/
void IntelligentScissors::optimalPath(Point s)
{
	// Pointers from each pixel indicating the minimum cost path.
	P = vector<vector<Point> >(imgGray.rows, vector<Point>(imgGray.cols));
	
	// Boolean function indicating if q has been expanded/processed.
	vector<vector<bool> > e (imgGray.rows, vector<bool>(imgGray.cols, false));

	// List of active pixels sorted by total cost (initially empty).
	multiset<Pixel*, comp> L;
	
	Pixel *q, *r;
	double g_tmp;

	// Initialize active list with zero cost seed pixel.	
	getPixel(s)->cost = 0.0;
	L.insert(getPixel(s));

	while(!L.empty()) // While still points to expand.
	{
		q = *L.begin(); L.erase(L.begin()); // Remove minimum cost pixel q from active list.
		e[q->point.x][q->point.y] = true; // Mark q as expanded.
		for(int n=0; n<8; n++)
		{
			Point a = getNeighbor(q->point, n);
			r = getPixel(a);			
			if(r != NULL && !e[r->point.x][r->point.y])
			{
				g_tmp = q->cost + q->N[n]; // Compute total cost to neighbor.
				
				if(find(L, r))
				{
					if(g_tmp < r->cost)
						L.erase(r); // Remove higher cost neighbor's from list.
				}
				else // if neighbor not on list.
				{
					r->cost = g_tmp; // assign neighbor's total cost.
					P[r->point.x][r->point.y] = q->point; // set back pointer.
					L.insert(r); // and place on (or return to) active list.
				}
			}
		}
	}
}

/*
 * Busca si un Pixel pertecene a un multiset de Pixel.
*/
bool IntelligentScissors::find(multiset<Pixel*, comp> &L, Pixel* p)
{
	multiset<Pixel*, comp>::iterator it;
	for(it=L.begin(); it!=L.end(); ++it)
		if((*it) == p)
			return true;
	return false;
}

/*
 * Devuelve la imagen.
*/
const Mat& IntelligentScissors::getImage()
{
	return imgColor;
}

/*
 * Devuelve el camino óptimo encontrado.
*/
vector<Point> IntelligentScissors::getPath()
{
	return P_result;
}

