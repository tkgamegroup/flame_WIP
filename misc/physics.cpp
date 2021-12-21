namespace flame
{
	struct CollisionGroup
	{
		int originalID;
		unsigned int originalmask;
	};

	struct Rigidbody
	{
		int boneID = -1;
		int originCollisionGroupID;
		int originCollisionFreeFlag;
		int collisionGroupID = -1;
		float density = 10.f;
		float velocityAttenuation;
		float rotationAttenuation;
		float bounce;
		float friction;
	};

	struct Joint
	{
		int rigid0ID;
		int rigid1ID;
		glm::vec3 maxCoord = glm::vec3(0.f);
		glm::vec3 minCoord = glm::vec3(0.f);
		glm::vec3 maxRotation = glm::vec3(0.f);
		glm::vec3 minRotation = glm::vec3(0.f);
		glm::vec3 springConstant = glm::vec3(0.f);
		glm::vec3 sprintRotationConstant = glm::vec3(0.f);
	};

	void createPhysicsScene()
	{
		//	auto group1ID = 1;
		//	for (auto g : scene->pCollisionGroups)
		//	{
		//		for (int i = 0; i < 16; i++)
		//		{
		//			if ((g->originalmask & (1 << i)) == 0)
		//			{
		//				auto group2ID = 1;
		//				for (auto h : scene->pCollisionGroups)
		//				{
		//					if (h->originalID == i)
		//					{
		//						PxSetGroupCollisionFlag(group1ID, group2ID, false);
		//					}
		//					group2ID++;
		//				}
		//			}
		//		}
		//		group1ID++;
		//	}

		//	//PxSetGroupCollisionFlag(3, 3, false);
		//	//PxSetGroupCollisionFlag(3, 4, false);
		//	//PxSetGroupCollisionFlag(4, 4, false);
		//	//for (int i = 0; i < 100; i++)
		//	//	for (int j = 0; j < 100; j++)
		//	//		PxSetGroupCollisionFlag(i + 1, j + 1, false);
		//}
	}
}
