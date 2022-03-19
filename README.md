# vk-start

A small Vulkan app that can be forked to get a quick start with Vulkan app development.

Requires VulkanSDK to be installed, all the other dependencies are included as submodules.

## Clone

    git clone --recurse-submodules -j4 git@github.com:jparimaa/vk-start.git

## Build

Run CMake with default settings.

Something like

    cmake . && make

## Default output

Doesn't do any kind of "real" shading, just sampling some textures.

![vk-start](vk-start.png?raw=true "vk-start")
