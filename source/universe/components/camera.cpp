namespace flame
{
	void cCameraPrivate::get_points(vec3* dst, float n, float f)
	{
		if (n < 0.f)
			n = near;
		if (f < 0.f)
			f = far;

		auto tan_hf_fovy = tan(radians(fovy * 0.5f));

		auto y1 = n * tan_hf_fovy;
		auto y2 = f * tan_hf_fovy;
		auto x1 = y1 * aspect;
		auto x2 = y2 * aspect;

		dst[0] = view_inv * vec4(-x1, y1, -n, 1.f);
		dst[1] = view_inv * vec4(x1, y1, -n, 1.f);
		dst[2] = view_inv * vec4(x1, -y1, -n, 1.f);
		dst[3] = view_inv * vec4(-x1, -y1, -n, 1.f);

		dst[4] = view_inv * vec4(-x2, y2, -f, 1.f);
		dst[5] = view_inv * vec4(x2, y2, -f, 1.f);
		dst[6] = view_inv * vec4(x2, -y2, -f, 1.f);
		dst[7] = view_inv * vec4(-x2, -y2, -f, 1.f);
	}

	Frustum cCameraPrivate::get_frustum(float n, float f)
	{
		vec3 ps[8];
		get_points(ps, n, f);
		return Frustum(ps);
	}

	vec3 cCameraPrivate::screen_to_world(const uvec2& pos)
	{
		auto p = proj_inv * vec4((vec2)pos / (vec2)screen_size * 2.f - 1.f, near, 1.f);
		p = p / p.w;
		return view_inv * p;
	}

	ivec2 cCameraPrivate::world_to_screen(const vec3& pos, const ivec4& border)
	{
		auto p = proj * view * vec4(pos, 1.f);
		p = p / p.w;
		if (p.z > 0.f && p.z < 1.f)
		{
			auto ret = ivec2((p.xy() + 1.f) * 0.5f * (vec2)screen_size);
			if (ret.x > border.x &&
				ret.x < screen_size.x - border.z &&
				ret.y > border.y &&
				ret.y < screen_size.y - border.w)
			return ret;
		}
		return ivec2(10000);
	}
}
