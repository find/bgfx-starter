#define ENTRY_CONFIG_IMPLEMENT_MAIN 1

#include "bx/uint32_t.h"
#include "bx/timer.h"
#include "entry/entry.h"
#include "entry/input.h"

#include "bgfx/bgfx.h"
#include "bgfx_utils.h"
#include <camera.h>
#include <debugdraw/debugdraw.h>
#include <imgui/imgui.h>

class ExampleHelloWorld : public entry::AppI
{
public:
  ExampleHelloWorld(char const* name, char const* description):
    AppI(name, description)
  {
  }

  void init(int _argc, char const * const * _argv, uint32_t w, uint32_t h) override
  {
    Args args(_argc, _argv);

    m_width = w;
    m_height = h;
    m_lastTime = bx::getHPCounter();
    m_debug = BGFX_DEBUG_TEXT;
    m_reset = BGFX_RESET_VSYNC;

    bgfx::Init init;
    init.type = args.m_type;
    init.vendorId = args.m_pciId;
    init.resolution.width = m_width;
    init.resolution.height = m_height;
    init.resolution.reset = m_reset;

    bgfx::init(init);
    bgfx::reset(m_width, m_height, m_reset);

    // Enable debug text.
    bgfx::setDebug(m_debug);

    // Set view 0 clear state.
    bgfx::setViewClear(0
      , BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH
      , 0x3030a0ff
      , 1.0f
      , 0
    );

    imguiCreate();

    cameraCreate();
    cameraSetPosition({ 0.f, 2.f, -12.f });
    ddInit();
  }

  virtual int shutdown() override
  {
    ddShutdown();
    cameraDestroy();
    imguiDestroy();

    // Shutdown bgfx.
    bgfx::shutdown();

    return 0;
  }

  void updateUI()
  {
    uint8_t const* input = inputGetChar();
    imguiBeginFrame(m_mouseState.m_mx, m_mouseState.m_my,
      (m_mouseState.m_buttons[entry::MouseButton::Left] ? IMGUI_MBUT_LEFT : 0)
      | (m_mouseState.m_buttons[entry::MouseButton::Right] ? IMGUI_MBUT_RIGHT : 0)
      | (m_mouseState.m_buttons[entry::MouseButton::Middle] ? IMGUI_MBUT_MIDDLE : 0),
      m_mouseState.m_mz, m_width, m_height, input ? *input : 0);
    ImGui::SetNextWindowSizeConstraints(ImVec2(200, 100), ImVec2(m_width, m_height));
    ImGui::Begin("ImGui");
    ImGui::Text("Hello!");
    ImGui::SliderFloat("sample var", &m_sampleVar, 0.0f, 100.0f);
    if (ImGui::Button("Toogle Debug")) {
      m_debug ^= BGFX_DEBUG_STATS;
      bgfx::setDebug(m_debug);
    }
    if (ImGui::Button("Toogle Text")) {
      m_debug ^= BGFX_DEBUG_TEXT;
      bgfx::setDebug(m_debug);
    }
    if (ImGui::Button("Toogle Wireframe")) {
      m_debug ^= BGFX_DEBUG_WIREFRAME;
      bgfx::setDebug(m_debug);
    }
    ImGui::End();

	showExampleDialog(this);

    imguiEndFrame();

    bgfx::dbgTextClear();
    bgfx::dbgTextPrintf(0, 1, 0x4f, "helloworld");
    bgfx::dbgTextPrintf(0, 2, 0x6f, "Initialization and setup imgui.");
  }

  bool update() override
  {
    if (!entry::processEvents(m_width, m_height, m_debug, m_reset, &m_mouseState)) {
      updateUI();

      int64_t currentTime = bx::getHPCounter();
      float const dt = float(double(currentTime - m_lastTime) / double(bx::getHPFrequency()));
      m_lastTime = currentTime;

      cameraUpdate(dt, m_mouseState);

      float view[16] = { 0 }, proj[16] = { 0 };
      cameraGetViewMtx(view);

      bx::mtxProj(proj, 60.0f, float(m_width) / float(m_height), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);

      bgfx::setViewTransform(0, view, proj);
      bgfx::setViewRect(0, 0, 0, uint16_t(m_width), uint16_t(m_height));


      // Set view 0 default viewport.
      bgfx::setViewRect(0, 0, 0, uint16_t(m_width), uint16_t(m_height));

      // This dummy draw call is here to make sure that view 0 is cleared
      // if no other draw calls are submitted to view 0.
      bgfx::touch(0);

	  DebugDrawEncoder dde;
	  dde.begin(0);
      bx::Vec3 center = { 0,0,0 };
      dde.drawGrid(Axis::Y, center, 20, 0.1f);
      dde.push();
      dde.setColor(0xffaabbff);
      bx::Vec3 from = { 0,0,0 };
      bx::Vec3 to   = { 3,4,5 };
      dde.drawCylinder(from, to, 0.2f);
      dde.pop();
      dde.end();

      // Advance to next frame. Rendering thread will be kicked to
      // process submitted rendering primitives.
      bgfx::frame();

      return true;
    }

    return false;
  }
  
  entry::MouseState m_mouseState;
  float m_sampleVar = 0.0f;

  uint32_t m_width = 800;
  uint32_t m_height = 600;
  uint32_t m_debug = 0;
  uint32_t m_reset = BGFX_RESET_VSYNC | BGFX_RESET_MSAA_X4;
  int64_t  m_lastTime = 0;
};

ENTRY_IMPLEMENT_MAIN(ExampleHelloWorld, "hello world", "just say hello to the world");
