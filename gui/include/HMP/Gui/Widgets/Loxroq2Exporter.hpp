#pragma once

#include <HMP/Gui/SidebarWidget.hpp>
#include <HMP/Meshing/types.hpp>
#include <vector>
#include <cinolib/gl/FreeCamera.hpp>
#include <string>
#include <optional>

namespace HMP::Gui::Widgets
{

	class Loxroq2Exporter final : public SidebarWidget
	{

	private:

		void run() const;

		void drawSidebar() override;

	public:

		Loxroq2Exporter();

	};

}