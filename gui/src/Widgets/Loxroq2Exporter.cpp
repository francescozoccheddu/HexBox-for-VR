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
#include <cpputils/range/of.hpp>
#include <cpputils/range/index.hpp>

namespace HMP::Gui::Widgets
{

	Loxroq2Exporter::Loxroq2Exporter() : SidebarWidget{"loxroq2 exporter"}
	{
	}

	void Loxroq2Exporter::drawSidebar()
	{
		if (ImGui::Button("Export"))
		{
			run();
		}
	}

	void Loxroq2Exporter::run() const
	{
		const std::optional<std::string> filename{Utils::FilePicking::save("mesh")};
		if (filename)
		{
			std::ofstream file{};
			file.open(*filename);
			HMP::Utils::Serialization::Serializer serializer{file};
			const Meshing::Mesher::Mesh &mesh{app().mesh};
			serializer << mesh.vector_verts().size();
			for (const Vec &vec : mesh.vector_verts())
			{
				serializer << vec;
			}
			serializer << cpputils::range::count(static_cast<Id>(mesh.num_polys())).map([&](const Id _pid) {
				return app().mesher.shown(_pid) ? 1 : 0;
			}).sum();
			for (Id pid{}; pid < mesh.num_polys(); pid++)
			{
				if (!app().mesher.shown(pid))
				{
					continue;
				}
				const std::vector<Id> polyVids{mesh.poly_verts_id(pid)};
				serializer << polyVids.size();
				for (const Id vid : polyVids)
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
						mesh.poly_vert(pid, 7));
				serializer << 6 * 2;
				for (const Id fid : mesh.adj_p2f(pid))
				{
					std::vector<Id> faceVids{mesh.face_verts_id(fid)};
					if (mesh.poly_face_is_CW(pid, fid))
					{
						std::reverse(faceVids.begin(), faceVids.end());
					}
					for (const TriVertIs triIs : {TriVertIs{0, 1, 2}, TriVertIs{0, 2, 3}})
					{
						for (const I i : triIs)
						{
							serializer << std::find(polyVids.begin(), polyVids.end(), faceVids[i]) - polyVids.begin();
						}
					}
				}
			}
			file.close();
		}
	}

}