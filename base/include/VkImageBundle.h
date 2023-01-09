#pragma once
//#include "VkImageBase.h"
//#include <memory>
//#include <vector>
//
//class VkImageBundle
//{
//  public:
//	VkImageBundle() = delete;
//
//	VkImageBundle(std::vector<std::shared_ptr<VkImageBase>> _images, size_t bundle_size);
//	~VkImageBundle() = default;
//
//	VkImageBundle(const VkImageBundle &) = default;
//	VkImageBundle &operator=(const VkImageBundle &) = default;
//
//	VkImageBundle(VkImageBundle &&)                 = default;
//	VkImageBundle &operator=(VkImageBundle &&) = default;
//
//
//	[[nodiscard]] size_t GetBundleCount() const;
//	const VkImageBase &operator[](size_t index) const;
//	[[nodiscard]] const VkImageBase &GetOne(size_t index) const;
//	[[nodiscard]] const std::vector<std::shared_ptr<VkImageBase>> &GetImagesArray() const;
//
//
//
//
//  private:
//	size_t                                   bundle_count{};
//	std::vector<std::shared_ptr<VkImageBase>> images;
//
//};
