#pragma once
#include <QImage.h>
#include <iostream>

//���������� 
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

//������� ��� ���������������� ����������
QImage GrayScale(const QImage& img);
QImage Sephia(const QImage& image);
QImage Brigntness(const QImage& image);
QImage Sobel(const QImage& image, char* file_name1, char* file_name2);

//������� ��� ����� ������������ ������
QImage Invers(const QImage& img);
QImage Matrix(const QImage& im, std::string name_filt, char* name_file = NULL);
QImage Median(const QImage& image);                     //��������� ������

QImage Dilation(const QImage& image, char* name_file);
QImage Erosion(const QImage& image, char* name_file);

QImage Waves(const QImage& image);                      //�����
QImage Transfer(const QImage& image);                   //�������
QImage Turn(const QImage& image);                       //�������
QImage Lin_gist(const QImage& image);                   //�������� ����������
QImage Perfect_refl(const QImage& image);               //��������� ����������
QImage Grad(const QImage& image, char* file_name);      //Grad
