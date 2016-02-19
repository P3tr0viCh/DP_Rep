// ---------------------------------------------------------------------------

#pragma hdrstop

#include <System.SysUtils.hpp>

#include "DP_RepMain.h"

#include "DP_RepAdd.h"
#include "DP_RepPass.h"
#include "DP_RepStrings.h"

#include <UtilsMisc.h>
#include <UtilsStr.h>

// ---------------------------------------------------------------------------
#pragma package(smart_init)

#define sDot "• "

bool LoadData(String AFolderName,
	TDateTime ADateFrom, TDateTime ADateTo, String &ARecords) {
	String DB, S = "";

	bool Result;

	if (AFolderName == "")
		return false;

	ProcMess();

	DB = IncludeTrailingPathDelimiter(AFolderName) + "Base.mdb";

	Result = FileExists(DB);
	if (Result) {
		Main->Connection->ConnectionString =
			Format(IDS_SQL_CONNECTION, ARRAYOFCONST((DB, PASSWORD)));

		// S = Main->Connection->ConnectionString; Result = false;

		try {
			Main->Connection->Open();
		}
		catch (Exception *E) {
			S      = Format(IDS_ERROR_DB_OPEN, E->Message);
			Result = false;
		}

		if (Result) {
			Main->Query->SQL->Clear();

			Main->Query->SQL->Add
				(Format(IDS_SQL_QUERY,
					ARRAYOFCONST((DateTimeToMDBStr(ADateFrom),
						DateTimeToMDBStr(ADateTo)))));

//			S = Main->Query->SQL[0][0]; Result = false;

			try {
				Main->Query->Open();
			}
			catch (Exception *E) {
				S      = Format(IDS_ERROR_DB_LOAD, E->Message);
				Result = false;
			}

			if (Result) {
				while (!Main->Query->Eof) {
					S = S + sDot + Format(IDS_DB_RECORD,
						ARRAYOFCONST((Main->Query->Fields->Fields[0]->AsString,
						FmtFloat(Main->Query->Fields->Fields[1]->AsFloat)))) +
						sLineBreak;
					Main->Query->Next();
				}

				if (S == "")
					S = sDot + LoadStr(IDS_DB_NO_RECORDS) + sLineBreak;
			}
		}
	}
	else
		S = Format(IDS_ERROR_DB_NOT_EXISTS, DB);

	if (Result)
		ARecords = ARecords + S;
	else
		ARecords = ARecords + sDot + S + sLineBreak;

	Main->Query->Close();
	Main->Connection->Close();

	return Result;
}

String StrToUrl(String S, bool UseUTF8) {
	int Code, i;
	String Result = "";

	if (UseUTF8) {
		UTF8String U = S;

		for (i = 1; i <= U.Length(); i++) {
			if ((U[i] >= 'A' & U[i] <= 'Z') | (U[i] >= 'a' & U[i] <= 'z') |
				(U[i] >= '0' & U[i] <= '9')) {
				Result = Result + U[i];
			}
			else
				switch (U[i]) {
				case '\r':
					break; // TODO
				case '!':
				case '*':
				case '\'':
				case '(':
				case ')':
				case ';':
				case ':':
				case '@':
				case '&':
				case '=':
				case '+':
				case '$':
				case ',':
				case '/':
				case '?':
				case '#':
				case '[':
				case ']':

				case '-':
				case '_':
				case '.':
				case '~':
					Result = Result + U[i];
					break;
				default:
					Code = U[i];

					if (Code < 0)
						Code += 256;
					Result = Result + '%' + IntToHex(Code, 2);
				}
		} // for
	}
	else {
		AnsiString U = S;

		for (i = 1; i <= U.Length(); i++) {
			if ((U[i] >= 'A' & U[i] <= 'Z') | (U[i] >= 'a' & U[i] <= 'z') |
				(U[i] >= '0' & U[i] <= '9')) {
				Result = Result + U[i];
			}
			else
				switch (U[i]) {
				case '\r':
					break; // TODO
				case '!':
				case '*':
				case '\'':
				case '(':
				case ')':
				case ';':
				case ':':
				case '@':
				case '&':
				case '=':
				case '+':
				case '$':
				case ',':
				case '/':
				case '?':
				case '#':
				case '[':
				case ']':

				case '-':
				case '_':
				case '.':
				case '~':
					Result = Result + U[i];
					break;
				default:
					Code = U[i];

					if (Code < 0)
						Code += 256;
					Result = Result + '%' + IntToHex(Code, 2);
				}
		} // for
	}

	return Result;
}

String DToS(TDate ADate) {
	return FormatDateTime("yyyy.mm.dd", ADate);
}

String DTToS(TDateTime ADateTime) {
	return FormatDateTime("yyyy/mm/dd hh:nn", ADateTime);
}

String DateTimeToMDBStr(TDateTime ADateTime) {
	return FormatDateTime("M'/'d'/'yyyy' 'h':'n':'s", ADateTime);
}

String FmtFloat(Double Value) {
	return FormatFloat("0.00", Value);
}
