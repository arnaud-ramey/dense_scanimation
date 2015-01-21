#include <stdio.h>
#include <stdlib.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

typedef cv::Mat3b Img;
typedef cv::Mat4b TransparencyImg;
typedef std::vector<Img> ImgList;

////////////////////////////////////////////////////////////////////////////////

bool constrained_mode = true;

////////////////////////////////////////////////////////////////////////////////

int background_color_idx = 1;
int MAX_BACKGROUND_COLOR = 3;

inline cv::Vec3b current_background_color() {
  switch(background_color_idx) {
  case 0:
    return cv::Vec3b(0, 0, 0);
    break;
  case 1:
    return cv::Vec3b(255, 255, 255);
    break;
  case 2:
  default:
    return cv::Vec3b(255, 0, 0);
    break;
  } // end switch (filter_mode)
}

////////////////////////////////////////////////////////////////////////////////

int filter_function = 1;
int MAX_FILTER_FUNCTIONS = 4;

////////////////////////////////////////////////////////////////////////////////

inline int image_idx(const int & x, const int & y,
                     const int & img_nb,
                     const double col_width = 1) {
  switch(filter_function) {
  case 0:
    return ((int) (x / col_width)) % img_nb;
    break;
  case 1:
    return (x / 2 + 5 * y) % img_nb;
    break;
  case 2:
    return (x / 3 + 3 * y) % img_nb;
    break;
  case 3:
  default:
    return (x / 2 + y / 2) % img_nb;
    break;
  } // end switch (filter_mode)
}

////////////////////////////////////////////////////////////////////////////////

inline void anim(const ImgList imgs) {
  int img_idx = 0;
  while(true) {
    cv::imshow("anim", imgs[img_idx]);
    cv::waitKey(50);
    img_idx = (img_idx + 1) % imgs.size();
  } // end while true
} // en anim();

////////////////////////////////////////////////////////////////////////////////

inline TransparencyImg generate_filter(cv::Size size, const int img_nb) {
  TransparencyImg image(size);
  image = cv::Vec4b(0, 0, 0, 255);
  for (int row = 0; row < image.rows; ++row) {
    for (int col = 0; col < image.cols; ++col) {
      if (image_idx(col, row, img_nb) == 0) // make a hole
        image.at<cv::Vec4b>(row, col) = cv::Vec4b(0, 0, 0, 0);
    } // end loop col
  } // end loop row
  return image;
}

////////////////////////////////////////////////////////////////////////////////

inline Img generate_mixed_image(const ImgList & img_list) {
  printf("generate_mixed_image(img_list size:%i)\n", img_list.size());
  // find the maximum size
  cv::Size max_size (-1, -1);
  for (int img_idx = 0; img_idx < img_list.size(); ++img_idx) {
    max_size.width =  std::max(max_size.width, img_list[img_idx].cols);
    max_size.height = std::max(max_size.height, img_list[img_idx].rows);
  } // end loop img_idx
  printf("max_size:%i x %i\n", max_size.width, max_size.height);

  Img big_image(max_size), curr_img_redim(max_size);
  ImgList big_images;
  for (int img_idx = 0; img_idx < img_list.size(); ++img_idx) {
    //printf("img_idx:%i\n", img_idx);
    const Img* curr_img = &(img_list[img_idx]);
    // find the centered zone of the curr_img_redim
    int x_trans = (max_size.width - curr_img->cols) / 2;
    int y_trans = (max_size.height - curr_img->rows) / 2;
    cv::Rect roi(x_trans, y_trans, curr_img->cols, curr_img->rows);
    //printf("roi:(%i, %i)+(%i, %i)\n", roi.x, roi.y, roi.width, roi.height);

    // copy the wanted image
    curr_img_redim = current_background_color(); // background
    Img curr_roi = curr_img_redim(roi);
    curr_img->copyTo(curr_roi);

    // remove the background
    // run thourgh the image
    cv::Vec3b corner_color = curr_roi.at<cv::Vec3b>(0, 0);
    for (int row = 0; row < curr_roi.size().height; ++row) {
      for (int col = 0; col < curr_roi.size().width; ++col) {
        if (curr_roi.at<cv::Vec3b>(row, col) == corner_color)
          curr_roi.at<cv::Vec3b>(row, col) = current_background_color();
      } // end loop x
    } // end loop y

    // keep this image
    big_images.push_back(curr_img_redim.clone());

    //    cv::imshow("curr_img_redim", curr_img_redim);
    //    cv::imshow("curr_img", *curr_img);
    //    cv::waitKey(0);

  } // end loop img_idx

  //anim(big_images);

  // create the big image
  big_image = cv::Vec3b(0, 0, 0);
  for (int row = 0; row < max_size.height; ++row) {
    for (int col = 0; col < max_size.width; ++col) {
      big_image.at<cv::Vec3b>(row, col) =
          big_images.at( image_idx(col, row, big_images.size()) )
          .at<cv::Vec3b>(row, col);
    } // end loop x
  } // end loop y

  return big_image;
} // end generate_mixed_image()

////////////////////////////////////////////////////////////////////////////////

inline void generate_mixed_image_and_filter(const ImgList & img_list) {
  Img big_image = generate_mixed_image(img_list);
  TransparencyImg filter = generate_filter(big_image.size(), img_list.size());

  cv::imshow("big_image", big_image);
  cv::imshow("filter", filter);
  cv::waitKey(1000);
  cv::destroyAllWindows();

  cv::imwrite("big_image.png", big_image);
  cv::imwrite("filter.png", filter);
}

////////////////////////////////////////////////////////////////////////////////

/*! copies those matrix elements to "m" that are
    marked with non-zero mask elements. */
template<class Image>
inline void paste_img(const Image & topaste, Image & dst,
                      int topaste_x, int topaste_y,
                      cv::Mat* mask = NULL) {
  //  printf("paste_img()\n");

  if (topaste_x >= dst.cols || topaste_x + topaste.cols < 0) {
    printf("case1\n");
    return;
  }
  if (topaste_y >= dst.rows || topaste_y + topaste.rows < 0){
    printf("case2\n");
    return;
  }

  cv::Rect topaste_roi, dst_roi;
  // x - width
  // if it does not fit in x - missing beginning
  if (topaste_x < 0) {
    topaste_roi.x = -topaste_x;
    topaste_roi.width = topaste_x + topaste.cols;
    dst_roi.x = 0;
  }
  // if it does not fit in x - missing end
  else if (topaste_x + topaste.cols > dst.cols) {
    topaste_roi.x = 0;
    topaste_roi.width = dst.cols - topaste_x;
    dst_roi.x = topaste_x;
  }
  else { // it fits in x
    topaste_roi.x = 0;
    topaste_roi.width = topaste.cols;
    dst_roi.x = topaste_x;
  }
  dst_roi.width = topaste_roi.width;

  // y - height
  // if it does not fit in y - missing beginning
  if (topaste_y < 0) {
    topaste_roi.y = -topaste_y;
    topaste_roi.height = topaste_y + topaste.rows;
    dst_roi.y = 0;
  }
  // if it does not fit in y - missing end
  else if (topaste_y + topaste.rows > dst.rows) {
    topaste_roi.y = 0;
    topaste_roi.height = dst.rows - topaste_y;
    dst_roi.y = topaste_y;
  }
  else { // it fits in y
    topaste_roi.y = 0;
    topaste_roi.height = topaste.rows;
    dst_roi.y = topaste_y;
  }
  dst_roi.height = topaste_roi.height;

  // make the proper pasting
  //  printf("topaste_roi:(%ix%i)+(%ix%i), dst_roi:(%ix%i)+(%ix%i)\n",
  //         topaste_roi.x, topaste_roi.y, topaste_roi.width, topaste_roi.height,
  //         dst_roi.x, dst_roi.y, dst_roi.width, dst_roi.height);

  Image topaste_sub = topaste(topaste_roi),
      dst_sub = dst(dst_roi);
  if (mask != NULL) {
    cv::Mat mask_sub = (*mask)(topaste_roi);
    topaste_sub.copyTo(dst_sub, mask_sub);
  }
  else
    topaste_sub.copyTo(dst_sub);
}

////////////////////////////////////////////////////////////////////////////////

struct CallbackData {
  Img* interface;
  Img* interface_with_bars;
  Img* filter;
  cv::Mat1b* filter_mask;
  cv::Size big_image_size;
  short image_nb;
};

////////////////////////////////////////////////////////////////////////////////

inline void mouse_cb(int event, int x, int y, int flags, void* param) {
  //printf("mouse_cb(x:%u, y:%i);\n", x, y);
  CallbackData* d = (CallbackData*) param;
  d->interface->copyTo(*d->interface_with_bars);

  // foo tests
  //x -= d->filter->cols / 2;  y -= d->filter->rows / 2;

  if (constrained_mode)
    y = d->big_image_size.height / 2;

  //  cv::circle(*d->interface_with_bars, cv::Point(x, y), 5,
  //             CV_RGB(255, 0, 0));

  // make the bars
  paste_img(*d->filter, *d->interface_with_bars, x, y, d->filter_mask);

  cv::imshow("interface", *d->interface_with_bars);

} // end mouse_cb();

////////////////////////////////////////////////////////////////////////////////

inline void interface(const ImgList & img_list) {
  Img big_image = generate_mixed_image(img_list);
  Img interface, filter;
  cv::namedWindow("interface");
  cvMoveWindow("interface", 0, 0);

  CallbackData data;
  cv::setMouseCallback("interface", mouse_cb, &data);

  while (true) {
    // init big_image
    big_image = generate_mixed_image(img_list);
    interface.create(2 * big_image.rows, 2 * big_image.cols);
    interface = current_background_color();
    cv::putText(interface,
                "f:filter b:background c:constrained",
                cv::Point(10, interface.rows - 10),
                cv::FONT_HERSHEY_PLAIN,
                1, CV_RGB(180, 0, 0));
    // copy big image
    Img roi = interface(cv::Rect((interface.cols - big_image.cols) / 2,
                                 (interface.rows - big_image.rows) / 2,
                                 big_image.cols,
                                 big_image.rows));
    big_image.copyTo(roi);
    Img interface_with_bars = interface.clone();
    // make filter
    TransparencyImg filter_rgba = generate_filter(big_image.size(), img_list.size());
    std::vector<cv::Mat> channels;
    cv::split(filter_rgba, channels);
    cv::Mat1b filter_mask = channels.back();
    channels.pop_back();
    cv::merge(channels, filter);

    // pass data
    data.interface = &interface;
    data.interface_with_bars = &interface_with_bars;
    data.filter = &filter;
    data.filter_mask = &filter_mask;
    data.big_image_size = big_image.size();
    data.image_nb = img_list.size();

    cv::imshow("interface", interface);

    char c = cv::waitKey(0);
    if (c == 'f')
      filter_function = (filter_function + 1) % MAX_FILTER_FUNCTIONS;
    else if (c == 'b')
      background_color_idx = (background_color_idx + 1) % MAX_BACKGROUND_COLOR;
    else if (c == 'c')
      constrained_mode = !constrained_mode;
    else break;
  } // end while (true)
}

////////////////////////////////////////////////////////////////////////////////

int main(int, char**) {
  printf("main()\n");
  // load the images
  std::string prefix = "../donjon/", suffix = ".png";
  ssize_t n_imgs =
      //7;
      13;

  ImgList list;
  for (int idx = 1; idx <= n_imgs; ++idx) {
    std::ostringstream filename;
    filename << prefix << (idx < 10 ? "0" : "")
             <<  idx << suffix;
    //printf("filename:'%s'\n", filename.str().c_str());
    cv::Mat3b img_rgb = cv::imread(filename.str(), CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
    list.push_back(img_rgb);
  } // end loop idx

  // generate the mixed image
  generate_mixed_image_and_filter(list);
  interface(list);

  return 0;
}

