#pragma once
#include <UI/Dialogs/Dialog.h>
#include <UI/ViewPane/ViewPane.h>
// These panes aren't all used in Editor.h, but included so that including Editor.h elsewhere enables all pane types
#include <UI/ViewPane/TextPane.h>
#include <UI/ViewPane/CheckPane.h>
#include <UI/ViewPane/DropDownPane.h>
#include <UI/ViewPane/ListPane.h>

class CParentWnd;

namespace dialog
{
	namespace editor
	{
	// Buttons for CEditor
	//#define CEDITOR_BUTTON_OK 0x00000001 // Duplicated from MFCMAPI.h - do not modify
#define CEDITOR_BUTTON_ACTION1 0x00000002
#define CEDITOR_BUTTON_ACTION2 0x00000004
//#define CEDITOR_BUTTON_CANCEL 0x00000008 // Duplicated from MFCMAPI.h - do not modify
#define CEDITOR_BUTTON_ACTION3 0x00000010

		template <typename T> viewpane::DoListEditCallback ListEditCallBack(T* editor)
		{
			return [editor](auto a, auto b, auto c) { return editor->DoListEdit(a, b, c); };
		}

		class CEditor : public CMyDialog
		{
		public:
			// Main Edit Constructor
			CEditor(_In_opt_ CWnd* pParentWnd, UINT uidTitle, UINT uidPrompt, ULONG ulButtonFlags);
			CEditor(
				_In_opt_ CWnd* pParentWnd,
				UINT uidTitle,
				UINT uidPrompt,
				ULONG ulButtonFlags,
				UINT uidActionButtonText1,
				UINT uidActionButtonText2,
				UINT uidActionButtonText3);
			virtual ~CEditor();

			_Check_return_ HRESULT DisplayDialog();

			// These functions can be used to set up a data editing dialog
			void SetPromptPostFix(_In_ const std::wstring& szMsg);
			void InitPane(ULONG iNum, viewpane::ViewPane* lpPane);
			void SetStringA(ULONG i, const std::string& szMsg) const;
			void SetStringW(ULONG i, const std::wstring& szMsg) const;
			void SetStringf(ULONG i, LPCWSTR szMsg, ...) const;
#ifdef CHECKFORMATPARAMS
#undef SetStringf
#define SetStringf(i, fmt, ...) SetStringf(i, fmt, __VA_ARGS__), wprintf(fmt, __VA_ARGS__)
#endif

			void LoadString(ULONG i, UINT uidMsg) const;
			void SetBinary(ULONG i, _In_opt_count_(cb) LPBYTE lpb, size_t cb) const;
			void SetHex(ULONG i, ULONG ulVal) const;
			void SetDecimal(ULONG i, ULONG ulVal) const;
			void SetSize(ULONG i, size_t cb) const;

			// Get values after we've done the DisplayDialog
			viewpane::ViewPane* GetPane(ULONG iPane) const;
			std::wstring GetStringW(ULONG i) const;
			_Check_return_ ULONG GetHex(ULONG i) const;
			_Check_return_ ULONG GetDecimal(ULONG i) const;
			_Check_return_ ULONG GetPropTag(ULONG i) const;
			_Check_return_ bool GetCheck(ULONG i) const;
			_Check_return_ int GetDropDown(ULONG i) const;
			_Check_return_ DWORD_PTR GetDropDownValue(ULONG i) const;
			_Check_return_ HRESULT
			GetEntryID(ULONG i, bool bIsBase64, _Out_ size_t* lpcbBin, _Out_ LPENTRYID* lppEID) const;
			GUID GetSelectedGUID(ULONG iControl, bool bByteSwapped) const;

			// AddIn functions
			void SetAddInTitle(const std::wstring& szTitle);
			void SetAddInLabel(ULONG i, const std::wstring& szLabel) const;

			// Use this function to implement list editing
			// return true to indicate the entry was changed, false to indicate it was not
			_Check_return_ virtual bool
			DoListEdit(ULONG ulListNum, int iItem, _In_ controls::sortlistdata::SortListData* lpData);

		protected:
			// Functions used by derived classes during init
			void InsertColumn(ULONG ulListNum, int nCol, UINT uidText) const;
			void InsertColumn(ULONG ulListNum, int nCol, UINT uidText, ULONG ulPropType) const;
			void SetListString(ULONG iControl, ULONG iListRow, ULONG iListCol, const std::wstring& szListString) const;
			_Check_return_ controls::sortlistdata::SortListData*
			InsertListRow(ULONG iControl, int iRow, const std::wstring& szText) const;
			void ClearList(ULONG iControl) const;
			void ResizeList(ULONG iControl, bool bSort) const;

			// Functions used by derived classes during handle change events
			std::vector<BYTE> GetBinary(ULONG i) const;
			_Check_return_ std::string GetStringA(ULONG iControl) const;
			_Check_return_ ULONG GetListCount(ULONG iControl) const;
			_Check_return_ controls::sortlistdata::SortListData* GetListRowData(ULONG iControl, int iRow) const;
			_Check_return_ bool IsDirty(ULONG iControl) const;

			// Called to enable/disable buttons based on number of items
			void UpdateButtons() const;
			BOOL OnInitDialog() override;
			void OnOK() override;
			void OnRecalcLayout();

			// protected since derived classes need to call the base implementation
			_Check_return_ virtual ULONG HandleChange(UINT nID);

			void EnableScroll();

		private:
			// Overridable functions
			// use these functions to implement custom edit buttons
			virtual void OnEditAction1();
			virtual void OnEditAction2();
			virtual void OnEditAction3();

			// private constructor invoked from public constructors
			void Constructor(
				_In_opt_ CWnd* pParentWnd,
				UINT uidTitle,
				UINT uidPrompt,
				ULONG ulButtonFlags,
				UINT uidActionButtonText1,
				UINT uidActionButtonText2,
				UINT uidActionButtonText3);

			void DeleteControls();
			_Check_return_ SIZE ComputeWorkArea(SIZE sScreen);
			void OnGetMinMaxInfo(_Inout_ MINMAXINFO* lpMMI);
			void OnSetDefaultSize();
			_Check_return_ LRESULT OnNcHitTest(CPoint point);
			void OnSize(UINT nType, int cx, int cy);
			LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
			void SetMargins() const;

			// List functions and data
			_Check_return_ bool OnEditListEntry(ULONG ulListNum) const;
			ULONG m_ulListNum{}; // Only supporting one list right now - this is the control number for it

			// Our UI controls. Only valid during display.
			bool m_bHasPrompt{};
			CEdit m_Prompt;
			CButton m_OkButton;
			CButton m_ActionButton1;
			CButton m_ActionButton2;
			CButton m_ActionButton3;
			CButton m_CancelButton;
			ULONG m_cButtons{};

			// Variables that get set in the constructor
			ULONG m_bButtonFlags{};

			// Size calculations
			int m_iMargin{};
			int m_iSideMargin{};
			int m_iSmallHeightMargin{};
			int m_iLargeHeightMargin{};
			int m_iButtonWidth{};
			int m_iEditHeight{};
			int m_iTextHeight{};
			int m_iButtonHeight{};
			int m_iMinWidth{};
			int m_iMinHeight{};

			// Title bar, prompt and icon
			UINT m_uidTitle{};
			std::wstring m_szTitle;
			UINT m_uidPrompt{};
			std::wstring m_szPromptPostFix;
			std::wstring m_szAddInTitle;
			HICON m_hIcon{};

			UINT m_uidActionButtonText1{};
			UINT m_uidActionButtonText2{};
			UINT m_uidActionButtonText3{};

			std::vector<viewpane::ViewPane*> m_lpControls{}; // array of controls

			bool m_bEnableScroll{};
			CWnd m_ScrollWindow;
			HWND m_hWndVertScroll{};
			bool m_bScrollVisible{};

			int m_iScrollClient{};

			DECLARE_MESSAGE_MAP()
		};
	}
}