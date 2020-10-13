
#include "diag.h"
#include "engine.h"
#include "imgui/imgui.h"
#include <spdlog/spdlog.h>

void DiagnosticSystem::Tick()
{
	static TimePoint nextTime;
	nextTime = std::chrono::steady_clock::now();
	m_DeltaTime += (nextTime - m_LastFrameTime);
	m_LastFrameTime = nextTime;
	m_FrameCounter++;
}

void DiagnosticSystem::RenderGUI()
{
	ImGui::SetNextWindowSize(ImVec2(120, 60));
	ImGui::SetNextWindowPos(ImVec2(10, 30));
	ImGui::Begin("Diagnostics");
	ImGui::LabelText("", "FPS: %d", m_LastFrameCounter);
	ImGui::End();
}

void DiagnosticSystem::SpinUp()
{
	Engine::Dispatcher().sink<GUIRender>().connect<&DiagnosticSystem::RenderGUI>(this);
	Engine::Dispatcher().sink<Frame>().connect<&DiagnosticSystem::Tick>(this);
}

void DiagnosticSystem::Run()
{
	auto delta = m_DeltaTime.count();
	if (delta >= 1.0)
	{
		m_LastFrameCounter = m_FrameCounter;
		m_FrameCounter = 0;
		m_DeltaTime = Duration::zero();
	}
}

void DiagnosticSystem::WindDown()
{
	Engine::Dispatcher().sink<Frame>().disconnect<&DiagnosticSystem::Tick>(this);
	Engine::Dispatcher().sink<GUIRender>().disconnect<&DiagnosticSystem::RenderGUI>(this);
}
