#ifndef CVXFONT_HPP
#define CVXFONT_HPP


#include <cstdio>
#include <cstring>
#include <cmath>
#include <vector>
#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include <opencv2/opencv.hpp>

//#define DEBUG

typedef struct  TGlyph_
{
  FT_UInt    index;  /* glyph index                  */
  FT_Vector  pos;    /* glyph origin on the baseline */
  FT_Glyph   image;  /* glyph image                  */

} TGlyph, *PGlyph;


class CvxFont {
    public:
        CvxFont(std::string);
        ~CvxFont();
        void setMatrix(double angle = 0);
        void setPen(cv::Point pos = cv::Point(0, 0));
		void setFontSize(FT_UInt fontSize = 32);

		void strToMat(cv::Mat &img, std::wstring,  cv::Scalar fontColor);
//		void charToMat(cv::Mat &img, wchar_t *ch, cv::Scalar fontColor);
        cv::Mat charToMat(cv::Mat &img, wchar_t *ch, cv::Scalar fontColor);
    private:
        FT_Face face;
        FT_Library library;
        FT_Matrix matrix;                 /* transformation matrix */
        FT_Vector pen;                    /* untransformed origin  */
		FT_UInt fontSize;

		std::vector<TGlyph> glyphs;
};

#endif
