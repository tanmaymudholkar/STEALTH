#ifndef H_STATISTICSHISTOGRAMS
#define H_STATISTICSHISTOGRAMS

#include <iostream>
#include <cstdlib>
#include <map>

#include "TFile.h"
#include "TH1.h"
#include "TH1F.h"
#include "TEfficiency.h"

#include "objectProperties.h"
#include "selectionCriteria.h"
#include "MCRegions.h"

class statisticsHistograms {
 public:
  std::map<std::string, TH1F*> stats;
  std::map<std::string, TEfficiency*> stats_HLTEmulation;
  std::map<std::string, TEfficiency*> stats_IDEfficiency;
  float IDEfficiency_STMin = -1.;
  float IDEfficiency_STMax = -1.;

  std::string getStatisticsHistogramName(const eventProperty& event_property, const selectionRegion& region) {
    return ((eventPropertyAttributes.at(event_property)).name + "_" + selectionRegionNames.at(region) + "_selectedEvents");
  }

  std::string getStatisticsHistogramName(const eventProperty& event_property, const selectionRegion& region, const int& MCRegionIndex) {
    return ((eventPropertyAttributes.at(event_property)).name + "_" + selectionRegionNames.at(region) + "_selectedEvents_MC_" + MCRegions::regionNames.at(MCRegionIndex));
  }

  std::string getStatisticsHistogramName(const eventProperty& event_property, const eventSelectionCriterion& criterion) {
    return ((eventPropertyAttributes.at(event_property)).name + "_marginallyUnselectedEvents_" + eventSelectionCriterionNames.at(criterion));
  }

  std::string getStatisticsHistogramName(const eventProperty& event_property, const eventSelectionCriterion& criterion, const int& MCRegionIndex) {
    return ((eventPropertyAttributes.at(event_property)).name + "_marginallyUnselectedEvents_" + eventSelectionCriterionNames.at(criterion) + "_MC_" + MCRegions::regionNames.at(MCRegionIndex));
  }

  std::string getStatisticsHistogramName(const truthPhotonProperty& truth_photon_property, const selectionRegion& region, const int& MCRegionIndex) {
    return ((truthPhotonPropertyAttributes.at(truth_photon_property)).name + "_" + selectionRegionNames.at(region) + "_truePhotons_MC_" + MCRegions::regionNames.at(MCRegionIndex));
  }

  std::string getStatisticsHistogramName(const truthJetCandidateProperty& truth_jetCandidate_property, const selectionRegion& region, const int& MCRegionIndex) {
    return ((truthJetCandidatePropertyAttributes.at(truth_jetCandidate_property)).name + "_" + selectionRegionNames.at(region) + "_trueJetCandidates_all_MC_" + MCRegions::regionNames.at(MCRegionIndex));
  }

  std::string getStatisticsHistogramName(const truthJetCandidateProperty& truth_jetCandidate_property, const selectionRegion& region, const bool& trueFromGluino_falseFromSinglet, const int& MCRegionIndex) {
    std::string name = (truthJetCandidatePropertyAttributes.at(truth_jetCandidate_property)).name + "_" + selectionRegionNames.at(region) + "_trueJetCandidates_from";
    if (trueFromGluino_falseFromSinglet) name += "Gluino";
    else name += "Singlet";
    name += "_MC_" + MCRegions::regionNames.at(MCRegionIndex);
    return name;
  }

  std::string getStatisticsHistogramName(const photonProperty& photon_property, const selectionRegion& region, const bool& trueMedium_falseFake) {
    std::string name = ((photonPropertyAttributes.at(photon_property)).name + "_" + selectionRegionNames.at(region) + "_selected");
    if (trueMedium_falseFake) name += "MediumCaloPhotons";
    else name += "FakeCaloPhotons";
    return name;
  }

  std::string getStatisticsHistogramName(const photonProperty& photon_property, const selectionRegion& region, const bool& trueMedium_falseFake, const bool& trueClose_falseAway, const int& MCRegionIndex) {
    std::string name = ((photonPropertyAttributes.at(photon_property)).name + "_" + selectionRegionNames.at(region) + "_selected");
    if (trueMedium_falseFake) name += "MediumCaloPhotons_";
    else name += "FakeCaloPhotons_";
    if (trueClose_falseAway) name += "closeToTruePhoton_MC_";
    else name += "awayFromTruePhotons_MC_";
    name += MCRegions::regionNames.at(MCRegionIndex);
    return name;
  }

  std::string getStatisticsHistogramName(const photonProperty& photon_property, const selectionRegion& region, const mediumPhotonCriterion& medium_photon_criterion) {
    return ((photonPropertyAttributes.at(photon_property)).name + "_" + selectionRegionNames.at(region) + "_marginallyUnselectedMediumCaloPhotons_" + mediumPhotonCriterionNames.at(medium_photon_criterion));
  }

  std::string getStatisticsHistogramName(const photonProperty& photon_property, const selectionRegion& region, const mediumPhotonCriterion& medium_photon_criterion, const bool& trueClose_falseAway, const int& MCRegionIndex) {
    std::string name = (photonPropertyAttributes.at(photon_property)).name + "_" + selectionRegionNames.at(region) + "_marginallyUnselectedMediumCaloPhotons_" + mediumPhotonCriterionNames.at(medium_photon_criterion) + "_";
    if (trueClose_falseAway) name += "closeToTruePhoton_MC_";
    else name += "awayFromTruePhotons_MC_";
    name += MCRegions::regionNames.at(MCRegionIndex);
    return name;
  }

  std::string getStatisticsHistogramName(const photonProperty& photon_property, const selectionRegion& region, const fakePhotonCriterion& fake_photon_criterion) {
    return ((photonPropertyAttributes.at(photon_property)).name + "_" + selectionRegionNames.at(region) + "_marginallyUnselectedFakeCaloPhotons_" + fakePhotonCriterionNames.at(fake_photon_criterion));
  }

  std::string getStatisticsHistogramName(const photonProperty& photon_property, const selectionRegion& region, const fakePhotonCriterion& fake_photon_criterion, const bool& trueClose_falseAway, const int& MCRegionIndex) {
    std::string name = (photonPropertyAttributes.at(photon_property)).name + "_" + selectionRegionNames.at(region) + "_marginallyUnselectedFakeCaloPhotons_" + fakePhotonCriterionNames.at(fake_photon_criterion) + "_";
    if (trueClose_falseAway) name += "closeToTruePhoton_MC_";
    else name += "awayFromTruePhotons_MC_";
    name += MCRegions::regionNames.at(MCRegionIndex);
    return name;
  }

  std::string getStatisticsHistogramName(const jetProperty& jet_property, const selectionRegion& region) {
    return ((jetPropertyAttributes.at(jet_property)).name + "_" + selectionRegionNames.at(region) + "_selectedCaloJets");
  }

  std::string getStatisticsHistogramName(const jetProperty& jet_property, const selectionRegion& region, const bool& trueClose_falseAway, const int& MCRegionIndex) {
    std::string name = (jetPropertyAttributes.at(jet_property)).name + "_" + selectionRegionNames.at(region) + "_selectedCaloJets_";
    if (trueClose_falseAway) name += "closeToTruePhoton_MC_";
    else name += "awayFromTruePhotons_MC_";
    name += MCRegions::regionNames.at(MCRegionIndex);
    return name;
  }

  std::string getStatisticsHistogramName(const jetProperty& jet_property, const selectionRegion& region, const jetCriterion& criterion) {
    return ((jetPropertyAttributes.at(jet_property)).name + "_" + selectionRegionNames.at(region) + "_marginallyUnselectedCaloJets_" + jetCriterionNames.at(criterion));
  }

  std::string getStatisticsHistogramName(const jetProperty& jet_property, const selectionRegion& region, const jetCriterion& criterion, const bool& trueClose_falseAway, const int& MCRegionIndex) {
    std::string name = (jetPropertyAttributes.at(jet_property)).name + "_" + selectionRegionNames.at(region) + "_marginallyUnselectedCaloJets_" + jetCriterionNames.at(criterion) + "_";
    if (trueClose_falseAway) name += "closeToTruePhoton_MC_";
    else name += "awayFromTruePhotons_MC_";
    name += MCRegions::regionNames.at(MCRegionIndex);
    return name;
  }

  std::string getStatisticsHistogramName(const genJetProperty& gen_jet_property, const selectionRegion& region, const int& MCRegionIndex) {
    return ((genJetPropertyAttributes.at(gen_jet_property)).name + "_" + selectionRegionNames.at(region) + "_all_genJets_MC_" + MCRegions::regionNames.at(MCRegionIndex));
  }

  std::string getStatisticsHistogramName(const genJetProperty& gen_jet_property, const bool& mom_trueGluino_falseSinglet, const selectionRegion& region, const int& MCRegionIndex) {
    std::string name = (genJetPropertyAttributes.at(gen_jet_property)).name + "_" + selectionRegionNames.at(region) + "_";
    if (mom_trueGluino_falseSinglet) name += "gluinoMom_";
    else name += "singletMom_";
    name += "genJets_MC_" + MCRegions::regionNames.at(MCRegionIndex);
    return name;
  }

  void initializeWithCheck(const std::string& name, const int& nBins, const float& xmin, const float& xmax) {
    if (stats.find(name) == stats.end()) {
      stats[name] = new TH1F(name.c_str(), name.c_str(), nBins, xmin, xmax);
      // stats[name].SetCanExtend(TH1::kAllAxes);
    }
    else {
      std::cout << "ERROR: tried to create new 1D statistics histogram with name \"" << name << "\", but it already exists!" << std::endl;
      std::exit(EXIT_FAILURE);
    }
  }

  void initializeHLTEfficienciesWithCheck(const std::string& name, const std::vector<double>& etaBinEdges, const std::vector<double>& pTBinEdges) {
    if (stats_HLTEmulation.find(name) == stats_HLTEmulation.end()) {
      stats_HLTEmulation[name] = new TEfficiency(name.c_str(), (name + ";eta;pT").c_str(), (etaBinEdges.size()-1), &(etaBinEdges.at(0)), (pTBinEdges.size()-1), &(pTBinEdges.at(0)));
    }
    else {
      std::cout << "ERROR: tried to create new 2D HLT efficiency object with name \"" << name << "\", but it already exists!" << std::endl;
      std::exit(EXIT_FAILURE);
    }
  }

  void initializeIDEfficienciesWithCheck(const std::string& name, const std::vector<double>& STBoundaries) {
    if (stats_IDEfficiency.find(name) == stats_IDEfficiency.end()) {
      stats_IDEfficiency[name] = new TEfficiency(name.c_str(), (name + ";ST;efficiency").c_str(), (STBoundaries.size()-1), &(STBoundaries.at(0)));
    }
    else {
      std::cout << "ERROR: tried to create new 1D ID efficiency object with name \"" << name << "\", but it already exists!" << std::endl;
      std::exit(EXIT_FAILURE);
    }
  }

  statisticsHistograms(const bool& isMC, const std::vector<double>& etaBinEdges, const std::vector<double>& pTBinEdges, const std::vector<double>& STBoundaries) {
    std::vector<double> STBoundariesModified;
    assert(STBoundaries.size() > 1);
    for (int STBoundaryIndex = 0; STBoundaryIndex < (static_cast<int>(STBoundaries.size()) - 1); ++STBoundaryIndex) STBoundariesModified.push_back(STBoundaries.at(STBoundaryIndex)); // Create a copy of STRegionBoundaries with all but the last bin, which is set at some unphysical high value.
    STBoundariesModified.push_back(3500.); // Set the last bin edge to 3500 GeV; this way the plots are readable.
    IDEfficiency_STMin = STBoundariesModified.at(0);
    IDEfficiency_STMax = STBoundariesModified.at(STBoundariesModified.size()-1);
    std::string fullName;
    for (auto&& eventPropertyAttributesElement: eventPropertyAttributes) {
      auto& event_property = (eventPropertyAttributesElement.first);
      auto& attributesElement = (eventPropertyAttributesElement.second);
      for (auto&& selectionRegionNamesElement: selectionRegionNames) {
        auto& region = selectionRegionNamesElement.first;
        fullName = getStatisticsHistogramName(event_property, region);
        initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
        if (isMC) {
          for (auto&& MCRegionNamesElement: MCRegions::regionNames) {
            auto& MCRegionIndex = MCRegionNamesElement.first;
            fullName = getStatisticsHistogramName(event_property, region, MCRegionIndex);
            initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
          }
        }
      }
      for (auto&& eventSelectionCriterionNamesElement: eventSelectionCriterionNames) {
        auto& event_selection_criterion = eventSelectionCriterionNamesElement.first;
        fullName = getStatisticsHistogramName(event_property, event_selection_criterion);
        initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
        if (isMC) {
          for (auto&& MCRegionNamesElement: MCRegions::regionNames) {
            auto& MCRegionIndex = MCRegionNamesElement.first;
            fullName = getStatisticsHistogramName(event_property, event_selection_criterion, MCRegionIndex);
            initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
          }
        }
      }
    }

    if (isMC) {
      for (auto&& truthPhotonPropertyAttributesElement: truthPhotonPropertyAttributes) {
        auto& truth_photon_property = (truthPhotonPropertyAttributesElement.first);
        auto& attributesElement = (truthPhotonPropertyAttributesElement.second);
        for (auto&& selectionRegionNamesElement: selectionRegionNames) {
          auto& region = selectionRegionNamesElement.first;
          for (auto&& MCRegionNamesElement: MCRegions::regionNames) {
            auto& MCRegionIndex = MCRegionNamesElement.first;
            fullName = getStatisticsHistogramName(truth_photon_property, region, MCRegionIndex);
            initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
          }
        }
      }
      for (auto&& jetCandidatePropertyAttributesElement: truthJetCandidatePropertyAttributes) {
        auto& truth_jetCandidate_property = (jetCandidatePropertyAttributesElement.first);
        auto& attributesElement = (jetCandidatePropertyAttributesElement.second);
        for (auto&& selectionRegionNamesElement: selectionRegionNames) {
          auto& region = selectionRegionNamesElement.first;
          for (auto&& MCRegionNamesElement: MCRegions::regionNames) {
            auto& MCRegionIndex = MCRegionNamesElement.first;
            fullName = getStatisticsHistogramName(truth_jetCandidate_property, region, MCRegionIndex);
            initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
            fullName = getStatisticsHistogramName(truth_jetCandidate_property, region, true, MCRegionIndex);
            initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
            fullName = getStatisticsHistogramName(truth_jetCandidate_property, region, false, MCRegionIndex);
            initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
          }
        }
      }
      for (auto&& genJetPropertyAttributesElement: genJetPropertyAttributes) {
        auto& gen_jet_property = (genJetPropertyAttributesElement.first);
        auto& attributesElement = (genJetPropertyAttributesElement.second);
        for (auto&& selectionRegionNamesElement: selectionRegionNames) {
          auto& region = selectionRegionNamesElement.first;
          for (auto&& MCRegionNamesElement: MCRegions::regionNames) {
            auto& MCRegionIndex = MCRegionNamesElement.first;
            fullName = getStatisticsHistogramName(gen_jet_property, region, MCRegionIndex);
            initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
            fullName = getStatisticsHistogramName(gen_jet_property, true, region, MCRegionIndex);
            initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
            fullName = getStatisticsHistogramName(gen_jet_property, false, region, MCRegionIndex);
            initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
          }
        }
      }
    } // end of condition isMC

    for (auto&& selectionRegionNamesElement: selectionRegionNames) {
      auto& region = selectionRegionNamesElement.first;
      for (auto&& photonPropertyAttributesElement: photonPropertyAttributes) {
        auto& photon_property = (photonPropertyAttributesElement.first);
        auto& attributesElement = (photonPropertyAttributesElement.second);
        fullName = getStatisticsHistogramName(photon_property, region, true);
        initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
        fullName = getStatisticsHistogramName(photon_property, region, false);
        initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
        if (isMC) {
          for (auto&& MCRegionNamesElement: MCRegions::regionNames) {
            auto& MCRegionIndex = MCRegionNamesElement.first;
            fullName = getStatisticsHistogramName(photon_property, region, true, true, MCRegionIndex);
            initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
            fullName = getStatisticsHistogramName(photon_property, region, true, false, MCRegionIndex);
            initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
            fullName = getStatisticsHistogramName(photon_property, region, false, true, MCRegionIndex);
            initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
            fullName = getStatisticsHistogramName(photon_property, region, false, false, MCRegionIndex);
            initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
          }
        } // MC plots
        for (auto&& mediumPhotonCriterionNamesElement: mediumPhotonCriterionNames) {
          auto& medium_photon_criterion = mediumPhotonCriterionNamesElement.first;
          fullName = getStatisticsHistogramName(photon_property, region, medium_photon_criterion);
          initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
          if (isMC) {
            for (auto&& MCRegionNamesElement: MCRegions::regionNames) {
              auto& MCRegionIndex = MCRegionNamesElement.first;
              fullName = getStatisticsHistogramName(photon_property, region, medium_photon_criterion, true, MCRegionIndex);
              initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
              fullName = getStatisticsHistogramName(photon_property, region, medium_photon_criterion, false, MCRegionIndex);
              initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
            }
          } // MC plots
        } // medium photon criteria
        for (auto&& fakePhotonCriterionNamesElement: fakePhotonCriterionNames) {
          auto& fake_photon_criterion = fakePhotonCriterionNamesElement.first;
          fullName = getStatisticsHistogramName(photon_property, region, fake_photon_criterion);
          initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
          if (isMC) {
            for (auto&& MCRegionNamesElement: MCRegions::regionNames) {
              auto& MCRegionIndex = MCRegionNamesElement.first;
              fullName = getStatisticsHistogramName(photon_property, region, fake_photon_criterion, true, MCRegionIndex);
              initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
              fullName = getStatisticsHistogramName(photon_property, region, fake_photon_criterion, false, MCRegionIndex);
              initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
            }
          } // MC plots
        } // fake photon criteria
      } // photon plots
      for (auto&& jetPropertyAttributesElement: jetPropertyAttributes) {
        auto& jet_property = (jetPropertyAttributesElement.first);
        auto& attributesElement = (jetPropertyAttributesElement.second);
        fullName = getStatisticsHistogramName(jet_property, region);
        initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
        if (isMC) {
          for (auto&& MCRegionNamesElement: MCRegions::regionNames) {
            auto& MCRegionIndex = MCRegionNamesElement.first;
            fullName = getStatisticsHistogramName(jet_property, region, true, MCRegionIndex);
            initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
            fullName = getStatisticsHistogramName(jet_property, region, false, MCRegionIndex);
            initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
          }
        } // MC plots
        for (auto&& jetCriterionNamesElement: jetCriterionNames) {
          auto& jet_criterion = jetCriterionNamesElement.first;
          fullName = getStatisticsHistogramName(jet_property, region, jet_criterion);
          initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
          if (isMC) {
            for (auto&& MCRegionNamesElement: MCRegions::regionNames) {
              auto& MCRegionIndex = MCRegionNamesElement.first;
              fullName = getStatisticsHistogramName(jet_property, region, jet_criterion, true, MCRegionIndex);
              initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
              fullName = getStatisticsHistogramName(jet_property, region, jet_criterion, false, MCRegionIndex);
              initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
            }
          }
        }
      } // jet plots

      // HLT efficiencies
      fullName = std::string("hltEfficiency_leadingPhoton_" + selectionRegionNames.at(region));
      initializeHLTEfficienciesWithCheck(fullName, etaBinEdges, pTBinEdges);
      fullName = std::string("hltEfficiency_subLeadingPhoton_" + selectionRegionNames.at(region));
      initializeHLTEfficienciesWithCheck(fullName, etaBinEdges, pTBinEdges);

      // ID efficiencies
      fullName = std::string("IDEfficiency_" + selectionRegionNames.at(region));
      initializeIDEfficienciesWithCheck(fullName, STBoundariesModified);
    } // ends loop over selection regions
  }

  void fillStatisticsHistogramByName(const std::string& histogramName, const float& value, const float& weight) {
    if (stats.find(histogramName) == stats.end()) {
      std::cout << "ERROR: tried to fill statistics histogram with name \"" << histogramName << "\"; a histogram with this name was not initialized!" << std::endl;
      std::exit(EXIT_FAILURE);
    }
    else {
      (stats[histogramName])->Fill(value, weight);
    }
  }

  void fillStatisticsHistogramByName(const std::string& histogramName, const float& value) {
    fillStatisticsHistogramByName(histogramName, value, 1.0);
  }

  void fillHLTEfficiencyByName(const std::string& efficiencyName, const bool& passesHLTEmulation, const float& eta, const float& pT) {
    if (stats_HLTEmulation.find(efficiencyName) == stats_HLTEmulation.end()) {
      std::cout << "ERROR: tried to fill HLT emulation statistics histogram with name \"" << efficiencyName << "\"; a histogram with this name was not initialized!" << std::endl;
      std::exit(EXIT_FAILURE);
    }
    else {
      (stats_HLTEmulation[efficiencyName])->Fill(passesHLTEmulation, eta, pT);
    }
  }

  void fillIDEfficiencyByName(const std::string& efficiencyName, const bool& passesID, const float& ST) {
    if (ST <= IDEfficiency_STMin) return;
    else if (ST >= IDEfficiency_STMax) return;

    if (stats_IDEfficiency.find(efficiencyName) == stats_IDEfficiency.end()) {
      std::cout << "ERROR: tried to fill ID efficiency statistics histogram with name \"" << efficiencyName << "\"; a histogram with this name was not initialized!" << std::endl;
      std::exit(EXIT_FAILURE);
    }
    else {
      (stats_IDEfficiency[efficiencyName])->Fill(passesID, ST);
    }
  }

  void fill1DStatisticsHistograms(eventProperties& selectedEventPropertiesMap,
                                  const bool& isMarginallyUnselectedEvent,
                                  unselectedEventProperties& marginallyUnselectedEventPropertiesPair,
                                  truthPhotonPropertiesCollection& selectedTruePhotonProperties,
                                  truthJetCandidatePropertiesCollection& selectedTrueJetCandidateProperties_all,
                                  truthJetCandidatePropertiesCollection& selectedTrueJetCandidateProperties_fromGluino,
                                  truthJetCandidatePropertiesCollection& selectedTrueJetCandidateProperties_fromSinglet,
                                  photonPropertiesCollection& selectedMediumPhotonProperties,
                                  photonPropertiesCollection& selectedMediumPhotonProperties_closeToTruePhoton,
                                  photonPropertiesCollection& selectedMediumPhotonProperties_awayFromTruePhoton,
                                  unselectedMediumPhotonPropertiesCollection& marginallyUnselectedMediumPhotonProperties,
                                  unselectedMediumPhotonPropertiesCollection& marginallyUnselectedMediumPhotonProperties_closeToTruePhoton,
                                  unselectedMediumPhotonPropertiesCollection& marginallyUnselectedMediumPhotonProperties_awayFromTruePhoton,
                                  photonPropertiesCollection& selectedFakePhotonProperties,
                                  photonPropertiesCollection& selectedFakePhotonProperties_closeToTruePhoton,
                                  photonPropertiesCollection& selectedFakePhotonProperties_awayFromTruePhoton,
                                  unselectedFakePhotonPropertiesCollection& marginallyUnselectedFakePhotonProperties,
                                  unselectedFakePhotonPropertiesCollection& marginallyUnselectedFakePhotonProperties_closeToTruePhoton,
                                  unselectedFakePhotonPropertiesCollection& marginallyUnselectedFakePhotonProperties_awayFromTruePhoton,
                                  jetPropertiesCollection& selectedJetProperties,
                                  jetPropertiesCollection& selectedJetProperties_closeToTruePhoton,
                                  jetPropertiesCollection& selectedJetProperties_awayFromTruePhoton,
                                  unselectedJetPropertiesCollection& marginallyUnselectedJetProperties,
                                  unselectedJetPropertiesCollection& marginallyUnselectedJetProperties_closeToTruePhoton,
                                  unselectedJetPropertiesCollection& marginallyUnselectedJetProperties_awayFromTruePhoton,
                                  genJetPropertiesCollection& gen_jet_properties_collection,
                                  genJetPropertiesCollection& gluino_mom_gen_jet_properties_collection,
                                  genJetPropertiesCollection& singlet_mom_gen_jet_properties_collection,
                                  selectionRegion& region,
                                  const bool& isMC,
                                  const int& MCRegionIndex) {
    if (region == selectionRegion::nSelectionRegions) return;

    if (!(isMarginallyUnselectedEvent)) {
      for (auto&& selectedEventPropertiesMapElement: selectedEventPropertiesMap) {
        auto& event_property = selectedEventPropertiesMapElement.first;
        float& value = selectedEventPropertiesMapElement.second;
        fillStatisticsHistogramByName(getStatisticsHistogramName(event_property, region), value);
      }
    }
    else {
      auto& criterion = marginallyUnselectedEventPropertiesPair.first;
      auto& propertiesMap = marginallyUnselectedEventPropertiesPair.second;
      for (auto&& propertiesMapElement: propertiesMap) {
        auto& event_property = propertiesMapElement.first;
        auto& value = propertiesMapElement.second;
        fillStatisticsHistogramByName(getStatisticsHistogramName(event_property, criterion), value);
      }
    }

    for (auto&& selectedMediumPhotonPropertiesMap: selectedMediumPhotonProperties) {
      for (auto&& element: selectedMediumPhotonPropertiesMap) {
        auto& property = element.first;
        auto& value = element.second;
        fillStatisticsHistogramByName(getStatisticsHistogramName(property, region, true), value);
      }
    }

    for (auto&& marginallyUnselectedMediumPhotonPropertiesPair: marginallyUnselectedMediumPhotonProperties) {
      mediumPhotonCriterion& criterion = marginallyUnselectedMediumPhotonPropertiesPair.first;
      photonProperties& propertiesMap = marginallyUnselectedMediumPhotonPropertiesPair.second;
      for (auto&& propertiesMapElement: propertiesMap) {
        auto& property = propertiesMapElement.first;
        auto& value = propertiesMapElement.second;
        fillStatisticsHistogramByName(getStatisticsHistogramName(property, region, criterion), value);
      }
    }

    for (auto&& selectedFakePhotonPropertiesMap: selectedFakePhotonProperties) {
      for (auto&& element: selectedFakePhotonPropertiesMap) {
        auto& property = element.first;
        auto& value = element.second;
        fillStatisticsHistogramByName(getStatisticsHistogramName(property, region, false), value);
      }
    }

    for (auto&& marginallyUnselectedFakePhotonPropertiesPair: marginallyUnselectedFakePhotonProperties) {
      auto& criterion = marginallyUnselectedFakePhotonPropertiesPair.first;
      auto& propertiesMap = marginallyUnselectedFakePhotonPropertiesPair.second;
      for (auto&& propertiesMapElement: propertiesMap) {
        auto& property = propertiesMapElement.first;
        auto& value = propertiesMapElement.second;
        fillStatisticsHistogramByName(getStatisticsHistogramName(property, region, criterion), value);
      }
    }

    for (auto&& selectedJetPropertiesMap: selectedJetProperties) {
      for (auto&& element: selectedJetPropertiesMap) {
        auto& property = element.first;
        auto& value = element.second;
        fillStatisticsHistogramByName(getStatisticsHistogramName(property, region), value);
      }
    }

    for (auto&& marginallyUnselectedJetPropertiesPair: marginallyUnselectedJetProperties) {
      auto& criterion = marginallyUnselectedJetPropertiesPair.first;
      auto& propertiesMap = marginallyUnselectedJetPropertiesPair.second;
      for (auto&& element: propertiesMap) {
        auto& property = element.first;
        auto& value = element.second;
        fillStatisticsHistogramByName(getStatisticsHistogramName(property, region, criterion), value);
      }
    }

    if (isMC && (MCRegionIndex > 0)) {
      if (!(isMarginallyUnselectedEvent)) {
        for (auto&& selectedEventPropertiesMapElement: selectedEventPropertiesMap) {
          auto& event_property = selectedEventPropertiesMapElement.first;
          float& value = selectedEventPropertiesMapElement.second;
          fillStatisticsHistogramByName(getStatisticsHistogramName(event_property, region, MCRegionIndex), value);
        }
      }
      else {
        auto& criterion = marginallyUnselectedEventPropertiesPair.first;
        auto& propertiesMap = marginallyUnselectedEventPropertiesPair.second;
        for (auto&& propertiesMapElement: propertiesMap) {
          auto& event_property = propertiesMapElement.first;
          auto& value = propertiesMapElement.second;
          fillStatisticsHistogramByName(getStatisticsHistogramName(event_property, criterion, MCRegionIndex), value);
        }
      }

      for (auto&& selectedTruePhotonPropertiesMap: selectedTruePhotonProperties) {
        for (auto&& element: selectedTruePhotonPropertiesMap) {
          auto& property = element.first;
          auto& value = element.second;
          fillStatisticsHistogramByName(getStatisticsHistogramName(property, region, MCRegionIndex), value);
        }
      }

      for (auto&& selectedTrueJetCandidatePropertiesMap: selectedTrueJetCandidateProperties_all) {
        for (auto&& element: selectedTrueJetCandidatePropertiesMap) {
          auto& property = element.first;
          auto& value = element.second;
          fillStatisticsHistogramByName(getStatisticsHistogramName(property, region, MCRegionIndex), value);
        }
      }

      for (auto&& selectedTrueJetCandidatePropertiesMap: selectedTrueJetCandidateProperties_fromGluino) {
        for (auto&& element: selectedTrueJetCandidatePropertiesMap) {
          auto& property = element.first;
          auto& value = element.second;
          fillStatisticsHistogramByName(getStatisticsHistogramName(property, region, true, MCRegionIndex), value);
        }
      }

      for (auto&& selectedTrueJetCandidatePropertiesMap: selectedTrueJetCandidateProperties_fromSinglet) {
        for (auto&& element: selectedTrueJetCandidatePropertiesMap) {
          auto& property = element.first;
          auto& value = element.second;
          fillStatisticsHistogramByName(getStatisticsHistogramName(property, region, false, MCRegionIndex), value);
        }
      }

      for (auto&& selectedMediumPhotonProperties_closeToTruePhotonMap: selectedMediumPhotonProperties_closeToTruePhoton) {
        for (auto&& element: selectedMediumPhotonProperties_closeToTruePhotonMap) {
          auto& property = element.first;
          auto& value = element.second;
          fillStatisticsHistogramByName(getStatisticsHistogramName(property, region, true, true, MCRegionIndex), value);
        }
      }

      for (auto&& selectedMediumPhotonProperties_awayFromTruePhotonMap: selectedMediumPhotonProperties_awayFromTruePhoton) {
        for (auto&& element: selectedMediumPhotonProperties_awayFromTruePhotonMap) {
          auto& property = element.first;
          auto& value = element.second;
          fillStatisticsHistogramByName(getStatisticsHistogramName(property, region, true, true, MCRegionIndex), value);
        }
      }

      for (auto&& marginallyUnselectedMediumPhotonProperties_closeToTruePhotonPair: marginallyUnselectedMediumPhotonProperties_closeToTruePhoton) {
        auto& criterion = marginallyUnselectedMediumPhotonProperties_closeToTruePhotonPair.first;
        auto& propertiesMap = marginallyUnselectedMediumPhotonProperties_closeToTruePhotonPair.second;
        for (auto&& propertiesMapElement: propertiesMap) {
          auto& property = propertiesMapElement.first;
          auto& value = propertiesMapElement.second;
          fillStatisticsHistogramByName(getStatisticsHistogramName(property, region, criterion, true, MCRegionIndex), value);
        }
      }

      for (auto&& marginallyUnselectedMediumPhotonProperties_awayFromTruePhotonPair: marginallyUnselectedMediumPhotonProperties_awayFromTruePhoton) {
        auto& criterion = marginallyUnselectedMediumPhotonProperties_awayFromTruePhotonPair.first;
        auto& propertiesMap = marginallyUnselectedMediumPhotonProperties_awayFromTruePhotonPair.second;
        for (auto&& propertiesMapElement: propertiesMap) {
          auto& property = propertiesMapElement.first;
          auto& value = propertiesMapElement.second;
          fillStatisticsHistogramByName(getStatisticsHistogramName(property, region, criterion, false, MCRegionIndex), value);
        }
      }

      for (auto&& selectedFakePhotonProperties_closeToTruePhotonMap: selectedFakePhotonProperties_closeToTruePhoton) {
        for (auto&& element: selectedFakePhotonProperties_closeToTruePhotonMap) {
          auto& property = element.first;
          auto& value = element.second;
          fillStatisticsHistogramByName(getStatisticsHistogramName(property, region, false, true, MCRegionIndex), value);
        }
      }

      for (auto&& selectedFakePhotonProperties_awayFromTruePhotonMap: selectedFakePhotonProperties_awayFromTruePhoton) {
        for (auto&& element: selectedFakePhotonProperties_awayFromTruePhotonMap) {
          auto& property = element.first;
          auto& value = element.second;
          fillStatisticsHistogramByName(getStatisticsHistogramName(property, region, false, false, MCRegionIndex), value);  
        }
      }

      for (auto&& marginallyUnselectedFakePhotonProperties_closeToTruePhotonPair: marginallyUnselectedFakePhotonProperties_closeToTruePhoton) {
        auto& criterion = marginallyUnselectedFakePhotonProperties_closeToTruePhotonPair.first;
        auto& propertiesMap = marginallyUnselectedFakePhotonProperties_closeToTruePhotonPair.second;
        for (auto&& propertiesMapElement: propertiesMap) {
          auto& property = propertiesMapElement.first;
          auto& value = propertiesMapElement.second;
          fillStatisticsHistogramByName(getStatisticsHistogramName(property, region, criterion, true, MCRegionIndex), value);
        }
      }

      for (auto&& marginallyUnselectedFakePhotonProperties_awayFromTruePhotonPair: marginallyUnselectedFakePhotonProperties_awayFromTruePhoton) {
        auto& criterion = marginallyUnselectedFakePhotonProperties_awayFromTruePhotonPair.first;
        auto& propertiesMap = marginallyUnselectedFakePhotonProperties_awayFromTruePhotonPair.second;
        for (auto&& propertiesMapElement: propertiesMap) {
          auto& property = propertiesMapElement.first;
          auto& value = propertiesMapElement.second;
          fillStatisticsHistogramByName(getStatisticsHistogramName(property, region, criterion, false, MCRegionIndex), value);
        }
      }

      for (auto&& selectedJetProperties_closeToTruePhotonMap: selectedJetProperties_closeToTruePhoton) {
        for (auto&& element: selectedJetProperties_closeToTruePhotonMap) {
          auto& property = element.first;
          auto& value = element.second;
          fillStatisticsHistogramByName(getStatisticsHistogramName(property, region, true, MCRegionIndex), value);
        }
      }

      for (auto&& selectedJetProperties_awayFromTruePhotonMap: selectedJetProperties_awayFromTruePhoton) {
        for (auto&& element: selectedJetProperties_awayFromTruePhotonMap) {
          auto& property = element.first;
          auto& value = element.second;
          fillStatisticsHistogramByName(getStatisticsHistogramName(property, region, false, MCRegionIndex), value);
        }
      }

      for (auto&& marginallyUnselectedJetProperties_closeToTruePhotonPair: marginallyUnselectedJetProperties_closeToTruePhoton) {
        auto& criterion = marginallyUnselectedJetProperties_closeToTruePhotonPair.first;
        auto& propertiesMap = marginallyUnselectedJetProperties_closeToTruePhotonPair.second;
        for (auto&& element: propertiesMap) {
          auto& property = element.first;
          auto& value = element.second;
          fillStatisticsHistogramByName(getStatisticsHistogramName(property, region, criterion, true, MCRegionIndex), value);
        }
      }

      for (auto&& marginallyUnselectedJetProperties_awayFromTruePhotonPair: marginallyUnselectedJetProperties_awayFromTruePhoton) {
        auto& criterion = marginallyUnselectedJetProperties_awayFromTruePhotonPair.first;
        auto& propertiesMap = marginallyUnselectedJetProperties_awayFromTruePhotonPair.second;
        for (auto&& element: propertiesMap) {
          auto& property = element.first;
          auto& value = element.second;
          fillStatisticsHistogramByName(getStatisticsHistogramName(property, region, criterion, false, MCRegionIndex), value);
        }
      }

      for (auto& gen_jet_properties_map: gen_jet_properties_collection) {
        for (auto&& element: gen_jet_properties_map) {
          auto& property = element.first;
          auto& value = element.second;
          fillStatisticsHistogramByName(getStatisticsHistogramName(property, region, MCRegionIndex), value);
        }
      }

      for (auto& gluino_mom_gen_jet_properties_map: gluino_mom_gen_jet_properties_collection) {
        for (auto&& element: gluino_mom_gen_jet_properties_map) {
          auto& property = element.first;
          auto& value = element.second;
          fillStatisticsHistogramByName(getStatisticsHistogramName(property, true, region, MCRegionIndex), value);
        }
      }

      for (auto& singlet_mom_gen_jet_properties_map: singlet_mom_gen_jet_properties_collection) {
        for (auto&& element: singlet_mom_gen_jet_properties_map) {
          auto& property = element.first;
          auto& value = element.second;
          fillStatisticsHistogramByName(getStatisticsHistogramName(property, false, region, MCRegionIndex), value);
        }
      }
    }
  }

  void fillHLTEmulationStatisticsHistograms(const float& eta_leadingPhoton,
                                            const float& pT_leadingPhoton,
                                            const float& eta_subLeadingPhoton,
                                            const float& pT_subLeadingPhoton,
                                            const bool& passesHLTEmulation,
                                            const selectionRegion& region) {
    // HLT emulation statistics
    fillHLTEfficiencyByName(std::string("hltEfficiency_leadingPhoton_" + selectionRegionNames.at(region)), passesHLTEmulation, eta_leadingPhoton, pT_leadingPhoton);
    fillHLTEfficiencyByName(std::string("hltEfficiency_subLeadingPhoton_" + selectionRegionNames.at(region)), passesHLTEmulation, eta_subLeadingPhoton, pT_subLeadingPhoton);
  }

  void fillIDEfficiencyStatisticsHistograms(const float& eventST, const selectionRegion& eventRegion) {
    int eventRegionInt = static_cast<int>(eventRegion);
    for (int regionIndex = selectionRegionFirst; regionIndex < static_cast<int>(selectionRegion::nSelectionRegions); ++regionIndex) {
      selectionRegion region = static_cast<selectionRegion>(regionIndex);
      fillIDEfficiencyByName(std::string("IDEfficiency_" + selectionRegionNames.at(region)), (regionIndex == eventRegionInt), eventST);
    }
  }

  void writeToFile(const std::string& outputFileRelativePath) {
    TFile *outputFile = TFile::Open(outputFileRelativePath.c_str(), "RECREATE");
    if (!(outputFile->IsOpen()) || outputFile->IsZombie()) {
      std::cout << "ERROR: Unable to open output file to write. File path: " << outputFileRelativePath << std::endl;
    }
    for (auto&& statsElement: stats) {
      outputFile->WriteTObject(statsElement.second);
    }
    for (auto&& statsElement: stats_HLTEmulation) {
      outputFile->WriteTObject(statsElement.second);
    }
    for (auto&& IDElement: stats_IDEfficiency) {
      outputFile->WriteTObject(IDElement.second);
    }
    outputFile->Close();
  }
};

#endif
