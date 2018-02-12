// ---------------------------------------------------------------------------

#ifndef DP_RepSettingsDBH
#define DP_RepSettingsDBH
// ---------------------------------------------------------------------------
#include <System.Classes.hpp>

// ---------------------------------------------------------------------------
class TDB : public TObject {
private:
	String FName;
	String FPath;
	String FPing;
	String FPhone;

public:
	__fastcall TDB();

	__property String Name = {read = FName, write = FName};
	__property String Path = {read = FPath, write = FPath};
	__property String Ping = {read = FPing, write = FPing};
	__property String Phone = {read = FPhone, write = FPhone};
};

// ---------------------------------------------------------------------------
#endif
