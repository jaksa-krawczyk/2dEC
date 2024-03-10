#ifndef IMGUIHANDLER_HPP
#define IMGUIHANDLER_HPP

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>

#include <vector>

template<typename VelArrType>
class ImGuiHandler
{
	private:
		const VelArrType& velocities;
		std::vector<float> xComponent;
		std::vector<float> yComponent;
		std::vector<float> speed;

		void velocitiesHistograms()
		{
			for (std::uint32_t i = 0; i < velocities.size(); ++i)
			{
				xComponent[i] = velocities[i].x;
				yComponent[i] = velocities[i].y;
				speed[i] = glm::length(velocities[i]);
			}

			ImGui::Begin("Velocities histograms");
			if (ImPlot::BeginPlot("##VelocitiesHist"))
			{
				ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_None, ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoTickLabels);
				ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);
				ImPlot::PlotHistogram("x-component", xComponent.data(), velocities.size(), 50, 1, ImPlotRange(-75.f, 75.f));
				ImPlot::PlotHistogram("y-component", yComponent.data(), velocities.size(), 50, 1, ImPlotRange(-75.f, 75.f));
				ImPlot::EndPlot();
			}

			if (ImPlot::BeginPlot("##VelocitiesHist"))
			{
				ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_None, ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoTickLabels);
				ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);
				ImPlot::PlotHistogram("speed", speed.data(), velocities.size(), 50, 1, ImPlotRange(-0.5f, 100.f));
				ImPlot::EndPlot();
			}
			ImGui::End();
		}

	public:
		ImGuiHandler(const VelArrType& velocities_) noexcept : velocities(velocities_)
		{
			xComponent.reserve(velocities.size());
			yComponent.reserve(velocities.size());
			speed.reserve(velocities.size());
		}

		bool initialize(GLFWwindow* window)
		{
			ImGui::CreateContext();
			ImPlot::CreateContext();
			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

			if (!ImGui_ImplGlfw_InitForOpenGL(window, true) || !ImGui_ImplOpenGL3_Init())
			{
				return false;
			}
			return true;
		}

		void render()
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			velocitiesHistograms();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		void cleanup() noexcept
		{
			ImPlot::DestroyContext();
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
		}
};

#endif