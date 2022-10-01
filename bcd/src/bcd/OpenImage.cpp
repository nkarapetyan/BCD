#include <bcd/OpenImage.h>

/*************************************************************************
 *
 * @author: Kelly Benson
 * @email: bensonke@email.sc.edu
 * @date: May 16, 2015
 *
 * This file is for the OpenImage calculation and work behind that.
 * When a OpenImage object is created it should ask for an image file
 * and then calculate the OpenImage/draw it.
 *
**/


/*************************************************************************
 * Constructor
 *
**/
OpenImage::OpenImage() {};


/*************************************************************************
 * Destructor
 *
**/
OpenImage::~OpenImage() {};


/*************************************************************************
 * Function 'open()'
 *
 * Method that sets the QImage variable imageBuffer to the appropriate
 * image file from the input folder. Acts in many ways to initialize
 * specific variables.
 *
 * Returns:
 *   None
 *
 * Parameters:
 *   directory - directory that holds the image file
 *   fileName - the file name of the image being loaded
 *
**/
RegionData OpenImage::open(string directory, string fileName)
{
    RegionData data;
    if (mapFiles.empty() == true)
    {
        dir = directory;
        file = fileName;
        setMapFiles();
    }

    try
    {
        string search = dir + "/" + file;

        // File was found
        if(mapFiles.find(search) != mapFiles.end())
        {
            data.loadWorld(search);
        }

        // File was not found so iter returns set.end()
        else
        {
            std::cout << "File not found";
        }

        return data;
    }
    catch (const std::string& err)
    {
        std::cerr << "HERE ERROR WHY?";
        return data;
    }

}


/*************************************************************************
 * Function 'findFiles()'
 *
 * Method that finds all of the files in the directory
 *
 * Returns:
 *   set<string> - Set of string values that stores the file names.
 *
 * Parameters:
 *   None
 *
**/
set<string> OpenImage::findFiles()
{
    DIR* directory;
    struct dirent* dirPath;
    set<string> files;

    const char* dirC = dir.c_str();
    directory = opendir(dirC);

    if(directory == NULL)
    {
        exit(0);
    }

    bool keep = true;

    // Reading in the files from the directory.
    while(keep == true)
    {
        dirPath = readdir(directory);
        if(dirPath == NULL)
        {
            keep = false;
        }
        else
        {
            string fileName = dirPath->d_name;
            if (fileName != "." && fileName != "..")
            {
                fileName = dir + "/" + fileName;
                files.insert(fileName);
            }
        }
    }
    closedir(directory);
    return files;
}


/*************************************************************************
 * Function 'setMapFiles()'
 *
 * Method that sets the mapFiles variable to all the map files found in
 * the input folder.
 *
 * Returns:
 *   None
 *
 * Parameters:
 *   None
 *
**/
void OpenImage::setMapFiles()
{
    set<string> files;

    //gets all of the files and then sorts them into a
    //  set of strings. This also works because sets
    //  automatically sort alphabetically.
    files = findFiles();

    string s = "";
    set<string>::iterator iter;
    for(iter = files.begin(); iter != files.end(); ++iter)
    {
        s = (*iter);

        if(s.find("png") != std::string::npos)
        {
            mapFiles.insert(s);
        }
    }
}


/*************************************************************************
 * Function 'getMapFiles()'
 *
 * Method that gets the mapFiles variable.
 *
 * Returns:
 *   mapFiles - set that has all the names of the map files
 *
 * Parameters:
 *   None
 *
**/
set<string> OpenImage::getMapFiles()
{
    return mapFiles;
}


/*************************************************************************
 * Function 'printMapFiles()'
 *
 * Method that prints off the names of all the files found using
 * findFiles() and then set to mapFiles. Used for testing that all the
 * map files are being found.
 *
 * Returns:
 *   mapFiles - set that has all the names of the map files
 *
 * Parameters:
 *   None
 *
**/
void OpenImage::printMapFiles()
{
    if(mapFiles.empty() == true)
    {
        setMapFiles();
    }

    string s = "";
    string files = "";

    set<string>::iterator iter;
    for(iter = mapFiles.begin(); iter != mapFiles.end(); ++iter)
    {
        s = (*iter);
        files += s;
        files += "\n";
    }

    std::cout << files;
}
