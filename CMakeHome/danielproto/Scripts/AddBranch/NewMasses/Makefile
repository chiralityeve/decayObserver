CXXFLAGS=-Wall -O2 -g $(shell root-config --cflags)
LDLIBS=$(shell root-config --libs) 

all: AddBranch AddBranch_MC AddBranch_LambdaMuMu_MC

AddBranch: AddBranch.cpp
AddBranch_MC: AddBranch_MC.cpp
AddBranch_LambdaMuMu_MC: AddBranch_LambdaMuMu_MC.cpp
