#include "Filter.h"
//#include <QImage.h>
#include<fstream>
#include<cmath>
#include<algorithm>
#include<vector>

template <class T>
T clamp(T value, T max, T min)
{
	if (value > max)
		return max;
	if (value < min)
		return min;
	return value;
}

QImage Filter::process(const QImage& img) const
{
	QImage result(img);
	for (int x = 0; x < img.width(); x++)
	{
		for (int y = 0; y < img.height(); y++)
		{
			QColor color = calcNewPixelColor(img, x, y);
			result.setPixelColor(x, y, color);
		}
	}
	return result;
}

QColor InvertFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	QColor color = img.pixelColor(x, y);
	color.setRgb(255 - color.red(), 255 - color.green(), 255 - color.blue());
	return color;
}



QColor MatrixFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	float returnR = 0;
	float returnG = 0;
	float returnB = 0;
	int size = mKernel.getSize();
	int radius = mKernel.getRadius();
	for (int i = -radius; i <= radius; i++)
	{
		for (int j = -radius; j <= radius; j++)
		{
			int idx = (i + radius) * size + j + radius;
			QColor color = img.pixelColor(clamp(x + j, img.width() - 1, 0), clamp(y + i, img.height() - 1, 0));
			returnR += color.red() * mKernel[idx];
			returnG += color.green() * mKernel[idx];
			returnB += color.blue() * mKernel[idx];
		}
	}

	return QColor(clamp(returnR, 225.f, 0.f), clamp(returnG, 225.f, 0.f), clamp(returnB, 225.f, 0.f));
}

template<class T, class T2>
T max(T2 a, T b)
{
	return a > b ? a : b;
}
template <class T, class T2>
T min(T2 a, T b)
{
	return a < b ? a : b;
}
float limit_color(float color)
{
	if (color <= 0.f)
		return 0.f;
	if (color >= 255.f)
		return 255.f;
	return color;
}
int limit_pixel(int pix, int x)
{
	if (pix <= 0)
		return 0;
	if (pix >= x)
		return x - 1;
	return pix;
}


QImage Invers(const QImage& image)
{
	QImage res = image;
	for (int x = 0; x < res.width() * 0.5; x++)
	{
		for (int y = 0; y < res.height(); y++)
		{
			QColor col = image.pixelColor(x, y);
			col.setRgb(255 - col.red(), 255 - col.green(), 255 - col.blue());
			res.setPixelColor(x, y, col);
		}
	}
	return res;
}
QImage Matrix(const QImage& image, std::string name_filt, char* name_file)
{
	QImage res = image;
	int n = 0, m = 0;
	float** matrix;
	float norm = 0.f;
	if (name_file != NULL)
	{
		char str[MAX_SIZE];
		std::ifstream ifs(name_file);
		ifs.getline(str, MAX_SIZE, '\n');
		n = str[0] - '0';
		m = str[2] - '0';
		matrix = new float* [n];
		for (int i = 0; i < n; i++)
			matrix[i] = new float[m];
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < m; j++)
			{
				if (j != m - 1)
					ifs.getline(str, MAX_SIZE, ' ');
				else
					ifs.getline(str, MAX_SIZE, '\n');
				matrix[i][j] = std::atof(str);
				norm += matrix[i][j];
			}
		}
		if (norm != 0 && norm != 1)
		{
			for (int i = 0; i < n; i++)
			{
				for (int j = 0; j < m; j++)
				{
					matrix[i][j] /= norm;
				}
			}
		}
	}
	else
	{
		if (name_filt == "Blur")
		{
			n = BLUR_SIZE;
			m = BLUR_SIZE;
			matrix = new float* [n];
			norm = n * m;
			for (int i = 0; i < n; i++)
			{
				matrix[i] = new float[m];
				for (int j = 0; j < m; j++)
				{
					matrix[i][j] = 1.0 / norm;
				}
			}
		}
		if (name_filt == "motion_blur")
		{
			n = motion_blur;
			m = motion_blur;
			matrix = new float* [n];
			for (int i = 0; i < n; i++)
			{
				matrix[i] = new float[m];
				for (int j = 0; j < m; j++)
				{
					if (i != j)
						matrix[i][j] = 0;
					else
					{
						matrix[i][j] = 1.f / n;
					}
				}
			}
		}
		if (name_filt == "Gauss")
		{
			n = GAUSS_SIZE;
			m = GAUSS_SIZE;
			matrix = new float* [n];
			for (int i = 0; i < n; i++)
			{
				matrix[i] = new float[m];
				for (int j = 0; j < m; j++)
				{
					matrix[i][j] = exp(-((i - n / 2) * (i - n / 2) + (j - m / 2) * (j - m / 2)) / (2 * sigma * sigma));
					norm += matrix[i][j];
				}
			}
			if (norm != 0 && norm != 1)
			{
				for (int i = 0; i < n; i++)
				{
					for (int j = 0; j < m; j++)
					{
						matrix[i][j] /= norm;
					}
				}
			}
		}
	}
	for (int x = 0; x < res.width() * 0.5; x++)
	{
		for (int y = 0; y < res.height(); y++)
		{
			int x_pix = limit_pixel(x - m / 2, res.width());
			int y_pix = limit_pixel(y - n / 2, res.height());
			float red = 0, green = 0, blue = 0;
			for (int i = 0; i < n; i++)
			{
				y_pix = limit_pixel(y - n / 2 + i, res.height());
				for (int j = 0; j < m; j++)
				{
					x_pix = limit_pixel(x - m / 2 + j, res.width());
					red += image.pixelColor(x_pix, y_pix).red() * matrix[i][j];
					green += image.pixelColor(x_pix, y_pix).green() * matrix[i][j];
					blue += image.pixelColor(x_pix, y_pix).blue() * matrix[i][j];
				}
			}
			red = limit_color(red);
			green = limit_color(green);
			blue = limit_color(blue);
			QColor new_color;
			new_color.setRgb(red, green, blue);
			res.setPixelColor(x, y, new_color);
		}
	}
	for (int i = 0; i < n; i++)
		delete[] matrix[i];
	delete[] matrix;
	return res;
}


QImage GrayScale(const QImage& image)
{
	QImage res = image;
	for (int x = 0; x < res.width() * 0.5; x++)
	{
		for (int y = 0; y < res.height(); y++)
		{
			QColor new_color;
			float intensity = image.pixelColor(x, y).red() * 0.299 + image.pixelColor(x, y).blue() * 0.114 + image.pixelColor(x, y).green() * 0.587;
			intensity = limit_color(intensity);
			new_color.setRgb(intensity, intensity, intensity);
			res.setPixelColor(x, y, new_color);
		}
	}
	return res;
}


QImage Sephia(const QImage& image)
{
	QImage res = image;
	for (int x = 0; x < res.width() * 0.5; x++)
	{
		for (int y = 0; y < res.height(); y++)
		{
			QColor new_color;
			float intensity = image.pixelColor(x, y).red() * 0.299 + image.pixelColor(x, y).blue() * 0.114 + image.pixelColor(x, y).green() * 0.587;
			new_color.setRgb(limit_color(intensity + 2 * sephia), limit_color(intensity + 0.5 * sephia), limit_color(intensity - sephia));
			res.setPixelColor(x, y, new_color);
		}
	}
	return res;
}


QImage Brigntness(const QImage& image)
{
	QImage res = image;
	for (int x = 0; x < res.width() * 0.5; x++)
	{
		for (int y = 0; y < res.height(); y++)
		{
			QColor new_color;
			float red = image.pixelColor(x, y).red() + brightness;
			red = limit_color(red);
			float green = image.pixelColor(x, y).green() + brightness;
			green = limit_color(green);
			float blue = image.pixelColor(x, y).blue() + brightness;
			blue = limit_color(blue);
			new_color.setRgb(red, green, blue);
			res.setPixelColor(x, y, new_color);
		}
	}
	return res;
}
QImage Median(const QImage& image)
{
	QImage res = image;
	for (int x = 0; x < res.width() * 0.5; x++)
	{
		for (int y = 0; y < res.height(); y++)
		{
			int x_pix = limit_pixel(x - med / 2, res.width());
			int y_pix = limit_pixel(y - med / 2, res.height());
			std::vector<float> red, green, blue;
			for (int i = 0; i < med; i++)
			{
				y_pix = limit_pixel(y - med / 2 + i, res.height());
				for (int j = 0; j < med; j++)
				{
					x_pix = limit_pixel(x - med / 2 + j, res.width());
					red.push_back(image.pixelColor(x_pix, y_pix).red());
					blue.push_back(image.pixelColor(x_pix, y_pix).blue());
					green.push_back(image.pixelColor(x_pix, y_pix).green());
				}
			}
			std::sort(red.begin(), red.end());
			std::sort(green.begin(), green.end());
			std::sort(blue.begin(), blue.end());
			QColor new_color;
			new_color.setRgb(red[(med * med - 1) / 2], green[(med * med - 1) / 2], blue[(med * med - 1) / 2]);
			res.setPixelColor(x, y, new_color);
		}
	}
	return res;
}


QImage Dilation(const QImage& image, char* name_file)
{
	char str[MAX_SIZE];
	std::ifstream ifs(name_file);
	ifs.getline(str, MAX_SIZE, '\n');
	int n = str[0] - '0';
	int m = str[2] - '0';
	int** matrix = new int* [n];
	for (int i = 0; i < n; i++)
		matrix[i] = new int[m];
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m; j++)
		{
			if (j != m - 1)
				ifs.getline(str, MAX_SIZE, ' ');
			else
				ifs.getline(str, MAX_SIZE, '\n');
			matrix[i][j] = std::atoi(str);
		}
	}
	QImage res = image;
	for (int x = 0; x < res.width() * 0.5; x++)
	{
		for (int y = 0; y < res.height(); y++)
		{
			int x_pix = limit_pixel(x - m / 2, res.width());
			int y_pix = limit_pixel(y - n / 2, res.height());
			int red = 0, green = 0, blue = 0;
			for (int i = 0; i < n; i++)
			{
				y_pix = limit_pixel(y - n / 2 + i, res.height());
				for (int j = 0; j < m; j++)
				{
					if (matrix[i][j] != 0)
					{
						x_pix = limit_pixel(x - m / 2 + j, res.width());
						red = max(red, image.pixelColor(x_pix, y_pix).red());
						green = max(green, image.pixelColor(x_pix, y_pix).green());
						blue = max(blue, image.pixelColor(x_pix, y_pix).blue());
					}
				}
			}
			QColor new_color;
			new_color.setRgb(red, green, blue);
			res.setPixelColor(x, y, new_color);
		}
	}
	return res;
}


QImage Erosion(const QImage& image, char* name_file)
{
	char str[MAX_SIZE];
	std::ifstream ifs(name_file);
	ifs.getline(str, MAX_SIZE, '\n');
	int n = str[0] - '0';
	int m = str[2] - '0';
	int** matrix = new int* [n];
	for (int i = 0; i < n; i++)
		matrix[i] = new int[m];
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m; j++)
		{
			if (j != m - 1)
				ifs.getline(str, MAX_SIZE, ' ');
			else
				ifs.getline(str, MAX_SIZE, '\n');
			matrix[i][j] = std::atoi(str);
		}
	}
	QImage res = image;
	for (int x = 0; x < res.width() * 0.5; x++)
	{
		for (int y = 0; y < res.height(); y++)
		{
			int x_pix = limit_pixel(x - m / 2, res.width());
			int y_pix = limit_pixel(y - n / 2, res.height());
			int red = 255, green = 255, blue = 255;
			for (int i = 0; i < n; i++)
			{
				y_pix = limit_pixel(y - n / 2 + i, res.height());
				for (int j = 0; j < m; j++)
				{
					if (matrix[i][j] != 0)
					{
						x_pix = limit_pixel(x - m / 2 + j, res.width());
						red = min(red, image.pixelColor(x_pix, y_pix).red());
						green = min(green, image.pixelColor(x_pix, y_pix).green());
						blue = min(blue, image.pixelColor(x_pix, y_pix).blue());
					}
				}
			}
			QColor new_color;
			new_color.setRgb(red, green, blue);
			res.setPixelColor(x, y, new_color);
		}
	}
	return res;
}


QImage Waves(const QImage& image)
{
	QImage res = image;
	for (int k = 0; k < res.width() * 0.5; k++)
	{
		for (int l = 0; l < res.height(); l++)
		{
			int y_pix = l;
			int x_pix = limit_pixel(k + 20 * sin(2 * PI * l / 60), image.width());
			QColor new_color;
			new_color.setRgb(image.pixelColor(x_pix, y_pix).red(), image.pixelColor(x_pix, y_pix).green(), image.pixelColor(x_pix, y_pix).blue());
			res.setPixelColor(k, l, new_color);
		}
	}
	return res;
}


QImage Transfer(const QImage& image)
{
	QImage res = image;
	for (int k = 0; k < res.width(); k++)
	{
		for (int l = 0; l < res.height(); l++)
		{
			int x_pix = k + 100;
			int y_pix = l;
			QColor new_color;
			if (x_pix >= image.width())
				new_color.setRgb(0, 0, 0);
			else
				new_color.setRgb(image.pixelColor(x_pix, y_pix).red(), image.pixelColor(x_pix, y_pix).green(), image.pixelColor(x_pix, y_pix).blue());
			res.setPixelColor(k, l, new_color);
		}
	}
	return res;
}
QImage Turn(const QImage& image)
{
	QImage res = image;
	int x0 = res.width() * 0.5;
	int y0 = res.height() * 0.5;
	for (int k = 0; k < res.width(); k++)
	{
		for (int l = 0; l < res.height(); l++)
		{
			int x_pix = (k - x0) * cos(45) - (l - y0) * sin(45) + x0;
			int y_pix = (k - x0) * sin(45) + (l - y0) * cos(45) + y0;
			QColor new_color;
			if (x_pix >= image.width() || y_pix >= image.height() || x_pix < 0 || y_pix < 0)
				new_color.setRgb(0, 0, 0);
			else
				new_color.setRgb(image.pixelColor(x_pix, y_pix).red(), image.pixelColor(x_pix, y_pix).green(), image.pixelColor(x_pix, y_pix).blue());
			res.setPixelColor(k, l, new_color);
		}
	}
	return res;
}
QImage Lin_gist(const QImage& image)
{
	QImage res = image;
	int rMax = 0, gMax = 0, bMax = 0;
	int rMin = 255, gMin = 255, bMin = 255;
	for (int x = 0; x < res.width(); x++)
	{
		for (int y = 0; y < res.height(); y++)
		{
			rMax = max(rMax, image.pixelColor(x, y).red());
			gMax = max(gMax, image.pixelColor(x, y).green());
			bMax = max(bMax, image.pixelColor(x, y).blue());
			rMin = min(rMin, image.pixelColor(x, y).red());
			gMin = min(gMin, image.pixelColor(x, y).green());
			bMin = min(bMin, image.pixelColor(x, y).blue());
		}
	}
	for (int x = 0; x < res.width() * 0.5; x++)
	{
		for (int y = 0; y < res.height(); y++)
		{
			QColor new_color;
			new_color.setRgb((image.pixelColor(x, y).red() - rMin) * 255 / (rMax - rMin), (image.pixelColor(x, y).green() - gMin) * 255 / (gMax - gMin), (image.pixelColor(x, y).blue() - bMin) * 255 / (bMax - bMin));
			res.setPixelColor(x, y, new_color);
		}
	}
	return res;
}
QImage Perfect_refl(const QImage& image)
{
	QImage res = image;
	int rMax = 0, gMax = 0, bMax = 0;
	for (int x = 0; x < res.width(); x++)
	{
		for (int y = 0; y < res.height(); y++)
		{
			rMax = max(rMax, image.pixelColor(x, y).red());
			gMax = max(gMax, image.pixelColor(x, y).green());
			bMax = max(bMax, image.pixelColor(x, y).blue());
		}
	}
	for (int x = 0; x < res.width() * 0.5; x++)
	{
		for (int y = 0; y < res.height(); y++)
		{
			QColor new_color;
			new_color.setRgb(image.pixelColor(x, y).red() * 255 / rMax, image.pixelColor(x, y).green() * 255 / gMax, image.pixelColor(x, y).blue() * 255 / bMax);
			res.setPixelColor(x, y, new_color);
		}
	}
	return res;
}


QImage Sobel(const QImage& image, char* name_file1, char* name_file2)
{
	QImage res = image;
	char str[MAX_SIZE];
	std::ifstream ifsX(name_file1);
	ifsX.getline(str, MAX_SIZE, '\n');
	int nX = str[0] - '0';
	int mX = str[2] - '0';
	int normX = 0;
	int** matrixX = new int* [nX];
	for (int i = 0; i < nX; i++)
		matrixX[i] = new int[mX];
	for (int i = 0; i < nX; i++)
	{
		for (int j = 0; j < mX; j++)
		{
			if (j != mX - 1)
				ifsX.getline(str, MAX_SIZE, ' ');
			else
				ifsX.getline(str, MAX_SIZE, '\n');
			matrixX[i][j] = std::atoi(str);
			normX += matrixX[i][j];
		}
	}
	if (normX != 0 && normX != 1)
	{
		for (int i = 0; i < nX; i++)
		{
			for (int j = 0; j < mX; j++)
			{
				matrixX[i][j] /= normX;
			}
		}
	}
	std::ifstream ifsY(name_file2);
	ifsY.getline(str, MAX_SIZE, '\n');
	int nY = str[0] - '0';
	int mY = str[2] - '0';
	int normY = 0;
	int** matrixY = new int* [nY];
	for (int i = 0; i < nY; i++)
		matrixY[i] = new int[mY];
	for (int i = 0; i < nY; i++)
	{
		for (int j = 0; j < mY; j++)
		{
			if (j != mY - 1)
				ifsY.getline(str, MAX_SIZE, ' ');
			else
				ifsY.getline(str, MAX_SIZE, '\n');
			matrixY[i][j] = std::atoi(str);
			normY += matrixY[i][j];
		}
	}
	if (normY != 0 && normY != 1)
	{
		for (int i = 0; i < nY; i++)
		{
			for (int j = 0; j < mY; j++)
			{
				matrixY[i][j] /= normY;
			}
		}
	}
	for (int x = 0; x < res.width() * 0.5; x++)
	{
		for (int y = 0; y < res.height(); y++)
		{
			int x_pix = limit_pixel(x - mX / 2, res.width());
			int y_pix = limit_pixel(y - nX / 2, res.height());
			int redX = 0, greenX = 0, blueX = 0;
			for (int i = 0; i < nX; i++)
			{
				y_pix = limit_pixel(y - nX + i / 2 + i, res.height());
				for (int j = 0; j < mX; j++)
				{
					x_pix = limit_pixel(x - mX / 2 + j, res.width());
					redX += image.pixelColor(x_pix, y_pix).red() * matrixX[i][j];
					greenX += image.pixelColor(x_pix, y_pix).green() * matrixX[i][j];
					blueX += image.pixelColor(x_pix, y_pix).blue() * matrixX[i][j];
				}
			}
			x_pix = limit_pixel(x - mY / 2, res.width());
			y_pix = limit_pixel(y - nY / 2, res.height());
			int redY = 0, greenY = 0, blueY = 0;
			for (int i = 0; i < nY; i++)
			{
				y_pix = limit_pixel(y - nY + i / 2 + i, res.height());
				for (int j = 0; j < mY; j++)
				{
					x_pix = limit_pixel(x - mY / 2 + j, res.width());
					redY += image.pixelColor(x_pix, y_pix).red() * matrixY[i][j];
					greenY += image.pixelColor(x_pix, y_pix).green() * matrixY[i][j];
					blueY += image.pixelColor(x_pix, y_pix).blue() * matrixY[i][j];
				}
			}
			QColor new_color;
			new_color.setRgb(limit_color(sqrt(redX * redX + redY * redY)), limit_color(sqrt(greenX * greenX + greenY * greenY)), limit_color(sqrt(blueX * blueX + blueY * blueY)));
			res.setPixelColor(x, y, new_color);
		}
	}
	for (int i = 0; i < nX; i++)
		delete[] matrixX[i];
	delete[]matrixX;
	for (int i = 0; i < nY; i++)
		delete[] matrixY[i];
	delete[]matrixY;
	return res;
}


QImage Grad(const QImage& image, char* file_name)
{
	QImage res = image;
	std::ifstream ifs(file_name);
	char str[MAX_SIZE];
	ifs.getline(str, MAX_SIZE, '\n');
	int n = str[0] - '0';
	int m = str[0] - '0';
	int** matrix = new int* [n];
	for (int i = 0; i < n; i++)
	{
		matrix[i] = new int[m];
		for (int j = 0; j < m; j++)
		{
			if (j != m - 1)
				ifs.getline(str, MAX_SIZE, ' ');
			else
				ifs.getline(str, MAX_SIZE, '\n');
			matrix[i][j] = std::atoi(str);
		}
	}
	for (int x = 0; x < res.width() * 0.5; x++)
	{
		for (int y = 0; y < res.height(); y++)
		{
			int x_pix = limit_pixel(x - m / 2, res.width());
			int y_pix = limit_pixel(y - n / 2, res.height());
			int minR = 255, minG = 255, minB = 255, maxR = 0, maxG = 0, maxB = 0;
			for (int i = 0; i < n; i++)
			{
				y_pix = limit_pixel(y - n / 2 + i, res.height());
				for (int j = 0; j < m; j++)
				{
					x_pix = limit_pixel(x - m / 2 + j, res.width());
					minR = min(image.pixelColor(x_pix, y_pix).red(), minR);
					minG = min(image.pixelColor(x_pix, y_pix).green(), minG);
					minB = min(image.pixelColor(x_pix, y_pix).blue(), minB);
					maxR = max(image.pixelColor(x_pix, y_pix).red(), maxR);
					maxG = max(image.pixelColor(x_pix, y_pix).green(), maxG);
					maxB = max(image.pixelColor(x_pix, y_pix).blue(), maxB);
				}
				QColor new_color;
				new_color.setRgb(maxR - minR, maxG - minG, maxB - minB);
				res.setPixelColor(x, y, new_color);

			}
		}
	}
	return res;
}