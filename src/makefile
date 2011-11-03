#/*******************************************************************************
# *
# * Makefile
# * 
# * Peter Fine - EASy MSC, Adaptive Systems Coursework
# * Created March '05
# * 
# ******************************************************************************/

EVOLVERDIR = Evolver
UTILSDIR = MyUtils
NETWORKSDIR = Networks
CXXFLAGS= -O3 -I$(UTILSDIR) -I$(EVOLVERDIR) -I$(NETWORKSDIR)
# the following should be used when building the evolver only! 
EVOLUTIONFLAGS=-O3 -march=nocona -funsafe-math-optimizations -I$(UTILSDIR) -I$(EVOLVERDIR) -I$(NETWORKSDIR)
UTILSOBJS = $(UTILSDIR)/Item.o $(UTILSDIR)/Properties.o $(UTILSDIR)/Recorder.o $(UTILSDIR)/MatlabWriter.o 
EVOLVEROBJS = $(EVOLVERDIR)/GeneticAlgorithm.o $(EVOLVERDIR)/Genotype.o \
$(EVOLVERDIR)/NeuralNetwork.o $(EVOLVERDIR)/SimulationManager.o \
$(EVOLVERDIR)/MersenneTwister.o
RobustSensorsEVOLVEOBJS = RobustSensorsManager.o NetworkFactory.o RobustSensorsSimulator.o
RobustSensorsNONEVOLVEOBJS = RobustSensorsManagerNonEvo.o NetworkFactory.o RobustSensorsSimulator.o
RobustSensorsVIEWOBJS = RobustSensorsViewer.o NetworkFactory.o RobustSensorsSimulator.o RobustSensorsRecorder.o
NETWORKOBJS = Networks/CTRNNetwork.o Networks/TanCTRNNetwork.o Networks/HomeostaticBraitenburgNetwork.o

all:
	make evolver
	make viewer
	make interpret
	
fastEvolver:
	make CXXFLAGS="$(EVOLUTIONFLAGS)" evolver
	
evolver: $(RobustSensorsEVOLVEOBJS) $(NETWORKOBJS) $(EVOLVEROBJS) $(UTILSOBJS)
	g++ $(CXXFLAGS) RobustSensorsMain.cpp $(EVOLVEROBJS) $(UTILSOBJS) $(RobustSensorsEVOLVEOBJS) \
	$(NETWORKOBJS) -o ../bin/Evolve
	echo done
	
debugEvolver:
	make cleanAll
	make CXXFLAGS="-g -I$(UTILSDIR) -I$(EVOLVERDIR) -I$(NETWORKSDIR)" evolver

nonevolver: $(RobustSensorsNONEVOLVEOBJS) $(NETWORKOBJS) $(EVOLVEROBJS) $(UTILSOBJS)
	g++ $(CXXFLAGS) RobustSensorsNonEvoMain.cpp $(EVOLVEROBJS) $(UTILSOBJS) $(RobustSensorsNONEVOLVEOBJS) \
	$(NETWORKOBJS) -o ../bin/NonEvolve
	echo done

viewer: $(RobustSensorsVIEWOBJS) $(NETWORKOBJS) $(EVOLVEROBJS) $(UTILSOBJS)
	g++ $(CXXFLAGS) RobustSensorsViewerMain.cpp $(EVOLVEROBJS) $(UTILSOBJS) $(RobustSensorsVIEWOBJS) \
	$(NETWORKOBJS) -o ../bin/View
	echo done
	
interpret: $(UTILSDIR)
	g++ $(CXXFLAGS) $(UTILSDIR)/interpretOutput.cpp -o ../bin/InterpretOutput
	echo done
	
cleanO:
	-rm *.o
	-rm $(NETWORKSDIR)/*.o
	-rm $(UTILSDIR)/*.o
	-rm $(EVOLVERDIR)/*.o
	
cleanAll:
	-rm *.o
	-rm $(NETWORKSDIR)/*.o
	-rm $(UTILSDIR)/*.o
	-rm $(EVOLVERDIR)/*.o
	-rm ../bin/Evolve.exe
	-rm ../bin/NonEvolve.exe
	-rm ../bin/View.exe
