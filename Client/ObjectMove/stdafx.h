// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
//#define FRAME_BUFFER_WIDTH      640
//#define FRAME_BUFFER_HEIGHT      480

#define VS_SLOT_VIEWPROJECTION_MATRIX	0x00
#define VS_SLOT_WORLD_MATRIX				0x01

// Windows ��� ����:
#include <windows.h>

// C ��Ÿ�� ��� �����Դϴ�.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <Mmsystem.h>
#include <math.h>

#include <D3Dcompiler.h>
#include <D3DX10math.h>


#include <d3d9types.h>

// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.