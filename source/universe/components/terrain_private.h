#pragma once

#include "terrain.h"
#include "node_private.h"

namespace flame
{
	struct cTerrainPrivate : cTerrain, NodeDrawer
	{
		vec2 extent = vec2(100.f);
		uint blocks = 64;
		uint tess_levels = 32.f;

		std::string height_map_name;
		std::string material_name;

		graphics::Image* height_texture = nullptr;
		std::unique_ptr<graphics::Image> normal_texture;
		std::unique_ptr<graphics::Image> tangent_texture;
		graphics::Material* material = nullptr;
		int height_map_id = -1;
		int normal_map_id = -1;
		int tangent_map_id = -1;
		int material_id = -1;

		vec2 get_extent() const override { return extent; }
		void set_extent(const vec2& ext) override;
		uint get_blocks() const override { return blocks; }
		void set_blocks(uint b) override;
		uint get_tess_levels() const override { return tess_levels; }
		void set_tess_levels(uint l) override;

		const char* get_height_map() const override { return height_map_name.c_str(); }
		void set_height_map(std::string_view name);
		void set_height_map(const char* name) override { set_height_map(std::string(name)); }
		const char* get_material_name() const override { return material_name.c_str(); }
		void set_material_name(std::string_view name);
		void set_material_name(const char* name) override { set_material_name(std::string(name)); }

		void draw(sRendererPtr s_renderer, bool, bool) override;

		void on_added() override;
		void on_removed() override;
		void on_entered_world() override;
		void on_left_world() override;
	};
}
