#pragma once
#include <iostream>
#include <math.h>
#include <algorithm>
#include <assert.h>
#include <math.h>
#include <string>
#include <thread>
#include <fstream>
#include <sstream>

#include "Source/ParticleSystem/Particle.h"
#include "Source/ParticleSystem/ParticleEmitter.h"
#include "Source/ParticleSystem/ParticleManager.h"
#include "Source/ParticleSystem/ParticleSystem.h"

#include "EngineSource/Structs.h"
#include "EngineSource/3D Engine/Extern.h"
#include "EngineSource/3D Engine/Model/Texture.h"
#include "EngineSource/3D Engine/RenderingManager.h"
#include "EngineSource/3D Engine/Components/Camera.h"
#include "EngineSource/3D Engine/Components/StateMachine.h"
#include "EngineSource/3D Engine/Model/Meshes/StaticMesh.h"
#include "EngineSource/3D Engine/Model/Meshes/DynamicMesh.h"
#include "EngineSource/3D Engine/Components/Base/Drawable.h"
#include "EngineSource/3D Engine/Model/Meshes/AnimatedModel.h"
#include "EngineSource/3D Engine/Model/Managers/MeshManager.h"
#include "EngineSource/3D Engine/Model/Managers/TextureManager.h"
#include "EngineSource/3D Engine/3DRendering/Framework/Engine3D.h"
#include "EngineSource/3D Engine/3DRendering/Rendering/ShadowMap.h"
#include "EngineSource/3D Engine/3DRendering/Rendering/ForwardRender.h"
#include "EngineSource/3D Engine/3DRendering/Framework/DirectXRenderingHelpClass.h"
#include "EngineSource/3D Engine/3DRendering/Rendering/VisabilityPass/VisabilityPass.h"
#include "EngineSource/3D Engine/3DRendering/Rendering/VisabilityPass/Component/VisibilityComponent.h"

#include "EngineSource/Helper/Timer.h"
#include "EngineSource/Window/window.h"
#include "EngineSource/Light/PointLight.h"
#include "EngineSource/Shader/ShaderManager.h"
#include "EngineSource/SettingsLoader/SettingsLoader.h"
#include "EngineSource/3D Engine/WICTextureLoader/WICTextureLoader.h"

//2d
#include "2D Engine/Render2D.h"

//RipTag
#include "Source/Physics/Bounce.h"

//import
#include "ImportLibrary/FormatHeader.h"
#include "ImportLibrary/formatImporter.h"

//input
#include "../InputManager/InputManager/InputHandler.h"

//imgui
#include "EngineSource/Debugg/ImGui/imgui.h"
#include "EngineSource/Debugg/ImGui/ImGuiManager.h"
#include "EngineSource/Debugg/ImGui/imgui_impl_win32.h"
#include "EngineSource/Debugg/ImGui/imgui_impl_dx11.h"