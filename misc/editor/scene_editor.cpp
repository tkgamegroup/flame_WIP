void SceneEditor::on_menu_bar()
{
	if (ImGui::BeginMenu("Create"))
	{
		if (ImGui::BeginMenu("Light"))
		{
			flame::LightType light_type = flame::LightType(-1);
			if (ImGui::MenuItem("Parallax"))
				light_type = flame::LightTypeParallax;
			if (ImGui::MenuItem("Point"))
				light_type = flame::LightTypePoint;
			if (ImGui::MenuItem("Spot"))
				light_type = flame::LightTypeSpot;
			if (light_type != -1)
			{
				auto n = new flame::Node;
				n->name = "Light";
				n->set_coord(camera_node->get_world_coord());
				auto i = new flame::LightComponent;
				i->set_type(light_type);
				n->add_component(i);
				scene->add_child(n);
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("3D"))
		{
			const char *basic_model_names[] = {
				"Triangle",
				"Cube",
				"Sphere",
				"Cylinder",
				"Cone",
				"Arrow",
				"Torus",
				"Hammer"
			};
			for (int i = 0; i < TK_ARRAYSIZE(basic_model_names); i++)
			{
				if (ImGui::MenuItem(basic_model_names[i]))
				{
					auto m = flame::getModel(basic_model_names[i]);
					if (m)
					{
						auto n = new flame::Node(flame::NodeTypeNode);
						n->name = "Object";
						n->set_coord(camera_node->get_world_coord());
						auto i = new flame::ModelInstanceComponent;
						i->set_model(m);
						n->add_component(i);
						scene->add_child(n);
					}
				}
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Terrain"))
			{
				auto n = new flame::Node(flame::NodeTypeNode);
				n->set_coord(camera_node->get_world_coord());
				auto t = new flame::TerrainComponent;
				n->add_component(t);
				scene->add_child(n);
			}
			if (ImGui::MenuItem("Water"))
			{
				auto n = new flame::Node(flame::NodeTypeNode);
				n->set_coord(camera_node->get_world_coord());
				auto w = new flame::WaterComponent;
				n->add_component(w);
				scene->add_child(n);
			}
			ImGui::EndMenu();
		}

		ImGui::EndMenu();
	}
}

void SceneEditor::on_show()
{
	auto draw_list = ImGui::GetWindowDrawList();
	auto canvas_size = ImVec2(flame::resolution.x(), flame::resolution.y());
	draw_list->AddImage(ImTextureID(layer.image->ui_index), pos, pos + canvas_size);
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("file"))
		{
			static char filename[260];
			strcpy(filename, (char*)payload->Data);
			std::experimental::filesystem::path path(filename);
			auto ext = path.extension();
			if (flame::is_model_file(ext.string()))
			{
				auto m = flame::getModel(filename);
				if (m)
				{
					auto n = new flame::Node(flame::NodeTypeNode);
					n->name = "Object";
					n->set_coord(camera_node->get_world_coord());
					auto i = new flame::ModelInstanceComponent;
					i->set_model(m);
					n->add_component(i);
					scene->add_child(n);
				}
			}
		}
		ImGui::EndDragDropTarget();
	}

	if (curr_tool == transformerTool.get())
	{
		transformerTool->target = selected.get_node();
		if (transformerTool->target == scene)
			transformerTool->target = nullptr;
		transformerTool->show(glm::vec2(pos.x, pos.y), glm::vec2(canvas_size.x, canvas_size.y), camera);
	}

	if (ImGui::IsItemHovered())
	{
		if (ImGui::IsKeyDown(VK_DELETE))
			on_delete();
	}
}