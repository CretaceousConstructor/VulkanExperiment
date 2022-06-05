#include "VkImageDirector.h"


VkImageDirector::VkImageDirector(std::shared_ptr<VkImageBuilder>_builder)
	:builder(_builder)
{

}

std::shared_ptr<VkImageWrapper> VkImageDirector::Construct() const
{
	return builder->GetResult();
}


