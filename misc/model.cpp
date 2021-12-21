namespace flame
{
	void add_cylinder_vertex(Model *m, glm::mat3 rotation, glm::vec3 center, float halfHeight, float radius, int subdiv)
	{
		std::vector<std::vector<int>> indexs;
		indexs.resize(4);

		// top cap
		for (int i = 0; i < subdiv; i++)
		{
			auto ang = glm::radians(i * 360.f / subdiv);
			indexs[0].push_back(m->vertexes.size());
			m->add_vertex_position_normal(rotation * glm::vec3(cos(ang) * radius, halfHeight, sin(ang) * radius) + 
				center, rotation * glm::vec3(0.f, 1.f, 0.f));
		}

		// bottom cap
		for (int i = 0; i < subdiv; i++)
		{
			auto ang = glm::radians(i * 360.f / subdiv);
			indexs[1].push_back(m->vertexes.size());
			m->add_vertex_position_normal(rotation * glm::vec3(cos(ang) * radius, -halfHeight, sin(ang) * radius) + 
				center, rotation * glm::vec3(0.f, -1.f, 0.f));
		}

		// top
		for (int i = 0; i < subdiv; i++)
		{
			auto ang = glm::radians(i * 360.f / subdiv);
			indexs[2].push_back(m->vertexes.size());
			m->add_vertex_position_normal(rotation * glm::vec3(cos(ang) * radius, halfHeight, sin(ang) * radius) + 
				center, rotation * glm::vec3(cos(ang), 0.f, sin(ang)));
		}

		// bottom
		for (int i = 0; i < subdiv; i++)
		{
			auto ang = glm::radians(i * 360.f / subdiv);
			indexs[3].push_back(m->vertexes.size());
			m->add_vertex_position_normal(rotation * glm::vec3(cos(ang) * radius, -halfHeight, sin(ang) * radius) + 
				center, rotation * glm::vec3(cos(ang), 0.f, sin(ang)));
		}

		// top cap
		for (int i = 1; i < subdiv - 1; i++)
		{
			m->indices.push_back(indexs[0][i]);
			m->indices.push_back(indexs[0][0]);
			m->indices.push_back(indexs[0][i + 1]);
		}

		// bottom cap
		for (int i = 1; i < subdiv - 1; i++)
		{
			m->indices.push_back(indexs[1][i + 1]);
			m->indices.push_back(indexs[1][0]);
			m->indices.push_back(indexs[1][i]);
		}

		// middle
		for (int i = 0; i < subdiv; i++)
		{
			auto ii = i + 1;
			if (ii == subdiv)
				ii = 0;

			m->indices.push_back(indexs[2][i]);
			m->indices.push_back(indexs[2][ii]);
			m->indices.push_back(indexs[3][i]);

			m->indices.push_back(indexs[2][ii]);
			m->indices.push_back(indexs[3][ii]);
			m->indices.push_back(indexs[3][i]);
		}
	}

	void add_cone_vertex(Model *m, glm::mat3 rotation, glm::vec3 center, float height, float radius, int subdiv)
	{
		std::vector<std::vector<int>> indexs;
		indexs.resize(3);

		// top
		{
			indexs[0].push_back(m->vertexes.size());
			m->add_vertex_position_normal(rotation * glm::vec3(0.f, height, 0.f) + center, rotation * glm::vec3(0.f, 1.f, 0.f));
		}

		// cap
		for (int i = 0; i < subdiv; i++)
		{
			auto ang = glm::radians(i * 360.f / subdiv);
			indexs[1].push_back(m->vertexes.size());
			m->add_vertex_position_normal(rotation * glm::vec3(cos(ang) * radius, 0.f, sin(ang) * radius) + center, rotation * 
				glm::vec3(0.f, -1.f, 0.f));
		}

		for (int i = 0; i < subdiv; i++)
		{
			auto ang = glm::radians(i * 360.f / subdiv);
			indexs[2].push_back(m->vertexes.size());
			m->add_vertex_position_normal(rotation * glm::vec3(cos(ang) * radius, 0.f, sin(ang) * radius) + center, rotation * 
				glm::vec3(cos(ang), 0.f, sin(ang)));
		}

		for (int i = 0; i < subdiv; i++)
		{
			auto ii = i + 1;
			if (ii == subdiv)
				ii = 0;

			m->indices.push_back(indexs[2][i]);
			m->indices.push_back(indexs[0][0]);
			m->indices.push_back(indexs[2][ii]);
		}

		for (int i = 1; i < subdiv - 1; i++)
		{
			m->indices.push_back(indexs[1][i + 1]);
			m->indices.push_back(indexs[1][0]);
			m->indices.push_back(indexs[1][i]);
		}
	}

	void add_torus_vertex(Model *m, glm::mat3 rotation, glm::vec3 center, float radius, float sectionRadius, int axisSubdiv, 
		int heightSubdiv)
	{
		std::vector<std::vector<int>> indexs;
		indexs.resize(axisSubdiv);

		for (int i = 0; i < axisSubdiv; i++)
		{
			float ang = i * 360.f / axisSubdiv;
			glm::mat3 R = glm::mat3(glm::rotate(glm::radians(-ang), glm::vec3(0.f, 1.f, 0.f)));
			for (int j = 0; j < heightSubdiv; j++)
			{
				auto secang = glm::radians(j * 360.f / heightSubdiv);
				indexs[i].push_back(m->vertexes.size());
				m->add_vertex_position_normal(rotation * (center + R * (glm::vec3(cos(secang) * sectionRadius + radius, 
					sin(secang) * sectionRadius, 0.f))), rotation * R * glm::vec3(cos(secang), sin(secang), 0.f));
			}
		}

		for (int i = 0; i < axisSubdiv; i++)
		{
			auto ii = i + 1; if (ii == axisSubdiv) ii = 0;

			for (int j = 0; j < heightSubdiv; j++)
			{
				auto jj = j + 1; if (jj == heightSubdiv) jj = 0;

				m->indices.push_back(indexs[i][j]);
				m->indices.push_back(indexs[i][jj]);
				m->indices.push_back(indexs[ii][j]);

				m->indices.push_back(indexs[i][jj]);
				m->indices.push_back(indexs[ii][jj]);
				m->indices.push_back(indexs[ii][j]);
			}
		}
	}

	namespace TKM
	{
		void load(Model *m, const std::string &filename)
		{
			auto ikCount = read<int>(file);
			m->iks.resize(boneCount);
			for (int i = 0; i < ikCount; i++)
			{
				auto b = m->new_bone_ik();

				b->targetID = read<int>(file);
				b->effectorID = read<int>(file);
				b->iterations = read<short>(file);
				b->weight = read<float>(file);

				b->chain.resize(read<int>(file));
				file.read((char*)b->chain.data(), sizeof(int) * b->chain.size());
			}

			#if FLAME_ENABLE_PHYSICS
			auto rigidbodyCount = read_int(file);
			for (int i = 0; i < rigidbodyCount; i++)
			{
				auto r = m->new_rigidbody();

				r->type = (RigidbodyType)read_int(file);
				r->name = read_string(file);
				r->originCollisionGroupID = read_int(file);
				r->originCollisionFreeFlag = read_int(file);
				r->boneID = read_int(file);
				r->coord = read_float3(file);
				r->quat = read_float4(file);
				r->density = read_float(file);
				r->velocityAttenuation = read_float(file);
				r->rotationAttenuation = read_float(file);
				r->bounce = read_float(file);
				r->friction = read_float(file);

				auto shapeCount = read_int(file);
				for (int j = 0; j < shapeCount; j++)
				{
					auto s = r->new_shape();
					s->coord = read_float3(file);
					s->quat = read_float4(file);
					s->scale = read_float3(file);
					s->type = (ShapeType)read_int(file);
				}
			}

			auto jointCount = read_int(file);
			for (int i = 0; i < jointCount; i++)
			{
				auto j = m->new_joint();
				j->coord = read_float3(file);
				j->quat = read_float4(file);
				j->rigid0ID = read_int(file);
				j->rigid1ID = read_int(file);
				j->maxCoord = read_float3(file);
				j->minCoord = read_float3(file);
				j->maxRotation = read_float3(file);
				j->minRotation = read_float3(file);
				j->springConstant = read_float3(file);
				j->sprintRotationConstant = read_float3(file);
			}
			#endif
		}
	}

	void init_model()
	{
		{
			auto m = std::make_shared<Model>();
			m->filename = "Arrow";

			glm::mat3 matR = glm::mat3(glm::rotate(glm::radians(-90.f), glm::vec3(0.f, 0.f, 1.f)));

			add_cylinder_vertex(m.get(), matR, glm::vec3(0.4f, 0.f, 0.f), 0.4f, 0.01f, 32);
			add_cone_vertex(m.get(), matR, glm::vec3(0.8f, 0.f, 0.f), 0.2f, 0.05f, 32);

			auto g = new Geometry;
			g->name = "0";
			g->material = default_material;
			g->indiceCount = m->indices.size();
			m->geometries.emplace_back(g);
		}

		{
			auto m = std::make_shared<Model>();
			m->filename = "Torus";

			glm::mat3 matR = glm::mat3(glm::rotate(glm::radians(-90.f), glm::vec3(0.f, 0.f, 1.f)));

			add_torus_vertex(m.get(), matR, glm::vec3(), 1.f, 0.01f, 32, 32);

			auto g = new Geometry;
			g->name = "0";
			g->material = default_material;
			g->indiceCount = m->indices.size();
			m->geometries.emplace_back(g);
		}

		{
			auto m = std::make_shared<Model>();
			m->filename = "Hammer";

			glm::mat3 matR = glm::mat3(glm::rotate(glm::radians(-90.f), glm::vec3(0.f, 0.f, 1.f)));

			add_cylinder_vertex(m.get(), matR, glm::vec3(0.45f, 0.f, 0.f), 0.45f, 0.01f, 32);
			int ic0 = m->indices.size();
			add_cube_vertex(m.get(), matR, glm::vec3(0.9f, 0.f, 0.f), 0.1f);
			int ic1 = m->indices.size();

			auto g0 = new Geometry;
			g0->name = "0";
			g0->material = default_material;
			g0->indiceCount = ic0;
			auto g1 = new Geometry;
			g1->name = "1";
			g1->material = default_material;
			g1->indiceBase = ic0;
			g1->indiceCount = ic1 - ic0;
			m->geometries.emplace_back(g0);
			m->geometries.emplace_back(g1);
		}
	}
}
