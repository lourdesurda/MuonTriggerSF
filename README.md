# MuonTriggerSF
Run 3 CMS muon trigger efficiency analysis: ROOT macros and CRAB submission scripts.

# Original instructions
https://cernbox.cern.ch/files/link/public/WdzFi8D1tnnRhMl?items-per-page=100&files-public-link-view-mode=resource-table&tiles-size=2

# Environment:
```
CMSSW_15_0_2
To set up the environment:
cmsrel CMSSW_15_0_2
cd CMSSW_15_0_2/src
cmsenv
```
#Now clone the repository 

# Produce ntuples from MiniAOD using CRAB:
```
python3 crab_submission.py
```

# Merge ntuples after CRAB jobs are completed:
```
hadd merged.root <PATH_TO_YOUR_RESULTS>/*/*.root
```

# Compute efficiency histograms:
```
root -l              # Start ROOT
.L debrecen.C+   # Load your macro with ACLiC (compiles it)
debrecen d;          # Create an instance of your class
d.Init("path/to/your/rootfile.root");  # Initialize it with your CRAB ntuple
d.Loop();            # Loop over events to fill histograms
.q                    # Quit ROOT
```

# It will produce
```
efficiency/root/output_test.root
```

# Derive scale factors:
```
root -l SF.C
```
