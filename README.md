# 🌐 Webserv

> A lightweight HTTP web server implemented from scratch in C++.  
> Fully compliant with parts of the HTTP/1.1 specification, handling static content, CGI execution, and concurrent connections.

---

## 📦 Project Overview

> **Webserv is a C++ network project from 42Seoul that involves implementing a fully functional web server without using any external frameworks or libraries.**

- Built using C++98 standard
- Supports multiple client connections using non-blocking I/O
- Implements parts of HTTP/1.1 protocol
- Provides static content delivery and CGI execution
- Server configuration is driven by a custom `.conf` file parser

> **Webserv는 42서울에서 C++ 기반으로 웹 서버를 직접 구현하는 네트워크 프로젝트입니다.**

- C++로 직접 구현한 웹 서버
- 다중 클라이언트 연결 지원 (non-blocking I/O)
- HTTP/1.1 프로토콜 일부 구현
- 정적 파일 서비스 + CGI 실행 지원
- Nginx 스타일의 `.conf` 설정 파일 기반 서버 구성

---

## 🧱 Tech Stack

| Category | Technology |
|----------|------------|
| Language | C++98 |
| Network | BSD Socket, kqueue |
| Parsing | Manual HTTP request parsing |
| Execution | CGI with fork + execve |
| Configuration | Custom parser (Nginx-style syntax) |
| Testing | curl, ab (Apache Bench), Browser |

| 항목 | 내용 |
|------|------|
| 언어 | C++98 |
| 네트워크 | BSD Socket, epoll/kqueue/select |
| 파싱 | 수동 HTTP 요청 파싱 |
| CGI 실행 | fork + execve |
| 설정 파일 | 사용자 정의 파서 (Nginx 스타일) |
| 테스트 도구 | curl, ab (Apache Bench), 웹 브라우저 등 |


---

## ⚙️ Key Features

- [x] Support for HTTP GET, POST, DELETE
- [x] Configurable via custom config file
- [x] Multiple server blocks (virtual hosts)
- [x] Custom error pages
- [x] CGI support (`.py`, `.php`, `.cgi`)
- [x] Connection keep-alive
- [x] Chunked body support, max body size control
- [x] Path-based access control (e.g. upload restrictions)

- [x] HTTP GET, POST, DELETE 지원
- [x] Config 기반 서버 설정
- [x] 가상 호스트 지원 (서버 블록)
- [x] 사용자 정의 오류 페이지
- [x] CGI 실행 지원 (`.py`, `.php`, `.cgi`)
- [x] Keep-Alive 처리
- [x] chunked body 지원 및 요청 본문 제한
- [x] 경로 기반 접근 제어

---

## 🗂️ Project Structure

```
serve/
├── Makefile
├── conf/
│   └── default.conf
├── html/
│   ├── index.html
│   ├── favicon.ico
│   ├── cgi-bin/
│   │   ├── upload.cgi
│   │   ├── visit.cgi
│   │   ├── chunked.txt
│   │   └── size_test
│   ├── donghong/
│   │   ├── index.html
│   │   ├── upload.html
│   │   ├── freeboard.txt
│   │   ├── 50x.html
│   │   ├── bin/
│   │   │   ├── upload.cgi
│   │   │   ├── visit.cgi
│   │   │   └── error_page.cgi
│   │   └── storage/img.png
│   └── donghyu2/
│       ├── index.html
│       └── upload.html
├── src/
│   ├── webserv.cpp / webserv.hpp
│   ├── log.cpp / log.hpp
│   ├── error.cpp / error.hpp
│   ├── core/
│   │   ├── config.cpp / config.hpp
│   │   ├── File.cpp / File.hpp
│   │   ├── filter.hpp / structure.hpp
│   │   └── utill.cpp / utill.hpp
│   ├── http/
│   │   ├── HTTP.cpp / HTTP.hpp
│   │   ├── Request.cpp / Request.hpp
│   │   ├── Response.cpp / Response.hpp
│   │   ├── Transaction.cpp / Transaction.hpp
│   │   ├── HTTP_method.cpp
│   │   ├── CGI.cpp / CGI.hpp
│   │   └── key/
│   │       ├── key_environ.txt
│   │       ├── key_header_in.txt
│   │       ├── key_header_out.txt
│   │       ├── key_mime.txt
│   │       └── key_status.txt
│   └── tcp/
│       ├── Client.cpp / Client.hpp
│       ├── Server.cpp / Server.hpp
│       └── SocketInit.cpp / SocketInit.hpp
```

---

## 🖥️ Server Architecture Diagram

```plaintext
          +-------------+       +---------------------+
Client -->| Socket Acc. |<----->|   Event Loop (poll) |
          +-------------+       +---------------------+
                  |                       |
          +----------------+      +---------------+
          | Request Parser |<---->|  Config Match |
          +----------------+      +---------------+
                  |
          +--------------------+
          | Static or CGI Path|
          +--------------------+
             |              |
   [Static File]      [CGI via fork + execve]
             |              |
      +---------------------+
      |   Response Builder  |
      +---------------------+
              ↓
           Client
```

---

## 🔧 How to Run

```bash
make
./webserv conf/default.conf
```

Visit `http://localhost:8080` in your browser.

웹 브라우저에서 http://localhost:8080 접속

---

## 🔍 Example Tests

```bash
curl -v localhost:8080

curl -X POST -F 'file=@test.txt' http://localhost:8080/donghong/upload.html

```

---

## 💡 Key Takeaways

- Network programming in C++ using sockets
- Manual HTTP parsing and response handling
- Static file serving and CGI pipeline
- Handling concurrent connections (non-blocking)
- Resource management and error handling (RAII)
- Designing a real-world server architecture

- C++ 네트워크 통신 구조
- HTTP 파싱 및 응답 형식 처리
- 정적 파일과 CGI 파이프라인 구현
- 비동기 커넥션 처리 (non-blocking)
- 자원 관리 및 예외 처리 (RAII)
- 실제 서비스를 고려한 설계 구조


---

## 👤 Author

- wonies : TCP/IP 통신을 위한 kqueue 기반 논블로킹 소켓 처리 구현 담당
- kichanLee : HTTP 파싱 & TCP/IP 통신 
  
