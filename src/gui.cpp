#include "gui.h"
#include "tabs.h"
#include "widgets.h"

#include <imgui_internal.h>
#include <windows.h>

#include <filesystem>
#include <iterator>
#include <string>

namespace
{
    constexpr const char* brand = "Example";
    constexpr const char* username = "user";

    struct page
    {
        const char* icon;
        const char* name;
        void (*draw)();
    };

    constexpr page pages[] = {
        { icon::combat, "Combat", tabs::combat },
        { icon::visuals, "Visuals", tabs::visuals },
        { icon::world, "World", tabs::world },
        { icon::character, "Character", tabs::character },
        { icon::options, "Options", tabs::options },
        { icon::configs, "Configs", tabs::configs },
        { icon::npc, "NPC", tabs::npc },
        { icon::teams, "Teams", tabs::teams },
    };

    int current;

    std::string font(const char* name)
    {
        wchar_t exe[MAX_PATH];
        GetModuleFileNameW(nullptr, exe, MAX_PATH);
        return (std::filesystem::path(exe).parent_path() / "assets" / "fonts" / name).string();
    }

    float width(const char* str)
    {
        return ImGui::CalcTextSize(str).x;
    }

    void text(ImDrawList* dl, ImVec2 pos, ImU32 color, const char* str)
    {
        dl->AddText(ImFloor(pos), color, str);
    }

    void title_bar(ImDrawList* dl, float w)
    {
        dl->AddRectFilledMultiColor({ 1, 1 }, { w - 1, 44 }, IM_COL32(44, 45, 50, 255), IM_COL32(44, 45, 50, 255),
                                    IM_COL32(39, 39, 45, 255), IM_COL32(39, 39, 45, 255));

        float x = 18;
        text(dl, { x, 15 }, col::accent, brand);
        x += width(brand);
        dl->AddCircleFilled({ x + 6, 22 }, 1.5f, IM_COL32(48, 48, 54, 255));
        x += 11;
        text(dl, { x, 15 }, col::text, "Interface");
        x += width("Interface") + 7;
        ui::draw_badge(dl, { x, 14 }, "Pro");

        text(dl, { w - 17 - width(username), 15 }, col::text, username);
    }

    void tab_bar(ImDrawList* dl)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        float x = 18;

        for (int i = 0; i < static_cast<int>(std::size(pages)); ++i)
        {
            const page& p = pages[i];
            float icon_w = width(p.icon);
            ImRect bb({ x, 52 }, { x + 6 + icon_w + 3 + width(p.name) + 6, 76 });
            ImGuiID id = window->GetID(p.name);
            ImGui::ItemAdd(bb, id);

            bool hovered, held;
            if (ImGui::ButtonBehavior(bb, id, &hovered, &held))
                current = i;

            bool active = current == i;
            if (active)
                dl->AddRectFilled(bb.Min, bb.Max, IM_COL32(54, 54, 59, 255), 12);

            ImU32 color = active ? col::accent : hovered ? col::bright : col::text;
            float y = bb.Min.y + (bb.GetHeight() - ImGui::GetFontSize()) * 0.5f;
            text(dl, { bb.Min.x + 6, y }, color, p.icon);
            text(dl, { bb.Min.x + 6 + icon_w + 3, y }, color, p.name);
            x = bb.Max.x + 9;
        }
    }

    void footer(ImDrawList* dl, ImVec2 size)
    {
        dl->AddRectFilled({ 1, size.y - 26 }, { size.x - 1, size.y - 1 }, IM_COL32(39, 38, 44, 255));

        float y = size.y - 20;
        text(dl, { 18, y }, col::accent, icon::users);
        text(dl, { 18 + width(icon::users) + 4, y }, col::dim, "1 online");
        text(dl, { (size.x - width("menu-ui")) * 0.5f, y }, col::dim, "menu-ui");

        const char* date = __DATE__;
        float x = size.x - 20 - width(date);
        text(dl, { x, y }, col::accent_dim, date);
        text(dl, { x - width("Build: "), y }, col::dim, "Build: ");
    }
}

void gui::init()
{
    static constexpr ImWchar private_use[] = { 0xe000, 0xf8ff, 0 };
    ImFontConfig inter;
    inter.GlyphExcludeRanges = private_use;
    inter.OversampleH = 1;
    inter.RasterizerMultiply = 1.45f;

    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF(font("InterDisplay-Medium.ttf").c_str(), 13, &inter);

    ImFontConfig icons;
    icons.MergeMode = true;
    icons.ExtraSizeScale = 0.83f;
    icons.GlyphOffset = { 0, 1 };
    io.Fonts->AddFontFromFileTTF(font("lucide.ttf").c_str(), 13, &icons);

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding = { 0, 0 };
    style.WindowBorderSize = 0;
    style.ItemSpacing = { 8, 8 };
    style.ChildRounding = 6;
    style.ChildBorderSize = 1;
    style.PopupRounding = 4;
    style.PopupBorderSize = 1;
    style.FrameRounding = 4;
    style.FrameBorderSize = 1;
    style.ScrollbarSize = 4;
    style.ScrollbarRounding = 2;

    ImVec4* c = style.Colors;
    c[ImGuiCol_Text] = ImColor(col::text);
    c[ImGuiCol_TextDisabled] = ImColor(col::dim);
    c[ImGuiCol_ChildBg] = c[ImGuiCol_PopupBg] = ImColor(col::panel);
    c[ImGuiCol_Border] = ImColor(col::outline);
    c[ImGuiCol_FrameBg] = c[ImGuiCol_FrameBgActive] = ImColor(col::control);
    c[ImGuiCol_FrameBgHovered] = ImColor(col::hovered);
    c[ImGuiCol_TextSelectedBg] = ImColor(181, 96, 96, 90);
    c[ImGuiCol_InputTextCursor] = ImColor(col::accent);
    c[ImGuiCol_ScrollbarBg] = ImVec4(0, 0, 0, 0);
    c[ImGuiCol_ScrollbarGrab] = c[ImGuiCol_ScrollbarGrabHovered] = c[ImGuiCol_ScrollbarGrabActive] = ImColor(col::check);
    c[ImGuiCol_NavCursor] = ImVec4(0, 0, 0, 0);
}

ImVec2 gui::size()
{
    return { 606, 970 };
}

void gui::render()
{
    ImVec2 size = gui::size();
    ImGui::SetNextWindowPos({ 0, 0 });
    ImGui::SetNextWindowSize(size);
    ImGui::Begin("##menu", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground
                                   | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus);

    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddRect({ 0, 0 }, size, col::border);
    title_bar(dl, size.x);
    tab_bar(dl);
    pages[current].draw();
    footer(dl, size);

    ImGui::End();
}
