-- premake5.lua
workspace "ChernoTracing"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "ChernoTracing"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
include "Walnut/WalnutExternal.lua"

include "ChernoTracing"