#pragma once

#include "Vector.h"

#pragma comment(lib, "d2d1.lib")
#include <d2d1.h>

#pragma comment(lib, "dwrite.lib")
#include <dwrite.h>

#include <string>
#include <format>
#include <unordered_map>

//#define OVERLAY_SRGB

// Forward declarations
class COverlay;
struct SText;

#ifdef OVERLAY_SRGB
#define COLORF_FROMRGBA(r, g, b, a) D2D1_COLOR_F { std::powf(r / 255.0F, 2.2F), std::powf(g / 255.0F, 2.2F), std::powf(b / 255.0F, 2.2F), a }
#else
#define COLORF_FROMRGBA(r, g, b, a) D2D1_COLOR_F { r / 255.0F, g / 255.0F, b / 255.0F, a }
#endif

enum ETEXT_ALIGNMENT
{
	ETEXT_ALIGNMENT_HL = 0x00000001, ETEXT_ALIGNMENT_HR = 0x00000010, ETEXT_ALIGNMENT_HC = 0x00000100,
	ETEXT_ALIGNMENT_VT = 0x00001000, ETEXT_ALIGNMENT_VB = 0x00010000, ETEXT_ALIGNMENT_VC = 0x00100000
};

struct SGradientSettings
{
	D2D1_COLOR_F m_cColor1, m_cColor2;
	CVector m_vStart, m_vEnd;
};

class CRenderingCanvas
{
private:
	HWND m_hWindow;
	CVector m_vSize, m_vCenter;

	bool SetupOverlay();

public:
	CRenderingCanvas(const char *pClass, const char *pTitle);
	~CRenderingCanvas();

	HWND GetHandle();

	CVector GetCenter();
	CVector GetSize();
};

class CDirectRenderer2D
{
private:
	CRenderingCanvas &m_rCanvas;

	ID2D1SolidColorBrush *m_pSolidColorBrush;

	ID2D1GradientStopCollection *m_pGradientStopCollection;
	ID2D1LinearGradientBrush *m_pLinearGradientBrush;

	ID2D1Factory *m_pFactory;
	ID2D1HwndRenderTarget *m_pRenderTarget;

	IDWriteFactory *m_pWriteFactory;
	IDWriteTextFormat *m_pTextFormat;

	bool SetupDirect2D();
	bool SetupDirectWrite();

	friend class COverlay;
	friend class CText;

public:
	CDirectRenderer2D(CRenderingCanvas &rCanvas);
	~CDirectRenderer2D();
};

class COverlay
{
private:
	static COverlay *m_pOverlay;

	COverlay();
	~COverlay();

	CRenderingCanvas m_rCanvas;
	bool m_bInteractive;

	CDirectRenderer2D m_dRenderer;
	float m_fThickness, m_fRoundedRadius;

	std::unordered_map<std::wstring, IDWriteTextLayout*> m_uLayouts;

public:
	static COverlay* Get();
	static void Release();

	void BeginDraw();
	void EndDraw();
	void Clear();

	CVector GetScreenSize();
	CVector GetScreenCenter();
	float GetAspectRatio();

	void SetAntialias(bool bEnabled);
	bool GetAntialias();

	void SetThickness(float fThickness);
	void SetRoundedRadius(float fRadius);

	void UpdateGradientBrush(SGradientSettings gSettings);
	void UpdateGradientBrush(CVector vStart, CVector vEnd);
	void UpdateColorBrush(D2D1_COLOR_F cColor);

	void PushClipRender(CVector vPosition, CVector vSize);
	void PopClipRender();

	void DrawLine(CVector vPoint1, CVector vPoint2, D2D1_COLOR_F cColor);
	void DrawLine(CVector vPoint1, CVector vPoint2);

	void DrawCrosshair(CVector vPoint, float fRadius, D2D1_COLOR_F cColor);

	void DrawCircle(CVector vPoint, float fRadius, D2D1_COLOR_F cColor);
	void DrawCircle(CVector vPoint, float fRadius);

	void DrawFilledCircle(CVector vPoint, float fRadius, D2D1_COLOR_F cColor);
	void DrawFilledCircle(CVector vPoint, float fRadius);

	void DrawEllipse(CVector vPoint, float fRadius1, float fRadius2, D2D1_COLOR_F cColor);
	void DrawEllipse(CVector vPoint, float fRadius1, float fRadius2);

	void DrawFilledEllipse(CVector vPoint, float fRadius1, float fRadius2, D2D1_COLOR_F cColor);
	void DrawFilledEllipse(CVector vPoint, float fRadius1, float fRadius2);

	void DrawRect(CVector vPoint, CVector vSize, D2D1_COLOR_F cColor, bool bCentered = false);
	void DrawRect(CVector vPoint, CVector vSize, bool bCentered = false);

	void DrawSizeRect(CVector vPoint1, CVector vPoint2, float fFactor, D2D1_COLOR_F cColor);
	void DrawSizeRect(CVector vPoint1, CVector vPoint2, float fFactor);

	void DrawFilledSizeRect(CVector vPoint1, CVector vPoint2, float fFactor, D2D1_COLOR_F cColor);
	void DrawFilledSizeRect(CVector vPoint1, CVector vPoint2, float fFactor);

	void DrawFilledRect(CVector vPoint, CVector vSize, D2D1_COLOR_F cColor, bool bCentered = false);
	void DrawFilledRect(CVector vPoint, CVector vSize, bool bCentered = false);

	void DrawRoundedRect(CVector vPoint, CVector vSize, D2D1_COLOR_F cColor, bool bCentered = false);
	void DrawRoundedRect(CVector vPoint, CVector vSize, bool bCentered = false);

	void DrawFilledRoundedRect(CVector vPoint, CVector vSize, D2D1_COLOR_F cColor, bool bCentered = false);
	void DrawFilledRoundedRect(CVector vPoint, CVector vSize, bool bCentered = false);

	void DrawRhombus(CVector vPoint1, CVector vPoint2, D2D1_COLOR_F cColor);
	void DrawRhombus(CVector vPoint1, CVector vPoint2);

	void RenderText(CVector vPoint, SText tText, D2D1_COLOR_F cText);
	void RenderText(CVector vPoint, SText tText);

	IDWriteTextLayout* CreateTextLayout(std::wstring_view wContent);
	IDWriteTextLayout* GetCachedTextLayout(std::wstring wContent);
};

struct SText
{
	IDWriteTextLayout *m_pTextLayout;
	CVector m_vBounds, m_vOffset;
	bool m_bShadow;

	CVector UpdateBounds();
};