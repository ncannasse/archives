
native class Xml
{
   public var firstChild : XmlNode;
   public var ignoreWhite : bool;

   function new( src:String ) : void;

   function load( url:String ) : void;
   function onLoad( success:bool ) : void;

   function createElement( nodeName:String ) : XmlNode;
   function createTextNode( content:String ) : XmlNode;
   function appendChild( child:XmlNode ) ;

   function toString() : String;
   function parseXML( s : String ) : void;
}

