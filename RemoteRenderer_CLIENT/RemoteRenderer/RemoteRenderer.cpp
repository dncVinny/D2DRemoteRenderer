#include "Overlay.h"

#include <iostream>

int main()
{
    COverlay* overlay = COverlay::Get();

    while (!GetAsyncKeyState(VK_END))
    {
        overlay->BeginDraw();
        overlay->Clear();

        overlay->DrawCircle(overlay->GetScreenCenter(), 125.0f, COLORF_FROMRGBA(255, 0, 0, 1.0f));

        POINT cursor;
        GetCursorPos(&cursor);

        overlay->DrawLine(overlay->GetScreenCenter(), { (float)cursor.x, (float)cursor.y }, COLORF_FROMRGBA(255, 0, 0, 1.0f));

        overlay->EndDraw();
    }

	return 0;
}