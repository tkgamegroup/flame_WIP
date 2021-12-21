
	void AnimationRunner::update()
	{
		const float dist = 1.f / 60.f;

		if (curr_anim)
		{
			reset_bones();

			for (int i = 0; i < curr_frame_index.size(); i++)
			{
				auto &t = curr_anim->tracks[i];
				auto index = curr_frame_index[i];
				auto left_keyframe = t.second->keyframes[index].get();
				index++;
				if (index == t.second->keyframes.size())
					index = 0;
				auto right_keyframe = t.second->keyframes[index].get();

				auto beta = 0.f;
				if (left_keyframe != right_keyframe)
				{
					beta = (curr_frame - left_keyframe->frame) /
						(right_keyframe->frame - left_keyframe->frame);
				}

				auto data = &bone_data[t.first];
				data->rotation = flame::quaternion_to_mat3(glm::normalize((1.f - beta) *
					left_keyframe->quaternion + beta * right_keyframe->quaternion));
				data->coord = left_keyframe->coord + (right_keyframe->coord - 
					right_keyframe->coord) * beta;
			}

			bool wrap = false;
			curr_frame += elapsed_time / dist;
			if (curr_anim->total_frame > 0)
			{
				if (curr_frame >= curr_anim->total_frame)
				{
					curr_frame = glm::mod(curr_frame, (float)curr_anim->total_frame);
					wrap = true;
				}
			}

			auto dst = curr_frame;
			if (wrap)
				dst += curr_anim->total_frame;
			for (int i = 0; i < curr_frame_index.size(); i++)
			{
				auto &t = curr_anim->tracks[i];
				if (t.second->keyframes.size() == 0)
					continue;
				auto index = curr_frame_index[i];
				while (t.second->keyframes[index]->frame <= dst)
				{
					index++;
					if (index == t.second->keyframes.size())
					{
						index = 0;
						dst -= curr_anim->total_frame;
					}
				}
				if (index == 0)
					index = t.second->keyframes.size() - 1;
				else
					index -= 1;
				curr_frame_index[i] = index;
			}
		}

		refresh_bone();

		if (enable_IK)
		{
			//	for (int i = 0; i < pModel->iks.size(); i++)
			//	{
			//		auto &ik = pModel->iks[i];
			//		auto t = glm::vec3(boneMatrix[ik.targetID][3]);
			//		//t.z *= -1.f;
			//		for (int times = 0; times < ik.iterations; times++)
			//		{
			//			for (int index = 0; index < ik.chain.size(); index++)
			//			{
			//				//index = iChain;
			//				auto e = glm::vec3(boneMatrix[ik.effectorID][3]);
			//				//e.z *= -1.f;
			//				if (glm::length(t - e) < 0.0001f)
			//				{
			//					goto nextIk;
			//				}

			//				auto boneID = ik.chain[index];

			//				auto p = glm::vec3(boneMatrix[boneID][3]);
			//				//p.z *= -1.f;

			//				auto pe = glm::normalize(e - p);
			//				auto pt = glm::normalize(t - p);
			//				auto theDot = glm::dot(pe, pt);
			//				theDot = glm::clamp(theDot, 0.f, 1.f);
			//				auto theAcos = glm::acos(theDot);
			//				auto ang = glm::degrees(theAcos);
			//				if (glm::abs(ang) > 0.5f)
			//				{
			//					auto n = glm::normalize(glm::cross(pe, pt));
			//					if (glm::abs(n.z) < 1.f)
			//					{
			//						n.z = 0;
			//						n = glm::normalize(n);
			//					}
			//					boneData[boneID].rotation = glm::mat3(glm::rotate(ang, n)) * boneData[boneID].rotation;
			//					//refreshBone(ik.effectorID, boneData, outMat);
			//					pModel->refreshBone(boneID, boneData, boneMatrix);
			//					p = glm::vec3(boneMatrix[boneID][3]);
			//					e = glm::vec3(boneMatrix[ik.effectorID][3]);
			//					pe = glm::normalize(e - p);
			//					auto dot = glm::dot(pe, pt);
			//					int cut = 1;
			//				}
			//				//break;
			//			}
			//		}
			//	nextIk:
			//		//break;
			//		continue;
			//	}
			//}
		}

		for (int i = 0; i < model->bones.size(); i++)
			bone_matrix[i] *= glm::translate(-model->bones[i]->rootCoord);

		bone_buffer->update(bone_matrix.get(), sizeof(glm::mat4) * model->bones.size());
	}
