// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <cassert>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

#include "Utils/Image.h"
#include "App.h"
#include "Utils/Log.h"
#include "Math/Color.h"
#include "Math/MathUtils.h"

using namespace Raycer;

Image::Image()
{
}

Image::Image(int length_)
{
	setSize(length_);
}

Image::Image(int width_, int height_)
{
	setSize(width_, height_);
}

Image::Image(const Image& image)
{
	int otherWidth = image.getWidth();
	int otherHeight = image.getHeight();
	int otherLength = image.getLength();

	setSize(otherWidth, otherHeight);

	for (int i = 0; i < otherLength; ++i)
		pixelData[i] = image.pixelData[i];
}

Image::Image(const std::string& fileName)
{
	load(fileName);
}

Image::~Image()
{
	if (pixelData != nullptr)
	{
		_mm_free(pixelData);
		pixelData = nullptr;
	}
}

Image& Image::operator=(const Image& image)
{
	int otherWidth = image.getWidth();
	int otherHeight = image.getHeight();
	int otherLength = image.getLength();

	setSize(otherWidth, otherHeight);

	for (int i = 0; i < otherLength; ++i)
		pixelData[i] = image.pixelData[i];

	return *this;
}

void Image::load(const std::string& fileName)
{
	App::getLog().logInfo("Loading image from %s", fileName);

	int newWidth, newHeight, newLength, components;
	uint32_t* imageData = (uint32_t*)stbi_load(fileName.c_str(), &newWidth, &newHeight, &components, 4);
	newLength = newWidth * newHeight;

	if (imageData == nullptr)
		throw std::runtime_error("Could not load data from the image file");

	setSize(newWidth, newHeight);

	for (int i = 0; i < newLength; ++i)
		pixelData[i] = imageData[i];

	stbi_image_free(imageData);
}

namespace
{
	bool endsWith(const std::string& text, const std::string& suffix)
	{
		return text.rfind(suffix) == (text.size() - suffix.size());
	}
}

void Image::saveAs(const std::string& fileName) const
{
	App::getLog().logInfo("Saving image to %s", fileName);

	int result = 0;

	if (endsWith(fileName, ".png"))
		result = stbi_write_png(fileName.c_str(), width, height, 4, (uint8_t*)pixelData, width * sizeof(uint32_t));
	else if (endsWith(fileName, ".bmp"))
		result = stbi_write_bmp(fileName.c_str(), width, height, 4, (uint8_t*)pixelData);
	else if (endsWith(fileName, ".tga"))
		result = stbi_write_tga(fileName.c_str(), width, height, 4, (uint8_t*)pixelData);
	else
		throw std::runtime_error("Could not save the image (non-supported format)");

	if (result == 0)
		throw std::runtime_error("Could not save the image");
}

void Image::setSize(int length_)
{
	setSize(length_, 1);
}

void Image::setSize(int width_, int height_)
{
	width = width_;
	height = height_;
	length = width * height;

	if (pixelData != nullptr)
	{
		_mm_free(pixelData);
		pixelData = nullptr;
	}

	if (length == 0)
		return;

	pixelData = (uint32_t*)_mm_malloc(length * sizeof(uint32_t), 64);

	if (pixelData == nullptr)
		throw std::runtime_error("Could not allocate memory for the image");

	memset(pixelData, 0, length * sizeof(uint32_t));
}

void Image::setPixel(int x, int y, const Color& color)
{
	assert(x < width && y < height);

	pixelData[y * width + x] = color.getRgbaValue();
}

void Image::setPixel(int index, const Color& color)
{
	assert(index < length);

	pixelData[index] = color.getRgbaValue();
}

void Image::swapBytes()
{
	Image swappedImage(width, height);

	for (int i = 0; i < length; ++i)
	{
		uint32_t rgba = pixelData[i];
		uint32_t abgr = 0;

		abgr |= (rgba << 24) & 0xff000000;
		abgr |= (rgba << 8) & 0x00ff0000;
		abgr |= (rgba >> 8) & 0x0000ff00;
		abgr |= (rgba >> 24) & 0x000000ff;

		swappedImage.pixelData[i] = abgr;
	}

	std::swap(pixelData, swappedImage.pixelData);
}

void Image::flip()
{
	Image flippedImage(width, height);

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
			flippedImage.pixelData[(height - 1 - y) * width + x] = pixelData[y * width + x];
	}

	std::swap(pixelData, flippedImage.pixelData);
}

int Image::getWidth() const
{
	return width;
}

int Image::getHeight() const
{
	return height;
}

int Image::getLength() const
{
	return length;
}

Color Image::getPixel(int x, int y) const
{
	assert(x < width && y < height);

	return Color::fromRgbaValue(pixelData[y * width + x]);
}

uint32_t* Image::getPixelData() const
{
	return pixelData;
}

Color Image::getPixelNearest(double u, double v) const
{
	assert(u >= 0.0 && u <= 1.0 && v >= 0.0 && v <= 1.0);

	int x = (int)(u * (double)(width - 1) + 0.5);
	int y = (int)(v * (double)(height - 1) + 0.5);

	return getPixel(x, y);
}

Color Image::getPixelLinear(double u, double v) const
{
	assert(u >= 0.0 && u <= 1.0 && v >= 0.0 && v <= 1.0);

	double dx = u * (double)(width - 1) - 0.5;
	double dy = v * (double)(height - 1) - 0.5;
	int ix = (int)dx;
	int iy = (int)dy;
	double tx2 = dx - (double)ix;
	double ty2 = dy - (double)iy;
	tx2 = MathUtils::smoothstep(tx2);
	ty2 = MathUtils::smoothstep(ty2);
	double tx1 = 1.0 - tx2;
	double ty1 = 1.0 - ty2;

	Color c11 = getPixel(ix, iy);
	Color c21 = getPixel(ix + 1, iy);
	Color c12 = getPixel(ix, iy + 1);
	Color c22 = getPixel(ix + 1, iy + 1);

	return (tx1 * c11 + tx2 * c21) * ty1 + (tx1 * c12 + tx2 * c22) * ty2;
}