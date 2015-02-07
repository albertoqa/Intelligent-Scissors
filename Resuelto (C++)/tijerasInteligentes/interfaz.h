#pragma once

#include "tijeras_inteligentes.h"
#include <string>
#include <iostream>
using namespace System;
using namespace std;



/***Variables enlace entre interfaz y aplicación***/

//Imagen para trabajar con openCV en gris y para mostrarla a color para el usuario
Mat imPintar, imGris;

//Path de la imagen a mostrar en el pictureBox
string filePath;

//Creación de la matriz para comprobar si un pixel ha sido expandido o no
vector< vector <bool> > expandidos;

//Creación de la matriz de costes
vector< vector <costes_vecinos> > m_costes;

//Creación de la matriz de los costes finales (mínimos)
vector< vector <float> > m_costes_final;

//Creación de la matriz para el camino óptimo
vector< vector <cv::Point> > p;



//Coordenadas ratón
int X;
int Y;
int priX;
int priY;

//Booleanas para la gestión de la interfaz
bool primer_click = true;


namespace tijerasInteligentes {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Resumen de interfaz
	/// </summary>
	public ref class interfaz : public System::Windows::Forms::Form
	{
	public:
		interfaz(void)
		{
			InitializeComponent();
			//
			//TODO: agregar código de constructor aquí
			//
		}

	protected:
		/// <summary>
		/// Limpiar los recursos que se estén utilizando.
		/// </summary>
		~interfaz()
		{
			if (components)
			{
				delete components;
			}
		}



	private: System::Windows::Forms::Button^  selecImagen;
	private: System::Windows::Forms::PictureBox^  campoImagen;
	private: System::Windows::Forms::ToolTip^  toolTip1;
	private: System::Windows::Forms::OpenFileDialog^  openFileDialog1;
	private: System::Windows::Forms::Button^  cerrar;
	private: System::Windows::Forms::Button^  terminar;


	private: System::ComponentModel::IContainer^  components;

	protected: 



	protected: 

	private:
		/// <summary>
		/// Variable del diseñador requerida.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Método necesario para admitir el Diseñador. No se puede modificar
		/// el contenido del método con el editor de código.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->selecImagen = (gcnew System::Windows::Forms::Button());
			this->campoImagen = (gcnew System::Windows::Forms::PictureBox());
			this->toolTip1 = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->openFileDialog1 = (gcnew System::Windows::Forms::OpenFileDialog());
			this->cerrar = (gcnew System::Windows::Forms::Button());
			this->terminar = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->campoImagen))->BeginInit();
			this->SuspendLayout();
			// 
			// selecImagen
			// 
			this->selecImagen->Location = System::Drawing::Point(12, 12);
			this->selecImagen->Name = L"selecImagen";
			this->selecImagen->Size = System::Drawing::Size(306, 34);
			this->selecImagen->TabIndex = 3;
			this->selecImagen->Text = L"Seleccionar imagen";
			this->selecImagen->UseVisualStyleBackColor = true;
			this->selecImagen->Click += gcnew System::EventHandler(this, &interfaz::selecImagen_Click);
			// 
			// campoImagen
			// 
			this->campoImagen->Location = System::Drawing::Point(12, 52);
			this->campoImagen->Name = L"campoImagen";
			this->campoImagen->Size = System::Drawing::Size(954, 496);
			this->campoImagen->TabIndex = 4;
			this->campoImagen->TabStop = false;
			this->campoImagen->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &interfaz::campoImagen_MouseDown);
			// 
			// openFileDialog1
			// 
			this->openFileDialog1->FileName = L"openFileDialog1";
			// 
			// cerrar
			// 
			this->cerrar->Location = System::Drawing::Point(663, 12);
			this->cerrar->Name = L"cerrar";
			this->cerrar->Size = System::Drawing::Size(303, 34);
			this->cerrar->TabIndex = 5;
			this->cerrar->Text = L"Salir aplicación";
			this->cerrar->UseVisualStyleBackColor = true;
			this->cerrar->Click += gcnew System::EventHandler(this, &interfaz::cerrar_Click);
			// 
			// terminar
			// 
			this->terminar->Location = System::Drawing::Point(341, 12);
			this->terminar->Name = L"terminar";
			this->terminar->Size = System::Drawing::Size(306, 34);
			this->terminar->TabIndex = 6;
			this->terminar->Text = L"Terminar";
			this->terminar->UseVisualStyleBackColor = true;
			this->terminar->Click += gcnew System::EventHandler(this, &interfaz::terminar_Click);
			// 
			// interfaz
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::ActiveCaption;
			this->ClientSize = System::Drawing::Size(978, 560);
			this->Controls->Add(this->terminar);
			this->Controls->Add(this->cerrar);
			this->Controls->Add(this->campoImagen);
			this->Controls->Add(this->selecImagen);
			this->Name = L"interfaz";
			this->Text = L"interfaz";
			this->Load += gcnew System::EventHandler(this, &interfaz::interfaz_Load);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->campoImagen))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void interfaz_Load(System::Object^  sender, System::EventArgs^  e) {
					toolTip1->Active = true;
					toolTip1->SetToolTip(campoImagen, "Imagen");
					toolTip1->SetToolTip(selecImagen, "Abre el explorador de archivos para seleccionar una imagen");
					toolTip1->SetToolTip(terminar, "Autocompleta la selección");
					toolTip1->SetToolTip(cerrar, "Cierra la aplicación");
				}

	private: System::Void selecImagen_Click(System::Object^  sender, System::EventArgs^  e) {
					// Seleccionamos la imagen y hacemos copias para pintarla y trabajar con ella 
					openFileDialog1->ShowDialog();
					campoImagen->Load(openFileDialog1->FileName);
					MarshalString (openFileDialog1->FileName, filePath );
					imGris = imread(filePath, 0);
					imPintar = imread(filePath);
					imwrite("imagenes/myImagen.jpg", imPintar);
					//Inicializamos la matriz de expandidos y de costes
					ini_expandidos(imGris, expandidos);
					ini_costes(imGris, m_costes);
					ini_costes_final(imGris, m_costes_final);
					ini_p(imGris, p);
					//Calculamos la matriz de costes
					cal_m_costes(imGris, m_costes);
			 }

   private: System::Void terminar_Click(System::Object^  sender, System::EventArgs^  e) {
					cv::Point t = cv::Point(priX, priY);
					// Pintamos el camino
					pintar_camino(imPintar, p, t);
					//Refrescamos la imagen de la interfaz gráfica.
					std::string str2 = "imagenes/myImagen.jpg";
					System::String^ str3 = gcnew System::String(str2.c_str(), 0, str2.length());
					campoImagen->Load(str3);
		 }

   private: System::Void cerrar_Click(System::Object^  sender, System::EventArgs^  e) {
					//Cerramos la aplicación sin errores.
					Application::Exit();
				}
				
	private: void campoImagen_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
					if(primer_click){
						cv::Point s = cv::Point(e->X, e->Y);
						//Guardamos el primer punto por si deseamos terminar
						priX = e->X;
						priY = e->Y;
						//Calculamos la matriz de coste mínimo para el pixel clickado
						calculo_camino_optimo(imGris, m_costes, s, expandidos, p, m_costes_final);
						pintaPunto(imPintar, s);
						//Refrescamos la imagen de la interfaz gráfica.
						std::string str1 = "imagenes/myImagen.jpg";
						System::String^ str = gcnew System::String(str1.c_str(), 0, str1.length());
						campoImagen->Load(str);
						//calculo_camino_optimo(imGris, m_costes, s, expandidos, p);
						primer_click = false;
					}else{
						cv::Point t = cv::Point(e->X, e->Y);
						// Pintamos el punto y el camino
						pintaPunto(imPintar, t);
						pintar_camino(imPintar, p, t);
						//Refrescamos la imagen de la interfaz gráfica.
						std::string str2 = "imagenes/myImagen.jpg";
						System::String^ str3 = gcnew System::String(str2.c_str(), 0, str2.length());
						campoImagen->Load(str3);
						//Calculamos la matriz de coste mínimo para el pixel clickado
						actualiza_estructuras(imGris, m_costes_final, p,expandidos);
						calculo_camino_optimo(imGris, m_costes, t, expandidos, p, m_costes_final);

					}

				}


};
}
