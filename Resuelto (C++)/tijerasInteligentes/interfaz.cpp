#include "interfaz.h"

using namespace System;
using namespace System ::Windows::Forms;
[STAThread]
int main(array<System::String ^> ^argv){
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	tijerasInteligentes::interfaz form;
	Application::Run(%form);
}

