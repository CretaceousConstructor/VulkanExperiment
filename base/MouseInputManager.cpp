#include "MouseInputManager.h"

MouseInputManager::MouseInputManager(VkGraphicsComponent &gfx_) :
    gfx(gfx_),
    io{&ImGui::GetIO()}
{
}

float MouseInputManager::GetPitchDiff() const
{
	return io->MouseDelta.y;
}

float MouseInputManager::GetYawDiff() const
{
	return io->MouseDelta.x;
}

float MouseInputManager::GetMouseScroll() const
{
	return 0.1f * io->MouseWheel;
}
