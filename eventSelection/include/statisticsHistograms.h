#ifndef H_STATISTICSHISTOGRAMS
#define H_STATISTICSHISTOGRAMS

#include <iostream>
#include <cstdlib>
#include <map>

#include "TFile.h"
#include "TH1.h"
#include "TH1F.h"

#include "objectProperties.h"
#include "selectionCriteria.h"

class statisticsHistograms {
 public:
  std::map<std::string, TH1F> stats;
  std::map<int, std::string> MCBinNames;

  std::string getStatisticsHistogramName(const eventProperty& event_property, const selectionRegion& region) {
    return ((eventPropertyAttributes[event_property]).name + "_" + selectionRegionNames[region] + "_selectedEvents");
  }

  std::string getStatisticsHistogramName(const eventProperty& event_property, const eventSelectionCriterion& criterion) {
    return ((eventPropertyAttributes[event_property]).name + "_marginallyUnselectedEvents_" + eventSelectionCriterionNames[criterion]);
  }
  
  std::string getStatisticsHistogramName(const truthPhotonProperty& truth_photon_property, const selectionRegion& region, const int& MCBinIndex) {
    return ((truthPhotonPropertyAttributes[truth_photon_property]).name + "_" + selectionRegionNames[region] + "_truePhotons_MC_" + MCBinNames[MCBinIndex]);
  }

  std::string getStatisticsHistogramName(const truthJetCandidateProperty& truth_jetCandidate_property, const selectionRegion& region, const int& MCBinIndex) {
    return ((truthJetCandidatePropertyAttributes[truth_jetCandidate_property]).name + "_" + selectionRegionNames[region] + "_trueJetCandidates_MC_" + MCBinNames[MCBinIndex]);
  }
  
  std::string getStatisticsHistogramName(const photonProperty& photon_property, const selectionRegion& region, const bool& trueMedium_falseFake) {
    std::string name = ((photonPropertyAttributes[photon_property]).name + "_" + selectionRegionNames[region] + "_selected");
    if (trueMedium_falseFake) name += "MediumCaloPhotons";
    else name += "FakeCaloPhotons";
    return name;
  }

  std::string getStatisticsHistogramName(const photonProperty& photon_property, const selectionRegion& region, const bool& trueMedium_falseFake, const bool& trueClose_falseAway, const int& MCBinIndex) {
    std::string name = ((photonPropertyAttributes[photon_property]).name + "_" + selectionRegionNames[region] + "_selected");
    if (trueMedium_falseFake) name += "MediumCaloPhotons_";
    else name += "FakeCaloPhotons_";
    if (trueClose_falseAway) name += "closeToTruePhoton_MC_";
    else name += "awayFromTruePhotons_MC_";
    name += MCBinNames[MCBinIndex];
    return name;
  }

  std::string getStatisticsHistogramName(const photonProperty& photon_property, const selectionRegion& region, const mediumPhotonCriterion& medium_photon_criterion) {
    return ((photonPropertyAttributes[photon_property]).name + "_" + selectionRegionNames[region] + "_marginallyUnselectedMediumCaloPhotons_" + mediumPhotonCriterionNames[medium_photon_criterion]);
  }

  std::string getStatisticsHistogramName(const photonProperty& photon_property, const selectionRegion& region, const mediumPhotonCriterion& medium_photon_criterion, const bool& trueClose_falseAway, const int& MCBinIndex) {
    std::string name = (photonPropertyAttributes[photon_property]).name + "_" + selectionRegionNames[region] + "_marginallyUnselectedMediumCaloPhotons_" + mediumPhotonCriterionNames[medium_photon_criterion] + "_";
    if (trueClose_falseAway) name += "closeToTruePhoton_MC_";
    else name += "awayFromTruePhotons_MC_";
    name += MCBinNames[MCBinIndex];
    return name;
  }

  std::string getStatisticsHistogramName(const photonProperty& photon_property, const selectionRegion& region, const fakePhotonCriterion& fake_photon_criterion) {
    return ((photonPropertyAttributes[photon_property]).name + "_" + selectionRegionNames[region] + "_marginallyUnselectedFakeCaloPhotons_" + fakePhotonCriterionNames[fake_photon_criterion]);
  }

  std::string getStatisticsHistogramName(const photonProperty& photon_property, const selectionRegion& region, const fakePhotonCriterion& fake_photon_criterion, const bool& trueClose_falseAway, const int& MCBinIndex) {
    std::string name = (photonPropertyAttributes[photon_property]).name + "_" + selectionRegionNames[region] + "_marginallyUnselectedFakeCaloPhotons_" + fakePhotonCriterionNames[fake_photon_criterion] + "_";
    if (trueClose_falseAway) name += "closeToTruePhoton_MC_";
    else name += "awayFromTruePhotons_MC_";
    name += MCBinNames[MCBinIndex];
    return name;
  }

  std::string getStatisticsHistogramName(const jetProperty& jet_property, const selectionRegion& region) {
    return ((jetPropertyAttributes[jet_property]).name + "_" + selectionRegionNames[region] + "_selectedCaloJets");
  }

  std::string getStatisticsHistogramName(const jetProperty& jet_property, const selectionRegion& region, const bool& trueClose_falseAway, const int& MCBinIndex) {
    std::string name = (jetPropertyAttributes[jet_property]).name + "_" + selectionRegionNames[region] + "_selectedCaloJets_";
    if (trueClose_falseAway) name += "closeToTruePhoton_MC_";
    else name += "awayFromTruePhotons_MC_";
    name += MCBinNames[MCBinIndex];
    return name;
  }

  std::string getStatisticsHistogramName(const jetProperty& jet_property, const selectionRegion& region, const jetCriterion& criterion) {
    return ((jetPropertyAttributes[jet_property]).name + "_" + selectionRegionNames[region] + "_marginallyUnselectedCaloJets_" + jetCriterionNames[criterion]);
  }

  std::string getStatisticsHistogramName(const jetProperty& jet_property, const selectionRegion& region, const jetCriterion& criterion, const bool& trueClose_falseAway, const int& MCBinIndex) {
    std::string name = (jetPropertyAttributes[jet_property]).name + "_" + selectionRegionNames[region] + "_marginallyUnselectedCaloJets_" + jetCriterionNames[criterion] + "_";
    if (trueClose_falseAway) name += "closeToTruePhoton_MC_";
    else name += "awayFromTruePhotons_MC_";
    name += MCBinNames[MCBinIndex];
    return name;
  }

  void initializeWithCheck(std::string& name, int& nBins, float& xmin, float& xmax) {
    if (stats.find(name) == stats.end()) {
      stats.insert(std::make_pair(name, TH1F(name.c_str(), name.c_str(), nBins, xmin, xmax)));
      // stats[name].SetCanExtend(TH1::kAllAxes);
    }
    else {
      std::cout << "ERROR: tried to create new statistics histogram with name \"" << name << "\", but it already exists!" << std::endl;
      std::exit(EXIT_FAILURE);
    }
  }

  statisticsHistograms(const bool& isMC, std::map<int, std::string>& MCBinNames_) {
    for (auto&& MCBinNamesElement: MCBinNames_) {
      MCBinNames.insert(std::make_pair(MCBinNamesElement.first, MCBinNamesElement.second));
    }
    std::string fullName;
    for (auto&& eventPropertyAttributesElement: eventPropertyAttributes) {
      auto& event_property = (eventPropertyAttributesElement.first);
      auto& attributesElement = (eventPropertyAttributesElement.second);
      for (auto&& selectionRegionNamesElement: selectionRegionNames) {
        auto& region = selectionRegionNamesElement.first;
        fullName = getStatisticsHistogramName(event_property, region);
        initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
      }
      for (auto&& eventSelectionCriterionNamesElement: eventSelectionCriterionNames) {
        auto& event_selection_criterion = eventSelectionCriterionNamesElement.first;
        fullName = getStatisticsHistogramName(event_property, event_selection_criterion);
        initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
      }
    }

    if (isMC) {
      for (auto&& truthPhotonPropertyAttributesElement: truthPhotonPropertyAttributes) {
        auto& truth_photon_property = (truthPhotonPropertyAttributesElement.first);
        auto& attributesElement = (truthPhotonPropertyAttributesElement.second);
        for (auto&& selectionRegionNamesElement: selectionRegionNames) {
          auto& region = selectionRegionNamesElement.first;
          for (auto&& MCBinNamesElement: MCBinNames) {
            auto& MCBinIndex = MCBinNamesElement.first;
            fullName = getStatisticsHistogramName(truth_photon_property, region, MCBinIndex);
            initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
          }
        }
      }
      for (auto&& jetCandidatePropertyAttributesElement: truthJetCandidatePropertyAttributes) {
        auto& truth_jetCandidate_property = (jetCandidatePropertyAttributesElement.first);
        auto& attributesElement = (jetCandidatePropertyAttributesElement.second);
        for (auto&& selectionRegionNamesElement: selectionRegionNames) {
          auto& region = selectionRegionNamesElement.first;
          for (auto&& MCBinNamesElement: MCBinNames) {
            auto& MCBinIndex = MCBinNamesElement.first;
            fullName = getStatisticsHistogramName(truth_jetCandidate_property, region, MCBinIndex);
            initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
          }
        }
      }
    }

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
          for (auto&& MCBinNamesElement: MCBinNames) {
            auto& MCBinIndex = MCBinNamesElement.first;
            fullName = getStatisticsHistogramName(photon_property, region, true, true, MCBinIndex);
            initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
            fullName = getStatisticsHistogramName(photon_property, region, true, false, MCBinIndex);
            initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
            fullName = getStatisticsHistogramName(photon_property, region, false, true, MCBinIndex);
            initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
            fullName = getStatisticsHistogramName(photon_property, region, false, false, MCBinIndex);
            initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
          }
        } // MC plots
        for (auto&& mediumPhotonCriterionNamesElement: mediumPhotonCriterionNames) {
          auto& medium_photon_criterion = mediumPhotonCriterionNamesElement.first;
          fullName = getStatisticsHistogramName(photon_property, region, medium_photon_criterion);
          initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
          if (isMC) {
            for (auto&& MCBinNamesElement: MCBinNames) {
              auto& MCBinIndex = MCBinNamesElement.first;
              fullName = getStatisticsHistogramName(photon_property, region, medium_photon_criterion, true, MCBinIndex);
              initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
              fullName = getStatisticsHistogramName(photon_property, region, medium_photon_criterion, false, MCBinIndex);
              initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
            }
          } // MC plots
        } // medium photon criteria
        for (auto&& fakePhotonCriterionNamesElement: fakePhotonCriterionNames) {
          auto& fake_photon_criterion = fakePhotonCriterionNamesElement.first;
          fullName = getStatisticsHistogramName(photon_property, region, fake_photon_criterion);
          initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
          if (isMC) {
            for (auto&& MCBinNamesElement: MCBinNames) {
              auto& MCBinIndex = MCBinNamesElement.first;
              fullName = getStatisticsHistogramName(photon_property, region, fake_photon_criterion, true, MCBinIndex);
              initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
              fullName = getStatisticsHistogramName(photon_property, region, fake_photon_criterion, false, MCBinIndex);
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
          for (auto&& MCBinNamesElement: MCBinNames) {
            auto& MCBinIndex = MCBinNamesElement.first;
            fullName = getStatisticsHistogramName(jet_property, region, true, MCBinIndex);
            initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
            fullName = getStatisticsHistogramName(jet_property, region, false, MCBinIndex);
            initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
          }
        } // MC plots
        for (auto&& jetCriterionNamesElement: jetCriterionNames) {
          auto& jet_criterion = jetCriterionNamesElement.first;
          fullName = getStatisticsHistogramName(jet_property, region, jet_criterion);
          initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
          if (isMC) {
            for (auto&& MCBinNamesElement: MCBinNames) {
              auto& MCBinIndex = MCBinNamesElement.first;
              fullName = getStatisticsHistogramName(jet_property, region, jet_criterion, true, MCBinIndex);
              initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
              fullName = getStatisticsHistogramName(jet_property, region, jet_criterion, false, MCBinIndex);
              initializeWithCheck(fullName, attributesElement.plot_nBins, attributesElement.plot_minRange, attributesElement.plot_maxRange);
            }
          }
        }
      } // jet plots
    }
  }

  void fillStatisticsHistogramByName(const std::string& histogramName, const float& value, const float& weight) {
    if (stats.find(histogramName) == stats.end()) {
      std::cout << "ERROR: tried to fill statistics histogram with name \"" << histogramName << "\"; a histogram with this name was not initialized!" << std::endl;
      std::exit(EXIT_FAILURE);
    }
    else {
      (stats[histogramName]).Fill(value, weight);
    }
  }

  void fillStatisticsHistogramByName(const std::string& histogramName, const float& value) {
    fillStatisticsHistogramByName(histogramName, value, 1.0);
  }

  void fillStatisticsHistograms(eventProperties& selectedEventPropertiesMap,
                                const bool& isMarginallyUnselectedEvent,
                                unselectedEventProperties& marginallyUnselectedEventPropertiesPair,
                                truthPhotonPropertiesCollection& selectedTruePhotonProperties,
                                truthJetCandidatePropertiesCollection& selectedTrueJetCandidateProperties,
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
                                selectionRegion& region,
                                const bool& isMC,
                                const int& MCBinIndex) {
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

    if (isMC && (MCBinIndex > 0)) {
      for (auto&& selectedTruePhotonPropertiesMap: selectedTruePhotonProperties) {
        for (auto&& element: selectedTruePhotonPropertiesMap) {
          auto& property = element.first;
          auto& value = element.second;
          fillStatisticsHistogramByName(getStatisticsHistogramName(property, region, MCBinIndex), value);
        }
      }

      for (auto&& selectedTrueJetCandidatePropertiesMap: selectedTrueJetCandidateProperties) {
        for (auto&& element: selectedTrueJetCandidatePropertiesMap) {
          auto& property = element.first;
          auto& value = element.second;
          fillStatisticsHistogramByName(getStatisticsHistogramName(property, region, MCBinIndex), value);
        }
      }

      for (auto&& selectedMediumPhotonProperties_closeToTruePhotonMap: selectedMediumPhotonProperties_closeToTruePhoton) {
        for (auto&& element: selectedMediumPhotonProperties_closeToTruePhotonMap) {
          auto& property = element.first;
          auto& value = element.second;
          fillStatisticsHistogramByName(getStatisticsHistogramName(property, region, true, true, MCBinIndex), value);
        }
      }

      for (auto&& selectedMediumPhotonProperties_awayFromTruePhotonMap: selectedMediumPhotonProperties_awayFromTruePhoton) {
        for (auto&& element: selectedMediumPhotonProperties_awayFromTruePhotonMap) {
          auto& property = element.first;
          auto& value = element.second;
          fillStatisticsHistogramByName(getStatisticsHistogramName(property, region, true, true, MCBinIndex), value);
        }
      }

      for (auto&& marginallyUnselectedMediumPhotonProperties_closeToTruePhotonPair: marginallyUnselectedMediumPhotonProperties_closeToTruePhoton) {
        auto& criterion = marginallyUnselectedMediumPhotonProperties_closeToTruePhotonPair.first;
        auto& propertiesMap = marginallyUnselectedMediumPhotonProperties_closeToTruePhotonPair.second;
        for (auto&& propertiesMapElement: propertiesMap) {
          auto& property = propertiesMapElement.first;
          auto& value = propertiesMapElement.second;
          fillStatisticsHistogramByName(getStatisticsHistogramName(property, region, criterion, true, MCBinIndex), value);
        }
      }

      for (auto&& marginallyUnselectedMediumPhotonProperties_awayFromTruePhotonPair: marginallyUnselectedMediumPhotonProperties_awayFromTruePhoton) {
        auto& criterion = marginallyUnselectedMediumPhotonProperties_awayFromTruePhotonPair.first;
        auto& propertiesMap = marginallyUnselectedMediumPhotonProperties_awayFromTruePhotonPair.second;
        for (auto&& propertiesMapElement: propertiesMap) {
          auto& property = propertiesMapElement.first;
          auto& value = propertiesMapElement.second;
          fillStatisticsHistogramByName(getStatisticsHistogramName(property, region, criterion, false, MCBinIndex), value);
        }
      }

      for (auto&& selectedFakePhotonProperties_closeToTruePhotonMap: selectedFakePhotonProperties_closeToTruePhoton) {
        for (auto&& element: selectedFakePhotonProperties_closeToTruePhotonMap) {
          auto& property = element.first;
          auto& value = element.second;
          fillStatisticsHistogramByName(getStatisticsHistogramName(property, region, false, true, MCBinIndex), value);
        }
      }

      for (auto&& selectedFakePhotonProperties_awayFromTruePhotonMap: selectedFakePhotonProperties_awayFromTruePhoton) {
        for (auto&& element: selectedFakePhotonProperties_awayFromTruePhotonMap) {
          auto& property = element.first;
          auto& value = element.second;
          fillStatisticsHistogramByName(getStatisticsHistogramName(property, region, false, false, MCBinIndex), value);  
        }
      }

      for (auto&& marginallyUnselectedFakePhotonProperties_closeToTruePhotonPair: marginallyUnselectedFakePhotonProperties_closeToTruePhoton) {
        auto& criterion = marginallyUnselectedFakePhotonProperties_closeToTruePhotonPair.first;
        auto& propertiesMap = marginallyUnselectedFakePhotonProperties_closeToTruePhotonPair.second;
        for (auto&& propertiesMapElement: propertiesMap) {
          auto& property = propertiesMapElement.first;
          auto& value = propertiesMapElement.second;
          fillStatisticsHistogramByName(getStatisticsHistogramName(property, region, criterion, true, MCBinIndex), value);
        }
      }

      for (auto&& marginallyUnselectedFakePhotonProperties_awayFromTruePhotonPair: marginallyUnselectedFakePhotonProperties_awayFromTruePhoton) {
        auto& criterion = marginallyUnselectedFakePhotonProperties_awayFromTruePhotonPair.first;
        auto& propertiesMap = marginallyUnselectedFakePhotonProperties_awayFromTruePhotonPair.second;
        for (auto&& propertiesMapElement: propertiesMap) {
          auto& property = propertiesMapElement.first;
          auto& value = propertiesMapElement.second;
          fillStatisticsHistogramByName(getStatisticsHistogramName(property, region, criterion, false, MCBinIndex), value);
        }
      }

      for (auto&& selectedJetProperties_closeToTruePhotonMap: selectedJetProperties_closeToTruePhoton) {
        for (auto&& element: selectedJetProperties_closeToTruePhotonMap) {
          auto& property = element.first;
          auto& value = element.second;
          fillStatisticsHistogramByName(getStatisticsHistogramName(property, region, true, MCBinIndex), value);
        }
      }

      for (auto&& selectedJetProperties_awayFromTruePhotonMap: selectedJetProperties_awayFromTruePhoton) {
        for (auto&& element: selectedJetProperties_awayFromTruePhotonMap) {
          auto& property = element.first;
          auto& value = element.second;
          fillStatisticsHistogramByName(getStatisticsHistogramName(property, region, false, MCBinIndex), value);
        }
      }

      for (auto&& marginallyUnselectedJetProperties_closeToTruePhotonPair: marginallyUnselectedJetProperties_closeToTruePhoton) {
        auto& criterion = marginallyUnselectedJetProperties_closeToTruePhotonPair.first;
        auto& propertiesMap = marginallyUnselectedJetProperties_closeToTruePhotonPair.second;
        for (auto&& element: propertiesMap) {
          auto& property = element.first;
          auto& value = element.second;
          fillStatisticsHistogramByName(getStatisticsHistogramName(property, region, criterion, true, MCBinIndex), value);
        }
      }

      for (auto&& marginallyUnselectedJetProperties_awayFromTruePhotonPair: marginallyUnselectedJetProperties_awayFromTruePhoton) {
        auto& criterion = marginallyUnselectedJetProperties_awayFromTruePhotonPair.first;
        auto& propertiesMap = marginallyUnselectedJetProperties_awayFromTruePhotonPair.second;
        for (auto&& element: propertiesMap) {
          auto& property = element.first;
          auto& value = element.second;
          fillStatisticsHistogramByName(getStatisticsHistogramName(property, region, criterion, false, MCBinIndex), value);
        }
      }
    }
  }

  void writeToFile(const std::string& outputFileRelativePath) {
    TFile *outputFile = TFile::Open(outputFileRelativePath.c_str(), "RECREATE");
    if (!(outputFile->IsOpen()) || outputFile->IsZombie()) {
      std::cout << "ERROR: Unable to open output file to write. File path: " << outputFileRelativePath << std::endl;
    }
    for (auto&& statsElement: stats) {
      outputFile->WriteTObject(&(statsElement.second));
    }
    outputFile->Close();
  }
};

#endif