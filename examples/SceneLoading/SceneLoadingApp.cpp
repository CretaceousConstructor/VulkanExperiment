#include "SceneLoadingApp.h"

void SceneLoadingApp::InitRenderer()
{

	renderer = std::make_unique<SceneLoadingRenderer>(gfx);


}
