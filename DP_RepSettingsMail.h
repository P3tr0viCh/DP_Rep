// ---------------------------------------------------------------------------

#ifndef DP_RepSettingsMailH
#define DP_RepSettingsMailH
// ---------------------------------------------------------------------------
#include <System.Classes.hpp>

// ---------------------------------------------------------------------------
class TMail : public TObject {
private:
	String FAddress;

	TStringList* FFilter;

public:
	__fastcall TMail();
	__fastcall ~TMail();

	__property String Address = {read = FAddress, write = FAddress};

	__property TStringList* Filter = {read = FFilter};
};

// ---------------------------------------------------------------------------
#endif
