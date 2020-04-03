/* ************************************************************************ */
/*																			*/
/*	 FX SAMPLE																*/
/*																			*/
/* ************************************************************************ */


class TestFX : public AnyFX {
public:

	TestFX() {
	}

	~TestFX() {
	}


	void Begin() {
	}

	void Process() {
	}

	void End() {
	}


	static AnyFX *New( Params *p ) {
		bool b = p->Default();
		return new TestFX();
	}

};


/* ************************************************************************ */
