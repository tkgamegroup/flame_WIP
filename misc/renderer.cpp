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
	}
}
