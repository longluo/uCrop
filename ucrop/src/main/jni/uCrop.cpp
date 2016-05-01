//
// Created by Oleksii Shliama on 3/13/16.
//

#include <stdio.h>
#include <jni.h>
#include <vector>
#include <android/log.h>
#include "com_yalantis_ucrop_view_CropImageView.h"

using namespace std;

#define cimg_display 0
#define cimg_use_jpeg
#define cimg_use_png
#define cimg_use_openmp

#include "CImg.h"

using namespace cimg_library;

#define LOG_TAG "uCrop JNI"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

JNIEXPORT jboolean JNICALL Java_com_yalantis_ucrop_view_CropImageView_cropCImg(JNIEnv *env, jobject obj, jstring pathSource, jstring pathResult, jint left, jint top, jint width, jint height, jfloat angle) {

    LOGD("cropFileCImg");

    const char *file_source_path = env->GetStringUTFChars(pathSource, 0);
    const char *file_result_path = env->GetStringUTFChars(pathResult, 0);
    LOGD("file_source_path: %s \nfile_result_path: %s",file_source_path, file_result_path);

    try {
                LOGD("1");
        const CImg<unsigned char> img(file_source_path);
            LOGD("2");
        const int
        x0 = left, y0 = top,
        x1 = left + width, y1 = top + height;

        // Create warp field.
        CImg<float> warp(cimg::abs(x1 - x0 + 1), cimg::abs(y1 - y0 + 1), 1, 2);
            LOGD("3");
        const float
        rad = angle * cimg::PI/180,
        ca = std::cos(rad), sa = std::sin(rad),
        ux = cimg::abs(img.width() * ca), uy = cimg::abs(img.width() * sa),
        vx = cimg::abs(img.height() * sa), vy = cimg::abs(img.height() * ca),
        w2 = 0.5f * img.width(), h2 = 0.5f * img.height(),
        dw2 = 0.5f * (ux + vx), dh2 = 0.5f * (uy + vy);
            LOGD("4");
        cimg_forXY(warp, x, y) {
            const float
            u = x + x0 - dw2, v = y + y0 - dh2;

            warp(x, y, 0) = w2 + u*ca + v*sa;
            warp(x, y, 1) = h2 - u*sa + v*ca;
        }
            LOGD("5");
        img.get_warp(warp, 0, 1, 2).save(file_result_path);
        ~img;
            LOGD("6");
        env->ReleaseStringUTFChars(pathSource, file_source_path);
        env->ReleaseStringUTFChars(pathResult, file_result_path);

        return true;

    } catch (CImgInstanceException e) {
        env->ThrowNew(env->FindClass("java/lang/OutOfMemoryError"), e.what());
    } catch (CImgIOException e) {
        env->ThrowNew(env->FindClass("java/io/IOException"), e.what());
    }

    return false;
}