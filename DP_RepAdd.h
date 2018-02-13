// ---------------------------------------------------------------------------

#ifndef DP_RepAddH
#define DP_RepAddH
// ---------------------------------------------------------------------------
#include <System.Classes.hpp>

#include "DP_RepSettings.h"

#define sDot "• "

String StrToUrl(String S, bool UseUTF8);

String DToS(TDate ADate);
String DTToS(TDateTime ADateTime);
String DateTimeToMDBStr(TDateTime ADateTime);

String FmtFloat(Double Value);

void Split(TStringList* AOut, String S, String Separator);

String FormatDBResult(TObjectList* DBResults, TSettings* Settings,
	TStringList * Filter, TDateTime ADateTimeFrom, TDateTime ADateTimeTo);

// ---------------------------------------------------------------------------
#endif
