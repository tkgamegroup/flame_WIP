namespace flame
{
	struct WaterShaderStruct
	{
		glm::vec3 coord;
		int block_cx;
		int block_cy;
		float block_size;
		float height;
		float tessellation_factor;
		float tiling_scale;
		float mapDimension;
		unsigned int dummy0;
		unsigned int dummy1;
	};

	static Pipeline *scattering_pipeline;
	static Pipeline *convolve_pipeline;
	static Pipeline *water_pipeline;

	DeferredRenderer::DeferredRenderer(bool _enable_shadow, DisplayLayer *_dst) :
	{
		if (!defe_inited)
		{
			scattering_pipeline = new Pipeline(PipelineInfo()
				.set_cx(512).set_cy(256)
				.set_cull_mode(VK_CULL_MODE_NONE)
				.add_shader("fullscreen.vert", { "USE_UV" })
				.add_shader("sky/scattering.frag", {}),
				renderpass_color16.get(), 0);
			convolve_pipeline = new Pipeline(PipelineInfo()
				.set_cull_mode(VK_CULL_MODE_NONE)
				.add_shader("fullscreen.vert", { "USE_UV" })
				.add_shader("sky/convolve.frag", {}),
				renderpass_color16.get(), 0, true);
			water_pipeline = new Pipeline(PipelineInfo()
				.set_patch_control_points(4)
				.set_depth_test(true)
				.set_depth_write(true)
				.set_primitive_topology(VK_PRIMITIVE_TOPOLOGY_PATCH_LIST)
				.add_shader("deferred/water.vert", {})
				.add_shader("deferred/water.tesc", {})
				.add_shader("deferred/water.tese", {})
				.add_shader("deferred/water.frag", {})
				renderpass_defe.get(), 0);
		}
	}

	void DeferredRenderer::render(Scene *scene, CameraComponent *camera)
	{
		if (sky_dirty)
		{
			switch (scene->get_sky_type())
			{
				case SkyTypeAtmosphereScattering:
				{
					auto as = (SkyAtmosphereScattering*)scene->get_sky();

					auto cb = begin_once_command_buffer();
					auto fb = get_framebuffer(envrImage.get(), renderpass_color16.get());

					cb->begin_renderpass(renderpass_color16.get(), fb.get());
					cb->bind_pipeline(scattering_pipeline);
					auto dir = -as->sun_light->get_parent()->get_axis()[2];
					cb->push_constant(VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(dir), &dir);
					cb->draw(3);
					cb->end_renderpass();

					end_once_command_buffer(cb);

					break;
				}
				case SkyTypePanorama:
				{
					auto pa = (SkyPanorama*)scene->get_sky();

					if (pa->panoImage)
					{
						auto cb = begin_once_command_buffer();
						auto fb = get_framebuffer(envrImage.get(), renderpass_color16.get());

						cb->begin_renderpass(renderpass_color16.get(), fb.get());
						cb->bind_pipeline(copy_pipeline);
						cb->set_viewport_and_scissor(EnvrSizeCx, EnvrSizeCy);
						updateDescriptorSets(1, &copy_pipeline->descriptor_set->get_write(0, 0, &get_texture_info(pa->panoImage.get(), colorSampler)));
						cb->bind_descriptor_set();
						cb->draw(3);
						cb->end_renderpass();

						end_once_command_buffer(cb);
					}
					break;
				}
			}

			sky_dirty = false;
		}

		if (waters.get_size() > 0)
		{
			std::vector<VkBufferCopy> ranges;
			defalut_staging_buffer->map(0, sizeof(WaterShaderStruct) * waters.get_size());
			auto map = (unsigned char*)defalut_staging_buffer->mapped;

			waters.iterate([&](int index, void *p, bool &remove) {
				auto w = (WaterComponent*)p;
				auto n = w->get_parent();
				if (water_auxes[index].attribute_updated_frame < n->get_transform_dirty_frame() || 
					water_auxes[index].attribute_updated_frame < w->get_attribute_dirty_frame())
				{
					auto srcOffset = sizeof(WaterShaderStruct) * ranges.size();
					WaterShaderStruct stru;
					stru.coord = glm::vec3(n->get_world_matrix()[3]);
					stru.block_cx = w->get_block_cx();
					stru.block_cy = w->get_block_cy();
					stru.block_size = w->get_block_size();
					stru.height = w->get_height();
					stru.tessellation_factor = w->get_tessellation_factor();
					stru.tiling_scale = w->get_tiling_scale();
					stru.mapDimension = 1024;
					memcpy(map + srcOffset, &stru, sizeof(WaterShaderStruct));
					VkBufferCopy range = {};
					range.srcOffset = srcOffset;
					range.dstOffset = sizeof(WaterShaderStruct) * index;
					range.size = sizeof(WaterShaderStruct);
					ranges.push_back(range);

					water_auxes[index].attribute_updated_frame = total_frame_count;
				}
				return true;
			});
			defalut_staging_buffer->unmap();
			defalut_staging_buffer->copy_to(waterBuffer.get(), ranges.size(), ranges.data());
		}

		// mrt
		// water
		if (waters.get_size() > 0)
		{
			cb_defe->bind_pipeline(water_pipeline);
			cb_defe->bind_descriptor_set(&ds_water->v);
			waters.iterate([&](int index, void *p, bool &remove) {
				auto w = (WaterComponent*)p;
				cb_defe->draw(4, 0, (index << 16) + w->get_block_cx() * w->get_block_cx());
				return true;
			});
		}
	}
}
