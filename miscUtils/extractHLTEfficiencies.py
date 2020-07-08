#!/usr/bin/env python

from __future__ import print_function, division

import ROOT, os, sys, argparse, stealthEnv
inputArgumentsParser = argparse.ArgumentParser(description='Extract HLT efficiencies histograms and save them to output image.')
inputArgumentsParser.add_argument('--outputFolder', default="~/nobackup/analysisAreas/HLTEfficiencies", help='Path to folder in which to store output files.',type=str)
inputArgumentsParser.add_argument('--outputPrefix', default="HLTEfficiencies", help='Path to folder in which to store output files.',type=str)
inputArguments = inputArgumentsParser.parse_args()

ROOT.gROOT.SetBatch(ROOT.kTRUE)

sources = {
    "clean": "{eP}/{sER}/statistics/combined_DoublePhoton_HLTEfficiencyStudies/merged_statistics_MC_hgg_allJets_2017.root".format(eP=stealthEnv.EOSPrefix, sER=stealthEnv.stealthEOSRoot),
    "unclean": "{eP}/{sER}/statistics/combined_DoublePhoton_HLTEfficiencyStudies/merged_statistics_MC_QCD_allJets_2017.root".format(eP=stealthEnv.EOSPrefix, sER=stealthEnv.stealthEOSRoot)
}

targets = {
    "signal": "hltEfficiency1D_leadingPhoton_signal",
    "signal_loose": "hltEfficiency1D_leadingPhoton_signal_loose",
    "control_fakefake": "hltEfficiency1D_leadingPhoton_control_fakefake"
}

for selection, efficiencyName in targets.items():
    print("Extracting selection: {s}, efficiencyName: {eN}".format(s=selection, eN=efficiencyName))
    outputFile = ROOT.TFile.Open(inputArguments.outputFolder + "/" + inputArguments.outputPrefix + "_" + selection + ".root", "RECREATE")
    if ((outputFile.IsZombie() == ROOT.kTRUE) or not(outputFile.IsOpen() == ROOT.kTRUE)): sys.exit("ERROR: Unable to open file \"{f}\"".format(f=inputArguments.outputPrefix + "_" + selection + ".root"))
    for sourceType, path in sources.items():
        print("Fetching efficiency of type: \"{sT}\" from path: {p}".format(sT=sourceType, p=path))
        inputFile = ROOT.TFile.Open(path, "READ")
        if ((inputFile.IsZombie() == ROOT.kTRUE) or not(inputFile.IsOpen() == ROOT.kTRUE)): sys.exit("ERROR: Unable to open file at path \"{p}\"".format(p=path))
        efficiencyToFetch = ROOT.TEfficiency()
        efficiency_label = (efficiencyName.replace("1D_leadingPhoton", "")).replace("_" + selection, "") + "_" + sourceType
        efficiencyToFetch.SetName(efficiency_label)
        inputFile.GetObject(efficiencyName, efficiencyToFetch)
        efficiencyToFetch.SetName(efficiency_label)
        c = ROOT.TCanvas("output_" + efficiency_label, "output_" + efficiency_label, 1024, 768)
        efficiencyToFetch.Draw()
        c.SaveAs(inputArguments.outputFolder + "/" + efficiency_label + "_" + selection + ".png")
        outputFile.WriteTObject(efficiencyToFetch)
        inputFile.Close()
    outputFile.Close()

print("All done!")