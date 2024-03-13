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

		bool showHistograms = false;
		bool pause = false;

		void velocitiesHistograms() noexcept
		{
			if (!showHistograms)
			{
				return;
			}

			for (std::uint32_t i = 0; i < velocities.size(); ++i)
			{
				xComponent[i] = velocities[i].x;
				yComponent[i] = velocities[i].y;
				speed[i] = glm::length(velocities[i]);
			}

			static int componentsBins = 30;
			ImGui::SetNextWindowSize(ImVec2(550, 700), ImGuiCond_Appearing);
			ImGui::Begin("Velocities histograms", &showHistograms);
			ImGui::SliderInt("Nr of component bins", &componentsBins, 10, 100);
			if (ImPlot::BeginPlot("##VelocitiesHist"))
			{
				ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_None, ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoTickLabels);
				ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);
				ImPlot::PlotHistogram("x-component", xComponent.data(), velocities.size(), componentsBins, 1, ImPlotRange(), ImPlotHistogramFlags_Density);
				ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);
				ImPlot::PlotHistogram("y-component", yComponent.data(), velocities.size(), componentsBins, 1, ImPlotRange(), ImPlotHistogramFlags_Density);
				ImPlot::EndPlot();
			}
			static int speedBins = 30;
			ImGui::SliderInt("Nr of speed bins", &speedBins, 10, 100);
			if (ImPlot::BeginPlot("##VelocitiesHist"))
			{
				ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_None, ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoTickLabels);
				ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);
				ImPlot::PlotHistogram("speed", speed.data(), velocities.size(), speedBins, 1, ImPlotRange(), ImPlotHistogramFlags_Density);
				ImPlot::EndPlot();
			}
			ImGui::End();
		}

		void showControlPanel() noexcept
		{
			ImGui::Begin("Control panel");
			ImGui::Checkbox("Show velocity statistics", &showHistograms);
			if (ImGui::Button("Pause simulation"))
			{
				pause = true;
			}
			ImGui::SameLine();
			if (ImGui::Button("Resume simulation"))
			{
				pause = false;
			}
			ImGui::Text("Simulation average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

	public:
		ImGuiHandler(const VelArrType& velocities_) : velocities(velocities_)
		{
			xComponent.reserve(velocities.size());
			yComponent.reserve(velocities.size());
			speed.reserve(velocities.size());
		}

		bool pauseSimulation() noexcept
		{
			return pause;
		}

		bool initialize(GLFWwindow* window) noexcept
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

		void render() noexcept
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			showControlPanel();
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