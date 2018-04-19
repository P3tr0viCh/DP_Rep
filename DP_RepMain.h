// ---------------------------------------------------------------------------

#ifndef DP_RepMainH
#define DP_RepMainH
// ---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Data.DB.hpp>
#include <Data.Win.ADODB.hpp>
#include <System.ImageList.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.ToolWin.hpp>

#include "DP_RepSettings.h"

// ---------------------------------------------------------------------------
class TMain : public TForm {
__published: // IDE-managed Components

	TStatusBar *StatusBar;
	TToolBar *ToolBar;
	TToolButton *tbtnUpdate;
	TToolButton *tbtnMail;
	TToolButton *tbtnSave;
	TToolButton *tbtnSeparator01;
	TToolButton *tbtnAbout;
	TToolButton *tbtnSeparator02;
	TToolButton *tbtnClose;
	TPanel *PanelDate;
	TSpeedButton *btnDateNext;
	TSpeedButton *btnDatePrev;
	TDateTimePicker *DateTimePicker;
	TImageList *ImageList;
	TADOConnection *Connection;
	TADOQuery *Query;
	TPopupMenu *pmMemo;
	TMenuItem *miCopy;
	TMemo *Memo;
	TLabel *lblDateTime;

	void __fastcall tbtnCloseClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall btnDatePrevClick(TObject *Sender);
	void __fastcall DateTimePickerChange(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall tbtnAboutClick(TObject *Sender);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall tbtnSaveClick(TObject *Sender);
	void __fastcall miCopyClick(TObject *Sender);
	void __fastcall tbtnUpdateClick(TObject *Sender);
	void __fastcall tbtnMailClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);

private: // User declarations

	TSettings* Settings;

	TDBResultList* DBResults;

	TDateTime DateTimeFrom;
	TDateTime DateTimeTo;

	void LoadSettings();

	bool CheckForSave();
	bool Ping(String HostAddr);
	int CheckPing();
	void ReportSave();
	bool ReportUpdate();
	void ReportMail();
	void UpdateDateTime();

public: // User declarations
	__fastcall TMain(TComponent* Owner);
};

// ---------------------------------------------------------------------------
extern PACKAGE TMain *Main;
// ---------------------------------------------------------------------------
#endif
