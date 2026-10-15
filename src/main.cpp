#include <windows.h>
#include <dwmapi.h>
#include <d3d11.h>
#include <wrl/client.h>

#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include "gui.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

using Microsoft::WRL::ComPtr;

namespace
{
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;
    ComPtr<IDXGISwapChain> swap_chain;
    ComPtr<ID3D11RenderTargetView> target;

    void create_target()
    {
        ComPtr<ID3D11Texture2D> back_buffer;
        swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
        device->CreateRenderTargetView(back_buffer.Get(), nullptr, &target);
    }

    bool create_device(HWND hwnd)
    {
        DXGI_SWAP_CHAIN_DESC desc{};
        desc.BufferCount = 2;
        desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.OutputWindow = hwnd;
        desc.SampleDesc.Count = 1;
        desc.Windowed = TRUE;
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

        constexpr D3D_DRIVER_TYPE drivers[] = { D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP };
        for (auto driver : drivers)
        {
            if (SUCCEEDED(D3D11CreateDeviceAndSwapChain(nullptr, driver, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION,
                                                        &desc, &swap_chain, &device, nullptr, &context)))
            {
                create_target();
                return true;
            }
        }
        return false;
    }

    LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
    {
        if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
            return true;

        switch (msg)
        {
        case WM_SIZE:
            if (swap_chain && wparam != SIZE_MINIMIZED)
            {
                target.Reset();
                swap_chain->ResizeBuffers(0, LOWORD(lparam), HIWORD(lparam), DXGI_FORMAT_UNKNOWN, 0);
                create_target();
            }
            return 0;
        case WM_SYSCOMMAND:
            if ((wparam & 0xfff0) == SC_KEYMENU)
                return 0;
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }
        return DefWindowProcW(hwnd, msg, wparam, lparam);
    }

    void drag(HWND hwnd)
    {
        static bool dragging;
        static POINT grab;

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered()
            && !ImGui::IsPopupOpen("", ImGuiPopupFlags_AnyPopupId))
        {
            RECT rc;
            GetWindowRect(hwnd, &rc);
            GetCursorPos(&grab);
            grab.x -= rc.left;
            grab.y -= rc.top;
            dragging = true;
        }

        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
            dragging = false;

        if (dragging)
        {
            POINT cursor;
            GetCursorPos(&cursor);
            SetWindowPos(hwnd, nullptr, cursor.x - grab.x, cursor.y - grab.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        }
    }
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int)
{
    WNDCLASSEXW wc{ sizeof(wc) };
    wc.style = CS_CLASSDC;
    wc.lpfnWndProc = wnd_proc;
    wc.hInstance = instance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = L"menu";
    RegisterClassExW(&wc);

    ImVec2 size = gui::size();
    int w = static_cast<int>(size.x), h = static_cast<int>(size.y);

    RECT work;
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &work, 0);
    int x = work.left + (work.right - work.left - w) / 2;
    int y = work.top + (work.bottom - work.top - h) / 2;
    if (y < work.top)
        y = work.top;

    HWND hwnd = CreateWindowExW(0, wc.lpszClassName, L"menu", WS_POPUP | WS_MINIMIZEBOX, x, y, w, h,
                                nullptr, nullptr, instance, nullptr);

    DWM_WINDOW_CORNER_PREFERENCE corners = DWMWCP_ROUND;
    COLORREF border = RGB(57, 57, 63);
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &corners, sizeof(corners));
    DwmSetWindowAttribute(hwnd, DWMWA_BORDER_COLOR, &border, sizeof(border));

    if (!create_device(hwnd))
        return 1;

    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    ImGui::CreateContext();
    ImGui::GetIO().IniFilename = nullptr;
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(device.Get(), context.Get());
    gui::init();

    for (bool running = true; running;)
    {
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
            if (msg.message == WM_QUIT)
                running = false;
        }

        if (!running)
            break;

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        gui::render();
        drag(hwnd);

        ImGui::Render();
        constexpr float clear[4] = { 31 / 255.0f, 31 / 255.0f, 37 / 255.0f, 1.0f };
        context->OMSetRenderTargets(1, target.GetAddressOf(), nullptr);
        context->ClearRenderTargetView(target.Get(), clear);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        swap_chain->Present(1, 0);
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    DestroyWindow(hwnd);
    UnregisterClassW(wc.lpszClassName, instance);
    return 0;
}
