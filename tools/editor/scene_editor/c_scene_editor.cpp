#include "scene_editor.h"

cSceneEditor::cSceneEditor() :
	Component("cSceneEditor")
{
	auto& ui = scene_editor.window->ui;

	auto e_page = ui.e_begin_docker_page(L"Editor").second;
	{
		auto c_layout = ui.c_layout(LayoutVertical);
		c_layout->item_padding = 4.f;
		c_layout->width_fit_children = false;
		c_layout->height_fit_children = false;

		e_page->add_component(this);
	}
	
		ui.e_begin_layout(LayoutHorizontal, 4.f);
			ui.e_begin_combobox();
				ui.e_combobox_item(L"2D");
				ui.e_combobox_item(L"3D");
			ui.e_end_combobox(0);
			tool_type = 1;
			ui.e_begin_combobox();
				ui.e_combobox_item(L"Select");
				ui.e_combobox_item(L"Gizmo");
			ui.e_end_combobox(tool_type)->get_component(cCombobox)->data_changed_listeners.add([](Capture& c, uint hash, void*) {
				if (hash == FLAME_CHASH("index"))
					scene_editor.editor->tool_type = c.current<cCombobox>()->index;
				return true;
			}, Capture());
		ui.e_end_layout();

		edt.create(ui, [](Capture&, const vec4& r) {
			if (r.x == r.z && r.y == r.z)
				scene_editor.select(nullptr);
			else
				scene_editor.select(scene_editor.editor->search_hovering(r));
		}, Capture());
		edt.scale_level_max = 20;

			{
				auto e_overlay = edt.overlay->entity;

				edt.overlay->cmds.add([](Capture& c, graphics::Canvas* canvas) {
					auto element = c.thiz<cElement>();
					if (!element->clipped && scene_editor.selected)
					{
						auto se = scene_editor.selected->get_component(cElement);
						if (se)
						{
							std::vector<vec2> points;
							path_rect(points, se->global_pos, se->global_size);
							points.push_back(points[0]);
							canvas->stroke(points.size(), points.data(), cvec4(0, 0, 0, 255), 2.f);
						}
					}
					return true;
				}, Capture().set_thiz(edt.overlay));
				ui.current_entity = e_overlay;
				auto c_receiver = e_overlay->get_component(cReceiver);
				c_receiver->pass_checkers.add([](Capture&, cReceiver*, bool* pass) {
					*pass = true;
					return true;
				}, Capture());
				c_receiver->mouse_listeners.add([](Capture& c, KeyStateFlags action, MouseKey key, const ivec2& pos) {
					if (is_mouse_down(action, key, true) && key == Mouse_Left)
						scene_editor.select(scene_editor.editor->search_hovering(vec4(vec2(pos), vec2(pos))));
					return true;
				}, Capture());

				ui.parents.push(e_overlay);
					gizmo.create();
					gizmo.base = edt.base;
				ui.parents.pop();
			}

	ui.e_end_docker_page();
}

Entity* cSceneEditor::search_hovering(const vec4& r)
{
	Entity* s = nullptr;
	if (scene_editor.prefab)
		search_hovering_r(scene_editor.prefab, s, r);
	return s;
}

void cSceneEditor::search_hovering_r(Entity* e, Entity*& s, const vec4& r)
{
	if (e->children.s > 0)
	{
		for (auto i = (int)e->children.s - 1; i >= 0; i--)
		{
			auto c = e->children[i];
			if (c->global_visibility)
				search_hovering_r(c, s, r);
		}
	}
	if (s)
		return;

	auto element = e->get_component(cElement);
	if (element && rect_overlapping(element->clipped_rect, r))
		s = e;
}

void cSceneEditor::on_select()
{
	gizmo.on_select();
}
