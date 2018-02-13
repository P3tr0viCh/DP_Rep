// ---------------------------------------------------------------------------

#pragma hdrstop

#include <System.SysUtils.hpp>

#include <UtilsMisc.h>
#include <UtilsStr.h>

#include "DP_RepStrings.h"

#include "DP_RepAdd.h"

#include "DP_RepDBResult.h"

// ---------------------------------------------------------------------------
#pragma package(smart_init)

// ---------------------------------------------------------------------------
// TODO: Переделать эту шляпу >:[
String StrToUrl(String S, bool UseUTF8) {
	int Code, i;
	String Result = "";

	if (UseUTF8) {
		UTF8String U = S;

		for (i = 1; i <= U.Length(); i++) {
			if ((U[i] >= 'A' && U[i] <= 'Z') || (U[i] >= 'a' && U[i] <= 'z') ||
				(U[i] >= '0' && U[i] <= '9')) {
				Result = Result + U[i];
			}
			else
				switch (U[i]) {
				case '\r':
					break; // TODO: Забыл, к чему это todo :(
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

					if (Code < 0) {
						Code += 256;
					}
					
					Result = Result + '%' + IntToHex(Code, 2);
				}
		} // for
	}
	else {
		AnsiString A = S;

		for (i = 1; i <= A.Length(); i++) {
			if ((A[i] >= 'A' && A[i] <= 'Z') || (A[i] >= 'a' && A[i] <= 'z') ||
				(A[i] >= '0' && A[i] <= '9')) {
				Result = Result + A[i];
			}
			else
				switch (A[i]) {
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
					Result = Result + A[i];
					break;
				default:
					Code = A[i];

					if (Code < 0) {
						Code += 256;
					}
					
					Result = Result + '%' + IntToHex(Code, 2);
				}
		} // for
	}

	return Result;
}

// ---------------------------------------------------------------------------
String DToS(TDate ADate) {
	return FormatDateTime("yyyy.mm.dd", ADate);
}

// ---------------------------------------------------------------------------
String DTToS(TDateTime ADateTime) {
	return FormatDateTime("yyyy/mm/dd hh:nn", ADateTime);
}

// ---------------------------------------------------------------------------
String DateTimeToMDBStr(TDateTime ADateTime) {
	return FormatDateTime("M'/'d'/'yyyy' 'h':'n':'s", ADateTime);
}

// ---------------------------------------------------------------------------
String FmtFloat(Double Value) {
	return FormatFloat("0.00", Value);
}

// ---------------------------------------------------------------------------
void Split(TStringList* AOut, String S, String Separator) {
	if (IsEmpty(S)) {
		return;
	}

	if (Trim(S) == "*") {
		return;
	}

	S += ";";

	String S2;
	do {
		SplitStr(S, Separator, 0, S2, S);

		S2 = Trim(S2);

		if (!IsEmpty(S2)) {
			AOut->Add(S2);
		}
	}
	while (!IsEmpty(S));
}

// ---------------------------------------------------------------------------
String FormatDBResult(TObjectList * DBResults, TSettings * Settings,
	TStringList * Filter, TDateTime ADateTimeFrom, TDateTime ADateTimeTo) {
	String Result = "";

	TDBResult* DBResult;
	TDBRecord* DBRecord;

	String Product;

	bool UseFilter = Filter != NULL && Filter->Count > 0;

	String SDBResult;

	for (int i = 0; i < DBResults->Count; i++) {
		DBResult = (TDBResult*)DBResults->List[i];

		SDBResult = "";

		if (IsEmpty(DBResult->ErrorMessage)) {
			if (DBResult->Records->Count > 0) {
				for (int i = 0; i < DBResult->Records->Count; i++) {
					DBRecord = (TDBRecord*)DBResult->Records->List[i];

					Product = DBRecord->Product;

					if (UseFilter) {
						if (Filter->IndexOf(Product) >= 0) {
							SDBResult +=
								sDot + Format(IDS_DB_RECORD,
								ARRAYOFCONST((Product,
								FmtFloat(DBRecord->Weigh)))) + sLineBreak;
						}
					}
					else {
						SDBResult +=
							sDot + Format(IDS_DB_RECORD,
							ARRAYOFCONST((Product, FmtFloat(DBRecord->Weigh))))
							+ sLineBreak;
					}
				}
			}
			else {
				if (!UseFilter) {
					SDBResult = sDot + LoadStr(IDS_DB_NO_RECORDS) + sLineBreak;
				}
			}
		}
		else {
			if (!UseFilter) {
				SDBResult = sDot + DBResult->ErrorMessage + sLineBreak;
			}
		}

		if (!IsEmpty(SDBResult)) {
			SDBResult = sLineBreak + DBResult->DB->Name + sLineBreak +
				SDBResult;
		}

		Result += SDBResult;
	}

	String ReportCaption;
	if (Settings->ReportType == 1) {
		ReportCaption = Format(IDS_REPORT_CAPTION_1,
			ARRAYOFCONST((DTToS(ADateTimeFrom), DTToS(ADateTimeTo))));
	}
	else {
		ReportCaption = Format(IDS_REPORT_CAPTION_0, DToS(ADateTimeFrom));
	}
	ReportCaption += sLineBreak;

	if (IsEmpty(Result)) {
		Result = sLineBreak;
		Result += sDot + LoadStr(IDS_DB_NO_RECORDS) + sLineBreak;
	}

	Result = ReportCaption + Result;

	return Result;
}
