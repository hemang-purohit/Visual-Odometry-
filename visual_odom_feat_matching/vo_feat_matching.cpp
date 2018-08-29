#include "vo_features.h"

using namespace cv;
using namespace std;

#define MAX_FRAME 4000;
#define MIN_NUM_FEAT 2000;

double getAbsoluteScale(int frame_id, int sequence_id, double z_cal)
{
  string line;
  int i = 0;
  ifstream myfile("/home/hemang/Downloads/home/dataset/sequences/00/00.txt"); // add your own path where 00.txt is stored in youe system 
  double x = 0, y=0, z=0;
  double xprev, yprev, zprev;
  if (myfile.is_open()) {
    while ((getline(myfile, line)) && (i<=frame_id)) {
      zprev = z;
      xprev = x;
      yprev = y;
      istringstream in(line);
      // cout<< line <<'\n';
      for (size_t j = 0; j < 12; j++) {
        in >> z;
        if (j==7) {
          y=z;
        }
        if (j==3) {
          x=z;
        }
      }
      i++;
    }
    myfile.close();
  }

  else{
    cout<< "Unable to open file";
    return 0;
  }

  return sqrt((x-xprev)*(x-xprev) + (y-yprev)*(y-yprev) + (z-zprev)*(z-zprev));

}

int main(int argc, char** argv)
{
  Mat img_1, img_2;
  Mat R_f , t_f;

  double scale = 1.00;
  char filename1[400];
  char filename2[400];
  sprintf(filename1, "/home/hemang/Downloads/home/dataset/sequences/00/image_2/%06d.png", 0); // replace "/home/hemang/Downloads/home/" with your path where these image files are stored 
  sprintf(filename2, "/home/hemang/Downloads/home/dataset/sequences/00/image_2/%06d.png", 1);

  char text[120];
  img_1 = imread(filename1, IMREAD_GRAYSCALE);
  img_2 = imread(filename2, IMREAD_GRAYSCALE);

  if (!img_1.data || !img_2.data) {
    cout << "Error reading images" << endl;
    return -1;
  }

  std::vector<Point2f> points1, points2;
  featureDetectionAndMatching(img_1, img_2, points1, points2);

  cout << "points1 size" << points1.size() << endl;
  cout << "points2 size" << points2.size() << endl;
  double focal = 718.8560;
  Point2d pp (607.1928, 185.2157);

  //calculation of Pose from Essential matrix here
  Mat E, R , t, mask;
  E = findEssentialMat(points2, points1, focal, pp, RANSAC, 0.999, 1.0, mask); \\Essential matrix calculation 
  recoverPose(E, points2, points1, R, t, focal, pp, mask); \\ SVD opeartion using opencv function 
  cout << "R = " << R << endl;
  cout << "t = " << t << endl; \\ please note this t is not upto scale 
  \\ cout << "E = " << E << endl;

  Mat prevImage = img_2;
  Mat currImage;

  char filename[100];
  R_f = R.clone();
  t_f = t.clone();

  clock_t begin = clock();

  //create windows to display video and Trajectory
  namedWindow("Road facing camera",  WINDOW_AUTOSIZE);
  namedWindow("Trajectory",  WINDOW_AUTOSIZE);

  Mat traj = Mat::zeros(600, 600, CV_8UC3);

  for (int numFrame = 2; numFrame < 100; numFrame++) {
    sprintf(filename, "/home/hemang/Downloads/home/dataset/sequences/00/image_2/%06d.png", numFrame);

    currImage = imread(filename, IMREAD_GRAYSCALE);
    std::vector<Point2f> prevKeypoints, currKeypoints;
    featureDetectionAndMatching(prevImage, currImage, prevKeypoints, currKeypoints);

    E = findEssentialMat(currKeypoints, prevKeypoints, focal, pp, RANSAC, 0.999, 1.0, mask);
    recoverPose(E, currKeypoints, prevKeypoints, R, t, focal, pp, mask);

    cout << "prev keypoints size" << prevKeypoints.size() <<endl;

    scale = getAbsoluteScale(numFrame, 0, t.at<double>(2));

    cout << "Scale is "<< scale <<endl;

    if ((scale>0.1)&&(t.at<double>(2) > t.at<double>(0)) && (t.at<double>(2) > t.at<double>(1))) {
      t_f = t_f + scale*(R_f*t);
      R_f = R*R_f;
    }
    else{
      //cout << "scale below 0.1 or incorrect translation" <<endl;
    }

    prevImage = currImage.clone();

    int x = int(t_f.at<double>(0)) + 300;
    int y = int(t_f.at<double>(2)) + 100;
    circle(traj, Point(x,y), 1, CV_RGB(255,0,0),1);

    rectangle(traj, Point(10,30), Point(550,50), CV_RGB(0,0,0), CV_FILLED);
    sprintf(text, "coordinates : x = %02fm y = %02fm z=%02fm", t_f.at<double>(0),t_f.at<double>(1), t_f.at<double>(2));
    putText(traj, text, Point(10,50), FONT_HERSHEY_PLAIN, 1, Scalar::all(255), 1, 8);

    imshow("Road facing camera", currImage);
    imshow("Trajectory", traj);

    waitKey(1);

  }

  clock_t end = clock();
  double elapsed_secs = double(end -begin) / CLOCKS_PER_SEC;
  cout<< "Total time taken : " << elapsed_secs << "s" <<endl;

  return 0;

}
