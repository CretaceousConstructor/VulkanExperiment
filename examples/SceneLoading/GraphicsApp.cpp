#include "GraphicsApp.h"

void GraphicsApp::InitRenderer()
{
	renderer = std::make_unique<RealtimeRenderer>(gfx);
}
