class SpodManager<'item> {

	static var cnx : sql.Connection = null;
	static var init_list : List<SpodManager<{}>> = new List();
	static var cache_field = Std.fieldId("__cache__");
	static var object_cache : Hash<'item> = new Hash();

	static function initialize() {
		init_list.iter(fun(m : SpodManager<{}>) {
			if( m.relations.RELATIONS != null )
				m.relations.RELATIONS().iter(callback(m,initRelation));
		});
		init_list = new List();
	}

	static function runMainLoop(mainFun : void -> void,logError : 'a -> void, count : int) {
		try {
			mainFun();
		} catch(e) {
			if( count > 0 && cnx.isDeadlock(e) ) {
				cnx.request("ROLLBACK"); // should be already done, but in case...
				cnx.request("START TRANSACTION");
				runMainLoop(mainFun,logError,count-1);
				return;
			}
			logError(e); // should ROLLBACK if needed
		}
	}

	static function loop( dbparams, dbname, mainFun : void -> void, logError : 'a -> void ) {
		initialize();
		cnx = Sql.connect(dbparams);
		cnx.selectDB(dbname);
		cnx.request("START TRANSACTION");
		runMainLoop(mainFun,logError,3);
		cnx.request("COMMIT");
		cnx.close();
		cnx = null;
		object_cache = new Hash();
	}

	static function quoteField(f : String) {
		if( f == "read" || f == "desc" || f == "out" || f == "range" )
			return "`"+f+"`";
		return f;
	}

	var relations : { RELATIONS : void -> Array<{ prop : String, key : String, manager : {} }> };
	var nkeys : int;
	var table_name : String;
	var table_ids : Array<String>;
	var table_field_ids : Array<int>;
	var table_fields : Array<String>;
	var table_id_fields : Array<int>;
	var class_proto : {};

	function new( t : { NAME : String, ID : String } ) {
		// get basic infos
		table_name = t.NAME;
		table_ids = t.ID.split(":").toArray();
		table_field_ids = table_ids.map(fun(f){ return Std.fieldId(f); });
		table_ids = table_ids.map(fun(f) { return quoteField(f) });
		nkeys = table_ids._length;
		relations = Std.cast(t);
		class_proto = downcast(t).prototype;

		// get the list of private fields
		var apriv : Array<String> = downcast(t).PRIVATE_FIELDS;
		if( apriv == null ) apriv = new Array();
		var priv = apriv.map(fun(i) { return Std.fieldId(i) }).toList();
		priv.add(Std.fieldId("local_manager"));

		// get the fields not marked privates
		var proto = class_proto;
		var fields = Std.fields(proto);
		table_id_fields = fields.filter(fun(i : int) {
			return !priv.exists(fun(i2 : int) { return i == i2 }) && Std.typeof(Std.field(proto,i)) != Std.types.tfunction;
		}).toArray();
		table_fields = table_id_fields.map(fun(i) { return quoteField(Std.fieldName(i)) });

		// add manager
		downcast(proto).local_manager = this;
		init_list.add(Std.cast(this));
	}

	// -------------------- Useful API -------------------------

	function select( cond : String ) {
		var s = new StringBuf();
		s.add("SELECT * FROM ");
		s.add(table_name);
		s.add(" WHERE ");
		s.add(cond);
		s.add(" FOR UPDATE");
		return s.toString();
	}

	function selectReadOnly( cond : String ) {
		var s = new StringBuf();
		s.add("SELECT * FROM ");
		s.add(table_name);
		s.add(" WHERE ");
		s.add(cond);
		return s.toString();
	}

	function getGeneric( id : int, readonly : bool ) {
		if( nkeys != 1 )
			Std.throw("Invalid number of keys");
		if( id == null )
			return null;
		var x = object_cache.get(id + table_name);
		if( x != null )
			return x;
		var s = new StringBuf();
		s.add("SELECT * FROM ");
		s.add(table_name);
		s.add(" WHERE ");
		s.add(table_ids[0]);
		s.add(" = ");
		addQuote(s,id);
		if( !readonly )
			s.add(" FOR UPDATE");
		return object(s.toString());
	}

	function get( id : int ) : 'item {
		return getGeneric(id,false);
	}

	function getReadOnly( id : int ) : 'item {
		return getGeneric(id,true);
	}

	function getWithKeys( keys : {} ) : 'item {
		var s = new StringBuf();
		s.add("SELECT * FROM ");
		s.add(table_name);
		s.add(" WHERE ");
		addKey(s,Std.cast(keys));
		s.add(" FOR UPDATE");
		return object(s.toString());
	}

	function search( x : {} ) : List<'item> {
		var s = new StringBuf();
		s.add("SELECT * FROM ");
		s.add(table_name);
		s.add(" WHERE ");
		var fl = Std.fields(x);
		var me = this;
		var first = true;
		fl.iter(fun(f) {
			if( first )
				first = false;
			else
				s.add(" AND ");
			s.add(quoteField(Std.fieldName(f)));
			var d = Std.field(x,f);
			if( d == null )
				s.add(" IS NULL");
			else {
				s.add(" = ");
				me.addQuote(s,d);
			}
		});
		s.add(" FOR UPDATE");
		return objects(s.toString());
	}

	function list( start, length ) : List<'item> {
		return objects(select("TRUE "+limit(start,length)));
	}

	function limit( start : int, length : int ) : String {
		if( start == null && length == null )
			return "";
		return " LIMIT "+start+","+length;
	}

	function count() : int {
		return execute("SELECT COUNT(*) FROM "+table_name).getIntResult(0);
	}

	function lastInsertId() : int {
		return execute("SELECT LAST_INSERT_ID()").getIntResult(0);
	}

	function make( x : 'a ) : 'item {
		if( x == null )
			return null;
		var x2 = getFromCache(Std.cast(x));
		if( x2 != null )
			return x2;
		addToCache(Std.cast(x));
		Std.neko("$objsetproto(x,this.class_proto)");
		Std.setField(Std.cast(x),cache_field,Std.neko("$new(x)"));
		return Std.cast(x);
	}

	function unmake( x : 'item ) : void {
	}


	function quote( s : String ) : String {
		return "'" + cnx.escape( s ) + "'";
	}

	function execute( sql : String ) {
		return cnx.request(sql);
	}

	function result( sql : String ) : 'a {
		var r = cnx.request(sql);
		r.next();
		return r.current;
	}

	function results( sql : String ) : List<'a> {
		return cnx.request(sql).results();
	}

	function object( sql : String ) : 'item {
		var r = cnx.request(sql);
		r.next();
		if( r.current == null )
			return null;
		return make(r.current);
	}

	function objects( sql : String ) : List<'item> {
		return cnx.request(sql).results().map(callback(this,make));
	}

	// -------------------- Internal Functions -------------------------

	function initRelation(r : { prop : String, key : String, manager : {} } ) {
		// setup getter/setter
		var manager : SpodManager<{}> = Std.cast(r.manager);
		if( manager == null ) Std.throw("Relation "+r.prop+"("+r.key+") have null manager");
		if( manager.nkeys != 1 ) Std.throw("Relation "+r.prop+"("+r.key+") on a multiple key table");
		var hprop = Std.fieldId("$"+r.prop);
		var hkey = Std.fieldId(r.key);
		Std.setField(class_proto,Std.fieldId("get_"+r.prop),fun() {
			var othis = Std.neko("this");
			var f = Std.field(othis,hprop);
			if( f != null )
				return f;
			f = manager.get(Std.field(othis,hkey));
			Std.setField(othis,hprop,f);
			return f;
		});
		Std.setField(class_proto,Std.fieldId("set_"+r.prop),fun(f) {
			var othis = Std.neko("this");
			Std.setField(othis,hprop,f);
			Std.setField(othis,hkey,Std.field(f,manager.table_field_ids[0]));
			return f;
		});
		// remove _prop from precomputed table_fields
		var f = "_"+r.prop;
		var fid = Std.fieldId(f);
		var fl = table_fields.toList();
		fl.remove(f);
		table_fields = fl.toArray();
		var fli = table_id_fields.toList();
		fli.remove(fid);
		table_id_fields = fli.toArray();
	}

	function doSync( i : 'item ) {
		object_cache.remove(makeCacheKey(i));
		var x : {} = Std.cast(i);
		var x2 : {} = Std.cast(getWithKeys(x));
		var fl = Std.fields(x);
		fl.iter(fun(fid) { Std.setField(x,fid,null) }); // set all fields to null
		fl = Std.fields(x2);
		fl.iter(fun(fid) { Std.setField(x,fid,Std.field(x2,fid)) }); // copy fields from new object
		Std.setField(x,cache_field,Std.field(x2,cache_field)); // set same DB cache
		addToCache(i);
	}

	function doInsert( x : 'item ) {
		unmake(x);
		var s = new StringBuf();
		var fields = new List();
		var fids = new List();
		var i = 0;
		var me = this;
		table_id_fields.iter(fun(fid) {
			if( Std.field(Std.cast(x),fid) != null ) {
				fields.add(me.table_fields[i]);
				fids.add(fid);
			}
			i++;
		});
		s.add("INSERT INTO ");
		s.add(table_name);
		s.add(" (");
		s.add(fields.join(","));
		s.add(") VALUES (");
		var tids = fids.toArray();
		var l = tids._length;
		for(i=0;i<l;i++) {
			var v = Std.field(Std.cast(x),tids[i]);
			addQuote(s,v);
			if( i != l - 1 )
				s.add(", ");
		}
		s.add(")");
		execute(s.toString());
		// inserted one key without id : suppose autoincrement
		if( nkeys == 1 && Std.field(Std.cast(x),table_field_ids[0]) == null )
			Std.setField(Std.cast(x),table_field_ids[0],lastInsertId());
		addToCache(x);
	}

	function doUpdate( x : 'item ) {
		unmake(x);
		var s = new StringBuf();
		s.add("UPDATE ");
		s.add(table_name);
		s.add(" SET ");
		var i;
		var l = table_fields._length;
		var cache = Std.field(Std.cast(x),cache_field);
		var mod = false;
		for(i=0;i<l;i++) {
			var fid = table_id_fields[i];
			var v = Std.field(Std.cast(x),fid);
			var vc = Std.field(cache,fid);
			if( v != vc ) {
				if( mod )
					s.add(", ");
				mod = true;
				s.add(table_fields[i]);
				s.add(" = ");
				addQuote(s,v);
				Std.setField(cache,fid,v);
			}
		}
		if( !mod )
			return;
		s.add(" WHERE ");
		addKey(s,x);
		execute(s.toString());
	}

	function doDelete( x : 'item ) {
		var s = new StringBuf();
		s.add("DELETE FROM ");
		s.add(table_name);
		s.add(" WHERE ");
		addKey(s,x);
		execute(s.toString());
	}

	function doToString( it : 'item ) : String {
		var s = new StringBuf();
		s.add(table_name);
		s.add("#");
		var i;
		for(i=0;i<nkeys;i++) {
			if( i != 0 )
				s.add(":");
			s.add(Std.field(Std.cast(it),table_field_ids[i]));
		}
		return s.toString();
	}

	function addQuote( s : StringBuf, v : 'a ) {
		var t = Std.neko("$typeof(v)");
		if( Std.neko("t == $tint || t == $tnull || t == $tbool") )
			s.add(v);
		else {
			s.add("'");
			s.add(cnx.escape(Std.neko("String.new($string(v))")));
			s.add("'");
		}
	}

	function addKey( s : StringBuf, x : 'item ) {
		var i;
		for(i=0;i<nkeys;i++) {
			if( i != 0 )
				s.add(" AND ");
			s.add(table_ids[i]);
			s.add(" = ");
			var f = Std.field(Std.cast(x),table_field_ids[i]);
			if( f == null )
				Std.throw("Missing key "+table_ids[i]);
			addQuote(s,f);
		}
	}

	// object cache handling code

	function makeCacheKey( x : 'item ) {
		if( nkeys == 1 ) {
			var k : String = Std.field(Std.cast(x),table_field_ids[0]);
			if( k == null )
				Std.throw("Missing key "+table_ids[0]);
			if( !Std.is(k,String) )
				k = new String(k);
			return k + table_name;
		}
		var s = new StringBuf();
		var i;
		for(i=0;i<nkeys;i++) {
			var k = Std.field(Std.cast(x),table_field_ids[i]);
			if( k == null )
				Std.throw("Missing key "+table_ids[i]);
			s.add(k);
			s.add("#");
		}
		s.add(table_name);
		return s.toString();
	}

	function addToCache( x : 'item ) {
		object_cache.set(makeCacheKey(x),x);
	}

	function getFromCache( x : 'item ) : 'item {
		return object_cache.get(makeCacheKey(x));
	}

}
