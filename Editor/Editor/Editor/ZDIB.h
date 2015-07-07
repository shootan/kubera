#ifndef _Z_DIB_H_
#define _Z_DIB_H_

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// ZDIB 사용법
//
//	1. 객체 선언 
//		ZDIB			m_dib;
//
//	2. BMP 파일의 경로를 지정.
//		m_dib.LoadDibPath("test.bmp");
//
//	3. BMP 파일 출력 (OnPaint() 함수내)
//		m_dib.DrawDIB(&dc, r);
//		또는 
//		m_dib.ShowDibImage(&dc, 10,10);
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////

#define PALETTE_VERSION     0x300
#define IS_OLD_TYPE_DIB(bitmap_info)  (*(int *)(bitmap_info) == sizeof(BITMAPINFOHEADER))

class ZDIB
{
private :
	char m_is_load, m_transparent_flag;
	int m_dib_width, m_dib_height;
	COLORREF m_transparent_color;

	CSize m_display_size;
	CString m_path_name;
	HANDLE mh_dib_object;
	CPalette *mp_palette_data;

public :
	ZDIB();
	virtual ~ZDIB();

	char LoadDibPath(const char *parm_pathname);
	CSize GetDibImageSize();
	CPalette *GetPalette();
	char IsLoadedDIB();
	void InitDIBData();
	int DIBWidth (const char *parm_bitmap_info);
	int DIBHeight(const char *parm_bitmap_info);

	HANDLE ReadFileForDIB(CFile *parm_bmp_file);
	void CreatePaletteForDIB();
	short GetNumberOfPaletteColor(const char *parm_bitmap_info);
	char *StartDataBitForDIB(char *parm_bitmap_info);
	short GetPaletteSize(char *parm_bitmap_info);

	void SetUserDisplayrFixSize(int parm_cx, int parm_cy);
	void SetUserDisplayrRateSize(double parm_cx, double parm_cy);
	void SetTransparentImage(char parm_flag, COLORREF parm_erase_color);

	void DrawDIB(CDC *parm_dc,RECT *parm_dc_rect);
	void ShowDibImage(CDC *parm_dc, int parm_x, int parm_y);
};

#endif