*This project has been created as part of the 42 curriculum by \<ibrouin\>, \<jlemoine\>, \<mickzhan\>.*

# ft_irc

An IRC server written in C++98, compliant with a subset of RFC 2812.

---

## Description

`ircserv` is a single-threaded, event-driven IRC server. It handles multiple simultaneous clients through a single `poll()` call, with no fork and no thread, and every I/O operation set to non-blocking.

The server handles client registration, channels, private and public messaging, and operator privileges.

**Reference client:** irssi.

---

## Build

```bash
make          # builds ircserv
make clean    # removes object files
make fclean   # removes object files and the binary
make re       # fclean + make
```

Flags: `-Wall -Wextra -Werror -std=c++98`

---

## Usage

```bash
./ircserv <port> <password>
```

- `port` — listening port, integer between 1024 and 65535
- `password` — single server-wide password, required from every client through `PASS`

---

## Architecture

| Layer | Responsibility | Files |
|---|---|---|
| **Transport** | socket, `poll()`, input buffers, client lifecycle | `Server.cpp`, `Client.cpp` |
| **Protocol** | CRLF splitting, parsing into `Message`, dispatch, reply building | `Server::callCommand`, `Command.cpp` |
| **Domain** | channels, modes, privileges, broadcasting | `Channel.cpp`, handlers |

### Command dispatch

Every command is one entry in a static `CmdInfo` table binding a verb to a function pointer and its metadata:

```
{ verb, handler, minParams, needsRegister }
```

The dispatcher uppercases the incoming verb, looks up the matching row, applies the generic checks (`421`, `451`, `461`), then calls the handler. Commands whose missing-parameter case has a dedicated numeric (`NICK` → `431`, `PRIVMSG` → `411`/`412`, `PING` → `409`) declare `minParams = 0` and handle it themselves.

### Framing

TCP is a byte stream with no message boundaries: a single `recv()` may deliver a fraction of one command, or several commands glued together. Each client therefore owns an accumulation buffer (`_inBuf`). On every read, the incoming bytes are appended to that buffer, then complete lines are extracted one at a time for as long as a delimiter is present. Whatever is left without a delimiter waits for the next read.

A safeguard disconnects any client whose buffer grows past 1024 bytes without a delimiter.

---

## Supported commands

| Command | Syntax | Registration required |
|---|---|---|
| `PASS` | `PASS <password>` | no |
| `NICK` | `NICK <nickname>` | no |
| `USER` | `USER <user> <mode> <unused> :<realname>` | no |
| `PING` | `PING <token>` | no |
| `JOIN` | `JOIN <#chan> [<key>]` | yes |
| `TOPIC` | `TOPIC <#chan> [:<topic>]` | yes |
| `PRIVMSG` | `PRIVMSG <target> :<text>` | yes |
| `NOTICE` | `NOTICE <target> :<text>` | yes |
| `INVITE` | `INVITE <nick> <#chan>` | yes |
| `KICK` | `KICK <#chan> <nick> [:<reason>]` | yes |
| `MODE` | `MODE <#chan> <+\|-flags> [args]` | yes |

### Registration

`PASS`, `NICK` and `USER` must all be supplied, in any order. As soon as the three are validated, the server emits numerics `001` through `004`. Any other command sent before that point is answered with `451`.

```
PASS mypassword
NICK julio
USER ju 0 * :Julio D
     ^^ ^ ^  ^^^^^^^
     |  | |  realname — trailing, free text, spaces allowed
     |  | unused — historically a hostname, ignored, send *
     |  mode — RFC 2812 bitmask, unused, send 0
     username — second field of the nick!user@host prefix
```

### Channel modes

| Mode | Effect | Argument | Error |
|---|---|---|---|
| `+i` | invite-only | — | `473` |
| `+t` | topic restricted to operators | — | `482` |
| `+k` | channel key | on `+` only | `475` |
| `+o` | grant/revoke operator status | always | `441` |
| `+l` | member limit | on `+` only | `471` |

The sign persists until the next one appears, and arguments are consumed in order by the flags that require one.

---

## Frame formats

Two families of outgoing frames, not to be confused.

**Numeric** — the server answers **one** client. The first parameter is always the recipient's nick (or `*` when they do not have one yet).

```
:ircserv 332 julio #dev :parser rewrite
```

**Relay** — the server notifies a **group** that a user acted. No numeric code: the verb that was received, prefixed with the author's prefix.

```
:julio!ju@127.0.0.1 PRIVMSG #dev :hello
```

In other words, the outgoing frame is the incoming frame with the author's prefix prepended.

### Broadcast rules

| Commands | Recipients |
|---|---|
| `PRIVMSG`, `NOTICE` | every member **except** the author |
| `JOIN`, `PART`, `QUIT`, `KICK`, `TOPIC`, `MODE` | every member, **author included** |

The author must receive the events that concern them: that is how their client updates its member list and its title bar.

### Handler execution order

```
validate → build the frame → broadcast → mutate state
```

Broadcasting before mutating guarantees that the target of a `KICK` actually receives the notification of its own removal.

### `NOTICE`: never any automatic reply

`NOTICE` does exactly what `PRIVMSG` does, with one rule on top: **no error reply is ever emitted**, neither by the server nor by a bot. That rule prevents two bots from answering each other indefinitely. `PRIVMSG`, by contrast, does return its errors (`401`, `403`, `404`, `411`, `412`).

---

## Useful commands

**Connecting a client**
```bash
nc 127.0.0.1 (or localhost) <port>
irssi -c 127.0.0.1 (or localhost) -p <port> -w <password>
```

**One-liner handshake**
```bash
printf 'PASS <pwd>\r\nNICK julio\r\nUSER ju 0 * :Julio D\r\n' | nc -i 2 127.0.0.1 <port>
```

**Active connections on the port**
```bash
lsof -i :<port>
ss -ltnp 'sport = :<port>'
```

**Killing processes holding the port**
```bash
pkill -f ircserv
kill -9 $(lsof -t -i :<port>)
```

**Memory and descriptor leaks**
```bash
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=yes ./ircserv <port> <password>
```

---

## Test suite — parsing

Each line must produce the expected decomposition. The trailing is stored in the parameter vector without its `:`, and an empty trailing still counts as one parameter.

| Frame | `cmd` | `params` |
|---|---|---|
| `JOIN #dev` | `JOIN` | `["#dev"]` |
| `PRIVMSG #dev :hello everyone` | `PRIVMSG` | `["#dev", "hello everyone"]` |
| `TOPIC #dev` | `TOPIC` | `["#dev"]` |
| `TOPIC #dev :` | `TOPIC` | `["#dev", ""]` |
| `NICK` | `NICK` | `[]` |
| `PRIVMSG #dev ::double` | `PRIVMSG` | `["#dev", ":double"]` |
| `JOIN    #dev` | `JOIN` | `["#dev"]` |
| `:julio!ju@host PRIVMSG #dev :hi` | `PRIVMSG` | `["#dev", "hi"]` |
| `KICK #dev bob :repeated flood` | `KICK` | `["#dev", "bob", "repeated flood"]` |

The two `TOPIC` lines are the decisive ones: `TOPIC #dev` **queries** the topic, `TOPIC #dev :` **clears** it. Conflating absent with empty makes clearing impossible.

---

## Test suite — behaviour

- Two clients exchanging private and channel messages
- Operator versus plain member on `KICK`, `TOPIC` under `+t`, `MODE`
- Every mode exercised in both directions
- Abrupt disconnection (`kill -9` on the client) without crashing the server
- Fragmented command: partial input, pause, then completion
- Several commands inside a single packet
- Ctrl+C on the server: clean shutdown, port immediately reusable
- No descriptor left open

---

## Resources

- [RFC 2812 — IRC Client Protocol](https://datatracker.ietf.org/doc/html/rfc2812)
- [RFC GLOBAL - Response codes](https://alien.net.au/irc/irc2numerics.html)
- [Modern IRC Documentation](https://modern.ircdocs.horse/)

