#include "tijeras_inteligentes.h"


//Función para pasar de System::String a stl::string
void MarshalString(System::String ^ s, string & os){
	using namespace Runtime::InteropServices;
	const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
	os = chars;
	Marshal::FreeHGlobal(IntPtr((void*)chars));
}

//Función utilizada para ordenar la lista de forma decreciente
bool ordenaDecreciente (const pixel_coste  & first, const pixel_coste & second){
	return ( first.coste < second.coste );
}

//Inicialización de la matriz que controla si los puntos han sido expandidos
void ini_expandidos(Mat &im, vector< vector <bool> > & expandidos){
	for (int i = 0 ; i < im.rows ; i++)
		expandidos.push_back(vector<bool> (im.cols,false));
}

//Inicialización de la matriz de costes
void ini_costes(Mat &im, vector< vector <costes_vecinos> > & m_costes){
	for (int i = 0 ; i < im.rows ; i++)
		m_costes.push_back(vector<costes_vecinos> (im.cols));
}

//Inicialización de la matriz de costes finales (minimos)
void ini_costes_final(Mat &im, vector< vector <float> > & m_costes_final){
	for (int i = 0 ; i < im.rows ; i++)
		m_costes_final.push_back(vector<float> (im.cols,-1.0));
}

//Inicialización de la matriz de camino óptimo
void ini_p(Mat &im, vector< vector <Point> > & p){
	for (int i = 0 ; i < im.rows ; i++)
		p.push_back(vector<Point> (im.cols,Point(0,0)));
}

//Función para actualizar las estructuras en cada click inicial
void actualiza_estructuras(Mat &im, vector< vector <float> > & m_costes_final, vector< vector <Point> > & p, vector< vector <bool> > & expandidos){
	for (int i = 0 ; i < im.rows ; i++){
		for (int j = 0 ; j < im.cols ; j++){
			m_costes_final[i][j] = -1.0;
			p[i][j] = Point(0,0);
			expandidos[i][j] = false;
		}
	}
}


//Función que devuelve si un pixel ha sido expandido o procesado
bool e(vector< vector <bool> > &expandidos , Point q){return expandidos[q.x][q.y];}

//Función para eliminar un punto de la lista
int eliminarPunto(vector < pixel_coste > & L, Point p){

	for (std::vector< pixel_coste >::iterator it=L.begin(); it != L.end(); ++it)
		if((*it).pixel == p){
			it = L.erase(it);
			return 0;
		}

		return -1;
}

//Función que pinta un circulo en las coordenadas indicadas
void pintaPunto(Mat & im, Point p){
	//Pintamos el punto y grabamos en disco la imagen
	circle(im, p, 5, Scalar(0,0,255), -1, 8, 0 );
	imwrite("imagenes/myImagen.jpg", im);
}

//Función para calcular la matriz de costes
void cal_m_costes(const Mat &im, vector< vector <costes_vecinos> > & m_costes){
	//Constantes necesarias para el calculo del coste.
	float w_z = 0.43, w_d = 0.43, w_g = 0.14;
	//Número de filas y columnas
	int fils = m_costes.size();
	int cols = m_costes[0].size();
	//Vecino auxiliar para la inicialiación de los márgenes a un valor alto
	costes_vecinos auxiliar;
	for (int i = 0 ; i < 8 ; i++){
		auxiliar.costes[i] = 100000000;
	}

	/*Inicialización de márgenes de la estructura 
	Margen de un pixel para evitar recortar por el borde de la imagen
	*/
	for (int i = 0 ; i < fils ; i++){
		m_costes[i][0] = auxiliar;
		m_costes[i][cols-1] = auxiliar;
	}

	for (int i = 0 ; i < cols ; i++){
		m_costes[0][i] = auxiliar;
		m_costes[fils-1][i] = auxiliar;
	}


	/*Calculamos f_z utilizando la laplaciana*/
	Mat f_z, convolucion;
	Mat pinta(im.rows, im.cols, CV_8U);
	//pintaIm(pinta);
	/// Eliminamos el ruido
	GaussianBlur( im, convolucion, Size(1,1), 5, 5, BORDER_DEFAULT);
	//Calculo de la laplaciana
	Laplacian( convolucion, f_z, CV_32F, 1, 1, 0, BORDER_DEFAULT );


	// Calculamos el gradiente para x e y
	Mat gradX, gradY;
	Sobel( im, gradX, CV_32F, 1, 0, 3, 1, 0, BORDER_DEFAULT );
	Sobel( im, gradY, CV_32F, 0, 1, 3, 1, 0, BORDER_DEFAULT );

	//Vamos a calcular la magnitud de gradiente G que será la raiz cuadrada
	//de la suma de cuadrados de los gradientes en "x" e "y"
	Mat G = Mat::zeros(im.rows,im.cols,CV_32F);
	float G_aux;
	float G_max = 0;

	for (int i = 0 ; i < G.rows ; i++){
		for(int j = 0 ; j < G.cols ; j++){
			G_aux = sqrt(gradX.at<float>(i,j)*gradX.at<float>(i,j)+gradY.at<float>(i,j)*gradY.at<float>(i,j));
			G.at<float>(i,j) = G_aux;
			if (G_max < G_aux)//Nos quedamos con el G máximo
				G_max = G_aux;
		}
	}

	bool diagonal;
	Point q;

	/*Para cada centro, tenemos un vector de 8 costes de los vecinos*/
	for (int i = 1 ; i < fils-1 ; i++){
		for (int j = 1 ; j < cols-1 ; j++){
			for ( int k = 0 ; k < 8 ; k++){
				//Definimos el punto p y el punto q (p-centro, q-vecino)
				Point p(i,j);
				/*
				En el caso de que el vecino esté arriba o abajo, derecha o izquierda hay que multiplicarlo por 1/sqrt(2)
				En el caso de que se encuentre el vecino en la diagonal, hay que multiplicarlo por 1
				*/
				switch (k) {
				case 0: q = Point(i-1,j-1); diagonal = true; break;
				case 1: q = Point(i-1,j); diagonal = false; break;
				case 2: q = Point(i-1,j+1); diagonal = true; break;
				case 3: q = Point(i,j-1); diagonal = false; break;
				case 4: q = Point(i,j+1); diagonal = false; break;
				case 5: q = Point(i+1,j-1); diagonal = true; break;
				case 6: q = Point(i+1,j); diagonal = false; break;
				case 7: q = Point(i+1,j+1); diagonal = true; break;
				default: exit(1); break;
				}

				float sumando1 = 0.0;
				float sumando2 = 0.0;
				float sumando3 = 0.0;

				//Calculamos el primer sumando
				if (f_z.at<float>(q.x,q.y) == 0)
					sumando1 = 0;
				else
					sumando1 = w_z;

				//Calculamos el segundo sumando
				Point Lpq;
				float dp, dq;
				float f_d = 0.0;
				Point Dp = Point(gradY.at<float>(p),-gradX.at<float>(p));
				Point Dq = Point(gradY.at<float>(q),-gradX.at<float>(q));

				if ((Dp.x*(q.x-p.x))+(Dp.y*(q.y-p.y))>=0)
					Lpq = Point(q.x-p.x, q.y-p.y);
				else
					Lpq = Point(p.x-q.x, p.y-q.y);


				// Normalizamos D' y L 
				Point Dp_normal, Dq_normal, Lpq_normal;
				Dp_normal.x = Dp.x/(sqrt((float)(Dp.x*Dp.x + Dp.y*Dp.y)));
				Dp_normal.y = Dp.y/(sqrt((float)(Dp.x*Dp.x + Dp.y*Dp.y)));
				Dq_normal.x = Dq.x/(sqrt((float)(Dq.x*Dq.x + Dq.y*Dq.y)));
				Dq_normal.y = Dq.y/(sqrt((float)(Dq.x*Dq.x + Dq.y*Dq.y)));
				Lpq_normal.x = Lpq.x/(sqrt((float)(Lpq.x*Lpq.x + Lpq.y*Lpq.y)));
				Lpq_normal.y = Lpq.y/(sqrt((float)(Lpq.x*Lpq.x + Lpq.y*Lpq.y)));

				dp = Dp_normal.x*Lpq_normal.x-Dp_normal.y*Lpq_normal.y;
				dq = Dq_normal.x*Lpq_normal.x-Dq_normal.y*Lpq_normal.y;
											
				
				if( dp < 3.0e+009 && dp > -3.0e+009)
					dp = 0.0;
				if( dq < 3.0e+009 && dq > -3.0e+009)
					dq = 0.0;

				f_d = 1/M_PI*(acos(dp)+acos(dq));
				

				
				sumando2 = f_d*w_d;

				//Calculamos el tercer sumando
				if(diagonal){
					sumando3 = w_g*1-(G.at<float>(q.x,q.y)/G_max)*1;
				}else{
					sumando3 = w_g*1-(G.at<float>(q.x,q.y)/G_max)*(1/sqrt(2.0));
				}
				//Introduzco el coste del vecino
				m_costes[i][j].costes[k] = sumando1 + sumando2 + sumando3;

			}
		}
	}

}

//Función para calcular dado un punto inicial "s", la matriz de caminos óptimos "p"
void calculo_camino_optimo(Mat & im, vector< vector <costes_vecinos> > & m_costes, Point s, vector< vector <bool> > & expandidos, vector< vector <Point> > & p, vector< vector <float> > & m_costes_final){
	//Creación de variables
	Point q, r;
	float g_q = 0, g_r = 0, g_tmp = 0;
	
	// Marcamos el pixel semilla con coste 0. 
	int pixeles_activos = 1;
	m_costes_final[s.x][s.y] = 0;
		

	//Indicamos con -1,-1 el punto inicial para poder comprobar que es el primero de los introducidos
	p[s.x][s.y] = Point(-1,-1);

	//Mientras que haya pixeles activos
	while( pixeles_activos != 0 ){
		// Obtenemos el pixel de menor coste
		float min_coste = 100000000;
		for (int i = 0 ; i < im.rows ; i++)
			for (int j = 0 ; j < im.cols ; j++)
				if ( m_costes_final[i][j] < min_coste && m_costes_final[i][j] != -1.0){
					min_coste = m_costes_final[i][j];
					g_q = min_coste;
					q = Point(i,j);
				}
		// Marcamos el pixel con coste -1 para indicar que ya no esta activo
		m_costes_final[q.x][q.y] = -1.0;
		pixeles_activos--;

		//Marcamos el pixel q como expandido
		expandidos[q.x][q.y] = true;

		for ( int k = 0 ; k < 8 ; k++){
			//Definimos el punto r
			switch (k) {
			case 0: r = Point(q.x-1,q.y-1); break;
			case 1: r = Point(q.x-1,q.y); break;
			case 2: r = Point(q.x-1,q.y+1); break;
			case 3: r = Point(q.x,q.y-1); break;
			case 4: r = Point(q.x,q.y+1); break;
			case 5: r = Point(q.x+1,q.y-1); break;
			case 6: r = Point(q.x+1,q.y); ;break;
			case 7: r = Point(q.x+1,q.y+1); break;
			default: exit(1); break;
			}

			//Comprobamos que el punto r no se salga de las dimensiones de la imagen (Problema con los márgenes)
			if((r.x >= 0) && (r.y >= 0) && (r.x < im.rows) && (r.y < im.cols)){
				//Comprobamos que r no haya sido expandido
				if (!e(expandidos, r)){
					//Calculo el coste total para el vecino r
					g_tmp = g_q + m_costes[q.x][q.y].costes[k];

					g_r = m_costes_final[r.x][r.y];
					//Si r está en la lista y g temporal es menor que g de r
					if (g_r >= 0){
						if(g_tmp < g_r){
							//Marcamos en la matriz de costes el punto a -1 para indicar que no está activo
							m_costes_final[r.x][r.y] = -1.0;
							pixeles_activos--;
						}
					}

					//Como el coste devuelto es -1, el punto r no está activo 
					if(m_costes_final[r.x][r.y] == -1){ 
						g_r = g_tmp;

						//En la matriz de camino óptimo señalamos el siguiente punto para seguir por el camino óptimo.
						p[r.x][r.y] = q;

						//Introducimos el coste de r
						m_costes_final[r.x][r.y] = g_r;
						pixeles_activos++;

					}
				}
			}
		}
	}

}

//Función para pintar el camino óptimo dado un punto y una matriz de caminos óptimos
void pintar_camino(Mat & im, vector< vector <Point> > & p, Point t){
	Point aux = t;
	while(aux.x != -1){
		circle(im, aux, 2, Scalar(0,0,150), -1, 8, 0 );
		aux = p[aux.x][aux.y];
	}
	//Grabamos la imagen en disco	
	imwrite("imagenes/myImagen.jpg", im);
}

