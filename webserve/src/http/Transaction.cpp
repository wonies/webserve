#include "HTTP.hpp"
#include "Client.hpp"

#include "Transaction.hpp"

/*	HTTP MESSAGE FORMMAT

	HTTP-message   = start-line CRLF
                   *( field-line CRLF )
                   CRLF
                   [ message-body ]

	request-line = method SP request-target SP HTTP-version
	status-line = HTTP-version SP status-code SP [ reason-phrase ]

	field-line = field-name ":" OWS field-value OWS
*/

/* ACCESS */
const config_t& Transaction::config( void ) { return _rqst.config(); }

connection_e
Transaction::connection( void ) {
	if ( _rqst.header().connection == CN_CLOSE || _rspn.header().connection == CN_CLOSE ) return CN_CLOSE;
	else return CN_KEEP_ALIVE;
}

/* INTANTIATE */
Transaction::Transaction( Client& client ): _cl( client ), _rqst( client ) {
	log( "HTTP\t: constructing Transaction" );

	_setBodyEnd();
	if ( _invokeCGI( _rqst, _cl.subprocs ) ) CGI::proceed( _rqst, _cl.subprocs );
}

/* METHOD - act: do the requested method and build plain response message */
void
Transaction::act( void ) {
	_rspn.act( _rqst );
	build( _rspn, _cl.out );
}

void
Transaction::_setBodyEnd( void ) {
	if ( _rqst.header().transfer_encoding == TE_CHUNKED ) _cl.in.chunk = TRUE;
	else _cl.in.body_size = _rqst.header().content_length;
}

bool
Transaction::_invokeCGI( const Request& rqst, process_t& procs ) {
	if ( !rqst.location().cgi || isDir( rqst.info ) ) return FALSE;

	size_t	dot = rqst.line().uri.rfind( '.' );
	str_t	ext;

	if ( found( dot ) ) ext = rqst.line().uri.substr( dot + 1 );
	if ( ext.empty() ) return FALSE;

	if ( ext != "cgi" && ext != "exe" ) {
		try { procs.argv.push_back( CGI::script_bin.at( ext ) );  }
		catch( exception_t& exc ) { return FALSE; }
	}

	procs.argv.push_back( rqst.line().uri );
	return TRUE;
}

/*	
	METHOD -
	recvMsg: store request line and header field at stream buffer
	then determine if the receiving message is done

	recvBody: store contetnts at steram buffer and
	determine if the receving body contents is done
*/

bool
Transaction::recvMsg( msg_buffer_t& in, const char* buf, ssize_t& byte_read ) {
	if ( !in.msg_done ) {
		in.msg.write( buf, byte_read );

		size_t pos_header_end = in.msg.str().find( MSG_END, in.msg_read - \
			( in.msg_read * ( in.msg_read < 3 ) + 3 * !( in.msg_read < 3 ) ) );

		if ( !found( pos_header_end ) ) in.msg_read += byte_read;
		else {
			in.msg_done = TRUE;
			logging.fs << in.msg.str() << std::endl;

			size_t body_begin	= pos_header_end - in.msg_read + SIZE_MSG_END;
			in.body_read		= byte_read - body_begin;

			if ( in.body_read )
				in.body.write( &buf[body_begin], in.body_read );

			byte_read = 0;
		}
	}
	return in.msg_done;
}

bool
// Transaction::recvBody( msg_buffer_t& in, const process_t& procs, const char* buf, const ssize_t& byte_read ) 
Transaction::recvBody( Client &cl,  const char* buf, const ssize_t& byte_read ) 
{
	// Keep FALSE untill meet the content-length or tail of chunk (0CRLFCRLF)
	if ( !cl.in.chunk ) return _recvBodyPlain( cl, buf, byte_read );
	else {
		if ( byte_read == 0 && cl.in.body_read ) return _recvBodyChunkPredata( cl ); 	
		return _recvBodyChunk( cl, buf );
	}
}

bool
Transaction::_recvBodyPlain( Client& cl, const char* buf, const ssize_t& byte_read ) {
	cl.in.body_read += byte_read;

	if ( !cl.subprocs.pid )
		cl.in.body.write( buf, byte_read );

	else if ( !dead( cl.subprocs ) ) {
		if ( byte_read == 0 && cl.in.body_read )
			CGI::writeTo( cl.subprocs, cl.in.body.str().c_str(), cl.in.body_read );
		else
			CGI::writeTo( cl.subprocs, buf, byte_read );
	}

	if ( cl.in.body_read ) {
		osstream_t oss;
		oss << "TCP\t: body read by " << byte_read << " so far: " << cl.in.body_read << " / " << cl.in.body_size << std::endl;
		log( oss.str() );
	}

	std::clog << "recvBody checking " << cl.in.body_size << ", " << cl.in.body_read << std::endl;
	return cl.in.body_size == cl.in.body_read;
}

bool
Transaction::_recvBodyChunk( Client & cl, const char* buf ) {
	isstream_t	iss( buf );

	if ( cl.in.incomplete && _recvBodyChunkIncomplete( cl, iss ) ) return TRUE;
	return _recvBodyChunkData( cl, iss );
}

bool
Transaction::_recvBodyChunkData( Client& cl, isstream_t& iss ) {
	char		data[SIZE_BUFF];
	
	ssize_t		frac = 1;
	ssize_t		left;

	while ( frac ) {
		iss >> std::hex >> frac >> std::ws;

		if ( iss.fail() || frac > SIZE_BUFF ) throw errstat_t( 400 );

		left = iss.str().size();
		if ( left < frac + SIZE_CRLF ) cl.in.incomplete = frac + SIZE_CRLF - left;
		if ( left < frac ) frac = left;

		iss.read( data, frac );
		if ( !cl.subprocs.pid ) cl.in.body.write( data, frac );
		else if ( !dead( cl.subprocs ) ) {CGI::writeTo( cl.subprocs, data, frac );}
		else return TRUE;

		
	
		if ( cl.in.incomplete ) break;

		iss >> std::ws;
	}
	return frac == 0;
}

bool
Transaction::_recvBodyChunkPredata( Client& cl ) {
	isstream_t	iss( cl.in.body.str() );

	cl.in.body.str( "" );
	cl.in.body.clear();

	return _recvBodyChunkData( cl, iss );
}

bool
Transaction::_recvBodyChunkIncomplete(Client& cl, isstream_t& iss ) {
	char data[SIZE_BUFF];

	iss.read( data, cl.in.incomplete );
	if ( !cl.subprocs.pid ) cl.in.body.write( data, cl.in.incomplete );
	else if ( !dead( cl.subprocs ) ) CGI::writeTo( cl.subprocs, data, cl.in.incomplete );
	else return TRUE;


	iss >> std::ws;
	cl.in.incomplete = 0;

	return FALSE;
}

/* METHOD - build: build response message base with response object */
void
Transaction::build( const Response& rspn, msg_buffer_t& out ) {
	_buildLine( rspn, out.msg );
	_buildHeader( rspn, out.msg );
	if ( rspn.body() )
		_buildBody( rspn, out );
}

void
Transaction::_buildLine( const Response& rspn, sstream_t& out_msg ) {
	map_uint_str_t::iterator iter = HTTP::key.status.find( rspn.line().status );

	out_msg <<
	HTTP::http.signature << '/' <<
	HTTP::http.version.at( static_cast<size_t>( rspn.line().version ) ) << SP <<
	iter->first << SP <<
	iter->second << CRLF;
}

void
Transaction::_buildHeader( const Response& rspn, sstream_t& out_msg ) {
	for ( vec_uint_t::const_iterator iter = rspn.header().list.begin();
		iter != rspn.header().list.end(); ++iter ) {
		_buildHeaderName( *iter, out_msg );
		_buildHeaderValue( rspn.header(), *iter, out_msg );
	}
	out_msg << CRLF;
}

void
Transaction::_buildHeaderName( uint_t id, sstream_t& out_msg ) {
	out_msg << HTTP::key.header_out.at( id ) << ": ";
}

void
Transaction::_buildHeaderValue( const response_header_t& header, uint_t id, sstream_t& out_msg ) {
	switch( id ) {
		case OUT_SERVER			: out_msg << header.server; break;
		case OUT_DATE			: out_msg << timeToStr( header.date ); break;
		case OUT_CONNECTION		: out_msg << str_connection[header.connection]; break;
		case OUT_TRANSFER_ENC	: out_msg << HTTP::http.encoding.at( header.transfer_encoding ); break;
		case OUT_CONTENT_LEN	: out_msg << header.content_length; break;
		case OUT_CONTENT_TYPE	: out_msg << header.content_type; break;
		case OUT_LOCATION		: out_msg << header.location; break;

		case OUT_ALLOW			: {
			vec_uint_t::const_iterator iter = header.allow.begin();

			while ( iter != header.allow.end() ) {
				out_msg << HTTP::http.method.at( *iter );
				if ( ++iter != header.allow.end() ) out_msg << ", ";
			}
			break;
		}
		case OUT_SET_COOKIE		: out_msg << header.cookie; break;
	}
	out_msg << CRLF;
}

void
Transaction::_buildBody( const Response& rspn, msg_buffer_t& out ) {
	out.body << rspn.body().rdbuf();
}

void
Transaction::buildError( const uint_t& status, Client& cl ) {
	if ( cl.action ) build( Response( status, cl.action->config() ), cl.out );
	else build( Response( status, cl.server().configDefault() ), cl.out );
}
