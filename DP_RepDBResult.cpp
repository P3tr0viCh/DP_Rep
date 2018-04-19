// ---------------------------------------------------------------------------

#pragma hdrstop

#include <UtilsLog.h>
#include <UtilsStr.h>
#include <UtilsMisc.h>

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

	FRecords = new TDBRecordList();

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
bool TDBResult::InternalLoadData(TDateTime ADateTimeFrom, TDateTime ADateTimeTo)
{
	Clear();

	String DBPath = DB->Path;

	if (IsEmpty(DBPath)) {
		FErrorMessage = LoadStr(IDS_ERROR_DB_PATH_NOT_EXISTS);
		return false;
	}

	DBPath = IncludeTrailingPathDelimiter(DBPath) + "Base.mdb";

	if (!FileExists(DBPath)) {
		FErrorMessage = Format(IDS_ERROR_DB_NOT_EXISTS, DBPath);
		return false;
	}

	try {
		FConnection->ConnectionString =
			Format(IDS_SQL_CONNECTION, ARRAYOFCONST((DBPath, PASSWORD)));

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

// ---------------------------------------------------------------------------
bool TDBResult::LoadData(TDateTime ADateTimeFrom, TDateTime ADateTimeTo) {
	WriteToLog(Format(IDS_LOG_LOAD_DATA_START, DB->Name));

	bool Result = InternalLoadData(ADateTimeFrom, ADateTimeTo);

	if (Result) {
		WriteToLog(Format(IDS_LOG_LOAD_DATA_END_OK,
			ARRAYOFCONST((DB->Name, LoadStr(IDS_LOG_OK)))));
	}
	else {
		WriteToLog(Format(IDS_LOG_LOAD_DATA_END_FAIL,
			ARRAYOFCONST((DB->Name, LoadStr(IDS_LOG_FAIL), ErrorMessage))));
	}

	return Result;
}

// ---------------------------------------------------------------------------
