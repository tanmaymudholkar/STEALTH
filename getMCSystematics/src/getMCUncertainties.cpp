#include "../include/getMCUncertainties.h"

optionsStruct getOptionsFromParser(tmArgumentParser& argumentParser) {
  optionsStruct options = optionsStruct();
  options.inputPath = argumentParser.getArgumentString("inputPath");
  options.MCTemplate = argumentParser.getArgumentString("MCTemplate");
  options.outputDirectory = argumentParser.getArgumentString("outputDirectory");
  options.outputPrefix = argumentParser.getArgumentString("outputPrefix");
  options.nGluinoMassBins = std::stoi(argumentParser.getArgumentString("nGluinoMassBins"));
  options.minGluinoMass = std::stod(argumentParser.getArgumentString("minGluinoMass"));
  options.maxGluinoMass = std::stod(argumentParser.getArgumentString("maxGluinoMass"));
  options.nNeutralinoMassBins = std::stoi(argumentParser.getArgumentString("nNeutralinoMassBins"));
  options.minNeutralinoMass = std::stod(argumentParser.getArgumentString("minNeutralinoMass"));
  options.maxNeutralinoMass = std::stod(argumentParser.getArgumentString("maxNeutralinoMass"));
  options.sTMin_normWindow = std::stod(argumentParser.getArgumentString("sTMin_normWindow"));
  options.sTMax_normWindow = std::stod(argumentParser.getArgumentString("sTMax_normWindow"));
  options.sTStartMainRegion = std::stod(argumentParser.getArgumentString("sTStartMainRegion"));
  return options;
}

std::string getHistogramName(const std::string& histogramType, const std::string& zone, const int& nJetsBin) {
  std::stringstream nameStream;
  nameStream << histogramType << "_" << nJetsBin << "Jets_" << zone;
  return nameStream.str();
}

std::string getHistogramTitle(optionsStruct options, const std::string& histogramType, const std::string& zone, const int& nJetsBin) {
  std::string histogramTypeString;
  if (histogramType == "JECUncertainty") histogramTypeString = "JEC Uncertainty";
  else if (histogramType == "JECRatioUpToNominal") histogramTypeString = "nEvents passing: (JEC up)/(no JEC shift)";
  else if (histogramType == "JECRatioDownToNominal") histogramTypeString = "nEvents passing: (JEC down)/(no JEC shift)";
  else if (histogramType == "MCStatisticsFractionalError") histogramTypeString = "Fractional error due to MC statistics";
  else {
    std::cout << "ERROR: Unrecognized histogram type: " << histogramType << std::endl;
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
  if (zone == "sub") sTRangeStringStream << std::fixed << std::setprecision(0) << options.sTMax_normWindow << " < #it{S}_{T} < " << options.sTStartMainRegion;
  else if (zone == "main") sTRangeStringStream << "#it{S}_{T} > " << std::fixed << std::setprecision(0) << options.sTStartMainRegion;
  else {
    std::cout << "ERROR: Unknown zone: " << zone << std::endl;
    std::exit(EXIT_FAILURE);
  }
  std::string sTRangeString = sTRangeStringStream.str();

  std::string axesLabelsString = ";m_{#tilde{#it{g}}};m_{#tilde{#it{#chi_{1}^{0}}}}";

  std::stringstream titleStream;
  titleStream << histogramTypeString << ", " << nJetsString << ", " << sTRangeString << axesLabelsString;
  return titleStream.str();
}

outputHistogramsStruct* initializeOutputHistograms(optionsStruct& options, const std::vector<std::string>& allowedZones) {
  outputHistogramsStruct* outputHistograms = new outputHistogramsStruct();
  for (const auto& zone: allowedZones) {
    for (int nJetsBin = 4; nJetsBin <= 6; ++nJetsBin) {
      outputHistograms->h_JECUncertainty[zone][nJetsBin] = new TH2F(("h_" + getHistogramName("JECUncertainty", zone, nJetsBin)).c_str(), getHistogramTitle(options, "JECUncertainty", zone, nJetsBin).c_str(), options.nGluinoMassBins, options.minGluinoMass, options.maxGluinoMass, options.nNeutralinoMassBins, options.minNeutralinoMass, options.maxNeutralinoMass);
      outputHistograms->h_ratios_JECUpToNominal[zone][nJetsBin] = new TH2F(("h_" + getHistogramName("JECRatioUpToNominal", zone, nJetsBin)).c_str(), getHistogramTitle(options, "JECRatioUpToNominal", zone, nJetsBin).c_str(), options.nGluinoMassBins, options.minGluinoMass, options.maxGluinoMass, options.nNeutralinoMassBins, options.minNeutralinoMass, options.maxNeutralinoMass);
      outputHistograms->h_ratios_JECDownToNominal[zone][nJetsBin] = new TH2F(("h_" + getHistogramName("JECRatioDownToNominal", zone, nJetsBin)).c_str(), getHistogramTitle(options, "JECRatioDownToNominal", zone, nJetsBin).c_str(), options.nGluinoMassBins, options.minGluinoMass, options.maxGluinoMass, options.nNeutralinoMassBins, options.minNeutralinoMass, options.maxNeutralinoMass);
      outputHistograms->h_MCStatisticsFractionalError[zone][nJetsBin] = new TH2F(("h_" + getHistogramName("MCStatisticsFractionalError", zone, nJetsBin)).c_str(), getHistogramTitle(options, "MCStatisticsFractionalError", zone, nJetsBin).c_str(), options.nGluinoMassBins, options.minGluinoMass, options.maxGluinoMass, options.nNeutralinoMassBins, options.minNeutralinoMass, options.maxNeutralinoMass);
    } // end loop over nJetsBin
  } // end loop over zone
  return outputHistograms;
}

inputHistogramsStruct* readInputHistograms(TFile *inputFile, const std::vector<std::string>& allowedJECs, const std::vector<std::string>& allowedZones) {
  inputHistogramsStruct *inputHistograms = new inputHistogramsStruct();
  for (const auto& jec: allowedJECs) {
    for (const auto& zone: allowedZones) {
      for (int nJetsBin = 4; nJetsBin <= 6; ++nJetsBin) {
        std::stringstream nameStreamTotalNEvents;
        nameStreamTotalNEvents << "h_total_nMCEvents_" << jec << "_" << nJetsBin << "Jets_" << zone;
        inputHistograms->h_totalNEvents[jec][zone][nJetsBin] = (TH2F*)(inputFile->Get(nameStreamTotalNEvents.str().c_str()));
        if (inputHistograms->h_totalNEvents[jec][zone][nJetsBin] == nullptr) {
          std::cout << "Unable to find histogram with name " << nameStreamTotalNEvents.str() << std::endl;
          std::exit(EXIT_FAILURE);
        }
        std::stringstream nameStreamWeightedNEvents;
        nameStreamWeightedNEvents << "h_weighted_nMCEvents_" << jec << "_" << nJetsBin << "Jets_" << zone;
        inputHistograms->h_weightedNEvents[jec][zone][nJetsBin] = (TH2F*)(inputFile->Get(nameStreamWeightedNEvents.str().c_str()));
        if (inputHistograms->h_weightedNEvents[jec][zone][nJetsBin] == nullptr) {
          std::cout << "Unable to find histogram with name " << nameStreamWeightedNEvents.str() << std::endl;
          std::exit(EXIT_FAILURE);
        }
      } // ends loop over NJetsBin
    } // ends loop over zones
  } // ends loop over JECs
  return inputHistograms;
}

TH2F* readMCTemplate(TFile *inputFile) {
  TH2F *MCTemplateTH2 = (TH2F*)(inputFile->Get("h_susyMasses_template"));
  if (MCTemplateTH2 == nullptr) {
    std::cout << "ERROR: MC template not found" << std::endl;
    std::exit(EXIT_FAILURE);
  }
  return MCTemplateTH2;
}

void fillSystematicsHistograms(outputHistogramsStruct *outputHistograms, optionsStruct& options, const std::vector<std::string>& allowedJECs, const std::vector<std::string>& allowedZones) {
  TFile *inputFile = TFile::Open(options.inputPath.c_str(), "READ");
  if (inputFile->IsZombie() || !(inputFile->IsOpen())) {
    std::cout << "Error in opening file " << options.inputPath << std::endl;
    std::exit(EXIT_FAILURE);
  }
  inputHistogramsStruct *inputHistograms = readInputHistograms(inputFile, allowedJECs, allowedZones);

  TFile *MCTemplateFile = TFile::Open(options.MCTemplate.c_str(), "READ");
  if (MCTemplateFile->IsZombie() || !(MCTemplateFile->IsOpen())) {
    std::cout << "Error in opening file " << options.MCTemplate << std::endl;
    std::exit(EXIT_FAILURE);
  }
  TH2F *MCTemplateTH2 = readMCTemplate(MCTemplateFile);

  std::cout << "Getting systematics..." << std::endl;

  // Fill TGraphs and TH2s with the JEC fractional uncertainty and estimated error
  for (const auto& zone: allowedZones) {
    for (int nJetsBin = 4; nJetsBin <= 6; ++nJetsBin) {
      // Loop over only those bins that show a "1" in the MC template file
      for (int templateGluinoMassIndex = 1; templateGluinoMassIndex <= MCTemplateTH2->GetXaxis()->GetNbins(); ++templateGluinoMassIndex) {
        double gluinoMass = MCTemplateTH2->GetXaxis()->GetBinCenter(templateGluinoMassIndex);
        for (int templateNeutralinoMassIndex = 1; templateNeutralinoMassIndex <= MCTemplateTH2->GetYaxis()->GetNbins(); ++templateNeutralinoMassIndex) {
          if (!(1 == static_cast<int>(0.5 + MCTemplateTH2->GetBinContent(templateGluinoMassIndex, templateNeutralinoMassIndex)))) continue;
          double neutralinoMass = MCTemplateTH2->GetYaxis()->GetBinCenter(templateNeutralinoMassIndex);
          double weightedNEvents_jecUp = inputHistograms->h_weightedNEvents["JECUp"][zone][nJetsBin]->GetBinContent(inputHistograms->h_weightedNEvents["JECUp"][zone][nJetsBin]->FindFixBin(gluinoMass, neutralinoMass));
          double weightedNEvents_nominal = inputHistograms->h_weightedNEvents["JECNominal"][zone][nJetsBin]->GetBinContent(inputHistograms->h_weightedNEvents["JECNominal"][zone][nJetsBin]->FindFixBin(gluinoMass, neutralinoMass));
          double weightedNEvents_jecDown = inputHistograms->h_weightedNEvents["JECDown"][zone][nJetsBin]->GetBinContent(inputHistograms->h_weightedNEvents["JECDown"][zone][nJetsBin]->FindFixBin(gluinoMass, neutralinoMass));
          double totalNEvents_nominal = inputHistograms->h_totalNEvents["JECNominal"][zone][nJetsBin]->GetBinContent(inputHistograms->h_totalNEvents["JECNominal"][zone][nJetsBin]->FindFixBin(gluinoMass, neutralinoMass));
          if (totalNEvents_nominal == 0) {
            std::cout << "WARNING: zero events recorded at gluino mass: " << gluinoMass << ", neutralino mass: " << neutralinoMass << ", for zone: " << zone << ", nJets: " << nJetsBin << std::endl;
            continue;
          }
          if (weightedNEvents_nominal == 0) {
            std::cout << "ERROR: total number of recorded events is nonzero but weighted number of recorded events is 0 at gluino mass: " << gluinoMass << ", neutralino mass: " << neutralinoMass << ", for zone: " << zone << ", nJets: " << nJetsBin << std::endl;
            std::exit(EXIT_FAILURE);
          }

          double totalNEventsError_nominal = inputHistograms->h_totalNEvents["JECNominal"][zone][nJetsBin]->GetBinError(inputHistograms->h_totalNEvents["JECNominal"][zone][nJetsBin]->FindFixBin(gluinoMass, neutralinoMass));

          double fractionalMCStatisticsUncertainty = totalNEventsError_nominal/totalNEvents_nominal;
          outputHistograms->h_MCStatisticsFractionalError[zone][nJetsBin]->SetBinContent(outputHistograms->h_MCStatisticsFractionalError[zone][nJetsBin]->FindFixBin(gluinoMass, neutralinoMass), fractionalMCStatisticsUncertainty);

          double ratioUpToNominal = weightedNEvents_jecUp / weightedNEvents_nominal;
          double deviationUp = std::fabs(ratioUpToNominal - 1.0);
          double ratioDownToNominal = weightedNEvents_jecDown / weightedNEvents_nominal;
          double deviationDown = std::fabs(ratioDownToNominal - 1.0);
          double averageDeviation = 0.5*(deviationUp + deviationDown);
          outputHistograms->h_JECUncertainty[zone][nJetsBin]->SetBinContent(outputHistograms->h_JECUncertainty[zone][nJetsBin]->FindFixBin(gluinoMass, neutralinoMass), averageDeviation);
          outputHistograms->h_ratios_JECUpToNominal[zone][nJetsBin]->SetBinContent(outputHistograms->h_ratios_JECUpToNominal[zone][nJetsBin]->FindFixBin(gluinoMass, neutralinoMass), ratioUpToNominal);
          outputHistograms->h_ratios_JECDownToNominal[zone][nJetsBin]->SetBinContent(outputHistograms->h_ratios_JECDownToNominal[zone][nJetsBin]->FindFixBin(gluinoMass, neutralinoMass), ratioDownToNominal);
        } // end loop over neutralino mass
      } // end loop over gluino mass
    } // end loop over nJetsBin
  } // end loop over zone

  MCTemplateFile->Close();
  inputFile->Close();
}

void savePlots(outputHistogramsStruct *outputHistograms, optionsStruct &options, const std::vector<std::string>& allowedZones) {
  std::cout << "Saving output plots..." << std::endl;
  TFile *outputFile = TFile::Open((options.outputDirectory + "/" + options.outputPrefix + "_MCUncertainties_savedObjects.root").c_str(), "RECREATE");
  for (const auto& zone: allowedZones) {
    for (int nJetsBin = 4; nJetsBin <= 6; ++nJetsBin) {
      std::string histogramName_JECUncertainty = getHistogramName("JECUncertainty", zone, nJetsBin);
      tmROOTSaverUtils::saveSingleObject(outputHistograms->h_JECUncertainty[zone][nJetsBin], "c_h_" + histogramName_JECUncertainty, outputFile, options.outputDirectory + "/" + options.outputPrefix + "_" + histogramName_JECUncertainty + "_hist.png", 1024, 768, 0, ".0e", "TEXTCOLZ", false, false, true, 0, 0, 0, 0, 0, 0);
      std::string histogramName_ratios_JECUpToNominal = getHistogramName("ratios_JECUpToNominal", zone, nJetsBin);
      outputHistograms->h_ratios_JECUpToNominal[zone][nJetsBin]->SetMarkerSize(0.7);
      tmROOTSaverUtils::saveSingleObject(outputHistograms->h_ratios_JECUpToNominal[zone][nJetsBin], "c_h_" + histogramName_ratios_JECUpToNominal, outputFile, options.outputDirectory + "/" + options.outputPrefix + "_" + histogramName_ratios_JECUpToNominal + "_hist.png", 1024, 768, 0, ".1e", "TEXTCOLZ", false, false, true, 0, 0, 0, 0, 0.97, 1.03);
      std::string histogramName_ratios_JECDownToNominal = getHistogramName("ratios_JECDownToNominal", zone, nJetsBin);
      outputHistograms->h_ratios_JECDownToNominal[zone][nJetsBin]->SetMarkerSize(0.7);
      tmROOTSaverUtils::saveSingleObject(outputHistograms->h_ratios_JECDownToNominal[zone][nJetsBin], "c_h_" + histogramName_ratios_JECDownToNominal, outputFile, options.outputDirectory + "/" + options.outputPrefix + "_" + histogramName_ratios_JECDownToNominal + "_hist.png", 1024, 768, 0, ".1e", "TEXTCOLZ", false, false, true, 0, 0, 0, 0, 0.97, 1.03);
      std::string histogramName_MCStatisticsFractionalError = getHistogramName("MCStatisticsFractionalError", zone, nJetsBin);
      tmROOTSaverUtils::saveSingleObject(outputHistograms->h_MCStatisticsFractionalError[zone][nJetsBin], "c_h_" + histogramName_MCStatisticsFractionalError, outputFile, options.outputDirectory + "/" + options.outputPrefix + "_" + histogramName_MCStatisticsFractionalError + "_hist.png", 1024, 768, 0, ".0e", "TEXTCOLZ", false, false, true, 0, 0, 0, 0, 0, 0);
    }
  }
  outputFile->Close();
}

int main(int argc, char* argv[]) {
  gROOT->SetBatch();
  std::cout << "Starting program to get systematics due to uncertainty on JECs..." << std::endl;

  std::vector<std::string> allowedJECs{"JECDown", "JECNominal", "JECUp"};
  std::vector<std::string> allowedZones{"sub", "main"}; // "norm" not needed

  tmArgumentParser argumentParser = tmArgumentParser("Read in event histograms and calculate systematics due to uncertainty on jet energy corrections.");
  argumentParser.addArgument("inputPath", "analysis/MCEventHistograms/MC2018_savedObjects.root", true, "Path to ROOT file containing event histograms.");
  argumentParser.addArgument("MCTemplate", "plot_susyMasses_template.root", false, "Path to root file that contains a TH2F with bins containing points with generated masses set to 1 and all other bins set to 0.");
  argumentParser.addArgument("outputDirectory", "analysis/MCSystematics/", false, "Prefix to output files.");
  argumentParser.addArgument("outputPrefix", "", true, "Prefix to output files.");
  argumentParser.addArgument("nGluinoMassBins", "20", false, "nBins on the gluino mass axis."); // (800 - 25) GeV --> (1750 + 25) GeV in steps of 50 GeV
  argumentParser.addArgument("minGluinoMass", "775.0", false, "Min gluino mass for the 2D plots.");
  argumentParser.addArgument("maxGluinoMass", "1775.0", false, "Max gluino mass for the 2D plots.");
  argumentParser.addArgument("nNeutralinoMassBins", "133", false, "nBins on the neutralino mass axis.");
  argumentParser.addArgument("minNeutralinoMass", "93.75", false, "Min neutralino mass for the 2D plots.");
  argumentParser.addArgument("maxNeutralinoMass", "1756.25", false, "Max neutralino mass for the 2D plots."); // (100 - 6.25) GeV --> (1750 + 6.25) GeV in steps of 12.5 GeV
  argumentParser.addArgument("sTMin_normWindow", "1200.0", false, "Lower sT boundary of normalization window.");
  argumentParser.addArgument("sTMax_normWindow", "1300.0", false, "Upper sT boundary of normalization window.");
  argumentParser.addArgument("sTStartMainRegion", "2500.0", false, "Lowest value of sT in main observation bin.");

  argumentParser.setPassedStringValues(argc, argv);
  optionsStruct options = getOptionsFromParser(argumentParser);
  outputHistogramsStruct* outputHistograms = initializeOutputHistograms(options, allowedZones);
  fillSystematicsHistograms(outputHistograms, options, allowedJECs, allowedZones);
  savePlots(outputHistograms, options, allowedZones);
  return 0;
}