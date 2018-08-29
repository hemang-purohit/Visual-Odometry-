#include <iostream>
#include <ctype.h>
#include <algorithm>
#include <iterator>
#include <vector>
#include <ctime>
#include <sstream>
#include <fstream>
#include <string>
#include <cmath>
#include <stdio.h>

#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/calib3d.hpp"

using namespace cv;
using namespace std;
using namespace cv::xfeatures2d;

void featureDetectionAndMatching(Mat img_1, Mat img_2, vector<Point2f>& points1, vector<Point2f>& points2)
{
  //int nfeatures = 150;
  int minHessian = 400;
  Ptr<SURF> detector = SURF::create(minHessian); //create SIFT operator

  std::vector<KeyPoint> keypoints_1, keypoints_2;
  Mat descriptors_1, descriptors_2;

  detector->detectAndCompute(img_1, Mat(), keypoints_1, descriptors_1);
  detector->detectAndCompute(img_2, Mat(), keypoints_2, descriptors_2);

  //FLANN steps
  FlannBasedMatcher matcher;
  std::vector<DMatch> matches;
  matcher.match(descriptors_1, descriptors_2, matches);
  //cout << "keypoints_1 size " << keypoints_1.size() << endl;
  //cout << "keypoints_2 size " << keypoints_2.size() << endl;

  //cout << "descriptors_1 cols " << descriptors_1.cols << endl;
  //cout << "descriptors_2 cols " << descriptors_2.cols << endl;
  //cout << "descriptors_1 rows " << descriptors_1.rows << endl;
  //cout << "descriptors_2 rows " << descriptors_2.rows << endl;
  //cout << "matches size " << matches.size() << endl;
/*
  double max_dist = 0; double min_dist = 100;

  for (size_t i = 0; i < descriptors_1.rows; i++) {
    double dist = matches[i].distance;
    if (dist < min_dist) {
      min_dist = dist;
    }
    if (dist > max_dist) {
      max_dist = dist;
    }
  }

  //cout << "Max dist  = " << max_dist <<endl;
  //cout << "Min dist = "  << min_dist <<endl;

  std::vector<DMatch> good_matches;

  for (size_t i = 0; i < descriptors_1.rows; i++) {
    if (matches[i].distance <= max(2*min_dist, 0.02)) {
      good_matches.push_back(matches[i]);
    }
  }

  cout << "good matches size " << good_matches.size() <<endl;
*/
  cout << "matches size " << matches.size() <<endl;
  std::vector<KeyPoint> keypoints1, keypoints2;

  std::vector<int> j,k;
  for (int i = 0; i <(int)matches.size(); i++) {
    //printf( "-- Good Match [%d] Keypoint 1: %d  -- Keypoint 2: %d  \n", i, good_matches[i].queryIdx, good_matches[i].trainIdx ); }

    /*
    j.push_back(good_matches[i].queryIdx);
    k.push_back(good_matches[i].trainIdx);
  }
  cout << j.size() << endl;
  cout << k.size() << endl;
  cout << j << endl;
  cout << k << endl;
  */

    keypoints1.push_back(keypoints_1.at(matches[i].queryIdx));
    keypoints2.push_back(keypoints_2.at(matches[i].trainIdx));
  }
  KeyPoint::convert(keypoints1, points1, vector<int>());
  KeyPoint::convert(keypoints2, points2, vector<int>());


}
