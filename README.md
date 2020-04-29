# Internet Relay Chat (IRC)

This project was created in order to **simulate an Internet Relay Chat (IRC)** adapting [RFC 1459 specifications](https://tools.ietf.org/html/rfc1459).

### Development

All the project was developed using **C++ languague** and **g++ compiler** on Fedora 31 workstation.

### Members

This project was made by:

- Henrique de S. Q. dos Santos, NUSP 10819029 ([henriquesqs](https://github.com/henriquesqs));
- Witor M. A. de Oliveira, NUSP 10692190 ([witorMao](https://github.com/witormao)).

### How to use

- Certify that you have **make** installed;
- Use **make** to compile the source code;
- Open two terminals;
- On first terminal, use **make s** to run server side application. You won't see nothing after running this;
- On second terminal, use **make c** to run client side application. Wait for the welcome message and follow this instructions:
  - Enter **0.0.0.0** as ip address;
  - Enter **51000** as port;
  - Enter your **nickname**;
  - Type your message, type enter and then type "/send" to send your message;
  - If you want to end communication, type "/quit".
