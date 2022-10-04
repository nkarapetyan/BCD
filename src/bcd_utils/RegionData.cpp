#include <bcd_utils/RegionData.h>

/*************************************************************************
 * Function 'loadWorld()'
 *
 * Method that takes in a file name and then finds the path to that file.
 * that file is then opened using the parseWorldFile method.
 *
 * Returns:
 *   None
 *
 * Parameters:
 *   filename - the name of the image being loaded
 *
**/
void RegionData::loadWorld(string filename) throw (const std::string&)
{

//DEBUG
//std::cerr << "CHECK FILENAME --> " << filename << std::endl;
//DEBUG
    // Parse the filename to obtain base dir & file name (i.e. w/o extension)
    string dir, fileHeader, imageExtension;
    string imageFilename, mapFilename, worldFilename;
    loadWorldFilename = filename;

    // Obtain absolute path and file extension
    fs::path filepath = fs::system_complete(fs::path(filename));
    dir = filepath.parent_path().string();
    fileHeader = filepath.stem().string();
    imageExtension = filepath.extension().string();
    imageFilename = dir + "/" + fileHeader + imageExtension;
    mapFilename = dir + "/" + fileHeader + ".map" + imageExtension;
    worldFilename = dir + "/" + fileHeader + ".wf";

    //need to get rid of wf files

    // Load the world file and both images
    if (!fs::exists(worldFilename))
    {
        worldFilename = dir + "/" + "_.wf";
        if (!fs::exists(worldFilename))
        {
            loadWorldFilename = "";
            throw std::string("The world file " + worldFilename
                    + " does not exist.");
        }
    }

    parseWorldFile(worldFilename); // "world" is created in this function
    if (!fs::exists(imageFilename))
    {
        loadWorldFilename = "";
        throw std::string("The image file " + imageFilename
                + " does not exist.");
    }

    image = cv::imread(imageFilename);
    if (!fs::exists(mapFilename))
    {
        mapFilename = dir + "/" + fileHeader + imageExtension;
        if (!fs::exists(mapFilename))
        {
            loadWorldFilename = "";
            throw std::string("The map file " + mapFilename
                    + " does not exist.");
        }
    }

    cv::Mat grey = cv::imread(mapFilename, cv::IMREAD_GRAYSCALE);
    cv::threshold(grey, map, 100.0, 255, cv::THRESH_BINARY);

    transform = cv::getRotationMatrix2D(cv::Point2f(0,0),0,1.0);
    invertTransform = cv::getRotationMatrix2D(cv::Point2f(0,0),0,1.0);
    hasRotated = false;

    ori_image = image;
    ori_map = map;
};


/*************************************************************************
 * Function 'parseWorldFile()'
 *
 * Method that parses the image into a world file.
 *
 * Returns:
 *   None
 *
 * Parameters:
 *   filename - the file name of the image being parsed
 *
**/
void RegionData::parseWorldFile(string filename) throw (const std::string&)
{
    size_t delimPos;
    string line;
    double lat, lon;
    bool hasLat = false, hasLon = false, hasXRes = false, hasYRes = false;
    ifstream f;
    f.open(filename.c_str(), ios::in);

    if (f.is_open())
    {
        while (!f.eof())
        {
            getline(f, line);

            if (line.find("X Resolution:") != string::npos)
            {
                delimPos = line.find(":");
                std::istringstream stream(line.substr(delimPos+1));
                stream >> xRes;
                hasXRes = true;
            }
            else if (line.find("Y Resolution:") != string::npos)
            {
                delimPos = line.find(":");
                std::istringstream stream(line.substr(delimPos+1));
                stream >> yRes;
                hasYRes = true;
            }
            else if (line.find("Upper Left Latitude:") != string::npos)
            {
                delimPos = line.find(":");
                std::istringstream stream(line.substr(delimPos+1));
                stream >> lat;
                hasLat = true;
            }
            else if (line.find("Upper Left Longitude:") != string::npos)
            {
                delimPos = line.find(":");
                std::istringstream stream(line.substr(delimPos+1));
                stream >> lon;
                hasLon = true;
            }
        }
        f.close();
    }
    else
    {
        loadWorldFilename = "";
        throw std::string("Could not open log file " + filename
                + " for parsing.");
    }

    if (hasLat && hasLon && hasXRes && hasYRes)
    {
        topLeftWorldLatitude = lat;
        topLeftWorldLongitude = lon;
    }
    else
    {
        topLeftWorldLatitude = INVALID_DEGREE_VALUE;
        topLeftWorldLongitude = INVALID_DEGREE_VALUE;
        xRes = 1;
        yRes = 1;
        loadWorldFilename = "";
        throw std::string("The world file " + filename + " is missing data.");
    }
};


/*************************************************************************
 * Function 'rotateImage()'
 *
 * Method rotates an image.
 *
 * Returns:
 *   cv::Mat - returns an image
 *
 * Parameters:
 *   source - image source
 *   angle - the amount to rotate the image by
 *   src_center - point in the image
 *
**/
cv::Mat RegionData::rotateImage(const cv::Mat& source, double angle,
        cv::Point2f src_center) {
    transform = cv::getRotationMatrix2D(src_center, angle, 1.0);
    cv::Mat boundaries = cv::Mat(3,4,transform.type());
    double *data = (double*) boundaries.data;
    // Row 0
    data[0] = 0;
    data[1] = 0;
    data[2] = source.cols;
    data[3] = source.cols;
    // Row 1
    data[4] = 0;
    data[5] = source.rows;
    data[6] = 0;
    data[7] = source.rows;
    // Row 2
    data[8] = 1;
    data[9] = 1;
    data[10] = 1;
    data[11] = 1;

    cv::Mat f_bound = transform*boundaries;
    data = (double*) f_bound.data;

    double w = max(max(data[0],data[1]),max(data[2],data[3]))
        - min(min(data[0],data[1]),min(data[2],data[3]));
    double h = max(max(data[4],data[5]),max(data[6],data[7]))
        - min(min(data[4],data[5]),min(data[6],data[7]));

    data = (double*) transform.data;
    data[2] = data[2] - src_center.x + (int)w / 2;
    data[5] = data[5] - src_center.y + (int)h / 2;

    cv::Mat dst;
    cv::warpAffine(source, dst, transform, cv::Size((int)w, (int)h));
    cv::invertAffineTransform(transform, invertTransform);
    hasRotated = true;

    return dst;
};


/*************************************************************************
 * Function 'rotateImage()'
 *
 * Method rotates an image.
 *
 * Returns:
 *   cv::Mat - returns an image
 *
 * Parameters:
 *   source - image source
 *   newTransform -
 *   new_width -
 *   new_height -
 *
**/
cv::Mat RegionData::rotateImage(const cv::Mat& source,
        const cv::Mat& newTransform, double new_width, double new_height) {
    transform = newTransform.clone();
    cv::Mat dst;
    cv::warpAffine(source, dst, transform, cv::Size(new_width, new_height));
    cv::invertAffineTransform(transform, invertTransform);
    hasRotated = true;
    return dst;
};


/***
 * Author: Patrick Virie
 * PCA over free space in the input region.
 * Cov Matrix C
 * eV = CV
 * 0 = det(C - eI)
 * solving quadratic yields two es.
 * 0 = (C00 - e)(C11 - e) - C10C01
 * 0 = (C00C11-C10C01) -(C11 + C00)e + e^2
 * solving linear equation for each e yeilds eigen vector.
 * 0 = (C-eI)V
 * 0 = (C00 - e)x + C01y --(1)
 * x/y = C01/(e-C00)
 * using the fact that we want unit size vectors.
 */
std::pair<double, cv::Point2f> RegionData::computeAlignmentToFreespace()
{
    double cx, cy;
    double e0, e1;
    double v0x, v0y;
    double v1x, v1y;
    double sX, sY, sXX, sYY, sXY;
    int count = 0;
    sX = sY = sXX = sYY = sXY = 0;

    for(int x=0; x < ori_map.cols; ++x)
    {
        for(int y=0; y < ori_map.rows; ++y)
        {
            unsigned char curP = ori_map.at<uchar>(y,x);
            if(curP == WHITE)
            {
                sX += x;
                sY += y;
                ++count;
            }
        }
    }
    cx = sX/count;
    cy = sY/count;

    for(int x=0; x < ori_map.cols; ++x)
    {
        for(int y=0; y < ori_map.rows; ++y)
        {
            unsigned char curP = ori_map.at<uchar>(y,x);
            if(curP == WHITE)
            {
                sXX += (x-cx)*(x-cx);
                sXY += (x-cx)*(y-cy);
                sYY += (y-cy)*(y-cy);
                ++count;
            }
        }
    }

    // 0 = (C00C11-C10C01) -(C11 + C00)e + e^2
    solveQuadraticEquation(1, -(sXX+sYY), (sXX*sYY-sXY*sXY), e0, e1);

    double sqrs;
    if(e1 > e0) swap(e0,e1);

#define DIVZ(x) (abs(x)>1e-5? (x):1e-5)

    //x/y = C01/(e-C00)
    v0y = 1;
    v0x = sXY/DIVZ(e0-sXX);
    sqrs = sqrt(v0y*v0y + v0x*v0x);
    v0y = v0y/DIVZ(sqrs);
    v0x = v0x/DIVZ(sqrs);
    //x/y = C01/(e-C00)
    v1y = 1;
    v1x = sXY/DIVZ(e1-sXX);
    sqrs = sqrt(v1y*v1y + v1x*v1x);
    v1y = v1y/DIVZ(sqrs);
    v1x = v1x/DIVZ(sqrs);

#undef DIVZ


#ifdef P_PRINT_DEBUG
    cv::line(image, cvPoint(cx,cy), cvPoint(cx + 100*v0x, cy + 100*v0y),
            CV_RGB(0, 255, 0), 2);
    cv::line(image, cvPoint(cx,cy), cvPoint(cx + 50*v1x, cy + 50*v1y),
            CV_RGB(0, 255, 0), 2);
#ifdef ENABLE_IMSHOW
    cv::imshow("Image View", image);
#endif
#endif

#ifdef P_PRINT_DEBUG
#ifdef ENABLE_IMSHOW
    cv::namedWindow("Rotated Map");
    cv::imshow("Rotated Map", ori_map);
#endif
#endif

    return make_pair(atan2(v1y,v1x)*180/CV_PI, cv::Point2f(cx, cy));
};


/***
 * Author: Patrick Virie
 * Compute histogram of gradients (HoG)
 * then rotate along the direction that contains maximum number of elements
 */
std::pair<double, cv::Point2f> RegionData::computeAlignmentToObstacles() {
    cv::Mat gox;
    cv::Sobel(ori_map, gox, CV_64F, 1, 0, 9, 1, 0, cv::BORDER_CONSTANT);
    cv::Mat goy;
    cv::Sobel(ori_map, goy, CV_64F, 0, 1, 9, 1, 0, cv::BORDER_CONSTANT);

    vector<int> HoG;
    vector<double> HoG_residue;
    int factor = 5;
    HoG.resize(180);
    HoG_residue.resize(180);
    fill(HoG.begin(), HoG.end(), 0);
    fill(HoG_residue.begin(), HoG_residue.end(), 0);

    for(int x=0; x < ori_map.cols; ++x)
    {
        for(int y=0; y < ori_map.rows; ++y)
        {
            double curPX = gox.at<double>(y,x);
            double curPY = goy.at<double>(y,x);

            double size = sqrt(curPX*curPX + curPY*curPY);
            if(size > 100)
            {
                double _t = atan2(curPY,curPX)*180/CV_PI;
                _t = (_t < 0 ? _t + 180 : _t);
                _t = (_t >= 180 ? _t - 180 : _t);
                int slot = (int)(_t);
                HoG[slot]++;
                HoG_residue[slot] += (_t - slot);
            }
        }
    }

    gox.release();
    goy.release();

    int preferSlot = 0;
    double max_val = 0;
    for (int i = 0; i < 180/factor; ++i)
    {
        double sum = 0;
        int sub_slot = 0;
        double sub_val = 0;

        for(int j = 0; j < factor; ++j)
        {
            if(HoG[i*factor + j] > sub_val)
            {
                sub_slot = j;
                sub_val = HoG[i*factor + j];
            }
            sum += HoG[i*factor + j];
        }
        if(sum > max_val)
        {
            max_val = sum;
            preferSlot = i*factor + sub_slot;
        }

#ifdef P_PRINT_DEBUG
        p_cout<< "Angle: " << i*factor << " Sum: " << sum << endl;
#endif
    }

    double angle = preferSlot + HoG_residue[preferSlot]/HoG[preferSlot];

#ifdef P_PRINT_DEBUG
    p_cout << "Max angle" << angle << endl;
#ifdef ENABLE_IMSHOW
    cv::imshow("Rotated Map", ori_map);
#endif
#endif

    return make_pair(angle, cv::Point2f(ori_image.cols/2.0, ori_image.rows/2.0));
};


/***
 * Author: Patrick Virie
 */
bool RegionData::solveQuadraticEquation(double A, double B, double C,
        double &r1, double &r2) {
    double temp = B*B - 4*A*C;
    if (temp < 0)
        return false;
    temp = sqrt(temp);
    if(B < 0)
    {
        r1 = -2*C/(B+temp);
        r2 = -2*C/(B-temp);
    }
    else
    {
        r1 = (-B+temp) / 2*A;
        r2 = (-B-temp) / 2*A;
    }
    return true;
};


/*************************************************************************
 * Function 'getImage()'
 *
 * Method that gets the .
 *
 * Returns:
 *
 *
 * Parameters:
 *   None
 *
**/
cv::Mat RegionData::getImage()
{
    return image;
}


/*************************************************************************
 * Function 'getMap()'
 *
 * Method that gets the .
 *
 * Returns:
 *
 *
 * Parameters:
 *   None
 *
**/
cv::Mat RegionData::getMap()
{
    return map;
}


/*************************************************************************
 * Function 'getOriginalImage()'
 *
 * Method that gets the .
 *
 * Returns:
 *
 *
 * Parameters:
 *   None
 *
**/
cv::Mat RegionData::getOriginalImage()
{
    return ori_image;
}


/*************************************************************************
 * Function 'getOriginalMap()'
 *
 * Method that gets the .
 *
 * Returns:
 *
 *
 * Parameters:
 *   None
 *
**/
cv::Mat RegionData::getOriginalMap()
{
    return ori_map;
};


/*************************************************************************
 * Function 'clear()'
 *
 * Clears variables.
 *
 * Returns:
 *    None
 *
 * Parameters:
 *    None
 *
**/
void RegionData::clear()
{
    ori_image = cv::Mat();
    ori_map = cv::Mat();
    image = cv::Mat();
    map = cv::Mat();

    xRes = 1.0;
    yRes = 1.0;
    transform = cv::getRotationMatrix2D(cv::Point2f(0,0), 0, 1.0);
    invertTransform = cv::getRotationMatrix2D(cv::Point2f(0,0), 0, 1.0);
    hasRotated = false;
};


/*************************************************************************
 * Function 'transformPoint()'
 *
 *
 *
 * Returns:
 *    std::pair<double, double>
 *
 * Parameters:
 *    double xPixel
 *    double yPixel
 *
**/
std::pair<double, double> RegionData::transformPoint(double xPixel, double yPixel)
{
    double *data = (double*) transform.data;
    return std::make_pair(xPixel*data[0] + yPixel*data[1] + data[2],
            xPixel*data[3] + yPixel*data[4] + data[5]);
};


/*************************************************************************
 * Function 'invertTransformPoint()'
 *
 *
 *
 * Returns:
 *    std::pair<double, double>
 *
 * Parameters:
 *    double xPixel
 *    double yPixel
 *
**/
std::pair<double, double> RegionData::invertTransformPoint(double xPixel,
        double yPixel)
{
    double *data = (double*) invertTransform.data;
    return std::make_pair(xPixel*data[0] + yPixel*data[1] + data[2],
            xPixel*data[3] + yPixel*data[4] + data[5]);
};


/*************************************************************************
 * Function 'rotateVector()'
 *
 * Same as transformPoint, except does not shift by offsets
 *
 * Returns:
 *    std::pair<double, double>
 *
 * Parameters:
 *    double xPixel
 *    double yPixel
 *
**/
std::pair<double, double> RegionData::rotateVector(double xPixel, double yPixel)
{
    double *data = (double*) transform.data;
    return std::make_pair(xPixel*data[0] + yPixel*data[1],
            xPixel*data[3] + yPixel*data[4]);
};


/*************************************************************************
 * Function 'invertRotateVector()'
 *
 *
 * Returns:
 *    std::pair<double, double>
 *
 * Parameters:
 *    double xPixel
 *    double yPixel
 *
**/
std::pair<double, double> RegionData::invertRotateVector(double xPixel,
    double yPixel)
{
    double *data = (double*) invertTransform.data;
    return std::make_pair(xPixel*data[0] + yPixel*data[1],
            xPixel*data[3] + yPixel*data[4]);
};


/*************************************************************************
 * Function 'rotateAllData()'
 *
 * Utility macro to rotate both the world and the map
 * NOTE: angle is in degrees, IN RIGHT HAND COORD SYSTEM!!!
 *
 * Returns:
 *    None
 *
 * Parameters:
 *    double angle
 *    cv::Point2f src_center
 *
**/
inline void RegionData::rotateAllData(double angle, cv::Point2f src_center)
{
    image = rotateImage(ori_image, angle, src_center);
    map = rotateImage(ori_map, angle, src_center);
};


/*************************************************************************
 * Function 'rotateAllData()'
 *
 *
 * Returns:
 *    None
 *
 * Parameters:
 *    double angle
 *
**/
inline void RegionData::rotateAllData(double angle)
{
    rotateAllData(angle, cv::Point2f(ori_image.cols/2.0,
                    ori_image.rows/2.0));
};


/*************************************************************************
 * Function 'rotateAllData()'
 *
 *
 * Returns:
 *    None
 *
 * Parameters:
 *    double angle
 *    cv::Point2f> angle_ctr
 *
**/
inline void RegionData::rotateAllData(std::pair<double, cv::Point2f> angle_ctr)
{
    rotateAllData(angle_ctr.first, angle_ctr.second);
};


/*************************************************************************
 * Function 'rotateAllData()'
 *
 *
 * Returns:
 *    None
 *
 * Parameters:
 *    const cv::Mat& newTransform
 *    double width
 *    double height
 *
**/
inline void RegionData::rotateAllData(const cv::Mat& newTransform, double width,
        double height)
{
    image = rotateImage(ori_image, newTransform, width, height);
    map = rotateImage(ori_map, newTransform, width, height);
};
