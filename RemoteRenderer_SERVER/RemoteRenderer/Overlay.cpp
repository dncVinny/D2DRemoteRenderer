#include "Overlay.h"
#include "Commands.h"

#include <TlHelp32.h>

#pragma comment(lib, "dwmapi.lib")
#include <dwmapi.h>

#include <stdexcept>

#pragma region Helper Functions
inline D2D_POINT_2F GetPoint2F(CVector vPoint)
{
	return D2D_POINT_2F{ vPoint.x, vPoint.y };
}
#pragma endregion

#pragma region CRenderingCanvas
CRenderingCanvas::CRenderingCanvas(const char *pClass, const char *pTitle) :
	m_hWindow(FindWindowA(pClass, pTitle))
{
	if (!SetupOverlay())
		throw std::runtime_error("[CRenderingCanvas]: failed to setup overlay.");

	RECT rWindowRegion;
	GetWindowRect(m_hWindow, &rWindowRegion);

	m_vSize = {
		(float)(rWindowRegion.right - rWindowRegion.left),
		(float)(rWindowRegion.bottom - rWindowRegion.top)
	};

	m_vCenter = m_vSize * 0.5F;
}
CRenderingCanvas::~CRenderingCanvas()
{

}

// [Private]:

bool CRenderingCanvas::SetupOverlay()
{
	if (!m_hWindow)
		return false;

	SetWindowLongPtrA(m_hWindow, GWL_EXSTYLE, GetWindowLongA(m_hWindow, GWL_EXSTYLE) | WS_EX_TRANSPARENT);

	MARGINS margin { -1 };
	if (DwmExtendFrameIntoClientArea(m_hWindow, &margin) != S_OK)
		return false;

	if (!SetLayeredWindowAttributes(m_hWindow, 0x000000, 0xFF, 0x02))
		return false;

	if (!SetWindowPos(m_hWindow, HWND_TOPMOST, 0, 0, 0, 0, 0x0002 | 0x0001))
		return false;

	ShowWindow(m_hWindow, SW_SHOW);

	return true;
}

// [Public]:
HWND CRenderingCanvas::GetHandle()
{
	return m_hWindow;
}

CVector CRenderingCanvas::GetCenter()
{
	return m_vCenter;
}
CVector CRenderingCanvas::GetSize()
{
	return m_vSize;
}
#pragma endregion

#pragma region CDirectRenderer2D
CDirectRenderer2D::CDirectRenderer2D(CRenderingCanvas &rCanvas) :
	m_rCanvas(rCanvas)
{
	if (!SetupDirect2D())
		throw std::runtime_error("[CDirectRenderer2D]: failed to setup direct2d.");

	if (!SetupDirectWrite())
		throw std::runtime_error("[CDirectRenderer2D]: failed to setup directwrite.");
}
CDirectRenderer2D::~CDirectRenderer2D()
{
	// Direct2D objects
	if (m_pLinearGradientBrush)
		m_pLinearGradientBrush->Release();

	if (m_pGradientStopCollection)
		m_pGradientStopCollection->Release();

	if (m_pSolidColorBrush)
		m_pSolidColorBrush->Release();

	if (m_pRenderTarget)
		m_pRenderTarget->Release();

	if (m_pFactory)
		m_pFactory->Release();

	// DirectWrite objects
	if (m_pTextFormat)
		m_pTextFormat->Release();

	if (m_pWriteFactory)
		m_pWriteFactory->Release();
}

// [Private]:
bool CDirectRenderer2D::SetupDirect2D()
{
	D2D1_RENDER_TARGET_PROPERTIES rRenderTargetProperties = {
		D2D1_RENDER_TARGET_TYPE_HARDWARE,
		{ DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED },
		0.0F, 0.0F,
		D2D1_RENDER_TARGET_USAGE_NONE, D2D1_FEATURE_LEVEL_DEFAULT,
	};
	D2D1_HWND_RENDER_TARGET_PROPERTIES rHwndRenderTargetProperties = {
		m_rCanvas.GetHandle(),
		{ m_rCanvas.GetSize().x, m_rCanvas.GetSize().y },
		D2D1_PRESENT_OPTIONS_RETAIN_CONTENTS
	};
	D2D1_GRADIENT_STOP gGradientStop[2] = {
		{ 0.0F, COLORF_FROMRGBA(255, 255, 255, 1.0F) },
		{ 1.0F, COLORF_FROMRGBA(45, 45, 45, 1.0F) }
	};
	D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES pLinearBrushGradient = {
		{ 0.0F, 0.0F },
		{ 1.0F, 0.0F }
	};

	if FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pFactory))
		return false;
	if FAILED(m_pFactory->CreateHwndRenderTarget(&rRenderTargetProperties, &rHwndRenderTargetProperties, &m_pRenderTarget))
		return false;
	if FAILED(m_pRenderTarget->CreateSolidColorBrush(COLORF_FROMRGBA(255, 255, 255, 1.0F), &m_pSolidColorBrush))
		return false;
	if FAILED(m_pRenderTarget->CreateGradientStopCollection(gGradientStop, 2, D2D1_GAMMA_1_0, D2D1_EXTEND_MODE_CLAMP, &m_pGradientStopCollection))
		return false;
	if FAILED(m_pRenderTarget->CreateLinearGradientBrush(pLinearBrushGradient, m_pGradientStopCollection, &m_pLinearGradientBrush))
		return false;


	return true;
}
bool CDirectRenderer2D::SetupDirectWrite()
{
	if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(m_pWriteFactory), (IUnknown**)&m_pWriteFactory)) ||
		FAILED(m_pWriteFactory->CreateTextFormat(L"Consolas", 0, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 10.0F, L"en-us", &m_pTextFormat)))
		return false;

	m_pRenderTarget->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_DEFAULT);
	m_pTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

	return true;
}

// [Public]:
#pragma endregion

#pragma region COverlay
COverlay *COverlay::m_pOverlay = nullptr;

COverlay::COverlay() :
	m_rCanvas("CEF-OSC-WIDGET", "NVIDIA GeForce Overlay"),
	m_bInteractive(false),
	m_dRenderer(m_rCanvas),
	m_fThickness(1.0F), m_fRoundedRadius(2.0F) { }
COverlay::~COverlay()
{
	Sleep(100);

	BeginDraw();
	Clear();
	EndDraw();

	for (const auto &[key, value] : m_uLayouts)
	{
		if (value)
			value->Release();
	}
}

// [Private]:
IDWriteTextLayout* COverlay::CreateTextLayout(std::wstring_view wContent)
{
	IDWriteTextLayout *pTemp;

	CVector vBufferResolution = m_pOverlay->GetScreenSize();

	if SUCCEEDED(m_pOverlay->m_dRenderer.m_pWriteFactory->CreateTextLayout(wContent.data(), (unsigned int)wContent.length(),
		m_dRenderer.m_pTextFormat, vBufferResolution.x, vBufferResolution.y, &pTemp))
	{
		return pTemp;
	}

	return nullptr;
}
IDWriteTextLayout* COverlay::GetCachedTextLayout(std::wstring wContent)
{
	if (m_uLayouts.find(wContent) != m_uLayouts.end())
		return m_uLayouts[wContent];

	IDWriteTextLayout *pTextLayout = CreateTextLayout(wContent);

	if (pTextLayout)
	{
		m_uLayouts[wContent] = pTextLayout;
		return pTextLayout;
	}

	return nullptr;
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

void COverlay::BeginDraw()
{
	m_dRenderer.m_pRenderTarget->BeginDraw();
}
void COverlay::EndDraw()
{
	m_dRenderer.m_pRenderTarget->EndDraw();
}
void COverlay::Clear()
{
	static D2D1_COLOR_F cTransparent = COLORF_FROMRGBA(0, 0, 0, 0.0F);
	m_dRenderer.m_pRenderTarget->Clear(cTransparent);
}

CVector COverlay::GetScreenSize()
{
	return m_rCanvas.GetSize();
}
CVector COverlay::GetScreenCenter()
{
	return m_rCanvas.GetCenter();
}
float COverlay::GetAspectRatio()
{
	return m_rCanvas.GetSize().x / m_rCanvas.GetSize().y;
}

void COverlay::SetAntialias(bool bEnabled)
{
	m_dRenderer.m_pRenderTarget->SetAntialiasMode(bEnabled ? D2D1_ANTIALIAS_MODE_PER_PRIMITIVE : D2D1_ANTIALIAS_MODE_ALIASED);
}
bool COverlay::GetAntialias()
{
	return m_dRenderer.m_pRenderTarget->GetAntialiasMode() == D2D1_ANTIALIAS_MODE_ALIASED;
}

void COverlay::SetThickness(float fThickness)
{
	m_fThickness = fThickness;
}
void COverlay::SetRoundedRadius(float fRadius)
{
	m_fRoundedRadius = fRadius;
}

void COverlay::PushClipRender(CVector vPosition, CVector vSize)
{
	D2D1_LAYER_PARAMETERS lParameters = {
		{ vPosition.x, vPosition.y, vPosition.x + vSize.x, vPosition.y + vSize.y }, nullptr,
		D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
		D2D1::IdentityMatrix(), 1.0F,
		nullptr, D2D1_LAYER_OPTIONS_NONE
	};

	m_dRenderer.m_pRenderTarget->PushLayer(lParameters, nullptr);
}
void COverlay::PopClipRender()
{
	m_dRenderer.m_pRenderTarget->PopLayer();
}

void COverlay::UpdateGradientBrush(SGradientSettings gSettings)
{
	if (m_dRenderer.m_pLinearGradientBrush)
		m_dRenderer.m_pLinearGradientBrush->Release();

	if (m_dRenderer.m_pGradientStopCollection)
		m_dRenderer.m_pGradientStopCollection->Release();

	D2D1_GRADIENT_STOP gGradientStop[2] = {
		{ 0.0F, gSettings.m_cColor1 },
		{ 1.0F, gSettings.m_cColor2 }
	};

	if SUCCEEDED(m_dRenderer.m_pRenderTarget->CreateGradientStopCollection(gGradientStop, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &m_dRenderer.m_pGradientStopCollection))
	{
		D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES pLinearBrushGradient = {
			GetPoint2F(gSettings.m_vStart),
			GetPoint2F(gSettings.m_vEnd)
		};

		m_dRenderer.m_pRenderTarget->CreateLinearGradientBrush(pLinearBrushGradient, m_dRenderer.m_pGradientStopCollection, &m_dRenderer.m_pLinearGradientBrush);
	}
}
void COverlay::UpdateGradientBrush(CVector vStart, CVector vEnd)
{
	m_dRenderer.m_pLinearGradientBrush->SetStartPoint(GetPoint2F(vStart));
	m_dRenderer.m_pLinearGradientBrush->SetEndPoint(GetPoint2F(vEnd));
}
void COverlay::UpdateColorBrush(D2D1_COLOR_F cColor)
{
	D2D1_COLOR_F cTemp = m_dRenderer.m_pSolidColorBrush->GetColor();

	if (cTemp.r != cColor.r || cTemp.g != cColor.g || cTemp.b != cColor.b || cTemp.a != cColor.a)
		m_dRenderer.m_pSolidColorBrush->SetColor(cColor);
}

void COverlay::DrawLine(CVector vPoint1, CVector vPoint2, D2D1_COLOR_F cColor)
{
	m_dRenderer.m_pSolidColorBrush->SetColor(cColor);
	m_dRenderer.m_pRenderTarget->DrawLine(GetPoint2F(vPoint1), GetPoint2F(vPoint2), m_dRenderer.m_pSolidColorBrush, m_fThickness);
}
void COverlay::DrawLine(CVector vPoint1, CVector vPoint2)
{
	UpdateGradientBrush(vPoint1, vPoint2);
	m_dRenderer.m_pRenderTarget->DrawLine(GetPoint2F(vPoint1), GetPoint2F(vPoint2), m_dRenderer.m_pLinearGradientBrush, m_fThickness);
}

void COverlay::DrawCrosshair(CVector vPoint, float fRadius, D2D1_COLOR_F cColor)
{
	SetAntialias(false);

	DrawLine(CVector(vPoint.x - fRadius - 1.0F, vPoint.y), CVector(vPoint.x + fRadius, vPoint.y), cColor);
	DrawLine(CVector(vPoint.x, vPoint.y - fRadius - 1.0F), CVector(vPoint.x, vPoint.y + fRadius), cColor);

	SetAntialias(true);
}

void COverlay::DrawCircle(CVector vPoint, float fRadius, D2D1_COLOR_F cColor)
{
	m_dRenderer.m_pSolidColorBrush->SetColor(cColor);
	m_dRenderer.m_pRenderTarget->DrawEllipse({ GetPoint2F(vPoint), fRadius, fRadius }, m_dRenderer.m_pSolidColorBrush, m_fThickness);
}
void COverlay::DrawCircle(CVector vPoint, float fRadius)
{
	UpdateGradientBrush(vPoint - CVector(fRadius, 0.0F), vPoint + CVector(fRadius, 0.0F));
	m_dRenderer.m_pRenderTarget->DrawEllipse({ GetPoint2F(vPoint), fRadius, fRadius }, m_dRenderer.m_pLinearGradientBrush, m_fThickness);
}

void COverlay::DrawFilledCircle(CVector vPoint, float fRadius, D2D1_COLOR_F cColor)
{
	m_dRenderer.m_pSolidColorBrush->SetColor(cColor);
	m_dRenderer.m_pRenderTarget->FillEllipse({ GetPoint2F(vPoint), fRadius, fRadius }, m_dRenderer.m_pSolidColorBrush);
}
void COverlay::DrawFilledCircle(CVector vPoint, float fRadius)
{
	UpdateGradientBrush(vPoint - CVector(fRadius, 0.0F), vPoint + CVector(fRadius, 0.0F));
	m_dRenderer.m_pRenderTarget->FillEllipse({ GetPoint2F(vPoint), fRadius, fRadius }, m_dRenderer.m_pLinearGradientBrush);
}

void COverlay::DrawEllipse(CVector vPoint, float fRadius1, float fRadius2, D2D1_COLOR_F cColor)
{
	m_dRenderer.m_pSolidColorBrush->SetColor(cColor);
	m_dRenderer.m_pRenderTarget->DrawEllipse({ GetPoint2F(vPoint), fRadius1, fRadius2 }, m_dRenderer.m_pSolidColorBrush, m_fThickness);
}
void COverlay::DrawEllipse(CVector vPoint, float fRadius1, float fRadius2)
{
	UpdateGradientBrush(vPoint - CVector(fRadius1, fRadius1), vPoint + CVector(fRadius1, fRadius1));
	m_dRenderer.m_pRenderTarget->DrawEllipse({ GetPoint2F(vPoint), fRadius1, fRadius2 }, m_dRenderer.m_pLinearGradientBrush, m_fThickness);
}

void COverlay::DrawFilledEllipse(CVector vPoint, float fRadius1, float fRadius2, D2D1_COLOR_F cColor)
{
	m_dRenderer.m_pSolidColorBrush->SetColor(cColor);
	m_dRenderer.m_pRenderTarget->FillEllipse({ GetPoint2F(vPoint), fRadius1, fRadius2 }, m_dRenderer.m_pSolidColorBrush);
}
void COverlay::DrawFilledEllipse(CVector vPoint, float fRadius1, float fRadius2)
{
	UpdateGradientBrush(vPoint - CVector(fRadius1, fRadius1), vPoint + CVector(fRadius1, fRadius1));
	m_dRenderer.m_pRenderTarget->FillEllipse({ GetPoint2F(vPoint), fRadius1, fRadius2 }, m_dRenderer.m_pLinearGradientBrush);
}

void COverlay::DrawRect(CVector vPoint, CVector vSize, D2D1_COLOR_F cColor, bool bCentered)
{
	m_dRenderer.m_pSolidColorBrush->SetColor(cColor);
	vPoint = bCentered ? vPoint - (vSize * 0.5F) : vPoint;

	m_dRenderer.m_pRenderTarget->DrawRectangle({ vPoint.x, vPoint.y, vPoint.x + vSize.x, vPoint.y + vSize.y }, m_dRenderer.m_pSolidColorBrush, m_fThickness);
}
void COverlay::DrawRect(CVector vPoint, CVector vSize, bool bCentered)
{
	UpdateGradientBrush(vPoint - (bCentered ? CVector(vSize.x, 0.0F) : CVector::vEmpty), vPoint + (bCentered ? CVector(vSize.x, 0.0F) : CVector::vEmpty));
	m_dRenderer.m_pRenderTarget->DrawRectangle({ vPoint.x, vPoint.y, vPoint.x + vSize.x, vPoint.y + vSize.y }, m_dRenderer.m_pLinearGradientBrush, m_fThickness);
}

void COverlay::DrawSizeRect(CVector vPoint1, CVector vPoint2, float fFactor, D2D1_COLOR_F cColor)
{
	m_dRenderer.m_pSolidColorBrush->SetColor(cColor);

	float fMagnitude = ((vPoint2.y - vPoint1.y) * 0.5F);
	CVector vAverage = ((vPoint1 + vPoint2) * 0.5F);

	m_dRenderer.m_pRenderTarget->DrawRectangle({ vAverage.x - (fMagnitude / fFactor), vAverage.y - fMagnitude, vAverage.x + (fMagnitude / fFactor), vAverage.y + fMagnitude }, m_dRenderer.m_pSolidColorBrush, m_fThickness);
}
void COverlay::DrawSizeRect(CVector vPoint1, CVector vPoint2, float fFactor)
{
	float fMagnitude = ((vPoint2.y - vPoint1.y) * 0.5F);
	UpdateGradientBrush(vPoint1 - CVector(fMagnitude / fFactor, 0.0F), vPoint2 + CVector(fMagnitude / fFactor, 0.0F));

	CVector vAverage = ((vPoint1 + vPoint2) * 0.5F);

	m_dRenderer.m_pRenderTarget->DrawRectangle({ vAverage.x - (fMagnitude / fFactor), vAverage.y - fMagnitude, vAverage.x + (fMagnitude / fFactor), vAverage.y + fMagnitude }, m_dRenderer.m_pLinearGradientBrush, m_fThickness);
}

void COverlay::DrawFilledSizeRect(CVector vPoint1, CVector vPoint2, float fFactor, D2D1_COLOR_F cColor)
{
	m_dRenderer.m_pSolidColorBrush->SetColor(cColor);

	float fMagnitude = ((vPoint2.y - vPoint1.y) * 0.5F);
	CVector vAverage = ((vPoint1 + vPoint2) * 0.5F);

	m_dRenderer.m_pRenderTarget->FillRectangle({ vAverage.x - (fMagnitude / fFactor), vAverage.y - fMagnitude, vAverage.x + (fMagnitude / fFactor), vAverage.y + fMagnitude }, m_dRenderer.m_pSolidColorBrush);
}
void COverlay::DrawFilledSizeRect(CVector vPoint1, CVector vPoint2, float fFactor)
{
	float fMagnitude = ((vPoint2.y - vPoint1.y) * 0.5F);
	UpdateGradientBrush(vPoint1 - CVector(fMagnitude / fFactor, 0.0F), vPoint2 + CVector(fMagnitude / fFactor, 0.0F));

	CVector vAverage = ((vPoint1 + vPoint2) * 0.5F);

	m_dRenderer.m_pRenderTarget->FillRectangle({ vAverage.x - (fMagnitude / fFactor), vAverage.y - fMagnitude, vAverage.x + (fMagnitude / fFactor), vAverage.y + fMagnitude }, m_dRenderer.m_pLinearGradientBrush);
}

void COverlay::DrawFilledRect(CVector vPoint, CVector vSize, D2D1_COLOR_F cColor, bool bCentered)
{
	m_dRenderer.m_pSolidColorBrush->SetColor(cColor);

	vPoint = bCentered ? vPoint - (vSize * 0.5F) : vPoint;
	m_dRenderer.m_pRenderTarget->FillRectangle({ vPoint.x, vPoint.y, vPoint.x + vSize.x, vPoint.y + vSize.y }, m_dRenderer.m_pSolidColorBrush);
}
void COverlay::DrawFilledRect(CVector vPoint, CVector vSize, bool bCentered)
{
	UpdateGradientBrush(vPoint - (bCentered ? CVector(vSize.x, 0.0F) : CVector::vEmpty), vPoint + (bCentered ? CVector(vSize.x, 0.0F) : CVector::vEmpty));

	vPoint = bCentered ? vPoint - (vSize * 0.5F) : vPoint;
	m_dRenderer.m_pRenderTarget->FillRectangle({ vPoint.x, vPoint.y, vPoint.x + vSize.x, vPoint.y + vSize.y }, m_dRenderer.m_pLinearGradientBrush);
}

void COverlay::DrawRoundedRect(CVector vPoint, CVector vSize, D2D1_COLOR_F cColor, bool bCentered)
{
	m_dRenderer.m_pSolidColorBrush->SetColor(cColor);

	vPoint = bCentered ? vPoint - (vSize * 0.5F) : vPoint;
	m_dRenderer.m_pRenderTarget->DrawRoundedRectangle({ { vPoint.x, vPoint.y, vPoint.x + vSize.x, vPoint.y + vSize.y }, m_fRoundedRadius, m_fRoundedRadius }, m_dRenderer.m_pSolidColorBrush, m_fThickness);
}
void COverlay::DrawRoundedRect(CVector vPoint, CVector vSize, bool bCentered)
{
	UpdateGradientBrush(vPoint - (bCentered ? CVector(vSize.x, 0.0F) : CVector::vEmpty), vPoint + (bCentered ? CVector(vSize.x, 0.0F) : CVector(vSize.x, 0.0F)));

	vPoint = bCentered ? vPoint - (vSize * 0.5F) : vPoint;
	m_dRenderer.m_pRenderTarget->DrawRoundedRectangle({ { vPoint.x, vPoint.y, vPoint.x + vSize.x, vPoint.y + vSize.y }, m_fRoundedRadius, m_fRoundedRadius }, m_dRenderer.m_pLinearGradientBrush, m_fThickness);
}

void COverlay::DrawFilledRoundedRect(CVector vPoint, CVector vSize, D2D1_COLOR_F cColor, bool bCentered)
{
	m_dRenderer.m_pSolidColorBrush->SetColor(cColor);

	vPoint = bCentered ? vPoint - (vSize * 0.5F) : vPoint;
	m_dRenderer.m_pRenderTarget->FillRoundedRectangle({ { vPoint.x, vPoint.y, vPoint.x + vSize.x, vPoint.y + vSize.y }, m_fRoundedRadius, m_fRoundedRadius }, m_dRenderer.m_pSolidColorBrush);
}
void COverlay::DrawFilledRoundedRect(CVector vPoint, CVector vSize, bool bCentered)
{
	UpdateGradientBrush(vPoint - (bCentered ? CVector(vSize.x, 0.0F) : CVector::vEmpty), vPoint + (bCentered ? CVector(vSize.x, 0.0F) : CVector(vSize.x, 0.0F)));

	vPoint = bCentered ? vPoint - (vSize * 0.5F) : vPoint;
	m_dRenderer.m_pRenderTarget->FillRoundedRectangle({ { vPoint.x, vPoint.y, vPoint.x + vSize.x, vPoint.y + vSize.y }, m_fRoundedRadius, m_fRoundedRadius }, m_dRenderer.m_pLinearGradientBrush);
}

void COverlay::DrawRhombus(CVector vPoint1, CVector vPoint2, D2D1_COLOR_F cColor)
{
	m_dRenderer.m_pSolidColorBrush->SetColor(cColor);

	float fMagnitude = (vPoint1.y - vPoint2.y) / 4.0F;
	CVector vMagnitude = { fMagnitude, 0.0F };

	m_dRenderer.m_pRenderTarget->DrawLine(GetPoint2F(vPoint1 - vMagnitude), GetPoint2F(vPoint2 - vMagnitude), m_dRenderer.m_pSolidColorBrush, m_fThickness);
	m_dRenderer.m_pRenderTarget->DrawLine(GetPoint2F(vPoint1 + vMagnitude), GetPoint2F(vPoint2 + vMagnitude), m_dRenderer.m_pSolidColorBrush, m_fThickness);
	m_dRenderer.m_pRenderTarget->DrawLine(GetPoint2F(vPoint1 - vMagnitude), GetPoint2F(vPoint1 + vMagnitude), m_dRenderer.m_pSolidColorBrush, m_fThickness);
	m_dRenderer.m_pRenderTarget->DrawLine(GetPoint2F(vPoint2 - vMagnitude), GetPoint2F(vPoint2 + vMagnitude), m_dRenderer.m_pSolidColorBrush, m_fThickness);
}
void COverlay::DrawRhombus(CVector vPoint1, CVector vPoint2)
{
	float fMagnitude = (vPoint1.y - vPoint2.y) / 4.0F;
	UpdateGradientBrush(vPoint1 - CVector(fMagnitude, 0.0F), vPoint2 - CVector(fMagnitude, 0.0F));

	CVector vMagnitude = { fMagnitude, 0.0F };

	m_dRenderer.m_pRenderTarget->DrawLine(GetPoint2F(vPoint1 - vMagnitude), GetPoint2F(vPoint2 - vMagnitude), m_dRenderer.m_pLinearGradientBrush, m_fThickness);
	m_dRenderer.m_pRenderTarget->DrawLine(GetPoint2F(vPoint1 + vMagnitude), GetPoint2F(vPoint2 + vMagnitude), m_dRenderer.m_pLinearGradientBrush, m_fThickness);
	m_dRenderer.m_pRenderTarget->DrawLine(GetPoint2F(vPoint1 - vMagnitude), GetPoint2F(vPoint1 + vMagnitude), m_dRenderer.m_pLinearGradientBrush, m_fThickness);
	m_dRenderer.m_pRenderTarget->DrawLine(GetPoint2F(vPoint2 - vMagnitude), GetPoint2F(vPoint2 + vMagnitude), m_dRenderer.m_pLinearGradientBrush, m_fThickness);
}

static D2D1_COLOR_F cShadow = COLORF_FROMRGBA(25, 25, 25, 1.0F);
void COverlay::RenderText(CVector vPoint, SText tText, D2D1_COLOR_F cText)
{
	if (tText.m_pTextLayout)
	{
		if (tText.m_bShadow)
		{
			UpdateColorBrush(cShadow);
			m_dRenderer.m_pRenderTarget->DrawTextLayout(GetPoint2F(vPoint + tText.m_vOffset + 1.0F), tText.m_pTextLayout, m_dRenderer.m_pSolidColorBrush);
		}

		UpdateColorBrush(cText);
		m_dRenderer.m_pRenderTarget->DrawTextLayout(GetPoint2F(vPoint + tText.m_vOffset), tText.m_pTextLayout, m_dRenderer.m_pSolidColorBrush);
	}
}
void COverlay::RenderText(CVector vPoint, SText tText)
{
	if (tText.m_pTextLayout)
	{
		UpdateGradientBrush(vPoint, vPoint + tText.m_vBounds);

		if (tText.m_bShadow)
		{
			UpdateColorBrush(cShadow);

			m_dRenderer.m_pRenderTarget->DrawTextLayout(GetPoint2F(vPoint + tText.m_vOffset + 1.0F), tText.m_pTextLayout, m_dRenderer.m_pSolidColorBrush);
		}

		m_dRenderer.m_pRenderTarget->DrawTextLayout(GetPoint2F(vPoint + tText.m_vOffset), tText.m_pTextLayout, m_dRenderer.m_pLinearGradientBrush);
	}
}
#pragma endregion

#pragma region CText
// [Public]:
CVector SText::UpdateBounds()
{
	DWRITE_TEXT_METRICS dTextMetrics;
	m_pTextLayout->GetMetrics(&dTextMetrics);

	CVector vBounds = { dTextMetrics.width, dTextMetrics.height };

	m_pTextLayout->SetMaxWidth(vBounds.x);
	m_pTextLayout->SetMaxHeight(vBounds.y);

	return vBounds;
}

// [Private]:
#pragma endregion
