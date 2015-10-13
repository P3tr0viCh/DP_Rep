// ---------------------------------------------------------------------------

#include <vcl.h>

#include <winsock2.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include <stdio.h>

#pragma hdrstop

#include "DP_RepMain.h"
#include "DP_RepAdd.h"
#include "DP_RepStrings.h"

#include <AboutFrm.h>

#include <UtilsMisc.h>
#include <UtilsFileIni.h>
#include <UtilsStr.h>
#include <UtilsFiles.h>
#include <UtilsKAndM.h>

// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma resource "*.dfm"

TMain *Main;

// ---------------------------------------------------------------------------
__fastcall TMain::TMain(TComponent* Owner) : TForm(Owner) {
}
// ---------------------------------------------------------------------------

void __fastcall TMain::tbtnCloseClick(TObject *Sender) {
	Close();
}
// ---------------------------------------------------------------------------

void __fastcall TMain::FormCreate(TObject *Sender) {
	DateTimePicker->Date = Date() - 1;

	TFileIni* FileIni = CreateINIFile();

	FileIni->ReadFormBounds(this);

	delete FileIni;
}
// ---------------------------------------------------------------------------

void __fastcall TMain::FormDestroy(TObject *Sender) {
	TFileIni* FileIni = CreateINIFile();

	FileIni->WriteFormBounds(this);

	delete FileIni;
}

// ---------------------------------------------------------------------------
void __fastcall TMain::FormShow(TObject *Sender) {
	bool NeedUpdate, OKUpdate;

	if (!IsShift()) {
		NeedUpdate = true;
		OKUpdate   = true;

		if (FindCmdLineSwitch("UPDATE")) {
			OKUpdate   = ReportUpdate();
			NeedUpdate = false;
		}

		if (FindCmdLineSwitch("MAIL")) {
			if (NeedUpdate) {
				OKUpdate   = ReportUpdate();
				NeedUpdate = false;
			}
			if (OKUpdate)
				ReportMail();
		}

		if (FindCmdLineSwitch("SAVE")) {
			if (NeedUpdate)
				OKUpdate = ReportUpdate();
			if (OKUpdate)
				ReportSave();
		}
		if (FindCmdLineSwitch("CLOSE") & OKUpdate)
			Close();
	}
}
// ---------------------------------------------------------------------------

void __fastcall TMain::btnDatePrevClick(TObject * Sender) {
	DateTimePicker->Date = DateTimePicker->Date + ((TSpeedButton*) Sender)->Tag;
	Memo->Clear();
}
// ---------------------------------------------------------------------------

void __fastcall TMain::DateTimePickerChange(TObject * Sender) {
	Memo->Clear();
}
// ---------------------------------------------------------------------------

void __fastcall TMain::tbtnAboutClick(TObject * Sender) {
	ShowAbout(18, MAXBYTE, 3, NULL, NULL, NULL, NULL, NULL,
		"© Ï3òð0âè×ú (Ê.Ï. Äóðàåâ, Óðàëüñêàÿ Ñòàëü, ÖÂÒÑ, 2004-2015)|@n.gololobov@uralsteel.com");
}
// ---------------------------------------------------------------------------

void __fastcall TMain::FormKeyDown(TObject * Sender, WORD & Key,
	TShiftState Shift) {
	if (!Memo->Enabled)
		return;

	if (Shift.Empty())
		switch (Key) {
		case VK_F1:
			tbtnAbout->Click();
			break;
		case VK_F3:
			tbtnSave->Click();
			break;
		case VK_F5:
			tbtnUpdate->Click();
			break;
		case VK_F8:
			tbtnMail->Click();
			break;
		}
}
// ---------------------------------------------------------------------------

bool TMain::CheckForSave() {
	bool Result = Memo->Lines->Text.IsEmpty();
	if (Result)
		MsgBoxErr(LoadStr(IDS_ERROR_MEMO_CLEAR));
	return Result;
}

void TMain::ReportSave() {
	if (CheckForSave())
		return;
	Memo->Lines->SaveToFile(FileInAppDir(Format("%s.txt",
		DToS(DateTimePicker->Date))));
}

bool TMain::Ping(String HostName) {
	bool Result;

	HANDLE hIcmpFile;
	unsigned long ipaddr;
	DWORD dwRetVal;
	char SendData[32]    = "Data Buffer";
	LPVOID ReplyBuffer;
	DWORD ReplySize;

	ipaddr = inet_addr(AnsiString(HostName).c_str());
	Result = ipaddr != INADDR_NONE;
	if (Result) {

		hIcmpFile = IcmpCreateFile();
		Result    = hIcmpFile != INVALID_HANDLE_VALUE;
		if (Result) {

			ReplySize   = sizeof(ICMP_ECHO_REPLY)+sizeof(SendData);
			ReplyBuffer = (VOID*) malloc(ReplySize);

			Result = ReplyBuffer != NULL;
			if (Result) {

				dwRetVal = IcmpSendEcho(hIcmpFile, ipaddr, SendData,
					sizeof(SendData), NULL, ReplyBuffer, ReplySize, 1000);

				Result = dwRetVal != 0;
				if (Result) {
					PICMP_ECHO_REPLY pEchoReply =
						(PICMP_ECHO_REPLY) ReplyBuffer;

					Result = pEchoReply->Status == IP_SUCCESS;
				}
			}

			IcmpCloseHandle(hIcmpFile);
		}
	}

	return Result;
}

int TMain::CheckPing() {
	int Result = ID_NO;
	String Host, Phone, S = "";

	ShowWaitCursor();

	TFileIni* FileIni = CreateINIFile();

	for (int i = 1; i <= FileIni->ReadInteger("DB", "Count", 0); i++) {
		ProcMess();

		Host = FileIni->ReadString("DB", "Ping" + IntToStr(i), "");

		if (Host != "")
			if (!Ping(Host))
				S = ConcatStrings(S, Format(IDS_PING_ERROR,
				ARRAYOFCONST((FileIni->ReadString("DB", "Name" + IntToStr(i),
				""), FileIni->ReadString("DB", "Phone" + IntToStr(i), "")))),
				(String) sLineBreak + (String) sLineBreak);
	}

	FileIni->Free();

	RestoreCursor();

	if (S != "")
		Result = MsgBox(S + sLineBreak + sLineBreak + LoadStr(IDS_PING_RESULT),
		MB_YESNOCANCEL | MB_ICONQUESTION);

	return Result;
}

bool TMain::ReportUpdate() {
	int i, line;
	String S    = "", Name = "", DBResult, DBResultOK, DBResultError;
	bool Result = true;

	ShowWaitCursor();

	tbtnUpdate->Enabled = false;
	tbtnMail->Enabled   = false;
	tbtnSave->Enabled   = false;

	PanelDate->Enabled = false;

	Memo->Lines->Clear();

	Memo->Lines->Add(LoadStr(IDS_UPDATE_WAIT) + sLineBreak);

	ProcMess();

	S = Format(IDS_REPORT_CAPTION, DToS(DateTimePicker->Date)) + sLineBreak;

	DBResultOK    = LoadStr(IDS_UPDATE_OK);
	DBResultError = LoadStr(IDS_UPDATE_ERROR);

	TFileIni* FileIni = CreateINIFile();

	for (int i = 1; i <= FileIni->ReadInteger("DB", "Count", 0); i++) {
		ProcMess();

		Name = FileIni->ReadString("DB", "Name" + IntToStr(i), "");

		S = S + sLineBreak + Name + sLineBreak;

		line = Memo->Lines->Add(Name + "... ");

		if (LoadData(FileIni->ReadString("DB", "Path" + IntToStr(i), ""),
			DateTimePicker->Date, S))
			DBResult = DBResultOK;
		else {
			Result   = false;
			DBResult = DBResultError;
		}

		Memo->Lines->Strings[line] = Memo->Lines->Strings[line] + DBResult +
			sLineBreak;

		// Delay(2000);
	}

	FileIni->Free();

	Delay(200);

	Memo->Text = S;

	tbtnUpdate->Enabled = true;
	tbtnMail->Enabled   = true;
	tbtnSave->Enabled   = true;
	PanelDate->Enabled  = true;

	RestoreCursor();

	return Result;
}

void TMain::ReportMail() {
	if (CheckForSave())
		return;

	ShowWaitCursor();

	try {
		TFileIni* FileIni = CreateINIFile();

		bool UseUTF8      = FileIni->ReadBool("E-Mail", "UseUTF8", false);
		String MailClient = FileIni->ReadString("E-Mail", "Client", "");

		String S = Format(IDS_MAIL_TO,
			ARRAYOFCONST((FileIni->ReadString("E-Mail", "Address", ""),
			StrToUrl(FileIni->ReadString("E-Mail", "Subject", ""), UseUTF8),
			StrToUrl(Memo->Lines->Text, UseUTF8))));

		delete FileIni;

		// MsgBox(S); CopyToClipBoard(S); RestoreCursor(); return;

		if (MailClient != "") {
			if (!EXEIsRunning(MailClient, true)) {
				ShellExec(MailClient);
				Delay(1000);
			}
		}

		ShellExec(S);

	}
	catch (Exception *E) {
		MsgBoxErr(E->Message);
	}

	RestoreCursor();
}

void __fastcall TMain::miCopyClick(TObject * Sender) {
	CopyToClipBoard(Memo->Lines->Text);
}
// ---------------------------------------------------------------------------

void __fastcall TMain::tbtnUpdateClick(TObject * Sender) {
	int Result;

	do {
		Result = CheckPing();
	}
	while (Result == ID_YES);

	if (Result != ID_CANCEL) ReportUpdate();
}
// ---------------------------------------------------------------------------

void __fastcall TMain::tbtnMailClick(TObject * Sender) {
	ReportMail();
}
// ---------------------------------------------------------------------------

void __fastcall TMain::tbtnSaveClick(TObject * Sender) {
	ReportSave();
}
// ---------------------------------------------------------------------------
