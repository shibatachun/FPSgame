#pragma once

enum class API
{
	VULKAN,
	OPENGL,
	DIRECTX
};

class IRenderer {
public:
	
	virtual ~IRenderer() = default;
	virtual bool Init() = 0;
	virtual void DrawFrame() = 0;
	virtual void Cleanup() = 0;

};