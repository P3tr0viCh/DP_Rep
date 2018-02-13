//---------------------------------------------------------------------------

#pragma hdrstop

#include "DP_RepSettingsMail.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

// ---------------------------------------------------------------------------
__fastcall TMail::TMail() {
	FAddress = "";

	FFilter = new TStringList();
	FFilter->CaseSensitive = false;
}

// ---------------------------------------------------------------------------
__fastcall TMail::~TMail() {
    FFilter->Free();
}
