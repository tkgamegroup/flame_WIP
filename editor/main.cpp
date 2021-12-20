struct U_godrayparm
{
	Vec2 pos;
	float intensity;
};

void make_cbs(int idx) 
{
	cb[idx]->begin_renderpass(rp_godray, fb_godray);
	cb[idx]->bind_pipeline(p_godray);
	cb[idx]->bind_descriptorset(ds_godray, 0);
	cb[idx]->draw(3, 1, 0);
	cb[idx]->end_renderpass();
};

auto day = true;

int main(int argc, char **args)
{
	cb_update_thread = d->tcp->create_commandbuffer();

	ubo_godrayparm = new graphics::UBO<1>(d, sizeof(U_godrayparm));

	{
		u_godrayparm = (U_godrayparm*)ubo_godrayparm->stag->mapped;
		u_godrayparm->pos = Vec2(0.5f);
		ubo_godrayparm->update();
	}

	graphics::RenderpassAttachmentInfo rp_godray_att_info[] = {
		{ graphics::Format_R8G8B8A8_UNORM, false }
	};
	int rp_godray_col_att = 0;
	graphics::RenderpassSubpassInfo rp_godray_sp_info[] = {
		{ &rp_godray_col_att, 1, -1 }
	};
	graphics::RenderpassInfo rp_godray_info;
	rp_godray_info.attachment_count = 1;
	rp_godray_info.attachments = rp_godray_att_info;
	rp_godray_info.subpass_count = 1;
	rp_godray_info.subpasses = rp_godray_sp_info;
	rp_godray = graphics::get_renderpass(d, &rp_godray_info);
	graphics::Imageview *fb_godray_vs[] = {
		img_v_dst
	};
	fb_godray = graphics::get_framebuffer(d, res, rp_godray, 1, fb_godray_vs);

	graphics::RenderpassAttachmentInfo rp_combine_att_info[] = {
		{ graphics::Format_R8G8B8A8_UNORM, false }
	};
	int rp_combine_col_att = 0;
	graphics::RenderpassSubpassInfo rp_combine_sp_info[] = {
		{ &rp_combine_col_att, 1, -1 }
	};
	graphics::RenderpassInfo rp_combine_info;
	rp_combine_info.attachment_count = 1;
	rp_combine_info.attachments = rp_combine_att_info;
	rp_combine_info.subpass_count = 1;
	rp_combine_info.subpasses = rp_combine_sp_info;
	rp_combine = graphics::get_renderpass(d, &rp_combine_info);

	auto vert = graphics::create_shader(d, "forward_ins.vert");
	vert->build();
	auto frag = graphics::create_shader(d, "forward_ins.frag");
	frag->build();

	p_forward_ins = create_pipeline(d);
	p_forward_ins->set_renderpass(rp, 0);
	p_forward_ins->set_vertex_attributes({ {
		graphics::VertexAttributeFloat3,
		graphics::VertexAttributeFloat2,
		graphics::VertexAttributeFloat3
	} });
	p_forward_ins->set_size(res);
	p_forward_ins->set_depth_test(true);
	p_forward_ins->set_depth_write(true);
	p_forward_ins->add_shader(vert);
	p_forward_ins->add_shader(frag);
	p_forward_ins->build_graphics();

	for (auto i = 0; i < 2; i++)
	{
		ds_matrix[i] = d->dp->create_descriptorset(p_forward_ins, 1);
		ds_matrix[i]->set_uniformbuffer(0, 0, ubo_matrix->v[0]);
		ds_matrix[i]->set_uniformbuffer(1, 0, ubo_ins->v[i]);

		ds_forward_ins[i] = d->dp->create_descriptorset(p_forward_ins, 0);
		ds_forward_ins[i]->set_uniformbuffer(1, 0, ubo_light->v[i]);
	}

	auto vert_godray = graphics::create_shader(d, "fullscreen.vert");
	vert_godray->add_define("USE_UV");
	vert_godray->build();
	auto frag_godray = graphics::create_shader(d, "godray.frag");
	frag_godray->build();

	p_godray = create_pipeline(d);
	p_godray->set_renderpass(rp_godray, 0);
	p_godray->set_size(res);
	p_godray->set_cull_mode(graphics::CullModeNone);
	p_godray->set_depth_test(false);
	p_godray->set_depth_write(false);
	p_godray->add_shader(vert_godray);
	p_godray->add_shader(frag_godray);
	p_godray->set_blend_state(0, true, graphics::BlendFactorOne, graphics::BlendFactorOne,
		graphics::BlendFactorOne, graphics::BlendFactorOne);
	p_godray->build_graphics();

	ds_godray = d->dp->create_descriptorset(p_godray, 0);
	ds_godray->set_uniformbuffer(0, 0, ubo_godrayparm->v[0]);
	auto sampler = graphics::create_sampler(d, graphics::FilterLinear, graphics::FilterLinear,
		false);
	ds_godray->set_imageview(1, 0, img_v_sky, sampler);

	auto sb = create_buffer(d, vb->size + ib->size, graphics::BufferUsageTransferSrc,
		graphics::MemPropHost | graphics::MemPropHostCoherent);
	sb->map();
	{
		auto c = d->gcp->create_commandbuffer();
		c->begin(true);
		memcpy(sb->mapped, m->vertex_buffers[0].pVertex, vb->size);
		graphics::BufferCopy r1 = { 0, 0, vb->size };
		c->copy_buffer(sb, vb, 1, &r1);
		memcpy((unsigned char*)sb->mapped + vb->size, m->pIndices, ib->size);
		graphics::BufferCopy r2 = { vb->size, 0, ib->size };
		c->copy_buffer(sb, ib, 1, &r2);
		c->end();
		d->gq->submit(c, nullptr, nullptr);
		d->gq->wait_idle();
		d->gcp->destroy_commandbuffer(c);
	}
	sb->unmap();
	destroy_buffer(sb);

	auto dst_img_ui_idx = ui->find_and_set_img_view(img_dst->get_view());

	auto gizmo = UI::create_gizmo();

	make_cbs(ins_idx);

	sm->run([&](){
		ins_update_t += sm->elapsed_time;

		{
			if (dot(camera.view_dir, sun_dir) > 0.f)
			{
				auto p = camera.proj_view * Vec4(sun_dir, 0.f);
				if (p.w != 0.f)
					p /= p.w;
				u_godrayparm->pos = Vec2(p) * 0.5f + 0.5f;
				u_godrayparm->intensity = 1.f;
			}
			else
				u_godrayparm->intensity = 0.f;
			ubo_godrayparm->update();
		}

		ui->begin(s->size.x, s->size.y, sm->elapsed_time);

		ui->begin_mainmenu();
		if (ui->begin_menu("Demo"))
		{
			if (ui->menuitem("Create Object"))
				create_obj(camera.pos);
			if (ui->menuitem("Center Camera"))
			{
				AABB aabb;
				aabb.reset();
				for (auto &o : objs)
					aabb.merge(o->aabb);
				auto c = aabb.center();
				Vec3 p[8];
				aabb.get_points(p);

				if (c == camera.pos)
					camera.pos += Vec3(1.f, 0.f, 0.f);

				auto v = c - camera.pos;
				v.normalize();
				auto s = cross(v, Vec3(0.f, 1.f, 0.f));
				s.normalize();
				auto u = cross(s, v);
				u.normalize();
				auto m = Mat3(s, u, -v);
				m.transpose();
				aabb.reset();
				for (auto i = 0; i < 8; i++)
				{
					p[i] = m * p[i];
					aabb.merge(p[i]);
				}

				auto z = aabb.depth();
				auto tanHfFovy = tan(camera.fovy * ANG_RAD / 2.f);
				z = max(z, aabb.height() / tanHfFovy);
				z = max(z, aabb.width() / camera.aspect / tanHfFovy);
				if (z > camera.zFar)
					z = camera.zFar;
				camera.view_dir = v;
				camera.x_dir = s;
				camera.up_dir = u;
				camera.calc_ang();
				camera.pos = c + -v * (z / 2.f);
				camera.update_view();
				camera.update_frustum_plane();
				ubo_matrix->update(sizeof(Mat4), sizeof(Mat4), &camera.view);
			}
			if (ui->menuitem("Day", nullptr, day))
			{
				if (!day)
				{
					cell_array->mtx.lock();
					cell_array->clear_lits();
					for (auto &l : lits)
						delete l;
					lits.clear();
					cell_array->mtx.unlock();

					need_update_ins = true;
					day = true;
				}
			}
			if (ui->menuitem("Night", nullptr, !day))
			{
				if (day)
				{
					cell_array->mtx.lock();
					for (auto &o : objs)
					{
						auto l = new ThreeDWorld::PointLight;
						l->pos = Vec3(
							(rand() % 20) - 10,
							(rand() % 20) - 10,
							(rand() % 20) - 10) + o->pos;
						l->color = Vec3(
							(rand() % 256) / 255.f,
							(rand() % 256) / 255.f,
							(rand() % 256) / 255.f);
						lits.push_back(l);
						cell_array->add_lit(l);
					}
					cell_array->mtx.unlock();

					need_update_ins = true;
					day = false;
				}
			}
			if (ui->menuitem("Information"))
			{
				ui->add_message_dialog("Info", "Press 'W' 'A' 'S' 'D' and right click to move camera\n"
					"Press '1' '2' '3' '4' to change tools");
			}
			ui->end_menu();
		}
		auto menu_rect = ui->get_curr_window_rect();
		ui->end_mainmenu();

		ui->begin_plain_window("bg", Vec2(0.f, menu_rect.max.y), Vec2(res), true);
		ui->image(dst_img_ui_idx, Vec2(res));
		auto img_rect = ui->get_last_item_rect();

		if (!gizmo->is_using() && ui->is_last_item_hovered()
			&& s->just_down_M(0))
		{
			d->gq->submit(cb_sel[ins_idx], nullptr, nullptr);
			d->gq->wait_idle();

			cb_get->begin();
			cb_get->change_image_layout(img_sel,
				graphics::ImageLayoutShaderReadOnly, graphics::ImageLayoutTransferSrc);
			graphics::BufferTextureCopy cpy;
			cpy.buffer_offset = 0;
			cpy.image_width = 1;
			cpy.image_height = 1;
			cpy.image_x = s->mouse_pos.x - img_rect.min.x;
			cpy.image_y = s->mouse_pos.y - img_rect.min.y;
			cpy.image_level = 0;
			cb_get->copy_image_to_buffer(img_sel, sel_buf, 1, &cpy);
			cb_get->change_image_layout(img_sel,
				graphics::ImageLayoutTransferSrc, graphics::ImageLayoutAttachment);
			cb_get->end();

			d->gq->submit(cb_get, nullptr, nullptr);
			d->gq->wait_idle();

			uchar bytes[4];
			memcpy(bytes, sel_buf->mapped, 4);
			auto id = bytes[0] +
				(bytes[1] << 8) +
				(bytes[2] << 16) +
				(bytes[3] << 24);
			if (id == 0)
				sel_obj = nullptr;
			else
				sel_obj = screen_objs[ins_idx][id - 1];

			need_update_ins = true;
		}

		if (s->just_down_K(Key_1))
			curr_tool = 0;
		if (s->just_down_K(Key_2))
			curr_tool = 1;
		if (s->just_down_K(Key_3))
			curr_tool = 2;
		if (s->just_down_K(Key_4))
			curr_tool = 3;

		if (curr_tool > 0 && sel_obj && sel_obj->show_idx != -1)
		{
			UI::Gizmo::TransType type;
			switch (curr_tool)
			{
			case 1:
				type = UI::Gizmo::TransMove;
				break;
			case 2:
				type = UI::Gizmo::TransRotate;
				break;
			case 3:
				type = UI::Gizmo::TransScale;
				break;

			}
			if (gizmo->show(ui, type, &camera, sel_obj))
			{
				sel_obj->update_mat();
				ubo_ins->update(sizeof(Mat4) * sel_obj->show_idx, sizeof(Mat4), &sel_obj->mat);
			}
		}

		ui->end_window();

		auto dl_ol = ui->get_overlap_drawlist();
		dl_ol.add_text(Vec2(0.f, s->size.y - 20.f), Vec4(1.f), "%5.2f %5.2f %5.2f",
			camera.pos.x, camera.pos.y, camera.pos.z);

		ui->end();

		ubo_godrayparm->flush(cb_update, 0);

		auto index = sc->acquire_image(image_avalible);

		cb_ui->begin();
		ui->record_commandbuffer(cb_ui, sc->get_rp(true), sc->get_fb(index));
		cb_ui->end();
	});

	return 0;
}

