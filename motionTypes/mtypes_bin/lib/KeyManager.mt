class KeyManager {

	static var KEYNAMES = null;

	var config : Array<int>;
	var times : Array<float>;
	var pressed : Array<bool>;
	var released : Array<int>;

	function init_keys() {
		var i;

		KEYNAMES = new Array() ;

		// Premier passage pour remplir de "?"
		for (i=0;i<256;i++)
			KEYNAMES[i] = "?";

		// Les lettres normales
		for (i=65;i<=90;i++)
			KEYNAMES[i] = String.fromCharCode(i);

		// Les chiffres
		for (i=48;i<=57;i++)
			KEYNAMES[i] = String.fromCharCode(i);

		// Les chiffres du pavé numérique
		for (i=96;i<=105;i++)
			KEYNAMES[i] = "PavNum "+(i-96);

		// Touches spéciales du pavé
		KEYNAMES[106] = "PavNum *" ;
		KEYNAMES[107] = "PavNum +" ;
		KEYNAMES[108] = "PavNum Entrée" ;
		KEYNAMES[109] = "PavNum -" ;
		KEYNAMES[110] = "PavNum Suppr" ;
		KEYNAMES[111] = "PavNum /" ;

		// Touches de fonction
		for (i=112;i<=123;i++)
			KEYNAMES[i]= "$F" + (i-111) ;

		// La partie lourde: tout le reste !
		KEYNAMES[8] = "$Retour";
		KEYNAMES[9] = "$TAB";
		KEYNAMES[13] = "$Entrée";
		KEYNAMES[16] = "$Majuscule";
		KEYNAMES[17] = "$Controle";
		KEYNAMES[18] = "$Alt";
		KEYNAMES[20] = "Verr.Maj.";
		KEYNAMES[27] = "$Echappe";
		KEYNAMES[32] = "$Espace";
		KEYNAMES[33] = "Page préc.";
		KEYNAMES[34] = "Page suiv.";
		KEYNAMES[35] = "$Fin";
		KEYNAMES[36] = "$Début";
		KEYNAMES[37] = "$Gauche";
		KEYNAMES[38] = "$Haut";
		KEYNAMES[39] = "$Droite";
		KEYNAMES[40] = "$Bas";
		KEYNAMES[45] = "$Insérer";
		KEYNAMES[46] = "$Supprimer";
		KEYNAMES[47] = "$Aide";
		KEYNAMES[144] = "$VerrNum";
		KEYNAMES[186] = "$$ £";
		KEYNAMES[187] = " =  +";
		KEYNAMES[189] = "- _";
		KEYNAMES[191] = ": /";
		KEYNAMES[192] = "ù %";
		KEYNAMES[219] = "° )";
		KEYNAMES[220] = "* µ";
		KEYNAMES[221] = "^ ¨";
		KEYNAMES[222] = "²";

		// protected variables from obfu
		for(i=0;i<256;i++) {
			var k = KEYNAMES[i];
			if( k.charAt(0) == "$" )
				KEYNAMES[i] = k.substring(1);
		}
	}

	function new() {
		if( KEYNAMES == null )
			init_keys();
		config = new Array();
		times = new Array();
		pressed = new Array();
		released = new Array();
		var me = this;
		Key.addListener({
			onKeyDown : fun() {
				var k = Key.getCode();
				me.pressed[k] = true;
			}
			onKeyUp : fun() {
				var k = Key.getCode();
				Std.deleteField(me.pressed,Std.cast(k));
				me.released.push(k);
			}
		});
	}

	function setConfig( id : int, key : int ) {
		config[id] = key;
	}

	function getConfig( id : int ) : int {
		return config[id];
	}

	function getKeyName( key : int ) : String {
		return KEYNAMES[key];
	}


	function isDown(k) {
		return Key.isDown(k);
	}

	function isPressed(k) {
		return (times[k] == 0);
	}

	function isReleased(k) {
		return (times[k] == -1);
	}

	function timePressed(k) {
		return (times[k] <= 0)?0:times[k];
	}

	function update(dt) {
		var i;
		for(i=0;i<pressed.length;i++)
			if( pressed[i] ) {
				var v = times[i];
				if( v == null )
					times[i] = 0;
				else
					times[i] = v + dt;
			}		
		for(i=0;i<released.length;i++) {
			var k = released[i];
			if( times[k] >= 0 )
				times[k] = -1;
			else {
				times[k] = null;
				released.splice(i--,1);
			}
		}
	}

}