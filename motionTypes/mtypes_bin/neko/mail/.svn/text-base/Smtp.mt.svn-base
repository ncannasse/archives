class mail.Smtp {

	static function send( host : String, from : String, to : String, data : String ){
		var cnx = new Socket();
		
		try {
			cnx.connect(host,25);
		}catch( e ){
			Std.throw("SMTP connection failed: "+e);
		}
		
		// get server init line
		cnx.readLine();

		cnx.send( "MAIL FROM:<" + from + ">\r\n" );
		var ret = String.trim(cnx.readLine());
		if( ret.sub(0,3) != "250" ){
			cnx.close();
			Std.throw("SMTP error on FROM : " + ret);
		}

		cnx.send( "RCPT TO:<" + to + ">\r\n" );
		ret = String.trim(cnx.readLine());
		if( ret.sub(0,3) != "250" ){
			cnx.close();
			Std.throw("SMTP error on RCPT : " + ret);
		}

		cnx.send( "DATA\r\n" );
		ret = String.trim(cnx.readLine());
		if( ret.sub(0,3) != "354" ){
			cnx.close();
			Std.throw("SMTP error on DATA : " + ret);
		}

		if( data.sub(data._length -2,2) != "\r\n" ) 
			data += "\r\n";

		cnx.send( data + ".\r\n" );
		ret = String.trim(cnx.readLine());
		if( ret.sub(0,3) != "250" ){
			cnx.close();
			Std.throw("SMTP error on mail content: " + ret);
		}

		cnx.send( "QUIT\r\n" );
		cnx.close();
	}
	
}
