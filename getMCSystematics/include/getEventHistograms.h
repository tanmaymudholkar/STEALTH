#include <cstdlib>
#include <algorithm>
#include <initializer_list>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <cassert>

#include "tmArgumentParser.h"
#include "tmProgressBar.h"
#include "tmMiscellaneous.h"
#include "tmROOTSaverUtils.h"
#include "TROOT.h"
#include "TChain.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"
#include "TFile.h"
#include "TAxis.h"
#include "TH1.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TH2I.h"
#include "TH2F.h"
#include "TProfile2D.h"
#include "TObjArray.h"
#include "TLegend.h"
#include "TLegendEntry.h"
#include "TPaveStats.h"
#include "TEfficiency.h"

#include "../../eventSelection/include/STRegionsStruct.h"
#include "../../eventSelection/include/shiftedObservablesStruct.h"
#include "../../eventSelection/include/MCTemplateReader.h"
#include "../../eventSelection/include/constants.h"

struct parameterSpaceRegion {
  double minGluinoMass;
  double maxGluinoMass;
  double minNeutralinoMass;
  double maxNeutralinoMass;
  parameterSpaceRegion() {
    minGluinoMass = -1.0;
    maxGluinoMass = -1.0;
    minNeutralinoMass = -1.0;
    maxNeutralinoMass = -1.0;
  }
  void setParameters(double minGluinoMass_, double maxGluinoMass_, double minNeutralinoMass_, double maxNeutralinoMass_) {
    minGluinoMass = minGluinoMass_;
    maxGluinoMass = maxGluinoMass_;
    minNeutralinoMass = minNeutralinoMass_;
    maxNeutralinoMass = maxNeutralinoMass_;
  }
  bool contains(double gluinoMass, double neutralinoMass) {
    return (((gluinoMass > minGluinoMass) && (gluinoMass < maxGluinoMass)) && ((neutralinoMass > minNeutralinoMass) && (neutralinoMass < maxNeutralinoMass)));
  }
};

struct argumentsStruct {
  std::string inputMCPathMain, inputPUWeightsPathMain, inputHLTEfficienciesPathMain, MCTemplatePath, crossSectionsFilePath, eventProgenitor, outputDirectory, outputPrefix;
  std::vector<std::string> inputMCPathsAux, inputPUWeightsPathsAux, inputHLTEfficienciesPathsAux;
  std::vector<double> integratedLuminositiesAux;
  int n_sTBinsToPlot;
  std::string inputFile_STRegionBoundaries;
  /* long maxMCEvents; */
  double sTMax_toPlot, integratedLuminosityMain;
  std::map<int, parameterSpaceRegion> specialZonesFor_sTDistributions;
};

struct outputHistogramsStruct {
  // histograms for different types of weights

  TH1F* h_overallWeights;
  TH1F* h_hltEfficiency;
  TH1F* h_pileupWeights;
  TH1F* h_prefiringWeights;
  TH1F* h_photonMCScaleFactorWeights;

  // histograms for nominal number of events
  // syntax: histograms[regionIndex][nJetsBin] where regionIndex ranges from 1 to (1 + number of ST signal bins), where regionIndex 1 corresponds to the normalization bin
  std::map< int, std::map< int, TH2I* > > h_totalNEvents;
  std::map< int, std::map< int, TH2F* > > h_lumiBasedYearWeightedNEvents; // nominal
  std::map< int, std::map< int, TH2F* > > h_lumiBasedYearWeightedNEvents_prefiringDown;
  std::map< int, std::map< int, TH2F* > > h_lumiBasedYearWeightedNEvents_prefiringUp;
  std::map< int, std::map< int, TH2F* > > h_lumiBasedYearWeightedNEvents_HLTDown;
  std::map< int, std::map< int, TH2F* > > h_lumiBasedYearWeightedNEvents_HLTUp;
  std::map< int, std::map< int, TH2F* > > h_lumiBasedYearWeightedNEvents_photonScaleFactorDown;
  std::map< int, std::map< int, TH2F* > > h_lumiBasedYearWeightedNEvents_photonScaleFactorUp;

  // shifted distributions
  // syntax: histograms[shiftType][regionIndex][nJetsBin] where shiftType is a predefined enum used in the event selection
  std::map< shiftType, std::map< int, std::map< int, TH2I* > > > h_totalNEvents_shifted;
  // no need for weighted histograms for shifted distributions

  // first the nominal distributions
  // syntax: histograms[specialZoneIndex][nJetsBin]
  std::map< int, std::map< int, TH1F* > > h_sTDistributions;
  // syntax: histograms[specialZoneIndex][shiftType][nJetsBin]
  std::map< int, std::map< shiftType, std::map< int, TH1F* > > > h_sTDistributions_shifted;
};

std::string getHistogramName(shiftType shiftTypeIndex, std::string histogramType, int regionIndex, int nJetsBin) {
  std::stringstream nameStream;
  if (histogramType == "totalNEvents_shifted") {
    nameStream << "totalNEvents_shifted_" << shiftTypeNames[shiftTypeIndex] << "_STRegion" << regionIndex << "_" << nJetsBin << "Jets";
  }
  else if (histogramType == "sTDistribution") {
    nameStream << "sTDistribution_shiftType_" << shiftTypeNames[shiftTypeIndex] << "_zoneIndex_" << regionIndex << "_" << nJetsBin << "Jets";
  }
  else {
    std::cout << "ERROR: histogramType: " << histogramType << " is incompatible with this call signature." << std::endl;
    std::exit(EXIT_FAILURE);
  }
  return nameStream.str();
}

std::string getHistogramName(std::string histogramType, int regionIndex, int nJetsBin) {
  std::stringstream nameStream;
  std::string tmp = "lumiBasedYearWeightedNEvents";
  if ((histogramType == "totalNEvents") || (histogramType.compare(0, tmp.length(), tmp) == 0)) nameStream << histogramType << "_STRegion" << regionIndex << "_" << nJetsBin << "Jets";
  else if (histogramType == "sTDistribution") {
    nameStream << "sTDistribution_zoneIndex_" << regionIndex << "_" << nJetsBin << "Jets";
  }
  else {
    std::cout << "ERROR: histogramType: " << histogramType << " is incompatible with this call signature." << std::endl;
    std::exit(EXIT_FAILURE);
  }
  return nameStream.str();
}

std::string getHistogramTitle(shiftType shiftTypeIndex, std::string histogramType, int regionIndex, int nJetsBin, argumentsStruct& arguments, const STRegionsStruct& STRegions) {
  if (histogramType == "totalNEvents_shifted") {
    std::string histogramTypeString = "Total MC events, shifted: " + shiftTypeNames[shiftTypeIndex];

    std::stringstream nJetsStringStream;
    if (nJetsBin >= 2 && nJetsBin < 6) nJetsStringStream << nJetsBin << " Jets";
    else if (nJetsBin == 6) nJetsStringStream << "#geq 6 Jets";
    else {
      std::cout << "ERROR: Inappropriate nJets bin: " << nJetsBin << std::endl;
      std::exit(EXIT_FAILURE);
    }
    std::string nJetsString = nJetsStringStream.str();

    std::stringstream sTRangeStringStream;
    if (regionIndex >= 1 && regionIndex < (1+STRegions.nSTSignalBins)){ // all except the last bin
      sTRangeStringStream << std::fixed << std::setprecision(0) << (STRegions.STAxis).GetBinLowEdge(regionIndex) << " < #it{S}_{T} < " << (STRegions.STAxis).GetBinUpEdge(regionIndex);
    }
    else if (regionIndex == (1+STRegions.nSTSignalBins)) {
      sTRangeStringStream << "#it{S}_{T} > " << std::fixed << std::setprecision(0) << (STRegions.STAxis).GetBinLowEdge(regionIndex);
    }
    else {
      std::cout << "ERROR: unexpected zone ID: " << regionIndex << std::endl;
      std::exit(EXIT_FAILURE);
    }
    std::string sTRangeString = sTRangeStringStream.str();

    std::string axesLabelsString = ";m_{#tilde{#it{g}}};m_{#tilde{#it{#chi_{1}^{0}}}}";

    std::stringstream titleStream;
    titleStream << histogramTypeString << ", " << nJetsString << ", " << sTRangeString << axesLabelsString;
    return titleStream.str();
  }
  else if (histogramType == "sTDistribution") {
    std::stringstream histogramTitle;
    histogramTitle << "sT Distribution, shift: " << shiftTypeNames[shiftTypeIndex]
                   << std::fixed << std::setprecision(0)
                   << arguments.specialZonesFor_sTDistributions[regionIndex].minGluinoMass
                   << " < m_{#tilde{#it{g}}} < "
                   << arguments.specialZonesFor_sTDistributions[regionIndex].maxGluinoMass << ", "
                   << arguments.specialZonesFor_sTDistributions[regionIndex].minNeutralinoMass
                   << " < m_{#tilde{#it{#chi_{1}^{0}}}} < "
                   << arguments.specialZonesFor_sTDistributions[regionIndex].maxNeutralinoMass
                   << ", " << nJetsBin << " Jets;#it{S}_{T}(GeV);Weighted nEvents/("
                   << static_cast<int>(0.5 + ((arguments.sTMax_toPlot - STRegions.STNormRangeMin)/arguments.n_sTBinsToPlot))
                   << " GeV)";
    return histogramTitle.str();
  }
  else {
    std::cout << "ERROR: histogramType: " << histogramType << " is incompatible with this call signature." << std::endl;
    std::exit(EXIT_FAILURE);
  }
}

std::string getHistogramTitle(std::string histogramType, int regionIndex, int nJetsBin, argumentsStruct& arguments, const STRegionsStruct& STRegions) {
  std::string histogramTypeString;
  if (histogramType == "totalNEvents") {
    histogramTypeString = "Total MC Events";
  }
  else if (histogramType == "lumiBasedYearWeightedNEvents") {
    histogramTypeString = "Weighted MC Events";
  }
  else if (histogramType == "lumiBasedYearWeightedNEvents_prefiringDown") {
    histogramTypeString = "Weighted MC Events, prefiring scaled down";
  }
  else if (histogramType == "lumiBasedYearWeightedNEvents_prefiringUp") {
    histogramTypeString = "Weighted MC Events, prefiring scaled up";
  }
  else if (histogramType == "lumiBasedYearWeightedNEvents_HLTDown") {
    histogramTypeString = "Weighted MC Events, HLT scaled down";
  }
  else if (histogramType == "lumiBasedYearWeightedNEvents_HLTUp") {
    histogramTypeString = "Weighted MC Events, HLT scaled up";
  }
  else if (histogramType == "lumiBasedYearWeightedNEvents_photonScaleFactorDown") {
    histogramTypeString = "Weighted MC Events, photon scale factor down";
  }
  else if (histogramType == "lumiBasedYearWeightedNEvents_photonScaleFactorUp") {
    histogramTypeString = "Weighted MC Events, photon scale factor up";
  }
  else if (histogramType == "sTDistribution") {
    std::stringstream histogramTitle;
    histogramTitle << "sT Distribution, " << std::fixed << std::setprecision(0)
                   << arguments.specialZonesFor_sTDistributions[regionIndex].minGluinoMass
                   << " < m_{#tilde{#it{g}}} < "
                   << arguments.specialZonesFor_sTDistributions[regionIndex].maxGluinoMass << ", "
                   << arguments.specialZonesFor_sTDistributions[regionIndex].minNeutralinoMass
                   << " < m_{#tilde{#it{#chi_{1}^{0}}}} < "
                   << arguments.specialZonesFor_sTDistributions[regionIndex].maxNeutralinoMass
                   << ", " << nJetsBin << " Jets;#it{S}_{T}(GeV);Weighted nEvents/("
                   << static_cast<int>(0.5 + ((arguments.sTMax_toPlot - STRegions.STNormRangeMin)/arguments.n_sTBinsToPlot))
                   << " GeV)";
    return histogramTitle.str();
  }
  else {
    std::cout << "ERROR: histogramType: " << histogramType << " is incompatible with this call signature." << std::endl;
    std::exit(EXIT_FAILURE);
  }

  std::stringstream nJetsStringStream;
  if (nJetsBin >= 2 && nJetsBin < 6) nJetsStringStream << nJetsBin << " Jets";
  else if (nJetsBin == 6) nJetsStringStream << "#geq 6 Jets";
  else {
    std::cout << "ERROR: Inappropriate nJets bin: " << nJetsBin << std::endl;
    std::exit(EXIT_FAILURE);
  }
  std::string nJetsString = nJetsStringStream.str();

  std::stringstream sTRangeStringStream;
  if (regionIndex >= 1 && regionIndex < (1+STRegions.nSTSignalBins)){ // all except the last bin
    sTRangeStringStream << std::fixed << std::setprecision(0) << (STRegions.STAxis).GetBinLowEdge(regionIndex) << " < #it{S}_{T} < " << (STRegions.STAxis).GetBinUpEdge(regionIndex);
  }
  else if (regionIndex == (1+STRegions.nSTSignalBins)) {
    sTRangeStringStream << "#it{S}_{T} > " << std::fixed << std::setprecision(0) << (STRegions.STAxis).GetBinLowEdge(regionIndex);
  }
  else {
    std::cout << "ERROR: unexpected zone ID: " << regionIndex << std::endl;
    std::exit(EXIT_FAILURE);
  }
  std::string sTRangeString = sTRangeStringStream.str();

  std::string axesLabelsString = ";m_{#tilde{#it{g}}};m_{#tilde{#it{#chi_{1}^{0}}}}";

  std::stringstream titleStream;
  titleStream << histogramTypeString << ", " << nJetsString << ", " << sTRangeString << axesLabelsString;
  return titleStream.str();
}

argumentsStruct getArgumentsFromParser(tmArgumentParser& argumentParser) {
  argumentsStruct arguments = argumentsStruct();
  arguments.inputMCPathMain = argumentParser.getArgumentString("inputMCPathMain");
  arguments.inputPUWeightsPathMain = argumentParser.getArgumentString("inputPUWeightsPathMain");
  arguments.inputHLTEfficienciesPathMain = argumentParser.getArgumentString("inputHLTEfficienciesPathMain");
  arguments.integratedLuminosityMain = std::stod(argumentParser.getArgumentString("integratedLuminosityMain"));
  std::string inputMCPathsAuxString = argumentParser.getArgumentString("inputMCPathsAux");
  if (!(inputMCPathsAuxString == "")) {
    std::vector<std::string> inputMCPathsAuxStringSplit = tmMiscUtils::getSplitString(inputMCPathsAuxString, ";");
    for (std::string inputMCPathAux: inputMCPathsAuxStringSplit) {
      (arguments.inputMCPathsAux).push_back(inputMCPathAux);
    }

    std::string inputPUWeightsPathsAuxString = argumentParser.getArgumentString("inputPUWeightsPathsAux");
    std::vector<std::string> inputPUWeightsPathsAuxStringSplit = tmMiscUtils::getSplitString(inputPUWeightsPathsAuxString, ";");
    assert(arguments.inputMCPathsAux.size() == inputPUWeightsPathsAuxStringSplit.size());
    for (std::string inputPUWeightsPathAux: inputPUWeightsPathsAuxStringSplit) {
      (arguments.inputPUWeightsPathsAux).push_back(inputPUWeightsPathAux);
    }

    std::string inputHLTEfficienciesPathsAuxString = argumentParser.getArgumentString("inputHLTEfficienciesPathsAux");
    std::vector<std::string> inputHLTEfficienciesPathsAuxStringSplit = tmMiscUtils::getSplitString(inputHLTEfficienciesPathsAuxString, ";");
    assert(arguments.inputMCPathsAux.size() == inputHLTEfficienciesPathsAuxStringSplit.size());
    for (std::string inputHLTEfficienciesPathAux: inputHLTEfficienciesPathsAuxStringSplit) {
      (arguments.inputHLTEfficienciesPathsAux).push_back(inputHLTEfficienciesPathAux);
    }

    std::string integratedLuminositiesAuxString = argumentParser.getArgumentString("integratedLuminositiesAux");
    std::vector<std::string> integratedLuminositiesAuxStringSplit = tmMiscUtils::getSplitString(integratedLuminositiesAuxString, ";");
    assert(arguments.inputMCPathsAux.size() == integratedLuminositiesAuxStringSplit.size());
    for (unsigned int auxIndex = 0; auxIndex < static_cast<unsigned int>((arguments.inputMCPathsAux).size()); ++auxIndex) {
      (arguments.integratedLuminositiesAux).push_back(std::stod(integratedLuminositiesAuxStringSplit.at(auxIndex)));
    }
  }

  /* arguments.maxMCEvents = std::stol(argumentParser.getArgumentString("maxMCEvents")); */
  arguments.crossSectionsFilePath = argumentParser.getArgumentString("crossSectionsFilePath");
  arguments.eventProgenitor = argumentParser.getArgumentString("eventProgenitor");
  arguments.inputFile_STRegionBoundaries = argumentParser.getArgumentString("inputFile_STRegionBoundaries");
  arguments.sTMax_toPlot = std::stod(argumentParser.getArgumentString("sTMax_toPlot"));
  arguments.n_sTBinsToPlot = std::stoi(argumentParser.getArgumentString("n_sTBinsToPlot"));
  arguments.outputDirectory = argumentParser.getArgumentString("outputDirectory");
  arguments.outputPrefix = argumentParser.getArgumentString("outputPrefix");
  arguments.MCTemplatePath = argumentParser.getArgumentString("MCTemplatePath");
  std::vector<std::string> regionArguments = tmMiscUtils::getSplitString(argumentParser.getArgumentString("regionsIn_sTHistograms"), "|");
  int specialZoneIndex = 1;
  for (const auto& regionArgument : regionArguments) {
    std::vector<std::string> massBoundaries = tmMiscUtils::getSplitString(regionArgument, ":");
    if (!(massBoundaries.size() == 4)) {
      std::cout << "ERROR: passed mass boundaries must have exactly four values in the format minGluinoMass:maxGluinoMass:minNeutralinoMas:maxNeutralinoMass. The problematic boundaries set passed is: " << regionArgument << std::endl;
      std::exit(EXIT_FAILURE);
    }
    arguments.specialZonesFor_sTDistributions[specialZoneIndex] = parameterSpaceRegion();
    arguments.specialZonesFor_sTDistributions[specialZoneIndex].setParameters(std::stod(massBoundaries[0]), std::stod(massBoundaries[1]), std::stod(massBoundaries[2]), std::stod(massBoundaries[3]));
    ++specialZoneIndex;
  }
  return arguments;
}

std::map<shiftType, EColor> shiftTypeColors = {
  {shiftType::JECDown, kBlue},
  {shiftType::JECUp, kBlue},
  {shiftType::UnclusteredMETDown, kRed},
  {shiftType::UnclusteredMETUp, kRed},
  {shiftType::JERMETDown, kMagenta},
  {shiftType::JERMETUp, kMagenta},
  {shiftType::missingHEMDown, kGreen},
  {shiftType::missingHEMUp, kGreen}
};
