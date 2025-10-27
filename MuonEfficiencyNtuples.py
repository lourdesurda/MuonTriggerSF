import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing

options = VarParsing('analysis')

options.register('year', '',
    VarParsing.multiplicity.singleton,
    VarParsing.varType.string,
    "Dataset year (e.g. 2024)"
)
options.register('datasetName', '',
    VarParsing.multiplicity.singleton,
    VarParsing.varType.string,
    "Dataset name (e.g. Run2024C-MINIv6NANOv15-v1)"
)
options.register('inputFile', '',
    VarParsing.multiplicity.singleton,
    VarParsing.varType.string,
    "Input file (local test or CRAB-provided)"
)

options.parseArguments()

year = options.year
datasetName = options.datasetName
inputFile = options.inputFile if options.inputFile else 'file:localtest/default.root'

process = cms.Process("MuonEfficiency")

process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1
process.MessageLogger.cerr.threshold = 'INFO'
process.MessageLogger.cerr.INFO = cms.untracked.PSet(limit = cms.untracked.int32(0))

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))

process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(False),
    TryToContinue = cms.untracked.vstring('ProductNotFound')
)

process.source = cms.Source(
    "PoolSource",
    fileNames = cms.untracked.vstring(inputFile)
)

process.muon = cms.EDAnalyzer(
    'MuonExercise2',
    tracks = cms.untracked.InputTag("slimmedMuons")
)

process.TFileService = cms.Service(
    "TFileService",
    fileName = cms.string(f"sm_data_{year}_{datasetName}.root")
)

process.p = cms.Path(process.muon)
