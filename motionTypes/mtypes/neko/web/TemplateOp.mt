class TemplateOp {

	static var TK_POPEN = 0;
	static var TK_PCLOSE = 1;
	static var TK_OP = 2;
	static var TK_VAR = 3;

	static var OP_NORMAL = 0;
	static var OP_IF = 1;
	static var OP_END = 2;
	static var OP_ELSE = 3;
	static var OP_FOREACH = 4;
	static var OP_ELSEIF = 5;
	static var OP_MACRO = 6;

	var op : int;
	var str : String;
	var subs : List<TemplateOp>;
	var elses : List<TemplateOp>;
	var expr : void -> String;

}
