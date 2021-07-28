#pragma once

#include "imgui.h"
#include <d3d12.h>
#include <cstdint>

bool LoadTextureFromFile(const char* filename, ID3D12Device* d3d_device, D3D12_CPU_DESCRIPTOR_HANDLE srv_cpu_handle, ID3D12Resource** out_tex_resource, int* out_width, int* out_height);
bool LoadTextureFromImageData(unsigned char* image_data, int image_width, int image_height, ID3D12Device* d3d_device, D3D12_CPU_DESCRIPTOR_HANDLE srv_cpu_handle, ID3D12Resource** out_tex_resource);
bool LoadTextureFromImageData8Bpp(unsigned char* image_data, int image_width, int image_height, ID3D12Device* d3d_device, D3D12_CPU_DESCRIPTOR_HANDLE srv_cpu_handle, ID3D12Resource** out_tex_resource);