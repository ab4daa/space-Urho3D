#pragma once
#include <Urho3D/Graphics/TextureCube.h>
#include <Urho3D/Graphics/Model.h>

namespace Urho3D
{

	URHO3D_EVENT(E_SPACEBOXGEN, SpaceBoxGenEvt)
	{
		URHO3D_PARAM(P_SUN_ENABLE, SunEnable); // bool
		URHO3D_PARAM(P_SUN_DIR, SunDir); // vector3
		URHO3D_PARAM(P_SUN_COLOR, SunColor); // color
	}

	class SpaceBoxGen : public Object
	{
		URHO3D_OBJECT(SpaceBoxGen, Object);
	public:
		explicit SpaceBoxGen(Context* context);
		~SpaceBoxGen();
		void Generate();
		const Vector3& GetSunDirection() const { return SunDirection; }
		const Color& GetSunColor() const { return SunColor; }

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
		Vector3 SunDirection;
		Color SunColor;
	};
}
