
native class XmlSocket
{
   public function new();
   public function connect( host:String, port:int ) : void;
   public function onConnect( success:bool ) : void;
   public function onClose() : void;
   public function onXML( xml:Xml ) : void;
   public function onData( data : String ) : void;
   public function send( xml:String ) : void;
   public function close() : void;

}

