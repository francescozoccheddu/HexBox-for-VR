#include <HMP/Gui/Widgets/Loxroq2Exporter.hpp>

#include <HMP/Gui/Utils/Controls.hpp>
#include <HMP/Gui/Utils/FilePicking.hpp>
#include <HMP/Gui/themer.hpp>
#include <HMP/Gui/App.hpp>
#include <algorithm>
#include <imgui.h>
#include <string>
#include <fstream>
#include <ctime>
#include <cassert>
#include <iomanip>
#include <cinolib/quality_hex.h>
#include <HMP/Gui/Widgets/Target.hpp>
#include <cpputils/serialization/Serializer.hpp>

namespace HMP::Gui::Widgets
{

	Loxroq2Exporter::Loxroq2Exporter() : SidebarWidget{ "loxroq2 exporter" }
	{}

	void Loxroq2Exporter::drawSidebar()
	{
		if (ImGui::Button("Export"))
		{
			run();
		}
	}

	void Loxroq2Exporter::run() const
	{
		const std::optional<std::string> filename{ Utils::FilePicking::save("mesh") };
		if (filename)
		{
			std::ofstream file{};
			file.open(*filename);
			HMP::Utils::Serialization::Serializer serializer{ file };
			const Meshing::Mesher::Mesh& mesh{ app().mesh };
			serializer << mesh.vector_verts().size();
			for (const Vec& vec : mesh.vector_verts())
			{
				serializer << vec;
			}
			serializer << mesh.num_polys();
			for (Id pid{}; pid < mesh.num_polys(); pid++)
			{
				for (const Id vid : mesh.poly_verts_id(pid))
				{
					serializer << vid;
				}
				serializer << cinolib::hex_scaled_jacobian(
					mesh.poly_vert(pid, 0),
					mesh.poly_vert(pid, 1),
					mesh.poly_vert(pid, 2),
					mesh.poly_vert(pid, 3),
					mesh.poly_vert(pid, 4),
					mesh.poly_vert(pid, 5),
					mesh.poly_vert(pid, 6),
					mesh.poly_vert(pid, 7)
				);
			}
			file.close();
		}
	}

}