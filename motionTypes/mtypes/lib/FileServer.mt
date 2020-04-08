class FileServer
{

	static function initPath() {
		var p = downcast(Std.getRoot())._url.split("\\").join("/");
		var k = p.lastIndexOf("/",p.length);
		return p.substr(0,k+1);
	}

	static var HOST = "localhost";
	static var PATH = initPath();
	static var PORT = 6666;

	var onLoad : String -> void;
	var onChange : void -> void;

	var s : XmlSocket;
	var connected : bool;
	var commands : Array<String>;

	function new() {
		connected = false;
		commands = new Array();
		connect();
	}

	function connect() {
		if( connected ) {
			connected = false;
			onChange();
		}
		s = new XmlSocket();
		s.onClose = callback(this,connect);
		s.onData = callback(this,onReceivedData);
		s.onConnect = callback(this,onConnectionResult);
		commands.unshift("<hello url=\""+PATH+"\"/>");
		s.connect(HOST,PORT);
	}

	function close() {
		s.onClose = null;
		s.close();
	}

	function sendCommand(x) {
		if( !connected )
			commands.push(x);
		else {
			var l = string(x.length+1);
			while( l.length < 6 )
				l = "0"+l;
			s.send(l+x);
		}
	}

	function onConnectionResult(b) {
		if( connected != b ) {
			connected = b;
			onChange();
		}
		if( !b ) {
			var me = this;
			var id;
			id = Std.getGlobal("setInterval")(fun() {
				me.s.connect(HOST,PORT);
				Std.getGlobal("clearInterval")(id);
			},40);
		} else {
			var i;
			for(i=0;i<commands.length;i++)
				sendCommand(commands[i]);
			commands = new Array();
		}
	}

	function onReceivedData(d) {
		var x = new Xml(d).firstChild;
		switch( x.nodeName ) {
		case "load":
			onLoad(x.firstChild.nodeValue);
			break;
		}
	}

	function load( file : String ) {
		sendCommand("<load file=\""+file+"\"/>");
	}

	function save( file : String, data : String ) {
		sendCommand("<save file=\""+file+"\">"+data+"</save>");
	}

}