//
// Copyright (c) 2008-2019 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include <random>
#include <Urho3D/Urho3DAll.h>
#include "RenderToTexture.h"

static unsigned int generate_random_seed()
{
	std::random_device rd;
	std::default_random_engine gen = std::default_random_engine(rd());
	std::uniform_int_distribution<unsigned int> dis(0, UINT_MAX);

	return dis(gen);
}

static void addDebugArrow(Urho3D::DebugRenderer * r, const Urho3D::Vector3 &from, const Urho3D::Vector3 &to, const Urho3D::Color &color, const Urho3D::Vector3 &cameraPos)
{
	const float max_arrow_len = 3.0f;
	const float ratio = 0.25f;
	const float degree = cosf(30.0f);

	float arrow_len = max_arrow_len;
	if ((to - from).LengthSquared() * ratio * ratio < arrow_len)
		arrow_len = (to - from).Length() * ratio;

	const Urho3D::Vector3 v1(from - to);
	Urho3D::Vector3 flip_p(to + v1.Normalized() * arrow_len);
	Urho3D::Vector3 v_expand((flip_p - cameraPos).CrossProduct(v1).Normalized());

	r->AddLine(from, to, color, true);
	r->AddLine(to, flip_p + v_expand * degree * arrow_len, color, true);
	r->AddLine(to, flip_p - v_expand * degree * arrow_len, color, true);
}

static const StringHash TEXTURECUBE_SIZE("TEXTURECUBE SIZE");
static const Vector3 default_light_dir(0.5f, -1.0f, 0.5f);
static const Color default_light_color(0.2f, 0.2f, 0.2f);

URHO3D_DEFINE_APPLICATION_MAIN(RenderToTexture)

RenderToTexture::RenderToTexture(Context* context) :
    Sample(context)
{
	SetRandomSeed(generate_random_seed());
}

void RenderToTexture::Setup()
{
	Sample::Setup();
	engineParameters_[EP_LOG_NAME] = "Urho3D.log";
}

void RenderToTexture::Start()
{
    // Execute base class startup
    Sample::Start();

    // Create the scene content
    CreateScene();

    // Create the UI content
    CreateInstructions();

    // Setup the viewport for displaying the scene
    SetupViewport();

    // Hook up to the frame update events
    SubscribeToEvents();

    // Set the mouse mode to use in the sample
    Sample::InitMouseMode(MM_RELATIVE);
}

void RenderToTexture::CreateScene()
{
    auto* cache = GetSubsystem<ResourceCache>();

    {
        // Create the scene in which we move around
        scene_ = new Scene(context_);

        // Create octree, use also default volume (-1000, -1000, -1000) to (1000, 1000, 1000)
        scene_->CreateComponent<Octree>();

        // Create a Zone component for ambient lighting & fog control
        Node* zoneNode = scene_->CreateChild("Zone");
        auto* zone = zoneNode->CreateComponent<Zone>();
        zone->SetBoundingBox(BoundingBox(-1000.0f, 1000.0f));
        zone->SetAmbientColor(Color(0.1f, 0.1f, 0.1f));
        zone->SetFogStart(500.0f);
        zone->SetFogEnd(800.0f);

		scene_->CreateComponent<DebugRenderer>();
		scene_->GetComponent<DebugRenderer>()->SetLineAntiAlias(true);

        lightNode = scene_->CreateChild("DirectionalLight");
        lightNode->SetDirection(default_light_dir);
        auto* light = lightNode->CreateComponent<Light>();
        light->SetLightType(LIGHT_DIRECTIONAL);
        light->SetColor(default_light_color);
        light->SetSpecularIntensity(1.0f);
		light->SetCastShadows(true);

		Node * boxNode = scene_->CreateChild("Box");
		boxNode->SetPosition(Vector3(20.5f, 30.0f, 20.5f));
		boxNode->SetScale(Vector3(20.0f, 20.0f, 20.f));
		auto* boxObject = boxNode->CreateComponent<StaticModel>();
		boxObject->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
		boxObject->SetMaterial(cache->GetResource<Material>("Materials/Stone.xml"));
		boxObject->SetCastShadows(true);

        // Create a "floor" consisting of several tiles
        for (int y = -5; y <= 5; ++y)
        {
            for (int x = -5; x <= 5; ++x)
            {
                Node* floorNode = scene_->CreateChild("FloorTile");
                floorNode->SetPosition(Vector3(x * 20.5f, -0.5f, y * 20.5f));
                floorNode->SetScale(Vector3(20.0f, 1.0f, 20.f));
                auto* floorObject = floorNode->CreateComponent<StaticModel>();
                floorObject->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
                floorObject->SetMaterial(cache->GetResource<Material>("Materials/Stone.xml"));
				floorObject->SetCastShadows(true);
            }
        }

        {
			// Create skybox. The Skybox component is used like StaticModel, but it will be always located at the camera, giving the
			// illusion of the box planes being far away. Use just the ordinary Box model and a suitable material, whose shader will
			// generate the necessary 3D texture coordinates for cube mapping
			SubscribeToEvent(E_SPACEBOXGEN, URHO3D_HANDLER(RenderToTexture, ChangeLight));
			Node * space = scene_->CreateChild("Space Box");
			space->SetScale(500.0f); // The scale actually does not matter
			auto* spacebox = space->CreateComponent<Skybox>();
			spacebox->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
			SharedPtr<Material> space_mat = MakeShared<Material>(GetContext());
			space_mat->SetCullMode(CULL_NONE);
			space_mat->SetNumTechniques(1);
			space_mat->SetTechnique(0, cache->GetResource<Technique>("Techniques/DiffSkybox.xml"), QUALITY_MAX);
			gen = MakeShared<SpaceBoxGen>(context_);
			space_mat->SetTexture(TU_DIFFUSE, gen->SpaceCube);
			gen->Generate();
			spacebox->SetMaterial(space_mat);
        }

        // Create the camera which we will move around. Limit far clip distance to match the fog
        cameraNode_ = scene_->CreateChild("Camera");
        auto* camera = cameraNode_->CreateComponent<Camera>();
        camera->SetFarClip(800.0f);

        // Set an initial position for the camera scene node above the plane
        cameraNode_->SetPosition(Vector3(0.0f, 7.0f, -30.0f));
    }
}

void RenderToTexture::CreateInstructions()
{
    auto* cache = GetSubsystem<ResourceCache>();
    auto* ui = GetSubsystem<UI>();
	ui->GetRoot()->SetDefaultStyle(cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));

    // Construct new Text object, set string to display and font to use
    auto* instructionText = ui->GetRoot()->CreateChild<Text>();
    instructionText->SetText("Use WASD keys to move/ Press Space to toggle free mouse");
    instructionText->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 15);

    // Position the text relative to the screen center
    instructionText->SetHorizontalAlignment(HA_CENTER);
    instructionText->SetVerticalAlignment(VA_CENTER);
    instructionText->SetPosition(0, ui->GetRoot()->GetHeight() / 4);

	uielement_ = ui->GetRoot()->CreateChild<UIElement>();
	uielement_->SetAlignment(HA_LEFT, VA_TOP);
	uielement_->SetLayout(LM_VERTICAL, 20, IntRect(20, 40, 20, 40));

	UIElement * uielement_g = uielement_->CreateChild<UIElement>();
	uielement_g->SetAlignment(HA_LEFT, VA_TOP);
	uielement_g->SetLayout(LM_HORIZONTAL, 5);

	Button * g = uielement_g->CreateChild<Button>();
	g->SetStyleAuto();
	g->SetMinHeight(20);
	g->SetFocusMode(FM_NOTFOCUSABLE);
	Text * t = g->CreateChild<Text>();
	t->SetText("Generate");
	t->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 12);
	t->SetAlignment(HA_CENTER, VA_CENTER);
	SubscribeToEvent(g, E_RELEASED, URHO3D_HANDLER(RenderToTexture, GenerateClicked));

	UIElement * uielement_cube = uielement_->CreateChild<UIElement>();
	uielement_cube->SetAlignment(HA_LEFT, VA_TOP);
	uielement_cube->SetLayout(LM_HORIZONTAL, 5);

	Text * tCubeSize = uielement_cube->CreateChild<Text>();
	tCubeSize->SetText("TextureCube Size:");
	tCubeSize->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 12);

	DropDownList * cubeSizeList = uielement_cube->CreateChild<DropDownList>();
	cubeSizeList->SetStyleAuto();
	cubeSizeList->SetFocusMode(FM_NOTFOCUSABLE);

	const int Sizes[5] = { 256,512,1024,2048,4096 };
	for (int i = 0; i < 5; ++i)
	{
		Text * item = cubeSizeList->CreateChild<Text>();
		item->SetStyle("EditorEnumAttributeText");
		item->SetText(String(Sizes[i]));
		item->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 12);
		item->SetAlignment(HA_CENTER, VA_CENTER);
		item->SetVar(TEXTURECUBE_SIZE, Sizes[i]);
		cubeSizeList->AddItem(item);
	}
	cubeSizeList->SetSelection(2);
	SubscribeToEvent(cubeSizeList, E_ITEMSELECTED, URHO3D_HANDLER(RenderToTexture, SelectSize));

	CreateCheckbox(String("point stars"), URHO3D_HANDLER(RenderToTexture, Toggle_Point_Star));
	CreateCheckbox(String("bright stars"), URHO3D_HANDLER(RenderToTexture, Toggle_Bright_Star));
	CreateCheckbox(String("nebula"), URHO3D_HANDLER(RenderToTexture, Toggle_Nebula));
	CreateCheckbox(String("sun"), URHO3D_HANDLER(RenderToTexture, Toggle_Sun));
}

void RenderToTexture::SetupViewport()
{
    auto* renderer = GetSubsystem<Renderer>();

    // Set up a viewport to the Renderer subsystem so that the 3D scene can be seen
    SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
    renderer->SetViewport(0, viewport);
}

void RenderToTexture::MoveCamera(float timeStep)
{
    // Do not move if the UI has a focused element (the console)
    if (GetSubsystem<UI>()->GetFocusElement())
        return;

    auto* input = GetSubsystem<Input>();

    // Movement speed as world units per second
    const float MOVE_SPEED = 20.0f;
    // Mouse sensitivity as degrees per pixel
    const float MOUSE_SENSITIVITY = 0.1f;

	cameraNode_->Translate(Vector3::FORWARD * input->GetMouseMoveWheel() * MOVE_SPEED);

	if (mouseFree == false || (mouseFree && input->GetMouseButtonDown(MOUSEB_RIGHT))) {
		// Use this frame's mouse motion to adjust camera node yaw and pitch. Clamp the pitch between -90 and 90 degrees
		IntVector2 mouseMove = input->GetMouseMove();
		yaw_ += MOUSE_SENSITIVITY * mouseMove.x_;
		pitch_ += MOUSE_SENSITIVITY * mouseMove.y_;
		pitch_ = Clamp(pitch_, -90.0f, 90.0f);

		// Construct new orientation for the camera scene node from yaw and pitch. Roll is fixed to zero
		cameraNode_->SetRotation(Quaternion(pitch_, yaw_, 0.0f));
	}

    // Read WASD keys and move the camera scene node to the corresponding direction if they are pressed
    if (input->GetKeyDown(KEY_W))
        cameraNode_->Translate(Vector3::FORWARD * MOVE_SPEED * timeStep);
    if (input->GetKeyDown(KEY_S))
        cameraNode_->Translate(Vector3::BACK * MOVE_SPEED * timeStep);
    if (input->GetKeyDown(KEY_A))
        cameraNode_->Translate(Vector3::LEFT * MOVE_SPEED * timeStep);
    if (input->GetKeyDown(KEY_D))
        cameraNode_->Translate(Vector3::RIGHT * MOVE_SPEED * timeStep);
}

void RenderToTexture::SubscribeToEvents()
{
    // Subscribe HandleUpdate() function for processing update events
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(RenderToTexture, HandleUpdate));

	// Subscribe HandlePostRenderUpdate() function for processing the post-render update event, during which we request debug geometry
	SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(RenderToTexture, HandlePostRenderUpdate));
}

void RenderToTexture::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace Update;

    // Take the frame time step, which is stored as a float
    float timeStep = eventData[P_TIMESTEP].GetFloat();

    // Move the camera, scale movement with time step
    MoveCamera(timeStep);

	auto* input = GetSubsystem<Input>();
	if (input->GetKeyPress(Key::KEY_SPACE))
	{
		mouseFree = !mouseFree;
		if (mouseFree)
			Sample::InitMouseMode(MM_FREE);
		else
			Sample::InitMouseMode(MM_RELATIVE);
	}
}

void RenderToTexture::GenerateClicked(StringHash eventType, VariantMap& eventData)
{
	gen->Generate();
}

void RenderToTexture::SelectSize(StringHash eventType, VariantMap& eventData)
{
	auto* list = static_cast<DropDownList*>(eventData[Toggled::P_ELEMENT].GetPtr());
	UIElement * item = list->GetSelectedItem();
	gen->cubeSize = item->GetVar(TEXTURECUBE_SIZE).GetInt();
	gen->Generate();
}

void RenderToTexture::CreateCheckbox(const String& label, EventHandler* handler)
{
	SharedPtr<UIElement> container(new UIElement(context_));
	container->SetAlignment(HA_LEFT, VA_TOP);
	container->SetLayout(LM_HORIZONTAL, 8);
	uielement_->AddChild(container);

	SharedPtr<CheckBox> box(new CheckBox(context_));
	container->AddChild(box);
	box->SetStyleAuto();
	box->SetChecked(true);

	SharedPtr<Text> text(new Text(context_));
	container->AddChild(text);
	text->SetText(label);
	text->SetStyleAuto();

	SubscribeToEvent(box, E_TOGGLED, handler);
}

void RenderToTexture::Toggle_Point_Star(StringHash eventType, VariantMap& eventData)
{
	auto* box = static_cast<CheckBox*>(eventData[Toggled::P_ELEMENT].GetPtr());
	gen->point_star_enable = box->IsChecked();
	gen->Generate();
}

void RenderToTexture::Toggle_Bright_Star(StringHash eventType, VariantMap& eventData)
{
	auto* box = static_cast<CheckBox*>(eventData[Toggled::P_ELEMENT].GetPtr());
	gen->bright_star_enable = box->IsChecked();
	gen->Generate();
}

void RenderToTexture::Toggle_Nebula(StringHash eventType, VariantMap& eventData)
{
	auto* box = static_cast<CheckBox*>(eventData[Toggled::P_ELEMENT].GetPtr());
	gen->nebula_enable = box->IsChecked();
	gen->Generate();
}

void RenderToTexture::Toggle_Sun(StringHash eventType, VariantMap& eventData)
{
	auto* box = static_cast<CheckBox*>(eventData[Toggled::P_ELEMENT].GetPtr());
	gen->sun_enable = box->IsChecked();
	gen->Generate();
}

void RenderToTexture::HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData)
{
	if (gen->sun_enable)
	{
		DebugRenderer* debug = scene_->GetComponent<DebugRenderer>();
		addDebugArrow(debug, Vector3::ZERO, gen->GetSunDirection().Normalized() * 50.0f, Color::GREEN, cameraNode_->GetWorldPosition());
	}
}

void RenderToTexture::ChangeLight(StringHash eventType, VariantMap& eventData)
{
	using namespace SpaceBoxGenEvt;
	auto* light = lightNode->GetComponent<Light>();
	if (eventData[P_SUN_ENABLE].GetBool())
	{
		lightNode->SetDirection(-eventData[P_SUN_DIR].GetVector3());
		light->SetColor(eventData[P_SUN_COLOR].GetColor());
	}
	else
	{
		lightNode->SetDirection(default_light_dir);
		light->SetColor(default_light_color);
	}
}

