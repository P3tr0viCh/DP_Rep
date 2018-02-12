// ---------------------------------------------------------------------------

#ifndef DP_RepSettingsH
#define DP_RepSettingsH
// ---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <System.Contnrs.hpp>

#include "DP_RepSettingsDB.h"
#include "DP_RepSettingsMail.h"

// ---------------------------------------------------------------------------
class TSettings : public TObject {
private:
	int FReportType;

	bool FMailUseUTF8;
	String FMailClient;
	String FMailSubject;

	TObjectList* FDBs;
	TObjectList* FMails;

public:
	__fastcall TSettings();
	__fastcall ~TSettings();

	void __fastcall Load();

	// 0 -- отчёт за сутки с 00:00 до 23:59
	// 1 -- отчёт за бухгалтерские сутки с 20:00 до 00:00 предыдущего дня
	// и с 00:00 до 19:59 текущего дня
	__property int ReportType = {read = FReportType};

	__property bool MailUseUTF8 = {read = FMailUseUTF8};
	__property String MailClient = {read = FMailClient};
	__property String MailSubject = {read = FMailSubject};

	__property TObjectList* DBs = {read = FDBs};
	__property TObjectList* Mails = {read = FMails};
};

// ---------------------------------------------------------------------------
#endif
