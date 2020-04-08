native class Stage {
    public static var align: String;
    public static var height : int;
    public static var width : int;
    
    static function addListener(listener:{ onResize : void -> void });
    static function removeListener(listener:{ onResize : void -> void });
}
