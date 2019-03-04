#!/bin/bash

cd /uscms/home/tmudholk/private/stealth/STEALTH && source setupEnv.sh && set -x

if [ -f analysis_running.lock ]; then
    echo "Found lock file -- only one script can run at a time..."
    set +x
    exit
fi

touch analysis_running.lock

function removeLockAndExit(){
    rm -f analysis_running.lock
    set +x
    exit
}

function checkMove(){
    if [ "$#" -ne 2 ]; then
        echo "ERROR: checkMove needs two arguments. Passed aguments: $@"
        removeLockAndExit
    elif [ "${1}" != "${2}" ]; then
        if [ -d "${2}" ]; then
            echo "ERROR: called \"checkMove ${1} ${2}\", but ${2} already exists!"
            removeLockAndExit
        elif [ -d "${1}" ]; then
            mv "${1}/" "${2}/"
        fi
    fi
}

COMMON_XROOT_PREFIX="root://cmseos.fnal.gov/"
INPUTDATADIR_CONTROL="${COMMON_XROOT_PREFIX}/store/user/lpcsusystealth/selections/combinedControl"
INPUTDATADIR_SIGNAL="${COMMON_XROOT_PREFIX}/store/user/lpcsusystealth/selections/combinedSignal"
INPUTDATADIR_MCSIGNAL="${COMMON_XROOT_PREFIX}/store/user/lpcsusystealth/selections/combinedSignal"

# L_total = L_2016 + L_2017 => deltaL_total/L_total = (deltaL_2016/L_2016)*(L_2016/L_total) + (deltaL_2017/L_2017)*(L_2017/L_total)
# From http://cms-results.web.cern.ch/cms-results/public-results/preliminary-results/LUM-17-004/index.html, the 2017 uncertainty is 2.3 percent
# From http://cms-results.web.cern.ch/cms-results/public-results/preliminary-results/LUM-17-001/index.html, the 2016 uncertainty is 2.5 percent
MCPATTERNSIGNAL_MAIN="MC_2018Production_DoubleMedium_optimized2017.root"
INTEGLUMI_MAIN="41900.0"
MCPATTERNSIGNAL_AUX="MC_2018Production_DoubleMedium_optimized2016.root"
INTEGLUMI_AUX="35920.0"
INTEGLUMI_FRACTIONALERROR="0.024"
INPUTFILESLIST_CONTROL="${INPUTDATADIR_CONTROL}/data_DoubleEG_201*_DoubleFake.root;${INPUTDATADIR_CONTROL}/data_DoubleEG_201*_OneMediumOneFake.root"
INPUTFILESLIST_SIGNAL="${INPUTDATADIR_SIGNAL}/data_DoubleEG_201*_DoubleMedium.root"
OUTPUTPATH_EOS_DATACARDS="/store/user/lpcsusystealth/combineToolOutputs"
OPTIONAL_IDENTIFIER=""
OPTIONAL_IDENTIFIER_WITHOUT_UNDERSCORE=""
USE_STANDARD_MC_SELECTION="true"
SPECIFIC_STEP_INDEX=""
STEPS_TO_RUN=(1 2 3 4 5 6)

for argValuePair in "$@"; do
    argName=`echo ${argValuePair} | cut --delimiter="=" --fields=1`
    argValue=`echo ${argValuePair} | cut --delimiter="=" --fields=2`
    EQUALSIGNTEST=`echo ${argValuePair} | grep "="`
    if [ "${EQUALSIGNTEST}" = "" ]; then
        echo "Argument not in format \"arg=val\": ${argValuePair}"
        removeLockAndExit
    elif [ "${argName}" = "" -o "${argValue}" = "" ]; then
        echo "Malformed argument: \"${argValuePair}\"; should be in the format arg=val"
        removeLockAndExit
    elif [ "${argName}" = "identifier" ]; then
        OPTIONAL_IDENTIFIER="_${argValue}"
        OPTIONAL_IDENTIFIER_WITHOUT_UNDERSCORE="${argValue}"
    elif [ "${argName}" = "useStandardMCSelection" ]; then
        if [ "${argValue}" = "false" ]; then
            USE_STANDARD_MC_SELECTION="false"
        else
            echo "Invalid value for \"useStandardMCSelection\": ${argValue}"
        fi
    elif [ "${argName}" = "useSingleMediumControl" ]; then
        if [ "${argValue}" = "true" ]; then
            INPUTFILESLIST_CONTROL="${INPUTDATADIR_CONTROL}/data_DoubleEG_201*_SingleMedium.root"
        else
            echo "Invalid value for \"useSingleMediumControl\": ${argValue}"
        fi
    elif [ "${argName}" = "specificStep" ]; then
        SPECIFIC_STEP_INDEX="${argValue}"
    elif [ "${argName}" = "chain" ]; then
        if [ "${argValue}" = "data" ]; then
            STEPS_TO_RUN=(1 2)
        elif [ "${argValue}" = "MC" ]; then
            STEPS_TO_RUN=(3 4)
        elif [ "${argValue}" = "combine" ]; then
            STEPS_TO_RUN=(5 6)
        else
            echo "Unrecognized chain: \"${argValue}\""
            removeLockAndExit
        fi
    else
        echo "Unrecognized argument: \"${argName}\""
        removeLockAndExit
    fi
done

if [ "${OPTIONAL_IDENTIFIER}" != "" ]; then
    INPUTDATADIR_CONTROL="${COMMON_XROOT_PREFIX}/store/user/lpcsusystealth/selections/testsMerged/${OPTIONAL_IDENTIFIER_WITHOUT_UNDERSCORE}/combinedControl"
    INPUTDATADIR_SIGNAL="${COMMON_XROOT_PREFIX}/store/user/lpcsusystealth/selections/testsMerged/${OPTIONAL_IDENTIFIER_WITHOUT_UNDERSCORE}/combinedSignal"
    INPUTFILESLIST_CONTROL="${INPUTDATADIR_CONTROL}/data_DoubleEG_201*_SingleMedium${OPTIONAL_IDENTIFIER}.root"
    INPUTFILESLIST_SIGNAL="${INPUTDATADIR_SIGNAL}/data_DoubleEG_201*_DoubleMedium${OPTIONAL_IDENTIFIER}.root"

    if [ ${USE_STANDARD_MC_SELECTION} = "false" ]; then
        INPUTDATADIR_MCSIGNAL="${COMMON_XROOT_PREFIX}/store/user/lpcsusystealth/selections/testsMerged/${OPTIONAL_IDENTIFIER_WITHOUT_UNDERSCORE}/combinedSignal"
        MCPATTERNSIGNAL="MC_2018Production_DoubleMedium${OPTIONAL_IDENTIFIER}.root"
    fi
fi

function runStep(){
    case ${1} in
        1)
            ./getDataEventHistogramsAndSystematics.py --inputFilesList "${INPUTFILESLIST_CONTROL}" --outputPrefix control
            ;;
        2)
            ./getDataEventHistogramsAndSystematics.py --inputFilesList "${INPUTFILESLIST_SIGNAL}" --outputPrefix signal --isSignal
            ;;
        3)
            ./getMCSystematics/bin/getEventHistograms inputMCPathMain=${INPUTDATADIR_MCSIGNAL}/${MCPATTERNSIGNAL_MAIN} integratedLuminosityMain=${INTEGLUMI_MAIN} inputMCPathAux=${INPUTDATADIR_MCSIGNAL}/${MCPATTERNSIGNAL_AUX} integratedLuminosityAux=${INTEGLUMI_AUX} outputPrefix=MC_2018
            ;;
        4)
            ./getMCSystematics/bin/getMCUncertainties inputPath=analysis/MCEventHistograms/MC_2018_savedObjects.root outputPrefix=MC_2018
            ;;
        5)
            ./createDataCards.py --outputPrefix "fullChain" --inputFile_MCEventHistograms "analysis/MCEventHistograms/MC_2018_savedObjects.root" --inputFile_MCUncertainties "analysis/MCSystematics/MC_2018_MCUncertainties_savedObjects.root" --inputFile_dataSystematics "analysis/dataSystematics/signal_dataSystematics.dat" --inputFile_dataSystematics_sTScaling "analysis/dataSystematics/control_dataSystematics_sTScaling.dat" --inputFile_dataSystematics_expectedEventCounters "analysis/dataSystematics/signal_eventCounters.dat" --inputFile_dataSystematics_observedEventCounters "analysis/dataSystematics/signal_observedEventCounters.dat" --luminosityUncertainty ${INTEGLUMI_FRACTIONALERROR}
            ;;
        6)
            echo "Removing directory with datacards if it exists..."
            eos root://cmseos.fnal.gov ls ${OUTPUTPATH_EOS_DATACARDS} && eos root://cmseos.fnal.gov rm -r ${OUTPUTPATH_EOS_DATACARDS} && eos root://cmseos.fnal.gov mkdir -p ${OUTPUTPATH_EOS_DATACARDS}
            ./submitCombineToolJobs.py --dataCardsPrefix fullChain --minGluinoMass 975.0 --outputDirectory "${COMMON_XROOT_PREFIX}${OUTPUTPATH_EOS_DATACARDS}"
            ;;
        *)
            echo "Unrecognized or empty step index: ${1}"
            removeLockAndExit
            ;;
    esac
}

if [ "${OPTIONAL_IDENTIFIER}" != "" ]; then
    checkMove "analysis" "analysis_original"
    checkMove "analysis${OPTIONAL_IDENTIFIER}" "analysis"
fi
mkdir -p analysis/{MCEventHistograms,MCSystematics,dataCards,dataEventHistograms,dataSystematics,signalContamination}

if [ "${SPECIFIC_STEP_INDEX}" != "" ]; then
    runStep ${SPECIFIC_STEP_INDEX}
else
    for stepIndex in ${STEPS_TO_RUN[@]}; do
        runStep ${stepIndex}
        STEP_SUCCESSFUL="$?"
        if [ "${STEP_SUCCESSFUL}" -ne 0 ]; then
            echo "Failed step: ${stepIndex}"
            break
        fi
    done
fi

if [ "${OPTIONAL_IDENTIFIER}" != "" ]; then
    checkMove "analysis" "analysis${OPTIONAL_IDENTIFIER}"
    checkMove "analysis_original" "analysis"
fi
removeLockAndExit
