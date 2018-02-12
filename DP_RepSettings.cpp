// ---------------------------------------------------------------------------

#pragma hdrstop

#include "DP_RepSettings.h"

#include "DP_RepAdd.h"

#include "UtilsStr.h"
#include "UtilsFileIni.h"

// ---------------------------------------------------------------------------
#pragma package(smart_init)

// ---------------------------------------------------------------------------
__fastcall TSettings::TSettings() {
	FDBs = new TObjectList();
	FMails = new TObjectList();

	FReportType = 0;

	FMailUseUTF8 = false;
	FMailClient = "";
	FMailSubject = "";
}

// ---------------------------------------------------------------------------
__fastcall TSettings::~TSettings() {
	FMails->Free();
	FDBs->Free();
}

// ---------------------------------------------------------------------------
void __fastcall TSettings::Load() {
	TFileIni* FileIni = TFileIni::GetNewInstance();
	try {
		FReportType = FileIni->ReadInteger("Report", "Type", FReportType);

		FMailUseUTF8 = FileIni->ReadBool("E-Mail", "UseUTF8", FMailUseUTF8);
		FMailClient = FileIni->ReadString("E-Mail", "Client", FMailClient);
		FMailSubject = FileIni->ReadString("E-Mail", "Subject", FMailSubject);

		TStringList * List = new TStringList();

		FileIni->ReadSection("DB", List);

		TDB* DB;

		for (int i = 0; i < List->Count; i++) {
			String DBSection = List->Strings[i];

			if (FileIni->SectionExists(DBSection)) {
				if (!FileIni->ReadBool("DB", DBSection, false)) {
					continue;
				}

				DB = new TDB();
				DB->Name = FileIni->ReadString(DBSection, "Name", DB->Name);
				DB->Path = FileIni->ReadString(DBSection, "Path", DB->Path);
				DB->Ping = FileIni->ReadString(DBSection, "Ping", DB->Ping);
				DB->Phone = FileIni->ReadString(DBSection, "Phone", DB->Phone);

				DBs->Add(DB);
			}
		}

		FileIni->ReadSection("E-Mail", List);

		TMail* Mail;

		for (int i = 0; i < List->Count; i++) {
			String MailSection = List->Strings[i];

			String Filter;

			if (FileIni->SectionExists(MailSection)) {
				if (!FileIni->ReadBool("E-Mail", MailSection, false)) {
					continue;
				}

				Mail = new TMail();

				Mail->Address = FileIni->ReadString(MailSection, "Address",
					Mail->Address);

				Filter = FileIni->ReadString(MailSection, "Filter", "");

				Split(Mail->Filter, Filter, ";");

				Mails->Add(Mail);
			}
		}

		List->Free();
	}
	__finally {
		delete FileIni;
	}
}
