// ---------------------------------------------------------------------------

#ifndef DP_RepDBResultH
#define DP_RepDBResultH
// ---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <System.Contnrs.hpp>

#include <Data.DB.hpp>
#include <Data.Win.ADODB.hpp>

#include "DP_RepSettingsDB.h"

// ---------------------------------------------------------------------------
class TDBRecord : public TObject {
private:
	String FProduct;
	float FWeigh;

public:
	__fastcall TDBRecord(String AProduct, float AWeigh) {
		FProduct = AProduct;
		FWeigh = AWeigh;
	};

	__property String Product = {read = FProduct};
	__property float Weigh = {read = FWeigh};
};

// ---------------------------------------------------------------------------
class TDBResult : public TObject {
private:
	TADOConnection* FConnection;
	TADOQuery* FQuery;

	TDB* FDB;

	String FErrorMessage;

	TObjectList* FRecords;

	void Clear();

public:
	__fastcall TDBResult(TADOConnection* AConnection, TADOQuery* AQuery,
		TDB* ADB);
	__fastcall ~TDBResult();

	bool LoadData(TDateTime ADateTimeFrom, TDateTime ADateTimeTo);

	__property String ErrorMessage = {read = FErrorMessage};

	__property TDB* DB = {read = FDB};

	__property TObjectList* Records = {read = FRecords};
};

// ---------------------------------------------------------------------------
#endif
