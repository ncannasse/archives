native class SharedObject
{

  public var data : {} ;

  static function getLocal(name:String) : SharedObject ;

  function clear() : void ;
  function flush() : bool ;
  function getSize() : int ;
  function onStatus( infoObj:{code:String, level:String} ) : void ;
}