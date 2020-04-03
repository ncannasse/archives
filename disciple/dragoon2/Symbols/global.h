#pragma once

#define NOM_COMPLET "Symbols Editor v1.2"
#define AUTHOR		"Nicolas CANNASSE / Warplayer"
#define HELP		"Symbols Editor v1.2\n(c)2000 by Warplayer\n\nStandard plug-in for convertion Number/Symbol\nOthers plug-ins can call TActions::RunPlugin(\"symbols.dll\",\"ConvertSymbol\", TSYMBOL * );\n\nstruct TSYMBOL { \n    long ID;\n    char Buf[256];\n   int Group;\n};"
