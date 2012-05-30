/*  Project nGenEx
	Destroyer Studios LLC
	Copyright © 1997-2004. All Rights Reserved.

	SUBSYSTEM:    nGenEx.lib
	FILE:         ListBox.h
	AUTHOR:       John DiCamillo


	OVERVIEW
	========
	ListBox ActiveWindow class
*/

#ifndef ListBox_h
#define ListBox_h

#include <vector>
#include "Types.h"
#include "ScrollWindow.h"
#include "List.h"

// +--------------------------------------------------------------------+

class Bitmap;
class ListBox;
class ListBoxCell;
class ListBoxItem;
class ListBoxColumn;

// +--------------------------------------------------------------------+

class ListBox : public ScrollWindow
{
public:
	enum SORT   {  LIST_SORT_NUMERIC_DESCENDING = -2,
		LIST_SORT_ALPHA_DESCENDING,
		LIST_SORT_NONE,
		LIST_SORT_ALPHA_ASCENDING,
		LIST_SORT_NUMERIC_ASCENDING,
		LIST_SORT_NEVER
	};

	enum ALIGN  {  LIST_ALIGN_LEFT   = DT_LEFT,
		LIST_ALIGN_CENTER = DT_CENTER,
		LIST_ALIGN_RIGHT  = DT_RIGHT
	};

	enum STYLE  {  LIST_ITEM_STYLE_PLAIN,
		LIST_ITEM_STYLE_BOX,
		LIST_ITEM_STYLE_FILLED_BOX
	};

	ListBox(ActiveWindow* p, int ax, int ay, int aw, int ah, DWORD aid);
	ListBox(Screen* s,       int ax, int ay, int aw, int ah, DWORD aid);
	virtual ~ListBox();

	// Operations:
	virtual void      DrawContent(const Rect& ctrl_rect);

	// Event Target Interface:
	virtual int       OnMouseMove(int x, int y);
	virtual int       OnLButtonDown(int x, int y);
	virtual int       OnLButtonUp(int x, int y);
	virtual int       OnMouseWheel(int wheel);
	virtual int       OnClick();

	virtual int       OnKeyDown(int vk, int flags);

	// pseudo-events:
	virtual int       OnDragStart(int x, int y);
	virtual int       OnDragDrop(int x, int y, ActiveWindow* source);

	// Property accessors:
	size_t            NumItems();
	size_t            NumColumns();

	Text           GetItemText(size_t index);
	void           SetItemText(size_t index, const char* text);
	DWORD          GetItemData(size_t index);
	void           SetItemData(size_t index, DWORD data);
	Bitmap*        GetItemImage(size_t index);
	void           SetItemImage(size_t index, Bitmap* img);
	Color          GetItemColor(size_t index);
	void           SetItemColor(size_t index, Color c);

	Text           GetItemText(size_t index, size_t column);
	void           SetItemText(size_t index, size_t column, const char* text);
	DWORD          GetItemData(size_t index, size_t column);
	void           SetItemData(size_t index, size_t column, DWORD data);
	Bitmap*        GetItemImage(size_t index, size_t column);
	void           SetItemImage(size_t index, size_t column, Bitmap* img);

	int            AddItem(const char* text);
	int            AddImage(Bitmap* img);
	int            AddItemWithData(const char* text, int data);
	void           InsertItem(size_t index, const char* text);
	void           InsertItemWithData(size_t index, const char* text, int data);
	void           ClearItems();
	void           RemoveItem(size_t index);
	void           RemoveSelectedItems();

	void           AddColumn(const char* title,
	int width,
	int align = ListBox::LIST_ALIGN_LEFT,
	int sort  = ListBox::LIST_SORT_NONE);

	Text           GetColumnTitle(size_t index);
	void           SetColumnTitle(size_t index, const char* title);
	int            GetColumnWidth(size_t index);
	void           SetColumnWidth(size_t index, int width);
	int            GetColumnAlign(size_t index);
	void           SetColumnAlign(size_t index, int align);
	int            GetColumnSort(size_t index);
	void           SetColumnSort(size_t index, int sort);
	Color          GetColumnColor(size_t index);
	void           SetColumnColor(size_t index, Color c);

	Color          GetItemColor(size_t index, size_t column);

	int            GetMultiSelect();
	void           SetMultiSelect(int nNewValue);
	bool           GetShowHeadings();
	void           SetShowHeadings(bool nNewValue);
	Color          GetSelectedColor();
	void           SetSelectedColor(Color c);

	int            GetItemStyle() const;
	void           SetItemStyle(int style);
	int            GetSelectedStyle() const;
	void           SetSelectedStyle(int style);

	bool           IsSelected(size_t index);
	void           SetSelected(size_t index, bool bNewValue=true);
	void           ClearSelection();

	int            GetSortColumn();
	void           SetSortColumn(size_t col_index);
	int            GetSortCriteria();
	void           SetSortCriteria(SORT sort);
	void           SortItems();
	void           SizeColumns();

	// read-only:
	virtual int    GetListIndex();
	virtual size_t GetLineCount();
	virtual int    GetSelCount();
	virtual int    GetSelection();
	virtual Text   GetSelectedItem();

protected:
	int            IndexFromPoint(int x, int y) const;

	// properties:
	std::vector<ListBoxItem>    items;
	std::vector<ListBoxColumn>  columns;

	bool           show_headings;
	int            multiselect;
	size_t         list_index;
	size_t         selcount;

	Color          selected_color;

	size_t            sort_column;
	int            item_style;
	int            seln_style;
};

#endif ListBox_h

