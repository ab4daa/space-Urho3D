#pragma once
#include <Urho3D/Graphics/TextureCube.h>
#include <Urho3D/Graphics/Model.h>

namespace Urho3D
{
	class SpaceBoxGen : public Object
	{
		URHO3D_OBJECT(SpaceBoxGen, Object);
	public:
		explicit SpaceBoxGen(Context* context);
		~SpaceBoxGen();
		void Generate();

		bool point_star_enable{ true };
		bool bright_star_enable{ true };
		bool nebula_enable{ true };
		bool sun_enable{ true };
		int cubeSize{ 1024 };
		SharedPtr<TextureCube> SpaceCube;

	private:
		void HandleEndFrame(StringHash eventType, VariantMap& eventData);

		SharedPtr<Scene> rttScene_;
		SharedPtr<Model> point_stars;
		SharedPtr<Model> box;
		SharedPtr<Node> CameraNodes[MAX_CUBEMAP_FACES];
	};
}
