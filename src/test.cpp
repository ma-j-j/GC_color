    template<typename ImageType>
    void loadImage(const ImageType &image)
    {
        if constexpr (std::is_same_v<ImageType, QString>) {
            loadFileImage(image);
        } else if constexpr (std::is_same_v<ImageType, QPixmap>) {
            loadQPixmapImage(image);
        } else if constexpr (std::is_same_v<ImageType, cv::Mat>) {
            loadMatImage(image);
        } else if constexpr (std::is_same_v<ImageType, QImage>) {
            loadQPixmapImage(QPixmap::fromImage(image));
        } else {
            static_assert(false, "Unsupported image type");
        }
    }
