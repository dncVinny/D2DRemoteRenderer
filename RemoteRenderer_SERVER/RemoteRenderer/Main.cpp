#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Overlay.h"

#include "IPC.h"
#include "Commands.h"

void Worker(HMODULE hModule)
{
    COverlay *pOverlay = COverlay::Get();

    IPC iProcComm("REMOTE_RENDERER");

    iProcComm.BindCallback([&]() {
        SCommand *pCommand = iProcComm.GetMemory().Get<SCommand>();

        switch (pCommand->m_uCode)
        {
            // COverlay
            case ECOMMAND_CODE_BEGINDRAW:
            {
                pOverlay->BeginDraw();
            }
            break;
            case ECOMMAND_CODE_ENDDRAW:
            {
                pOverlay->EndDraw();
            }
            break;
            case ECOMMAND_CODE_CLEAR:
            {
                pOverlay->Clear();
            }
            break;
            case ECOMMAND_CODE_GETSCREENSIZE:
            {
                command::SGetScreenSize cResponse {
                    ECOMMAND_CODE_GETSCREENSIZE,
                    pOverlay->GetScreenSize()
                };

                memcpy(pCommand, &cResponse, sizeof(cResponse));
            }
            break;
            case ECOMMAND_CODE_GETSCREENCENTER:
            {
                command::SGetScreenCenter cResponse{
                    ECOMMAND_CODE_GETSCREENSIZE,
                    pOverlay->GetScreenCenter()
                };

                memcpy(pCommand, &cResponse, sizeof(cResponse));
            }
            break;
            case ECOMMAND_CODE_GETASPECTRATIO:
            {
                command::SGetAspectRatio cResponse{
                    ECOMMAND_CODE_GETASPECTRATIO,
                    pOverlay->GetAspectRatio()
                };

                memcpy(pCommand, &cResponse, sizeof(cResponse));
            }
            break;
            case ECOMMAND_CODE_SETANTIALIAS:
            {
                command::SSetAntiAlias *pRequest = (command::SSetAntiAlias*)pCommand;
                pOverlay->SetAntialias(pRequest->m_bEnabled);
            }
            break;
            case ECOMMAND_CODE_GETANTIALIAS:
            {
                command::SGetAntiAlias cResponse{
                    ECOMMAND_CODE_GETANTIALIAS,
                    pOverlay->GetAntialias()
                };

                memcpy(pCommand, &cResponse, sizeof(cResponse));
            }
            break;
            case ECOMMAND_CODE_SETTHICKNESS:
            {
                command::SSetThickness *pRequest = (command::SSetThickness*)pCommand;
                pOverlay->SetThickness(pRequest->m_fThickness);
            }
            break;
            case ECOMMAND_CODE_SETROUNDEDRADIUS:
            {
                command::SSetRoundedRadius *pRequest = (command::SSetRoundedRadius*)pCommand;
                pOverlay->SetRoundedRadius(pRequest->m_fRadius);
            }
            break;
            case ECOMMAND_CODE_UPDATEGRADIENTBRUSH:
            {
                command::SUpdateGradientBrush *pRequest = (command::SUpdateGradientBrush*)pCommand;
                
                pOverlay->UpdateGradientBrush({
                    pRequest->m_cColor1,
                    pRequest->m_cColor2,
                    pRequest->m_vStart,
                    pRequest->m_vEnd
                });
            }
            break;
            case ECOMMAND_CODE_UPDATEGRADIENTBRUSHVECTOR:
            {
                command::SUpdateGradientBrushVector *pRequest = (command::SUpdateGradientBrushVector*)pCommand;
                pOverlay->UpdateGradientBrush(pRequest->m_vStart, pRequest->m_vEnd);
            }
            break;
            case ECOMMAND_CODE_UPDATECOLORBRUSH:
            {
                command::SUpdateColorBrush *pRequest = (command::SUpdateColorBrush*)pCommand;
                pOverlay->UpdateColorBrush(pRequest->m_cColor);
            }
            break;
            case ECOMMAND_CODE_PUSHCLIPRENDER:
            {
                command::SPushClipRender *pRequest = (command::SPushClipRender*)pCommand;
                pOverlay->PushClipRender(pRequest->m_vPosition, pRequest->m_vSize);
            }
            break;
            case ECOMMAND_CODE_POPCLIPRENDER:
            {
                command::SPopClipRender *pRequest = (command::SPopClipRender*)pCommand;
                pOverlay->PopClipRender();
            }
            break;
            case ECOMMAND_CODE_DRAWLINE:
            {
                command::SDrawLine *pRequest = (command::SDrawLine*)pCommand;

                if (pRequest->m_bUsingGradient)
                    pOverlay->DrawLine(pRequest->m_vPoint1, pRequest->m_vPoint2);
                else
                    pOverlay->DrawLine(pRequest->m_vPoint1, pRequest->m_vPoint2, pRequest->m_cColor);
            }
            break;
            case ECOMMAND_CODE_DRAWCROSSHAIR:
            {
                command::SDrawCrosshair *pRequest = (command::SDrawCrosshair*)pCommand;
                pOverlay->DrawCrosshair(pRequest->m_vPoint, pRequest->m_fRadius, pRequest->m_cColor);
            }
            break;
            case ECOMMAND_CODE_DRAWCIRCLE:
            {
                command::SDrawCircle *pRequest = (command::SDrawCircle*)pCommand;

                if (pRequest->m_bUsingGradient)
                    pOverlay->DrawCircle(pRequest->m_vPoint, pRequest->m_fRadius);
                else
                    pOverlay->DrawCircle(pRequest->m_vPoint, pRequest->m_fRadius, pRequest->m_cColor);
            }
            break;
            case ECOMMAND_CODE_DRAWFILLEDCIRCLE:
            {
                command::SDrawFilledCircle *pRequest = (command::SDrawFilledCircle*)pCommand;

                if (pRequest->m_bUsingGradient)
                    pOverlay->DrawFilledCircle(pRequest->m_vPoint, pRequest->m_fRadius);
                else
                    pOverlay->DrawFilledCircle(pRequest->m_vPoint, pRequest->m_fRadius, pRequest->m_cColor);
            }
            break;
            case ECOMMAND_CODE_DRAWELLIPSE:
            {
                command::SDrawEllipse *pRequest = (command::SDrawEllipse*)pCommand;

                if (pRequest->m_bUsingGradient)
                    pOverlay->DrawEllipse(pRequest->m_vPoint, pRequest->m_fRadius1, pRequest->m_fRadius2);
                else
                    pOverlay->DrawEllipse(pRequest->m_vPoint, pRequest->m_fRadius1, pRequest->m_fRadius2, pRequest->m_cColor);
            }
            break;
            case ECOMMAND_CODE_DRAWFILLEDELLIPSE:
            {
                command::SDrawFilledEllipse *pRequest = (command::SDrawFilledEllipse*)pCommand;

                if (pRequest->m_bUsingGradient)
                    pOverlay->DrawFilledEllipse(pRequest->m_vPoint, pRequest->m_fRadius1, pRequest->m_fRadius2);
                else
                    pOverlay->DrawFilledEllipse(pRequest->m_vPoint, pRequest->m_fRadius1, pRequest->m_fRadius2, pRequest->m_cColor);
            }
            break;
            case ECOMMAND_CODE_DRAWRECT:
            {
                command::SDrawRect *pRequest = (command::SDrawRect*)pCommand;

                if (pRequest->m_bUsingGradient)
                    pOverlay->DrawRect(pRequest->m_vPoint, pRequest->m_vSize, pRequest->m_bCentered);
                else
                    pOverlay->DrawRect(pRequest->m_vPoint, pRequest->m_vSize, pRequest->m_cColor, pRequest->m_bCentered);
            }
            break;
            case ECOMMAND_CODE_DRAWSIZERECT:
            {
                command::SDrawSizeRect *pRequest = (command::SDrawSizeRect*)pCommand;

                if (pRequest->m_bUsingGradient)
                    pOverlay->DrawSizeRect(pRequest->m_vPoint1, pRequest->m_vPoint2, pRequest->m_fFactor);
                else
                    pOverlay->DrawSizeRect(pRequest->m_vPoint1, pRequest->m_vPoint2, pRequest->m_fFactor, pRequest->m_cColor);
            }
            break;
            case ECOMMAND_CODE_DRAWFILLEDSIZERECT:
            {
                command::SDrawFilledSizeRect *pRequest = (command::SDrawFilledSizeRect*)pCommand;

                if (pRequest->m_bUsingGradient)
                    pOverlay->DrawFilledSizeRect(pRequest->m_vPoint1, pRequest->m_vPoint2, pRequest->m_fFactor);
                else
                    pOverlay->DrawFilledSizeRect(pRequest->m_vPoint1, pRequest->m_vPoint2, pRequest->m_fFactor, pRequest->m_cColor);
            }
            break;
            case ECOMMAND_CODE_DRAWFILLEDRECT:
            {
                command::SDrawFilledRect *pRequest = (command::SDrawFilledRect*)pCommand;

                if (pRequest->m_bUsingGradient)
                    pOverlay->DrawFilledRect(pRequest->m_vPoint, pRequest->m_vSize, pRequest->m_bCentered);
                else
                    pOverlay->DrawFilledRect(pRequest->m_vPoint, pRequest->m_vSize, pRequest->m_cColor, pRequest->m_bCentered);
            }
            break;
            case ECOMMAND_CODE_DRAWROUNDEDRECT:
            {
                command::SDrawRoundedRect *pRequest = (command::SDrawRoundedRect*)pCommand;

                if (pRequest->m_bUsingGradient)
                    pOverlay->DrawRoundedRect(pRequest->m_vPoint, pRequest->m_vSize, pRequest->m_bCentered);
                else
                    pOverlay->DrawRoundedRect(pRequest->m_vPoint, pRequest->m_vSize, pRequest->m_cColor, pRequest->m_bCentered);
            }
            break;
            case ECOMMAND_CODE_DRAWFILLEDROUNDEDRECT:
            {
                command::SDrawRoundedRect *pRequest = (command::SDrawRoundedRect*)pCommand;

                if (pRequest->m_bUsingGradient)
                    pOverlay->DrawFilledRoundedRect(pRequest->m_vPoint, pRequest->m_vSize, pRequest->m_bCentered);
                else
                    pOverlay->DrawFilledRoundedRect(pRequest->m_vPoint, pRequest->m_vSize, pRequest->m_cColor, pRequest->m_bCentered);
            }
            break;
            case ECOMMAND_CODE_DRAWRHOMBUS:
            {
                command::SDrawRhombus *pRequest = (command::SDrawRhombus*)pCommand;

                if (pRequest->m_bUsingGradient)
                    pOverlay->DrawRhombus(pRequest->m_vPoint1, pRequest->m_vPoint2);
                else
                    pOverlay->DrawRhombus(pRequest->m_vPoint1, pRequest->m_vPoint2, pRequest->m_cColor);
            }
            break;
            case ECOMMAND_CODE_RENDERTEXT:
            {
                command::SRenderText *pRequest = (command::SRenderText*)pCommand;

                SText tTemp = {
                    pRequest->m_pTextLayout,
                    pRequest->m_vBounds,
                    pRequest->m_vOffset,
                    pRequest->m_bShadow
                };

                if (pRequest->m_bUsingGradient)
                    pOverlay->RenderText(pRequest->m_vPoint, tTemp);
                else
                    pOverlay->RenderText(pRequest->m_vPoint, tTemp, pRequest->m_cText);
            }
            break;
            case ECOMMAND_CODE_CREATETEXTLAYOUT:
            {
                command::SCreateTextLayout *pRequest = (command::SCreateTextLayout*)pCommand;
                pRequest->m_pTextLayout = pOverlay->CreateTextLayout(pRequest->m_pContent);
            }
            break;
            case ECOMMAND_CODE_GETCACHEDTEXTLAYOUT:
            {
                command::SGetCachedTextLayout *pRequest = (command::SGetCachedTextLayout*)pCommand;
                pRequest->m_pTextLayout = pOverlay->GetCachedTextLayout(pRequest->m_pContent);
            }
            break;
            case ECOMMAND_CODE_RELEASETEXTLAYOUT:
            {
                command::SReleaseTextLayout *pRequest = (command::SReleaseTextLayout*)pCommand;

                if (pRequest->m_pTextLayout)
                    pRequest->m_pTextLayout->Release();
            }
            break;
            case ECOMMAND_CODE_GETFONTSIZE:
            {
                command::SGetFontSize *pRequest = (command::SGetFontSize*)pCommand;

                if (pRequest->m_pTextLayout)
                    pRequest->m_fFontSize = pRequest->m_pTextLayout->GetFontSize();
            }
            break;
            case ECOMMAND_CODE_SETFONTSIZE:
            {
                command::SSetFontSize *pRequest = (command::SSetFontSize*)pCommand;

                if (pRequest->m_pTextLayout)
                    pRequest->m_pTextLayout->SetFontSize(pRequest->m_fFontSize, pRequest->m_dTextRange);
            }
            break;
            case ECOMMAND_CODE_GETTEXTMETRICS:
            {
                command::SGetTextMetrics *pRequest = (command::SGetTextMetrics*)pCommand;

                if (pRequest->m_pTextLayout)
                    pRequest->m_pTextLayout->GetMetrics(&pRequest->m_dTextMetrics);
            }
            break;
            case ECOMMAND_CODE_SETMAXWIDTH:
            {
                command::SSetMaxWidth *pRequest = (command::SSetMaxWidth*)pCommand;

                if (pRequest->m_pTextLayout)
                    pRequest->m_pTextLayout->SetMaxWidth(pRequest->m_fMaxWidth);
            }
            break;
            case ECOMMAND_CODE_SETMAXHEIGHT:
            {
                command::SSetMaxHeight *pRequest = (command::SSetMaxHeight*)pCommand;

                if (pRequest->m_pTextLayout)
                    pRequest->m_pTextLayout->SetMaxHeight(pRequest->m_fMaxHeight);
            }
            break;
            case ECOMMAND_CODE_GETTEXTALIGNMENT:
            {
                command::SGetTextAlignment *pRequest = (command::SGetTextAlignment*)pCommand;

                if (pRequest->m_pTextLayout)
                    pRequest->m_dTextAlignment = pRequest->m_pTextLayout->GetTextAlignment();
            }
            break;
            case ECOMMAND_CODE_SETTEXTALIGNMENT:
            {
                command::SSetTextAlignment *pRequest = (command::SSetTextAlignment*)pCommand;

                if (pRequest->m_pTextLayout)
                    pRequest->m_pTextLayout->SetTextAlignment(pRequest->m_dTextAlignment);
            }
            break;
            default:
                iProcComm.Stop();
        }
    });

    iProcComm.Start();
    iProcComm.WaitForStop();

exit_code:
    COverlay::Release();

    FreeLibraryAndExitThread(hModule, 0);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dReason, LPVOID pReserved)
{
    if (dReason == DLL_PROCESS_ATTACH)
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)Worker, hModule, 0, nullptr);

    return true;
}