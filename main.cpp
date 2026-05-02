#include "MyForm.h"

using namespace OSProject;

[STAThreadAttribute]
int main()
{
    System::Windows::Forms::Application::EnableVisualStyles();
    System::Windows::Forms::Application::SetCompatibleTextRenderingDefault(false);
    System::Windows::Forms::Application::Run(gcnew MyForm());
    return 0;
}
