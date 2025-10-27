import os
import re
from datetime import datetime

# Datasets grouped by year
datasets_by_year = {
   "2022": [
        "/SingleMuon/Run2022A-22Sep2023-v1/MINIAOD",
        "/SingleMuon/Run2022B-22Sep2023-v1/MINIAOD",
        "/SingleMuon/Run2022C-22Sep2023-v1/MINIAOD",
    ],
    "2023": [
        "/Muon0/Run2023B-22Sep2023-v1/MINIAOD",
        "/Muon0/Run2023C-22Sep2023_v1-v1/MINIAOD",
        "/Muon0/Run2023C-22Sep2023_v2-v1/MINIAOD",
        "/Muon0/Run2023C-22Sep2023_v3-v1/MINIAOD",
        "/Muon0/Run2023C-22Sep2023_v4-v1/MINIAOD",
        "/Muon0/Run2023D-22Sep2023_v1-v1/MINIAOD",
        "/Muon0/Run2023D-22Sep2023_v2-v1/MINIAOD"
    ],
    "2024": [
        "/Muon0/Run2024D-MINIv6NANOv15-v1/MINIAOD",
        "/Muon0/Run2024E-MINIv6NANOv15-v1/MINIAOD",
        "/Muon0/Run2024F-MINIv6NANOv15-v1/MINIAOD",
        "/Muon0/Run2024G-MINIv6NANOv15-v1/MINIAOD",
        "/Muon0/Run2024H-MINIv6NANOv15-v1/MINIAOD",
        "/Muon0/Run2024I-MINIv6NANOv15-v1/MINIAOD",
        "/Muon0/Run2024I-MINIv6NANOv15_v2-v1/MINIAOD"
    ]
}

today = datetime.now().strftime("%Y-%m-%d")
submitVersion = f"MuonTrees_{today}"

for year, datasets in datasets_by_year.items():
    for dataset in datasets:
        run_name = dataset.split('/')[2]
        run_name_clean = re.sub(r'[^a-zA-Z0-9_-]', '_', run_name)
        request_name = f'MuEff_{year}_{run_name_clean}'[:99]

        cfg_filename = f"crabConfig_{run_name_clean}.py"
        with open(cfg_filename, "w") as cfg:
            cfg.write(f"""
from CRABClient.UserUtilities import config
config = config()
config.General.requestName = '{request_name}'
config.General.workArea = 'crab_2024'
config.General.transferLogs = True

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'MuonEfficiencyNtuples.py'
config.JobType.outputFiles = ['sm_data_{year}_{run_name_clean}.root']

config.Data.inputDataset = '{dataset}'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1
config.Data.totalUnits = -1
config.Data.outputDatasetTag = '{request_name}'
config.Data.outLFNDirBase = '/store/user/{os.environ["USER"]}/{submitVersion}'

config.Site.storageSite = 'T3_CH_CERNBOX'
""")

        print(f"Submittting: {request_name}")
        os.system(f"crab submit -c {cfg_filename}")
