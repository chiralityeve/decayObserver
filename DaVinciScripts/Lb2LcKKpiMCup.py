########################################################################

simulation = True

########################################################################
from os import environ
from Gaudi.Configuration import *
from Configurables import DecayTreeTuple,BTaggingTool,SubstitutePID
from Configurables import CombineParticles, FilterDesktop,TupleToolDecayTreeFitter,TupleToolDecay
from Configurables import TupleToolTrigger, BackgroundCategory, TupleToolTISTOS,TupleToolRecoStats, TupleToolTagging
from Configurables import DaVinci,HltSelReportsDecoder,HltVertexReportsDecoder,HltDecReportsDecoder
from DecayTreeTuple.Configuration import *
########################################################################
decay = "[Lambda_b~0 -> ^(Lambda_c~- -> ^p~- ^K+ ^pi-) ^(a_1(1260)+ -> ^K+ ^K- ^pi+)]CC"
########################################################################
LambdaBOfflineTree = DecayTreeTuple("LambdaBOfflineTree")
#data 
#LambdaBOfflineTree.Inputs = ["/Event/Bhadron/Phys/Lb2LcKKPiLc2PKPiBeauty2CharmLine/Particles"]
LambdaBOfflineTree.Inputs = ["/Event/AllStreams/Phys/Lb2LcKKPiLc2PKPiBeauty2CharmLine/Particles"] # MC
LambdaBOfflineTree.Decay = decay
LambdaBOfflineTree.ToolList = [ 
                           "TupleToolKinematic",
                           "TupleToolPid",
                           "TupleToolGeometry",
                           "TupleToolPrimaries",
                           "TupleToolTrackInfo",
                           "TupleToolEventInfo",
                           "TupleToolRecoStats",
                           "TupleToolMuonPid",
                           "TupleToolProtoPData",
                          ]

if simulation:
    LambdaBOfflineTree.ToolList +=[
        "TupleToolMCTruth",
        "TupleToolMCBackgroundInfo"
    ]

########################################################################
from Configurables import TupleToolPid
LambdaBOfflineTree.addTool(TupleToolPid, name = "TupleToolPid")
LambdaBOfflineTree.TupleToolPid.Verbose = True

from Configurables import TupleToolTrackInfo
LambdaBOfflineTree.addTool(TupleToolTrackInfo,name="TupleToolTrackInfo")
LambdaBOfflineTree.TupleToolTrackInfo.Verbose = True

from Configurables import TupleToolKinematic
LambdaBOfflineTree.addTool( TupleToolKinematic,name = "TupleToolKinematic" )
LambdaBOfflineTree.TupleToolKinematic.Verbose = True

from Configurables import TupleToolGeometry
LambdaBOfflineTree.addTool( TupleToolGeometry, name = "TupleToolGeometry" )
LambdaBOfflineTree.TupleToolGeometry.Verbose = True

LambdaBOfflineTree.addTool(TupleToolRecoStats, name="TupleToolRecoStats")
LambdaBOfflineTree.TupleToolRecoStats.Verbose = True
LambdaBOfflineTree.UseLabXSyntax = True
LambdaBOfflineTree.RevertToPositiveID = False

triggers = ['Hlt2Topo2BodySimpleDecision',
            'Hlt2Topo3BodySimpleDecision',
            'Hlt2Topo4BodySimpleDecision',
            'Hlt2Topo2BodyBBDTDecision',
            'Hlt2Topo3BodyBBDTDecision',
            'Hlt2Topo4BodyBBDTDecision',
            'Hlt2TopoMu2BodyBBDTDecision',
            'Hlt2TopoMu3BodyBBDTDecision',
            'Hlt2TopoMu4BodyBBDTDecision',
            'Hlt2TopoE2BodyBBDTDecision',
            'Hlt2TopoE3BodyBBDTDecision',
            'Hlt2TopoE4BodyBBDTDecision',
            'Hlt2TopoRad2BodyBBDTDecision',
            'Hlt2TopoRad2plus1BodyBBDTDecision',
            'Hlt2IncPhiDecision',
            'Hlt2IncPhiTrackFitDecision',
            'Hlt2IncPhiSidebandsDecision',
            'Hlt2Topo.*Decision',
            'Hlt2IncPhi.*Decision',
            ]

#triggers = [ 'L0HadronDecision',
#             'Hlt1TrackAllL0Decision',
#             'Hlt2Topo.*Decision',
#             'Hlt2IncPhi.*Decision',]

#LambdaBOfflineTree.addTool(TupleToolTrigger, name='TupleToolTrigger')
#LambdaBOfflineTree.TupleToolTrigger.Verbose = True
#LambdaBOfflineTree.TupleToolTrigger.TriggerList = triggers
#LambdaBOfflineTree.addTool(TupleToolTISTOS, name='TupleToolTISTOS')
#LambdaBOfflineTree.TupleToolTISTOS.Verbose = True
#LambdaBOfflineTree.TupleToolTISTOS.TriggerList = triggers

#############Kazu Mods  Start######################
LambdaBOfflineTree.addTool(TupleToolDecay, name = 'lab0')
LambdaBOfflineTree.Branches["lab0"] = "[Lambda_b~0 -> (Lambda_c~- -> p~- K+ pi-) (a_1(1260)+ -> K+ K- pi+)]CC"
#-- 
LambdaBOfflineTree.lab0.ToolList =  ["TupleToolDecayTreeFitter/MassFitBConsLc"] # fit with mass constraint
LambdaBOfflineTree.lab0.addTool(TupleToolDecayTreeFitter("MassFitBConsLc"))
LambdaBOfflineTree.lab0.MassFitBConsLc.constrainToOriginVertex = True
LambdaBOfflineTree.lab0.MassFitBConsLc.daughtersToConstrain = [ "Lambda_c~-" ]
##################################################
# Add other hypotheses
#bssubs = {'Beauty -> Baryon ^pi+ ^pi-  ^K+' : '[K+, K-, pi+]'}

#LambdaBOfflineTree.lab0.ToolList +=  ["TupleToolDecayTreeFitter/MassFitBConsLc_KaonHypo"] # fit with mass constraint
#LambdaBOfflineTree.lab0.addTool(TupleToolDecayTreeFitter("MassFitBConsLc_KaonHypo"))
#LambdaBOfflineTree.lab0.MassFitBConsLc_KaonHypo.constrainToOriginVertex = True
#LambdaBOfflineTree.lab0.MassFitBConsLc_KaonHypo.daughtersToConstrain = [ "Lambda_c~-" ]
#LambdaBOfflineTree.lab0.MassFitBConsLc_KaonHypo.Substitutions = bssubs
###################################################
#
# Standard configuration
#
from Configurables import DaVinci,LHCbApp
from PhysConf.MicroDST import uDstConf
uDstConf('/Event/Bhadron/')
#uDstConf('/Event/AllStreams/')


#from Configurables import EventNodeKiller
#eventNodeKiller = EventNodeKiller('DAQkiller')
#eventNodeKiller.Nodes = ['DAQ', 'pRec']

DaVinci().EvtMax = -1                        # Number of events
DaVinci().SkipEvents = 0                       # Events to skip
DaVinci().PrintFreq = 1000
DaVinci().DataType = "2012"                    # Default is "DC06"
#DaVinci().InputType = 'DST'
DaVinci().InputType = 'MDST'
DaVinci().Simulation   = simulation
DaVinci().HistogramFile = "dummy.root"      # Histogram file
DaVinci().UserAlgorithms = [LambdaBOfflineTree]
DaVinci().TupleFile = "LambdaB2LcHHH.root"
DaVinci().Lumi = True
DaVinci().DDDBtag    = "dddb-20130111"
DaVinci().CondDBtag    = "cond-20130114"
if simulation :
    DaVinci().DDDBtag    = "dddb-20130929-1"
    #DaVinci().CondDBtag    = "sim-20130522-1-vc-md100" # mag down
    DaVinci().CondDBtag    = "sim-20130522-1-vc-mu100" # mag up
#endif    

###############################

LHCbApp().XMLSummary="summary.xml"

#from Configurables import DataOnDemandSvc
#from Configurables import L0SelReportsMaker, L0DecReportsMaker
#DataOnDemandSvc().AlgMap["HltLikeL0/DecReports"] = L0DecReportsMaker( OutputLevel = 4 ) 
#DataOnDemandSvc().AlgMap["HltLikeL0/SelReports"] = L0SelReportsMaker( OutputLevel = 4 )

# Use the CondDB to get the right database tags 
#from Configurables import CondDB
#CondDB(UseOracle = False, DisableLFC = True)
