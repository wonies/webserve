#ifndef FILTER_HPP
# define FILTER_HPP

# include "structure.hpp"

# define NONE		0

# define CR			'\r'
# define LF			'\n'
# define CRLF		"\r\n"
# define SP			' '

# define MSG_END	"\r\n\r\n"

// /* ENUM */
// enum method_e {
// 	GET,
// 	POST,
// 	DELETE,
// 	NOT_ALLOWED,
// 	UNKNOWN
// };

// enum version_e {
// 	VERSION_9,
// 	VERSION_10,
// 	VERSION_11,
// 	VERSION_20,
// 	NOT_SUPPORTED
// };

// enum connection_e {
// 	KEEP_ALIVE
// };

// enum header_in_e {
// 	IN_HOST,
// 	IN_CONNECTION,
// 	IN_CHUNK,
// 	IN_CONTENT_LEN,
// 	IN_CONTENT_TYPE
// };

// enum header_out_e {
// 	OUT_SERVER,
// 	OUT_DATE,
// 	OUT_CONNECTION,
// 	OUT_CHUNK,
// 	OUT_CONTENT_LEN,
// 	OUT_CONTENT_TYPE,
// 	OUT_LOCATION,
// 	OUT_ALLOW
// };

// enum cgi_env_e {
// 	SERVER_NAME,
// 	SERVER_PORT,
// 	SERVER_PROTOCOL,
// 	REMOTE_ADDR,
// 	REMOTE_HOST,
// 	GATEWAY_INTERFACE,
// 	REQUEST_METHOD,
// 	SCRIPT_NAME,
// 	CONTENT_LENGTH,
// 	CONTENT_TYPE,
// 	PATH_INFO,
// 	PATH_TRANSLATED,
// 	QUERY_STRING
// };

// /* STRUCT - Keys, Config (Server, Location), Process */
// typedef struct {
// 	vec_str_t			header_in;
// 	vec_str_t			header_out;
// 	map_uint_str_t		status;
// 	map_str_type_t		mime;
// } 	keys_t;

// typedef std::map<method_e, bool> map_method_bool_t;

// typedef struct http_s {
// 	str_t				signature;
// 	vec_str_t			version;
// 	vec_str_t			method;

// 	type_t				type_unknown;
// }	http_t;

struct config_s;

typedef std::string name_t;

typedef struct location_s {
	location_s( const config_s& );

	str_t				alias;
	path_t				root;

	vec_uint_t			allow;

	vec_name_t			index;
	bool 				index_auto;
}	location_t;

typedef std::vector<location_t>	vec_location_t;

typedef struct config_s {
	config_s( void );

	std::vector<name_t> names;
	str_t				name;
	port_t				listen;
	path_t				root;

	name_t				file_40x;
	name_t				file_50x;

	size_t				client_max_body;

	vec_location_t		locations;
}	config_t;

// typedef struct msg_buffer_s {
// 	msg_buffer_s( void );

// 	void				reset( void );
	
// 	sstream_t			msg;
// 	bool				msg_done;
// 	ssize_t				msg_read;

// 	sstream_t			body;
// 	ssize_t				body_size;
// 	ssize_t				body_read;

// }	msg_buffer_t;

// /* STRUCT - Request & Response */
// typedef struct {
// 	version_e			version;
// 	method_e			method;
// 	path_t				uri;
// 	str_t				query;

// }	request_line_t;

// typedef struct request_header_s {
// 	request_header_s( void );

// 	str_t 				host;
// 	// date_t				date;
// 	unsigned 			connection : 2;
// 	unsigned 			chunked : 1;
// 	size_t				content_length;
// 	str_t				content_type;

// 	vec_uint_t			list;

// }	request_header_t;

// typedef struct response_line_s {
// 	response_line_s( void );

// 	version_e			version;
// 	uint_t				status;

// }	response_line_t;

// typedef struct response_header_s {
// 	response_header_s( void );

// 	str_t				server;
// 	// date_t				date;
// 	// date_t				last_modified;
// 	unsigned			connection : 2;
// 	unsigned			chunked : 1;
// 	size_t				content_length;
// 	str_t				content_type;
// 	str_t				location;
// 	vec_uint_t			allow;

// 	vec_uint_t			list;

// }	response_header_t;

#endif