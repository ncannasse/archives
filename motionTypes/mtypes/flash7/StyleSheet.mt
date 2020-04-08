
native class StyleSheet
{
   function new() : void;
   function load( url:String ) : void;
   function onLoad( success:bool ) : void;
   function parseCSS( str:String ) : bool;
}

