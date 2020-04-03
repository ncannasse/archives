# MTS (MotionScript)

The first scripting language compiler that was "complete" and used for actual projects.
I developed this originaly during my stay in Kyoto in 2003, quite inspired by reading OCaml bytecode interpreter sources.

MTS was originaly just a toy language/vm, but when working on Dinoparc in 2005 I was using Motion-Types for client side (flash) and wanted something similar for server side (didn't want to use PHP). So I wrote a MTS output for Motion-Types (which was already outputing ActionScript version 1) and put the MTS VM into a Apache module, added MySQL library and in about one week had a full server-side language.

MTS was later rewritten entirely to become NekoVM.

## Honsen

Honsen was some work in progress of a web player for MTS, like a Flash replacement but with 3D support with DirectX. There was working ActiveX and NPAPI (Firefox) plugins, exporter for 3DSMax, etc. It could also do 2D vector rendering using the AntiGrain (agg) library.