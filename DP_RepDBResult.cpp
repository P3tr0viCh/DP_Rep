// ---------------------------------------------------------------------------

#pragma hdrstop

#include <UtilsMisc.h>
#include <UtilsStr.h>

#include "DP_RepPass.h"
#include "DP_RepStrings.h"
#include "DP_RepAdd.h"

#include "DP_RepDBResult.h"

// ---------------------------------------------------------------------------
#pragma package(smart_init)

// ---------------------------------------------------------------------------
__fastcall TDBResult::TDBResult(TADOConnection* AConnection, TADOQuery* AQuery,
	TDB* ADB) {
	FConnection = AConnection;
	FQuery = AQuery;

	FDB = ADB;

	FRecords = new TObjectList();

	Clear();
}

// ---------------------------------------------------------------------------
__fastcall TDBResult::~TDBResult() {
	// FConnection->Free();
	FRecords->Free();
}

// ---------------------------------------------------------------------------
void TDBResult::Clear() {
	FErrorMessage = NULL;
	FRecords->Clear();
}

// ---------------------------------------------------------------------------
bool TDBResult::LoadData(TDateTime ADateTimeFrom, TDateTime ADateTimeTo) {
	Clear();

	String DB = FDB->Path;

	if (IsEmpty(DB)) {
		FErrorMessage = LoadStr(IDS_ERROR_DB_PATH_NOT_EXISTS);
		return false;
	}

	DB = IncludeTrailingPathDelimiter(DB) + "Base.mdb";

	if (!FileExists(DB)) {
		FErrorMessage = Format(IDS_ERROR_DB_NOT_EXISTS, DB);
		return false;
	}

	try {
		FConnection->ConnectionString =
			Format(IDS_SQL_CONNECTION, ARRAYOFCONST((DB, PASSWORD)));

		FConnection->Open();
	}
	catch (Exception *E) {
		FErrorMessage = Format(IDS_ERROR_DB_OPEN, E->Message);
		return false;
	}

	try {
		FQuery->SQL->Clear();

		FQuery->SQL->Add(Format(IDS_SQL_QUERY,
			ARRAYOFCONST((DateTimeToMDBStr(ADateTimeFrom),
			DateTimeToMDBStr(ADateTimeTo)))));

		FQuery->Open();
	}
	catch (Exception *E) {
		FErrorMessage = Format(IDS_ERROR_DB_LOAD, E->Message);
		return false;
	}

	while (!FQuery->Eof) {
		// S = S + sDot + Format(IDS_DB_RECORD,
		// ARRAYOFCONST((Main->Query->Fields->Fields[0]->AsString,
		// FmtFloat(Main->Query->Fields->Fields[1]->AsFloat)))) + sLineBreak;

		FRecords->Add(new TDBRecord(FQuery->Fields->Fields[0]->AsString,
			FQuery->Fields->Fields[1]->AsFloat));

		FQuery->Next();
	}

	FQuery->Close();
	FConnection->Close();

	return true;
}
