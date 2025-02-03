#pragma once

#include "IPC.h"
#include "Vector.h"

#include <d2d1.h>
#include <dwrite.h>

#include <string>
#include <format>
#include <unordered_map>

#include <wincodec.h>

// Forward declarations
class COverlay;
class CText;

#define COLORF_FROMRGBA(r, g, b, a) D2D1_COLOR_F { r / 255.0F, g / 255.0F, b / 255.0F, a }

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

class COverlay
{
private:
	static COverlay *m_pOverlay;

	COverlay();
	~COverlay();

	IPC m_iProcComm;

	IDWriteTextLayout* CreateTextLayout(std::wstring &wContent);
	IDWriteTextLayout* GetCachedTextLayout(std::wstring &wContent);

	friend class CText;

public:
	static COverlay* Get();
	static void Release();

	IPC& GetIPC();

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

	void RenderText(CVector vPoint, CText &tText, D2D1_COLOR_F cText);
	void RenderText(CVector vPoint, CText &tText);
};

class CText
{
private:
	COverlay *m_pOverlay;
	IPC &m_iProcComm;

	IDWriteTextLayout *m_pTextLayout;
	std::wstring m_wContent;

	DWRITE_TEXT_ALIGNMENT m_dAlignment;
	__int64 m_iOffset;

	CVector m_vBounds, m_vOffset;

	bool m_bShadow, m_bUseCached;
	float m_fFontSize;

	friend class COverlay;

	void UpdateBounds();

	void ReleaseTextLayout();

public:
	CText();
	CText(bool bUseCached);
	template <class ...Args>
	CText(std::wstring_view wFormat, Args &&...aArguments) :
		m_pOverlay(COverlay::Get()),
		m_iProcComm(m_pOverlay->GetIPC()),
		m_pTextLayout(nullptr),
		m_bShadow(false),
		m_bUseCached(true),
		m_fFontSize(10.0F)
	{
		SetContent(wFormat, aArguments...);
	}
	~CText();

	std::wstring& GetContent();
	template <class ...Args>
	void SetContent(std::wstring_view wFormat, Args &&...aArguments)
	{
		m_wContent = std::vformat(wFormat, std::make_wformat_args(aArguments...));

		if (m_bUseCached)
			m_pTextLayout = m_pOverlay->GetCachedTextLayout(m_wContent);
		else
		{
			ReleaseTextLayout();
			m_pTextLayout = m_pOverlay->CreateTextLayout(m_wContent);
		}

		if (m_pTextLayout)
		{
			SetFontSize(m_fFontSize);
			UpdateBounds();
			SetAlignment(m_dAlignment, m_iOffset);
		}
	}

	CVector& GetBounds();

	bool GetShadow();
	void SetShadow(bool bShadow);

	float GetFontSize();
	void SetFontSize(float fSize);

	DWRITE_TEXT_ALIGNMENT GetAlignment();
	void SetAlignment(DWRITE_TEXT_ALIGNMENT dAlignment, __int64 iOffset = 0x00000000);
};