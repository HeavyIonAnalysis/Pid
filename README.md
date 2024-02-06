# Particle Identification Framework

## General information
The procedure of particle identification is described in Sec. 4.4 of this [thesis](https://publikationen.ub.uni-frankfurt.de/opus4/frontdoor/deliver/index/docId/51779/file/main.pdf).
It consists of two steps - determination of particle type hypothesis (fitting) and application of hypothesis to a set of particles (filling).
Running of these steps is described below, in the section "Examples".

## Installation
ROOT6 is needed for installation.
Follow CERN ROOT [instructions](https://root.cern/install/) to install it.
Version compiled with c++17 flag is preferred, otherwise CMAKE_CXX_STANDARD flag needs to be explicitly specified (see below).

The Pid framework is agnostic to the I/O format.
In the current implementation the [AnalysisTree](https://github.com/HeavyIonAnalysis/AnalysisTree) interface is provided.
However one can use another data format (a proper interface should be developed then).

In order to install the Pid framework together with AnalysisTree interface, one needs either (1) to have pre-installed AnalysisTree or (2) to install the AnalysisTree automatically together with the Pid.
Option (2) is strongly recommended.\
(1) Type before performing the *cmake* command:

    export AnalysisTree_DIR=path-to-analysistree-installation/lib/cmake/AnalysisTree/
OR\
(2) Use following cmake flags when running the *cmake* command:

    -DPID_BUNDLED_AT=ON
    -DPID_BUNDLED_AT_VERSION=v2.3.2

Now perform following commands for the installation:

    git clone https://github.com/HeavyIonAnalysis/Pid.git
    cd Pid
    mkdir build install
    cd build
    source path-to-root-installation/bin/thisroot.sh
    cmake -DCMAKE_INSTALL_PREFIX=../install ../
    make -j install

*path-to-root-installation* and *path-to-analysistree-installation* must be replaced with your actual location of Root and AnalysisTree install directories respectively.

## Examples
Before running the Pid framework you need to set up the environment variables:

    source path-to-pid-installation/bin/PidConfig.sh

### Fitting
Let us consider the fitting procedure with an example of 5M Au+Au collisions generated with DCM-QGSM-SMM at beam momentum 12*A* GeV/*c*.
For this you need to prepare an input file with *m*<sup>2</sup>-*p* histograms, like input/m2_pq_vtx.apr20.dcmqgsm.12agev.root.
In the folder "reco_info" there is a histogram with all reconstructed tracks matched to TOF hits.
In folders "reco_vs_sim_info_from_tof", "reco_vs_sim_info_via_vtx" there are histograms for separate particles species (MC-true) - determined from "TOF hit - simulated particles" and "TOF hit - reconstructed track - simulated particles" matchings respectively.\
Another file which you need contains graphic cuts of certain particles species - it allows to ignore during fitting the particles entries in a non-specific parts of the *m*<sup>2</sup>-*p* histogram, which mainly originate from mismatching between track and TOF hit.
This file is input/cuts.root.\
The task which runs the fitting process is tasks/run_pid_dcm12.cpp (look for comments in this file for understanding the configuration example), and it produces a C++ executable install/bin/run_pid_dcm12.
To run this executable just type:

    ./run_pid_dcm12

This run will produce several files, let us digest them.
File pionpos.root contains a set of 1-D histograms named "h_..." where <...> means average value of momentum p, and histograms visualize fitting the pions yield as a function of *m*<sup>2</sup> in a certain momentum range.
Graphs pionpos_0, pionpos_1 and pionpos_2 represent the momentum dependence of fitting parameters of the previous fits: height, mean and sigma of the gaussian.
The height is saved as is, while mean and sigma are fitted.
Finally, the chi2 graph shows the chi2 dependence on momentum when *m*<sup>2</sup> fits were performed.
Similar structre is in files with different particles species (kaonpos, proton, pionneg, kaonneg).\
File allpos.root represents result of fitting all positively charged particles simultaneously.
It contains a set of 1-D histograms named "h_..." where <...> means average value of momentum p, and histograms visualize fitting all particles yields and background as a function of m2 in a certain momentum range.
Graphs pionpos_0, pionpos_1 and pionpos_2, kaonpos_0, ..., bgpos_1, bgpos_2 represent the momentum dependence of fitting parameters of the previous fits for all particles species (height, mean and sigma of the gaussian) and background (three coefficients of 2-nd order polynomial).
Finally, the chi2 graph shows the chi2 dependence on momentum when *m*<sup>2</sup> fits were performed.\
Running the macro/drawFits.C macro on the allpos.root (allneg.root) file allows to build all these distributions on the same canvas.

Another file produced during the fitting procedure is pid_getter.root.
There is a Pid::Getter which contains results of fitting.
In order to determine bayesian probabilities (purities) of particle species in a certain *m*<sup>2</sup>-*p* point (*p*=3 GeV/*c*, *m*<sup>2</sup>=1 (GeV/*c*<sup>2</sup>)<sup>2</sup>), one runs

    pid_getter->GetBayesianProbability(3, 1)

and obtains result in the following form:

    (std::map<int, double>) { -321 => 0.0000000, -211 => 0.0000000, -1 => 0.0000000, 1 => 0.0011928893, 211 => 8.0296922e-81, 321 => 7.1999527e-46, 2212 => 0.99880711 }
    // Here 1 and -1 stand for background in the right and left side of the histogram respectively.

Running the macro/RunGetter.C on the pid_getter.root file produces a set of plots showing purity distribution of different particles species and background.

### Filling
Once fitting is preformed and the pid_getter.root file is produced, filling the root-file containing reconstructed tracks can be done - each track will be assigned with probabilities of belonging to different particle species and (optionally) its particle type hypothesis.
This is done in the at_interface/PidFiller.*pp, which are managed by the task tasks/fill_pid.cpp (look for comments in this file for understanding the configuration example).
It produces an executable fill_pid which can be run:

    ./fill_pid filelist.list pid_getter.root

where filelist.list is a text file containing names of the AnalysisTree root-files to be worked on (en example of AnalysisTree file can be downloaded [here](https://sf.gsi.de/f/3ba5a9e3ff5248edba2c/?dl=1)).
After running this exacutable a pid.analysistree.root file is produced.
It is based on the input file with following changes:
branches RichRings and TrdTracks are missing, and branch VtxTracks is replaced with RecParticles branch.
The RecParticles differs from VtxTracks in, firstly, the type of branch (Particles instead of Track), and secondly - in few additional fields: prob_K, prob_d, prob_p, prob_pi, prob_bg - a probability of the particle to be a kaon, deuteron, proton, pion or background (undefined type).
Also there is a field 'pid' which shows the most probable particle type hypothesis.

### Doxygen documentation
    doxygen docs/Doxyfile
File Doxygen/html/index.html with documentation will be created
