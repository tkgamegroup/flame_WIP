struct NewImageDialog : flame::ui::FileSelector
{
	int cx = 512;
	int cy = 512;

	NewImageDialog() :
		FileSelector("New Image", flame::ui::FileSelectorSave, "", flame::ui::WindowModal | flame::ui::WindowNoSavedSettings)
	{
		first_cx = 800;
		first_cy = 600;

		callback = [this](std::string s) {
			if (std::experimental::filesystem::exists(s))
				return false;

			auto i = flame::create_image(cx, cy, 4, 32);
			flame::save_image(i, s);
			delete i;
			
			return true;
		};
	}

	virtual void on_right_area_show() override
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
		ImGui::PushItemWidth(200);
		ImGui::DragInt("cx", &cx);
		ImGui::DragInt("cy", &cy);
		const char *typeNames[] = {
			"color R8G8B8A8"
		};
		static int type = 0;
		ImGui::Combo("type", &type, typeNames, TK_ARRAYSIZE(typeNames));
		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
	}
};

