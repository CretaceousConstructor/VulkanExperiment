#include "KTXTextureApp.h"

void KTXTextureApp::InitRenderer()
{
	renderer = std::make_unique<KTXTextureRenderer>();
}
