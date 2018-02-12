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

	TFileIni* FileIni = TFileIni::GetNewInstance();
	try {
		FileIni->ReadFormBounds(this);
	}
	__finally {
		delete FileIni;
	}

	Settings = new TSettings();
	Settings->Load();

	UpdateDateTime();
}
// ---------------------------------------------------------------------------

void __fastcall TMain::FormDestroy(TObject *Sender) {
	Settings->Free();

	TFileIni* FileIni = TFileIni::GetNewInstance();
	try {
		FileIni->WriteFormBounds(this);
	}
	__finally {
		delete FileIni;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TMain::FormShow(TObject *Sender) {
	bool NeedUpdate, OKUpdate;

	if (!IsShift()) {
		NeedUpdate = true;
		OKUpdate = true;

		if (FindCmdLineSwitch("UPDATE")) {
			OKUpdate = ReportUpdate();
			NeedUpdate = false;
		}

		if (FindCmdLineSwitch("MAIL")) {
			if (NeedUpdate) {
				OKUpdate = ReportUpdate();
				NeedUpdate = false;
			}
			if (OKUpdate) {
				ReportMail();
			}
		}

		if (FindCmdLineSwitch("SAVE")) {
			if (NeedUpdate) {
				OKUpdate = ReportUpdate();
			}
			if (OKUpdate) {
				ReportSave();
			}
		}
		if (FindCmdLineSwitch("CLOSE") && OKUpdate) {
			Close();
		}
	}
}
// ---------------------------------------------------------------------------

void __fastcall TMain::btnDatePrevClick(TObject * Sender) {
	DateTimePicker->Date = DateTimePicker->Date + ((TSpeedButton*) Sender)->Tag;

	Memo->Clear();

	UpdateDateTime();
}
// ---------------------------------------------------------------------------

void __fastcall TMain::DateTimePickerChange(TObject * Sender) {
	Memo->Clear();
	UpdateDateTime();
}
// ---------------------------------------------------------------------------

void TMain::UpdateDateTime() {
	TDateTime ADate = DateTimePicker->Date;

	Word Year, Month, Day;

	if (Settings->ReportType == 1) {
		DecodeDate(ADate - 1, Year, Month, Day);

		DateTimeFrom = TDateTime(Year, Month, Day, 20, 0, 0, 0);

		DecodeDate(ADate, Year, Month, Day);

		DateTimeTo = TDateTime(Year, Month, Day, 19, 59, 59, 999);
	}
	else {
		DecodeDate(ADate, Year, Month, Day);

		DateTimeFrom = TDateTime(Year, Month, Day, 0, 0, 0, 0);

		DateTimeTo = TDateTime(Year, Month, Day, 23, 59, 59, 999);
	}

	lblDateTime->Caption = Format(IDS_DATETIME_LABEL,
		ARRAYOFCONST((DTToS(DateTimeFrom), DTToS(DateTimeTo))));
}

void __fastcall TMain::tbtnAboutClick(TObject * Sender) {
	ShowAbout(14, MAXBYTE, MAXBYTE, MAXBYTE, NULL, NULL, NULL, NULL, NULL,
		LoadStr(IDS_COPYRIGHT));
}
// ---------------------------------------------------------------------------

void __fastcall TMain::FormKeyDown(TObject * Sender, WORD & Key,
	TShiftState Shift) {
	if (!Memo->Enabled) {
		return;
	}

	if (Shift.Empty()) {
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
}
// ---------------------------------------------------------------------------

bool TMain::CheckForSave() {
	bool Result = Memo->Lines->Text.IsEmpty();
	if (Result) {
		MsgBoxErr(LoadStr(IDS_ERROR_MEMO_CLEAR));
	}

	return Result;
}

void TMain::ReportSave() {
	if (CheckForSave()) {
		return;
	}

	Memo->Lines->SaveToFile(FileInAppDir(Format("%s.txt",
		DToS(DateTimePicker->Date))));
}

bool TMain::Ping(String HostAddr) {
	bool Result;

	HANDLE hIcmpFile;
	unsigned long ipaddr;
	DWORD dwRetVal;
	char SendData[32] = "Data Buffer";
	LPVOID ReplyBuffer;
	DWORD ReplySize;

	ipaddr = inet_addr(AnsiString(HostAddr).c_str());
	Result = ipaddr != INADDR_NONE;
	if (Result) {
		hIcmpFile = IcmpCreateFile();
		Result = hIcmpFile != INVALID_HANDLE_VALUE;

		if (Result) {
			ReplySize = sizeof(ICMP_ECHO_REPLY)+sizeof(SendData);
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

	TDB* DB;
	for (int i = 0; i < Settings->DBs->Count; i++) {
		DB = (TDB*)Settings->DBs->List[i];

		ProcMess();

		Host = DB->Ping;

		if (Host != "") {
			if (!Ping(Host)) {
				S = ConcatStrings(S, Format(IDS_PING_ERROR,
					ARRAYOFCONST((DB->Name, DB->Phone))),
					(String) sLineBreak + (String) sLineBreak);
			}
		}

	}

	RestoreCursor();

	if (S != "") {
		Result = MsgBox(S + sLineBreak + sLineBreak + LoadStr(IDS_PING_RESULT),
			MB_YESNOCANCEL | MB_ICONQUESTION);
	}

	return Result;
}

bool TMain::ReportUpdate() {
	int i, line;
	String S = "", Name = "", DBResult, DBResultOK, DBResultError;
	bool Result = true;

	ShowWaitCursor();

	try {
		tbtnUpdate->Enabled = false;
		tbtnMail->Enabled = false;
		tbtnSave->Enabled = false;

		PanelDate->Enabled = false;

		Memo->Lines->Clear();

		Memo->Lines->Add(LoadStr(IDS_UPDATE_WAIT) + sLineBreak);

		ProcMess();

		if (Settings->ReportType == 1) {
			S = Format(IDS_REPORT_CAPTION_1,
				ARRAYOFCONST((DTToS(DateTimeFrom), DTToS(DateTimeTo))));
		}
		else {
			S = Format(IDS_REPORT_CAPTION_0, DToS(DateTimeFrom));
		}

		S += sLineBreak;

		DBResultOK = LoadStr(IDS_UPDATE_OK);
		DBResultError = LoadStr(IDS_UPDATE_ERROR);

		TDB* DB;
		for (int i = 0; i < Settings->DBs->Count; i++) {
			DB = (TDB*)Settings->DBs->List[i];

			ProcMess();

			Name = DB->Name;

			S = S + sLineBreak + Name + sLineBreak;

			line = Memo->Lines->Add(Name + "... ");

			if (LoadData(DB->Path, DateTimeFrom, DateTimeTo, S)) {
				DBResult = DBResultOK;
			}
			else {
				Result = false;
				DBResult = DBResultError;
			}

			Memo->Lines->Strings[line] = Memo->Lines->Strings[line] + DBResult +
				sLineBreak;

			// Delay(2000);
		}
	}
	catch (Exception *E) {
		S = E->Message;
	}

	Delay(200);

	Memo->Text = S;

	tbtnUpdate->Enabled = true;
	tbtnMail->Enabled = true;
	tbtnSave->Enabled = true;
	PanelDate->Enabled = true;

	RestoreCursor();

	return Result;
}

void TMain::ReportMail() {
	if (CheckForSave()) {
		return;
	}

	ShowWaitCursor();

	try {
		if (!IsEmpty(Settings->MailClient)) {
			if (!EXEIsRunning(Settings->MailClient, true)) {
				ShellExec(Settings->MailClient);
				Delay(1000);
			}
		}

		String S = "";

		TMail* Mail;

		for (int i = 0; i < Settings->Mails->Count; i++) {
			Mail = (TMail*)Settings->Mails->List[i];

			S = Format(IDS_MAIL_TO,
				ARRAYOFCONST((Mail->Address, StrToUrl(Settings->MailSubject,
				Settings->MailUseUTF8), StrToUrl(Memo->Lines->Text,
				Settings->MailUseUTF8))));

			ShellExec(S);
		}
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

	if (Result != ID_CANCEL) {
		ReportUpdate();
	}
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
