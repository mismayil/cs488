#include "Image.hpp"
#include <cstring>

#include <iostream>

#include <lodepng/lodepng.h>

const uint Image::m_colorComponents = 3; // Red, blue, green

//---------------------------------------------------------------------------------------
Image::Image()
  : m_width(0),
    m_height(0),
    m_data(0),
    uv(NULL)
{
}

//---------------------------------------------------------------------------------------
Image::Image(
		uint width,
		uint height
)
  : m_width(width),
    m_height(height)
{
	size_t numElements = m_width * m_height * m_colorComponents;
	m_data = new double[numElements];
	memset(m_data, 0, numElements*sizeof(double));
    uv = NULL;
}

//---------------------------------------------------------------------------------------
Image::Image(const Image & other)
  : m_width(other.m_width),
    m_height(other.m_height),
    m_data(other.m_data ? new double[m_width * m_height * m_colorComponents] : 0)
{
  if (m_data) {
    std::memcpy(m_data, other.m_data,
                m_width * m_height * m_colorComponents * sizeof(double));
  }
}

//---------------------------------------------------------------------------------------
Image::~Image()
{
  delete [] m_data;

  if (uv) {
      for (uint y = 0; y < m_height; y++) {
          delete [] uv[y];
      }

      delete [] uv;
  }
}

//---------------------------------------------------------------------------------------
Image & Image::operator=(const Image& other)
{
  delete [] m_data;

  m_width = other.m_width;
  m_height = other.m_height;
  m_data = (other.m_data ? new double[m_width * m_height * m_colorComponents] : 0);

  if (m_data) {
    std::memcpy(m_data,
                other.m_data,
                m_width * m_height * m_colorComponents * sizeof(double)
    );
  }

  return *this;
}

//---------------------------------------------------------------------------------------
uint Image::width() const
{
  return m_width;
}

//---------------------------------------------------------------------------------------
uint Image::height() const
{
  return m_height;
}

glm::vec3** Image::getuv() const {
    return uv;
}

//---------------------------------------------------------------------------------------
double Image::operator()(uint x, uint y, uint i) const
{
  return m_data[m_colorComponents * (m_width * y + x) + i];
}

//---------------------------------------------------------------------------------------
double & Image::operator()(uint x, uint y, uint i)
{
  return m_data[m_colorComponents * (m_width * y + x) + i];
}

//---------------------------------------------------------------------------------------
bool Image::savePng(const std::string & filename)
{
	std::vector<unsigned char> image;

	image.resize(m_width * m_height * m_colorComponents);

	double color;
	for (uint y(0); y < m_height; y++) {
		for (uint x(0); x < m_width; x++) {
			for (uint i(0); i < m_colorComponents; ++i) {
				color = m_data[m_colorComponents * (m_width * y + x) + i];
				image[m_colorComponents * (m_width * y + x) + i] = (unsigned char)(255 * color);
			}
		}
	}

	// Encode the image
	unsigned error = lodepng::encode(filename, image, m_width, m_height, LCT_RGB);

	if(error) {
		std::cerr << "encoder error " << error << ": " << lodepng_error_text(error)
				<< std::endl;
	}

	return true;
}

void Image::loadPng(const char* filename)
{
  std::vector<unsigned char> image; //the raw pixels

  //decode
  unsigned error = lodepng::decode(image, m_width, m_height, filename, LCT_RGB);

  if(error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

  uv = new glm::vec3*[m_height];

  for (uint y = 0; y < m_height; y++) {
      uv[y] = new glm::vec3[m_width];
  }

  for (uint y = 0; y < m_height; y++) {
      for (uint x = 0; x < m_width; x++) {
          for (uint i = 0; i < m_colorComponents; i++) {
              uv[y][x][i] = (float) image[m_colorComponents * (m_width * (m_height - y - 1) + x) + i] / 255.0f;
          }
      }
  }
}

//---------------------------------------------------------------------------------------
const double * Image::data() const
{
  return m_data;
}

//---------------------------------------------------------------------------------------
double * Image::data()
{
  return m_data;
}
