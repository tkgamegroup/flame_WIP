//if (aa) // fill aa
//{
//	auto vtx_cnt0 = cmd->vertices_count;
//	auto feather = 2.f;
//
//	points.push_back(points.front());
//	auto normals = calculate_normals(points, true);
//
//	auto col_t = col;
//	col_t.a = 0;
//
//	for (auto i = 0; i < points.size() - 1; i++)
//	{
//		if (i == 0)
//		{
//			auto p0 = points[0];
//			auto p1 = points[1];
//
//			auto n0 = normals[0];
//			auto n1 = normals[1];
//
//			auto vtx_cnt = cmd->vertices_count;
//
//			add_vtx(p0, uv, col);
//			add_vtx(p0 - n0 * feather, uv, col_t);
//			add_vtx(p1, uv, col);
//			add_vtx(p1 - n1 * feather, uv, col_t);
//			add_idx(vtx_cnt + 0); add_idx(vtx_cnt + 3); add_idx(vtx_cnt + 1); add_idx(vtx_cnt + 0); add_idx(vtx_cnt + 2); add_idx(vtx_cnt + 3);
//		}
//		else if (i == points.size() - 2)
//		{
//			auto vtx_cnt = cmd->vertices_count;
//
//			add_idx(vtx_cnt - 2); add_idx(vtx_cnt0 + 1); add_idx(vtx_cnt - 1); add_idx(vtx_cnt - 2); add_idx(vtx_cnt0 + 0); add_idx(vtx_cnt0 + 1);
//		}
//		else
//		{
//			auto p1 = points[i + 1];
//
//			auto n1 = normals[i + 1];
//
//			auto vtx_cnt = cmd->vertices_count;
//
//			add_vtx(p1, uv, col);
//			add_vtx(p1 - n1 * feather, uv, col_t);
//			add_idx(vtx_cnt - 2); add_idx(vtx_cnt + 1); add_idx(vtx_cnt - 1); add_idx(vtx_cnt - 2); add_idx(vtx_cnt + 0); add_idx(vtx_cnt + 1);
//		}
//	}
//}
// if (aa) // stroke aa
// {
// 
		//static const auto feather = 0.5f;
		//auto col_c = col;
		//col_c.a *= min(thickness / feather, 1.f);
		//auto col_t = col;
		//col_t.a = 0;
		//					
		//if (thickness > feather)
		//{
		//	auto edge = thickness - feather;
		//					
		//	for (auto i = 0; i < points.size() - 1; i++)
		//	{
		//		if (i == 0)
		//		{
		//			auto p0 = points[0];
		//			auto p1 = points[1];
		//					
		//			auto n0 = normals[0];
		//			auto n1 = normals[1];
		//					
		//			auto vtx_cnt = cmd->vertices_count;
		//					
		//			add_vtx(p0 + n0 * thickness, uv, col_t);
		//			add_vtx(p0 + n0 * edge, uv, col_c);
		//			add_vtx(p0 - n0 * edge, uv, col_c);
		//			add_vtx(p0 - n0 * thickness, uv, col_t);
		//			add_vtx(p1 + n1 * thickness, uv, col_t);
		//			add_vtx(p1 + n1 * edge, uv, col_c);
		//			add_vtx(p1 - n1 * edge, uv, col_c);
		//			add_vtx(p1 - n1 * thickness, uv, col_t);
		//			add_idx(vtx_cnt + 1); add_idx(vtx_cnt + 6); add_idx(vtx_cnt + 2); add_idx(vtx_cnt + 1); add_idx(vtx_cnt + 5); add_idx(vtx_cnt + 6);
		//			add_idx(vtx_cnt + 0); add_idx(vtx_cnt + 5); add_idx(vtx_cnt + 1); add_idx(vtx_cnt + 0); add_idx(vtx_cnt + 4); add_idx(vtx_cnt + 5);
		//			add_idx(vtx_cnt + 2); add_idx(vtx_cnt + 7); add_idx(vtx_cnt + 3); add_idx(vtx_cnt + 2); add_idx(vtx_cnt + 6); add_idx(vtx_cnt + 7);
		//		}
		//		else if (closed && i == points.size() - 2)
		//		{
		//			auto vtx_cnt = cmd->vertices_count;
		//					
		//			add_idx(vtx_cnt - 3); add_idx(vtx_cnt0 + 2); add_idx(vtx_cnt - 2); add_idx(vtx_cnt - 3); add_idx(vtx_cnt0 + 1); add_idx(vtx_cnt0 + 2);
		//			add_idx(vtx_cnt - 4); add_idx(vtx_cnt0 + 1); add_idx(vtx_cnt - 3); add_idx(vtx_cnt - 4); add_idx(vtx_cnt0 + 0); add_idx(vtx_cnt0 + 1);
		//			add_idx(vtx_cnt - 2); add_idx(vtx_cnt0 + 3); add_idx(vtx_cnt - 1); add_idx(vtx_cnt - 2); add_idx(vtx_cnt0 + 2); add_idx(vtx_cnt0 + 3);
		//		}
		//		else
		//		{
		//			auto p1 = points[i + 1];
		//					
		//			auto n1 = normals[i + 1];
		//					
		//			auto vtx_cnt = cmd->vertices_count;
		//					
		//			add_vtx(p1 + n1 * thickness, uv, col_t);
		//			add_vtx(p1 + n1 * edge, uv, col_c);
		//			add_vtx(p1 - n1 * edge, uv, col_c);
		//			add_vtx(p1 - n1 * thickness, uv, col_t);
		//			add_idx(vtx_cnt - 3); add_idx(vtx_cnt + 2); add_idx(vtx_cnt - 2); add_idx(vtx_cnt - 3); add_idx(vtx_cnt + 1); add_idx(vtx_cnt + 2);
		//			add_idx(vtx_cnt - 4); add_idx(vtx_cnt + 1); add_idx(vtx_cnt - 3); add_idx(vtx_cnt - 4); add_idx(vtx_cnt + 0); add_idx(vtx_cnt + 1);
		//			add_idx(vtx_cnt - 2); add_idx(vtx_cnt + 3); add_idx(vtx_cnt - 1); add_idx(vtx_cnt - 2); add_idx(vtx_cnt + 2); add_idx(vtx_cnt + 3);
		//		}
		//	}
		//}
		//else
		//{
		//	for (auto i = 0; i < points.size() - 1; i++)
		//	{
		//		if (i == 0)
		//		{
		//			auto p0 = points[0];
		//			auto p1 = points[1];
		//					
		//			auto n0 = normals[0];
		//			auto n1 = normals[1];
		//					
		//			auto vtx_cnt = cmd->vertices_count;
		//					
		//			add_vtx(p0 + n0 * feather, uv, col_t);
		//			add_vtx(p0, uv, col_c);
		//			add_vtx(p0 - n0 * feather, uv, col_t);
		//			add_vtx(p1 + n1 * feather, uv, col_t);
		//			add_vtx(p1, uv, col_c);
		//			add_vtx(p1 - n1 * feather, uv, col_t);
		//			add_idx(vtx_cnt + 0); add_idx(vtx_cnt + 4); add_idx(vtx_cnt + 1); add_idx(vtx_cnt + 0); add_idx(vtx_cnt + 3); add_idx(vtx_cnt + 4);
		//			add_idx(vtx_cnt + 1); add_idx(vtx_cnt + 5); add_idx(vtx_cnt + 2); add_idx(vtx_cnt + 1); add_idx(vtx_cnt + 4); add_idx(vtx_cnt + 5);
		//		}
		//		else if (closed && i == points.size() - 2)
		//		{
		//			auto vtx_cnt = cmd->vertices_count;
		//					
		//			add_idx(vtx_cnt - 3); add_idx(vtx_cnt0 + 1); add_idx(vtx_cnt - 2); add_idx(vtx_cnt - 3); add_idx(vtx_cnt0 + 0); add_idx(vtx_cnt0 + 1);
		//			add_idx(vtx_cnt - 2); add_idx(vtx_cnt0 + 2); add_idx(vtx_cnt - 1); add_idx(vtx_cnt - 2); add_idx(vtx_cnt0 + 1); add_idx(vtx_cnt0 + 2);
		//		}
		//		else
		//		{
		//			auto p1 = points[i + 1];
		//					
		//			auto n1 = normals[i + 1];
		//					
		//			auto vtx_cnt = cmd->vertices_count;
		//					
		//			add_vtx(p1 + n1 * feather, uv, col_t);
		//			add_vtx(p1, uv, col_c);
		//			add_vtx(p1 - n1 * feather, uv, col_t);
		//			add_idx(vtx_cnt - 3); add_idx(vtx_cnt + 1); add_idx(vtx_cnt - 2); add_idx(vtx_cnt - 3); add_idx(vtx_cnt + 0); add_idx(vtx_cnt + 1);
		//			add_idx(vtx_cnt - 2); add_idx(vtx_cnt + 2); add_idx(vtx_cnt - 1); add_idx(vtx_cnt - 2); add_idx(vtx_cnt + 1); add_idx(vtx_cnt + 2);
		//		}
		//	}
		//}
		//					
		//if (!closed)
		//{
		//	auto ext = max(feather, thickness);
		//					
		//	{
		//		auto vtx_cnt = cmd->vertices_count;
		//					
		//		auto p0 = points[0];
		//		auto p1 = points[1];
		//					
		//		auto n0 = normals[0];
		//					
		//		auto p = p0 - normalize(p1 - p0);
		//		add_vtx(p + n0 * ext, uv, col_t);
		//		add_vtx(p - n0 * ext, uv, col_t);
		//		add_vtx(p0 + n0 * ext, uv, col_t);
		//		add_vtx(p0, uv, col_c);
		//		add_vtx(p0 - n0 * ext, uv, col_t);
		//		add_idx(vtx_cnt + 1); add_idx(vtx_cnt + 4); add_idx(vtx_cnt + 2);
		//		add_idx(vtx_cnt + 1); add_idx(vtx_cnt + 3); add_idx(vtx_cnt + 4); 
		//		add_idx(vtx_cnt + 1); add_idx(vtx_cnt + 3); add_idx(vtx_cnt + 0);
		//	}
		//					
		//	{
		//		auto vtx_cnt = cmd->vertices_count;
		//					
		//		auto p0 = points[points.size() - 2];
		//		auto p1 = points[points.size() - 1];
		//					
		//		auto n1 = normals[points.size() - 1];
		//					
		//		auto p = p1 + normalize(p1 - p0);
		//		add_vtx(p1 + n1 * ext, uv, col_t);
		//		add_vtx(p1, uv, col_c);
		//		add_vtx(p1 - n1 * ext, uv, col_t);
		//		add_vtx(p + n1 * ext, uv, col_t);
		//		add_vtx(p - n1 * ext, uv, col_t);
		//		add_idx(vtx_cnt + 1); add_idx(vtx_cnt + 4); add_idx(vtx_cnt + 2);
		//		add_idx(vtx_cnt + 1); add_idx(vtx_cnt + 3); add_idx(vtx_cnt + 4);
		//		add_idx(vtx_cnt + 1); add_idx(vtx_cnt + 0); add_idx(vtx_cnt + 3);
		//	}
		//}
// }
//		struct CanvasPrivate : Canvas
//		{
//			std::unique_ptr<ImagePrivate> pickup_image;
//			std::unique_ptr<FramebufferPrivate> pickup_framebuffer;
//
//			ShaderGeometryBuffer<Triangle> triangle_buffer;
//		};
// 
//		inline void path_arc(std::vector<vec2>& points, const vec2& center, float radius, float a1, float a2, uint lod = 0)
//		{
//			int a = pieces_num * a1;
//			int b = pieces_num * a2;
//			lod += 1;
//			for (; a <= b; a += lod)
//				points.push_back(center + pieces[a % pieces_num] * radius);
//		}
//
//		// roundness: a RT b LB
//		inline void path_rect(std::vector<vec2>& points, const vec2& pos, const vec2& size, const vec4& roundness = vec4(0.f), uint lod = 0)
//		{
//			if (roundness[0] > 0.f)
//				path_arc(points, pos + vec2(roundness[0]), roundness[0], 0.5f, 0.75f, lod);
//			else
//				points.push_back(pos);
//			if (roundness[1] > 0.f)
//				path_arc(points, pos + vec2(size.x - roundness[1], roundness[1]), roundness[1], 0.75f, 1.f, lod);
//			else
//				points.push_back(pos + vec2(size.x, 0.f));
//			if (roundness[2] > 0.f)
//				path_arc(points, pos + size - vec2(roundness[2]), roundness[2], 0.f, 0.25f, lod);
//			else
//				points.push_back(pos + size);
//			if (roundness[3] > 0.f)
//				path_arc(points, pos + vec2(roundness[3], size.y - roundness[3]), roundness[3], 0.25f, 0.5f, lod);
//			else
//				points.push_back(pos + vec2(0.f, size.y));
//		}
//
//		inline void path_circle(std::vector<vec2>& points, const vec2& center, float radius, uint lod = 0)
//		{
//			path_arc(points, center, radius, 0.f, 1.f, lod);
//		}
//
//		inline void path_bezier(std::vector<vec2>& points, const vec2& p1, const vec2& p2, const vec2& p3, const vec2& p4, uint level = 0)
//		{
//			auto dx = p4.x - p1.x;
//			auto dy = p4.y - p1.y;
//			auto d2 = ((p2.x - p4.x) * dy - (p2.y - p4.y) * dx);
//			auto d3 = ((p3.x - p4.x) * dy - (p3.y - p4.y) * dx);
//			d2 = (d2 >= 0) ? d2 : -d2;
//			d3 = (d3 >= 0) ? d3 : -d3;
//			if ((d2 + d3) * (d2 + d3) < 1.25f * (dx * dx + dy * dy))
//			{
//				if (points.empty())
//					points.push_back(p1);
//				points.push_back(p4);
//			}
//			else if (level < 10)
//			{
//				auto p12 = (p1 + p2) * 0.5f;
//				auto p23 = (p2 + p3) * 0.5f;
//				auto p34 = (p3 + p4) * 0.5f;
//				auto p123 = (p12 + p23) * 0.5f;
//				auto p234 = (p23 + p34) * 0.5f;
//				auto p1234 = (p123 + p234) * 0.5f;
//
//				path_bezier(points, p1, p12, p123, p1234, level + 1);
//				path_bezier(points, p1234, p234, p34, p4, level + 1);
//			}
//		}
//
//		void CanvasPrivate::record(CommandBufferPrivate* cb, uint image_index)
//		{
//			for (auto& p : passes)
//			{
//				switch (p.type)
//				{
//				case PassBlur:
//				{
//					auto c = (CmdBlur*)cmds[p.cmd_ids[0]].get();
//					auto blur_radius = clamp(c->radius, 0U, 10U);
//					auto blur_range = c->range;
//					auto blur_size = vec2(blur_range.b.x - blur_range.a.x, blur_range.b.y - blur_range.a.y);
//					if (blur_size.x < 1.f || blur_size.y < 1.f)
//						continue;
//
//					cb->image_barrier(dst, {}, ImageLayoutShaderReadOnly, ImageLayoutShaderReadOnly, AccessColorAttachmentWrite);
//					cb->set_scissor(Rect(blur_range.a.x - blur_radius, blur_range.a.y - blur_radius,
//						blur_range.b.x + blur_radius, blur_range.b.y + blur_radius));
//					cb->begin_renderpass(nullptr, back_framebuffers[0].get());
//					cb->bind_pipeline(preferences->blurh_pipeline[blur_radius - 1].get());
//					//cb->bind_descriptor_set(dst_ds, 0);
//					//cb->push_constant_ht(S<"pxsz"_h>, 1.f / output_size.x);
//					cb->draw(3, 1, 0, 0);
//					cb->end_renderpass();
//
//					cb->image_barrier(back_image.get(), {}, ImageLayoutShaderReadOnly, ImageLayoutShaderReadOnly, AccessColorAttachmentWrite);
//					cb->set_scissor(blur_range);
//					cb->begin_renderpass(nullptr, dst_fb);
//					cb->bind_pipeline(preferences->blurv_pipeline[blur_radius - 1].get());
//					//cb->bind_descriptor_set(back_nearest_descriptorsets[0].get(), 0);
//					//cb->push_constant_ht(S<"pxsz"_h>, 1.f / output_size.y);
//					cb->draw(3, 1, 0, 0);
//					cb->end_renderpass();
//				}
//					break;
//				}
//			}
//		}
//	}
