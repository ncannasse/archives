//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USERES("Dragoon.res");
USEUNIT("Main_Defs.cpp");
USEFORM("Console.cpp", ConsoleForm);
USEFORM("Load_Form.cpp", LoadForm);
USEUNIT("PAK_Defs.cpp");
USEUNIT("PRJ_Defs.cpp");
USEFORM("Input_Form.cpp", InputForm);
USEFORM("Types_Form.cpp", TypesForm);
USEUNIT("Types_Defs.cpp");
USEUNIT("Folders_Defs.cpp");
USEFORM("PAK_Form.cpp", PAKForm);
USEUNIT("Balises.cpp");
USEFORM("PPrefs_Form.cpp", PPrefForm);
USEUNIT("Plugins.cpp");
USEFORM("PlugInfo_Form.cpp", PlugInfoForm);
USEFORM("Plug_Form.cpp", PlugForm);
USEFORM("PAK_Browser.cpp", PAKBrowser);
USEFORM("Main_Form.cpp", MainForm);
USEUNIT("PlugActions.cpp");
//---------------------------------------------------------------------------
void
InitApp()
{
       Application->Initialize();
       Application->Title = "Dragoon PlatForm";
       Application->CreateForm(__classid(TMainForm), &MainForm);
                 Application->CreateForm(__classid(TConsoleForm), &ConsoleForm);
                 Application->CreateForm(__classid(TLoadForm), &LoadForm);
                 Application->CreateForm(__classid(TInputForm), &InputForm);
                 Application->CreateForm(__classid(TTypesForm), &TypesForm);
                 Application->CreateForm(__classid(TPAKForm), &PAKForm);
                 Application->CreateForm(__classid(TPPrefForm), &PPrefForm);
                 Application->CreateForm(__classid(TPlugInfoForm), &PlugInfoForm);
                 Application->CreateForm(__classid(TPlugForm), &PlugForm);
                 Application->CreateForm(__classid(TPAKBrowser), &PAKBrowser);
                 Application->Run();
}
//---------------------------------------------------------------------------

WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR Cmd, int)
{
   try
   {
       InitApp();
   }
   catch (Exception &exception)
   {
       Application->ShowException(&exception);
   }

   return 0;
}
//---------------------------------------------------------------------------
