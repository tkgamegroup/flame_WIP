#include <flame/foundation/bitmap.h>

#include "scene_editor.h"
#include "../bp_editor/bp_editor.h"

cResourceExplorer::cResourceExplorer() :
	Component("cResourceExplorer")
{
	auto& ui = scene_editor.window->ui;

	ui.next_element_padding = 4.f;
	auto e_page = ui.e_begin_docker_page(L"Resource Explorer").second;
	{
		auto c_layout = ui.c_layout(LayoutVertical);
		c_layout->item_padding = 4.f;
		c_layout->width_fit_children = false;
		c_layout->height_fit_children = false;
		c_layout->fence = 2;

		e_page->add_component(this);
	}

	base_path = app.resource_path;

		address_bar = ui.e_empty();
		ui.c_element();
		ui.c_layout(LayoutHorizontal);

		ui.e_begin_scrollbar(ScrollbarVertical, true);
			e_list = ui.e_begin_list(true);
			{
				c_list_element = e_list->get_component(cElement);

				c_list_layout = e_list->get_component(cLayout);
				c_list_layout->type = LayoutTile;
				c_list_layout->column = 4;
			}
				ui.e_begin_popup_menu();
					ui.e_menu_item(L"New BP", [](Capture& c) {
						auto& ui = scene_editor.window->ui;
						ui.e_input_dialog(L"name", [](Capture& c, bool ok, const wchar_t* text) {
							if (ok)
							{
								std::ofstream file((scene_editor.resource_explorer->curr_path / text).replace_extension(L".bp"));
								file << "<BP />";
								file.close();
							}
						}, Capture());
					}, Capture());
				ui.e_end_popup_menu();
			ui.e_end_list();
		ui.e_end_scrollbar();

	ui.e_end_docker_page();

	ev_file_changed = create_event(false);
	ev_end_file_watcher = add_file_watcher(base_path.c_str(), [](Capture& c, FileChangeType type, const wchar_t* filename) {
		set_event(scene_editor.resource_explorer->ev_file_changed);
	}, Capture(), true, false);

	navigate(base_path);
}

void cResourceExplorer::navigate(const std::filesystem::path& path)
{
	curr_path = path;

	looper().add_event([](Capture& c) {
		auto& ui = scene_editor.window->ui;
		auto& base_path = scene_editor.resource_explorer->base_path;
		auto& curr_path = scene_editor.resource_explorer->curr_path;
		auto address_bar = scene_editor.resource_explorer->address_bar;
		auto list = scene_editor.resource_explorer->e_list;

		address_bar->remove_children(0, -1);
		ui.parents.push(address_bar);
		ui.push_style(ButtonColorNormal, common(cvec4(0)));
		ui.push_style(ButtonColorHovering, common(ui.style(FrameColorHovering).c));
		ui.push_style(ButtonColorActive, common(ui.style(FrameColorActive).c));

		ui.e_button(Icon_LEVEL_UP, [](Capture& c) {
			if (scene_editor.resource_explorer->curr_path != scene_editor.resource_explorer->base_path)
				scene_editor.resource_explorer->navigate(scene_editor.resource_explorer->curr_path.parent_path());
		}, Capture());

		std::vector<std::filesystem::path> stems;
		for (auto p = curr_path; ; p = p.parent_path())
		{
			stems.push_back(p);
			if (p == base_path)
				break;
		}
		std::reverse(stems.begin(), stems.end());

		for (auto i = 0; i < stems.size(); i++)
		{
			auto& s = stems[i];
			struct Capturing
			{
				wchar_t p[256];
			}capture;
			wcscpy_s(capture.p, s.c_str());
			ui.e_button(i == 0 ? s.c_str() : s.filename().c_str(), [](Capture& c) {
				auto& capture = c.data<Capturing>();
				scene_editor.resource_explorer->navigate(capture.p);
			}, Capture().set_data(&capture));

			std::vector<std::filesystem::path> sub_dirs;
			for (auto& it : std::filesystem::directory_iterator(s))
			{
				if (std::filesystem::is_directory(it.status()))
					sub_dirs.push_back(it.path());
			}
			if (!sub_dirs.empty())
			{
				ui.e_begin_button_menu(Icon_CARET_RIGHT);
				for (auto& p : sub_dirs)
				{
					struct Capturing
					{
						wchar_t p[256];
					}capture;
					wcscpy_s(capture.p, s.c_str());
					ui.e_menu_item(p.filename().c_str(), [](Capture& c) {
						auto& capture = c.data<Capturing>();
						scene_editor.resource_explorer->navigate(capture.p);
					}, Capture().set_data(&capture));
				}
				ui.e_end_button_menu();
			}
		}

		ui.pop_style(ButtonColorNormal);
		ui.pop_style(ButtonColorHovering);
		ui.pop_style(ButtonColorActive);
		ui.parents.pop();

		list->get_component(cList)->set_selected(nullptr, false);
		list->remove_children(0, -1);

		std::vector<std::filesystem::path> dirs;
		std::vector<std::filesystem::path> files;
		for (auto& it : std::filesystem::directory_iterator(curr_path))
		{
			if (std::filesystem::is_directory(it.status()))
			{
				if (it.path().filename().wstring()[0] != L'.')
					dirs.push_back(it.path());
			}
			else
			{
				auto ext = it.path().extension();
				if (ext != L".ilk" && ext != L".exp")
					files.push_back(it.path());
			}
		}
		ui.parents.push(list);
		for (auto& p : dirs)
		{
			auto item = scene_editor.resource_explorer->create_listitem(p.filename().wstring(), scene_editor.resource_explorer->folder_img_idx);
			struct Capturing
			{
				wchar_t p[256];
			}capture;
			wcscpy_s(capture.p, p.c_str());
			item->get_component(cReceiver)->mouse_listeners.add([](Capture& c, KeyStateFlags action, MouseKey key, const ivec2& pos) {
				if (is_mouse_clicked(action, key) && (action & KeyStateDouble))
				{
					auto& capture = c.data<Capturing>();
					scene_editor.resource_explorer->navigate(capture.p);
				}
				return true;
			}, Capture().set_data(&capture));
			ui.parents.push(item);
			ui.e_begin_popup_menu();
			ui.e_menu_item(L"Open", [](Capture& c) {
				auto& capture = c.data<Capturing>();
				scene_editor.resource_explorer->selected = capture.p;
				scene_editor.resource_explorer->navigate(scene_editor.resource_explorer->selected);
			}, Capture().set_data(&capture));
			ui.e_end_popup_menu();
			ui.parents.pop();
		}
		for (auto& p : files)
		{
			auto is_image_type = false;
			auto ext = p.extension();
			if (ext == L".bmp" ||
				ext == L".jpg" ||
				ext == L".png" ||
				ext == L".gif" ||
				ext == L".mp4")
				is_image_type = true;

			auto item = scene_editor.resource_explorer->create_listitem(p.filename().wstring(), is_image_type ? 0 : scene_editor.resource_explorer->file_img_idx);
			if (is_image_type)
			{
				auto e_image = item->children[0];
				e_image->get_component(cImage)->color = cvec4(100, 100, 100, 128);

				auto c_thumbnail = new<cThumbnail>();
				c_thumbnail->filename = std::filesystem::canonical(p).wstring();
				e_image->add_component(c_thumbnail);
			}
			ui.parents.push(item);
			if (ext == L".prefab")
			{
				struct Capturing
				{
					wchar_t p[256];
				}capture;
				wcscpy_s(capture.p, p.c_str());
				auto er = item->get_component(cReceiver);
				er->mouse_listeners.add([](Capture& c, KeyStateFlags action, MouseKey key, const ivec2& pos) {
					if (is_mouse_clicked(action, key) && (action & KeyStateDouble))
					{
						auto& capture = c.data<Capturing>();
						scene_editor.resource_explorer->selected = capture.p;
						scene_editor.load(scene_editor.resource_explorer->selected);
					}
					return true;
				}, Capture().set_data(&capture));
				ui.e_begin_popup_menu();
				ui.e_menu_item(L"Open", [](Capture& c) {
					c.thiz<cReceiver>()->send_mouse_event(KeyStateDown | KeyStateUp | KeyStateDouble, Mouse_Null, ivec2(0));
				}, Capture().set_thiz(er));
				ui.e_end_popup_menu();
			}
			else if (ext == L".bp")
			{
				struct Capturing
				{
					wchar_t p[256];
				}capture;
				wcscpy_s(capture.p, p.c_str());
				auto er = item->get_component(cReceiver);
				er->mouse_listeners.add([](Capture& c, KeyStateFlags action, MouseKey key, const ivec2& pos) {
					if (is_mouse_clicked(action, key) && (action & KeyStateDouble))
					{
						auto& capture = c.data<Capturing>();
						scene_editor.resource_explorer->selected = capture.p;
						if (!bp_editor.window)
							new BPEditorWindow(scene_editor.resource_explorer->selected);
					}
					return true;
				}, Capture().set_data(&capture));
				ui.e_begin_popup_menu();
				ui.e_menu_item(L"Open", [](Capture& c) {
					c.thiz<cReceiver>()->send_mouse_event(KeyStateDown | KeyStateUp | KeyStateDouble, Mouse_Null, ivec2(0));
				}, Capture().set_thiz(er));
				ui.e_end_popup_menu();
			}
			ui.parents.pop();
		}
		ui.parents.pop();
	}, Capture().set_thiz(this));
}
