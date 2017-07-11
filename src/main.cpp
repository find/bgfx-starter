#define ENTRY_CONFIG_IMPLEMENT_MAIN 1

#include "bx/uint32_t.h"
#include "entry/entry.h"
#include "entry/input.h"

#include "bgfx/bgfx.h"
#include "bgfx_utils.h"
#include <imgui/imgui.h>

class ExampleHelloWorld : public entry::AppI
{
public:
  ExampleHelloWorld(char const* name, char const* description):
    AppI(name, description)
  {
  }

	void init(int _argc, char const * const * _argv, uint32_t w, uint32_t h) BX_OVERRIDE
	{
		Args args(_argc, _argv);

		m_width = w;
		m_height = h;

		bgfx::init(args.m_type, args.m_pciId);
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
	}

	virtual int shutdown() BX_OVERRIDE
	{
		imguiDestroy();

		// Shutdown bgfx.
		bgfx::shutdown();

		return 0;
	}

	bool update() BX_OVERRIDE
	{
		if (!entry::processEvents(m_width, m_height, m_debug, m_reset, &m_mouseState)) {
			uint8_t const* input = inputGetChar();
			imguiBeginFrame(m_mouseState.m_mx, m_mouseState.m_my,
				  (m_mouseState.m_buttons[entry::MouseButton::Left] ? IMGUI_MBUT_LEFT : 0)
				| (m_mouseState.m_buttons[entry::MouseButton::Right] ? IMGUI_MBUT_RIGHT : 0)
				| (m_mouseState.m_buttons[entry::MouseButton::Middle] ? IMGUI_MBUT_MIDDLE : 0), 
				m_mouseState.m_mz, m_width, m_height, input?*input:0);

			// Set view 0 default viewport.
			bgfx::setViewRect(0, 0, 0, uint16_t(m_width), uint16_t(m_height));

			// This dummy draw call is here to make sure that view 0 is cleared
			// if no other draw calls are submitted to view 0.
			bgfx::touch(0);

			// Use debug font to print information about this example.
			bgfx::dbgTextClear();
			bgfx::dbgTextPrintf(0, 1, 0x4f, "helloworld");
			bgfx::dbgTextPrintf(0, 2, 0x6f, "Initialization and setup imgui.");

			ImGui::SetNextWindowSizeConstraints(ImVec2(200, 100), ImVec2(m_width, m_height));
			ImGui::Begin("ImGui");
			ImGui::Text("Hello!");
			ImGui::DragFloat("sample var", &m_sampleVar, 0.1f, 0.0f, 100.0f);
      if (ImGui::Button("Toogle Debug")) {
        m_debug ^= BGFX_DEBUG_STATS;
        bgfx::setDebug(m_debug);
      }
			ImGui::End();

			// IMGUI_endFrame();
			imguiEndFrame();

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
	uint32_t m_reset = 0;
	int m_whateverscoll = BGFX_RESET_VSYNC;
};

ENTRY_IMPLEMENT_MAIN(ExampleHelloWorld, "hello world", "just say hello to the world");