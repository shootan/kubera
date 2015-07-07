#include "stdafx.h"

#include "ZDIB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

ZDIB::ZDIB()
{
	m_dib_width = 10;
	m_dib_height = 10;
	m_is_load = 0;
	mh_dib_object = NULL;
	mp_palette_data = NULL;
}

ZDIB::~ZDIB()
{
	if(mh_dib_object != NULL) ::GlobalFree((HGLOBAL)mh_dib_object);
	if(mp_palette_data != NULL) delete mp_palette_data;
}

char ZDIB::LoadDibPath(const char *parm_pathname)
{
	m_is_load = 0;

	if(mh_dib_object != NULL) ::GlobalFree((HGLOBAL) mh_dib_object);
	if(mp_palette_data != NULL) delete mp_palette_data;

	mh_dib_object = NULL;
	mp_palette_data = NULL;
	m_path_name = parm_pathname;

	CFile bmp_file;
	if(bmp_file.Open(m_path_name, CFile::modeRead | CFile::shareDenyWrite, NULL)){
		mh_dib_object = ReadFileForDIB(&bmp_file);
		bmp_file.Close();
		if(mh_dib_object!=NULL){
			m_is_load = 1;
			InitDIBData();
			return 1;
		}
	}

	return -1;
}

void ZDIB::InitDIBData()
{
	if(mp_palette_data != NULL) delete mp_palette_data;
	mp_palette_data = NULL;

	m_transparent_flag = 0;
	m_transparent_color = RGB(255, 255, 255);
	
	const char *p_bitmap_info = (const char *)::GlobalLock((HGLOBAL)mh_dib_object);

	if(IS_OLD_TYPE_DIB(p_bitmap_info)){
		m_dib_width=((BITMAPINFOHEADER *)p_bitmap_info)->biWidth;
		m_dib_height=((BITMAPINFOHEADER *)p_bitmap_info)->biHeight;
	} else {
		m_dib_width=((BITMAPCOREHEADER *)p_bitmap_info)->bcWidth;
		m_dib_height=((BITMAPCOREHEADER *)p_bitmap_info)->bcHeight;
	}

	m_display_size.cx = m_dib_width;
	m_display_size.cy = m_dib_height;

	::GlobalUnlock((HGLOBAL) mh_dib_object);
	CreatePaletteForDIB();
}

HANDLE ZDIB::ReadFileForDIB(CFile *parm_bmp_file)
{
	BITMAPFILEHEADER bmp_header;
	int bmp_size = (int)parm_bmp_file->GetLength();

	if(parm_bmp_file->Read((char *)&bmp_header, sizeof(bmp_header)) != sizeof(bmp_header)) return NULL;
	if(bmp_header.bfType != ((short) ('M' << 8) | 'B')) return NULL;

	HANDLE h_temp_dib=(HANDLE)::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, bmp_size);
	char *p_temp_dib=(char *)::GlobalLock((HGLOBAL)h_temp_dib);
	if(parm_bmp_file->Read(p_temp_dib, bmp_size-sizeof(BITMAPFILEHEADER)) != bmp_size-sizeof(BITMAPFILEHEADER)){
		::GlobalUnlock((HGLOBAL)h_temp_dib);
		::GlobalFree((HGLOBAL)h_temp_dib);
		return NULL;
	} 
	::GlobalUnlock((HGLOBAL)h_temp_dib);
	return h_temp_dib;
}

void ZDIB::SetUserDisplayrFixSize(int parm_cx, int parm_cy)
{
	m_display_size.cx = parm_cx;
	m_display_size.cy = parm_cy;
}

void ZDIB::SetUserDisplayrRateSize(double parm_cx, double parm_cy)
{
	m_display_size.cx = int(m_dib_width*parm_cx);
	m_display_size.cx = int(m_dib_height*parm_cy);
}

void ZDIB::SetTransparentImage(char parm_flag, COLORREF parm_erase_color)
{
	m_transparent_flag = parm_flag;
	if(m_transparent_flag) m_transparent_color = parm_erase_color;
}

void ZDIB::ShowDibImage(CDC *parm_dc, int parm_x, int parm_y)
{
	if(!m_transparent_flag) DrawDIB(parm_dc, CRect(parm_x, parm_y, parm_x+m_display_size.cx, parm_y+m_display_size.cy));
	else {
		CDC dc_image;
		dc_image.CreateCompatibleDC(parm_dc);
		CBitmap org_image;
		org_image.CreateCompatibleBitmap(parm_dc, m_display_size.cx, m_display_size.cy);
		CBitmap *old_org_image = dc_image.SelectObject(&org_image);
		DrawDIB(&dc_image,CRect(0,0,m_display_size.cx,m_display_size.cy));

		CDC dc_and;
		dc_and.CreateCompatibleDC(parm_dc);
		CBitmap bitmap_and;
		bitmap_and.CreateBitmap(m_display_size.cx,m_display_size.cy,1,1,NULL);
		CBitmap *old_and_image = dc_and.SelectObject(&bitmap_and);
		dc_image.SetBkColor(m_transparent_color);
		dc_and.BitBlt(0,0,m_display_size.cx,m_display_size.cy,&dc_image,0,0,SRCCOPY);

		CDC dc_xor;
		dc_xor.CreateCompatibleDC(parm_dc);
		CBitmap bitmap_xor;
		bitmap_xor.CreateCompatibleBitmap(&dc_image,m_display_size.cx,m_display_size.cy);
		CBitmap *old_xor_image = dc_xor.SelectObject(&bitmap_xor);
		dc_xor.BitBlt(0,0,m_display_size.cx,m_display_size.cy,&dc_image,0,0,SRCCOPY);
		dc_xor.BitBlt(0,0,m_display_size.cx,m_display_size.cy,&dc_and,0,0,0x220326);

		CDC dc_temp;
		dc_temp.CreateCompatibleDC(parm_dc);
		CBitmap bitmap_temp;
		bitmap_temp.CreateCompatibleBitmap(&dc_image,m_display_size.cx,m_display_size.cy);
		CBitmap *old_temp_image = dc_temp.SelectObject(&bitmap_temp);

		dc_temp.BitBlt(0,0,m_display_size.cx,m_display_size.cy,parm_dc, parm_x, parm_y,SRCCOPY);

		dc_temp.BitBlt(0,0,m_display_size.cx,m_display_size.cy,&dc_and,0,0,SRCAND);
		dc_temp.BitBlt(0,0,m_display_size.cx,m_display_size.cy,&dc_xor,0,0,SRCINVERT);

		parm_dc->BitBlt(parm_x, parm_y, m_display_size.cx,m_display_size.cy,&dc_temp,0,0,SRCCOPY);

		dc_temp.SelectObject(old_temp_image);
		bitmap_temp.DeleteObject();
		dc_temp.DeleteDC();

		dc_xor.SelectObject(old_xor_image);
		bitmap_xor.DeleteObject();
		dc_xor.DeleteDC();

		dc_and.SelectObject(old_and_image);
		bitmap_and.DeleteObject();
		dc_and.DeleteDC();

		dc_image.SelectObject(old_org_image);
		org_image.DeleteObject();
		dc_image.DeleteDC();
	}
}

void ZDIB::DrawDIB(CDC *parm_dc,RECT *parm_dc_rect)
{
	if(mh_dib_object == NULL) return;

	HPALETTE h_palette=NULL;
	HPALETTE h_old_palette=NULL;

	char *p_dib_header = (char *)::GlobalLock((HGLOBAL) mh_dib_object);
	char *p_dib_data = StartDataBitForDIB(p_dib_header);

	if(mp_palette_data != NULL){
		h_palette = (HPALETTE) mp_palette_data->m_hObject;
		h_old_palette = ::SelectPalette(parm_dc->GetSafeHdc(), h_palette, TRUE);
	}

	parm_dc->SetStretchBltMode(COLORONCOLOR);
	int temp_dc_width = parm_dc_rect->right - parm_dc_rect->left, temp_dc_height = parm_dc_rect->bottom - parm_dc_rect->top;

	if(temp_dc_width == m_dib_width && temp_dc_height == m_dib_height){
		::SetDIBitsToDevice(parm_dc->GetSafeHdc(), parm_dc_rect->left, parm_dc_rect->top,
									temp_dc_width, temp_dc_height, 0, 0, 0, m_dib_height,
								    p_dib_data, (LPBITMAPINFO)p_dib_header, DIB_RGB_COLORS);
	} else {
		::StretchDIBits(parm_dc->GetSafeHdc(), parm_dc_rect->left, parm_dc_rect->top,
							   temp_dc_width, temp_dc_height, 0, 0, m_dib_width, m_dib_height, p_dib_data,
							   (LPBITMAPINFO)p_dib_header, DIB_RGB_COLORS, SRCCOPY);
	}

   ::GlobalUnlock((HGLOBAL) mh_dib_object);
   if(h_old_palette != NULL) ::SelectPalette(parm_dc->GetSafeHdc(),h_old_palette, TRUE);
}

void ZDIB::CreatePaletteForDIB()
{
	mp_palette_data = new CPalette;
	const char *p_bitmap_info = (const char *)::GlobalLock((HGLOBAL)mh_dib_object);
	BITMAPINFO *p_win30type=(BITMAPINFO *)p_bitmap_info;
	BITMAPCOREINFO *lpbmc=(BITMAPCOREINFO *)p_bitmap_info;

	short number_of_color = GetNumberOfPaletteColor(p_bitmap_info);

	if(number_of_color){
		HANDLE h_temp_palette = ::GlobalAlloc(GHND,sizeof(LOGPALETTE)+sizeof(PALETTEENTRY)*number_of_color);
		LOGPALETTE *p_temp_palette = (LOGPALETTE *)::GlobalLock((HGLOBAL)h_temp_palette);
		p_temp_palette->palVersion = PALETTE_VERSION;
		p_temp_palette->palNumEntries = number_of_color;

		if(IS_OLD_TYPE_DIB(p_bitmap_info)){
			BITMAPINFO *p_win30_type = (BITMAPINFO *)p_bitmap_info;
			for(int i = 0; i < number_of_color; i++) {
				p_temp_palette->palPalEntry[i].peRed = p_win30_type->bmiColors[i].rgbRed;
				p_temp_palette->palPalEntry[i].peGreen = p_win30_type->bmiColors[i].rgbGreen;
				p_temp_palette->palPalEntry[i].peBlue = p_win30_type->bmiColors[i].rgbBlue;
				p_temp_palette->palPalEntry[i].peFlags = 0;
			}
		} else {
			BITMAPCOREINFO *p_other_type=(BITMAPCOREINFO *)p_bitmap_info;
			for(int i = 0; i < number_of_color; i++) {
				p_temp_palette->palPalEntry[i].peRed = p_other_type->bmciColors[i].rgbtRed;
				p_temp_palette->palPalEntry[i].peGreen = p_other_type->bmciColors[i].rgbtGreen;
				p_temp_palette->palPalEntry[i].peBlue = p_other_type->bmciColors[i].rgbtBlue;
				p_temp_palette->palPalEntry[i].peFlags = 0;
			}
		}

		mp_palette_data->CreatePalette(p_temp_palette);
		::GlobalUnlock((HGLOBAL)h_temp_palette);
		::GlobalFree((HGLOBAL)h_temp_palette);
	}

	::GlobalUnlock((HGLOBAL)mh_dib_object);
}

char *ZDIB::StartDataBitForDIB(char *parm_bitmap_info)
{
	return parm_bitmap_info + *(int *)parm_bitmap_info + GetPaletteSize(parm_bitmap_info);
}

int ZDIB::DIBWidth(const char *parm_bitmap_info)
{
	if(IS_OLD_TYPE_DIB(parm_bitmap_info)) return ((BITMAPINFOHEADER *)parm_bitmap_info)->biWidth;
	return ((BITMAPCOREHEADER *)parm_bitmap_info)->bcWidth;
}

int ZDIB::DIBHeight(const char *parm_bitmap_info)
{
	if(IS_OLD_TYPE_DIB(parm_bitmap_info)) return ((BITMAPINFOHEADER *)parm_bitmap_info)->biHeight;
	return ((BITMAPCOREHEADER *)parm_bitmap_info)->bcHeight;
}

short ZDIB::GetPaletteSize(char *parm_bitmap_info)
{
	if(IS_OLD_TYPE_DIB(parm_bitmap_info)) return GetNumberOfPaletteColor(parm_bitmap_info) * sizeof(RGBQUAD);
	return GetNumberOfPaletteColor(parm_bitmap_info) * sizeof(RGBTRIPLE);
}

short ZDIB::GetNumberOfPaletteColor(const char *parm_bitmap_info)
{
	short bit_count = ((BITMAPCOREHEADER *)parm_bitmap_info)->bcBitCount;

	if(IS_OLD_TYPE_DIB(parm_bitmap_info)) {
		short actually_used_number = (short)((BITMAPINFOHEADER *)parm_bitmap_info)->biClrUsed;
		if(actually_used_number) return actually_used_number;
		else bit_count = ((BITMAPINFOHEADER *)parm_bitmap_info)->biBitCount;
	}

	switch (bit_count) {
		case 1: return 2;
		case 4:	return 16;
		case 8:	return 256;
	}
	return 0;
}

CPalette *ZDIB::GetPalette()
{ 
	return mp_palette_data;
}

char ZDIB::IsLoadedDIB() 
{
	return m_is_load; 
}

CSize ZDIB::GetDibImageSize()
{
	return CSize(m_dib_width,m_dib_height);
}	
