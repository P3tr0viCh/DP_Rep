// ---------------------------------------------------------------------------

#ifndef DP_RepAddH
#define DP_RepAddH
// ---------------------------------------------------------------------------

#define sDot "• "

// ---------------------------------------------------------------------------
bool LoadData(String AFolderName, TDateTime ADateFrom, TDateTime ADateTo,
	String &ARecords);

String StrToUrl(String S, bool UseUTF8);
String DToS(TDate ADate);
String DTToS(TDateTime ADateTime);
String DateTimeToMDBStr(TDateTime ADateTime);
String FmtFloat(Double Value);
void Split(TStringList* AOut, String S, String Separator);

// ---------------------------------------------------------------------------
#endif
