#pragma once
#include <QImage.h>
#include <iostream>

//Переменные 
const double PI = 3.141592;
const int MAX_SIZE = 10;
const int BLUR_SIZE = 5;
const int GAUSS_SIZE = 7;
const float sigma = 2.f;
const float sephia = 15.f;
const float brightness = 50.f;
const int med = 5;
const int motion_blur = 7;
const int gist = 3;

template<class T, class T2>
T max(T2 a, T b);
template<class T, class T2>
T min(T2 a, T b);
float limit_color(float color);
int limit_pixel(int pix, int x);

//Задания для самостоятельного выполнения
QImage GrayScale(const QImage& img);
QImage Sephia(const QImage& image);
QImage Brigntness(const QImage& image);
QImage Sobel(const QImage& image, char* file_name1, char* file_name2);

//Задания для сдачи лабораторной работы
QImage Invers(const QImage& img);
QImage Matrix(const QImage& im, std::string name_filt, char* name_file = NULL);
QImage Median(const QImage& image);                     //Медианный фильтр

QImage Dilation(const QImage& image, char* name_file);
QImage Erosion(const QImage& image, char* name_file);

QImage Waves(const QImage& image);                      //Волны
QImage Transfer(const QImage& image);                   //Перенос
QImage Turn(const QImage& image);                       //Поворот
QImage Lin_gist(const QImage& image);                   //Линейное растяжение
QImage Perfect_refl(const QImage& image);               //Идеальный отражатель
QImage Grad(const QImage& image, char* file_name);      //Grad

class Filter
{
protected:
	virtual QColor calcNewPixelColor(const QImage& img, int x, int y) const = 0;
public:
	virtual ~Filter() = default;
	virtual QImage process(const QImage & img) const;
};

class InvertFilter : public Filter
{
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
};

//ядро
class Kernel
{
protected:
	std::unique_ptr<float[]> data;
	std::size_t radius;
	std::size_t getLen() const { return getSize() * getSize(); }
public:
	Kernel(std::size_t radius) : radius(radius)
	{
		data = std::make_unique<float[]>(getLen());
	}

	Kernel(const Kernel& other) : Kernel(other.radius)
	{
		std::copy(other.data.get(), other.data.get() + getLen(), data.get());
	}

	std::size_t getRadius()const { return radius; }
	std::size_t getSize() const { return 2 * radius + 1; }
	float operator [] (std::size_t id) const { return data[id]; }
	float& operator [] (std::size_t id) { return data[id]; }
};

//Свертка с ядром
class MatrixFilter : public Filter
{
protected:
	Kernel mKernel;
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
public:
	MatrixFilter(const Kernel& kernel) : mKernel(kernel) {};
	virtual ~MatrixFilter() = default; 
};

//Фильтр размытия
class BlurKernel : public Kernel
{
public:
	using Kernel::Kernel;
	BlurKernel(std::size_t radius = 2) : Kernel(radius)
	{
		for (std::size_t i = 0; i < getLen(); i++)
		{
			data[i] = 1.0f / getLen();
		}
	}
};


class BlurFilter : public MatrixFilter
{
public:
	BlurFilter(std::size_t radius = 1) : MatrixFilter(BlurKernel(radius)) {}
};

class GaussianKernel : public Kernel
{
public:
	using Kernel :: Kernel;
	GaussianKernel(std::size_t radius = 2, float sigma = 3.f) : Kernel(radius)
	{
		float norm = 0;
		int signed_radius = static_cast<int>(radius);
		for (int x = -signed_radius; x <= signed_radius; x++)
		{
			for (int y = -signed_radius; y <= signed_radius; y++)
			{
				std::size_t idx = (x + radius) * getSize() + (y + radius);
				data[idx] = std::exp(-(x * x + y * y) / (sigma * sigma));
				norm += data[idx];
			}
		}
		for (std::size_t i = 0; i, getLen(); i++)
		{
			data[i] /= norm;
		}
	}
};

class GaussianFilter : public MatrixFilter
{
public:
	GaussianFilter(std::size_t radius = 1) : MatrixFilter(GaussianKernel(radius)) {}
};
