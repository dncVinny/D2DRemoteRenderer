#include "Overlay.h"

#include "Commands.h"
using namespace command;

#include <stdexcept>

#pragma region Helper Functions
inline D2D_POINT_2F GetPoint2F(CVector vPoint)
{
	return D2D_POINT_2F{ vPoint.x, vPoint.y };
}
#pragma endregion

COverlay *COverlay::m_pOverlay = nullptr;

#pragma region COverlay
COverlay::COverlay() :
	m_iProcComm("REMOTE_RENDERER") { }
COverlay::~COverlay()
{
	Sleep(100);

	BeginDraw();
	Clear();
	EndDraw();
}

// [Private]:
IDWriteTextLayout* COverlay::CreateTextLayout(std::wstring &wContent)
{
	SCreateTextLayout cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_CREATETEXTLAYOUT;
	wcsncpy(cRequest.m_pContent, wContent.data(), wContent.length() + 1);

	m_iProcComm.Send(cRequest, cRequest);
	return cRequest.m_pTextLayout;
}
IDWriteTextLayout* COverlay::GetCachedTextLayout(std::wstring &wContent)
{
	SGetCachedTextLayout cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_GETCACHEDTEXTLAYOUT;
	memcpy(cRequest.m_pContent, wContent.data(), sizeof(wchar_t) * (wContent.length() + 1));

	m_iProcComm.Send(cRequest, cRequest);

	return cRequest.m_pTextLayout;
}

// [Public]:
COverlay* COverlay::Get()
{
	if (!m_pOverlay)
		m_pOverlay = new COverlay();

	return m_pOverlay;
}
void COverlay::Release()
{
	if (m_pOverlay)
		delete m_pOverlay;
}

IPC& COverlay::GetIPC()
{
	return m_iProcComm;
}

void COverlay::BeginDraw()
{
	m_iProcComm.Send(SCommand{ ECOMMAND_CODE_BEGINDRAW });
}
void COverlay::EndDraw()
{
	m_iProcComm.Send(SCommand{ ECOMMAND_CODE_ENDDRAW });
}
void COverlay::Clear()
{
	m_iProcComm.Send(SCommand{ ECOMMAND_CODE_CLEAR });
}

CVector COverlay::GetScreenSize()
{
	SGetScreenSize cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_GETSCREENSIZE;

	m_iProcComm.Send(cRequest, cRequest);

	return cRequest.m_vSize;
}
CVector COverlay::GetScreenCenter()
{
	SGetScreenCenter cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_GETSCREENCENTER;

	m_iProcComm.Send(cRequest, cRequest);

	return cRequest.m_vPosition;
}
float COverlay::GetAspectRatio()
{
	SGetAspectRatio cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_GETASPECTRATIO;

	m_iProcComm.Send(cRequest, cRequest);

	return cRequest.m_fRatio;
}

void COverlay::SetAntialias(bool bEnabled)
{
	SSetAntiAlias cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_SETANTIALIAS;
	cRequest.m_bEnabled = bEnabled;

	m_iProcComm.Send(cRequest);
}
bool COverlay::GetAntialias()
{
	SGetAntiAlias cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_GETANTIALIAS;

	m_iProcComm.Send(cRequest, cRequest);

	return cRequest.m_bAntiAlias;
}

void COverlay::SetThickness(float fThickness)
{
	SSetThickness cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_SETTHICKNESS;
	cRequest.m_fThickness = fThickness;

	m_iProcComm.Send(cRequest);
}
void COverlay::SetRoundedRadius(float fRadius)
{
	SSetRoundedRadius cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_SETROUNDEDRADIUS;
	cRequest.m_fRadius = fRadius;

	m_iProcComm.Send(cRequest);
}

void COverlay::PushClipRender(CVector vPosition, CVector vSize)
{
	SPushClipRender cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_PUSHCLIPRENDER;
	cRequest.m_vPosition = vPosition;
	cRequest.m_vSize = vSize;

	m_iProcComm.Send(cRequest);
}
void COverlay::PopClipRender()
{
	m_iProcComm.Send(SCommand{ ECOMMAND_CODE_POPCLIPRENDER });
}

void COverlay::UpdateGradientBrush(SGradientSettings gSettings)
{
	SUpdateGradientBrush cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_UPDATEGRADIENTBRUSH;
	cRequest.m_cColor1 = gSettings.m_cColor1;
	cRequest.m_cColor2 = gSettings.m_cColor2;
	cRequest.m_vStart = gSettings.m_vStart;
	cRequest.m_vEnd = gSettings.m_vEnd;

	m_iProcComm.Send(cRequest);
}
void COverlay::UpdateGradientBrush(CVector vStart, CVector vEnd)
{
	SUpdateGradientBrushVector cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_UPDATEGRADIENTBRUSHVECTOR;
	cRequest.m_vStart = vStart;
	cRequest.m_vEnd = vEnd;

	m_iProcComm.Send(cRequest);
}
void COverlay::UpdateColorBrush(D2D1_COLOR_F cColor)
{
	SUpdateColorBrush cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_UPDATECOLORBRUSH;
	cRequest.m_cColor = cColor;

	m_iProcComm.Send(cRequest);
}

void COverlay::DrawLine(CVector vPoint1, CVector vPoint2, D2D1_COLOR_F cColor)
{
	SDrawLine cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_DRAWLINE;
	cRequest.m_vPoint1 = vPoint1;
	cRequest.m_vPoint2 = vPoint2;
	cRequest.m_cColor = cColor;

	m_iProcComm.Send(cRequest);
}
void COverlay::DrawLine(CVector vPoint1, CVector vPoint2)
{
	SDrawLine cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_DRAWLINE;
	cRequest.m_vPoint1 = vPoint1;
	cRequest.m_vPoint2 = vPoint2;
	cRequest.m_bUsingGradient = true;

	m_iProcComm.Send(cRequest);
}

void COverlay::DrawCrosshair(CVector vPoint, float fRadius, D2D1_COLOR_F cColor)
{
	SDrawCrosshair cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_DRAWCROSSHAIR;
	cRequest.m_vPoint = vPoint;
	cRequest.m_fRadius = fRadius;
	cRequest.m_cColor = cColor;

	m_iProcComm.Send(cRequest);
}

void COverlay::DrawCircle(CVector vPoint, float fRadius, D2D1_COLOR_F cColor)
{
	SDrawCircle cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_DRAWCIRCLE;
	cRequest.m_vPoint = vPoint;
	cRequest.m_fRadius = fRadius;
	cRequest.m_cColor = cColor;

	m_iProcComm.Send(cRequest);
}
void COverlay::DrawCircle(CVector vPoint, float fRadius)
{
	SDrawCircle cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_DRAWCIRCLE;
	cRequest.m_vPoint = vPoint;
	cRequest.m_fRadius = fRadius;
	cRequest.m_bUsingGradient = true;

	m_iProcComm.Send(cRequest);
}

void COverlay::DrawFilledCircle(CVector vPoint, float fRadius, D2D1_COLOR_F cColor)
{
	SDrawFilledCircle cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_DRAWFILLEDCIRCLE;
	cRequest.m_vPoint = vPoint;
	cRequest.m_fRadius = fRadius;
	cRequest.m_cColor = cColor;

	m_iProcComm.Send(cRequest);
}
void COverlay::DrawFilledCircle(CVector vPoint, float fRadius)
{
	SDrawFilledCircle cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_DRAWFILLEDCIRCLE;
	cRequest.m_vPoint = vPoint;
	cRequest.m_fRadius = fRadius;
	cRequest.m_bUsingGradient = true;

	m_iProcComm.Send(cRequest);
}

void COverlay::DrawEllipse(CVector vPoint, float fRadius1, float fRadius2, D2D1_COLOR_F cColor)
{
	SDrawEllipse cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_DRAWELLIPSE;
	cRequest.m_vPoint = vPoint;
	cRequest.m_fRadius1 = fRadius1;
	cRequest.m_fRadius1 = fRadius2;
	cRequest.m_cColor = cColor;

	m_iProcComm.Send(cRequest);
}
void COverlay::DrawEllipse(CVector vPoint, float fRadius1, float fRadius2)
{
	SDrawEllipse cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_DRAWELLIPSE;
	cRequest.m_vPoint = vPoint;
	cRequest.m_fRadius1 = fRadius1;
	cRequest.m_fRadius1 = fRadius2;
	cRequest.m_bUsingGradient = true;

	m_iProcComm.Send(cRequest);
}

void COverlay::DrawFilledEllipse(CVector vPoint, float fRadius1, float fRadius2, D2D1_COLOR_F cColor)
{
	SDrawFilledEllipse cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_DRAWFILLEDELLIPSE;
	cRequest.m_vPoint = vPoint;
	cRequest.m_fRadius1 = fRadius1;
	cRequest.m_fRadius1 = fRadius2;
	cRequest.m_cColor = cColor;

	m_iProcComm.Send(cRequest);
}
void COverlay::DrawFilledEllipse(CVector vPoint, float fRadius1, float fRadius2)
{
	SDrawFilledEllipse cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_DRAWFILLEDELLIPSE;
	cRequest.m_vPoint = vPoint;
	cRequest.m_fRadius1 = fRadius1;
	cRequest.m_fRadius1 = fRadius2;
	cRequest.m_bUsingGradient = true;

	m_iProcComm.Send(cRequest);
}

void COverlay::DrawRect(CVector vPoint, CVector vSize, D2D1_COLOR_F cColor, bool bCentered)
{
	SDrawRect cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_DRAWRECT;
	cRequest.m_vPoint = vPoint;
	cRequest.m_vSize = vSize;
	cRequest.m_bCentered = bCentered;
	cRequest.m_cColor = cColor;

	m_iProcComm.Send(cRequest);
}
void COverlay::DrawRect(CVector vPoint, CVector vSize, bool bCentered)
{
	SDrawRect cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_DRAWRECT;
	cRequest.m_vPoint = vPoint;
	cRequest.m_vSize = vSize;
	cRequest.m_bCentered = bCentered;
	cRequest.m_bUsingGradient = true;

	m_iProcComm.Send(cRequest);
}

void COverlay::DrawSizeRect(CVector vPoint1, CVector vPoint2, float fFactor, D2D1_COLOR_F cColor)
{
	SDrawSizeRect cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_DRAWSIZERECT;
	cRequest.m_vPoint1 = vPoint1;
	cRequest.m_vPoint2 = vPoint2;
	cRequest.m_fFactor = fFactor;
	cRequest.m_cColor = cColor;

	m_iProcComm.Send(cRequest);
}
void COverlay::DrawSizeRect(CVector vPoint1, CVector vPoint2, float fFactor)
{
	SDrawSizeRect cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_DRAWSIZERECT;
	cRequest.m_vPoint1 = vPoint1;
	cRequest.m_vPoint2 = vPoint2;
	cRequest.m_fFactor = fFactor;
	cRequest.m_bUsingGradient = true;

	m_iProcComm.Send(cRequest);
}

void COverlay::DrawFilledSizeRect(CVector vPoint1, CVector vPoint2, float fFactor, D2D1_COLOR_F cColor)
{
	SDrawFilledSizeRect cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_DRAWFILLEDSIZERECT;
	cRequest.m_vPoint1 = vPoint1;
	cRequest.m_vPoint2 = vPoint2;
	cRequest.m_fFactor = fFactor;
	cRequest.m_cColor = cColor;

	m_iProcComm.Send(cRequest);
}
void COverlay::DrawFilledSizeRect(CVector vPoint1, CVector vPoint2, float fFactor)
{
	SDrawFilledSizeRect cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_DRAWFILLEDSIZERECT;
	cRequest.m_vPoint1 = vPoint1;
	cRequest.m_vPoint2 = vPoint2;
	cRequest.m_fFactor = fFactor;
	cRequest.m_bUsingGradient = true;

	m_iProcComm.Send(cRequest);
}

void COverlay::DrawFilledRect(CVector vPoint, CVector vSize, D2D1_COLOR_F cColor, bool bCentered)
{
	SDrawFilledRect cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_DRAWFILLEDRECT;
	cRequest.m_vPoint = vPoint;
	cRequest.m_vSize = vSize;
	cRequest.m_cColor = cColor;
	cRequest.m_bCentered = bCentered;

	m_iProcComm.Send(cRequest);
}
void COverlay::DrawFilledRect(CVector vPoint, CVector vSize, bool bCentered)
{
	SDrawFilledRect cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_DRAWFILLEDRECT;
	cRequest.m_vPoint = vPoint;
	cRequest.m_vSize = vSize;
	cRequest.m_bCentered = bCentered;
	cRequest.m_bUsingGradient = true;

	m_iProcComm.Send(cRequest);
}

void COverlay::DrawRoundedRect(CVector vPoint, CVector vSize, D2D1_COLOR_F cColor, bool bCentered)
{
	SDrawRoundedRect cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_DRAWROUNDEDRECT;
	cRequest.m_vPoint = vPoint;
	cRequest.m_vSize = vSize;
	cRequest.m_cColor = cColor;
	cRequest.m_bCentered = bCentered;

	m_iProcComm.Send(cRequest);
}
void COverlay::DrawRoundedRect(CVector vPoint, CVector vSize, bool bCentered)
{
	SDrawRoundedRect cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_DRAWROUNDEDRECT;
	cRequest.m_vPoint = vPoint;
	cRequest.m_vSize = vSize;
	cRequest.m_bCentered = bCentered;
	cRequest.m_bUsingGradient = true;

	m_iProcComm.Send(cRequest);
}

void COverlay::DrawFilledRoundedRect(CVector vPoint, CVector vSize, D2D1_COLOR_F cColor, bool bCentered)
{
	SDrawFilledRoundedRect cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_DRAWFILLEDROUNDEDRECT;
	cRequest.m_vPoint = vPoint;
	cRequest.m_vSize = vSize;
	cRequest.m_cColor = cColor;
	cRequest.m_bCentered = bCentered;

	m_iProcComm.Send(cRequest);
}
void COverlay::DrawFilledRoundedRect(CVector vPoint, CVector vSize, bool bCentered)
{
	SDrawFilledRoundedRect cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_DRAWFILLEDROUNDEDRECT;
	cRequest.m_vPoint = vPoint;
	cRequest.m_vSize = vSize;
	cRequest.m_bCentered = bCentered;
	cRequest.m_bUsingGradient = true;

	m_iProcComm.Send(cRequest);
}

void COverlay::DrawRhombus(CVector vPoint1, CVector vPoint2, D2D1_COLOR_F cColor)
{
	SDrawRhombus cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_DRAWRHOMBUS;
	cRequest.m_vPoint1 = vPoint1;
	cRequest.m_vPoint2 = vPoint2;
	cRequest.m_cColor = cColor;

	m_iProcComm.Send(cRequest);
}
void COverlay::DrawRhombus(CVector vPoint1, CVector vPoint2)
{
	SDrawRhombus cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_DRAWRHOMBUS;
	cRequest.m_vPoint1 = vPoint1;
	cRequest.m_vPoint2 = vPoint2;
	cRequest.m_bUsingGradient = true;

	m_iProcComm.Send(cRequest);
}

void COverlay::RenderText(CVector vPoint, CText &tText, D2D1_COLOR_F cText)
{
	SRenderText cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_RENDERTEXT;
	cRequest.m_pTextLayout = tText.m_pTextLayout;
	cRequest.m_vBounds = tText.m_vBounds;
	cRequest.m_vOffset = tText.m_vOffset;
	cRequest.m_bShadow = tText.m_bShadow;
	cRequest.m_vPoint = vPoint;
	cRequest.m_cText = cText;

	m_iProcComm.Send(cRequest);
}
void COverlay::RenderText(CVector vPoint, CText &tText)
{
	SRenderText cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_RENDERTEXT;
	cRequest.m_pTextLayout = tText.m_pTextLayout;
	cRequest.m_vBounds = tText.m_vBounds;
	cRequest.m_vOffset = tText.m_vOffset;
	cRequest.m_bShadow = tText.m_bShadow;
	cRequest.m_vPoint = vPoint;
	cRequest.m_bUsingGradient = true;

	m_iProcComm.Send(cRequest);
}
#pragma endregion

#pragma region CText
CText::CText() :
	m_pOverlay(COverlay::Get()),
	m_iProcComm(m_pOverlay->GetIPC()),
	m_pTextLayout(nullptr),
	m_bShadow(false),
	m_bUseCached(true),
	m_fFontSize(10.0F) { }
CText::CText(bool bUseCached) :
	m_pOverlay(COverlay::Get()),
	m_iProcComm(m_pOverlay->GetIPC()),
	m_pTextLayout(nullptr),
	m_bShadow(false),
	m_bUseCached(bUseCached),
	m_fFontSize(10.0F) { }
CText::~CText()
{
	if (!m_bUseCached)
		ReleaseTextLayout();
}

// [Public]:
std::wstring& CText::GetContent()
{
	return m_wContent;
}

CVector& CText::GetBounds()
{
	return m_vBounds;
}

bool CText::GetShadow()
{
	return m_bShadow;
}
void CText::SetShadow(bool bShadow)
{
	m_bShadow = bShadow;
}

float CText::GetFontSize()
{
	return m_fFontSize;
}
void CText::SetFontSize(float fSize)
{
	if (m_pTextLayout)
	{
		SGetFontSize cGetRequest{ };
		cGetRequest.m_uCode = ECOMMAND_CODE_GETFONTSIZE;
		cGetRequest.m_pTextLayout = m_pTextLayout;

		m_iProcComm.Send(cGetRequest, cGetRequest);

		if (cGetRequest.m_fFontSize != fSize || cGetRequest.m_fFontSize != m_fFontSize)
		{
			m_fFontSize = fSize;

			SSetFontSize cSetRequest{ };
			cSetRequest.m_uCode = ECOMMAND_CODE_SETFONTSIZE;
			cSetRequest.m_pTextLayout = m_pTextLayout;
			cSetRequest.m_fFontSize = m_fFontSize;
			cSetRequest.m_dTextRange = { 0, (unsigned int)m_wContent.length() };

			m_iProcComm.Send(cSetRequest);

			UpdateBounds();
		}
	}
}

DWRITE_TEXT_ALIGNMENT CText::GetAlignment()
{
	if (m_pTextLayout)
	{
		SGetTextAlignment cRequest{ };
		cRequest.m_uCode = ECOMMAND_CODE_GETTEXTALIGNMENT;
		cRequest.m_pTextLayout = m_pTextLayout;

		m_iProcComm.Send(cRequest, cRequest);

		return cRequest.m_dTextAlignment;
	}

	return DWRITE_TEXT_ALIGNMENT_JUSTIFIED;
}
void CText::SetAlignment(DWRITE_TEXT_ALIGNMENT dAlignment, __int64 iOffset)
{
	if (m_pTextLayout && GetAlignment() != dAlignment)
	{
		m_dAlignment = dAlignment;

		SSetTextAlignment cRequest{ };
		cRequest.m_uCode = ECOMMAND_CODE_SETTEXTALIGNMENT;
		cRequest.m_pTextLayout = m_pTextLayout;
		cRequest.m_dTextAlignment = m_dAlignment;

		m_iProcComm.Send(cRequest);
	}

	m_iOffset = iOffset;

	// Horizontal
	if (m_iOffset & ETEXT_ALIGNMENT_HC)
		m_vOffset.x = -m_vBounds.x * 0.5F;

	if (m_iOffset & ETEXT_ALIGNMENT_HL)
		m_vOffset.x = -m_vBounds.x;

	if (m_iOffset & ETEXT_ALIGNMENT_HR)
		m_vOffset.x = m_vBounds.x;

	// Vertical
	if (m_iOffset & ETEXT_ALIGNMENT_VC)
		m_vOffset.y = -m_vBounds.y * 0.5F;

	if (m_iOffset & ETEXT_ALIGNMENT_VT)
		m_vOffset.y = -m_vBounds.y;

	if (m_iOffset & ETEXT_ALIGNMENT_VB)
		m_vOffset.y = m_vBounds.y;
}

// [Private]:
void CText::UpdateBounds()
{
	SGetTextMetrics cMetricsRequest{ };
	cMetricsRequest.m_uCode = ECOMMAND_CODE_GETTEXTMETRICS;
	cMetricsRequest.m_pTextLayout = m_pTextLayout;

	m_iProcComm.Send(cMetricsRequest, cMetricsRequest);

	m_vBounds = {
		cMetricsRequest.m_dTextMetrics.width,
		cMetricsRequest.m_dTextMetrics.height
	};

	SSetMaxWidth cMaxWidthRequest{ };
	cMaxWidthRequest.m_uCode = ECOMMAND_CODE_SETMAXWIDTH;
	cMaxWidthRequest.m_pTextLayout = m_pTextLayout;
	cMaxWidthRequest.m_fMaxWidth = m_vBounds.x;

	m_iProcComm.Send(cMaxWidthRequest);

	SSetMaxHeight cMaxHeightRequest{ };
	cMaxHeightRequest.m_uCode = ECOMMAND_CODE_SETMAXHEIGHT;
	cMaxHeightRequest.m_pTextLayout = m_pTextLayout;
	cMaxHeightRequest.m_fMaxHeight = m_vBounds.y;

	m_iProcComm.Send(cMaxHeightRequest);
}

void CText::ReleaseTextLayout()
{
	SReleaseTextLayout cRequest{ };
	cRequest.m_uCode = ECOMMAND_CODE_RELEASETEXTLAYOUT;
	cRequest.m_pTextLayout = m_pTextLayout;

	m_iProcComm.Send(cRequest);
}
#pragma endregion