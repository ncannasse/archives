class mtt2.Utils
{
	static function htmlencode( str:String ) : String
	{
		str = String.replace(str, "&", "&amp;");
		str = String.replace(str, "<", "&lt;");
		str = String.replace(str, ">", "&gt;");
		str = String.replace(str, "\"", "&quot;");
		return str;
	}

	static function htmldecode( str:String ) : String
	{
		str = String.replace(str, "&lt;", "<");
		str = String.replace(str, "&gt;", ">");
		str = String.replace(str, "&quot;", "\"");
		str = String.replace(str, "&amp;", "&");
		return str;
	}
}
