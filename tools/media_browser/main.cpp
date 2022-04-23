//struct Tag
//{
//	std::wstring name;
//};
//std::vector<Tag> tags;
//
//struct TagView
//{
//	cText* c_search = nullptr;
//	Entity* e_list = nullptr;
//
//	void init(Entity* e)
//	{
//		auto e_search_bar = e->find_child("tag_search_bar");
//		c_search = e_search_bar->get_component_t<cText>();
//		e_search_bar->add_data_listener(c_search, [](Capture& c, uint h) {
//			auto thiz = c.thiz<TagView>();
//			if (h == S<"text"_h>)
//				thiz->search(thiz->c_search->get_text());
//		}, Capture().set_thiz(this));
//
//		e_list = e->find_child("tag_list");
//
//		search(L"");
//	}
//
//	void search(std::wstring_view name)
//	{
//		tags.clear();
//		e_list->remove_all_children();
//
//		auto str = std::string();
//		if (!name.empty())
//			str = sfmt("WHERE name LIKE '%%%s%%'", w2s(name).c_str());
//		auto res = db->query_fmt([](Capture& c, database::Res* res) {
//			auto thiz = c.thiz<TagView>();
//			tags.resize(res->row_count);
//			for (auto i = 0; i < tags.size(); i++)
//			{
//				res->fetch_row();
//
//				tags[i].name = s2w(res->row[1]);
//
//				auto e = Entity::create();
//				e->load(L"tag_item");
//				e->find_child("name")->get_component_t<cText>()->set_text(tags[i].name.c_str());
//
//				thiz->e_list->add_child(e);
//			}
//		}, Capture().set_thiz(this), "SELECT * FROM tk.tags %s;", str.c_str());
//		assert(res == database::NoError);
//	}
//}tag_view;
//
//struct MainView
//{
//	cText* c_search = nullptr;
//	Entity* e_list = nullptr;
//
//	void init(Entity* e)
//	{
//		c_search = e->find_child("search_bar")->get_component_t<cText>();
//
//		e->find_child("search_btn")->get_component_t<cReceiver>()->add_mouse_click_listener([](Capture& c) {
//			c.thiz<MainView>()->search();
//		}, Capture().set_thiz(this));
//
//		e_list = e->find_child("list");
//	}
//
//	void search()
//	{
//		items.clear();
//		e_list->remove_all_children();
//
//		auto sp = SUS::split(w2s(c_search->get_text()));
//		auto tag_str = std::string("(");
//		for (auto i = 0; i < sp.size(); i++)
//		{
//			tag_str += "'" + sp[i] + "'";
//			if (i < sp.size() - 1)
//				tag_str += ", ";
//		}
//		if (sp.empty())
//			tag_str += "''";
//		tag_str += ")";
//		auto res = db->query_fmt([](Capture& c, database::Res* res) {
//			auto thiz = c.thiz<MainView>();
//			items.resize(res->row_count);
//			for (auto i = 0; i < items.size(); i++)
//			{
//				res->fetch_row();
//
//				items[i].get_filename(res->row[0], res->row[1]);
//
//				uint w, h;
//				uchar* data;
//				get_thumbnail(ThumbnailSize, items[i].filename.c_str(), &w, &h, &data);
//				auto id = thumbnails_atlas.alloc_image(w, h, data);
//
//				auto e = Entity::create();
//				e->load(L"item");
//				auto element = e->get_component_t<cElement>();
//				element->set_width(ThumbnailSize);
//				element->set_height(ThumbnailSize);
//				auto padding_v = (ThumbnailSize - h) * 0.5f;
//				element->set_padding(vec4(0, padding_v, 0, padding_v));
//				auto image = e->get_component_t<cImage>();
//				image->set_res_id(thumbnails_atlas.id);
//				auto atlas_size = vec2(thumbnails_atlas.image->get_size());
//				auto uv0 = vec2((id % thumbnails_atlas.cx) * ThumbnailSize, (id / thumbnails_atlas.cx) * ThumbnailSize);
//				auto uv1 = uv0 + vec2(w, h);
//				image->set_uv(vec4(uv0 / atlas_size, uv1 / atlas_size));
//				auto thumbnail = new cThumbnail;
//				thumbnail->element = element;
//				thumbnail->w = w;
//				thumbnail->h = h;
//				thumbnail->data = data;
//				thumbnail->id = id;
//				e->add_component(thumbnail);
//				e->add_data_listener(element, [](Capture& c, uint h) {
//					auto thiz = c.thiz<cThumbnail>();
//					if (h == S<"culled"_h>)
//						thiz->toggle();
//				}, Capture().set_thiz(thumbnail));
//				auto receiver = cReceiver::create();
//				receiver->add_mouse_click_listener([](Capture& c) {
//					image_view.show(c.data<int>());
//				}, Capture().set_data(&i));
//				e->add_component(receiver);
//				thiz->e_list->add_child(e);
//			}
//		}, Capture().set_thiz(this), "SELECT * FROM tk.ssss WHERE id in (SELECT ssss_id FROM tk.ssss_tags WHERE tag_id in (SELECT id FROM tk.tags WHERE name in (%s))) ORDER BY 'order';", tag_str.c_str());
//		assert(res == database::NoError);
//	}
//}main_view;
//
//void add_tag(const char* name)
//{
//	auto res = db->query_fmt("INSERT INTO `tk`.`tags` (`id`, `name`) VALUES ('%s', '%s');", std::to_string(ch(name)).c_str(), name);
//	assert(res == database::NoError || res == database::ErrorDuplicated);
//}
//
//void collect_files(const std::filesystem::path& dir, const std::vector<const char*>& tags)
//{
//	std::vector<std::pair<std::string, std::filesystem::path>> e_list;
//	for (auto& it : std::filesystem::directory_iterator(dir))
//	{
//		if (!std::filesystem::is_directory(it.status()))
//		{
//			auto& path = it.path();
//			auto fn = std::to_string(ch(w2s(path.wstring()).c_str()));
//			e_list.emplace_back(fn, path);
//		}
//	}
//	char time_str[100];
//	{
//		time_t t;
//		time(&t);
//		auto ti = localtime(&t);
//		strftime(time_str, sizeof(time_str), "%Y-%m-%d", ti);
//	}
//	for (auto i = 0; i < e_list.size(); i++)
//	{
//		auto& item = e_list[i];
//		auto ext = item.second.extension().string();
//		auto res = db->query_fmt("INSERT INTO `tk`.`ssss` (`id`, `ext`, `time`, `order`) VALUES ('%s', '%s', '%s', '%s');", item.first.c_str(), ext.c_str(), time_str, std::to_string(i).c_str());
//		assert(res == database::NoError || res == database::ErrorDuplicated);
//		std::filesystem::copy_file(item.second, stored_path.string() + item.first + ext, std::filesystem::copy_options::skip_existing);
//		for (auto& t : tags)
//		{
//			auto tag_id = std::to_string(ch(t));
//			{
//				uint row_count;
//				auto res = db->query_fmt(&row_count, "SELECT * FROM `tk`.`ssss_tags` WHERE ssss_id='%s' AND tag_id='%s';", item.first.c_str(), tag_id.c_str());
//				assert(res == database::NoError);
//				if (row_count > 0)
//					continue;
//			}
//			auto res = db->query_fmt("INSERT INTO `tk`.`ssss_tags` (`ssss_id`, `tag_id`) VALUES ('%s', '%s')", item.first.c_str(), tag_id.c_str());
//			assert(res == database::NoError);
//		}
//	}
//}
//
//int main(int argc, char** args)
//{
//	db = database::Connection::create("tk");
//
//	g_app.create();
//
//	auto w = new GraphicsWindow(&g_app, L"Media Browser", uvec2(1280, 720), WindowFrame | WindowResizable, true, true);
//	root = w->root;
//	canvas = w->canvas;
//
//	auto screen_size = get_screen_size();
//	thumbnails_atlas.create(ceil((float)screen_size.x / (float)ThumbnailSize), ceil((float)screen_size.y / (float)ThumbnailSize), ThumbnailSize);
//
//	auto e = Entity::create();
//	e->load(L"main");
//	root->add_child(e);
//
//	main_view.init(e);
//	image_view.init(e);
//	tag_view.init(e);
//
//	g_app.run();
//
//	return 0;
//}
