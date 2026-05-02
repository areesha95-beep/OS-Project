#pragma once
#pragma managed(push, off)
#include <sstream>
#include <string>
#include "phase1.h"
#include "phase2.h"
#include "phase3.h"
#include "phase4.h"
#include "phase5.h"
#include "phase6.h"
#include "phase7.h"
#include "phase8.h"
#include "phase9.h"
#pragma managed(pop)
namespace OSProject {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
			
			this->components = gcnew System::ComponentModel::Container();
			this->Size = System::Drawing::Size(600, 500);
			this->Text = L"Virtual Memory Simulator";
			this->Padding = System::Windows::Forms::Padding(0);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::Color::FromArgb(30, 30, 30);
			this->ForeColor = System::Drawing::Color::White;

			//Labels 
			lblConfig = gcnew System::Windows::Forms::Label();
			lblConfig->Text = L"Config File:";
			lblConfig->Location = System::Drawing::Point(20, 20);
			lblConfig->Size = System::Drawing::Size(80, 22);
			lblConfig->ForeColor = System::Drawing::Color::White;

			lblTrace = gcnew System::Windows::Forms::Label();
			lblTrace->Text = L"Trace File:";
			lblTrace->Location = System::Drawing::Point(20, 55);
			lblTrace->Size = System::Drawing::Size(80, 22);
			lblTrace->ForeColor = System::Drawing::Color::White;

			lblFrame = gcnew System::Windows::Forms::Label();
			lblFrame->Text = L"Frame Count:";
			lblFrame->Location = System::Drawing::Point(20, 90);
			lblFrame->Size = System::Drawing::Size(80, 22);
			lblFrame->ForeColor = System::Drawing::Color::White;

			lblAlgo = gcnew System::Windows::Forms::Label();
			lblAlgo->Text = L"Algorithm:";
			lblAlgo->Location = System::Drawing::Point(260, 90);
			lblAlgo->Size = System::Drawing::Size(70, 22);
			lblAlgo->ForeColor = System::Drawing::Color::White;

			//TextBoxes
			txtConfig = gcnew System::Windows::Forms::TextBox();
			txtConfig->Text = L"config.txt";
			txtConfig->Location = System::Drawing::Point(110, 18);
			txtConfig->Size = System::Drawing::Size(450, 22);
			txtConfig->BackColor = System::Drawing::Color::FromArgb(50, 50, 50);
			txtConfig->ForeColor = System::Drawing::Color::White;

			txtTrace = gcnew System::Windows::Forms::TextBox();
			txtTrace->Text = L"trace.txt";
			txtTrace->Location = System::Drawing::Point(110, 53);
			txtTrace->Size = System::Drawing::Size(450, 22);
			txtTrace->BackColor = System::Drawing::Color::FromArgb(50, 50, 50);
			txtTrace->ForeColor = System::Drawing::Color::White;

			txtFrame = gcnew System::Windows::Forms::TextBox();
			txtFrame->Text = L"4";
			txtFrame->Location = System::Drawing::Point(110, 88);
			txtFrame->Size = System::Drawing::Size(80, 22);
			txtFrame->BackColor = System::Drawing::Color::FromArgb(50, 50, 50);
			txtFrame->ForeColor = System::Drawing::Color::White;

			//ComboBox
			cmbAlgo = gcnew System::Windows::Forms::ComboBox();
			cmbAlgo->Items->Add(L"FIFO");
			cmbAlgo->Items->Add(L"LRU");
			cmbAlgo->Items->Add(L"OPT");
			cmbAlgo->SelectedIndex = 0;
			cmbAlgo->Location = System::Drawing::Point(335, 88);
			cmbAlgo->Size = System::Drawing::Size(90, 22);
			cmbAlgo->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			cmbAlgo->BackColor = System::Drawing::Color::FromArgb(50, 50, 50);
			cmbAlgo->ForeColor = System::Drawing::Color::White;

			//Button
			btnRun = gcnew System::Windows::Forms::Button();
			btnRun->Text = L"Run Simulation";
			btnRun->Location = System::Drawing::Point(20, 125);
			btnRun->Size = System::Drawing::Size(540, 30);
			btnRun->BackColor = System::Drawing::Color::FromArgb(0, 120, 200);
			btnRun->ForeColor = System::Drawing::Color::White;
			btnRun->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			btnRun->Click += gcnew System::EventHandler(this, &MyForm::btnRun_Click);

			// Output Box
			rtbOutput = gcnew System::Windows::Forms::RichTextBox();
			rtbOutput->Location = System::Drawing::Point(20, 170);
			rtbOutput->Size = System::Drawing::Size(555, 280);
			rtbOutput->BackColor = System::Drawing::Color::FromArgb(20, 20, 20);
			rtbOutput->ForeColor = System::Drawing::Color::LightGreen;
			rtbOutput->ReadOnly = true;
			rtbOutput->ScrollBars = System::Windows::Forms::RichTextBoxScrollBars::Vertical;

			//Add everything to the form 
			this->Controls->Add(lblConfig);
			this->Controls->Add(lblTrace);
			this->Controls->Add(lblFrame);
			this->Controls->Add(lblAlgo);
			this->Controls->Add(txtConfig);
			this->Controls->Add(txtTrace);
			this->Controls->Add(txtFrame);
			this->Controls->Add(cmbAlgo);
			this->Controls->Add(btnRun);
			this->Controls->Add(rtbOutput);
		}

	protected:
	
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}

	protected:
	private:
		System::ComponentModel::Container ^components;
		System::Windows::Forms::Label^ lblConfig;
		System::Windows::Forms::Label^ lblTrace;
		System::Windows::Forms::Label^ lblFrame;
		System::Windows::Forms::Label^ lblAlgo;
		System::Windows::Forms::TextBox^ txtConfig;
		System::Windows::Forms::TextBox^ txtTrace;
		System::Windows::Forms::TextBox^ txtFrame;
		System::Windows::Forms::ComboBox^ cmbAlgo;
		System::Windows::Forms::Button^ btnRun;

		System::Windows::Forms::RichTextBox^ rtbOutput;

#pragma region Windows Form Designer generated code
		
		void InitializeComponent(void)
		{
			this->SuspendLayout();
			
			this->AutoScaleDimensions = System::Drawing::SizeF(12, 25);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(820, 605);
			this->Name = L"MyForm";
			this->Text = L"MyForm";
			this->ResumeLayout(false);

		}
#pragma endregion
		void btnRun_Click(System::Object^ sender, System::EventArgs^ e)
		{
			rtbOutput->Clear();

			std::string configFile, traceFile, algoStr;
			for (int i = 0; i < txtConfig->Text->Length; i++) configFile += (char)txtConfig->Text[i];
			for (int i = 0; i < txtTrace->Text->Length; i++) traceFile += (char)txtTrace->Text[i];
			for (int i = 0; i < cmbAlgo->Text->Length; i++) algoStr += (char)cmbAlgo->Text[i];

			int frameCount = 4;
			try { frameCount = Int32::Parse(txtFrame->Text); }
			catch (...) { rtbOutput->AppendText(L"ERROR!! Invalid frame count.\r\n"); }

			// Capture cout output
			std::ostringstream buffer;
			std::streambuf* oldBuf = std::cout.rdbuf(buffer.rdbuf());

			ConfigPhase cp;
			cp.loadConfig(configFile);
			cp.cfg.frameCount = (unsigned int)frameCount;
			cp.cfg.replacementAlgo = algoStr;
			cp.printConfig();

			SimulatorPhase sim(cp.cfg);
			if (algoStr == "OPT") sim.preScan(traceFile);
			sim.run(traceFile);

			std::cout.rdbuf(oldBuf);

			std::string result = buffer.str();
			System::String^ output = gcnew System::String(result.c_str());
			rtbOutput->AppendText(output->Replace(L"\n", L"\r\n"));
		}
	private: System::Void label4_Click(System::Object^ sender, System::EventArgs^ e) {
	}
};
}
