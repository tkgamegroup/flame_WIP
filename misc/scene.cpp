namespace flame
{
	namespace _3d
	{
		struct BakeUnit
		{
			Vec3 pos;
			Vec3 normal;
			Vec3 up;
			Ivec2 pixel_coord;
		};

		struct RegisteredModel
		{
			std::vector<BakeUnit> bk_units;
		};

		struct ScenePrivate : Scene
		{
			float bk_ratio;
			Ivec2 bk_imgsize;

			Ivec2 bk_pen_pos;
			int bk_pen_lineheight;

			graphics::Image *bk_img;
			graphics::Descriptorset *ds_lightmap;
			graphics::Descriptorset *ds_pbribl;
		};

		static const int bake_wnd_size = 128;
		static Mat4 bake_window_proj;

		void ShareData::create(graphics::Device *_d)
		{
			auto hf_bake_wnd_size = bake_wnd_size / 2;
			auto n = Vec3(0.f, 0.f, -1.f);
			auto sum = 0.f;

			bk_fix_center = new float[bake_wnd_size * bake_wnd_size];
			for (auto y = 0; y < bake_wnd_size; y++)
			{
				for (auto x = 0; x < bake_wnd_size; x++)
				{
					auto vn = dot(Vec3((x + 0.5f - hf_bake_wnd_size) / hf_bake_wnd_size, (-y - 0.5f + hf_bake_wnd_size) / hf_bake_wnd_size, -1.f).get_normalized(), n);
					auto value = vn * vn;
					sum += value;
					bk_fix_center[y * bake_wnd_size + x] = value;
				}
			}

			bk_fix_left = new float[hf_bake_wnd_size * bake_wnd_size];
			for (auto y = 0; y < bake_wnd_size; y++)
			{
				for (auto x = 0; x < hf_bake_wnd_size; x++)
				{
					auto vn = dot(Vec3((x + 0.5f) / hf_bake_wnd_size, (-y - 0.5f + hf_bake_wnd_size) / hf_bake_wnd_size, -1.f).get_normalized(), n);
					auto dn = dot(Vec3(-1.f, (-y - 0.5f + hf_bake_wnd_size) / hf_bake_wnd_size, (-x - 0.5f) / hf_bake_wnd_size).get_normalized(), n);
					auto value = vn * dn;
					sum += value;
					bk_fix_left[y * hf_bake_wnd_size + x] = value;
				}
			}

			bk_fix_right = new float[hf_bake_wnd_size * bake_wnd_size];
			for (auto y = 0; y < bake_wnd_size; y++)
			{
				for (auto x = 0; x < hf_bake_wnd_size; x++)
				{
					auto vn = dot(Vec3((x + 0.5f - hf_bake_wnd_size) / hf_bake_wnd_size, (-y - 0.5f + hf_bake_wnd_size) / hf_bake_wnd_size, -1.f).get_normalized(), n);
					auto dn = dot(Vec3(1.f, (-y - 0.5f + hf_bake_wnd_size) / hf_bake_wnd_size, (x + 0.5f - hf_bake_wnd_size) / hf_bake_wnd_size).get_normalized(), n);
					auto value = vn * dn;
					sum += value;
					bk_fix_right[y * hf_bake_wnd_size + x] = value;
				}
			}

			bk_fix_top = new float[bake_wnd_size * hf_bake_wnd_size];
			for (auto y = 0; y < hf_bake_wnd_size; y++)
			{
				for (auto x = 0; x < bake_wnd_size; x++)
				{
					auto vn = dot(Vec3((x + 0.5f - hf_bake_wnd_size) / hf_bake_wnd_size, (-y - 0.5f) / hf_bake_wnd_size, -1.f).get_normalized(), n);
					auto dn = dot(Vec3((x + 0.5f - hf_bake_wnd_size) / hf_bake_wnd_size, 1.f, (-y - 0.5f) / hf_bake_wnd_size).get_normalized(), n);
					auto value = vn * dn;
					sum += value;
					bk_fix_top[y * bake_wnd_size + x] = value;
				}
			}

			bk_fix_bottom = new float[bake_wnd_size * hf_bake_wnd_size];
			for (auto y = 0; y < hf_bake_wnd_size; y++)
			{
				for (auto x = 0; x < bake_wnd_size; x++)
				{
					auto vn = dot(Vec3((x + 0.5f - hf_bake_wnd_size) / hf_bake_wnd_size, (-y - 0.5f + hf_bake_wnd_size) / hf_bake_wnd_size, -1.f).get_normalized(), n);
					auto dn = dot(Vec3((x + 0.5f - hf_bake_wnd_size) / hf_bake_wnd_size, -1.f, (y + 0.5f - hf_bake_wnd_size) / hf_bake_wnd_size).get_normalized(), n);
					auto value = vn * dn;
					sum += value;
					bk_fix_bottom[y * bake_wnd_size + x] = value;
				}
			}

			for (auto y = 0; y < bake_wnd_size; y++)
			{
				for (auto x = 0; x < bake_wnd_size; x++)
					bk_fix_center[y * bake_wnd_size + x] /= sum;
			}

			for (auto y = 0; y < bake_wnd_size; y++)
			{
				for (auto x = 0; x < hf_bake_wnd_size; x++)
					bk_fix_left[y * hf_bake_wnd_size + x] /= sum;
			}

			for (auto y = 0; y < bake_wnd_size; y++)
			{
				for (auto x = 0; x < hf_bake_wnd_size; x++)
					bk_fix_right[y * hf_bake_wnd_size + x] /= sum;
			}

			for (auto y = 0; y < hf_bake_wnd_size; y++)
			{
				for (auto x = 0; x < bake_wnd_size; x++)
					bk_fix_top[y * bake_wnd_size + x] /= sum;
			}

			for (auto y = 0; y < hf_bake_wnd_size; y++)
			{
				for (auto x = 0; x < bake_wnd_size; x++)
					bk_fix_bottom[y * bake_wnd_size + x] /= sum;
			}
		}

		inline ScenePrivate::ScenePrivate(const Ivec2 &resolution)
		{
			res = resolution;
			show_mode = ShowModeLightmap;
			show_frame = true;

			c = nullptr;

			bk_ratio = 0.f;
			bk_imgsize = Ivec2(0);
			bk_pen_pos = Ivec2(0);
			bk_pen_lineheight = 0;

			matrix_buf = graphics::Buffer::create(share_data.d, sizeof(Mat4) * 2 + sizeof(Vec4), graphics::BufferUsageUniform, graphics::MemPropHost);
			matrix_buf->map();

			col_image = graphics::Image::create(share_data.d, graphics::Format_R32G32B32A32_SFLOAT, resolution, 1, 1, graphics::SampleCount_1, graphics::ImageUsageSampled | graphics::ImageUsageAttachment, graphics::MemPropDevice);
			dep_image = graphics::Image::create(share_data.d, graphics::Format_Depth16, resolution, 1, 1, graphics::SampleCount_1, graphics::ImageUsageSampled | graphics::ImageUsageAttachment, graphics::MemPropDevice);

			bk_img = nullptr;

			graphics::FramebufferInfo fb_info;
			fb_info.rp = share_data.rp_scene;
			fb_info.views.resize(2);
			fb_info.views[0] = graphics::Imageview::get(col_image);
			fb_info.views[1] = graphics::Imageview::get(dep_image);
			framebuffer = graphics::Framebuffer::get(share_data.d, fb_info);

			clear_values = graphics::ClearValues::create(share_data.rp_scene);
			clear_values->set(0, Bvec4(0, 0, 0, 255));

			ds_skybrightsun = graphics::Descriptorset::create(share_data.d->dp, share_data.pl_sky_brightsun->layout()->dsl(0));
			ds_skybrightsun->set_uniformbuffer(0, 0, matrix_buf);
			
			ds_lightmap = graphics::Descriptorset::create(share_data.d->dp, share_data.pl_lightmap->layout()->dsl(0));
			ds_lightmap->set_uniformbuffer(0, 0, matrix_buf);

			ds_cameralight = graphics::Descriptorset::create(share_data.d->dp, share_data.pl_cameralight->layout()->dsl(0));
			ds_cameralight->set_uniformbuffer(0, 0, matrix_buf);

			ds_frame = graphics::Descriptorset::create(share_data.d->dp, share_data.pl_frame->layout()->dsl(0));
			ds_frame->set_uniformbuffer(0, 0, matrix_buf);

			cb = graphics::Commandbuffer::create(share_data.d->gcp);
		}

		inline void ScenePrivate::set_bake_props(float ratio, const Ivec2 &imgsize)
		{
			bk_ratio = ratio;
			bk_imgsize = imgsize;

			if (bk_img)
				graphics::Image::destroy(bk_img);
			bk_img = graphics::Image::create(share_data.d, graphics::Format_R32G32B32A32_SFLOAT, bk_imgsize, 1, 1, graphics::SampleCount_1, graphics::ImageUsageSampled | graphics::ImageUsageTransferDst, graphics::MemPropDevice);
			//r.bk_img->init(Bvec4(200, 100, 50, 255));
			bk_img->init(Bvec4(0, 0, 0, 255));
			//for (auto y = 0; y < m->bk_imgsize_.y; y++)
			//{
			//	for (auto x = 0; x < m->bk_imgsize_.x; x++)
			//	{
			//		auto b_or_w = (y % 2 == 0) ^ (x % 2 == 0);
			//		r.bk_img->set_pixel_col(x, y, b_or_w == 0 ? Bvec4(0, 0, 0, 255) : Bvec4(255, 255, 255, 255));
			//	}
			//}
			ds_lightmap->set_imageview(1, 0, graphics::Imageview::get(bk_img), share_data.d->sp_bi_linear);

			bake_window_proj = get_proj_mat(ANG_RAD * 90.f, 1.f, 1.f / ratio * 0.5f, 1000.f);

			bk_pen_pos = Ivec2(0);
			for (auto &r : ms)
			{
				r.bk_units.clear();

				std::vector<Vec2> uvs;

				for (auto &p : r.m->prims)
				{
					switch (p.pt)
					{
					case PrimitiveTopologyPlane:
					{
						auto normal = -cross(p.vx, p.vz).get_normalized();

						auto x_ext = p.vx.length();
						auto z_ext = p.vz.length();
						auto nx = p.vx.get_normalized();
						auto nz = p.vz.get_normalized();

						auto isize = Ivec2(ceil(x_ext * bk_ratio), ceil(z_ext * bk_ratio));

						auto uv0 = Vec2(bk_pen_pos) / Vec2(bk_imgsize);
						auto uv2 = uv0 + Vec2(x_ext * bk_ratio, z_ext * bk_ratio) / Vec2(bk_imgsize);
						auto uv1 = Vec2(uv2.x, uv0.y);
						auto uv3 = Vec2(uv0.x, uv2.y);

						uvs.push_back(uv0);
						uvs.push_back(uv2);
						uvs.push_back(uv1);
						uvs.push_back(uv0);
						uvs.push_back(uv3);
						uvs.push_back(uv2);

						bk_pen_lineheight = max(bk_pen_lineheight, isize.y);
						assert(bk_pen_pos.y + bk_pen_lineheight < bk_imgsize.y);
						if (bk_pen_pos.x + isize.x > bk_imgsize.x)
						{
							bk_pen_pos.x = 0;
							bk_pen_pos.y += bk_pen_lineheight;
							bk_pen_lineheight = 0;
						}

						auto i = r.bk_units.size();
						r.bk_units.resize(r.bk_units.size() + isize.x * isize.y);
						for (auto x = 0; x < isize.x; x++)
						{
							for (auto z = 0; z < isize.y; z++)
							{
								BakeUnit u;
								u.pos = nx * ((x + 0.5f) / bk_ratio) + nz * ((z + 0.5f) / bk_ratio) + p.p;
								u.normal = normal;
								u.up = -nz;
								u.pixel_coord = bk_pen_pos + Ivec2(x, z);
								r.bk_units[i] = u;
								i++;
							}
						}

						bk_pen_pos.x += isize.x;
					}
						break;
					}
				}

				if (r.uv_buf)
					graphics::Buffer::destroy(r.uv_buf);
				r.uv_buf = graphics::Buffer::create(share_data.d, uvs.size() * sizeof(Vec2), graphics::BufferUsageTransferDst | graphics::BufferUsageVertex, graphics::MemPropDevice, false, uvs.data());
			}
		}

		void ScenePrivate::draw_scene(graphics::Commandbuffer *cb, const Vec2 &camera_props)
		{
			cb->bind_pipeline(share_data.pl_sky_brightsun);
			cb->bind_descriptorset(ds_skybrightsun, 0);
			cb->push_constant(0, sizeof(Vec2), &camera_props);
			cb->draw(3, 1, 0, 0);

			cb->bind_pipeline(share_data.pl_lightmap);
			cb->bind_descriptorset(ds_lightmap, 0);
			for (auto &m : ms)
			{
				cb->bind_vertexbuffer(m.pos_buf, 0);
				cb->bind_vertexbuffer(m.uv_buf, 1);
				cb->draw(m.vc, 1, 0, 0);
			}
		}

		inline void ScenePrivate::record_cb()
		{
			cb->begin();
			cb->begin_renderpass(share_data.rp_scene, framebuffer, clear_values);

			auto vp = Rect(Vec2(0.f), Vec2(res));
			cb->set_viewport(vp);
			cb->set_scissor(vp);

			switch (show_mode)
			{
			case ShowModeLightmap:
				draw_scene(cb, Vec2(0.5773503f, (float)res.x / (float)res.y));
				break;
			case ShowModeCameraLight:
				cb->bind_pipeline(share_data.pl_sky_blue);
				cb->draw(3, 1, 0, 0);

				cb->bind_pipeline(share_data.pl_cameralight);
				cb->bind_descriptorset(ds_cameralight, 0);
				for (auto &m : ms)
				{
					cb->bind_vertexbuffer(m.pos_buf, 0);
					cb->bind_vertexbuffer(m.normal_buf, 1);
					cb->draw(m.vc, 1, 0, 0);
				}
				break;
			}

			if (show_frame)
			{
				cb->bind_pipeline(share_data.pl_frame);
				cb->bind_descriptorset(ds_frame, 0);
				for (auto &m : ms)
				{
					cb->bind_vertexbuffer(m.frame_buf, 0);
					cb->draw(m.vc_frame, 1, 0, 0);
				}
			}

			cb->end_renderpass();
			cb->end();
		}

		inline void ScenePrivate::bake(int pass)
		{
			auto cb = graphics::Commandbuffer::create(share_data.d->gcp);
			auto img_col = graphics::Image::create(share_data.d, graphics::Format_R32G32B32A32_SFLOAT, Ivec2(bake_wnd_size), 1, 1, graphics::SampleCount_1, graphics::ImageUsageSampled | graphics::ImageUsageAttachment | graphics::ImageUsageTransferSrc, graphics::MemPropDevice);
			auto img_dep = graphics::Image::create(share_data.d, graphics::Format_Depth16, Ivec2(bake_wnd_size), 1, 1, graphics::SampleCount_1, graphics::ImageUsageSampled | graphics::ImageUsageAttachment, graphics::MemPropDevice);
			graphics::FramebufferInfo fb_info;
			fb_info.rp = share_data.rp_scene;
			fb_info.views.resize(2);
			fb_info.views[0] = graphics::Imageview::get(img_col);
			fb_info.views[1] = graphics::Imageview::get(img_dep);
			auto fb = graphics::Framebuffer::get(share_data.d, fb_info);
			auto img_sum = graphics::Image::create(share_data.d, graphics::Format_R32G32B32A32_SFLOAT, Ivec2(1), 1, 1, graphics::SampleCount_1, graphics::ImageUsageStorage | graphics::ImageUsageTransferSrc, graphics::MemPropDevice);

			auto total = 0;
			for (auto &m : ms)
				total += m.bk_units.size();

			memcpy(matrix_buf->mapped, &bake_window_proj, sizeof(Mat4));
			auto hf_bake_wnd_size = bake_wnd_size / 2;
			auto wnd_xy = bake_wnd_size * bake_wnd_size;
			auto img_xy = bk_img->size.x * bk_img->size.y;
			auto stag = new Vec4[img_xy];
			auto pixels = new Vec4[wnd_xy];

			for (auto i_pass = 0; i_pass < pass; i_pass++)
			{
				printf("pass: %d/%d\n", i_pass + 1, pass);

				memset(stag, 0, img_xy * sizeof(Vec4));

				auto i = 0;
				for (auto &m : ms)
				{
					for (auto &u : m.bk_units)
					{
						auto col = Vec3(0.f);

						// front
						{
							auto view = get_view_mat(u.pos, u.pos + u.normal, u.up);
							memcpy((char*)matrix_buf->mapped + sizeof(Mat4), &view, sizeof(Mat4));
							matrix_buf->flush();

							cb->begin();
							cb->begin_renderpass(share_data.rp_scene, fb, clear_values);

							auto vp = Rect(Vec2(0.f), Vec2(bake_wnd_size));
							cb->set_viewport(vp);
							cb->set_scissor(vp);

							draw_scene(cb, Vec2(1.f));

							cb->end_renderpass();
							cb->end();
							share_data.d->gq->submit(cb, nullptr, nullptr);
							share_data.d->gq->wait_idle();

							img_col->get_pixels(pixels);
							for (auto y = 0; y < bake_wnd_size; y++)
							{
								for (auto x = 0; x < bake_wnd_size; x++)
								{
									auto idx = y * bake_wnd_size + x;
									auto &p = pixels[idx];
									col += Vec3(p) * share_data.bk_fix_center[idx];
								}
							}

							//img_col->save_png((L"d:/baked/" + std::to_wstring(i) + L"_c.png").c_str());
						}

						// left
						{
							auto view = get_view_mat(u.pos, u.pos - cross(u.normal, u.up), u.up);
							memcpy((char*)matrix_buf->mapped + sizeof(Mat4), &view, sizeof(Mat4));
							matrix_buf->flush();

							cb->begin();
							cb->begin_renderpass(share_data.rp_scene, fb, clear_values);

							cb->set_viewport(Rect(Vec2(0.f), Vec2(bake_wnd_size)));
							cb->set_scissor(Rect(Vec2(hf_bake_wnd_size, 0.f), Vec2(bake_wnd_size)));

							draw_scene(cb, Vec2(1.f));

							cb->end_renderpass();
							cb->end();
							share_data.d->gq->submit(cb, nullptr, nullptr);
							share_data.d->gq->wait_idle();

							img_col->get_pixels(pixels);
							for (auto y = 0; y < bake_wnd_size; y++)
							{
								for (auto x = 0; x < hf_bake_wnd_size; x++)
								{
									auto &p = pixels[y * bake_wnd_size + x + hf_bake_wnd_size];
									col += Vec3(to_f32(p.x), to_f32(p.y), to_f32(p.z)) * share_data.bk_fix_left[y * hf_bake_wnd_size + x];
								}
							}

							//img_col->save_png((L"d:/baked/" + std::to_wstring(i) + L"_l.png").c_str());
						}

						// right
						{
							auto view = get_view_mat(u.pos, u.pos + cross(u.normal, u.up), u.up);
							memcpy((char*)matrix_buf->mapped + sizeof(Mat4), &view, sizeof(Mat4));
							matrix_buf->flush();

							cb->begin();
							cb->begin_renderpass(share_data.rp_scene, fb, clear_values);

							cb->set_viewport(Rect(Vec2(0.f), Vec2(bake_wnd_size)));
							cb->set_scissor(Rect(Vec2(0.f), Vec2(hf_bake_wnd_size, bake_wnd_size)));

							draw_scene(cb, Vec2(1.f));

							cb->end_renderpass();
							cb->end();
							share_data.d->gq->submit(cb, nullptr, nullptr);
							share_data.d->gq->wait_idle();

							img_col->get_pixels(pixels);
							for (auto y = 0; y < bake_wnd_size; y++)
							{
								for (auto x = 0; x < hf_bake_wnd_size; x++)
								{
									auto &p = pixels[y * bake_wnd_size + x];
									col += Vec3(p) * share_data.bk_fix_right[y * hf_bake_wnd_size + x];
								}
							}

							//img_col->save_png((L"d:/baked/" + std::to_wstring(i) + L"_r.png").c_str());
						}

						// top
						{
							auto view = get_view_mat(u.pos, u.pos + u.up, -u.normal);
							memcpy((char*)matrix_buf->mapped + sizeof(Mat4), &view, sizeof(Mat4));
							matrix_buf->flush();

							cb->begin();
							cb->begin_renderpass(share_data.rp_scene, fb, clear_values);

							cb->set_viewport(Rect(Vec2(0.f), Vec2(bake_wnd_size)));
							cb->set_scissor(Rect(Vec2(0.f, hf_bake_wnd_size), Vec2(bake_wnd_size)));

							draw_scene(cb, Vec2(1.f));

							cb->end_renderpass();
							cb->end();
							share_data.d->gq->submit(cb, nullptr, nullptr);
							share_data.d->gq->wait_idle();

							img_col->get_pixels(pixels);
							for (auto y = 0; y < hf_bake_wnd_size; y++)
							{
								for (auto x = 0; x < bake_wnd_size; x++)
								{
									auto &p = pixels[(y + hf_bake_wnd_size) * bake_wnd_size + x];
									col += Vec3(p) * share_data.bk_fix_top[y * bake_wnd_size + x];
								}
							}

							//img_col->save_png((L"d:/baked/" + std::to_wstring(i) + L"_t.png").c_str());
						}

						// bottom
						{
							auto view = get_view_mat(u.pos, u.pos - u.up, u.normal);
							memcpy((char*)matrix_buf->mapped + sizeof(Mat4), &view, sizeof(Mat4));
							matrix_buf->flush();

							cb->begin();
							cb->begin_renderpass(share_data.rp_scene, fb, clear_values);

							cb->set_viewport(Rect(Vec2(0.f), Vec2(bake_wnd_size)));
							cb->set_scissor(Rect(Vec2(0.f), Vec2(bake_wnd_size, hf_bake_wnd_size)));

							draw_scene(cb, Vec2(1.f));

							cb->end_renderpass();
							cb->end();
							share_data.d->gq->submit(cb, nullptr, nullptr);
							share_data.d->gq->wait_idle();

							img_col->get_pixels(pixels);
							for (auto y = 0; y < hf_bake_wnd_size; y++)
							{
								for (auto x = 0; x < bake_wnd_size; x++)
								{
									auto &p = pixels[y * bake_wnd_size + x];
									col += Vec3(p) * share_data.bk_fix_bottom[y * bake_wnd_size + x];
								}
							}

							//img_col->save_png((L"d:/baked/" + std::to_wstring(i) + L"_b.png").c_str());
						}

						stag[u.pixel_coord.y * bk_img->size.x + u.pixel_coord.x] = Vec4(col, 1.f);

						printf("%d/%d\r", i + 1, total);
						i++;
					}
				}
				printf("\n");

				bk_img->set_pixels(stag);
			}
		}
	}
	
	void Scene::on_update()
	{
		//	for (auto &o : objects)
		//	{
		//		if (o->physics_type & (int)ObjectPhysicsType::dynamic)
		//		{
		//			for (auto &data : o->rigidbodyDatas)
		//			{
		//				if (data->rigidbody->boneID != -1)
		//				//{
		//				//	auto solver = pObject->animationSolver;
		//				//	if (r->mode != Rigidbody::Mode::eStatic)
		//				//	{
		//				//		auto objAxisT = glm::transpose(objAxis);
		//				//		auto rigidAxis = r->getAxis();
		//				//		auto rigidAxisT = glm::transpose(rigidAxis);
		//				//		auto trans = body->getGlobalPose();
		//				//		auto coord = glm::vec3(trans.p.x, trans.p.y, trans.p.z);
		//				//		glm::mat3 axis;
		//				//		Math::quaternionToMatrix(glm::vec4(trans.q.x, trans.q.y, trans.q.z, trans.q.w), axis);
		//				//		pObject->rigidDatas[id].coord = coord;
		//				//		pObject->rigidDatas[id].rotation = axis;
		//				//		auto boneAxis = objAxisT * axis * rigidAxisT;
		//				//		glm::vec3 boneCoord;
		//				//		if (r->mode != Rigidbody::Mode::eDynamicLockLocation)
		//				//			boneCoord = (objAxisT * (coord - objCoord) - boneAxis * (r->getCoord() * objScale)) / objScale;
		//				//		else
		//				//			boneCoord = glm::vec3(solver->boneMatrix[r->boneID][3]);
		//				//		solver->boneMatrix[r->boneID] = Math::makeMatrix(boneAxis, boneCoord);
		//				//	}
		//				//}
		//			}
		//		}
		//	}
	}

	//void Scene::addObject(Object *o) // if object has dynamic physics, it can be only moved by physics
	//{
	//	// since object can move to somewhere first, we create physics component here
	//	if (o->physics_type != 0)
	//	{
	//		// create joints

	//		//		int jID = 0;
	//		//		for (auto j : pModel->joints)
	//		//		{
	//		//			if (j->rigid0ID == j->rigid1ID)
	//		//			{
	//		//				jID++;
	//		//				continue;
	//		//			}

	//		//			auto pR0 = pModel->rigidbodies[j->rigid0ID];
	//		//			auto pR1 = pModel->rigidbodies[j->rigid1ID];
	//		//			auto coord0 = (pR0->getCoord() + pModel->bones[pR0->boneID].rootCoord);
	//		//			coord0 = j->getCoord() - coord0;
	//		//			//coord0 = - coord0;
	//		//			auto coord1 = (pR1->getCoord() + pModel->bones[pR1->boneID].rootCoord);
	//		//			coord1 = j->getCoord() - coord1;
	//		//			//coord1 =  - coord1;
	//		//			auto axis = j->getAxis();
	//		//			auto axis0 = glm::transpose(pR0->getAxis());
	//		//			auto axis1 = glm::transpose(pR1->getAxis());
	//		//			auto t = PxTransform(PxQuat(PxMat33(
	//		//				PxVec3(axis[0][0], axis[0][1], axis[0][2]),
	//		//				PxVec3(axis[1][0], axis[1][1], axis[1][2]),
	//		//				PxVec3(axis[2][0], axis[2][1], axis[2][2]))));
	//		//			auto t0 = PxTransform(PxQuat(PxMat33(
	//		//				PxVec3(axis0[0][0], axis0[0][1], axis0[0][2]),
	//		//				PxVec3(axis0[1][0], axis0[1][1], axis0[1][2]),
	//		//				PxVec3(axis0[2][0], axis0[2][1], axis0[2][2]))));
	//		//			auto t1 = PxTransform(PxQuat(PxMat33(
	//		//				PxVec3(axis1[0][0], axis1[0][1], axis1[0][2]),
	//		//				PxVec3(axis1[1][0], axis1[1][1], axis1[1][2]),
	//		//				PxVec3(axis1[2][0], axis1[2][1], axis1[2][2]))));
	//		//			//auto j = PxSphericalJointCreate(*physics, (PxRigidActor*)pR0->phyActor, t * PxTransform(PxVec3(coord0.x, coord0.y, coord0.z)),
	//		//			//	(PxRigidActor*)pR1->phyActor, t * PxTransform(PxVec3(coord1.x, coord1.y, coord1.z)));
	//		//			auto p = PxD6JointCreate(*pxPhysics, (PxRigidActor*)pR0->phyActor, t0 * PxTransform(PxVec3(coord0.x, coord0.y, coord0.z)) * t,
	//		//				(PxRigidActor*)pR1->phyActor, t1 * PxTransform(PxVec3(coord1.x, coord1.y, coord1.z)) * t);
	//		//			p->setConstraintFlag(PxConstraintFlag::Enum::eCOLLISION_ENABLED, true);
	//		//			p->setSwingLimit(PxJointLimitCone(PxPi / 4, PxPi / 4));
	//		//			p->setMotion(PxD6Axis::eSWING1, PxD6Motion::eLIMITED);
	//		//			p->setMotion(PxD6Axis::eSWING1, PxD6Motion::eLIMITED);
	//		//			//auto p = PxSphericalJointCreate(*physics, (PxRigidActor*)pR0->phyActor, PxTransform(PxVec3(coord0.x, coord0.y, coord0.z), PxQuat(PxMat33(
	//		//			//	PxVec3(axis1[0][0], axis1[0][1], axis1[0][2]),
	//		//			//	PxVec3(axis1[1][0], axis1[1][1], axis1[1][2]),
	//		//			//	PxVec3(axis1[2][0], axis1[2][1], axis1[2][2])))),
	//		//			//	(PxRigidActor*)pR1->phyActor, PxTransform(PxVec3(coord1.x, coord1.y, coord1.z), PxQuat(PxMat33(
	//		//			//		PxVec3(axis1[0][0], axis1[0][1], axis1[0][2]),
	//		//			//		PxVec3(axis1[1][0], axis1[1][1], axis1[1][2]),
	//		//			//		PxVec3(axis1[2][0], axis1[2][1], axis1[2][2])))));

	//		//			//break;
	//		//			//if (jID == 0)
	//		//			//p->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);
	//		//			jID++;
	//		//		}
	//	}
	//}

	//int Scene::getCollisionGroupID(int ID, unsigned int mask)
	//{
	//	auto count = pCollisionGroups.size();
	//	for (int i = 0; i < count; i++)
	//	{
	//		if (pCollisionGroups[i]->originalID == ID && pCollisionGroups[i]->originalmask == mask)
	//			return i;
	//	}
	//	auto c = new CollisionGroup;
	//	c->originalID = ID;
	//	c->originalmask = mask;
	//	pCollisionGroups.push_back(c);
	//	return count;
	//}
}

