#pragma once

#include "Overlay.h"

enum ECOMMAND_CODE
{
    ECOMMAND_CODE_INVALID,

    // COverlay
    ECOMMAND_CODE_BEGINDRAW, //NO PARAMS
    ECOMMAND_CODE_ENDDRAW, //NO PARAMS
    ECOMMAND_CODE_CLEAR, //NO PARAMS
    ECOMMAND_CODE_GETSCREENSIZE, //NO PARAMS
    ECOMMAND_CODE_GETSCREENCENTER, //NO PARAMS
    ECOMMAND_CODE_GETASPECTRATIO, //NO PARAMS
    ECOMMAND_CODE_SETANTIALIAS,
    ECOMMAND_CODE_GETANTIALIAS, //NO PARAMS
    ECOMMAND_CODE_SETTHICKNESS,
    ECOMMAND_CODE_SETROUNDEDRADIUS,
    ECOMMAND_CODE_UPDATEGRADIENTBRUSH,
    ECOMMAND_CODE_UPDATEGRADIENTBRUSHVECTOR,
    ECOMMAND_CODE_UPDATECOLORBRUSH,
    ECOMMAND_CODE_PUSHCLIPRENDER,
    ECOMMAND_CODE_POPCLIPRENDER, //NO PARAMS
    ECOMMAND_CODE_DRAWLINE,
    ECOMMAND_CODE_DRAWCROSSHAIR,
    ECOMMAND_CODE_DRAWCIRCLE,
    ECOMMAND_CODE_DRAWFILLEDCIRCLE,
    ECOMMAND_CODE_DRAWELLIPSE,
    ECOMMAND_CODE_DRAWFILLEDELLIPSE,
    ECOMMAND_CODE_DRAWRECT,
    ECOMMAND_CODE_DRAWSIZERECT,
    ECOMMAND_CODE_DRAWFILLEDSIZERECT,
    ECOMMAND_CODE_DRAWFILLEDRECT,
    ECOMMAND_CODE_DRAWROUNDEDRECT,
    ECOMMAND_CODE_DRAWFILLEDROUNDEDRECT,
    ECOMMAND_CODE_DRAWRHOMBUS,
    ECOMMAND_CODE_RENDERTEXT,
    ECOMMAND_CODE_CREATETEXTLAYOUT,
    ECOMMAND_CODE_GETCACHEDTEXTLAYOUT,

    // CText
    ECOMMAND_CODE_RELEASETEXTLAYOUT,
    ECOMMAND_CODE_GETFONTSIZE,
    ECOMMAND_CODE_SETFONTSIZE,
    ECOMMAND_CODE_GETTEXTMETRICS,
    ECOMMAND_CODE_SETMAXWIDTH,
    ECOMMAND_CODE_SETMAXHEIGHT,
    ECOMMAND_CODE_GETTEXTALIGNMENT,
    ECOMMAND_CODE_SETTEXTALIGNMENT,

    // General
    ECOMMAND_CODE_SHUTDOWN
};

#pragma pack(push,1)
struct SCommand
{
    unsigned char m_uCode;
};

namespace command
{
#pragma region COverlay
    struct SGetScreenSize : public SCommand
    {
        CVector m_vSize;
    };
    struct SGetScreenCenter : public SCommand
    {
        CVector m_vPosition;
    };
    struct SGetAspectRatio : public SCommand
    {
        float m_fRatio;
    };
    struct SSetAntiAlias : public SCommand
    {
        bool m_bEnabled;
    };
    struct SGetAntiAlias : public SCommand
    {
        bool m_bAntiAlias;
    };
    struct SSetThickness : public SCommand
    {
        float m_fThickness;
    };
    struct SSetRoundedRadius : public SCommand
    {
        float m_fRadius;
    };
    struct SUpdateGradientBrush : public SCommand, public SGradientSettings { };
    struct SUpdateGradientBrushVector : public SCommand
    {
        CVector m_vStart, m_vEnd;
    };
    struct SUpdateColorBrush : public SCommand
    {
        D2D1_COLOR_F m_cColor;
    };
    struct SPushClipRender : public SCommand
    {
        CVector m_vPosition, m_vSize;
    };
    struct SPopClipRender : public SCommand { };
    struct SDrawLine : public SCommand
    {
        CVector m_vPoint1, m_vPoint2;
        D2D1_COLOR_F m_cColor;
        bool m_bUsingGradient = false;
    };
    struct SDrawCrosshair : public SCommand
    {
        CVector m_vPoint;
        float m_fRadius;
        D2D1_COLOR_F m_cColor;
    };
    struct SDrawCircle : public SCommand
    {
        CVector m_vPoint;
        float m_fRadius;
        D2D1_COLOR_F m_cColor;
        bool m_bUsingGradient = false;
    };
    struct SDrawFilledCircle : public SCommand
    {
        CVector m_vPoint;
        float m_fRadius;
        D2D1_COLOR_F m_cColor;
        bool m_bUsingGradient = false;
    };
    struct SDrawEllipse : public SCommand
    {
        CVector m_vPoint;
        float m_fRadius1, m_fRadius2;
        D2D1_COLOR_F m_cColor;
        bool m_bUsingGradient = false;
    };
    struct SDrawFilledEllipse : public SCommand
    {
        CVector m_vPoint;
        float m_fRadius1, m_fRadius2;
        D2D1_COLOR_F m_cColor;
        bool m_bUsingGradient = false;
    };
    struct SDrawRect : public SCommand
    {
        CVector m_vPoint, m_vSize;
        D2D1_COLOR_F m_cColor;
        bool m_bCentered;
        bool m_bUsingGradient = false;
    };
    struct SDrawSizeRect : public SCommand
    {
        CVector m_vPoint1, m_vPoint2;
        float m_fFactor;
        D2D1_COLOR_F m_cColor;
        bool m_bUsingGradient = false;
    };
    struct SDrawFilledSizeRect : public SCommand
    {
        CVector m_vPoint1, m_vPoint2;
        float m_fFactor;
        D2D1_COLOR_F m_cColor;
        bool m_bUsingGradient = false;
    };
    struct SDrawFilledRect : public SCommand
    {
        CVector m_vPoint, m_vSize;
        D2D1_COLOR_F m_cColor;
        bool m_bCentered;
        bool m_bUsingGradient = false;
    };
    struct SDrawRoundedRect : public SCommand
    {
        CVector m_vPoint, m_vSize;
        D2D1_COLOR_F m_cColor;
        bool m_bCentered;
        bool m_bUsingGradient = false;
    };
    struct SDrawFilledRoundedRect : public SCommand
    {
        CVector m_vPoint, m_vSize;
        D2D1_COLOR_F m_cColor;
        bool m_bCentered;
        bool m_bUsingGradient = false;
    };
    struct SDrawRhombus : public SCommand
    {
        CVector m_vPoint1, m_vPoint2;
        D2D1_COLOR_F m_cColor;
        bool m_bUsingGradient = false;
    };
    struct SRenderText : public SCommand
    {
        IDWriteTextLayout *m_pTextLayout;
        CVector m_vPoint, m_vBounds, m_vOffset;
        bool m_bShadow;
        D2D1_COLOR_F m_cText;
        bool m_bUsingGradient = false;
    };
    struct SCreateTextLayout : public SCommand
    {
        IDWriteTextLayout *m_pTextLayout;
        wchar_t m_pContent[4088];
    };
    struct SGetCachedTextLayout : public SCommand
    {
        IDWriteTextLayout *m_pTextLayout;
        wchar_t m_pContent[4088];
    };
#pragma endregion

#pragma region CText
    struct SReleaseTextLayout : public SCommand
    {
        IDWriteTextLayout *m_pTextLayout;
    };
    struct SGetFontSize : public SCommand
    {
        IDWriteTextLayout *m_pTextLayout;
        float m_fFontSize;
    };
    struct SSetFontSize : public SCommand
    {
        IDWriteTextLayout *m_pTextLayout;
        float m_fFontSize;
        DWRITE_TEXT_RANGE m_dTextRange;
    };
    struct SGetTextMetrics : public SCommand
    {
        IDWriteTextLayout *m_pTextLayout;
        DWRITE_TEXT_METRICS m_dTextMetrics;
    };
    struct SSetMaxWidth : public SCommand
    {
        IDWriteTextLayout *m_pTextLayout;
        float m_fMaxWidth;
    };
    struct SSetMaxHeight : public SCommand
    {
        IDWriteTextLayout *m_pTextLayout;
        float m_fMaxHeight;
    };
    struct SGetTextAlignment : public SCommand
    {
        IDWriteTextLayout *m_pTextLayout;
        DWRITE_TEXT_ALIGNMENT m_dTextAlignment;
    };
    struct SSetTextAlignment : public SCommand
    {
        IDWriteTextLayout *m_pTextLayout;
        DWRITE_TEXT_ALIGNMENT m_dTextAlignment;
    };
#pragma endregion
}
#pragma pack(pop)