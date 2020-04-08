
// MTYPES BINDINGS

// when compiling using Flash6, this will overwrite the String class constructor.
if( _global.string == undefined )
	_global.string = function(x) { return x; }

Array.prototype.remove = function(obj) {
	var i;
	var l = this.length;
	for(i=0;i<l;i++)
		if( this[i] == obj ) {
			this.splice(i,1);
			return true;
		}
	return false;
}

Array.prototype.insert = function(i,x) {
	this.splice(i,0,x);
}

Array.prototype.duplicate = Array.prototype.slice;

Color.prototype.reset = function() {
	this.setTransform({ ra : 100, rb : 0, ba : 100, bb : 0, ga : 100, gb : 0, aa : 100, ab : 0 });
}
XMLNode.prototype.get = function(x) {
	return this.attributes[x];
}
XMLNode.prototype.set = function(x,y) {
	this.attributes[x] = y;
}
XMLNode.prototype.exists = function(x) {
	return this.attributes[x] != null;
}
XMLNode.prototype.attributesIter = function(f) {
	for (var k in this.attributes)
		f( k, this.attributes[k] );
}
XMLNode.prototype.childrenIter = function(f) {
	var x = this.firstChild;
	while( x != null ) {
		f(x);
		x = x.nextSibling;
	}
}

XML.prototype.get = function(x) {
	return this.attributes[x];
}
XML.prototype.set = function(x,y) {
	this.attributes[x] = y;
}
XML.prototype.exists = function(x) {
	return this.attributes[x] != null;
}
_global.ASSetPropFlags(Array.prototype, "remove", 1);
_global.ASSetPropFlags(Array.prototype, "insert", 1);
_global.ASSetPropFlags(Array.prototype, "duplicate", 1);
_global.ASSetPropFlags(Color.prototype, "reset", 1);
_global.ASSetPropFlags(XMLNode.prototype, "get", 1);
_global.ASSetPropFlags(XMLNode.prototype, "set", 1);
_global.ASSetPropFlags(XMLNode.prototype, "exists", 1);
_global.ASSetPropFlags(XMLNode.prototype, "attributesIter", 1);
_global.ASSetPropFlags(XMLNode.prototype, "childrenIter", 1);
_global.ASSetPropFlags(XML.prototype, "get", 1);
_global.ASSetPropFlags(XML.prototype, "set", 1);
_global.ASSetPropFlags(XML.prototype, "exists", 1);
_global.StyleSheet = _global.TextField.StyleSheet;

this.Std = new Object();
this.Std.icounter = 0;
this.Std.attachMC = function(mc,link,depth) {
	var inst = link+"@"+(this.icounter++);
	mc.attachMovie(link,inst,depth);
	return mc[inst];
}

this.Std.createEmptyMC = function (mc,depth) {
	var inst = "empty@"+(this.icounter++);
	mc.createEmptyMovieClip(inst,depth);
	return mc[inst];
}

this.Std.duplicateMC = function (mc,depth) {
	var inst = "dup@"+(this.icounter++);
	mc.duplicateMovieClip(inst,depth);
	return mc._parent[inst];
}

this.Std.getVar = function (mc,v) {
	return mc[v];
}

this.Std.setVar = function (mc,v,vval) {
	mc[v] = vval;
}

this.Std.getRoot = function() {
	return _root;
}

this.Std.getGlobal = function(v) {
	return _global[v];
}

this.Std.setGlobal = function(v,vv) {
	_global[v] = vv;
}

this.Std.createTextField = function(mc,depth) {
	var inst = "text@"+(this.icounter++);
	mc.createTextField(inst,depth,0,0,100,20);
	return mc[inst];
}

this.Std.cast = function(x) {
	return x;
}

this.Std.hitTest = function(mc1,mc2) {
	return mc1.hitTest(mc2);
}

this.Std.random = function(n) {
	return random(n);
}

this.Std.xmouse = function() {
	return _root._xmouse;
}

this.Std.ymouse = function() {
	return _root._ymouse;
}

this.Std.escape = _global.escape;

this.Std.unescape = _global.unescape;

this.Std.parseInt = _global.parseInt;

this.Std.toString = function(x) {
	return String(x);
}

this.Std.toStringBase = function(x,n) {
	return Number(x).toString(n);
}

this.Std.isNaN = _global.isNaN;

this.Std.infinity = Infinity;

this.Std.registerClass = Object.registerClass;

this.Std.copy = function(a,b) {
	for(var i in b)
		a[i] = b[i];
	return a;
}

this.Std.callback = function(o,f) {
	if( arguments.length == 2 )
		return function() { return o[f].apply(o,arguments); }
	else {
		var a = arguments.slice(2);
		return function() { return o[f].apply(o,a.concat(arguments)); }
	}
}

this.Std.getTimer = function() {
	return getTimer();
}

this.Std._typeof = function(o) {
	return typeof(o);
}

this.Std._instanceof = function(o,k) {
	return o instanceof k;
}

this.Std.forin = function(o,f) {
	var k;
	for(k in o)
		f(k);
}

this.Std.makeNew = function(o,a1,a2,a3,a4) {
	return new o(a1,a2,a3,a4);
}

this.Std.deleteField = function(o,k) {
	delete o[k];
}

this.Std.fscommand = function(c,a) {
	fscommand(c,a);
}

this.Std["throw"] = function(e) {
	throw(e);
}

////// LOG BINDINGS

this.Log = new Object();
this.Log.std = this.Std;
this.Log.initdone = false;

this.Log.clear = function() {
	this.tprint.text = "";
	this.ttrace.text = "";
}

this.Log.init = function(mc,w,h) {
	if( mc == undefined )
		mc = _root;
	if( w == undefined )
		w = Stage.width;
	if( h == undefined )
		h = Stage.height;
	this.maxHeight = h - 30;
	this.initdone = true;
	this.ttrace = this.std.createTextField(mc,99999);
	this.tprint = this.std.createTextField(mc,99998);
	this.ttrace._x = 5;
	this.ttrace._y = 20;
	this.ttrace._width = w-10;
	this.ttrace._height = h;
	this.tprint._x = w/2;
	this.tprint._y = 20;
	this.tprint._width = w/2;
	this.tprint._height = h-30;
	this.ttrace.wordWrap = true;
	this.tprint.wordWrap = true;
	this.ttrace.selectable = false;
	this.tprint.selectable = false;
	var me = this;
	this.tid = setInterval(function() {
		if( me.tprint.text != "" )
			me.tprint.text = "";
	},10);
}

this.Log.traceString = function(s) {
	if( !this.initdone ) this.init();
	this.ttrace.text += s.split("\n").join("\\n").split("\r").join("\\r")+"\n";
	while( this.ttrace.textHeight > this.maxHeight ) {
		var p = this.ttrace.text.indexOf("\r",0);
		var s2 = this.ttrace.text.substr(p+1);
		this.ttrace.text = s2;
	}
}

this.Log.setColor = function(c) {
	if( !this.initdone ) this.init();
	this.ttrace.textColor = c;
	this.tprint.textColor = c;
}

this.Log.print = function(x) {
	if( !this.initdone ) this.init();
	this.tprint.text += "~ "+this.toString(x,"")+"\n";
}

this.Log.destroy = function() {
	clearInterval(this.tid);
	this.initdone = false;
	this.ttrace.removeMovieClip();
	this.tprint.removeMovieClip();
}

this.Log.toString = function(o,s) {
	if( s == null )
		s = "";
	else if( s.length >= 20 )
		return "<...>";
	switch(typeof(o)) {
	case "object":
	case "movieclip":
		if( o instanceof Array ) {
			var l = o.length;
			var i;
			var str = "[";
			s += "    ";
			for(i=0;i<l;i++)
				str += ((i > 0)?",":"")+this.toString(o[i],s);
			s = s.substring(4);
			str += "]";
			return str;
		}
		var s2 = o.toString();
		if( typeof(s2) == "string" && s2 != "[object Object]" )
			return s2;
		var k;
		var str = "{\n";
		if( typeof(o) == "movieclip" )
			str = "MC("+o._name+") "+str;
		s += "    ";
		for(k in o) {
			str += s + k + " : "+this.toString(o[k],s)+"\n";
		}
		s = s.substring(4);
		str += s + "}";
		return str;
	case "function":
		return "<fun>";
	case "string":
		return o;
	default:
		return String(o);
	}

}

this.Log.trace = function(o) {
	var s = this.toString(o,"").split("\n");
	var i;
	var l = s.length;
	for(i=0;i<l;i++)
		this.traceString(s[i]);
}

/// HASH BINDINGS

this.Hash = function() {
}

this.Hash.prototype.get = function(k) {
	return this[k];
}

this.Hash.prototype.set = function(k,i) {
	this[k] = i;
}

this.Hash.prototype.remove = function(k) {
	var b = (this[k] != undefined);
	delete(this[k]);
	return b;
}

this.Hash.prototype.exists = function(k) {
	return (this[k] != undefined);
}

this.Hash.prototype.iter = function(f) {
	var k;
	for(k in this)
		f(k,this[k]);

}

_global.ASSetPropFlags(this.Hash.prototype, null, 1);
