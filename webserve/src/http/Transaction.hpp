#ifndef TRANSACTION_HPP
# define TRANSACTION_HPP

# include "Request.hpp"
# include "Response.hpp"

const char hexdigt[17] = "0123456789ABCDEF";

class Transaction {
	public:
		Transaction( Client& );

		const config_t&		config( void );
		connection_e		connection( void );			

		static bool			recvMsg( msg_buffer_t&, const char*, ssize_t& );
		static bool 		recvBody( Client & ,const char* , const ssize_t&  );

		static void			build( const Response&, msg_buffer_t& );
		static void			buildError( const uint_t&, Client& );

		void				act( void );

	private:
		Client&				_cl;

		Request				_rqst;
		Response			_rspn;

		static bool			_recvBodyPlain(Client& , const char*, const ssize_t& );
		static bool			_recvBodyChunk( Client& , const char* );
		static bool			_recvBodyChunkData( Client &, isstream_t& );
		static bool			_recvBodyChunkPredata(Client & );
		static bool			_recvBodyChunkIncomplete(Client &, isstream_t& );

		static void			_buildLine( const Response&, sstream_t& );
		static void			_buildHeader( const Response&, sstream_t& );
		static void			_buildHeaderName( uint_t, sstream_t& );
		static void			_buildHeaderValue( const response_header_t&, uint_t, sstream_t& );
		static void			_buildBody( const Response&, msg_buffer_t& );
		
		void				_validRequest( void );
		void				_setBodyEnd( void );
		bool				_invokeCGI( const Request&, process_t& );
};


# include "Client.hpp"

#endif