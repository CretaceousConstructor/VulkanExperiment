#include "KTXTextureApp.h"

void KTXTextureApp::InitRenderer()
{
	renderer = std::make_unique<KTXTextureRenderer>(window,device_manager,swap_chain_manager,command_manager);
}
