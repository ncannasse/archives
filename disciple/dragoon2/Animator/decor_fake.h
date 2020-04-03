#pragma once

enum { NORMAL, SUB, FOND };

class CDecor {

	public:


	class CTrans {
		
		public :
			void Gere( int ) { };

	} *Trans;

	long Ox,Oy;
	long Mx,My;
	BYTE *Mask;
	RECT DecorVisible;

	CDecor();
};

extern CDecor *Le_Decor;