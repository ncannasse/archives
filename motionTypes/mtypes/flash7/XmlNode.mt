
native class XmlNode
{
   public var firstChild  : XmlNode;
   public var nextSibling : XmlNode;
   public var nodeValue   : String;
   public var nodeName    : String;

   /**
    * XmlNode constructor.
    *
    * [code]
    * new XmlNode(1, nodeName);  // node
    * new XmlNode(2, nodeValue); // textnode
    * [/code]
    *
    * @param type        '1' for a regular node, '2' for a text node.
    * @param nameOrValue The node name or the node value for text nodes.
    */
   public function new(type:int, nameOrValue:String);

   public function appendChild( child:XmlNode );

   public function toString() : String;

   public function get( att : String ) : String;
   public function set( att : String, aval : String ) : void;
   public function exists( att : String ) : bool;

   public function attributesIter( f : String -> String -> void ) : void;
   public function childrenIter( f : XmlNode -> void ) : void;

   public function removeNode():void ;
}

