//---------------------------------------------------------------------------

#ifndef DP_RepAddH
#define DP_RepAddH
//---------------------------------------------------------------------------
#endif

bool LoadData(String AFolderName,
	TDateTime ADateFrom, TDateTime ADateTo, String &ARecords);

String StrToUrl(String S, bool UseUTF8);
String DToS(TDate ADate);
String DTToS(TDateTime ADateTime);
String DateTimeToMDBStr(TDateTime ADateTime);
String FmtFloat(Double Value);
