namespace flame
{
	struct BoneIK
	{
		int targetID = -1;
		int effectorID = -1;
		unsigned short iterations = 0;
		float weight = 0.f;
		std::vector<int> chain;
	};

	struct Model
	{
		std::vector<std::unique_ptr<BoneIK>> iks;

#if FLAME_ENABLE_PHYSICS != 0
		std::vector<std::unique_ptr<Rigidbody>> rigidbodies;
		std::vector<std::unique_ptr<Joint>> joints;
#endif
	};
}
