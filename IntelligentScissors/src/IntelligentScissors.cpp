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

IntelligentScissors::IntelligentScissors()
{}

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

void IntelligentScissors::initData()
{
  	GaussianBlur(imgGray, imgGaussian, Size(1,1), 5, 5, BORDER_DEFAULT);
	Laplacian(imgGaussian, imgLaplacian, CV_32F, 1, 1, 0, BORDER_DEFAULT);

	Sobel(imgGray, imgGradX, CV_32F, 1, 0, 3, 1, 0, BORDER_DEFAULT);
	Sobel(imgGray, imgGradY, CV_32F, 0, 1, 3, 1, 0, BORDER_DEFAULT);

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

bool IntelligentScissors::checkDimensions(Point p)
{
	if(p.x < imgGray.rows && p.y < imgGray.cols && p.x >= 0 && p.y >= 0)
		return true;
	return false;
}

double IntelligentScissors::fZ(Point q)
{
	if(imgLaplacian.at<float>(q.x, q.y) == 0.0)
		return 0.0;
	return wZ;
}

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

void IntelligentScissors::NormalizePoint(Point &p)
{
	Point normal;
	normal.x = p.x/(sqrt((float)(p.x*p.x + p.y*p.y)));
	normal.y = p.y/(sqrt((float)(p.x*p.x + p.y*p.y)));
	p = normal;
}

void IntelligentScissors::NormalizeSmallValues(double &v)
{
	if(v < 3.0e+009 && v > -3.0e+009) v = 0.0;
}

double IntelligentScissors::fG(Point q, int n)
{
	if(isDiagonal(n))
		return wG * (1 - (G.at<float>(q.x,q.y) / maxG));
	else
		return wG * ((1 - (G.at<float>(q.x,q.y) / maxG)) *  (1 / sqrt(2.0)));
}

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
		default:
			setError(2);
	}
	return false;
}

void IntelligentScissors::setError(int error)
{
	cout<<"ERROR "<<error<<endl;
	exit(-1);
}

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

void IntelligentScissors::drawPath(Point s)
{
	Point aux = s;
	while(aux != Point())
	{
		P_result.push_back(aux);
		circle(imgColor, Point(aux.y, aux.x), 2, Scalar(0,0,150), -1, 8, 0 );
		aux = P[aux.x][aux.y];
	}
}

void IntelligentScissors::drawPoint(Point s)
{
	s = Point(s.y, s.x);
	circle(imgColor, s, 5, Scalar(0,0,255), -1, 8, 0 );
}

void IntelligentScissors::optimalPath(Point s)
{
	P = vector<vector<Point> >(imgGray.rows, vector<Point>(imgGray.cols));
	vector<vector<bool> > e (imgGray.rows, vector<bool>(imgGray.cols, false));
	multiset<Pixel*, comp> L;
	Pixel *q, *r;
	double g_tmp;

	getPixel(s)->cost = 0.0;
	L.insert(getPixel(s));
	while(!L.empty())
	{
		q = *L.begin();
		L.erase(L.begin());
		e[q->point.x][q->point.y] = true;
		for(int n=0; n<8; n++)
		{
			Point a = getNeighbor(q->point, n);
			r = getPixel(a);			
			if(r != NULL && !e[r->point.x][r->point.y])
			{
				g_tmp = q->cost + q->N[n];
				
				if(find(L, r))
				{
					if(g_tmp < r->cost)
						L.erase(r);
				}
				else
				{
					r->cost = g_tmp;
					P[r->point.x][r->point.y] = q->point;
					L.insert(r);
				}
			}
		}
	}
}

bool IntelligentScissors::find(multiset<Pixel*, comp> &L, Pixel* p)
{
	multiset<Pixel*, comp>::iterator it;
	for(it=L.begin(); it!=L.end(); ++it)
		if((*it) == p)
			return true;
	return false;
}

const Mat& IntelligentScissors::getImage()
{
	return imgColor;
}

vector<Point> IntelligentScissors::getPath()
{
	return P_result;
}

