#include <QImage.h>
#include <string>
#include <iostream>
#include "Filter.h"
using namespace std;

void main(int argc, char* argv[])
{
	QImage image, image_new;
	string s, c, strFileImage1, strFileImage2;
	for (int i = 0; i < argc; i++)
	{
		if (strcmp(argv[i], "-p") == 0)
		{
			s = argv[i + 1];
			//c = argv[i + 2];
			break;
		}
	}

	strFileImage1 = s + "image\\t_img.jpg";
	QString ss = (QString)strFileImage1.c_str();
	if (!image.load(ss))
	{
		cout << "error load";
	}

	//Фильтры
	Invers(image).save(QString((s + "result\\Invert.png").c_str()));
	GrayScale(image).save(QString((s + "result\\GrayScale.png").c_str()));
	Sephia(image).save(QString((s + "result\\Sephia.png").c_str()));
	Brigntness(image).save(QString((s + "result\\Brigntness.png").c_str()));
	Waves(image).save(QString((s + "result\\Waves.png").c_str()));
	Transfer(image).save(QString((s + "result\\Transfer.png").c_str()));
	Turn(image).save(QString((s + "result\\Turn.png").c_str()));     
	Sobel(image, "sobel_x.txt", "sobel_y.txt").save(QString((s + "result\\Sobel.png").c_str()));
	Median(image).save(QString((s + "result\\Median.png").c_str()));

	//Мат.морфология
	QImage gradPH = Grad(image, "matmorf_struct_elem.txt");
	gradPH.save(QString((s + "result\\Grad.png").c_str()));
	Invers(gradPH).save(QString((s + "result\\GradInvers.png").c_str()));
	QImage dilation = Dilation(image, "matmorf_struct_elem.txt");
	dilation.save(QString((s + "result\\Dilation.png").c_str()));
	QImage erosion = Erosion(image, "matmorf_struct_elem.txt");
	erosion.save(QString((s + "result\\Erosion.png").c_str()));
	Dilation(erosion, "matmorf_struct_elem.txt").save(QString((s + "result\\Opening.png").c_str()));
	Erosion(dilation, "matmorf_struct_elem.txt").save(QString((s + "result\\Closing.png").c_str()));
	

	Matrix(image, "", "matrix.txt").save(QString((s + "result\\Matrix.png").c_str()));
	Matrix(image, "Blur").save(QString((s + "result\\Blur.png").c_str()));
	Matrix(image, "Gauss").save(QString((s + "result\\Gauss.png").c_str()));
	Matrix(image, "", "sobel_x.txt").save(QString((s + "result\\Sobel_x.png").c_str()));
	Matrix(image, "", "sobel_y.txt").save(QString((s + "result\\Sobel_y.png").c_str()));
	Matrix(image, "", "sharp.txt").save(QString((s + "result\\Sharp.png").c_str()));
	Matrix(image, "", "oper_sharr_x.txt").save(QString((s + "result\\Oper_sharr_x.png").c_str()));
	Matrix(image, "", "oper_sharr_y.txt"). save(QString((s + "result\\Oper_sharr_y.png").c_str()));
	Matrix(image, "motion_blur").save(QString((s + "result\\Motion_blur.png").c_str()));
	
	//Не работает
	//Lin_gist(image).save(QString((s + "result\\Lin_gist.png").c_str()));          
	//Perfect_refl(image).save(QString((s + "result\\Perfect_refl.png").c_str()));
	
}
   
