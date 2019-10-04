#!/usr/bin/env python

from __future__ import print_function, division

import ROOT, argparse, tmROOTUtils, pdb, tmGeneralUtils, os, sys, MCTemplateReader

inputArgumentsParser = argparse.ArgumentParser(description='Create data cards from MC and data systematics and nEvents data.')
inputArgumentsParser.add_argument('--outputPrefix', required=True, help='Prefix to output files.', type=str)
inputArgumentsParser.add_argument('--outputDirectory', default="analysis/dataCards/", help='Output directory.', type=str)
inputArgumentsParser.add_argument('--crossSectionsFile', default="SusyCrossSections13TevGluGlu.txt", help='Path to dat file that contains cross-sections as a function of gluino mass, from which to get the fractional uncertainties.',type=str)
inputArgumentsParser.add_argument('--MCTemplatePath', default="plot_susyMasses_template.root", help='Path to root file that contains a TH2F with bins containing points with generated masses set to 1 and all other bins set to 0.', type=str)
inputArgumentsParser.add_argument('--inputFile_STRegionBoundaries', default="STRegionBoundaries.dat", help='Path to file with ST region boundaries. First bin is the normalization bin, and the last bin is the last boundary to infinity.', type=str)
inputArgumentsParser.add_argument('--inputFile_MCEventHistograms', required=True, help='Input MC event histograms.', type=str)
inputArgumentsParser.add_argument('--inputFile_MCUncertainties', required=True, help='Input MC uncertainties.', type=str)
inputArgumentsParser.add_argument('--inputFile_dataSystematics', required=True, help='Input file containing fractional uncertainties from signal data.', type=str)
inputArgumentsParser.add_argument('--inputFile_dataSystematics_sTScaling', required=True, help='Input file containing sT scaling systematics from control data.', type=str)
inputArgumentsParser.add_argument('--inputFile_dataSystematics_expectedEventCounters', required=True, help='Input file containing expected number of events from signal data.', type=str)
inputArgumentsParser.add_argument('--inputFile_dataSystematics_observedEventCounters', required=True, help='Input file containing observed number of events from signal data.', type=str)
inputArgumentsParser.add_argument('--luminosityUncertainty', required=True, help='Uncertainty on the luminosity.', type=float)
inputArgumentsParser.add_argument('--runUnblinded', action='store_true', help="If this flag is set, then the signal region data is unblinded. Specifically, the entry for the observed number of events is filled from the data, rather than from the expectation values.")
inputArguments = inputArgumentsParser.parse_args()

def alignFixedWidthFloatLeft(width, precision, number):
    if not(isinstance(number, float)): sys.exit("alignFixedWidthFloatLeft called with non-float object: {o}".format(o=number))
    formatter = ""
    if (width == 0):
        formatter = "{{n:.{p}f}}".format(p=precision)
    else:
        formatter = "{{n:<{w}.{p}f}}".format(w=width, p=precision)
    returnString =formatter.format(n=number)
    # if (number == 0): return returnString
    # returnStringValue = float(returnString)
    # fractionalError = (returnStringValue - number)/number
    # if (fractionalError > 0.01):
    #     sys.exit("ERROR: the number {n} is not accurately translated into a floating point representation: {rep}".format(n=number, rep=returnString))
    return returnString

def alignFixedWidthStringLeft(width, inputString):
    if not(isinstance(inputString, str)): sys.exit("alignFixedWidthStringLeft called with non-string object: {o}".format(o=inputString))
    formatter = ""
    formatter = "{{s:<{w}}}".format(w=width)
    returnString = formatter.format(s=inputString)
    return returnString

def createDataCard(outputDirectory, outputFileName, lookupTable, nSTSignalBins, crossSectionsScaleFactor):
    dataCardTemplate = open('{outputDirectory}/{outputFileName}.txt'.format(outputDirectory=outputDirectory, outputFileName=outputFileName), 'w')
    dataCardTemplate.write("# Auto-generated by the script \"createDataCards.py\"\n")
    dataCardTemplate.write("imax {nC}  number of channels\n".format(nC=nSTSignalBins*3)) # 3 nJets bins
    dataCardTemplate.write("jmax 1   number of backgrounds\n")
    dataCardTemplate.write("kmax 11   number of nuisance parameters (sources of systematic uncertainties)\n")
    dataCardTemplate.write("------------\n")

    binDescriptions = alignFixedWidthStringLeft(15, "bin") # Example:                       "bin            STReg2_4Jets    STReg3_4Jets    STReg4_4Jets    STReg2_5Jets    STReg3_5Jets    STReg4_5Jets\n"
    binObservations = alignFixedWidthStringLeft(15, "observation") # Example:               "observation    ndataObs_r2_4J  ndataObs_r3_4J  ndataObs_r4_4J  ndataObs_r2_5J  ndataObs_r3_5J  ndataObs_r4_5J\n"
    for STRegionIndex in range(2, 2 + nSTSignalBins):
        for nJetsBin in range(4, 7):
            binDescriptions += alignFixedWidthStringLeft(20, ("STReg{i}_{n}Jets").format(i=STRegionIndex, n=nJetsBin))
            binObservations += alignFixedWidthFloatLeft(20, 3, lookupTable["ndataObs_r{i}_{n}J".format(i=STRegionIndex, n=nJetsBin)])
    dataCardTemplate.write(binDescriptions.rstrip() + "\n")
    dataCardTemplate.write(binObservations.rstrip() + "\n")
    dataCardTemplate.write("------------\n")
    binTitles = alignFixedWidthStringLeft(20, "bin") # Example:          "bin                   STReg2_4Jets    STReg2_4Jets    STReg3_4Jets    STReg3_4Jets    STReg4_4Jets    STReg4_4Jets    STReg2_5Jets    STReg2_5Jets    STReg3_5Jets    STReg3_5Jets    STReg4_5Jets    STReg4_5Jets\n"
    processLabels = alignFixedWidthStringLeft(20, "process") # Example:  "process               t7Wg            qcd             t7Wg            qcd             t7Wg            qcd             t7Wg            qcd             t7Wg            qcd             t7Wg            qcd\n"
    processIndices = alignFixedWidthStringLeft(20, "process") # Example: "process               0               1               0               1               0               1               0               1               0               1               0               1\n"
    processRates = alignFixedWidthStringLeft(20, "rate") # Example:      "rate                  nmc_r2_4J       ndataExp_r2_4J  nmc_r3_4J       ndataExp_r3_4J  nmc_r4_4J       ndataExp_r4_4J  nmc_r2_5J       ndataExp_r2_5J  nmc_r3_5J       ndataExp_r3_5J  nmc_r4_5J       ndataExp_r4_5J\n"

    for STRegionIndex in range(2, 2 + nSTSignalBins):
        for nJetsBin in range(4, 7):
            for processIndex in range(2):
                binTitles += alignFixedWidthStringLeft(17, ("STReg{i}_{n}Jets").format(i=STRegionIndex, n=nJetsBin))
                processIndices += alignFixedWidthStringLeft(17, "{i}".format(i=processIndex))
                if (processIndex == 0): # MC
                    processLabels += alignFixedWidthStringLeft(17, "t7Wg")
                    processRates += alignFixedWidthFloatLeft(17, 3, crossSectionsScaleFactor*(lookupTable["nmc_r{i}_{n}J".format(i=STRegionIndex, n=nJetsBin)]))
                else: # data
                    processLabels += alignFixedWidthStringLeft(17, "qcd")
                    processRates += alignFixedWidthFloatLeft(17, 3, lookupTable["ndataExp_r{i}_{n}J".format(i=STRegionIndex, n=nJetsBin)])
    dataCardTemplate.write(binTitles.rstrip() + "\n")
    dataCardTemplate.write(processLabels.rstrip() + "\n")
    dataCardTemplate.write(processIndices.rstrip() + "\n")
    dataCardTemplate.write(processRates.rstrip() + "\n")
    dataCardTemplate.write("------------\n")

    # Data uncertainties
    normUncertainties = alignFixedWidthStringLeft(13, "normEvents") + alignFixedWidthStringLeft(7, "lnN")
    shapeUncertainties = alignFixedWidthStringLeft(13, "shape") + alignFixedWidthStringLeft(7, "lnN")
    scaleUncertainties = alignFixedWidthStringLeft(13, "scaling") + alignFixedWidthStringLeft(7, "lnN")
    rhoUncertainties = alignFixedWidthStringLeft(13, "rho") + alignFixedWidthStringLeft(7, "lnN")

    # MC uncertainties
    lumiUncertainties = alignFixedWidthStringLeft(13, "lumi") + alignFixedWidthStringLeft(7, "lnN")
    MCStatsUncertainties = alignFixedWidthStringLeft(13, "MCStats") + alignFixedWidthStringLeft(7, "lnN")
    JECUncertainties = alignFixedWidthStringLeft(13, "JEC") + alignFixedWidthStringLeft(7, "lnN")
    UnclusteredMETUncertainties = alignFixedWidthStringLeft(13, "Unclstrd") + alignFixedWidthStringLeft(7, "lnN")
    JERMETUncertainties = alignFixedWidthStringLeft(13, "JER") + alignFixedWidthStringLeft(7, "lnN")
    prefiringWeightsUncertainties = alignFixedWidthStringLeft(13, "pref") + alignFixedWidthStringLeft(7, "lnN")
    photonScaleFactorUncertainties = alignFixedWidthStringLeft(13, "phoSF") + alignFixedWidthStringLeft(7, "lnN")

    # Get uncertainties from lookup table
    for nJetsBin in range(4, 7):
        for STRegionIndex in range(2, 2 + nSTSignalBins):
            for processIndex in range(2):
                if (processIndex == 0): # MC
                    normUncertainties += alignFixedWidthStringLeft(17, "-")
                    shapeUncertainties += alignFixedWidthStringLeft(17, "-")
                    scaleUncertainties += alignFixedWidthStringLeft(17, "-")
                    rhoUncertainties += alignFixedWidthStringLeft(17, "-")

                    lumiUncertainties += alignFixedWidthFloatLeft(17, 3, lookupTable["lumiUnc"])
                    MCStatsUncertainties += alignFixedWidthFloatLeft(17, 3, lookupTable["stat_r{i}_{n}J".format(i=STRegionIndex, n=nJetsBin)])
                    JECUncertainties += alignFixedWidthFloatLeft(17, 3, lookupTable["jec_r{i}_{n}J".format(i=STRegionIndex, n=nJetsBin)])
                    UnclusteredMETUncertainties += alignFixedWidthFloatLeft(17, 3, lookupTable["unclstrd_r{i}_{n}J".format(i=STRegionIndex, n=nJetsBin)])
                    JERMETUncertainties += alignFixedWidthFloatLeft(17, 3, lookupTable["jer_r{i}_{n}J".format(i=STRegionIndex, n=nJetsBin)])
                    prefiringWeightsUncertainties += alignFixedWidthFloatLeft(17, 3, lookupTable["pref_r{i}_{n}J".format(i=STRegionIndex, n=nJetsBin)])
                    photonScaleFactorUncertainties += alignFixedWidthFloatLeft(17, 3, lookupTable["phoSF_r{i}_{n}J".format(i=STRegionIndex, n=nJetsBin)])
                else: # data
                    normUncertainties += alignFixedWidthFloatLeft(17, 3, lookupTable["normUnc_{n}J".format(n=nJetsBin)])
                    shapeUncertainties += alignFixedWidthFloatLeft(17, 3, lookupTable["shapeUnc_r{i}".format(i=STRegionIndex)])
                    scaleUncertainties += alignFixedWidthFloatLeft(17, 3, lookupTable["scaleUnc_r{i}_{n}J".format(i=STRegionIndex, n=nJetsBin)])
                    rhoUncertainties += alignFixedWidthFloatLeft(17, 3, lookupTable["rhoUnc_r{i}".format(i=STRegionIndex)])

                    lumiUncertainties += alignFixedWidthStringLeft(17, "-")
                    MCStatsUncertainties += alignFixedWidthStringLeft(17, "-")
                    JECUncertainties += alignFixedWidthStringLeft(17, "-")
                    UnclusteredMETUncertainties += alignFixedWidthStringLeft(17, "-")
                    JERMETUncertainties += alignFixedWidthStringLeft(17, "-")
                    prefiringWeightsUncertainties += alignFixedWidthStringLeft(17, "-")
                    photonScaleFactorUncertainties += alignFixedWidthStringLeft(17, "-")

    # Write out uncertainties to template
    dataCardTemplate.write(normUncertainties.rstrip() + "\n")
    dataCardTemplate.write(shapeUncertainties.rstrip() + "\n")
    dataCardTemplate.write(scaleUncertainties.rstrip() + "\n")
    dataCardTemplate.write(rhoUncertainties.rstrip() + "\n")
    dataCardTemplate.write(lumiUncertainties.rstrip() + "\n")
    dataCardTemplate.write(MCStatsUncertainties.rstrip() + "\n")
    dataCardTemplate.write(JECUncertainties.rstrip() + "\n")
    dataCardTemplate.write(UnclusteredMETUncertainties.rstrip() + "\n")
    dataCardTemplate.write(JERMETUncertainties.rstrip() + "\n")
    dataCardTemplate.write(prefiringWeightsUncertainties.rstrip() + "\n")
    dataCardTemplate.write(photonScaleFactorUncertainties.rstrip() + "\n")
    dataCardTemplate.close()

crossSectionsInputFileObject = open(inputArguments.crossSectionsFile, 'r')
crossSectionsDictionary = {}
crossSectionsFractionalUncertaintyDictionary = {}
for line in crossSectionsInputFileObject:
    crossSectionsData = line.split()
    gluinoMassInt = int(0.5 + float(crossSectionsData[0]))
    crossSection = float(crossSectionsData[1])
    crossSectionFractionalUncertainty = 0.01*float(crossSectionsData[2])
    crossSectionsDictionary[gluinoMassInt] = crossSection
    crossSectionsFractionalUncertaintyDictionary[gluinoMassInt] = crossSectionFractionalUncertainty
crossSectionsInputFileObject.close()

# print("Read in cross-sections as a function of gluino mass:")
# tmGeneralUtils.prettyPrintDictionary(crossSectionsDictionary)

STRegionBoundariesFileObject = open(inputArguments.inputFile_STRegionBoundaries)
nSTBoundaries = 0
for STBoundaryString in STRegionBoundariesFileObject:
    if (STBoundaryString.strip()): nSTBoundaries += 1
nSTSignalBins = nSTBoundaries - 2 + 1 # First two lines are for the normalization bin, last boundary implied at infinity
print("Using {n} signal bins for ST.".format(n = nSTSignalBins))
STRegionBoundariesFileObject.close()

dataSystematics = tmGeneralUtils.getConfigurationFromFile(inputArguments.inputFile_dataSystematics)
dataSystematics_sTScaling = tmGeneralUtils.getConfigurationFromFile(inputArguments.inputFile_dataSystematics_sTScaling)
expectedEventCounters_data = tmGeneralUtils.getConfigurationFromFile(inputArguments.inputFile_dataSystematics_expectedEventCounters)
observedEventCounters_data = tmGeneralUtils.getConfigurationFromFile(inputArguments.inputFile_dataSystematics_observedEventCounters)

lookupTable = {}
for STRegionIndex in range(2, 2 + nSTSignalBins): # region index 1 is for norm bin
    lookupTable["shapeUnc_r{i}".format(i=STRegionIndex)] = 1.0 + dataSystematics["fractionalUncertainty_Shape_STRegion{i}".format(i=STRegionIndex)]
    lookupTable["rhoUnc_r{i}".format(i=STRegionIndex)] = 1.0 + dataSystematics["fractionalUncertainty_rho_STRegion{i}".format(i=STRegionIndex)]
    for nJetsBin in range(4, 7):
        lookupTable["ndataExp_r{i}_{n}J".format(i=STRegionIndex, n=nJetsBin)] = float(expectedEventCounters_data["expectedNEvents_STRegion{i}_{n}Jets".format(i=STRegionIndex, n=nJetsBin)])
        lookupTable["ndataObs_r{i}_{n}J".format(i=STRegionIndex, n=nJetsBin)] = float(expectedEventCounters_data["expectedNEvents_STRegion{i}_{n}Jets".format(i=STRegionIndex, n=nJetsBin)]) # Unless explicitly unblinded, "observed" = expected
        if (inputArguments.runUnblinded):
            lookupTable["ndataObs_r{i}_{n}J".format(i=STRegionIndex, n=nJetsBin)] = float(observedEventCounters_data["observedNEvents_STRegion{i}_{n}Jets".format(i=STRegionIndex, n=nJetsBin)])
        total_scale_uncertainty = dataSystematics_sTScaling["fractionalUncertainty_sTScaling_STRegion{i}_{n}Jets".format(i=STRegionIndex, n=nJetsBin)]
        residual_uncertainty = max(0, total_scale_uncertainty - dataSystematics["fractionalUncertainty_Shape_STRegion{i}".format(i=STRegionIndex)])
        lookupTable["scaleUnc_r{i}_{n}J".format(i=STRegionIndex, n=nJetsBin)] = 1.0 + residual_uncertainty

for nJetsBin in range(4, 7):
    lookupTable["normUnc_{n}J".format(n=nJetsBin)] = 1.0 + dataSystematics["fractionalUncertainty_normEvents_{n}Jets".format(n=nJetsBin)]

lookupTable["lumiUnc"] = 1.0 + inputArguments.luminosityUncertainty

histograms_weightedNEvents = {}
histograms_MCStatUncertainties = {}
histograms_JECUncertainties = {}
histograms_UnclusteredMETUncertainties = {}
histograms_JERMETUncertainties = {}
histograms_prefiringWeightsUncertainties = {}
histograms_photonScaleFactorUncertainties = {}

for STRegionIndex in range(2, 2 + nSTSignalBins):
    histograms_weightedNEvents[STRegionIndex] = {}
    histograms_MCStatUncertainties[STRegionIndex] = {}
    histograms_JECUncertainties[STRegionIndex] = {}
    histograms_UnclusteredMETUncertainties[STRegionIndex] = {}
    histograms_JERMETUncertainties[STRegionIndex] = {}
    histograms_prefiringWeightsUncertainties[STRegionIndex] = {}
    histograms_photonScaleFactorUncertainties[STRegionIndex] = {}

MCEventHistograms = ROOT.TFile.Open(inputArguments.inputFile_MCEventHistograms)
MCUncertainties = ROOT.TFile.Open(inputArguments.inputFile_MCUncertainties)
for nJetsBin in range(4, 7):
    for STRegionIndex in range(2, 2 + nSTSignalBins):
        histograms_weightedNEvents[STRegionIndex][nJetsBin] = ROOT.TH2F()
        MCEventHistograms.GetObject("h_lumiBasedYearWeightedNEvents_{n}Jets_STRegion{r}".format(n=nJetsBin, r=STRegionIndex), histograms_weightedNEvents[STRegionIndex][nJetsBin])
        if (not(histograms_weightedNEvents[STRegionIndex][nJetsBin])):
            sys.exit("ERROR: Histogram histograms_weightedNEvents[STRegionIndex][nJetsBin] appears to be a nullptr at STRegionIndex={r}, nJets={n}".format(r=STRegionIndex, n=nJetsBin))
        histograms_MCStatUncertainties[STRegionIndex][nJetsBin] = ROOT.TH2F()
        MCUncertainties.GetObject("h_MCStatisticsFractionalError_{n}Jets_STRegion{r}".format(n=nJetsBin, r=STRegionIndex), histograms_MCStatUncertainties[STRegionIndex][nJetsBin])
        if (not(histograms_MCStatUncertainties[STRegionIndex][nJetsBin])):
            sys.exit("ERROR: Histogram histograms_MCStatUncertainties[STRegionIndex][nJetsBin] appears to be a nullptr at STRegionIndex={r}, nJets={n}".format(r=STRegionIndex, n=nJetsBin))
        histograms_JECUncertainties[STRegionIndex][nJetsBin] = ROOT.TH2F()
        MCUncertainties.GetObject("h_JECUncertainty_{n}Jets_STRegion{r}".format(n=nJetsBin, r=STRegionIndex), histograms_JECUncertainties[STRegionIndex][nJetsBin])
        if (not(histograms_JECUncertainties[STRegionIndex][nJetsBin])):
            sys.exit("ERROR: Histogram histograms_JECUncertainties[STRegionIndex][nJetsBin] appears to be a nullptr at STRegionIndex={r}, nJets={n}".format(r=STRegionIndex, n=nJetsBin))
        histograms_UnclusteredMETUncertainties[STRegionIndex][nJetsBin] = ROOT.TH2F()
        MCUncertainties.GetObject("h_UnclusteredMETUncertainty_{n}Jets_STRegion{r}".format(n=nJetsBin, r=STRegionIndex), histograms_UnclusteredMETUncertainties[STRegionIndex][nJetsBin])
        if (not(histograms_UnclusteredMETUncertainties[STRegionIndex][nJetsBin])):
            sys.exit("ERROR: Histogram histograms_UnclusteredMETUncertainties[STRegionIndex][nJetsBin] appears to be a nullptr at STRegionIndex={r}, nJets={n}".format(r=STRegionIndex, n=nJetsBin))
        histograms_JERMETUncertainties[STRegionIndex][nJetsBin] = ROOT.TH2F()
        MCUncertainties.GetObject("h_JERMETUncertainty_{n}Jets_STRegion{r}".format(n=nJetsBin, r=STRegionIndex), histograms_JERMETUncertainties[STRegionIndex][nJetsBin])
        if (not(histograms_JERMETUncertainties[STRegionIndex][nJetsBin])):
            sys.exit("ERROR: Histogram histograms_JERMETUncertainties[STRegionIndex][nJetsBin] appears to be a nullptr at STRegionIndex={r}, nJets={n}".format(r=STRegionIndex, n=nJetsBin))
        histograms_prefiringWeightsUncertainties[STRegionIndex][nJetsBin] = ROOT.TH2F()
        MCUncertainties.GetObject("h_prefiringWeightsUncertainty_{n}Jets_STRegion{r}".format(n=nJetsBin, r=STRegionIndex), histograms_prefiringWeightsUncertainties[STRegionIndex][nJetsBin])
        if (not(histograms_prefiringWeightsUncertainties[STRegionIndex][nJetsBin])):
            sys.exit("ERROR: Histogram histograms_prefiringWeightsUncertainties[STRegionIndex][nJetsBin] appears to be a nullptr at STRegionIndex={r}, nJets={n}".format(r=STRegionIndex, n=nJetsBin))
        histograms_photonScaleFactorUncertainties[STRegionIndex][nJetsBin] = ROOT.TH2F()
        MCUncertainties.GetObject("h_photonMCScaleFactorUncertainty_{n}Jets_STRegion{r}".format(n=nJetsBin, r=STRegionIndex), histograms_photonScaleFactorUncertainties[STRegionIndex][nJetsBin])
        if (not(histograms_photonScaleFactorUncertainties[STRegionIndex][nJetsBin])):
            sys.exit("ERROR: Histogram histograms_photonScaleFactorUncertainties[STRegionIndex][nJetsBin] appears to be a nullptr at STRegionIndex={r}, nJets={n}".format(r=STRegionIndex, n=nJetsBin))

templateReader = MCTemplateReader.MCTemplateReader(inputArguments.MCTemplatePath)
for indexPair in templateReader.nextValidBin():
    gluinoBinIndex = indexPair[0]
    gluinoMass = (templateReader.gluinoMasses)[gluinoBinIndex]
    neutralinoBinIndex = indexPair[1]
    neutralinoMass = (templateReader.neutralinoMasses)[neutralinoBinIndex]
    crossSectionFractionalUncertaintyScaleFactor = 1.0 + crossSectionsFractionalUncertaintyDictionary[gluinoMass]
    print("Creating data cards for gluino mass = {gM}, neutralino mass = {nM}".format(gM=gluinoMass, nM=neutralinoMass))
    tempLookupTable = {}
    for nJetsBin in range(4, 7):
        for STRegionIndex in range(2, 2 + nSTSignalBins):
            weightedNEvents = (histograms_weightedNEvents[STRegionIndex][nJetsBin]).GetBinContent((histograms_weightedNEvents[STRegionIndex][nJetsBin]).FindFixBin(gluinoMass, neutralinoMass))
            tempLookupTable["nmc_r{i}_{n}J".format(i=STRegionIndex, n=nJetsBin)] = weightedNEvents
            statUncertainty = (histograms_MCStatUncertainties[STRegionIndex][nJetsBin]).GetBinContent((histograms_MCStatUncertainties[STRegionIndex][nJetsBin]).FindFixBin(gluinoMass, neutralinoMass))
            tempLookupTable["stat_r{i}_{n}J".format(i=STRegionIndex, n=nJetsBin)] = 1.0 + statUncertainty
            jecUncertainty = (histograms_JECUncertainties[STRegionIndex][nJetsBin]).GetBinContent((histograms_JECUncertainties[STRegionIndex][nJetsBin]).FindFixBin(gluinoMass, neutralinoMass))
            tempLookupTable["jec_r{i}_{n}J".format(i=STRegionIndex, n=nJetsBin)] = 1.0 + jecUncertainty
            unclusteredMETUncertainty = (histograms_UnclusteredMETUncertainties[STRegionIndex][nJetsBin]).GetBinContent((histograms_UnclusteredMETUncertainties[STRegionIndex][nJetsBin]).FindFixBin(gluinoMass, neutralinoMass))
            tempLookupTable["unclstrd_r{i}_{n}J".format(i=STRegionIndex, n=nJetsBin)] = 1.0 + unclusteredMETUncertainty
            JERMETUncertainty = (histograms_JERMETUncertainties[STRegionIndex][nJetsBin]).GetBinContent((histograms_JERMETUncertainties[STRegionIndex][nJetsBin]).FindFixBin(gluinoMass, neutralinoMass))
            tempLookupTable["jer_r{i}_{n}J".format(i=STRegionIndex, n=nJetsBin)] = 1.0 + JERMETUncertainty
            prefiringWeightsUncertainty = (histograms_prefiringWeightsUncertainties[STRegionIndex][nJetsBin]).GetBinContent((histograms_prefiringWeightsUncertainties[STRegionIndex][nJetsBin]).FindFixBin(gluinoMass, neutralinoMass))
            tempLookupTable["pref_r{i}_{n}J".format(i=STRegionIndex, n=nJetsBin)] = 1.0 + prefiringWeightsUncertainty
            photonScaleFactorUncertainty = (histograms_photonScaleFactorUncertainties[STRegionIndex][nJetsBin]).GetBinContent((histograms_photonScaleFactorUncertainties[STRegionIndex][nJetsBin]).FindFixBin(gluinoMass, neutralinoMass))
            tempLookupTable["phoSF_r{i}_{n}J".format(i=STRegionIndex, n=nJetsBin)] = 1.0 + photonScaleFactorUncertainty

    for lookupItem in tempLookupTable.keys():
        lookupTable[lookupItem] = tempLookupTable[lookupItem]
    createDataCard(inputArguments.outputDirectory, ("{outputPrefix}_dataCard_gluinoMassBin{gBI}_neutralinoMassBin{nBI}".format(outputPrefix=inputArguments.outputPrefix, gBI=gluinoBinIndex, nBI=neutralinoBinIndex)), lookupTable, nSTSignalBins, 1.0)
    createDataCard(inputArguments.outputDirectory, ("{outputPrefix}_dataCard_gluinoMassBin{gBI}_neutralinoMassBin{nBI}_crossSectionsDown".format(outputPrefix=inputArguments.outputPrefix, gBI=gluinoBinIndex, nBI=neutralinoBinIndex)), lookupTable, nSTSignalBins, 1.0/crossSectionFractionalUncertaintyScaleFactor)
    createDataCard(inputArguments.outputDirectory, ("{outputPrefix}_dataCard_gluinoMassBin{gBI}_neutralinoMassBin{nBI}_crossSectionsUp".format(outputPrefix=inputArguments.outputPrefix, gBI=gluinoBinIndex, nBI=neutralinoBinIndex)), lookupTable, nSTSignalBins, crossSectionFractionalUncertaintyScaleFactor)
    for lookupItem in tempLookupTable.keys():
        lookupTable.pop(lookupItem)
    tempLookupTable.clear()
