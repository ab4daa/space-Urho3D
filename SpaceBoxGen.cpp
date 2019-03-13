#include "SpaceBoxGen.h"
#include <Urho3D/Urho3DAll.h>

namespace Urho3D
{
	typedef struct vertex_s
	{
		Vector3 position;
		unsigned color;
	}vertex_data;

	static void buildStar(float size, const Vector3 &pos, float dist, vertex_data * vertexBufferOut)
	{
		const Vector3 vertexes[6] =
		{
			Vector3(-size, -size, 0.0f),
			Vector3(size, -size, 0.0f),
			Vector3(size, size, 0.0f),
			Vector3(-size, -size, 0.0f),
			Vector3(size, size, 0.0f),
			Vector3(-size, size, 0.0f)
		};

		float theta = Vector3::BACK.Angle(pos);
		Vector3 omega(Vector3::BACK.CrossProduct(pos));
		omega.Normalize();
		Quaternion q(theta, omega);

		for (unsigned ii = 0; ii < 6; ++ii)
		{
			Vector3& v = vertexBufferOut[ii].position;
			v = (q * vertexes[ii]) + (pos * dist);
		}

		float c = Pow(Random(1.0f), 4.0f);
		Color allColor(c, c, c, 1.0f);
		for (unsigned ii = 0; ii < 6; ++ii)
		{
			vertexBufferOut[ii].color = allColor.ToUInt();
		}
	}

	static Model * Create_Point_Stars(Context* ctx)
	{
		const unsigned int NSTARS = 100000;
		const unsigned numVertices = NSTARS * 6;
		vertex_data * vertexData = new vertex_data[numVertices];
		unsigned  * indexData = new unsigned[numVertices];

		for (unsigned int i = 0; i < NSTARS; ++i)
		{
			Vector3 pos(Random(-1.0f, 1.0f), Random(-1.0f, 1.0f), Random(-1.0f, 1.0f));
			pos.Normalize();
			buildStar(0.05f, pos, 128.0f, &(vertexData[i * 6]));
		}

		for (unsigned int i = 0; i < numVertices; ++i)
			indexData[i] = i;

		Model * fromScratchModel(new Model(ctx));
		VertexBuffer * vb(new VertexBuffer(ctx));
		IndexBuffer * ib(new IndexBuffer(ctx));
		Geometry * geom(new Geometry(ctx));

		// Shadowed buffer needed for raycasts to work, and so that data can be automatically restored on device loss
		vb->SetShadowed(true);
		// We could use the "legacy" element bitmask to define elements for more compact code, but let's demonstrate
		// defining the vertex elements explicitly to allow any element types and order
		PODVector<VertexElement> elements;
		elements.Push(VertexElement(TYPE_VECTOR3, SEM_POSITION));
		elements.Push(VertexElement(TYPE_UBYTE4_NORM, SEM_COLOR));
		vb->SetSize(numVertices, elements);
		vb->SetData(vertexData);

		ib->SetShadowed(true);
		ib->SetSize(numVertices, true);
		ib->SetData(indexData);

		geom->SetVertexBuffer(0, vb);
		geom->SetIndexBuffer(ib);
		geom->SetDrawRange(TRIANGLE_LIST, 0, numVertices);

		fromScratchModel->SetNumGeometries(1);
		fromScratchModel->SetGeometry(0, 0, geom);
		BoundingBox BB;
		BB.Define(vertexData[0].position);
		for (unsigned int i = 1; i < numVertices; ++i)
			BB.Merge(vertexData[i].position);
		fromScratchModel->SetBoundingBox(BB);

		delete[] vertexData;
		delete[] indexData;

		return fromScratchModel;
	}

	static Model * Create_Box(Context* ctx)
	{
		const unsigned boxVertexNum = 36;
		const Vector3 vertexes[boxVertexNum] =
		{
			Vector3(-1, -1, -1),
			Vector3(1, -1, -1),
			Vector3(1,  1, -1),
			Vector3(-1, -1, -1),
			Vector3(1,  1, -1),
			Vector3(-1,  1, -1),

			Vector3(1, -1,  1),
			Vector3(-1, -1,  1),
			Vector3(-1,  1,  1),
			Vector3(1, -1,  1),
			Vector3(-1,  1,  1),
			Vector3(1,  1,  1),

			Vector3(1, -1, -1),
			Vector3(1, -1,  1),
			Vector3(1,  1,  1),
			Vector3(1, -1, -1),
			Vector3(1,  1,  1),
			Vector3(1,  1, -1),

			Vector3(-1, -1,  1),
			Vector3(-1, -1, -1),
			Vector3(-1,  1, -1),
			Vector3(-1, -1,  1),
			Vector3(-1,  1, -1),
			Vector3(-1,  1,  1),

			Vector3(-1,  1, -1),
			Vector3(1,  1, -1),
			Vector3(1,  1,  1),
			Vector3(-1,  1, -1),
			Vector3(1,  1,  1),
			Vector3(-1,  1,  1),

			Vector3(-1, -1,  1),
			Vector3(1, -1,  1),
			Vector3(1, -1, -1),
			Vector3(-1, -1,  1),
			Vector3(1, -1, -1),
			Vector3(-1, -1, -1)
		};
		unsigned short indexData[boxVertexNum];
		for (unsigned ii = 0; ii < boxVertexNum; ++ii)
			indexData[ii] = ii;

		Model * fromScratchModel(new Model(ctx));
		VertexBuffer * vb(new VertexBuffer(ctx));
		IndexBuffer * ib(new IndexBuffer(ctx));
		Geometry * geom(new Geometry(ctx));

		// Shadowed buffer needed for raycasts to work, and so that data can be automatically restored on device loss
		vb->SetShadowed(true);
		// We could use the "legacy" element bitmask to define elements for more compact code, but let's demonstrate
		// defining the vertex elements explicitly to allow any element types and order
		PODVector<VertexElement> elements;
		elements.Push(VertexElement(TYPE_VECTOR3, SEM_POSITION));
		vb->SetSize(boxVertexNum, elements);
		vb->SetData(vertexes);

		ib->SetShadowed(true);
		ib->SetSize(boxVertexNum, false);
		ib->SetData(indexData);

		geom->SetVertexBuffer(0, vb);
		geom->SetIndexBuffer(ib);
		geom->SetDrawRange(TRIANGLE_LIST, 0, boxVertexNum);

		fromScratchModel->SetNumGeometries(1);
		fromScratchModel->SetGeometry(0, 0, geom);
		BoundingBox BB;
		BB.Define(vertexes[0]);
		for (unsigned int i = 1; i < boxVertexNum; ++i)
			BB.Merge(vertexes[i]);
		fromScratchModel->SetBoundingBox(BB);

		return fromScratchModel;
	}

	SpaceBoxGen::SpaceBoxGen(Context* context) : Object(context), SpaceCube(MakeShared<TextureCube>(context)) {}

	SpaceBoxGen::~SpaceBoxGen(){}

	void SpaceBoxGen::Generate()
	{
		auto* cache = GetSubsystem<ResourceCache>();
		// Create the scene which will be rendered to a texture
		rttScene_ = new Scene(context_);

		// Create octree, use default volume (-1000, -1000, -1000) to (1000, 1000, 1000)
		rttScene_->CreateComponent<Octree>();

		// Create a Zone for ambient light & fog control
		Node* zoneNode = rttScene_->CreateChild("Zone");
		auto* zone = zoneNode->CreateComponent<Zone>();
		// Set same volume as the Octree, set a close bluish fog and some ambient light
		zone->SetBoundingBox(BoundingBox(-1000.0f, 1000.0f));
		zone->SetAmbientColor(Color(0.05f, 0.1f, 0.15f));
		zone->SetFogColor(Color::BLACK);
		zone->SetFogStart(10.0f);
		zone->SetFogEnd(100.0f);

		point_stars = Create_Point_Stars(GetContext());
		Quaternion accumulate(Quaternion::IDENTITY);
		while (point_star_enable)
		{
			Quaternion x_rotate(Random(0.0f, 180.0f), Vector3::RIGHT);
			Quaternion y_rotate(Random(0.0f, 180.0f), Vector3::UP);
			Quaternion z_rotate(Random(0.0f, 180.0f), Vector3::FORWARD);
			Quaternion q(x_rotate * y_rotate * z_rotate);

			accumulate = q * accumulate;
			Node * pstar = rttScene_->CreateChild(String("point stars"));
			pstar->SetTransform(Vector3::ZERO, accumulate);
			StaticModel* pstarObject = pstar->CreateComponent<StaticModel>();
			pstarObject->SetModel(point_stars);
			pstarObject->SetMaterial(cache->GetResource<Material>("Materials/point_stars.xml"));

			if (Random(1.0f) < 0.2f)
				break;
		}

		box = Create_Box(GetContext());
		Material * star_mat = cache->GetResource<Material>("Materials/star.xml");
		while (bright_star_enable)
		{
			Node * star = rttScene_->CreateChild(String("bright star"));
			star->SetTransform(Vector3::ZERO, accumulate);
			StaticModel* starObject = star->CreateComponent<StaticModel>();
			starObject->SetModel(box);
			SharedPtr<Material> m = star_mat->Clone();
			m->SetShaderParameter("StarPosition", Vector3(Random(-1.0f, 1.0f), Random(-1.0f, 1.0f), Random(-1.0f, 1.0f)).Normalized());
			m->SetShaderParameter("StarColor", Vector3::ONE);
			m->SetShaderParameter("StarSize", 0.0f);
			m->SetShaderParameter("StarFalloff", Random(1.0f) * Pow(2, 20) + Pow(2, 20));
			starObject->SetMaterial(m);

			if (Random(1.0f) < 0.01f)
				break;
		}

		Material * nebula_mat = cache->GetResource<Material>("Materials/nebular.xml");
		while (nebula_enable)
		{
			Node * nebula = rttScene_->CreateChild(String("nebula"));
			nebula->SetTransform(Vector3::ZERO, Quaternion::IDENTITY);
			StaticModel* nebulaObject = nebula->CreateComponent<StaticModel>();
			nebulaObject->SetModel(box);
			SharedPtr<Material> m = nebula_mat->Clone();
			m->SetShaderParameter("NebularColor", Vector3(Random(1.0f), Random(1.0f), Random(1.0f)));
			m->SetShaderParameter("NebularOffset", Vector3(Random(1.0f) * 2000 - 1000, Random(1.0f) * 2000 - 1000, Random(1.0f) * 2000 - 1000));
			m->SetShaderParameter("NebularScale", Random(1.0f) * 0.5f + 0.25f);
			m->SetShaderParameter("NebularIntensity", Random(1.0f) * 0.2f + 0.9f);
			m->SetShaderParameter("NebularFalloff", Random(1.0f) * 3 + 3);
			nebulaObject->SetMaterial(m);

			if (Random(1.0f) < 0.5f)
				break;
		}

		if (sun_enable)
		{
			Material * sun_mat = cache->GetResource<Material>("Materials/sun.xml");
			Node * sun = rttScene_->CreateChild(String("sun"));
			sun->SetTransform(Vector3::ZERO, Quaternion::IDENTITY);
			StaticModel* sunObject = sun->CreateComponent<StaticModel>();
			sunObject->SetModel(box);
			SunDirection = Vector3(Random(-1.0f, 1.0f), Random(-1.0f, 1.0f), Random(-1.0f, 1.0f));
			SunColor = Color(Random(1.0f), Random(1.0f), Random(1.0f));
			sun_mat->SetShaderParameter("SunPosition", SunDirection);
			sun_mat->SetShaderParameter("SunColor", SunColor.ToVector3());
			sun_mat->SetShaderParameter("SunSize", Random(1.0f) * 0.0001f + 0.0001f);
			sun_mat->SetShaderParameter("SunFalloff", Random(1.0f) * 16 + 8);
			sunObject->SetMaterial(sun_mat);
		}

		const Vector3 dir[MAX_CUBEMAP_FACES] = {
			Vector3::RIGHT,
			Vector3::LEFT,
			Vector3::UP,
			Vector3::DOWN,
			Vector3::FORWARD,
			Vector3::BACK
		};

		const Vector3 up[MAX_CUBEMAP_FACES] = {
			Vector3::UP,
			Vector3::UP,
			Vector3::DOWN,
			Vector3::DOWN,
			Vector3::UP,
			Vector3::UP
		};

		for (unsigned ii = 0; ii < MAX_CUBEMAP_FACES; ++ii)
		{
			CameraNodes[ii] = rttScene_->CreateChild("Camera");
			auto* camera = CameraNodes[ii]->CreateComponent<Camera>();
			camera->SetFarClip(256.0f);
			camera->SetAspectRatio(1.0f);
			camera->SetFov(90.0f);
			CameraNodes[ii]->SetPosition(Vector3::ZERO);
			CameraNodes[ii]->LookAt(dir[ii], up[ii]);
		}

		if(SpaceCube->SetSize(cubeSize, Graphics::GetRGBAFormat(), TEXTURE_RENDERTARGET) == false)
			URHO3D_LOGERROR(String("SpaceCube->SetSize fail: cubeSize=") + String(cubeSize));
		for (unsigned ii = 0; ii < MAX_CUBEMAP_FACES; ++ii)
		{
			RenderSurface* s = SpaceCube->GetRenderSurface((CubeMapFace)ii);
			s->SetUpdateMode(SURFACE_MANUALUPDATE);
			s->QueueUpdate();
			SharedPtr<Viewport> v(new Viewport(context_, rttScene_, CameraNodes[ii]->GetComponent<Camera>()));
			v->SetRenderPath(cache->GetResource<XMLFile>("RenderPaths/SpaceBox.xml"));
			s->SetNumViewports(1);
			s->SetViewport(0, v);
		}

		/*notify sun position*/
		{
			using namespace SpaceBoxGenEvt;
			VariantMap &data = GetEventDataMap();
			data[P_SUN_ENABLE] = sun_enable;
			data[P_SUN_DIR] = SunDirection;
			data[P_SUN_COLOR] = SunColor;
			SendEvent(E_SPACEBOXGEN, data);
		}

		/*auto destroy scene*/
		SubscribeToEvent(E_ENDFRAME, URHO3D_HANDLER(SpaceBoxGen, HandleEndFrame));
	}

	/*destroy scene*/
	void SpaceBoxGen::HandleEndFrame(StringHash eventType, VariantMap& eventData)
	{
		UnsubscribeFromEvent(E_ENDFRAME);
		for (unsigned ii = 0; ii < MAX_CUBEMAP_FACES; ++ii)
		{
			CameraNodes[ii]->Remove();
			CameraNodes[ii] = nullptr;
			RenderSurface* s = SpaceCube->GetRenderSurface((CubeMapFace)ii);
			s->SetNumViewports(0);
		}
		rttScene_ = nullptr;
		point_stars = nullptr;
		box = nullptr;
	}
}
