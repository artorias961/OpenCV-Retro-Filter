// gba_filter.cpp
// Build (OpenCV 4.x):
//   g++ -O2 -std=c++17 gba_filter.cpp -o gba_filter `pkg-config --cflags --libs opencv4`
//
// Usage:
//   ./gba_filter input.jpg output_gba.png

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

static const int BAYER8[8][8] = {
    { 0, 48, 12, 60,  3, 51, 15, 63},
    {32, 16, 44, 28, 35, 19, 47, 31},
    { 8, 56,  4, 52, 11, 59,  7, 55},
    {40, 24, 36, 20, 43, 27, 39, 23},
    { 2, 50, 14, 62,  1, 49, 13, 61},
    {34, 18, 46, 30, 33, 17, 45, 29},
    {10, 58,  6, 54,  9, 57,  5, 53},
    {42, 26, 38, 22, 41, 25, 37, 21}
};

static inline uchar clampU8(int v) {
    return (uchar)std::max(0, std::min(255, v));
}

cv::Mat applyOrderedDither(const cv::Mat& bgr, int strength) {
    CV_Assert(bgr.type() == CV_8UC3);
    if (strength <= 0) return bgr.clone();

    cv::Mat out = bgr.clone();
    const int h = out.rows;
    const int w = out.cols;

    for (int y = 0; y < h; ++y) {
        const int by = y & 7;
        for (int x = 0; x < w; ++x) {
            const int bx = x & 7;
            const int t = BAYER8[by][bx]; // 0..63

            // Map to roughly [-strength/2, +strength/2]
            const float norm = (float(t) - 31.5f) / 63.0f;
            const int offset = (int)std::lround(norm * float(strength));

            cv::Vec3b& p = out.at<cv::Vec3b>(y, x);
            p[0] = clampU8(int(p[0]) + offset);
            p[1] = clampU8(int(p[1]) + offset);
            p[2] = clampU8(int(p[2]) + offset);
        }
    }
    return out;
}

cv::Mat kmeansQuantize(const cv::Mat& bgr, int K, int attempts = 3) {
    CV_Assert(bgr.type() == CV_8UC3);
    CV_Assert(K >= 2);

    cv::Mat samples;
    bgr.convertTo(samples, CV_32F);
    samples = samples.reshape(1, bgr.rows * bgr.cols); // Nx3 (since 3 channels -> 3 cols)

    cv::Mat labels, centers;

    cv::TermCriteria criteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 1.0);
    cv::kmeans(samples, K, labels, criteria, attempts, cv::KMEANS_PP_CENTERS, centers);

    // centers: Kx3 float
    centers.convertTo(centers, CV_8U);

    cv::Mat out(bgr.rows * bgr.cols, 3, CV_8U);
    for (int i = 0; i < labels.rows; ++i) {
        int ci = labels.at<int>(i, 0);
        out.at<uchar>(i, 0) = centers.at<uchar>(ci, 0);
        out.at<uchar>(i, 1) = centers.at<uchar>(ci, 1);
        out.at<uchar>(i, 2) = centers.at<uchar>(ci, 2);
    }

    out = out.reshape(3, bgr.rows); // back to HxWx3
    return out;
}

cv::Mat gbaRetroFilter(
    const cv::Mat& inputBgr,
    int targetWidth = 240,
    int paletteColors = 16,
    int ditherStrength = 18,
    bool addEdgeHint = true
) {
    CV_Assert(inputBgr.type() == CV_8UC3);

    cv::Mat bgr = inputBgr.clone();
    const int H = bgr.rows;
    const int W = bgr.cols;

    // 1) Mild contrast punch via YCrCb luma scaling
    {
        cv::Mat ycc;
        cv::cvtColor(bgr, ycc, cv::COLOR_BGR2YCrCb);
        std::vector<cv::Mat> ch;
        cv::split(ycc, ch);

        // y = alpha*y + beta (convertScaleAbs clamps)
        // tweak these if needed
        ch[0].convertTo(ch[0], -1, 1.10, 4.0);

        cv::merge(ch, ycc);
        cv::cvtColor(ycc, bgr, cv::COLOR_YCrCb2BGR);
    }

    // 2) Downscale for pixelation base
    const float scale = float(targetWidth) / float(W);
    const int targetHeight = std::max(1, int(std::lround(H * scale)));
    cv::Mat small;
    cv::resize(bgr, small, cv::Size(targetWidth, targetHeight), 0, 0, cv::INTER_AREA);

    // 3) Optional edge hint
    if (addEdgeHint) {
        cv::Mat gray, edges;
        cv::cvtColor(small, gray, cv::COLOR_BGR2GRAY);
        cv::Canny(gray, edges, 60, 140);
        cv::dilate(edges, edges, cv::Mat(), cv::Point(-1, -1), 1);

        cv::Mat edgesBgr;
        cv::cvtColor(edges, edgesBgr, cv::COLOR_GRAY2BGR);

        // darken where edges exist (edges are 0/255)
        // scale edges to a subtle amount
        cv::Mat halfEdges;
        edgesBgr.convertTo(halfEdges, CV_8U, 0.5); // 0 or ~127
        cv::subtract(small, halfEdges, small);
    }

    // 4) Ordered dithering
    small = applyOrderedDither(small, ditherStrength);

    // 5) Palette reduction via k-means
    cv::Mat smallQ = kmeansQuantize(small, paletteColors, 3);

    // 6) Nearest-neighbor upscale back to original size
    cv::Mat out;
    cv::resize(smallQ, out, cv::Size(W, H), 0, 0, cv::INTER_NEAREST);

    // 7) Light sharpen (unsharp mask style)
    {
        cv::Mat blurred;
        cv::GaussianBlur(out, blurred, cv::Size(3, 3), 0);
        cv::addWeighted(out, 1.15, blurred, -0.15, 0.0, out);
    }

    return out;
}

int main() {

    // ------------------------------------------------------------
    // Paths (current working directory)
    // ------------------------------------------------------------
    const std::string inputPath  = "test.jpg";
    const std::string outputPath = "gba_output.png";

    // ------------------------------------------------------------
    // Load input image
    // ------------------------------------------------------------
    cv::Mat img = cv::imread(inputPath, cv::IMREAD_COLOR);
    if (img.empty()) {
        std::cerr << "Error: could not read input image: " << inputPath << std::endl;
        return -1;
    }

    // ------------------------------------------------------------
    // Run GBA retro filter
    // ------------------------------------------------------------
    cv::Mat gbaImage = gbaRetroFilter(
        img,
        240,   // target width
        16,    // palette size
        18,    // dither strength
        true   // edge hint
    );

    // ------------------------------------------------------------
    // Save output image (same folder)
    // ------------------------------------------------------------
    if (!cv::imwrite(outputPath, gbaImage)) {
        std::cerr << "Error: could not write output image" << std::endl;
        return -1;
    }

    std::cout << "Saved output image: " << outputPath << std::endl;

    // ------------------------------------------------------------
    // Display results
    // ------------------------------------------------------------
    cv::imshow("Original", img);
    cv::imshow("GBA Retro Output", gbaImage);
    cv::waitKey(0);

    return 0;
}
