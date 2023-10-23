# IRC Server in C++

1. [Building the Server](#Building-an-IRC-Server-in-C++)
2. [Testing the Server](#testing-the-server)
3. [Special Tools/Functions](#special-toolsfunctions)
   - [socket](#socket)
   - [close](#close)
   - [setsockopt](#setsockopt)
   - [getsockname](#getsockname)
   - [getprotobyname](#getprotobyname)
   - [gethostbyname](#gethostbyname)
   - [getaddrinfo](#getaddrinfo)
   - [freeaddrinfo](#freeaddrinfo)
   - [bind](#bind)
   - [connect](#connect)
   - [listen](#listen)
   - [accept](#accept)
   - [htons, htonl, ntohs, ntohl](#htons-htonl-ntohs-ntohl)
   - [inet_addr, inet_ntoa](#inet_addr-inet_ntoa)
   - [send, recv](#send-recv)
   - [signal, sigaction](#signal-sigaction)
   - [lseek](#lseek)
   - [fstat](#fstat)
   - [fcntl](#fcntl)
   - [poll](#poll)
   - [select](#select)
   - [epoll](#epoll)
4. [Conclusion](#conclusion)


# Building an IRC Server in C++

Imagine you're building a special chat server, like the ones you use for texting with your friends, but this one is for more advanced users. It's called an IRC server, short for Internet Relay Chat. Your job is to create this server using an older version of a computer language called C++, which is like the language your computer understands.

This server will run on a computer, and people can connect to it to chat with each other. When people want to use your chat server, they'll have to tell it two things: the first is what "port" to connect to, which is like the door to the server, and the second is a "password," like a secret code to get in.

## Here's what you need to do:

- **Handle Multiple People:** Your server should be able to handle lots of people chatting at the same time, and it shouldn't crash or get stuck if many people join.

- **No Waiting:** You're not allowed to use a technique called "forking" to manage people connecting. Instead, everything should work quickly without any delays.

- **One Watcher:** You can only use one special tool called "poll" (or something similar) to watch over everything happening on your server. This tool keeps an eye on people connecting, sending messages, and other important things.

- **Reference Client:** Think of it like having a favorite app for chatting. You need to make sure your server works well with this special chat app. People using this app should be able to connect to your server without any issues.

- **Internet Connection:** Your server needs to communicate with people over the internet, using a system called TCP/IP. It's like the network that helps you access websites and send messages online.

- **Features:** Your server doesn't need to do everything that a big chat server does, but it should let people log in with a nickname, join chat rooms, send private messages, and receive messages from others in the chat room. You should also be able to have special chat room operators who can kick out users, invite others, set topics for the chat room, and control who can enter.

- **File Transfer:** Imagine if you could not only chat with your friends but also send them files like pictures, documents, or songs. That's what "handling file transfer" means. You can make your server smart enough to pass files between users, just like texting files to a friend.

- **A Bot:** Think of a bot like a little helper. You can create a friendly computer program that stays in your chat server. It can do all sorts of fun and useful things. For example, it can tell jokes, give you weather updates, or even play games with you. Bots make your chat server more interesting and interactive.

## Testing the server

Okay, imagine you've built this super cool chat server. Now, to make sure it works perfectly, you need to test it thoroughly. This means you want to check if it handles every possible situation, even if things get a bit tricky.

To do this, you'll use a tool called 'nc'. It's like a special keyboard that lets you talk to your server. When you type in `nc 127.0.0.1 6667`, you're telling your computer to connect to your own server.

Now, here's the interesting part: You're going to send a message in pieces. It's like if you're sending a sentence one word at a time. You'll press Ctrl+D to say you're done with each part. So, if you want to send "command", you'll do it like this: 'com', then Ctrl+D, 'man', then Ctrl+D, and 'd\n', and then Ctrl+D.

But why are you doing it this way? Well, sometimes, when you're chatting online, messages can arrive in bits and pieces, like when your internet connection is slow. Your server needs to be smart enough to put these pieces together and understand the whole message.

So, this test is like checking if your server can handle these tricky situations and still get the right message. It's like making sure your server is really, really good at listening and understanding, even if things get a bit jumbled up!

## Special-toolsfunctions 
These are special tools (or functions) that you'll use to build your chat server. Let's break them down:


 # socket:
    ```c
    int socket(int domain, int type, int protocol);
    ```
- **Parameters:**
- `domain`: Think of it as picking the language your phone will use (like English or Spanish).
- `type`: It's about deciding whether your phone will make regular calls or send quick text messages.
- `protocol`: This is like selecting the specific way your phone will work. You often leave it as 0 to let the system decide.

- **Explanation:**
The socket function sets up a communication channel between your computer and another. It's like preparing your phone for a particular kind of conversation.

# close:
```c
int close(int fd);
```
- **Parameters:**
    - `fd`: Imagine this as saying which phone you're done using.

- **Explanation:**
close is like hanging up the phone when you're finished with a call. You tell the system which connection to end.

# setsockopt:
```c
int setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len);
```
- **Parameters:**
    - `socket`: It's like pointing to your phone to adjust its settings.
    - `level`: Think of this as the type of settings you want to adjust (like general phone settings).
    - `option_name`: This is a specific setting you want to change (like turning on call waiting).
    - `option_value`: It's the value you want to set for that particular setting.
    - `option_len`: This tells the system the size of the setting.

- **Explanation:**
setsockopt lets you customize how your phone line works. It's like tweaking your phone's features to suit your needs, such as setting up call waiting.

# getsockname:
```c
int getsockname(int socket, struct sockaddr *address, socklen_t *address_len);
```
- **Parameters:**
    - `socket`: Think of this as pointing at your phone to find out some information about it.
    - `address`: It's like a place where you'll write down what you find out.
    - `address_len`: This is about knowing how much space you have to write down the information.

- **Explanation:**
    getsockname helps you check your phone's label to find out what number it has. It's like looking at your phone to see which phone line you're using.

# getprotobyname:
```c
struct protoent *getprotobyname(const char *name);
```
- **Parameters:**
    - `name`: It's like telling the system the name of the special code you want to use.

- **Explanation:**
    This function is like looking up a special code for how to talk to other computers. Each type of communication has its own code, and you're telling the system which one you want to use.

# gethostbyname:
```c
struct hostent *gethostbyname(const char *name);
```
- **Parameters:**
    - `name`: Imagine this as giving the system the name of the person you want to find.

- **Explanation:**
    This function helps you find out the address of another computer on the internet. It's like looking up someone's address in a phone book, but for computers.

# getaddrinfo:
```c
int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res);
```
- **Parameters:**
    - `node`: It's like telling the system which computer you want to learn more about.
    - `service`: Think of this as saying what kind of service you're interested in (like web browsing or file sharing).
    - `hints`: This is like giving the system a hint about what you're looking for.
    - `res`: It's a place where the system will write down the information it finds.

- **Explanation:**
    Similar to gethostbyname, but it gives you even more detailed information about the computer you want to talk to. It's like asking for a detailed profile of that computer.

# freeaddrinfo:
```c
void freeaddrinfo(struct addrinfo *res);
```
- **Parameters:**
    - `res`: Imagine this as telling the system where to find the information you want to clean up.

- **Explanation:**
    After you've looked up an address, you use this to clean up and put away the information you found. It's like putting away the phone book after you've found the number you need.

# bind:
```c
int bind(int socket, const struct sockaddr *address, socklen_t address_len);
```
- **Parameters:**
    - `socket`: Think of it as pointing to your phone to attach a label.
    - `address`: It's like writing down the address you want to use.
    - `address_len`: This is about knowing how much space you have to write down the address.

- **Explanation:**
    This function is like putting a label on your phone to make sure calls go to the right place. It attaches your server to a specific address and port, like putting a name on a mailbox.

# connect:
```c
int connect(int socket, const struct sockaddr *address, socklen_t address_len);
```
- **Parameters:**
    - `socket`: Think of it as pointing to your phone to dial a number.
    - `address`: It's like writing down the number you want to call.
    - `address_len`: This is about knowing how much space you have to write down the number.

- **Explanation:**
    This function is like dialing a phone number. It connects your server to another computer so they can talk, just like when you make a call.

# listen:
```c
int listen(int socket, int backlog);
```
- **Parameters:**
    - `socket`: It's like pointing to your phone and turning on the speaker so you can hear incoming calls.
    - `backlog`: This is about deciding how many people can be in the call waiting queue.

- **Explanation:**
    It's like putting your phone on speaker mode, so you can hear when someone is trying to call you. It sets up your server to be ready for incoming connections.

# accept:
```c
int accept(int socket, struct sockaddr *address, socklen_t *address_len);
```
- **Parameters:**
    - `socket`: Think of it as pointing to your phone to answer a call.
    - `address`: It's like writing down who's calling.
    - `address_len`: This is about knowing how much space you have to write down the caller's information.

- **Explanation:**
    When someone tries to call your server, this function lets you pick up and start talking to them. It's like answering a phone call.

# htons, htonl, ntohs, ntohl:
```c
uint16_t htons(uint16_t hostshort);
uint32_t htonl(uint32_t hostlong);
uint16_t ntohs(uint16_t netshort);
uint32_t ntohl(uint32_t netlong);
```
- **Parameters:**
    - `hostshort` (htons, ntohs): It's like taking a message and making sure it's in the right language so everyone can understand.
    - `hostlong` (htonl, ntohl): This is about making sure the message is in the right format so it can be read properly.

- **Explanation:**
    These functions make sure the data you send and receive is in the right format, so computers from different places can understand each other. It's like translating a message to a common language.

# inet_addr, inet_ntoa:
```c
in_addr_t inet_addr(const char *cp);
char *inet_ntoa(struct in_addr in);
```
- **Parameters:**
    - `cp` (inet_addr): It's like telling the system to read an address.
    - `inp` (inet_ntoa): This is like giving the system an address and asking for its name.

- **Explanation:**
    These functions help translate between the human-readable addresses (like website names) and the machine-friendly addresses (like numbers). It's like converting between a nickname and a full name.

# send, recv:
```c
ssize_t send(int socket, const void *buffer, size_t length, int flags);
ssize_t recv(int socket, void *buffer, size_t length, int flags);
```
- **Parameters:**
    - `socket`: Think of it as pointing to your phone to send or receive a message.
    - `buffer`: It's like the content of your message.
    - `length`: This is about knowing how long your message is.
    - `flags`: It's like adding special instructions to your message.

- **Explanation:**
    These are like sending and receiving messages on your phone. They let your server and other computers talk to each other. It's how information is passed back and forth.

# signal, sigaction:
```c
void (*signal(int signum, void (*handler)(int)))(int);
int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
```
- **Parameters:**
    - `signum` (signal, sigaction): Think of it as picking a special signal to get someone's attention.
    - `handler` (sigaction): It's like telling the system what to do when it gets that special signal.

- **Explanation:**
    These functions help your server react when something important happens. It's like having a special alert that tells your server to pay attention to something specific.

# lseek:
```c
off_t lseek(int fd, off_t offset, int whence);
```
- **Parameters:**
    - `fd`: Think of it as pointing to a book you're reading.
    - `offset`: It's like telling the system how many pages to skip.
    - `whence`: This is about deciding where to start counting pages from.

- **Explanation:**
    This function helps you find out where you are in a file and get information about it. It's like checking how far along you are in a book.

# - **fstat:
```c
int fstat(int fd, struct stat *buf);
```
- **Parameters:**
    - `fd`: Think of it as pointing to a book.
    - `buf`: It's like a place where you'll write down what you find out.

- **Explanation:**
    This helps you gather information about a file, like its size or when it was last modified. It's like looking at the back cover of a book for details.

# - **fcntl:
```c
int fcntl(int fd, int cmd, ...);
```
- **Parameters:**
    - `fd`: Think of it as pointing to a file you want to change.
    - `cmd`: It's like telling the system what change you want to make.
    - `arg`: This is additional information needed for the change.

- **Explanation:**
    This function lets you change how a file (like a chat log) behaves. It's like customizing your book with different fonts and colors.

# - **poll:
```c
int poll(struct pollfd *fds, nfds_t nfds, int timeout);
```
- **Parameters:**
    - `fds`: It's like a list of things you want to keep an eye on.
    - `nfds`: This is about knowing how many things are on your list.
    - `timeout`: It's like setting a timer to check your list after a certain amount of time.

- **Explanation:**
    This is like having a lookout tower to watch for ships. It helps your server keep an eye on what's happening without getting overwhelmed. It's especially useful when you're keeping track of multiple things at once.

# - **select:
```c
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
```
- **Parameters:**
    - `nfds`: This is the highest-numbered file descriptor in the sets, plus one. It's like telling the system how many items are on your watchlist.
    - `readfds`, `writefds`, `exceptfds`: These are like different lists of things you want to keep an eye on for reading, writing, or exceptional conditions.
    - `timeout`: It's like setting a timer to check your watchlist after a certain amount of time.

- **Explanation:**
    select is like being a lookout for different activities. You have different lists of things to watch for - like who's sending a message, who's ready to receive, and who's having a special condition. It's like keeping an eye on multiple situations at once.

# - **epoll:
```c
int epoll_create(int size);
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
```
- **Parameters:**
    - `epfd`: This is like having a special place where you organize your watchlist.
    - `events`: It's like preparing a list of things you're interested in, such as read or write events.
    - `maxevents`: This is about knowing how many things can be on your watchlist.

- **Explanation:**
    epoll is a bit like having a highly organized team of lookouts. You have a special place where you keep track of all the activities you're interested in. Then, you prepare a list of things you want to watch for, like who's sending a message or who's ready to receive. It's especially efficient when you have many things to keep an eye on.



    