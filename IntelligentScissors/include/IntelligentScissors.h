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
 * File: IntelligentScissors.h
 */

#ifndef INTELLIGENTSCISSORS_H_
#define INTELLIGENTSCISSORS_H_

#include <string>
#include <iostream>
#include <math.h>
#include <vector>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#define WZ 0.43
#define WD 0.43
#define WG 0.14

struct Pixel
{
	bool calculated;
	double N[8];
	double cost;
	Point point;
};

struct comp
{
	bool operator()(const Pixel *p1, const Pixel *p2)
	{
    	return p1->cost < p2->cost;
  	}
};

class IntelligentScissors
{
private:
	Mat imgColor;
	Mat imgGray;

	Mat imgGaussian;
	Mat imgLaplacian;
	Mat imgGradX;
	Mat imgGradY;	
	Mat G;
	double maxG;	
	double minG;

	vector<vector<Pixel> > l;
	vector<Point> P_result;
	vector<vector<Point> >P;

	Point startPoint;	

	double wZ;
	double wD;
	double wG;

	void initData();

	void calculateLocalCost(Point p);
	Pixel* getPixel(Point p);
	Point getNeighbor(Point p, int n);

	bool isDiagonal(int n);
	double fZ(Point q);
	double fD(Point p, Point q);
	double fG(Point q, int n);
	void NormalizeSmallValues(double &v);
	void NormalizePoint(Point &p);
	bool find(multiset<Pixel*, comp> &L, Pixel* p);

	void setError(int error);
	bool checkDimensions(Point p);

	void drawPath(Point s);
	void drawPoint(Point s);
	void optimalPath(Point s);

public:
	IntelligentScissors();
	IntelligentScissors(string filePath, float wZ = WZ, float wD = WD, float wG = WG);
	const Mat& getImage();
	void setPoint(int x, int y);
	vector<Point> getPath();
};


#endif /* INTELLIGENTSCISSORS_H_ */
