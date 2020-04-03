class SpodObject {

/*
	(optional)
	static var PRIVATE_FIELDS = ["my_priv_field"];
	static function RELATIONS() : Array<{ prop : String, key : String, manager : {} }> {
		return [{ key : "uid", prop : "user", manager : User.manager }];
	}

	static var NAME = "TableName";
	static var ID = "id";
	static var manager = new MyManager();
*/

	var local_manager : {};

	function insert() {
		downcast(local_manager).doInsert(this);
	}

	function update() {
		downcast(local_manager).doUpdate(this);
	}

	function delete() {
		downcast(local_manager).doDelete(this);
	}

	function sync() {
		downcast(local_manager).doSync(this);
	}

	function toString() {
		return downcast(local_manager).doToString(this);
	}

	function __string() {
		return Std.neko("this.toString().@s");
	}

}