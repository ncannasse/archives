native class frusion.gameclient.GameClient
{
    public var forceClose : bool;
    public var forcePause : bool;
    public var reseting : bool;
    public var connected : bool;
    public var gameRunning : bool;
    public var slots : Array<{}>;
    public var ranking : frusion.gameclient.RankingResult;
    public var dailyData : String;
    public var canSaveScore : bool;
    public var noSaveScore : bool;
    public var startTime : Date;

    public function new();
    public function isWhite() : bool;
    public function getVersion() : String;
    public function isBlack() : bool;
    public function isGrey() : bool;
    public function isGray() : bool;
    public function isRed() : bool;
    public function setDebugFunction(debugFunction : String -> void ) : void;
    public function logError(code, message) : void;
    public function fatalError(msg : String, msgUsr : String) : void;
    public function getFileInfos(fileName : String) : { name : String, size : int };
    public function closeService() : void;
    public function serviceConnect() : void;
    public function saveSlot(slotId : int, slot:{}) : void;
    public function startGame() : void;
    public function giveItem(item : String) : void;
    public function giveAccessory( acc : String ) : void;
    public function getUser() : String;
    public function endGame() : void;
    public function saveScore(score : int, miscData:String) : void;
    public function onServiceConnect() : void;
    public function onPause() : void;
    public function onGameClose() : void;
    public function onGameReset() : void;
    public function onStartGame() : void;
    public function onEndGame() : void;
    public function onSaveScoreFruticard() : void;
    public function onSaveScore() : void;
    public function onError() : void;
}
