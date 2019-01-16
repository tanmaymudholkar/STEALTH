#include "../include/runEventSelection.h"

bool checkHLTBit(const ULong64_t& inputHLTBits, const int& indexOfBitToCheck) {
  return (((inputHLTBits>>indexOfBitToCheck)&1) == 1);
}

float getRhoCorrectedIsolation(const float& uncorrectedIsolation, const PFTypesForEA& PFType, const float& absEta, const float& rho, const EAValuesStruct& region1EAs, const EAValuesStruct& region2EAs) {
  float effectiveArea;
  if (absEta <= region1EAs.regionUpperBound) effectiveArea = region1EAs.getEffectiveArea(PFType);
  else if (absEta <= region2EAs.regionUpperBound) effectiveArea = region2EAs.getEffectiveArea(PFType);
  else effectiveArea = 0.0; // photons with eta > 1.479 are going to fail the kinematic cut anyway
  float correctedIsolationTest = uncorrectedIsolation - rho*effectiveArea;
  return ((correctedIsolationTest > 0.0) ? correctedIsolationTest : 0.0);
}

photonExaminationResultsStruct examinePhoton(parametersStruct &parameters, countersStruct &counters, const float& rho, const photonsCollectionStruct& photonsCollection, const int& photonIndex) {
  bool passesCommonCuts = true;
  // Kinematic cuts
  float absEta = std::fabs((photonsCollection.eta)->at(photonIndex));
  applyCondition(counters, photonFailureCategory::eta, passesCommonCuts, (absEta < parameters.photonEtaCut));
  float photon_ET = std::fabs((photonsCollection.pT)->at(photonIndex));
  applyCondition(counters, photonFailureCategory::pT, passesCommonCuts, (photon_ET > parameters.pTCutSubLeading));

  // Electron veto
  applyCondition(counters, photonFailureCategory::conversionSafeElectronVeto, passesCommonCuts, (((photonsCollection.electronVeto)->at(photonIndex)) == constants::TRUETOINTT));

  bool passesLeadingpTCut = (photon_ET > parameters.pTCutLeading);

  applyCondition(counters, photonFailureCategory::hOverE, passesCommonCuts, (((photonsCollection.HOverE)->at(photonIndex)) < parameters.towerHOverECut)); // H-over-E criterion: same for medium and fake selection

  float pTDependentNeutralIsolationCut = (parameters.neutralIsolationCut).getPolynomialValue(photon_ET);
  float rhoCorrectedNeutralIsolation = getRhoCorrectedIsolation(((photonsCollection.PFNeutralIsolationUncorrected)->at(photonIndex)), PFTypesForEA::neutralHadron, absEta, rho, parameters.region1EAs, parameters.region2EAs);
  applyCondition(counters, photonFailureCategory::neutralIsolation, passesCommonCuts, (rhoCorrectedNeutralIsolation < pTDependentNeutralIsolationCut)); // neutral isolation criterion: same for medium and fake selection

  float pTDependentPhotonIsolationCut = (parameters.photonIsolationCut).getPolynomialValue(photon_ET);
  float rhoCorrectedPhotonIsolation = getRhoCorrectedIsolation(((photonsCollection.PFPhotonIsolationUncorrected)->at(photonIndex)), PFTypesForEA::photon, absEta, rho, parameters.region1EAs, parameters.region2EAs);
  applyCondition(counters, photonFailureCategory::photonIsolation, passesCommonCuts, (rhoCorrectedPhotonIsolation < pTDependentPhotonIsolationCut)); // photon isolation criterion: same for medium and fake selection

  float photon_sigmaIEtaIEta = ((photonsCollection.sigmaIEtaIEta)->at(photonIndex));
  bool passesMedium_sigmaIEtaIEtaCut = (photon_sigmaIEtaIEta < parameters.sigmaIEtaIEtaCut);
  bool passesLoose_sigmaIEtaIEtaCut = (photon_sigmaIEtaIEta < parameters.sigmaIEtaIEtaCutLoose);

  float photon_chargedIsolation = getRhoCorrectedIsolation(((photonsCollection.PFChargedIsolationUncorrected)->at(photonIndex)), PFTypesForEA::chargedHadron, absEta, rho, parameters.region1EAs, parameters.region2EAs);
  bool passesMedium_chargedIsolationCut = (photon_chargedIsolation < parameters.chargedIsolationCut);
  bool passesLoose_chargedIsolationCut = (photon_chargedIsolation < parameters.chargedIsolationCutLoose);

  bool passesMedium_sigmaIEtaIEtaANDChargedIsolationCuts = (passesMedium_sigmaIEtaIEtaCut && passesMedium_chargedIsolationCut);
  bool passesFake_sigmaIEtaIEtaANDChargedIsolationCuts = ((!(passesMedium_sigmaIEtaIEtaANDChargedIsolationCuts)) && (passesLoose_sigmaIEtaIEtaCut && passesLoose_chargedIsolationCut)); // if either sigma-ieta-ieta or charged isolation fail the medium cut, then check if both pass the loose cuts

  bool passesSelectionAsMedium = passesCommonCuts;
  applyCondition(counters, photonFailureCategory::sigmaietaiataANDchargedIsolation, passesSelectionAsMedium, passesMedium_sigmaIEtaIEtaANDChargedIsolationCuts);
  bool passesSelectionAsFake = passesCommonCuts;
  applyCondition(counters, photonFailureCategory::sigmaietaiataANDchargedIsolationLoose, passesSelectionAsFake, passesFake_sigmaIEtaIEtaANDChargedIsolationCuts);

  if (passesSelectionAsFake || passesSelectionAsMedium) incrementCounters(miscCounter::passingPhotons, counters);
  else incrementCounters(miscCounter::failingPhotons, counters);
  incrementCounters(miscCounter::totalPhotons, counters);

  photonExaminationResultsStruct photonExaminationResults = photonExaminationResultsStruct(passesSelectionAsMedium, passesSelectionAsFake, passesLeadingpTCut, ((photonsCollection.eta)->at(photonIndex)), ((photonsCollection.phi)->at(photonIndex)), ((photonsCollection.pT)->at(photonIndex)), ((photonsCollection.energy)->at(photonIndex)));
  return photonExaminationResults;
}

bool passesMCSelection(parametersStruct &parameters, const int& nMCParticles, const MCCollectionStruct& MCCollection) {
  int nPhotonsWithNeutralinoMom = 0;
  for (int MCIndex = 0; MCIndex < nMCParticles; ++MCIndex) {
    if ((((MCCollection.MCPIDs)->at(MCIndex)) == parameters.PIDs.photon) && (((MCCollection.MCMomPIDs)->at(MCIndex)) == parameters.PIDs.neutralino)) {
      if (passesBitMask((MCCollection.MCStatusFlags)->at(MCIndex), parameters.MCStatusFlagBitMask)) {
        ++nPhotonsWithNeutralinoMom;
      }
    }
  }
  return (nPhotonsWithNeutralinoMom == 2);
}

float getDiphotonInvariantMass(const std::vector<TLorentzVector>& selectedPhotonFourMomentaList) {
  TLorentzVector eventSum;
  for (const auto& selectedPhotonFourMomentum : selectedPhotonFourMomentaList) {
    eventSum += selectedPhotonFourMomentum;
  }
  return (eventSum.M());
}

float findPrefireWeight(const float& eta, const float& pT, const int& year, TEfficiency* efficiencyMap_2016, TH2F* efficiencyMap_2017) {
  switch(year) {
  case (2016) :
    return (1.0 - (efficiencyMap_2016->GetEfficiency(efficiencyMap_2016->FindFixBin(std::fabs(eta), pT)))); // 2016 map is a TEfficiency versus abs(eta)
  case (2017) :
    return (1.0 - (efficiencyMap_2017->GetBinContent(efficiencyMap_2017->FindFixBin(eta, pT)))); // 2017 map is a TH2F versus eta
  default :
    std::cout << "ERROR: Prefire efficiencies unavailable for year " << year << std::endl;
    std::exit(EXIT_FAILURE);
  }
}

jetExaminationResultsStruct examineJet(optionsStruct &options, parametersStruct &parameters, countersStruct &counters, const jetsCollectionStruct& jetsCollection, const int& jetIndex) {
  bool passesJetSelection = true;
  bool passesJetSelectionJECDown = false;
  bool passesJetSelectionJECUp = false;

  //Kinematic cuts: eta, pT
  float absEta = std::fabs((jetsCollection.eta)->at(jetIndex));
  applyCondition(counters, jetFailureCategory::eta, passesJetSelection, (absEta < parameters.jetEtaCut));
  float jet_pT = ((jetsCollection.pT)->at(jetIndex));
  float jecFractionalUncertainty = 0.;
  if (options.isMC) {
    jecFractionalUncertainty = (jetsCollection.JECUncertainty)->at(jetIndex);
    float jet_pT_JECDown = (1.0 - jecFractionalUncertainty)*jet_pT;
    float jet_pT_JECUp = (1.0 + jecFractionalUncertainty)*jet_pT;
    passesJetSelectionJECDown = (jet_pT_JECDown > parameters.jetpTCut);
    passesJetSelectionJECUp = (jet_pT_JECUp > parameters.jetpTCut);
  }
  applyCondition(counters, jetFailureCategory::pT, passesJetSelection, (jet_pT > parameters.jetpTCut));

  float prefireWeight = findPrefireWeight((jetsCollection.eta)->at(jetIndex), jet_pT, options.year, parameters.efficiencyMap_2016, parameters.efficiencyMap_2017);

  // ID cuts: loose ID, PUID, jetID
  // applyCondition(counters, jetFailureCategory::PFLooseID, passesJetSelection, (((jetsCollection.looseID)->at(jetIndex)))); // disable until better understanding
  applyCondition(counters, jetFailureCategory::puID, passesJetSelection, (((jetsCollection.PUID)->at(jetIndex)) > parameters.jetPUIDThreshold));
  applyCondition(counters, jetFailureCategory::jetID, passesJetSelection, (((jetsCollection.ID)->at(jetIndex)) == 6));

  if (passesJetSelection) incrementCounters(miscCounter::passingJets, counters);
  else incrementCounters(miscCounter::failingJets, counters);
  incrementCounters(miscCounter::totalJets, counters);

  jetExaminationResultsStruct result = jetExaminationResultsStruct(passesJetSelection, passesJetSelectionJECDown, passesJetSelectionJECUp, ((jetsCollection.eta)->at(jetIndex)), ((jetsCollection.phi)->at(jetIndex)), jet_pT, prefireWeight, jecFractionalUncertainty);
  return result;
}

float getMinDeltaR(const float& jetEta, const float& jetPhi, const std::vector<angularVariablesStruct>& selectedPhotonAnglesList) {
  float min_dR = -1.0;
  for (const auto& selectedPhotonAngles : selectedPhotonAnglesList) {
    float deltaEta = selectedPhotonAngles.eta - jetEta;
    float smallerPhi, largerPhi;
    if (selectedPhotonAngles.phi > jetPhi) {
      largerPhi = selectedPhotonAngles.phi;
      smallerPhi = jetPhi;
    }
    else {
      smallerPhi = selectedPhotonAngles.phi;
      largerPhi = jetPhi;
    }
    float deltaPhi_direction1 = largerPhi - smallerPhi;
    float deltaPhi_direction2 = constants::VALUEOFTWOPI - deltaPhi_direction1;
    float deltaPhi = (deltaPhi_direction1 < deltaPhi_direction2) ? deltaPhi_direction1 : deltaPhi_direction2;
    float dR = std::sqrt(deltaEta*deltaEta + deltaPhi*deltaPhi);
    if ((min_dR < 0) || (dR < min_dR)) min_dR = dR;
  }
  return min_dR;
}

eventExaminationResultsStruct examineEvent(optionsStruct &options, parametersStruct &parameters, countersStruct &counters, Long64_t& entryIndex, eventDetailsStruct& eventDetails, MCCollectionStruct &MCCollection, photonsCollectionStruct &photonsCollection, jetsCollectionStruct &jetsCollection /* , electronsCollectionStruct &electronsCollection, muonsCollectionStruct &muonsCollection */ ) {
  int evt_nJetsDR = 0;
  float evt_ST = 0.0;
  float evt_scaleFactor = 1.0;
  std::map<shiftType, float> shifted_ST = empty_STMap();
  std::map<shiftType, int> shifted_nJetsDR = empty_NJetsMap();
  bool passesEventSelection = true;
  if (!(options.isMC) && parameters.HLTPhotonBit >= 0) { // Apply HLT photon selection iff input is not MC and HLTBit is set to a positive integer
    applyCondition(counters, eventFailureCategory::HLTPhoton, passesEventSelection, checkHLTBit(eventDetails.HLTPhotonBits, parameters.HLTPhotonBit));
  }

  // Photon selection
  std::vector<angularVariablesStruct> selectedPhotonAnglesList;
  std::vector<TLorentzVector> selectedPhotonFourMomentaList;
  int nSelectedPhotonsPassingSubLeadingpTCut = 0;
  int nSelectedPhotonsPassingLeadingpTCut = 0;
  int nMediumPhotons = 0;
  int nFakePhotons = 0;
  for (Int_t photonIndex = 0; photonIndex < (eventDetails.nPhotons); ++photonIndex) {
    photonExaminationResultsStruct photonExaminationResults = examinePhoton(parameters, counters, (eventDetails.eventRho), photonsCollection, photonIndex);
    if (photonExaminationResults.passesSelectionAsMedium || photonExaminationResults.passesSelectionAsFake) {
      ++nSelectedPhotonsPassingSubLeadingpTCut;
      if (photonExaminationResults.passesLeadingpTCut) ++nSelectedPhotonsPassingLeadingpTCut;
      evt_ST += photonExaminationResults.pT;
      if (options.isMC) {
        for (int shiftTypeIndex = shiftTypeFirst; shiftTypeIndex != static_cast<int>(shiftType::nShiftTypes); ++shiftTypeIndex) {
          shiftType typeIndex = static_cast<shiftType>(shiftTypeIndex);
          addShiftedEToSTMap(photonExaminationResults.pT, shifted_ST, typeIndex); // no effect on photon's contribution to ST due to any of the shifts
        }
      }
      angularVariablesStruct angularVariables = angularVariablesStruct(photonExaminationResults.eta, photonExaminationResults.phi);
      selectedPhotonAnglesList.push_back(angularVariables);
      TLorentzVector photonFourMomentum;
      photonFourMomentum.SetPtEtaPhiE(photonExaminationResults.pT, photonExaminationResults.eta, photonExaminationResults.phi, photonExaminationResults.energy);
      selectedPhotonFourMomentaList.push_back(photonFourMomentum);
    }
    if (photonExaminationResults.passesSelectionAsMedium) ++nMediumPhotons;
    else if (photonExaminationResults.passesSelectionAsFake) ++nFakePhotons;
  }

  applyCondition(counters, eventFailureCategory::wrongNSelectedPhotons, passesEventSelection, ((nSelectedPhotonsPassingSubLeadingpTCut == 2) && (nSelectedPhotonsPassingLeadingpTCut >= 1)));
  applyCondition(counters, eventFailureCategory::incompatiblePhotonSelectionType, passesEventSelection, ((nMediumPhotons == parameters.nMediumPhotonsRequired) && (nFakePhotons == parameters.nFakePhotonsRequired)));

  // Apply invariant mass cut iff event selection is passed
  float evt_invariantMass = -1.0;
  if ((nSelectedPhotonsPassingSubLeadingpTCut == 2) && (nSelectedPhotonsPassingLeadingpTCut >= 1)) {
    evt_invariantMass = getDiphotonInvariantMass(selectedPhotonFourMomentaList);
    applyCondition(counters, eventFailureCategory::lowInvariantMass, passesEventSelection, (evt_invariantMass >= parameters.invariantMassCut));
  }

  // Additional photon selection, only for MC
  if (options.isMC) applyCondition(counters, eventFailureCategory::MCGenInformation, passesEventSelection, (passesMCSelection(parameters, (eventDetails.nMCParticles), MCCollection)));

  // Jet selection
  float evt_HT = 0;
  for (Int_t jetIndex = 0; jetIndex < (eventDetails.nJets); ++jetIndex) {
    jetExaminationResultsStruct jetExaminationResults = examineJet(options, parameters, counters, jetsCollection, jetIndex);
    evt_scaleFactor *= jetExaminationResults.prefireWeight; // All jets, whether or not they pass any of the cuts, contribute to the prefiring weight
    float min_dR = getMinDeltaR(jetExaminationResults.eta, jetExaminationResults.phi, selectedPhotonAnglesList);
    bool passesDeltaRCut = ((min_dR > parameters.minDeltaRCut) || (min_dR < 0.0));
    if (!passesDeltaRCut) {
      incrementCounters(jetFailureCategory::deltaR, counterType::global, counters);
      if (jetExaminationResults.passesSelectionJECNominal) incrementCounters(jetFailureCategory::deltaR, counterType::differential, counters);
    }
    if (jetExaminationResults.passesSelectionJECNominal) {
      evt_HT += jetExaminationResults.pT; // Add hT whether or not jet passes deltaR check
      if (passesDeltaRCut) {
        evt_ST += jetExaminationResults.pT; // Add to sT only if jet passes deltaR check, to avoid double-counting
        ++evt_nJetsDR; // Count only those jets that are sufficiently away from a photon
      }
    }
    if (options.isMC && passesDeltaRCut) {
      for (int shiftTypeIndex = shiftTypeFirst; shiftTypeIndex != static_cast<int>(shiftType::nShiftTypes); ++shiftTypeIndex) {
        shiftType typeIndex = static_cast<shiftType>(shiftTypeIndex);
        float shifted_contribution = (jetExaminationResults.pT);
        bool passes_selection = jetExaminationResults.passesSelectionJECNominal;
        if (typeIndex == shiftType::JECDown) {
          passes_selection = jetExaminationResults.passesSelectionJECDown;
          shifted_contribution = (1.0 - (jetExaminationResults.jecFractionalUncertainty))*(jetExaminationResults.pT);
        }
        else if (typeIndex == shiftType::JECUp) {
          passes_selection = jetExaminationResults.passesSelectionJECUp;
          shifted_contribution = (1.0 + (jetExaminationResults.jecFractionalUncertainty))*(jetExaminationResults.pT);
        }
        if (passes_selection) {
          addShiftedEToSTMap(shifted_contribution, shifted_ST, typeIndex);
          incrementNJetsMap(shifted_nJetsDR, typeIndex);
        }
      }
    }
  }
  int min_nJets = evt_nJetsDR;
  if (options.isMC) { // this makes sure that the nJets used to make the decision whether or not to save the event is the minimum nJets accounting for all the shifts
    int minNJetsShifted = getMinNJets(shifted_nJetsDR);
    if (minNJetsShifted < min_nJets) min_nJets = minNJetsShifted;
  }
  applyCondition(counters, eventFailureCategory::wrongNJets, passesEventSelection, (min_nJets >= 2));
  applyCondition(counters, eventFailureCategory::hTCut, passesEventSelection, (evt_HT >= parameters.HTCut));

  // Add MET to ST
  evt_ST += eventDetails.PFMET;

  // Add shifted energies
  if (options.isMC) {
    addShiftedEToSTMap(eventDetails.PFMET, shifted_ST, shiftType::JECDown);
    addShiftedEToSTMap(eventDetails.PFMET, shifted_ST, shiftType::JECUp);
    addShiftedEToSTMap(eventDetails.PFMET_UnclusteredDown, shifted_ST, shiftType::UnclusteredMETDown);
    addShiftedEToSTMap(eventDetails.PFMET_UnclusteredUp, shifted_ST, shiftType::UnclusteredMETUp);
    addShiftedEToSTMap(eventDetails.PFMET_JERDown, shifted_ST, shiftType::JERMETDown);
    addShiftedEToSTMap(eventDetails.PFMET_JERUp, shifted_ST, shiftType::JERMETUp);
  }

  eventExaminationResultsStruct eventResult = eventExaminationResultsStruct(entryIndex, passesEventSelection, evt_ST, evt_nJetsDR, evt_scaleFactor, shifted_ST, shifted_nJetsDR);
  return eventResult;
}

std::vector<eventExaminationResultsStruct> getSelectedEventsWithInfo(optionsStruct &options, parametersStruct &parameters, countersStruct &counters) {
  std::vector<eventExaminationResultsStruct> selectedEventsInfo;

  std::ifstream fileWithInputFilesList(options.inputFilesList);
  if (!fileWithInputFilesList.is_open()) {
    std::cout << "ERROR: Failed to open file with path: " << options.inputFilesList << std::endl;
    std::exit(EXIT_FAILURE);
  }
  TChain inputChain("ggNtuplizer/EventTree");
  std::cout << "Starting to add files to chain..." << std::endl;
  while (!fileWithInputFilesList.eof()) {
    std::string inputFileName;
    fileWithInputFilesList >> inputFileName;
    if (!inputFileName.empty()) {
      // std::cout << "Adding... " << inputFileName << std::endl;
      inputChain.Add(inputFileName.c_str()); // Add files to TChain
    }
  }
  std::cout << "Finished adding files to chain!" << std::endl;
  fileWithInputFilesList.close();

  Long64_t nEvts = inputChain.GetEntries();
  Long64_t nEntriesToProcess = 1 + options.counterEndInclusive - options.counterStartInclusive;
  std::cout << "Number of available events: " << nEvts << std::endl;

  inputChain.SetBranchStatus("*", 0); // so that only the needed branches, explicitly activated below, are read in per event

  eventDetailsStruct eventDetails = eventDetailsStruct(inputChain, options.isMC);
  photonsCollectionStruct photonsCollection = photonsCollectionStruct(inputChain);
  jetsCollectionStruct jetsCollection = jetsCollectionStruct(inputChain, options.isMC);
  // electronsCollectionStruct electronsCollection = electronsCollectionStruct(inputChain);
  // muonsCollectionStruct muonsCollection = muonsCollectionStruct(inputChain);
  MCCollectionStruct MCCollection = MCCollectionStruct(inputChain, options.isMC);

  tmProgressBar progressBar = tmProgressBar(static_cast<int>(nEntriesToProcess));
  int progressBarUpdatePeriod = ((nEntriesToProcess < 1000) ? 1 : static_cast<int>(0.5 + 1.0*(nEntriesToProcess/1000)));
  progressBar.initialize();
  for (Long64_t entryIndex = options.counterStartInclusive; entryIndex <= options.counterEndInclusive; ++entryIndex) {
    if (entryIndex > nEvts) {
      std::cout << "ERROR: Entry index falls outside event range. Index: " << entryIndex << ", available number of events: " << nEvts << std::endl;
      std::exit(EXIT_FAILURE);
    }
    Long64_t loadStatus = inputChain.LoadTree(entryIndex);
    if (loadStatus < 0) {
      std::cout << "ERROR in loading tree for entry index: " << entryIndex << "; load status = " << loadStatus << std::endl;
      std::exit(EXIT_FAILURE);
    }
    int nBytesRead = inputChain.GetEntry(entryIndex, 0); // Get only the required branches
    if (nBytesRead <= 0) {
      std::cout << "ERROR: Failed to read SOME information from entry at index: " << entryIndex << "; nBytesRead = " << nBytesRead << std::endl;
      std::exit(EXIT_FAILURE);
    }

    int entryProcessing = static_cast<int>(entryIndex - options.counterStartInclusive);
    if (entryProcessing > 0 && ((static_cast<int>(entryProcessing) % progressBarUpdatePeriod == 0) || entryProcessing == static_cast<int>(nEntriesToProcess-1))) progressBar.updateBar(static_cast<double>(1.0*entryProcessing/nEntriesToProcess), entryProcessing);

    eventExaminationResultsStruct eventExaminationResults = examineEvent(options, parameters, counters, entryIndex, eventDetails, MCCollection, photonsCollection, jetsCollection /* , electronsCollection, muonsCollection*/ );
    bool passesEventSelection = eventExaminationResults.passesSelection;
    incrementCounters(miscCounter::totalEvents, counters);
    if (!(passesEventSelection)) {
      incrementCounters(miscCounter::failingEvents, counters);
      continue;
    }
    incrementCounters(miscCounter::acceptedEvents, counters);
    selectedEventsInfo.push_back(eventExaminationResults);
  }
  progressBar.terminate();
  return selectedEventsInfo;
}

void writeSelectedEventsToFile(optionsStruct &options, TFile *outputFile, const std::vector<eventExaminationResultsStruct>& selectedEventsInfo) {
  std::cout << "Beginning to write selected events to file..." << std::endl;
  std::ifstream fileWithInputFilesList(options.inputFilesList);
  if (!fileWithInputFilesList.is_open()) {
    std::cout << "ERROR: Failed to open file with path: " << options.inputFilesList << std::endl;
    std::exit(EXIT_FAILURE);
  }
  TChain inputChain("ggNtuplizer/EventTree");
  std::cout << "Starting to add files to chain..." << std::endl;
  while (!fileWithInputFilesList.eof()) {
    std::string inputFileName;
    fileWithInputFilesList >> inputFileName;
    if (!inputFileName.empty()) {
      // std::cout << "Adding... " << inputFileName << std::endl;
      inputChain.Add(inputFileName.c_str()); // Add files to TChain
    }
  }
  std::cout << "Finished adding files to chain!" << std::endl;
  fileWithInputFilesList.close();

  TDirectory *outDir = outputFile->mkdir("ggNtuplizer");
  outDir->cd();
  TTree *outputTree = inputChain.CloneTree(0);

  int nJetsDR; // stores number of jets in event passing deltaR cut
  outputTree->Branch("b_nJets", &nJetsDR, "b_nJets/I");
  float ST; // stores event sT
  outputTree->Branch("b_evtST", &ST, "b_evtST/F");
  float scaleFactor; // stores event scale factor accounting for prefiring
  outputTree->Branch("b_evtScaleFactor", &scaleFactor, "b_evtScaleFactor/F");

  std::map<shiftType, float> shifted_ST;
  std::map<shiftType, int> shifted_nJetsDR;
  if (options.isMC) {
    std::string branchPrefix_ST = "b_evtST_shifted_";
    std::string branchPrefix_nJets = "b_nJets_shifted_";
    for (int shiftTypeIndex = shiftTypeFirst; shiftTypeIndex != static_cast<int>(shiftType::nShiftTypes); ++shiftTypeIndex) {
      shiftType typeIndex = static_cast<shiftType>(shiftTypeIndex);
      shifted_ST[typeIndex] = 0.;
      outputTree->Branch((getShiftedVariableBranchName(typeIndex, "evtST")).c_str(), &(shifted_ST[typeIndex]), (getShiftedVariableBranchName(typeIndex, "evtST") + "/F").c_str());
      shifted_nJetsDR[typeIndex] = 0;
      outputTree->Branch((getShiftedVariableBranchName(typeIndex, "nJets")).c_str(), &(shifted_nJetsDR[typeIndex]), (getShiftedVariableBranchName(typeIndex, "nJets") + "/I").c_str());
    }
  }

  int nSelectedEvents = static_cast<int>(0.5 + selectedEventsInfo.size());
  tmProgressBar progressBar = tmProgressBar(nSelectedEvents);
  int progressBarUpdatePeriod = ((nSelectedEvents < 1000) ? 1 : static_cast<int>(0.5 + 1.0*(nSelectedEvents/1000)));
  int processingIndex = -1;
  progressBar.initialize();

  for (auto& selectedEventInfo : selectedEventsInfo) {
    ++processingIndex;

    Long64_t index = selectedEventInfo.eventIndex;
    nJetsDR = selectedEventInfo.evt_nJetsDR;
    ST = selectedEventInfo.evt_ST;
    scaleFactor = selectedEventInfo.evt_scaleFactor;
    if (options.isMC) {
      for (int shiftTypeIndex = shiftTypeFirst; shiftTypeIndex != static_cast<int>(shiftType::nShiftTypes); ++shiftTypeIndex) {
        shiftType typeIndex = static_cast<shiftType>(shiftTypeIndex);
        shifted_ST[typeIndex] = (selectedEventInfo.evt_shifted_ST).at(typeIndex);
        shifted_nJetsDR[typeIndex] = (selectedEventInfo.evt_shifted_nJetsDR).at(typeIndex);
      }
    }

    Long64_t loadStatus = inputChain.LoadTree(index);
    if (loadStatus < 0) {
      std::cout << "ERROR in loading tree for entry index: " << index << "; load status = " << loadStatus << std::endl;
      std::exit(EXIT_FAILURE);
    }
    int nBytesRead = inputChain.GetEntry(index, 1); // Get all branches before filling output tree
    if (nBytesRead <= 0) {
      std::cout << "ERROR: For selected event, failed to read ALL information from entry at index: " << index << "; nBytesRead = " << nBytesRead << std::endl;
      std::exit(EXIT_FAILURE);
    }
    if (processingIndex > 0 && ((processingIndex%progressBarUpdatePeriod == 0) || processingIndex == static_cast<int>(nSelectedEvents-1))) progressBar.updateBar(static_cast<double>(1.0*processingIndex/nSelectedEvents), processingIndex);

    outputTree->Fill();
  }
  progressBar.terminate();
  outputFile->Write();
}

int main(int argc, char* argv[]) {
  gROOT->SetBatch();
  tmArgumentParser argumentParser = tmArgumentParser("Calculate systematics due to uncertainty on jet energy corrections.");
  argumentParser.addArgument("inputFilesList", "", true, "Path to file containing list of input files.");
  argumentParser.addArgument("outputFilePath", "", true, "Path to output file.");
  argumentParser.addArgument("isMC", "false", false, "Input file is a MC sample -- disable HLT photon trigger and enable additional MC selection.");
  argumentParser.addArgument("counterStartInclusive", "", true, "Event number from input file from which to start. The event with this index is included in the processing.");
  argumentParser.addArgument("counterEndInclusive", "", true, "Event number from input file at which to end. The event with this index is included in the processing.");
  argumentParser.addArgument("photonSelectionType", "fake", true, "Photon selection type: can be any one of: \"fake\", \"medium\", \"mediumfake\"");
  argumentParser.addArgument("year", "2017", false, "Year of data-taking. Affects the HLT photon Bit index in the format of the n-tuplizer on which to trigger (unless sample is MC), and the photon ID cuts which are based on year-dependent recommendations.");
  argumentParser.setPassedStringValues(argc, argv);

  optionsStruct options = getOptionsFromParser(argumentParser);

  parametersStruct parameters = parametersStruct();
  parameters.tuneParametersForYear(options.year, options.isMC);
  parameters.tuneParametersForPhotonSelectionType(options.photonSelectionType);

  countersStruct counters = countersStruct();
  initializeCounters(counters);

  std::stringstream optionsStringstream;
  optionsStringstream << options;
  TNamed *optionsObject = new TNamed("optionsString", optionsStringstream.str().c_str());
  std::stringstream parametersStringstream;
  parametersStringstream << parameters;
  TNamed *parametersObject = new TNamed("parametersString", parametersStringstream.str().c_str());
  TFile *outputFile = TFile::Open(options.outputFilePath.c_str(), "RECREATE");
  if (!(outputFile->IsOpen()) || outputFile->IsZombie()) {
    std::cout << "ERROR: Unable to open output file to write. File path: " << options.outputFilePath << std::endl;
  }

  std::vector<eventExaminationResultsStruct> selectedEventsInfo = getSelectedEventsWithInfo(options, parameters, counters);

  writeSelectedEventsToFile(options, outputFile, selectedEventsInfo);

  outputFile->WriteTObject(parametersObject);
  outputFile->WriteTObject(optionsObject);
  outputFile->Close();

  std::cout << getNDashes(100) << std::endl;
  printCounters(counters);

  std::cout << getNDashes(100) << std::endl
            << "Options:" << std::endl
            << optionsStringstream.str() << std::endl
            << getNDashes(100) << std::endl;
  std::cout << getNDashes(100) << std::endl
            << "Parameters:" << std::endl
            << parametersStringstream.str() << std::endl
            << getNDashes(100) << std::endl;
  return 0;
}
