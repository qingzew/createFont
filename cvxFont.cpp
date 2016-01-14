#include "cvxFont.hpp"
#include <cstdlib>
#include <ctime>


CvxFont::CvxFont(std::string fontFile) {
    //initialize library
    FT_Error err1 = FT_Init_FreeType(&library);
    if (err1) {
        std::cerr << "init library error" << std::endl;
        std::exit(1);
    }
    //create face object
    FT_Error err2 = FT_New_Face(library, fontFile.c_str(), 0, &face);
    if (err2) {
        std::cerr << "create face object error" << std::endl;
        std::exit(1);
    }
    setMatrix();
    setPen();
    setFontSize();
}

CvxFont::~CvxFont() {
    FT_Done_Face(face);
    FT_Done_FreeType(library);
}


void CvxFont::setMatrix(double angle) {
    /* set up matrix */
//    matrix.xx = (FT_Fixed)(cos(angle) * 0x10000L);
//    matrix.xy = (FT_Fixed)(sin(angle) * 0x10000L);
//    matrix.yx = (FT_Fixed)(sin(angle) * 0x10000L);
//    matrix.yy = (FT_Fixed)(cos(angle) * 0x10000L);

    matrix.xx = 0x10000L;
    matrix.xy = angle * 0x10000L;
    matrix.yx = 0;
    matrix.yy = 0x10000L;


}

void CvxFont::setPen(cv::Point pos) {
    pen.x = pos.x << 6;
    pen.y = pos.y << 6;
}

void CvxFont::setFontSize(FT_UInt fontSize) {
    this->fontSize = fontSize;
}

void CvxFont::strToMat(cv::Mat &img, std::wstring str, cv::Scalar fontColor) {
    FT_UInt previous = 0;
    for (int n=0; n<str.size(); n++) {
        FT_UInt glyphIndex = FT_Get_Char_Index(face, str[n]);

        FT_Error err0 = FT_Set_Pixel_Sizes(face, 0, fontSize);
        if (err0) {
            std::cerr << "FT_Set_Pixel_Sizes error" << std::endl;
            continue;
        }

        FT_Set_Transform(face, &matrix, &pen);

        FT_Error err1 = FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
        if (err1) {
            std::cerr << "FT_Load_Glyph error" << std::endl;
            continue;
        }

        FT_Error err2 = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
        if (err2) {
            std::cerr << "FT_Render_Glyph error" << std::endl;
            continue;
        }


        FT_Bool useKerning = FT_HAS_KERNING(face);
        if (useKerning &&  previous && glyphIndex) {
            FT_Vector delta;
            FT_Get_Kerning(face, previous, glyphIndex, FT_KERNING_DEFAULT, &delta);

            pen.x += delta.x >> 6;
        }

        FT_GlyphSlot slot = face->glyph;
        FT_Bitmap bitmap = slot->bitmap;

        int left = slot->bitmap_left;
        int top = slot->bitmap_top;
        int rows = slot->bitmap.rows;
        int width = slot->bitmap.width;
       for (int x=0; x<rows; x++) {
           for (int y=0; y<width; y++) {
               if (bitmap.buffer[x * width + y]) {
                   if (y+left>=0 && y+left<img.cols && x+fontSize-top>=0 && x+fontSize-top<img.rows) {
                       cv::Vec3b scalar = img.at<cv::Vec3b>(x + fontSize - top, y + left);
                       for (int k=0; k<3; k++) {
                           scalar.val[k] = fontColor.val[k];
                       }

                       img.at<cv::Vec3b>(x + fontSize - top, y + left) = scalar;
                   } else {
                       continue;
                   }

               }
           }
       }


        pen.x += slot->metrics.horiAdvance;
        previous = glyphIndex;

#ifdef DEBUG
        std::cout << "width: " << bitmap.width <<std::endl;
        std::cout << "rows: " << bitmap.rows <<std::endl;
        std::cout << "bitmap_left: " << slot->bitmap_left << std::endl;
        std::cout << "bitmap_top: " << slot->bitmap_top << std::endl;
        std::cout << "left: " << left << std::endl;
        std::cout << "top: " << top << std::endl;
        std::cout << "pen.x: " << pen.x << std::endl;
        std::cout << "pen.y: " << pen.y << std::endl;
        std::cout << "fontSize: "<< fontSize  << std::endl;
        std::cout << "*********************" << std::endl;
#endif


    }
}

//void CvxFont::charToMat(cv::Mat &img, wchar_t *ch, cv::Scalar fontColor) {
//    FT_UInt glyphIndex = FT_Get_Char_Index(face, *ch);
//
//    FT_Error err0 = FT_Set_Pixel_Sizes(face, 0, fontSize);
//    if (err0) {
//        std::cerr << "FT_Set_Pixel_Sizes error" << std::endl;
//    }
//    FT_Set_Transform(face, &matrix, &pen);
//
//    FT_Error err1 = FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
//    if (err1) {
//        std::cerr << "FT_Load_Glyph error" << std::endl;
//    }
//
//    FT_Error err2 = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
//    if (err2) {
//        std::cerr << "FT_Render_Glyph error" << std::endl;
//    }
//
//
//    FT_GlyphSlot slot = face->glyph;
//    FT_Bitmap bitmap = slot->bitmap;
//
//    int left = slot->bitmap_left;
//    int top = slot->bitmap_top - bitmap.rows;
//
//#ifdef DEBUG
//    std::cout << "width: " << bitmap.width <<std::endl;
//    std::cout << "rows: " << bitmap.rows <<std::endl;
//    std::cout << "bitmap_left: " << slot->bitmap_left << std::endl;
//    std::cout << "bitmap_top: " << slot->bitmap_top << std::endl;
//    std::cout << "img seize: " << img.size() << std::endl;
//    std::cout << "left: " << left << std::endl;
//    std::cout << "top: " << top << std::endl;
//#endif
//
//    for (int i=0; i<bitmap.width; i++) {
//        for (int j=0; j<bitmap.rows; j++) {
//            //if (bitmap.buffer[j * bitmap.width + i] & (0xC0 >> (j%8))) {
//            if ((int)bitmap.buffer[j * bitmap.width + i]) {
//                std::cout << (int)bitmap.buffer[j * bitmap.width + i] << " ";
//                if (top+j>=0 && top+j<img.rows && left+i>=0  && left+i<img.cols) {
//                    cv::Vec3b scalar = img.at<cv::Vec3b>(top+j, left+i);
//                    for (int k=0; k<3; k++) {
//                        scalar.val[k] = fontColor.val[k];
//                    }
//
//                    img.at<cv::Vec3b>(top+j, left+i) = scalar;
//                } else {
//                    continue;
//                }
//
//            }
//        }
//
//    }
//}

cv::Mat CvxFont::charToMat(cv::Mat &img, wchar_t *ch, cv::Scalar fontColor) {
    FT_UInt glyphIndex = FT_Get_Char_Index(face, *ch);

    FT_Error err0 = FT_Set_Pixel_Sizes(face, 0, fontSize);
    if (err0) {
        std::cerr << "FT_Set_Pixel_Sizes error" << std::endl;
    }
    FT_Set_Transform(face, &matrix, &pen);

    FT_Error err1 = FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
    if (err1) {
        std::cerr << "FT_Load_Glyph error" << std::endl;
    }

    FT_Error err2 = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
    if (err2) {
        std::cerr << "FT_Render_Glyph error" << std::endl;
    }


    FT_GlyphSlot slot = face->glyph;
    FT_Bitmap bitmap = slot->bitmap;

    cv::Mat font(bitmap.rows, bitmap.width, CV_8UC1, bitmap.buffer, bitmap.pitch);
    cv::bitwise_not(font, font);
    cv::imwrite("font.png", font);

    std::srand(std::time(0));
    int left = (slot->bitmap_left + bitmap.width > img.cols) ? (img.cols - bitmap.width <= 0 ? 0 : std::rand() % (img.cols - bitmap.width)) : slot->bitmap_left;
    int top = (slot->bitmap_top > img.rows) ? 0 : (slot->bitmap_top - bitmap.rows);
//    cv::Mat mask = font;
//    cv::threshold(font, mask, 244, 255, cv::THRESH_BINARY);
//
    cv::Mat roi = img(cv::Rect(left, top, bitmap.width, bitmap.rows));
    font.copyTo(roi);
    return img;
}


