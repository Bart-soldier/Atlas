#pragma once

#include <vulkan/vulkan.h>

#include <unordered_map>

namespace Atlas::VulkanResources
{
	inline std::unordered_map<uint32_t, VkDescriptorBufferInfo> s_Buffers;
	inline std::unordered_map<uint32_t, VkDescriptorImageInfo> s_Textures;
	inline std::unordered_map<uint32_t, uint32_t> s_TextureSlots;
	inline std::unordered_map<uint32_t, VkDescriptorSet> s_ImGuiTextures;

	inline void RegisterBuffer(uint32_t binding, VkBuffer buffer, VkDeviceSize range)
	{
		s_Buffers[binding] = { buffer, 0, range };
	}

	inline void RegisterTexture(uint32_t id, VkImageView view, VkSampler sampler, VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		s_Textures[id] = { sampler, view, layout };
		if (s_TextureSlots.find(0) == s_TextureSlots.end())
			s_TextureSlots[0] = id;
	}

	inline void SetTextureLayout(uint32_t id, VkImageLayout layout)
	{
		auto it = s_Textures.find(id);
		if (it != s_Textures.end())
			it->second.imageLayout = layout;
	}

	inline bool GetBuffer(uint32_t binding, VkDescriptorBufferInfo& info)
	{
		auto it = s_Buffers.find(binding);
		if (it == s_Buffers.end())
			return false;
		info = it->second;
		return true;
	}

	inline bool GetTexture(uint32_t id, VkDescriptorImageInfo& info)
	{
		auto it = s_Textures.find(id);
		if (it == s_Textures.end())
			return false;
		info = it->second;
		return true;
	}

	inline void SetTextureSlot(uint32_t slot, uint32_t id)
	{
		s_TextureSlots[slot] = id;
	}

	inline uint32_t GetTextureSlot(uint32_t slot)
	{
		auto it = s_TextureSlots.find(slot);
		return it == s_TextureSlots.end() ? 0 : it->second;
	}

	inline void SetImGuiTexture(uint32_t id, VkDescriptorSet descriptorSet)
	{
		s_ImGuiTextures[id] = descriptorSet;
	}

	inline VkDescriptorSet GetImGuiTexture(uint32_t id)
	{
		auto it = s_ImGuiTextures.find(id);
		return it == s_ImGuiTextures.end() ? VK_NULL_HANDLE : it->second;
	}

	inline void RemoveTexture(uint32_t id)
	{
		s_Textures.erase(id);
		s_ImGuiTextures.erase(id);
	}
}