#include <Boustrophedon/BCD.h>
#include <Boustrophedon/DrawImage.h>
#include <Leo/Point2D.h>
#include <Leo/ReebGraph.h>
#include <Leo/RegionData.h>

int parseInputArgs(int argc, char **argv, std::string& directory,
        std::string& image);

int main(int argc, char **argv) 
{
	
    std::string image;
    std::string directory;

    if(parseInputArgs(argc, argv, directory, image) == -1) {
	    return EXIT_FAILURE;
    }


    //ReebGraph graph;
    //RegionData data;

#ifdef DEBUG_BCD
    std::cerr << "BCD is starting...\n";
    std::cerr << "----------------\n";
    std::cerr << "directory-> " << directory << "\n";
    std::cerr << "image-> " << image << "\n";
#endif


    BCD bcd(directory, image);
    try{
	    bcd.buildBCD();
	    QString fileName = QString::fromStdString(directory+"out_"+image);
	    bcd.viewReebGraph(fileName);
	    std::cout << "\n-----------Reeb Graph Info:--------------\n";
	    bcd.printBCDInfo();
	    std::cout << "\n-----------------------------------------\n";
    }
    catch (std::invalid_argument ia) {
	    //std::cerr << ia.what() << "\n";
    }
};

int parseInputArgs(int argc, char **argv, std::string& directory,
        std::string& image)
{
    bool printUsage = false;
    // Parse and validate command line input arguments
    if (argc <= 2) 
    {
        printUsage = true;
    }

    else 
    {
        try 
        {
            directory = boost::lexical_cast<string>(argv[1]);
            image = boost::lexical_cast<string>(argv[2]);

            if (directory.size() <= 0) 
            {
                printUsage = true; 
            }
            else if (image.size() <= 0) 
            {
                printUsage = true;
            }
        }

        catch (std::exception err)
        {
            printUsage = true;
        }
    }


    if (printUsage)
    {
        std::cerr << "Usage: " << argv[0] << " [IMAGE_DIRECTORY] [IMAGE_NAME]" << std::endl;
        return -1;
    }

    return 0;
}

