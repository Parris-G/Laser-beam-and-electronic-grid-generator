#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

using namespace std;

// Constants for user input
const int x = 100;                      			// number of grid points in x direction
const int y = 100;                      			// number of grid points in y direction
const int z = 50;                       			// number of grid points in z direction
const int diameter = 30;               		 // beam spot diameter (grid points)
const int beamDepth = 5;                		// beam depth (grid points)
const int simBoxPlane = 5;              		// location of the surface in simulation box
const double totalBeamTemp = 5000;		// average temp at grid point
const string gridTemp = "ttm_file.in";  		// initial electronic temp file name
const string gridBeam = "beam_file.in"; 		// beam profile temp file name

// Derived variables (computed at compile-time)
const int centreX = x / 2;
const int centreY = y / 2;
const int radius = diameter / 2;
const double gridTemperature = totalBeamTemp / beamDepth;

// Function to calculate temperature
double temperature(int xCord, int yCord, int zCord)
{
    double temp = 0.0;

    if (zCord <= z / simBoxPlane && zCord >= (z / simBoxPlane) - beamDepth)
    {
        if (xCord >= centreX - radius && xCord <= centreX + radius)
        {

            if (yCord <= (sqrt(pow(radius - ((z / simBoxPlane) - zCord), 2) - pow((xCord - centreX), 2) + centreY) + centreX) &&
                yCord >= -((sqrt(pow(radius - ((z / simBoxPlane) - zCord), 2) - pow((xCord - centreX), 2) + centreY)) - centreX))
            {

                if (zCord == z / simBoxPlane)
                {
                    temp = gridTemperature;
                }
                else if (zCord < z / simBoxPlane && zCord >= (z / simBoxPlane) - beamDepth)
                {
                    temp = (gridTemperature / beamDepth) * (beamDepth - ((z / simBoxPlane) - zCord));
                }
            }
        }
    }
    return temp;
}

// Function to dump the grid file
void dumpGridFile(ofstream &file)
{
    for (int i = 0; i < z; i++)
    {
        for (int j = 0; j < y; j++)
        {
            for (int k = 0; k < x; k++)
            {
                file << k << ' ' << j << ' ' << i << ' ' << "300.0\n";
            }
        }
    }
}

// Function to dump the beam file
double dumpBeamFile(ofstream &file)
{
    double beamTotal = 0.0;
    int grid_count = 0;

    for (int i = 0; i < z; i++)
    {
        for (int j = 0; j < y; j++)
        {
            for (int k = 0; k < x; k++)
            {
                double beamCurrent = temperature(k, j, i);
                if (beamCurrent != 0)
                {
                    beamTotal = beamCurrent + beamTotal;
                    grid_count++;
                }
                file << k << ' ' << j << ' ' << i << ' ' << beamCurrent << '\n';
            }
        }
    }
    return beamTotal;
}

int main()
{
    // Open grid file
    ofstream gridFile(gridTemp);
    if (!gridFile)
    {
        cerr << "Error opening grid file." << endl;
        return 1;
    }

    // Dump the grid file
    dumpGridFile(gridFile);
    gridFile.close();

    // Open beam file
    ofstream beamFile(gridBeam);
    if (!beamFile)
    {
        cerr << "Error opening beam file." << endl;
        return 1;
    }

    // Dump the beam file and calculate beamTotal
    double beamTotal = dumpBeamFile(beamFile);
    beamFile.close();

    // Open and write to ReadMe file
    ofstream readmeFile("ReadMe.txt");
    if (!readmeFile)
    {
        cerr << "Error opening ReadMe file." << endl;
        return 1;
    }

    readmeFile << "Total input temperature distributed on the grid is: " << beamTotal << '\n';
    readmeFile.close();

    return 0;
}
