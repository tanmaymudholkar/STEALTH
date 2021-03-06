#ifndef H_STREGIONSSTRUCT
#define H_STREGIONSSTRUCT

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>

#include "TAxis.h"

struct STRegionsStruct {
  std::vector<double> STBoundaries;
  double STNormRangeMin, STNormRangeMax;
  int nSTSignalBins;
  TAxis STAxis;

  STRegionsStruct(std::string inputFileName) {
    double STBoundary;
    std::ifstream inputFileObject(inputFileName.c_str());
    if (inputFileObject.is_open()) {
      while (inputFileObject >> STBoundary) {
        STBoundaries.push_back(STBoundary);
      }
      inputFileObject.close();
    }
    else {
      std::cout << "ERROR: Unable to open file with name = " << inputFileName << std::endl;
      std::exit(EXIT_FAILURE);
    }
    STBoundaries.push_back(20000.0); // last bin
    nSTSignalBins = STBoundaries.size() - 2; // First two upper boundaries are for pre-normalization and normalization bin
    STNormRangeMin = STBoundaries[0];
    STNormRangeMax = STBoundaries[1];
    std::cout << "Using norm range: min = " << STNormRangeMin << ", max = " << STNormRangeMax << std::endl;
    STAxis = TAxis(-1 + STBoundaries.size(), STBoundaries.data());
  }
};

#endif
