#!/bin/bash

################### USER VARIABLES #####################################

#indentical hosts should be grouped together for better killall
hostlist=(01 03 04 05 06 07)

#set unchanging parameters.
timestep=0.1
noTrials=8
noLights=6 # note phase 3 angle changes at noLights += 2.
lightDurationMin=40
lightDurationMax=60
lightDistanceMax=15
lightDistanceMin=10
fitnessDuration=25
networkType=CTRNN
agentRadius=1

seedWithCentreCrossing=1
seedWithScaledWeights=0
scaleOutputToPositive=1

sensorAngle1D=1

noGenerations=6000
initialMutationVariance=0.02
variableMutationVariance=1
variableMutationVarianceMin=0.005
variableMutationVarianceMax=0.02
variableMutationVarianceMinFitnessThreshold=0.4
variableMutationVarianceMaxFitnessThreshold=0.8

useDemes=0
demeWidth=10
randomReplacement=0
useLookup=1

displayResults=0
keepOldGenerationData=0

experiment1() {
setTanhTausToOne=0
noInterneurons=10
constrainWithScaledWeights=1

	outputPath="..//output//"
	# must not have leading number!
	dataFileName="constrained_oneSensor_${noInterneurons}inters_Data"
	recordFileName="constrained_oneSensor_${noInterneurons}inters_Record"
  runCreateSeeds
}

experiment2() {
setTanhTausToOne=0
noInterneurons=10
constrainWithScaledWeights=0

	outputPath="..//output//"
	# must not have leading number!
	dataFileName="unConstrained_oneSensor_${noInterneurons}inters_Data"
	recordFileName="unConstrained_oneSensor_${noInterneurons}inters_Record"
  runCreateSeeds
}

noSeeds=${#hostlist[*]}

####################### CREATE RANDOM SEED ARRAY #######################
runCreateSeeds() {
  noSeeds=$[$noSeeds - 1]
  for seedNo in `seq 0 $noSeeds`
  do
    seedlist[$seedNo]=$RANDOM
  done
}

###################### KILL ALL ########################################

runKillAll() {
  noHosts=${#hostlist[*]}
  lastHost=-1
  for hostNo in `seq 0 $[noHosts - 1]`
  do
    host=${hostlist[hostNo]}

    if [ $host != $lastHost ]; then
      ssh star$host.tn.informatics.scitech.susx.ac.uk "killall Evolve"
    fi

    lastHost=$host
  done
}

########################### PROGRESS COUNT ###########################

runProgress() {

  cd ../output
	
  for file in `ls *Record*`
  do
    wcResult=(`wc -l $file`)
    lineCount=$[${wcResult[0]} * 100 - 100]
    percentage=$[ $lineCount / $noGenerations ]

    if [ $percentage -lt 100 ]; then
      echo "$percentage% - $file"
    fi
    
  done
}

########################### RUN SIMULATIONS ###########################

runEvolve() {
  hostNo=0
  getNextHost() {
    host=${hostlist[hostNo]}
    hostNo=$[$hostNo + 1]
  }

  bindir=`pwd`
  experimentNo=1
  while experiment$experimentNo
  do
    experimentNo=$[$experimentNo + 1]

    noSeeds=${#seedlist[*]}
    for seedNo in `seq 0 $[$noSeeds - 1]`
    do
      getNextHost
      seed=${seedlist[seedNo]}

			echo Host: $host, Seed: $seed

      cmd="cd '$bindir'"
      cmd2="nohup time ./Evolve \
			outputPath $outputPath \
			dataFileName $dataFileName\_$seed.data \
			recordFileName $recordFileName\_$seed.m \
			keepOldGenerationData $keepOldGenerationData \
      popSize $popSize \
      seed $seed \
      timestep $timestep \
			noTrials $noTrials \
			noLights $noLights \
			agentRadius $agentRadius \
			lightDurationMax $lightDurationMax \
			lightDurationMin $lightDurationMin \
			lightDistanceMax $lightDistanceMax \
			lightDistanceMin $lightDistanceMin \
			fitnessDuration $fitnessDuration \
			initialMutationVariance $initialMutationVariance \
			variableMutationVariance $variableMutationVariance \
			variableMutationVarianceMin $variableMutationVarianceMin \
			variableMutationVarianceMax $variableMutationVarianceMax \
			variableMutationVarianceMinFitnessThreshold $variableMutationVarianceMinFitnessThreshold \
			variableMutationVarianceMaxFitnessThreshold $variableMutationVarianceMaxFitnessThreshold \
			randomReplacement $randomReplacement \
			networkType $networkType \
			noGenerations $noGenerations \
			useDemes $useDemes \
			demeWidth $demeWidth \
			noInterneurons $noInterneurons \
			displayResults $displayResults \
			seedWithCentreCrossing $seedWithCentreCrossing \
			seedWithScaledWeights $seedWithScaledWeights \
			constrainWithScaledWeights $constrainWithScaledWeights \
			setTanhTausToOne $setTanhTausToOne \
			scaleOutputToPositive $scaleOutputToPositive \
			sensorAngle1D $sensorAngle1D \
			useLookup $useLookup \
			\
			E1 0 \
			E2 0 \
			angleDuration 0 \
			initialE 0 \
			maxE 0 \
			minE 0 \
			sensorInfluenceOnBattery 0 \
			HomeostaticBraitenburgMutationVariance 0 \
			tauE 0 > deleteme"

			ssh -f star$host.tn.informatics.scitech.susx.ac.uk "$cmd; $cmd2 &"

    done
    done

}

##################### PROCESS COMMANDS #################################

if [ "$1" == "killall" ]; then
  
  runKillAll
  exit

elif [ "$1" == "progress" ]; then

  runProgress
  exit

elif [ "$1" == "evolve" ]; then
  
  runEvolve
  exit

else
  echo "$1 not understood."
  exit
fi
