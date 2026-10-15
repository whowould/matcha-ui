#include "tabs.h"
#include "widgets.h"

namespace
{
    constexpr ImVec4 white{ 1, 1, 1, 1 };

    constexpr const char* hit_parts[] = { "Head", "Torso", "Closest" };
    constexpr const char* aim_types[] = { "Mouse", "Camera" };
    constexpr const char* rage_types[] = { "Camera Teleport", "Camera Lock" };
    constexpr const char* silent_methods[] = { "Experimental", "Raycast" };
    constexpr const char* easings[] = { "Linear", "Quad", "Sine" };
    constexpr const char* sizing[] = { "Bounding", "Dynamic" };
    constexpr const char* box_types[] = { "2D", "Corner", "3D" };
    constexpr const char* name_types[] = { "Name", "Display Name" };
    constexpr const char* text_positions[] = { "Above Name", "Below Name", "Left", "Right" };
    constexpr const char* chams_modes[] = { "Default", "Highlight" };
    constexpr const char* rendering[] = { "Static", "Pulse" };
    constexpr const char* origins[] = { "Top", "Bottom", "Mouse" };
    constexpr const char* hitbox_types[] = { "Old", "New" };
    constexpr const char* target_methods[] = { "Closest To Mouse", "Closest To Player" };
    constexpr const char* move_methods[] = { "Velocity", "CFrame" };
    constexpr const char* timer_methods[] = { "Steps", "Heartbeat" };
    constexpr const char* presets[] = { "Midnight", "Graphite", "Rose" };
    constexpr const char* usages[] = { "Low", "Mid", "High" };
    constexpr const char* styles[] = { "Classic", "Compact" };
    constexpr const char* watermarks[] = { "Single Row", "Double Row" };
}

void tabs::combat()
{
    static int aim_tab, silent_tab;
    static struct
    {
        bool enabled, team, visible, health, sticky, rage, predict, smooth = true, fov, fov_filled;
        ImGuiKey key = ImGuiKey_MouseRight;
        float distance = 500, sensitivity = 0.4f, x = 0.12f, y = 0.1f, smoothness = 4, radius = 120;
        int part, type, method, easing;
        ImVec4 fov_color = white;
    } aim;
    static struct
    {
        bool enabled, team, visible, health, sticky, predict, fov;
        ImGuiKey key = ImGuiKey_E;
        float distance = 500, amount = 0.13f, radius = 150;
        int part, method;
        ImVec4 fov_color = white;
    } silent;
    static struct
    {
        bool enabled, visible, team;
        ImGuiKey key = ImGuiKey_Q;
        float hitbox = 1;
        int delay = 1, release = 10;
    } trigger;
    static bool resolver;

    constexpr const char* aim_tabs[] = { "Aimbot", "Prediction", "Smoothness", "FOV" };
    constexpr const char* silent_tabs[] = { "Silent Aim", "Prediction", "FOV" };

    ui::column(0);
    ui::begin_panel("aimbot", aim_tabs, aim_tab);
    if (aim_tab == 0)
    {
        ui::checkbox("Enabled", aim.enabled);
        ui::keybind(aim.key);
        ui::checkbox("Team Check", aim.team);
        ui::checkbox("Visible Check", aim.visible);
        ui::checkbox("Health Check", aim.health);
        ui::checkbox("Sticky Aim", aim.sticky);
        ui::slider("Distance", aim.distance, 0, 10000);
        ui::slider("Sensitivity", aim.sensitivity, 0, 2.5f, "%.2f");
        ui::combo("Hit Part", aim.part, hit_parts);
        ui::combo("Aim Type", aim.type, aim_types);
        ui::checkbox("Rage Method", aim.rage);
        ui::help("Moves the camera directly instead of the mouse");
        ui::combo("Type", aim.method, rage_types);
    }
    else if (aim_tab == 1)
    {
        ui::checkbox("Enabled", aim.predict);
        ui::slider("Horizontal", aim.x, 0, 1, "%.2f");
        ui::slider("Vertical", aim.y, 0, 1, "%.2f");
    }
    else if (aim_tab == 2)
    {
        ui::checkbox("Enabled", aim.smooth);
        ui::slider("Smoothness", aim.smoothness, 1, 20, "%.1f");
        ui::combo("Easing", aim.easing, easings);
    }
    else
    {
        ui::checkbox("Show FOV", aim.fov);
        ui::color(aim.fov_color);
        ui::checkbox("Filled", aim.fov_filled);
        ui::slider("Radius", aim.radius, 10, 800);
    }
    ui::end_panel();

    ui::begin_panel("Misc");
    ui::checkbox("Resolver", resolver);
    ui::end_panel();

    ui::column(1);
    ui::begin_panel("silent", silent_tabs, silent_tab);
    if (silent_tab == 0)
    {
        ui::checkbox("Enabled", silent.enabled);
        ui::keybind(silent.key);
        ui::checkbox("Team Check", silent.team);
        ui::checkbox("Visible Check", silent.visible);
        ui::checkbox("Health Check", silent.health);
        ui::checkbox("Sticky Aim", silent.sticky);
        ui::slider("Distance", silent.distance, 0, 10000);
        ui::combo("Hit Part", silent.part, hit_parts);
        ui::combo("Method", silent.method, silent_methods);
    }
    else if (silent_tab == 1)
    {
        ui::checkbox("Enabled", silent.predict);
        ui::slider("Amount", silent.amount, 0, 1, "%.2f");
    }
    else
    {
        ui::checkbox("Show FOV", silent.fov);
        ui::color(silent.fov_color);
        ui::slider("Radius", silent.radius, 10, 800);
    }
    ui::end_panel();

    ui::begin_panel("Trigger Bot");
    ui::checkbox("Enabled", trigger.enabled);
    ui::keybind(trigger.key);
    ui::checkbox("Visible Check", trigger.visible);
    ui::checkbox("Team Check", trigger.team);
    ui::slider("Hitbox Mul", trigger.hitbox, 1, 5, "%.2f");
    ui::slider("Delay (ms)", trigger.delay, 0, 500);
    ui::slider("Release (ms)", trigger.release, 0, 500);
    ui::end_panel();
}

void tabs::visuals()
{
    static int esp_tab, info_tab;
    static struct
    {
        bool enabled, team, visible, team_color, gradient = true, background, outline = true, glow, self;
        ImGuiKey key = ImGuiKey_None;
        ImVec4 visible_color{ 0.63f, 0.78f, 1, 1 }, hidden_color{ 1, 0, 0, 1 };
        int sizing;
        float distance = 3000;
    } esp;
    static struct
    {
        bool enabled, dot;
        ImVec4 color = white;
        float size = 10, gap = 4, thickness = 1;
    } crosshair;
    static struct
    {
        bool dead, bots, limit;
        int fps = 60;
    } misc;
    static struct
    {
        bool moving, jumping, sitting;
    } flags;
    static struct
    {
        bool enabled = true, filled;
        ImVec4 color = white, fill{ 1, 1, 1, 0.2f };
        int type;
    } box;
    static struct
    {
        bool enabled = true;
        ImVec4 color = white;
        int type;
    } name;
    static struct
    {
        bool distance, item, skeleton, head, glow, picture;
        ImVec4 distance_color = white, item_color = white, skeleton_color = white, head_color = white;
    } info;
    static struct
    {
        bool enabled;
        ImVec4 color = white;
        float radius = 200, size = 12;
    } arrows;
    static struct
    {
        bool enabled;
        float range = 250, size = 160;
    } radar;
    static struct
    {
        bool bar = true, based, text;
        ImVec4 color{ 0, 1, 0, 1 };
        int position;
    } health;
    static struct
    {
        bool enabled, filled = true;
        ImVec4 fill{ 0.6f, 0.6f, 0.6f, 0.5f }, outline = white;
        int mode, rendering;
    } chams;
    static struct
    {
        bool enabled = true;
        ImVec4 color = white;
        int origin;
    } tracer;

    constexpr const char* esp_tabs[] = { "ESP", "Crosshair", "Misc", "Flags" };
    constexpr const char* info_tabs[] = { "Indicators", "OOF Arrow", "Radar" };

    ui::column(0);
    ui::begin_panel("esp", esp_tabs, esp_tab);
    if (esp_tab == 0)
    {
        ui::checkbox("Enabled", esp.enabled);
        ui::keybind(esp.key);
        ui::checkbox("Team Check", esp.team);
        ui::checkbox("Visible Check", esp.visible);
        ui::color(esp.hidden_color);
        ui::color(esp.visible_color);
        ui::checkbox("Team Based Color", esp.team_color);
        ui::checkbox("Text Gradient", esp.gradient);
        ui::checkbox("Text Background", esp.background);
        ui::checkbox("Outline", esp.outline);
        ui::checkbox("Glow", esp.glow);
        ui::checkbox("Self ESP", esp.self);
        ui::combo("Sizing Type", esp.sizing, sizing);
        ui::slider("Render Distance", esp.distance, 0, 30000);
    }
    else if (esp_tab == 1)
    {
        ui::checkbox("Enabled", crosshair.enabled);
        ui::color(crosshair.color);
        ui::checkbox("Center Dot", crosshair.dot);
        ui::slider("Size", crosshair.size, 2, 40);
        ui::slider("Gap", crosshair.gap, 0, 20);
        ui::slider("Thickness", crosshair.thickness, 1, 6);
    }
    else if (esp_tab == 2)
    {
        ui::checkbox("Hide Dead", misc.dead);
        ui::checkbox("Show Bots", misc.bots);
        ui::checkbox("Limit Refresh Rate", misc.limit);
        ui::slider("Refresh Rate", misc.fps, 15, 240);
    }
    else
    {
        ui::checkbox("Moving", flags.moving);
        ui::checkbox("Jumping", flags.jumping);
        ui::checkbox("Sitting", flags.sitting);
    }
    ui::end_panel();

    ui::begin_panel("Box");
    ui::checkbox("Enabled", box.enabled);
    ui::color(box.fill);
    ui::color(box.color);
    ui::checkbox("Fill Box", box.filled);
    ui::combo("Box Type", box.type, box_types);
    ui::end_panel();

    ui::begin_panel("Name");
    ui::checkbox("Enabled", name.enabled);
    ui::color(name.color);
    ui::combo("Type", name.type, name_types);
    ui::end_panel();

    ui::column(1);
    ui::begin_panel("info", info_tabs, info_tab);
    if (info_tab == 0)
    {
        ui::checkbox("Distance", info.distance);
        ui::color(info.distance_color);
        ui::checkbox("Equipped Item", info.item);
        ui::color(info.item_color);
        ui::checkbox("Skeleton", info.skeleton);
        ui::color(info.skeleton_color);
        ui::checkbox("Head Dot", info.head);
        ui::color(info.head_color);
        ui::checkbox("Head Dot Glow", info.glow);
        ui::checkbox("Profile Picture", info.picture);
    }
    else if (info_tab == 1)
    {
        ui::checkbox("Enabled", arrows.enabled);
        ui::color(arrows.color);
        ui::slider("Radius", arrows.radius, 50, 500);
        ui::slider("Size", arrows.size, 4, 32);
    }
    else
    {
        ui::checkbox("Enabled", radar.enabled);
        ui::slider("Range", radar.range, 50, 1000);
        ui::slider("Size", radar.size, 80, 400);
    }
    ui::end_panel();

    ui::begin_panel("Health");
    ui::checkbox("Health Bar", health.bar);
    ui::color(health.color);
    ui::checkbox("Health Based", health.based);
    ui::checkbox("Health Text", health.text);
    ui::combo("Text Pos", health.position, text_positions);
    ui::end_panel();

    ui::begin_panel("Chams");
    ui::combo("Mode", chams.mode, chams_modes);
    ui::checkbox("Enabled", chams.enabled);
    ui::color(chams.outline);
    ui::color(chams.fill);
    ui::checkbox("Filled", chams.filled);
    ui::combo("Rendering Type", chams.rendering, rendering);
    ui::end_panel();

    ui::begin_panel("Tracer");
    ui::checkbox("Enabled", tracer.enabled);
    ui::color(tracer.color);
    ui::combo("Origin", tracer.origin, origins);
    ui::end_panel();
}

void tabs::world()
{
    static struct
    {
        bool fov;
        float amount = 70;
    } camera;
    static struct
    {
        bool enabled, teleport, lock;
        ImGuiKey key = ImGuiKey_L, teleport_key = ImGuiKey_N;
        float speed = 1.5f;
    } freecam;
    static struct
    {
        char name[32];
        bool tween, visualize;
        float speed = 200;
    } waypoint;
    static struct
    {
        bool ambience, fog, glow, exposure, brightness, time;
        ImVec4 ambience_color = white, fog_color = white;
        float distance = 0, compensation = 2, level = 2, clock = 12;
    } lighting;

    ui::column(0);
    ui::begin_panel("Camera Modification");
    ui::checkbox("Camera Field Of View", camera.fov);
    ui::help("Overrides the camera's field of view");
    ui::slider("Amount", camera.amount, 1, 120);
    ui::end_panel();

    ui::begin_panel("Freecam");
    ui::checkbox("Enabled", freecam.enabled, col::green);
    ui::keybind(freecam.key);
    ui::checkbox("Teleport on click", freecam.teleport);
    ui::keybind(freecam.teleport_key);
    ui::checkbox("Lock Character Position", freecam.lock);
    ui::slider("Speed", freecam.speed, 0, 10, "%.2f");
    ui::end_panel();

    ui::begin_panel("Waypoint");
    ui::label("Name");
    ui::input("##waypoint", waypoint.name, sizeof(waypoint.name));
    ui::button("Create", 0, 24);
    ui::label("No Saved Waypoints", col::error);
    ui::button("Remove Waypoint", 0, 24);
    ui::button("Goto", 0, 24);
    ui::checkbox("Tween goto", waypoint.tween);
    ui::slider("Tween speed", waypoint.speed, 0, 1000, "%.2f");
    ui::checkbox("Visualize waypoint", waypoint.visualize);
    ui::end_panel();

    ui::column(1);
    ui::begin_panel("World Lighting");
    ui::checkbox("Ambience", lighting.ambience, col::yellow);
    ui::color(lighting.ambience_color);
    ui::checkbox("Custom Fog", lighting.fog, col::red);
    ui::color(lighting.fog_color);
    ui::checkbox("Glow", lighting.glow);
    ui::slider("Distance", lighting.distance, 0, 1000);
    ui::checkbox("Custom Exposure", lighting.exposure, col::green);
    ui::slider("Exposure Compensation", lighting.compensation, -3, 3, "%.2f");
    ui::checkbox("Custom Brightness", lighting.brightness, col::yellow);
    ui::slider("Brightness", lighting.level, 0, 10, "%.2f");
    ui::checkbox("Custom Time", lighting.time, col::green);
    ui::slider("Clock Time", lighting.clock, 0, 24, "%.2f");
    ui::end_panel();
}

void tabs::character()
{
    static struct
    {
        bool enabled, visualize, team, health;
        ImGuiKey key = ImGuiKey_J;
        int size = 20, type;
    } hitbox;
    static struct
    {
        bool enabled, circle;
        ImGuiKey key = ImGuiKey_X;
        ImVec4 color = white;
        int method;
        float radius = 10, speed = 5;
    } hover;
    static struct
    {
        bool enabled, walk, server, reset, tick;
        ImGuiKey key = ImGuiKey_MouseX1;
        int interval = 10;
    } desync;
    static struct
    {
        bool noclip, jump, click, speed, flight, power, floating;
        ImGuiKey noclip_key = ImGuiKey_Z, click_key = ImGuiKey_LeftCtrl, speed_key = ImGuiKey_V, flight_key = ImGuiKey_F;
        int speed_method, fly_method, speed_amount = 1, fly_amount = 1, jump_power = 50, height = 10;
    } movement;
    static struct
    {
        bool timer;
        ImGuiKey key = ImGuiKey_Y;
        int method;
        float multiply = 1;
    } exploit;

    ui::column(0);
    ui::begin_panel("Hitbox Extender");
    ui::checkbox("Enabled", hitbox.enabled, col::red);
    ui::keybind(hitbox.key);
    ui::checkbox("Visualize Hitbox", hitbox.visualize);
    ui::checkbox("Team Check", hitbox.team);
    ui::checkbox("Health Check", hitbox.health);
    ui::slider("Hitbox Size", hitbox.size, 1, 500);
    ui::combo("Type", hitbox.type, hitbox_types);
    ui::end_panel();

    ui::begin_panel("Target Hovering");
    ui::checkbox("Enabled", hover.enabled);
    ui::keybind(hover.key);
    ui::checkbox("Display Circle", hover.circle);
    ui::color(hover.color);
    ui::combo("Target Method", hover.method, target_methods);
    ui::slider("Radius", hover.radius, 0, 100, "%.2f");
    ui::slider("Speed", hover.speed, 0, 250, "%.2f");
    ui::end_panel();

    ui::begin_panel("Desync");
    ui::checkbox("Enabled", desync.enabled);
    ui::keybind(desync.key);
    ui::checkbox("Remove Walk Animation", desync.walk);
    ui::checkbox("Display Server Position", desync.server);
    ui::checkbox("Reset on Desync Off", desync.reset);
    ui::checkbox("Use Tick", desync.tick);
    ui::slider("Desync Interval", desync.interval, 1, 150);
    ui::end_panel();

    ui::column(1);
    ui::begin_panel("Movement");
    ui::checkbox("No-Clip", movement.noclip);
    ui::keybind(movement.noclip_key);
    ui::checkbox("Inf Jump", movement.jump);
    ui::help("Lets you jump again while in the air");
    ui::checkbox("Click TP", movement.click);
    ui::keybind(movement.click_key);
    ui::checkbox("Speed", movement.speed);
    ui::keybind(movement.speed_key);
    ui::combo("Speed Method", movement.speed_method, move_methods);
    ui::slider("Speed Amount", movement.speed_amount, 1, 100);
    ui::checkbox("Flight", movement.flight);
    ui::keybind(movement.flight_key);
    ui::combo("Fly Method", movement.fly_method, move_methods);
    ui::slider("Fly Amount", movement.fly_amount, 1, 100);
    ui::checkbox("Jump", movement.power, col::yellow);
    ui::slider("Jump Power", movement.jump_power, 0, 1000);
    ui::checkbox("Float", movement.floating, col::yellow);
    ui::slider("Height", movement.height, 0, 18);
    ui::end_panel();

    ui::begin_panel("Exploit");
    ui::checkbox("Timer Manipulation", exploit.timer);
    ui::keybind(exploit.key);
    ui::combo("Method", exploit.method, timer_methods);
    ui::slider("Multiply", exploit.multiply, 1, 10, "%.2f");
    ui::end_panel();
}

void tabs::options()
{
    static struct
    {
        ImGuiKey key = ImGuiKey_Insert;
        ImVec4 accent{ 0.71f, 0.38f, 0.38f, 1 };
        int preset;
    } theme;
    static struct
    {
        bool streamproof, vsync, rescan = true, input, set_fps, raycast, unsafe = true, hybrid = true;
        int fps = 240, raycast_rate = 2500, usage = 1;
    } general;
    static struct
    {
        int style;
        bool dark = true, snow = true, blur = true, glass = true;
    } look;
    static struct
    {
        bool eject;
        ImGuiKey key = ImGuiKey_F10;
    } session;
    static struct
    {
        bool watermark, explorer, keybinds = true, sound = true, music, keystroke, lag, arraylist;
        int style, rate = 500;
    } elements;

    ui::column(0);
    ui::begin_panel("Theme");
    ui::row("Menu Key");
    ui::keybind(theme.key);
    ui::row("Accent Color");
    ui::color(theme.accent);
    ui::combo("Preset", theme.preset, presets);
    ui::end_panel();

    ui::begin_panel("General");
    ui::checkbox("Streamproof", general.streamproof);
    ui::help("Hides the menu from screen capture");
    ui::checkbox("Vertical Sync", general.vsync);
    ui::help("Caps the menu to your refresh rate");
    ui::checkbox("Auto Rescan Game", general.rescan);
    ui::help("Rescans automatically after teleporting");
    ui::checkbox("InGame Input Detection", general.input);
    ui::checkbox("Set Roblox FPS", general.set_fps);
    ui::slider("FPS", general.fps, 0, 2000);
    ui::checkbox("Raycast Enable", general.raycast);
    ui::help("Uses raycasts for visibility checks");
    ui::slider("Raycast update (ms)", general.raycast_rate, 0, 10000);
    ui::checkbox("Allow Unsafe LuaU", general.unsafe);
    ui::help("Allows scripts to use unsafe functions");
    ui::checkbox("Hybrid Mode", general.hybrid, col::yellow);
    ui::help("Splits work between threads");
    ui::combo("Usage", general.usage, usages);
    ui::end_panel();

    ui::begin_panel("User Interface");
    ui::combo("Style", look.style, styles);
    ui::checkbox("Dark Background", look.dark);
    ui::checkbox("Snow Effect", look.snow);
    ui::checkbox("Blur", look.blur);
    ui::checkbox("Glass", look.glass);
    ui::end_panel();

    ui::column(1);
    ui::begin_panel("Session");
    ui::center("user", col::bright);
    ui::badge("Pro");
    ui::spacing(4);
    ui::row("Game ID", col::dim);
    if (ui::chip("Copy##game"))
        ImGui::SetClipboardText("1234567890");
    ui::value("1234567890");
    ui::row("Place ID", col::dim);
    if (ui::chip("Copy##place"))
        ImGui::SetClipboardText("9876543210123");
    ui::value("9876543210123", col::accent);
    ui::spacing(4);
    ui::buttons({ "Rejoin", "Server Hop" });
    ui::button("Rescan");
    ui::button("Eject");
    ui::checkbox("Eject Bind", session.eject);
    ui::keybind(session.key);
    ui::end_panel();

    ui::begin_panel("UI Elements");
    ui::checkbox("Watermark", elements.watermark);
    ui::combo("Watermark Style", elements.style, watermarks);
    ui::checkbox("View Explorer", elements.explorer);
    ui::help("Shows a live explorer window");
    ui::slider("Explorer update (ms)", elements.rate, 0, 10000);
    ui::checkbox("Keybind List", elements.keybinds);
    ui::help("Lists active keybinds on screen");
    ui::checkbox("Notification Sound", elements.sound);
    ui::help("Plays a sound with notifications");
    ui::checkbox("Music Overlay", elements.music);
    ui::help("Shows what's currently playing");
    ui::checkbox("Keystroke", elements.keystroke);
    ui::checkbox("Lag Notifier", elements.lag);
    ui::checkbox("Array List", elements.arraylist);
    ui::end_panel();
}

void tabs::configs()
{
    static int script = -1, config = -1;
    static char name[32];
    constexpr const char* scripts[] = { "untitled" };
    constexpr const char* saved[] = { "default", "legit" };

    ui::column(0);
    ui::begin_panel("Scripts", 838);
    ui::button("Open Folder##scripts");
    ui::label("Script List");
    ui::list("##scripts", scripts, script, 290);
    ui::button("Load Script");
    ui::button("Set Auto Execute");
    ui::end_panel();

    ui::column(1);
    ui::begin_panel("Config", 838);
    ui::button("Open Folder##configs");
    ui::label("Config List");
    ui::list("##configs", saved, config, 272);
    ui::label("Config Name");
    ui::input("##name", name, sizeof(name));
    ui::buttons({ "Load", "Save" });
    ui::end_panel();
}

void tabs::npc()
{
    static int tab, npc = -1, config = -1;
    static char name[32];
    constexpr const char* npc_tabs[] = { "General", "Visuals" };

    ui::column(0);
    ui::begin_panel("NPC List", ui::fill_height);
    ui::buttons({ "+ Model", "+ Directory" });
    ui::list("##npcs", {}, npc, -14);
    ui::end_panel();

    ui::column(1);
    ui::begin_panel("npc", npc_tabs, tab);
    ui::spacing(27);
    ui::center("Select an NPC to edit");
    ui::spacing(25);
    ui::end_panel(13);

    ui::begin_panel("Config##npc");
    ui::input("##npc_config", name, sizeof(name));
    ui::buttons({ "Load", "Save", "Folder" });
    ui::list("##npc_configs", {}, config, 95);
    ui::end_panel();
}

void tabs::teams()
{
    static char search[32];
    static bool custom, detect;

    ui::column(0);
    ui::begin_panel("Teams [0]", ui::fill_height);
    ui::input("##search", search, sizeof(search), "Search team...");
    ui::begin_box("##teams", -14);
    ui::checkbox("Use Custom Teams", custom);
    ui::checkbox("Auto Detect Allied Team", detect);
    ui::label("Team", col::dim);
    ui::value("Status", col::dim);
    ui::separator();
    ui::spacing(14);
    ui::center("No teams in this game");
    ui::end_box();
    ui::end_panel();

    ui::column(1);
    ui::begin_panel("Editor");
    ui::spacing(26);
    ui::center("Select a team to edit");
    ui::spacing(26);
    ui::end_panel(13);

    ui::begin_panel("Quick Actions");
    ui::buttons({ "All Enemy", "All Ally" });
    ui::button("Reset to Default");
    ui::end_panel();
}
