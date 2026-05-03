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

	static System::String^ ToSysStr(const std::string& s) {
		return gcnew System::String(s.c_str());
	}

	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void) { InitializeComponent(); BuildUI(); }

	protected:
		~MyForm() { if (components) delete components; }

	private:
		System::ComponentModel::Container^ components;

		Color clrBg = Color::FromArgb(15, 17, 26);
		Color clrSurface = Color::FromArgb(22, 27, 42);
		Color clrCard = Color::FromArgb(30, 37, 58);
		Color clrAccent = Color::FromArgb(56, 189, 248);   // sky-blue
		Color clrGreen = Color::FromArgb(52, 211, 153);
		Color clrYellow = Color::FromArgb(251, 191, 36);
		Color clrRed = Color::FromArgb(248, 113, 113);
		Color clrText = Color::FromArgb(226, 232, 240);
		Color clrMuted = Color::FromArgb(100, 116, 139);

		// Fonts 
		System::Drawing::Font^ fntTitle = gcnew System::Drawing::Font("Segoe UI", 16, FontStyle::Bold);
		System::Drawing::Font^ fntSub = gcnew System::Drawing::Font("Segoe UI", 8, FontStyle::Regular);
		System::Drawing::Font^ fntLabel = gcnew System::Drawing::Font("Segoe UI", 9, FontStyle::Regular);
		System::Drawing::Font^ fntBold = gcnew System::Drawing::Font("Segoe UI", 9, FontStyle::Bold);
		System::Drawing::Font^ fntBtn = gcnew System::Drawing::Font("Segoe UI", 9, FontStyle::Bold);
		System::Drawing::Font^ fntCard = gcnew System::Drawing::Font("Segoe UI", 18, FontStyle::Bold);
		System::Drawing::Font^ fntCardLbl = gcnew System::Drawing::Font("Segoe UI", 8, FontStyle::Regular);
		System::Drawing::Font^ fntMono = gcnew System::Drawing::Font("Consolas", 9, FontStyle::Regular);

		// Input controls
		System::Windows::Forms::Label^ lblConfig;
		System::Windows::Forms::Label^ lblTrace;
		System::Windows::Forms::Label^ lblAlgo;
		System::Windows::Forms::TextBox^ txtConfig;
		System::Windows::Forms::TextBox^ txtTrace;
		System::Windows::Forms::ComboBox^ cmbAlgo;
		System::Windows::Forms::Button^ btnRun;
		System::Windows::Forms::Button^ btnClear;

		//Stats cards 
		System::Windows::Forms::Label^ cardTLBHit;
		System::Windows::Forms::Label^ cardFaults;
		System::Windows::Forms::Label^ cardEAT;
		System::Windows::Forms::Label^ cardDiskW;

		//Computed config display 
		System::Windows::Forms::Label^ cfgRamSize;
		System::Windows::Forms::Label^ cfgPageSize;
		System::Windows::Forms::Label^ cfgFrames;
		System::Windows::Forms::Label^ cfgOffBits;
		System::Windows::Forms::Label^ cfgOffMask;
		System::Windows::Forms::Label^ cfgTLBSize;
		System::Windows::Forms::Label^ cfgTLBLat;
		System::Windows::Forms::Label^ cfgRAMLat;
		System::Windows::Forms::Label^ cfgDiskLat;

		// Output tabs
		System::Windows::Forms::TabControl^ tabOut;
		System::Windows::Forms::TabPage^ tabLog;
		System::Windows::Forms::TabPage^ tabReport;
		System::Windows::Forms::RichTextBox^ rtbLog;
		System::Windows::Forms::RichTextBox^ rtbReport;

		//  Status bar 
		System::Windows::Forms::StatusStrip^ statusBar;
		System::Windows::Forms::ToolStripStatusLabel^ lblStatus;

		//  BUILD UI
		void BuildUI()
		{
			this->SuspendLayout();
			this->Text = L"Virtual Memory & TLB Simulator";
			this->Size = System::Drawing::Size(940, 780);
			this->MinimumSize = System::Drawing::Size(880, 700);
			this->BackColor = clrBg;
			this->ForeColor = clrText;
			this->Font = fntLabel;
			this->StartPosition = FormStartPosition::CenterScreen;

			//  Header bar 
			Panel^ pnlHeader = gcnew Panel();
			pnlHeader->Dock = DockStyle::Top;
			pnlHeader->Height = 70;
			pnlHeader->BackColor = clrSurface;

			Panel^ accentBar = gcnew Panel();
			accentBar->Size = System::Drawing::Size(4, 70);
			accentBar->Location = Point(0, 0);
			accentBar->BackColor = clrAccent;
			pnlHeader->Controls->Add(accentBar);

			Label^ lblTitle = MakeLbl(L"Virtual Memory & TLB Simulator", Point(20, 10), fntTitle, clrAccent);
			Label^ lblSub = MakeLbl(L"EC-210 OS Project  |  FIFO · LRU · OPT  |  EAT Analysis",
				Point(22, 44), fntSub, clrMuted);
			pnlHeader->Controls->Add(lblTitle);
			pnlHeader->Controls->Add(lblSub);
			this->Controls->Add(pnlHeader);

			// Input group 
			GroupBox^ grpIn = MakeGroup(L"Simulation Parameters", Point(14, 82),
				System::Drawing::Size(900, 118));

			// Config file row
			lblConfig = MakeLbl(L"Config File:", Point(10, 26), fntLabel, clrText);
			txtConfig = MakeTxt(Point(108, 23), System::Drawing::Size(570, 22), L"config.txt");
			grpIn->Controls->Add(lblConfig);
			grpIn->Controls->Add(txtConfig);

			// Trace file row
			lblTrace = MakeLbl(L"Trace File:", Point(10, 56), fntLabel, clrText);
			txtTrace = MakeTxt(Point(108, 53), System::Drawing::Size(570, 22), L"trace.txt");
			grpIn->Controls->Add(lblTrace);
			grpIn->Controls->Add(txtTrace);

			// Algorithm + buttons row
			lblAlgo = MakeLbl(L"Algorithm:", Point(10, 86), fntLabel, clrText);

			cmbAlgo = gcnew ComboBox();
			cmbAlgo->Items->Add(L"FIFO");
			cmbAlgo->Items->Add(L"LRU");
			cmbAlgo->Items->Add(L"OPT");
			cmbAlgo->SelectedIndex = 0;
			cmbAlgo->Location = Point(108, 83);
			cmbAlgo->Size = System::Drawing::Size(90, 22);
			cmbAlgo->DropDownStyle = ComboBoxStyle::DropDownList;
			cmbAlgo->BackColor = clrCard;
			cmbAlgo->ForeColor = clrText;
			cmbAlgo->FlatStyle = FlatStyle::Flat;

			btnRun = MakeBtn(L"▶  Run Simulation", Point(216, 81),
				System::Drawing::Size(160, 30), clrAccent, Color::Black);
			btnRun->Click += gcnew EventHandler(this, &MyForm::OnRun);

			btnClear = MakeBtn(L"✕  Clear", Point(384, 81),
				System::Drawing::Size(90, 30), clrCard, clrMuted);
			btnClear->Click += gcnew EventHandler(this, &MyForm::OnClear);

			grpIn->Controls->Add(lblAlgo);
			grpIn->Controls->Add(cmbAlgo);
			grpIn->Controls->Add(btnRun);
			grpIn->Controls->Add(btnClear);
			this->Controls->Add(grpIn);

			//Stats cards row 
			Panel^ pnlCards = gcnew Panel();
			pnlCards->Location = Point(14, 210);
			pnlCards->Size = System::Drawing::Size(900, 80);
			pnlCards->BackColor = Color::Transparent;

			MakeCard(pnlCards, 0, L"—", L"TLB Hit Rate", clrAccent, cardTLBHit);
			MakeCard(pnlCards, 224, L"—", L"Page Faults", clrRed, cardFaults);
			MakeCard(pnlCards, 448, L"—", L"EAT (ns)", clrGreen, cardEAT);
			MakeCard(pnlCards, 672, L"—", L"Disk Writes", clrYellow, cardDiskW);

			this->Controls->Add(pnlCards);

			//  Computed config display
			GroupBox^ grpCfg = MakeGroup(L"Computed System Config  (from config.txt)",
				Point(14, 300), System::Drawing::Size(900, 64));

			int cx = 10;
			AddCfgCell(grpCfg, L"RAM:", cx, cfgRamSize);
			AddCfgCell(grpCfg, L"Page Size:", cx + 150, cfgPageSize);
			AddCfgCell(grpCfg, L"Frames:", cx + 300, cfgFrames);
			AddCfgCell(grpCfg, L"TLB Size:", cx + 430, cfgTLBSize);
			AddCfgCell(grpCfg, L"TLB Lat:", cx + 560, cfgTLBLat);
			AddCfgCell(grpCfg, L"RAM Lat:", cx + 670, cfgRAMLat);
			AddCfgCell(grpCfg, L"Disk Lat:", cx + 780, cfgDiskLat);

			this->Controls->Add(grpCfg);

			// Output tabs 
			tabOut = gcnew TabControl();
			tabOut->Location = Point(14, 374);
			tabOut->Size = System::Drawing::Size(900, 340);
			tabOut->DrawMode = TabDrawMode::OwnerDrawFixed;
			tabOut->SizeMode = TabSizeMode::Fixed;
			tabOut->ItemSize = System::Drawing::Size(180, 30);
			tabOut->DrawItem += gcnew DrawItemEventHandler(this, &MyForm::OnDrawTab);

			tabLog = MakeTab(L"  Execution Log");
			tabReport = MakeTab(L"  Performance Report");
			tabOut->Controls->Add(tabLog);
			tabOut->Controls->Add(tabReport);

			rtbLog = MakeRTB(clrSurface, Color::FromArgb(148, 210, 189));
			tabLog->Controls->Add(rtbLog);

			rtbReport = MakeRTB(clrSurface, clrAccent);
			tabReport->Controls->Add(rtbReport);

			this->Controls->Add(tabOut);

			// Status bar 
			statusBar = gcnew StatusStrip();
			statusBar->BackColor = clrCard;
			statusBar->ForeColor = clrMuted;
			lblStatus = gcnew ToolStripStatusLabel();
			lblStatus->Text = L"  Ready.  Load config.txt and trace.txt, then click Run.";
			statusBar->Items->Add(lblStatus);
			this->Controls->Add(statusBar);

			this->ResumeLayout(false);
		}

		//  FACTORY HELPERS
		Label^ MakeLbl(String^ text, Point loc, System::Drawing::Font^ f, Color c)
		{
			Label^ l = gcnew Label();
			l->Text = text;
			l->Location = loc;
			l->AutoSize = true;
			l->Font = f;
			l->ForeColor = c;
			l->BackColor = Color::Transparent;
			return l;
		}

		TextBox^ MakeTxt(Point loc, System::Drawing::Size sz, String^ def)
		{
			TextBox^ t = gcnew TextBox();
			t->Text = def;
			t->Location = loc;
			t->Size = sz;
			t->BackColor = clrCard;
			t->ForeColor = clrText;
			t->BorderStyle = BorderStyle::FixedSingle;
			return t;
		}

		Button^ MakeBtn(String^ text, Point loc, System::Drawing::Size sz, Color bg, Color fg)
		{
			Button^ b = gcnew Button();
			b->Text = text;
			b->Location = loc;
			b->Size = sz;
			b->BackColor = bg;
			b->ForeColor = fg;
			b->FlatStyle = FlatStyle::Flat;
			b->FlatAppearance->BorderSize = 0;
			b->Font = fntBtn;
			b->Cursor = Cursors::Hand;
			return b;
		}

		GroupBox^ MakeGroup(String^ title, Point loc, System::Drawing::Size sz)
		{
			GroupBox^ g = gcnew GroupBox();
			g->Text = title;
			g->Location = loc;
			g->Size = sz;
			g->ForeColor = clrAccent;
			g->BackColor = clrSurface;
			g->Font = fntLabel;
			return g;
		}

		// One stat card: left accent stripe + big value label + small caption
		void MakeCard(Panel^ parent, int x, String^ val, String^ caption,
			Color accent, Label^% valLbl)
		{
			Panel^ card = gcnew Panel();
			card->Location = Point(x, 0);
			card->Size = System::Drawing::Size(218, 76);
			card->BackColor = clrCard;

			Panel^ stripe = gcnew Panel();
			stripe->Size = System::Drawing::Size(4, 76);
			stripe->Location = Point(0, 0);
			stripe->BackColor = accent;
			card->Controls->Add(stripe);

			valLbl = gcnew Label();
			valLbl->Text = val;
			valLbl->Location = Point(14, 8);
			valLbl->AutoSize = true;
			valLbl->Font = fntCard;
			valLbl->ForeColor = accent;
			valLbl->BackColor = Color::Transparent;
			card->Controls->Add(valLbl);

			Label^ cap = gcnew Label();
			cap->Text = caption;
			cap->Location = Point(14, 50);
			cap->AutoSize = true;
			cap->Font = fntCardLbl;
			cap->ForeColor = clrMuted;
			cap->BackColor = Color::Transparent;
			card->Controls->Add(cap);

			parent->Controls->Add(card);
		}

		// One config info cell: key label + value label below it
		void AddCfgCell(GroupBox^ grp, String^ key, int x, Label^% valLbl)
		{
			Label^ k = MakeLbl(key, Point(x, 16), fntCardLbl, clrMuted);
			valLbl = gcnew Label();
			valLbl->Text = L"—";
			valLbl->Location = Point(x, 34);
			valLbl->AutoSize = true;
			valLbl->Font = fntBold;
			valLbl->ForeColor = clrText;
			valLbl->BackColor = Color::Transparent;
			grp->Controls->Add(k);
			grp->Controls->Add(valLbl);
		}

		TabPage^ MakeTab(String^ title)
		{
			TabPage^ tp = gcnew TabPage();
			tp->Text = title;
			tp->BackColor = clrSurface;
			tp->ForeColor = clrText;
			return tp;
		}

		RichTextBox^ MakeRTB(Color bg, Color fg)
		{
			RichTextBox^ r = gcnew RichTextBox();
			r->Dock = DockStyle::Fill;
			r->BackColor = bg;
			r->ForeColor = fg;
			r->ReadOnly = true;
			r->ScrollBars = RichTextBoxScrollBars::Vertical;
			r->Font = fntMono;
			r->BorderStyle = BorderStyle::None;
			return r;
		}

		//  POPULATE COMPUTED CONFIG PANEL  
		void ShowComputedConfig(SystemConfig& cfg)
		{
			unsigned long long ramMB = cfg.physicalRamSize / (1024ULL * 1024ULL);
			unsigned long long ramKB = cfg.physicalRamSize / 1024ULL;

			cfgRamSize->Text = (ramMB > 0)
				? System::String::Format(L"{0} MB", ramMB)
				: System::String::Format(L"{0} KB", ramKB);
			cfgPageSize->Text = System::String::Format(L"{0} KB", cfg.pageSize / 1024);
			cfgFrames->Text = System::String::Format(L"{0}", cfg.frameCount);
			cfgTLBSize->Text = System::String::Format(L"{0}", cfg.tlbSize);
			cfgTLBLat->Text = System::String::Format(L"{0} ns", cfg.tlbLatency);
			cfgRAMLat->Text = System::String::Format(L"{0} ns", cfg.ramLatency);
			cfgDiskLat->Text = System::String::Format(L"{0} ns", cfg.diskLatency);
		}

		//  POPULATE STATS CARDS  
		void ShowStats(StatsPhase* s)
		{
			cardTLBHit->Text = System::String::Format(L"{0:F1}%", s->tlbHitRate());
			cardFaults->Text = s->pageFaults.ToString();
			cardEAT->Text = System::String::Format(L"{0:F0}", s->computeEAT());
			cardDiskW->Text = s->diskWrites.ToString();
		}

		//  TAB DRAW (custom colors)
		void OnDrawTab(Object^ sender, DrawItemEventArgs^ e)
		{
			TabControl^ tc = safe_cast<TabControl^>(sender);
			bool sel = (e->Index == tc->SelectedIndex);
			e->Graphics->FillRectangle(gcnew SolidBrush(sel ? clrSurface : clrCard), e->Bounds);
			if (sel)
				e->Graphics->FillRectangle(gcnew SolidBrush(clrAccent),
					Rectangle(e->Bounds.X, e->Bounds.Y, e->Bounds.Width, 2));
			StringFormat^ sf = gcnew StringFormat();
			sf->Alignment = StringAlignment::Center;
			sf->LineAlignment = StringAlignment::Center;
			e->Graphics->DrawString(tc->TabPages[e->Index]->Text, fntLabel,
				gcnew SolidBrush(sel ? clrAccent : clrMuted),
				RectangleF((float)e->Bounds.X, (float)e->Bounds.Y,
					(float)e->Bounds.Width, (float)e->Bounds.Height), sf);
		}

		//  EVENT HANDLERS
		void OnClear(Object^ sender, EventArgs^ e)
		{
			rtbLog->Clear();
			rtbReport->Clear();
			cardTLBHit->Text = L"—";
			cardFaults->Text = L"—";
			cardEAT->Text = L"—";
			cardDiskW->Text = L"—";
			cfgRamSize->Text = L"—";
			cfgPageSize->Text = L"—";
			cfgFrames->Text = L"—";
			cfgTLBSize->Text = L"—";
			cfgTLBLat->Text = L"—";
			cfgRAMLat->Text = L"—";
			cfgDiskLat->Text = L"—";
			lblStatus->Text = L"  Cleared.";
		}

		void OnRun(Object^ sender, EventArgs^ e)
		{
			rtbLog->Clear();
			rtbReport->Clear();
			lblStatus->Text = L"  Running simulation...";
			Application::DoEvents();

			// File paths from GUI 
			std::string configFile, traceFile;
			for (int i = 0; i < txtConfig->Text->Length; i++) configFile += (char)txtConfig->Text[i];
			for (int i = 0; i < txtTrace->Text->Length; i++) traceFile += (char)txtTrace->Text[i];

			// Algorithm from combo box 
			std::string algoStr;
			for (int i = 0; i < cmbAlgo->Text->Length; i++) algoStr += (char)cmbAlgo->Text[i];

			//  Load config.txt — all numeric values come from the file
			// frameCount = physicalRamSize / pageSize  (computed in phase1.h)
			// offsetBits = log2(pageSize)              (computed in phase1.h)
			// offsetMask = pageSize - 1                (computed in phase1.h)
			// GUI does NOT override any of these.
			ConfigPhase cp;
			cp.loadConfig(configFile);
			cp.cfg.replacementAlgo = algoStr;   // only user choice, not a numeric value

			// Show what phase1 computed in the config panel 
			ShowComputedConfig(cp.cfg);

			//Redirect cout → buffers 
			std::ostringstream logBuf, repBuf;
			std::streambuf* oldBuf = std::cout.rdbuf(logBuf.rdbuf());

			cp.printConfig();

			SimulatorPhase sim(cp.cfg);
			if (algoStr == "OPT") sim.preScan(traceFile);
			sim.run(traceFile);

			// Capture the report separately
			std::cout.rdbuf(repBuf.rdbuf());
			sim.stats->printReport();
			std::cout.rdbuf(oldBuf);

			// Execution Log tab 
			rtbLog->AppendText(ToSysStr(logBuf.str())->Replace(L"\n", L"\r\n"));

			//Performance Report tab 
			rtbReport->AppendText(ToSysStr(repBuf.str())->Replace(L"\n", L"\r\n"));

			//Stats cards 
			ShowStats(sim.stats);

			// Auto-switch to report tab 
			tabOut->SelectedTab = tabReport;

			lblStatus->Text = System::String::Format(
				L"  Done.  Accesses: {0}  |  Page Faults: {1}  |  TLB Hit Rate: {2:F1}%  |  EAT: {3:F1} ns",
				sim.stats->totalAccesses,
				sim.stats->pageFaults,
				sim.stats->tlbHitRate(),
				sim.stats->computeEAT());
		}

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->components = gcnew System::ComponentModel::Container();
			this->SuspendLayout();
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(940, 780);
			this->Name = L"MyForm";
			this->ResumeLayout(false);
		}
#pragma endregion
	};
}
